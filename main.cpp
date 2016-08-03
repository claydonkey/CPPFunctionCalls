#include <cstdlib>
#include <iostream>
#include <functional>


using namespace std;
#define nONLYPASS

#if defined DEBUG

#define showAnswer(function)   \
printf ("Answer: %i\n" , function);
#define showInfo(function) \
std::cout <<  function << endl
#else
#define showAnswer(function)  \
printf ("Answer: %i\n" , function);
#define showInfo(function) 1
#endif

#if defined ONLYPASS
#define sFRes(function)
#elif defined DEBUG
#define sFRes(function) \
printf ("Result: %i\n" , function);
#else
#define sFRes(function) function
#endif

class TwoNumbers {
public:

    TwoNumbers(int first, int second) : first(first), second(second) {
    };
    int first;
    int second;
};

typedef pair<int, int> twoNumbers_t;
//typedef TwoNumbers twoNumbers_t;

class SimpleClass {
    int temp = 0;

private:

    int virtual function(twoNumbers_t n) {
	return n.first + n.second * temp++;
    }

    int static sfunction(twoNumbers_t n, void * context) {
	return n.first + n.second * static_cast<SimpleClass*> (context)->temp++;
    }
public:

    int memberFunction(twoNumbers_t & nos, void * context) {
	context == nullptr ? showInfo("member function1 with null context: ") : showInfo("member function1 HAS context: ");
	return function(nos);
    }

    int memberFunction(twoNumbers_t & nos) {
	return function(nos);
    }

    // int memberFunction1(twoNumbers_t & nos) {
    ///*annoying for std:function*/ return function(nos);
    // }

    int memberFunctionFunc1(twoNumbers_t & nos) {
	/*annoying for std:function*/ return function(nos);
    }

    int memberFunction(twoNumbers_t && ns) {
	twoNumbers_t nos = std::move(ns);
	return function(nos);
    }

    int memberFunctionFunc2(twoNumbers_t && ns) {
	twoNumbers_t nos = std::move(ns);
	return function(nos);
    }

    static int staticFunction(twoNumbers_t & nos, void * context) {
	return sfunction(nos, context);
    }

    static int staticFunction(twoNumbers_t && ns, void * context) {
	twoNumbers_t nos = std::move(ns);
	return sfunction(nos, context);
    }
};

typedef int (*FuncPtr_t)(twoNumbers_t & nos); //Function taking 2 integer arguments and returning integer;
typedef int (*cFuncPtr_t)(twoNumbers_t & nos, void* context); //Function taking 2 integer arguments and returning integer;
typedef int (*cFuncMovePtr_t)(twoNumbers_t && nos, void* context); //Function taking 2 integer arguments and returning integer;
typedef int (*FuncMovePtr_t)(twoNumbers_t && nos); //Function taking 2 integer arguments and returning integer;

typedef int ( SimpleClass::*MemFuncPtr_t)(twoNumbers_t & nos);
typedef int ( SimpleClass::*MemFuncMovePtr_t)(twoNumbers_t && nos);

int funcToMemFunction(twoNumbers_t & nos, void* context) {
    context == nullptr ? showInfo("funcToMemFunction with null context: ") : showInfo("funcToMemFunction HAS context: ");
    return static_cast<SimpleClass*> (context)->memberFunction(nos, context);
}

int funcToMemFunction(twoNumbers_t && nos, void* context) {
    context == nullptr ? showInfo("funcToMemFunction with null context: ") : showInfo("funcToMemFunction HAS context: ");
    return static_cast<SimpleClass*> (context)->memberFunction(std::move(nos));
}

static int forwardingFunction(FuncMovePtr_t pfunc, twoNumbers_t && nos) {
    showAnswer(pfunc(std::move(nos)));
}

static int forwardingFunction(cFuncMovePtr_t pfunc, twoNumbers_t && nos, void* context = nullptr) {
    context == nullptr ? showInfo("forwardingFunction with null context: ") : showInfo("forwardingFunction HAS context: ");
    showAnswer(pfunc(std::move(nos), context));
}

static int simpleFunction(FuncPtr_t pfunc, twoNumbers_t & nos) {
    showAnswer(pfunc(nos));
}

static int simpleFunction(cFuncPtr_t pfunc, twoNumbers_t & nos, void* context = nullptr) {
    context == nullptr ? showInfo("simpleFunction with null context: ") : showInfo("member simpleFunction HAS context: ");
    showAnswer(pfunc(nos, context));
}

int main() {

    twoNumbers_t nos(5, 10);
    SimpleClass obj;

    //Trampoline
    simpleFunction(&funcToMemFunction, nos, &obj);

    //Static
    cFuncPtr_t StaticMemFunction = &SimpleClass::staticFunction;
    sFRes(StaticMemFunction(nos, &obj));
    simpleFunction(StaticMemFunction, nos, &obj);

    // <Functional>
    using namespace std::placeholders;
    std::function<int( twoNumbers_t&) > f_simpleFunc = std::bind(&SimpleClass::memberFunctionFunc1, &obj, _1);
    auto p = f_simpleFunc.target < decltype(std::bind(&SimpleClass::memberFunctionFunc1, &obj, _1))>();
    sFRes(f_simpleFunc(nos));
    // (f_simpleFunc.target<int ( twoNumbers_t & ) >())(nos);
    // auto ptr_fun = f_simpleFunc.target<int ( twoNumbers_t &) >();

    simpleFunction((FuncPtr_t )&p, nos);

    //Expected
    MemFuncPtr_t MemFunction = &SimpleClass::memberFunction;
    sFRes((obj.*MemFunction) (nos));
    // simpleFunction(obj.*MemFunction,  nos);

    //Lambda
    simpleFunction(([](twoNumbers_t & n, void* context) {
	return ((SimpleClass*) (context))->memberFunction(n);
    }), nos, &obj);

    //Trampoline
    forwardingFunction(&funcToMemFunction, twoNumbers_t(5, 10), &obj);

    //Static
    cFuncMovePtr_t StaticMemMoveFunction = &SimpleClass::staticFunction;
    sFRes(StaticMemMoveFunction(twoNumbers_t(5, 10), &obj));
    forwardingFunction(StaticMemMoveFunction, twoNumbers_t(5, 10), &obj);

    // <Functional>
    using namespace std::placeholders;
    std::function<int( twoNumbers_t&&) > f_forwardingFunc = std::bind(&SimpleClass::memberFunctionFunc2, &obj, _1);

    sFRes(f_forwardingFunc(twoNumbers_t(5, 10)));
    auto ptr_fun2 = f_forwardingFunc.target<int ( twoNumbers_t &&) >();


    //Expected
    MemFuncMovePtr_t MemMoveFunction = &SimpleClass::memberFunction;
    sFRes((obj.*MemMoveFunction) (twoNumbers_t(5, 10)));
    // forwardingFunction(obj.*MemFunction, nullptr, twoNumbers_t(12, 34));


    //Lambda
    forwardingFunction([](twoNumbers_t && n, void* context) {
	return static_cast<SimpleClass*> (context)->memberFunction(n);
    }, twoNumbers_t(5, 10), &obj);


    return 0;
}


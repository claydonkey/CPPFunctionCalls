#include <cstdlib>
#include <iostream>
#include <functional>


using namespace std;
//#define ONLYPASS

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
public:

    int memberFunction(twoNumbers_t & nos, void * context) {
	context == nullptr ? showInfo("member function1 with null context: ") : showInfo("member function1 HAS context: ");
	return nos.first + nos.second;
    }

    int memberFunction(twoNumbers_t & nos) {
	temp += 1;
	return nos.first * nos.second;
    }

    int memberFunction1(twoNumbers_t & nos) { //annoying for std:function

	return nos.first - nos.second;
    }

    int memberFunction(twoNumbers_t && nos) {

	twoNumbers_t nosm = std::move(nos);
	return nosm.first + nosm.second;
    }

    static int staticFunction(twoNumbers_t & nos) {
	return nos.first + nos.second;
    }

    static int staticFunction(twoNumbers_t && nos) {
	twoNumbers_t nosm = std::move(nos);
	return nosm.first + nosm.second;
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


    twoNumbers_t nos(12, 34);
    SimpleClass obj;

    //Trampoline
    simpleFunction(&funcToMemFunction, nos);

    //Static
    FuncPtr_t StaticMemFunction = &SimpleClass::staticFunction;
    sFRes(StaticMemFunction(nos));
    simpleFunction(StaticMemFunction, nos);

    // <Functional>
    using namespace std::placeholders;
    std::function<int( twoNumbers_t&, void *) > f_simpleFunc = std::bind(&SimpleClass::memberFunction1, obj, _1);
    sFRes(f_simpleFunc(nos, nullptr));
    auto ptr_fun = f_simpleFunc.target<int ( twoNumbers_t &, void*) >();

    //   simpleFunction(*ptr_fun, nullptr, nos);

    //Expected
    MemFuncPtr_t MemFunction = &SimpleClass::memberFunction;
    sFRes((obj.*MemFunction) (nos));
    // simpleFunction(obj.*MemFunction,  nos);

    //Lambda
    simpleFunction(([](twoNumbers_t & n, void* context) {
	return ((SimpleClass*) (context))->memberFunction(n);
    }), nos, &obj);

    //Trampoline
    forwardingFunction(&funcToMemFunction, twoNumbers_t(12, 34));

    //Static
    FuncMovePtr_t StaticMemMoveFunction = &SimpleClass::staticFunction;
    sFRes(StaticMemMoveFunction(twoNumbers_t(12, 34)));
    forwardingFunction(StaticMemMoveFunction, twoNumbers_t(12, 34));

    //Expected
    MemFuncMovePtr_t MemMoveFunction = &SimpleClass::memberFunction;
    sFRes((obj.*MemMoveFunction) (twoNumbers_t(12, 34)));
    // forwardingFunction(obj.*MemFunction, nullptr, twoNumbers_t(12, 34));


    //Lambda
    forwardingFunction([](twoNumbers_t && n, void* context) {
	return static_cast<SimpleClass*> (context)->memberFunction(n);
    }, twoNumbers_t(12, 34), &obj);


    return 0;
}


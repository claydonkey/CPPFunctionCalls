#include <cstdlib>
#include <iostream>
#include <functional>
using namespace std;
#if defined DEBUG
#define showAnswer showResult
#define showResult(function)   cout << "Result: " << std::to_string(function) << endl
#define showInfo( function)   std::cout << function << endl

#else
#define showAnswer(function)  \
 cout << "Result: " << std::to_string(function) << endl
#define showResult(function) function
#define showInfo(function)
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
public:

    int memberFunction(twoNumbers_t & nos) {
	showInfo("member function with no  context: ");
	return nos.first * nos.second;
    }

    int memberFunction(void * context, twoNumbers_t & nos) {
	(context == nullptr ? std::cout << "member function1 with null context: " << endl : std::cout << "member function1 HAS context: " << endl);
	return nos.first + nos.second;
    }

    int memberFunction1(void * context, twoNumbers_t & nos) { //annoying for std:function
	(context == nullptr ? std::cout << "member function2 with null context: " << endl : std::cout << "member function2 HAS context: " << endl);
	return nos.first + nos.second;
    }

    int memberFunction(void *context, twoNumbers_t && nos) {
	(context == nullptr ? std::cout << "member function3 with null context: " << endl : std::cout << "member function3 HAS context: " << endl);
	twoNumbers_t nosm = std::move(nos);
	return nosm.first + nosm.second;
    }

    static int staticFunction(void * context, twoNumbers_t & nos) {
	(context == nullptr ? std::cout << "member function4 with null context: " << endl : std::cout << "member function4 HAS context: " << endl);
	return nos.first + nos.second;
    }

    static int staticFunction(void * context, twoNumbers_t && nos) {
	(context == nullptr ? std::cout << "member function5 with null context: " << endl : std::cout << "member function5 HAS context: " << endl);
	twoNumbers_t nosm = std::move(nos);
	return nosm.first + nosm.second;
    }

};

int funcToNowhere(void*, twoNumbers_t && nos) {
    showInfo("non-member no context! ");
    return nos.first + nos.second;
}

int funcToNowhere(void*, twoNumbers_t & nos) {
    showInfo("non-member no context! ");
    return nos.first + nos.second;
}

int funcToMemFunction(void* context, twoNumbers_t & nos) {
    return static_cast<SimpleClass*> (context)->memberFunction(context, nos);
}

int funcToMemFunction(void* context, twoNumbers_t && nos) {
    return static_cast<SimpleClass*> (context)->memberFunction(context, std::move(nos));
}

typedef int (*FuncPtr_t)(void* context, twoNumbers_t & nos); //Function taking 2 integer arguments and returning integer;
typedef int (*FuncMovePtr_t)(void* context, twoNumbers_t && nos); //Function taking 2 integer arguments and returning integer;
typedef int ( SimpleClass::*MemFuncPtr_t)(void* context, twoNumbers_t & nos);
typedef int ( SimpleClass::*MemFuncMovePtr_t)(void* context, twoNumbers_t && nos);

static int forwardingFunction(FuncMovePtr_t pfunc, void* context, twoNumbers_t && nos) {
    showResult(pfunc(context, std::move(nos)));
}

static int simpleFunction(FuncPtr_t pfunc, twoNumbers_t & nos) {
    showResult(pfunc(nullptr, nos));
}

static int simpleFunction(FuncPtr_t pfunc, void* context, twoNumbers_t & nos) {
    showResult(pfunc(context, nos));
}

int main() {


    twoNumbers_t nos(12, 34);
    SimpleClass obj;
    //Trampoline
    simpleFunction(&funcToNowhere, nos);
    simpleFunction(&funcToMemFunction, &obj, nos);

    //Static
    FuncPtr_t StaticMemFunction = &SimpleClass::staticFunction;
    showResult(StaticMemFunction(nullptr, nos));
    simpleFunction(StaticMemFunction, nos);


    // <Functional>
    using namespace std::placeholders;
    std::function<int( void *, twoNumbers_t&) > f_simpleFunc = std::bind(&SimpleClass::memberFunction1, obj, _1, _2);
    showResult(f_simpleFunc(nullptr, nos));
    showInfo(f_simpleFunc.target_type().name());
    auto ptr_fun = f_simpleFunc.target<int (void*, twoNumbers_t &) >();

    //   simpleFunction(*ptr_fun, nullptr, nos);

    //Expected
    MemFuncPtr_t MemFunction = &SimpleClass::memberFunction;
    showResult((obj.*MemFunction) (nullptr, nos));
    // simpleFunction(obj.*MemFunction, nullptr, nos);

    //Lambda
    simpleFunction(([](void* context, twoNumbers_t & n) {
	showInfo("Lambda: ");
	return (static_cast<SimpleClass*> (context))->memberFunction(nullptr, n);
    }), 0, nos);

    //Trampoline
    forwardingFunction(&funcToNowhere, 0, twoNumbers_t(12, 34));
    forwardingFunction(&funcToMemFunction, &obj, twoNumbers_t(12, 34));

    //Static
    FuncMovePtr_t StaticMemMoveFunction = &SimpleClass::staticFunction;
    showResult(StaticMemMoveFunction(nullptr, twoNumbers_t(12, 34)));
    forwardingFunction(StaticMemMoveFunction, nullptr, twoNumbers_t(12, 34));

    //Expected
    MemFuncMovePtr_t MemMoveFunction = &SimpleClass::memberFunction;
    showAnswer((obj.*MemMoveFunction) (nullptr, twoNumbers_t(12, 34)));
    // forwardingFunction(obj.*MemFunction, nullptr, twoNumbers_t(12, 34));


    //Lambda
    forwardingFunction([](void* context, twoNumbers_t && n) {
	showInfo("Lambda forward: ");
	return static_cast<SimpleClass*> (context)->memberFunction(nullptr, n);
    }, 0, twoNumbers_t(12, 34));


    return 0;
}


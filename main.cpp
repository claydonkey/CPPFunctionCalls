#include <cstdlib>
#include <iostream>
#include <functional>

#define DEBUG 0

#if (DEBUG)
#define showAnswer showResult
#define showResult(function) \
  cout << "Result: " << std::to_string(function) << endl
#define showInfo(function)  \
 cout  << function << endl
#else
#define showAnswer(function)  \
 cout << "Result: " << std::to_string(function) << endl
#define showResult(function) function
#define showInfo(function)
#endif
using namespace std;

class TwoNumbers {
public:

    TwoNumbers(int first, int second) : first(first), second(second) {
    };
    int first;
    int second;
};

//typedef pair<int, int> twoNumbers_t;
typedef TwoNumbers twoNumbers_t;

class SimpleClass {
public:

    int memFunction(twoNumbers_t & nos) {
	showInfo("member function with no  context: ");
	return nos.first + nos.second;
    }

    int memFunction(void *, twoNumbers_t & nos) {
	showInfo("member function with null context: ");
	return nos.first + nos.second;
    }

    int memFunction1(void *, twoNumbers_t & nos) {
	showInfo("member function with null context: ");
	return nos.first + nos.second;
    }

    int memFunction(void *, twoNumbers_t && nos) {
	twoNumbers_t nosm = std::move(nos);
	showInfo("member move function with null  context");
	return nosm.first + nosm.second;
    }

    static int staticFunction(void *, twoNumbers_t & nos) {
	return nos.first + nos.second;
    }

    static int staticFunction(void *, twoNumbers_t && nos) {
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
    return static_cast<SimpleClass*> (context)->memFunction(nos);
}

int funcToMemFunction(void* context, twoNumbers_t && nos) {
    return static_cast<SimpleClass*> (context)->memFunction(context, std::move(nos));
}

typedef int (*FuncPtr_t)(void*, twoNumbers_t & nos); //Function taking 2 integer arguments and returning integer;
typedef int (*FuncMovePtr_t)(void*, twoNumbers_t && nos); //Function taking 2 integer arguments and returning integer;
typedef int ( SimpleClass::*MemFuncPtr_t)(void*, twoNumbers_t & nos);
typedef int ( SimpleClass::*MemFuncMovePtr_t)(void*, twoNumbers_t && nos);

static int forwardingFunction(FuncMovePtr_t pfunc, void* context, twoNumbers_t && nos) {
    showResult(pfunc(context, std::move(nos)));
}

static int simpleFunction(FuncPtr_t pfunc, void* context, twoNumbers_t & nos) {
    showResult(pfunc(context, nos));
}

int main() {

    SimpleClass obj;
    twoNumbers_t nos(12, 34);

    //Trampoline
    simpleFunction(&funcToNowhere, 0, nos);
    simpleFunction(&funcToMemFunction, &obj, nos);

    //Static
    FuncPtr_t StaticMemFunction = &SimpleClass::staticFunction;
    showResult(StaticMemFunction(nullptr, nos));
    simpleFunction(StaticMemFunction, nullptr, nos);


    // <Functional>
    using namespace std::placeholders;
    std::function<int( void *, twoNumbers_t&) > f_simpleFunc = std::bind(&SimpleClass::memFunction1, obj, _1, _2);
    showResult(f_simpleFunc(nullptr, nos));
    showInfo(f_simpleFunc.target_type().name());
    auto ptr_fun = f_simpleFunc.target<int (void*, twoNumbers_t &) >();

    //   simpleFunction(*ptr_fun, nullptr, nos);

    //Expected
    MemFuncPtr_t MemFunction = &SimpleClass::memFunction;
    showResult((obj.*MemFunction) (nullptr, nos));
    // simpleFunction(obj.*MemFunction, nullptr, nos);

    //Lambda
    simpleFunction((FuncPtr_t)[ = ](void*, twoNumbers_t & n){
	showInfo("Lambda: ");
	return n.first + n.second;
    }, 0, nos);

    //Trampoline
    forwardingFunction(&funcToNowhere, 0, twoNumbers_t(12, 34));
    forwardingFunction(&funcToMemFunction, &obj, twoNumbers_t(12, 34));

    //Static
    FuncMovePtr_t StaticMemMoveFunction = &SimpleClass::staticFunction;
    showResult(StaticMemMoveFunction(nullptr, twoNumbers_t(12, 34)));
    forwardingFunction(StaticMemMoveFunction, nullptr, twoNumbers_t(12, 34));

    //Expected
    MemFuncMovePtr_t MemMoveFunction = &SimpleClass::memFunction;
    showAnswer((obj.*MemMoveFunction) (nullptr, twoNumbers_t(12, 34)));
  // forwardingFunction(obj.*MemFunction, nullptr, twoNumbers_t(12, 34));


    //Lambda
    forwardingFunction((FuncMovePtr_t)[](void*, twoNumbers_t && nos) {
	showInfo("Lambda forward: ");
	return nos.first + nos.second;
    }, 0, twoNumbers_t(12, 34));


    return 0;
}


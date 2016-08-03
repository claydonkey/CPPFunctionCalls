# PointerToMember
Flavours of C++ Functions 
Although touched on in [How Can I Pass a **Member Function** to a **Function Pointer**?][1], I feel somewhat dissatisfied with the solutions provided, as I don't want to introduce a dependency on the Boost library.

 [Comparing **std::function** for **member functions**][2] is a post that gets close to a solution but ultimately is less optimistic about the use of **std::function** in .
*(it seems that member functions cannot be passed as function pointers)*

**The Problem:**

A function `simpleFunction` which cannot be altered takes a **callback** `pfunc`:
 
    
    typedef int (*FuncPtr_t)(void*, std::pair<int,int>&);
    static int simpleFunction(FuncPtr_t pfunc, void *context, std::pair<int,int>& nos)
    {
        pfunc(context, nos);
    }

This function is intended to callback the method `memberFunction` in class `SimpleClass`:

    class SimpleClass {
    public:  
        int memberFunction(void*, std::pair<int,int>& nos) { return nos.first + nos.second; }
    };

---
I expected the following to work:

    MemFuncPtr_t MemFunction = &SimpleClass::memberFunction;
    simpleFunction(obj.*MemFunction, nos);

but `obj.*MemFunction` has a type: `int (SimpleClass::)(void*, std::pair<int,int>&)`

and it needs to be: `int (*)(void*, std::pair<int,int>&)`

*(wheras `(obj.*MemFunction) (nullptr, nos);` returns as expected)*

---
I can create and pass a trampoline:

    int functionToMemberFunction(void* context, std::pair<int,int> & nos) {
         return static_cast<SimpleClass*>(context)->memberFunction(nos);
    }

and pass it

`simpleFunction(&functionToMemberFunction, &obj, nos);`

but it compiles to around 40 instructions.

---
I can pass a lambda:

    simpleFunction((FuncPtr_t)[](void* , std::pair<int,int> & nos) {
        return nos.first + nos.second;
    }, 0, nos);

That's surprisingly well optimised but a bit ugly and syntactically cumbersome.
*(NB Both <functional> and lambdas require C++11)*

---
I can add a static member to `SimpleClass`:

    class SimpleClass {
    public:  
        int memberFunction(void*, std::pair<int,int>& nos) { return nos.first + nos.second; }
        static int staticFunction(void*, std::pair<int,int> & nos) { return nos.first + nos.second; }
    };

    FuncPtr_t StaticMemFunction = &SimpleClass::staticFunction;

and pass it

`simpleFunction(StaticMemFunction, nullptr, nos);`

and that's just, well ... a static function inside a class.

---

I can use the `<functional>` header:

    using namespace std::placeholders;
 
    std::function<int( void *, std::pair<int,int>&) > f_simpleFunc = 
    std::bind(&SimpleClass::memberFunction, obj, _1, _2);
   
    auto ptr_fun = f_simpleFunc.target<int (void*, std::pair<int,int> & ) >();
    
and try and pass it...

`simpleFunction(*ptr_fun, nullptr, nos);`

but ptr_fun reports null.

---

Looking at the x86 assembly - I am at a loss at how memory is addressed, calling a member function (there are an extra 5 instructions [3 `mov`, 1 `lea` and 1 `add`] over the `StaticMemFunction` call). I can only imagine that this is down to locating the class instance in memory and then the function within it.


  [1]: /q/4296281
  [2]: http://stackoverflow.com/questions/27798205/comparing-stdfunction-for-member-functions

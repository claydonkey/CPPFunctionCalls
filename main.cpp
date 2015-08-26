#include <cstdlib>
#include <iostream>

using namespace std;

//  function using the function pointers:


typedef int (*FuncPtr)(void*, int, int);

class SimpleClass {
public:

    int memberFunction(int i0, int i1) { //method
        cout << "member function needs context: this=" << this << " i0=" << i0 << " i1=" << i1 << "\n";
        return i0 + i1;
    }
};

void simpleFunction(FuncPtr fp, void* context, int i0, int i1) {

    cout << "Number: " << std::to_string(fp(context, i0, i1)) << "\n";
}

int non_member(void*, int i0, int i1) {
    cout << "non_member needs no context! i0=" << i0 << " i1=" << i1 << "\n";
    return i0 + i1;
}

int memberFunctionForwarder(void* context, int i0, int i1) {

    return ((SimpleClass * ) context)->memberFunction(i0, i1);
}

int main() {

    simpleFunction(&non_member, 0, 1, 2);
    SimpleClass obj;
    simpleFunction(&memberFunctionForwarder, &obj, 3, 4);


    simpleFunction((FuncPtr)[](void* v, int i0, int i1) {
        std::cout << "From Lambda needs no context! i0=" << i0 << " i1=" << i1 << "\n";
        return i0 + i1;
    }, 0, 5, 6);

    int s;

    std::cin >> s;
}


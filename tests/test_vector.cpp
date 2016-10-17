#include "../util/vector.hpp"
#include <assert.h>

int counter = 0;

struct A {
    int i;

    A(int i): i(i){
        counter++;
    }

    A(const A &a): i(a.i){
        counter++;
    }

    A& operator = (const A &a){
        i = a.i;
        return *this;
    }

    ~A(){
        counter--;
    }
};

int main(){
    {
        Vector<A> a;

        for (int i = 0; i < 1000; i++){
            if (rand() % 10 < 8 || a.empty()){
                a.push(i);
            }else{
                a.pop();
            }

        }
    }

    assert(counter == 0);

    size_t n = 100;
    Vector<int> a, b(n);
    for (size_t i = 0; i < n; i++) a.push(i);
    for (size_t i = 0; i < n; i++) b[i] = i;
    for (size_t i = 0; i < n; i++) assert(a[i] == int(i));
    for (size_t i = 0; i < n; i++) assert(a.pop() == int(n - 1 - i));
    assert(a.empty());

    return 0;
}

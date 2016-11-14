#include "../util/util.hpp"
#include "../util/vector.hpp"

int counter = 0;

struct A {
    int i;

    A(): i(-666){
        counter++;
    }

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

        size_t n = 3;
        Vector<Vector<Vector<Vector<A>>>> aaa(n);

        for (size_t i = 0; i < n; i++){
            aaa[i].resize(n);
            for (size_t j = 0; j < n; j++){
                aaa[i][j].resize(n);
                for (size_t k = 0; k < n; k++){
                    aaa[i][j][k].resize(n);
                    for (size_t l = 0; l < n; l++){
                        aaa[i][j][k][l] = A(666);
                    }
                }
            }
        }
    }

    my_assert(counter == 0);

    size_t n = 100;
    Vector<int> a, b(n);
    for (size_t i = 0; i < n; i++) a.push(i);
    for (size_t i = 0; i < n; i++) b[i] = i;
    for (size_t i = 0; i < n; i++) my_assert(a[i] == int(i));
    for (size_t i = 0; i < n; i++) my_assert(a.pop() == int(n - 1 - i));
    my_assert(a.empty());

    return 0;
}

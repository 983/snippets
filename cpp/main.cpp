#include "util/util.hpp"
#include "util/vector.hpp"

int main(){
    Vector<int> a;
    for (size_t i = 0; i < 100; i++){
        a.push(i);
    }
    while (!a.empty()){
        int x = a.pop();
        printf("%i\n", x);
    }
    return 0;
}

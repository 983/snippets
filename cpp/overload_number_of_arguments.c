#include <stdio.h>

#define _CONCAT(a, b) a ## b
#define CONCAT(a, b) _CONCAT(a, b)

#define _COUNT_ARGS(a, b, c, d, e, f, g, h, i, j, N, ...) N
#define COUNT_ARGS(...) _COUNT_ARGS(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define OVERLOAD(name, ...) CONCAT(name, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

void foo1(int x){
    printf("foo1 %i\n", x);
}

void foo2(int x, int y){
    printf("foo2 %i %i\n", x, y);
}

#define foo(...) OVERLOAD(foo, __VA_ARGS__)

int main(){
    
    foo(1);
    foo(1, 2);
    
    return 0;
}

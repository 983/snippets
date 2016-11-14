#pragma once

template <typename T>
void my_swap(T &a, T &b){
    T c(a);
    a = b;
    b = c;
}

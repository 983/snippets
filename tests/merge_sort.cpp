#include <stdio.h>
#include <assert.h>

#include <vector>
#include <algorithm>

#include "../util/time.hpp"
#include "../util/merge_sort.hpp"

void test_merge_sort(size_t n){
    std::vector<int> a(n), b(n), c(n);

    for (size_t i = 0; i < n; i++) a[i] = b[i] = rand();

    double t[3];
    t[0] = Time::sec();
    std::sort(a.begin(), a.end());
    t[1] = Time::sec();
    merge_sort(b.data(), n, c.data());
    t[2] = Time::sec();

    for (size_t i = 0; i < n; i++){
        assert(a[i] == b[i]);
    }
    printf("%f %f\n", t[1] - t[0], t[2] - t[1]);
}

int main(){
    test_merge_sort(0);
    for (size_t n = 1; n <= 128*1024; n *= 2){
        for (size_t k = 0; k < 5; k++){
            test_merge_sort(n);
        }
    }

    return 0;
}

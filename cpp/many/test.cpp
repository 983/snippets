#include "many.h"

MAKE_MANY_HEADER(ints, int)

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>

struct many_ints a[1];
std::vector<int> b;

int n_comparisons = 0;

void hopefully_the_same(){
    assert(many_ints_size(a) == b.size());
    n_comparisons++;

    for (size_t i = 0; i < b.size(); i++){
        assert(many_ints_get(a, i) == b[i]);
        n_comparisons++;
    }
}

double sec(){
    return clock() / (double)CLOCKS_PER_SEC;
}

int main(){
    double t = sec();

    many_ints_init(a);

    hopefully_the_same();

    for (int i = 0; i < 1000*1000; i++){
        many_ints_push(a, i);
        b.push_back(i);
    }

    hopefully_the_same();

    many_ints_free(a);
    many_ints_init(a);
    b = std::vector<int>();

    for (int i = 0; i < 1000*1000; i++){
        int what_to_do = rand() % 7;
        int j, k, err;

        switch (what_to_do){

        case 0:
            err = many_ints_push(a, i);
            assert(!err);
            b.push_back(i);
            break;

        case 1:
            err = many_ints_push(a, many_ints_size(a));
            assert(!err);
            many_ints_pop(a);

            b.push_back(b.size());
            b.pop_back();
            break;

        case 2:
            err = many_ints_push(a, many_ints_size(a));
            assert(!err);
            err = many_ints_push(a, many_ints_front(a));
            assert(!err);
            err = many_ints_push(a, many_ints_back(a));
            assert(!err);
            b.push_back(b.size());
            b.push_back(b.front());
            b.push_back(b.back());
            break;

        case 3:
            many_ints_clear(a);
            b.clear();
            break;

        case 4:
            many_ints_free(a);
            many_ints_init(a);
            b = std::vector<int>();
            break;

        case 5:
            err = many_ints_push(a, many_ints_size(a));
            assert(!err);
            b.push_back(b.size());

            j = rand() % many_ints_size(a);
            k = rand() % many_ints_size(a);

            many_ints_set(a, k, many_ints_get(a, j) + 1);

            b[k] = b[j] + 1;
            break;

        case 6:
            j = rand() % 5;
            err = many_ints_resize(a, j, -1);
            assert(!err);
            b.resize(j, -1);
        }

        hopefully_the_same();
    }

    many_ints_free(a);

    double dt = sec() - t;

    printf("All tests passed.\n\n");
    printf("It took %f seconds to execute a few million random operations.\n", dt);
    printf("Should take about 0.13 seconds on a lower end intel i5 CPU.\n");
    printf("Compared %f million values.\n", n_comparisons*1e-6);

    return 0;
}

MAKE_MANY_SOURCE(ints, int)

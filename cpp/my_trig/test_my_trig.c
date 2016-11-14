#include "my_trig.h"

#include <stdio.h>
#include <stdlib.h>

double randf(){
    return rand() / (double)RAND_MAX;
}

int main(){
    double max_sin_err = 3.36e-9;
    double max_cos_err = 4.68e-8;
    double max_atan_err = 1.31e-5;

    double x, y;
    int i;

    for (x = -1; x <= 1; x += 0.001){
        assert(fabs(atan(x) - my_atan_one(x)) < max_atan_err);
    }

    for (x = -PI_HALF; x <= PI_HALF; x += 0.001){
        assert(fabs(sin(x) - my_sin_pi_half(x)) < max_sin_err);
        assert(fabs(cos(x) - my_cos_pi_half(x)) < max_cos_err);
    }

    for (x = -1337; x <= 1337; x += 0.125){
        assert(fabs(sin(x) - my_sin(x)) < max_cos_err);
        assert(fabs(cos(x) - my_cos(x)) < max_sin_err);
        assert(fabs(atan(x) - my_atan(x)) < max_atan_err);
    }

    for (i = 0; i < 1000*1000; i++){
        double x = randf()*2e100 - 1e100;
        double y = randf()*2e100 - 1e100;
        assert(fabs(atan2(y, x) - my_atan2(y, x)) < max_atan_err);
    }

    for (i = 0; i < 1000*1000; i++){
        double x = randf()*100000 - 50000;
        assert(fabs(sin(x) - my_sin(x)) < max_cos_err);
        assert(fabs(cos(x) - my_cos(x)) < max_sin_err);
    }

    for (x = -10; x <= 10; x += 0.125){
        for (y = -10; y <= 10; y += 0.125){
            assert(fabs(atan2(y, x) - my_atan2(y, x)) < max_atan_err);
        }
    }

    printf("all tests passed\n");

    return 0;
}

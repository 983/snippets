#ifndef MY_TRIG_H
#define MY_TRIG_H

#include <math.h>
#include <assert.h>

#define PI 3.14159265358979323846264
#define PI_HALF (PI*0.5)

/*
Angles and parameters for atan2(y, x):

           y-axis
           ^
           |
     \ x/y | x/y /
      \    |    /
       \ +pi/2 /
  y/x   \  |  /   y/x
  +pi    \ | /    +0
          \|/
 -+pi------0-------0--> x-axis
          /|\                   The atan approximation is only accurate
  -pi    / | \    +0            for parameters in [-1, +1].
  y/x   /  |  \   y/x           Fortunately:
       / -pi/2 \                atan(x) = +pi/2 - atan(1/x), x > 0
      /    |    \               atan(x) = -pi/2 - atan(1/x), x < 0
     / x/y | x/y \              Derived by flipping around identity line
           |                    and adjusting angle offset.

Magic constants are derived by least square fit of a polynomial.
*/

static inline double my_atan_one(double x){
    double x2 = x*x;
    assert(x >= -1);
    assert(x <= +1);
    return x*(0.999851226277 + x2*(-0.33010340793 + x2*(0.179433975471 + x2*(-0.0841885378738 + 0.0204145920171*x2))));
}

static inline double my_sin_pi_half(double x){
    double x2 = x*x;
    assert(x >= -PI_HALF);
    assert(x <= +PI_HALF);
    return x*(0.999999976493 + x2*(-0.166666475799 + x2*(0.0083328989913 + x2*(-0.000198008510079 + x2*2.59040105615e-06))));
}

static inline double my_cos_pi_half(double x){
    double x2 = x*x;
    assert(x >= -PI_HALF);
    assert(x <= +PI_HALF);
    return 0.999999953202 + x2*(-0.499999049883 + x2*(0.0416635771194 + x2*(-0.00138536532302 + x2*2.31528568121e-05)));
}

/*
sin and cos are range reduced to [0, 2*PI],
shifted to [-PI/2, PI*3/4],
split into [-PI/2, PI/2] and [PI/2, PI*3/4] - PI,
and then approximated through their counterpart.
Note that
cos(x - PI/2) = +sin(x)
sin(x - PI/2) = -cos(x)
sin(x - PI) = -sin(x)
cos(x - PI) = -cos(x)
cos(-x) = +cos(+x)
sin(-x) = -sin(-x)
*/

static inline double my_sin(double x){
    x -= 2*PI*floor(x*(0.5/PI));
    x -= PI_HALF;
    return x < PI_HALF ? my_cos_pi_half(x) : -my_cos_pi_half(x - PI);
}

static inline double my_cos(double x){
    x -= 2*PI*floor(x*(0.5/PI));
    x -= PI_HALF;
    return x < PI_HALF ? -my_sin_pi_half(x) : my_sin_pi_half(x - PI);
}

static inline double my_atan(double x){
    return (-1 <= x && x <= 1) ? my_atan_one(x) :
        ((x > 1 ? PI_HALF : -PI_HALF) - my_atan_one(1/x));
}

static inline double my_atan2(double y, double x){
    if (y == 0) return x >= 0 ? 0 : PI;
    if (x == 0) return y > 0 ? PI_HALF : -PI_HALF;
    return my_atan(y/x) + (x <= 0 ? (y > 0 ? PI : -PI) : 0);
}

#endif

To use, include `my_trig.h`.
Available functions:

```c
double my_sin(double x);
double my_cos(double x);
double my_atan(double x);
double my_atan2(double y, double x);
```

`sin` is accurate within `3.36e-9` for x in `[-100000, 100000]`.  
`cos` is accurate within `4.68e-8` for x in `[-100000, 100000]`.  
`atan2` is accurate within `1.31e-5` for x and y in `[-1e100, 1e100]`.  
`atan2` should handle all cases where `x == 0` or `y == 0` correctly.  
Infinities and NaNs are not handled.  
  
There also are some more functions that are only valid for a small interval but also faster and branchless:

```c
double my_sin_pi_half(double x); // valid in [-pi/2, pi/2]
double my_cos_pi_half(double x); // valid in [-pi/2, pi/2]
double my_atan_one   (double x); // valid in [-1, 1]
```

#pragma once

#ifdef _WIN32
#include <windows.h>

struct Time {
    static double sec(){
        LARGE_INTEGER frequency, t;
        QueryPerformanceCounter(&t);
        QueryPerformanceFrequency(&frequency);
        return t.QuadPart / (double)frequency.QuadPart;
    }
};
#else
#include <time.h>

struct Time {
    static double sec(){
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        return t.tv_sec + 1e-9*t.tv_nsec;
    }
};
#endif

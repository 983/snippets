#pragma once

#include <stdio.h>

struct Assert {
    static void my_assert(int ok, const char *msg, int line, const char *path){
        if (!ok){
            printf("ERROR: Assertion failed:\n");
            printf("%s\n", msg);
            printf("File: %s\n", path);
            printf("Line: %i\n", line);
            // trigger debugger (regular assert failed to do it)
            *(int*)0 = 1;
        }
    }
};

#define my_assert(condition) Assert::my_assert(condition, #condition, __LINE__, __FILE__);

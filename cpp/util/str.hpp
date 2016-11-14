#pragma once

#include "vector.hpp"
#include "int.hpp"
#include <string.h>

struct Str : Vector<char> {

    Str(){}
    Str(size_t len): Vector<char>(len){}
    Str(const char *c, size_t len): Vector<char>(c, len){}
    Str(const char *c): Vector<char>(c, strlen(c)){}

    u32 to_u32(u32 base = 10) const {
        if (empty()) return 0;

        u32 x = 0;

        for (size_t i = 0; i < size(); i++){
            char c = at(i);
            if (c < '0' || c > '9') break;
            x = x*base + (c - '0');
        }

        return x;
    }

    operator u32 () const {
        return to_u32();
    }

    Vector<Str> split(char c = '\n'){
        Vector<Str> output;
        size_t last = 0;
        for (size_t i = 0; i < size(); i++){
            if (at(i) == c){
                size_t n = i - last;
                if (n > 0){
                    output.push(Str(&at(last), n));
                }
                last = i + 1;
            }
        }
        size_t n = size() - last;
        if (n > 0){
            output.push(Str(&at(last), n));
        }
        return output;
    }
};

#pragma once

#include "vector.hpp"
#include "str.hpp"
#include "my_assert.hpp"
#include "int.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct File {

    static Vector<u8> load(const char *path){
        FILE *fp = fopen(path, "rb");
        if (!fp){
            printf("ERROR: Could not open file: %s\n", path);
            exit(-1);
        }
        fseek(fp, 0, SEEK_END);
        size_t n = ftell(fp);
        Vector<u8> data(n);
        rewind(fp);
        fread(data.data(), 1, n, fp);
        fclose(fp);
        return data;
    }

    static Str load_str(const char *path){
        FILE *fp = fopen(path, "rb");
        if (!fp){
            printf("ERROR: Could not open file: %s\n", path);
            exit(-1);
        }
        fseek(fp, 0, SEEK_END);
        size_t n = ftell(fp);
        Str data(n);
        rewind(fp);
        fread(data.data(), 1, n, fp);
        fclose(fp);
        return data;
    }
};

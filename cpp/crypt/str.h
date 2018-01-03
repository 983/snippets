#pragma once

#include <stdio.h>
#include <stdint.h>

typedef struct {
    char *data;
    size_t n;
    size_t capacity;
} str;

void str_init_size_capacity(str *s, const char *c, size_t n, size_t capacity);
void str_init(str *s, const char *c);
void str_free(str *s);
void str_ensure_capacity(str *s, size_t capacity);
void str_write(str *s, FILE *fp);
void str_append(str *s, const char *c);
void str_append_n(str *s, const char *c, size_t n);
void str_append_str(str *s, const str *t);
void str_append_u64_base(str *s, uint64_t x, int base);
void str_append_u64(str *s, uint64_t x);
void str_append_hex(str *s, uint8_t *bytes, size_t n_bytes);

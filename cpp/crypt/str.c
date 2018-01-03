#include "str.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SWAP(T, a, b) do { T _temp = a; a = b; b = _temp; } while(0)

void str_init_size_capacity(str *s, const char *c, size_t n, size_t capacity){
    assert(n <= capacity);
    
    s->data = (char*)malloc(capacity + 1);
    memcpy(s->data, c, n);
    s->data[n] = '\0';
    s->n = n;
    s->capacity = n;
}

void str_init(str *s, const char *c){
    size_t n = strlen(c);
    str_init_size_capacity(s, c, n, n);
}

void str_free(str *s){
    free(s->data);
    s->n = 0;
    s->capacity = 0;
}

void str_ensure_capacity(str *s, size_t capacity){
    if (s->capacity >= capacity) return;
    
    size_t new_capacity = s->capacity*2 + 1;
    
    if (new_capacity < capacity){
        new_capacity = capacity;
    }
    
    str temp[1];
    str_init_size_capacity(temp, s->data, s->n, new_capacity);
    
    SWAP(str, *s, *temp);
    
    str_free(temp);
}

void str_write(str *s, FILE *fp){
    fwrite(s->data, 1, s->n, fp);
    assert(s->data[s->n] == '\0');
}

void str_append_n(str *s, const char *c, size_t n){
    str_ensure_capacity(s, s->n + n);
    
    memcpy(s->data + s->n, c, n);
    s->n += n;
    s->data[s->n] = '\0';
}

void str_append(str *s, const char *c){
    str_append_n(s, c, strlen(c));
}

void str_append_str(str *s, const str *t){
    str_append_n(s, t->data, t->n);
}

void str_append_u64_base(str *s, uint64_t x, int base){
    const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    
    char buf[64 + 1];
    int i = 0;
    
    if (x == 0){
        buf[i++] = '0';
    }
    
    while (x){
        char digit = digits[x % base];
        buf[i++] = digit;
        x /= base;
    }
    
    str_ensure_capacity(s, s->n + i);
    
    for (int j = i - 1; j >= 0; j--){
        s->data[s->n++] = buf[j];
    }
    s->data[s->n] = '\0';
}

void str_append_u64(str *s, uint64_t x){
    str_append_u64_base(s, x, 10);
}

void str_append_hex(str *s, uint8_t *bytes, size_t n_bytes){
    const char *digits = "0123456789abcdef";
    
    str_ensure_capacity(s, s->n + n_bytes*2);
    
    for (size_t i = 0; i < n_bytes; i++){
        s->data[s->n++] = digits[bytes[i] >> 4];
        s->data[s->n++] = digits[bytes[i] & 0xf];
    }
    s->data[s->n] = '\0';
}
/*
int main(){
    
    str s[1];
    str t[1];
    str_init(s, "Hello, World!");
    str_init(t, " the end\n");
    
    str_append(s, " asdf ");
    str_append_u64(s, 12345678901234567890ull);
    
    str_append(s, " ");
    
    uint8_t bytes[7] = {0, 1, 2, 3, 4, 5, 6};
    str_append_hex(s, bytes, sizeof(bytes));
    
    str_append_str(s, t);
    
    str_write(s, stdout);
    
    str_free(s);
    
    return 0;
}

*/

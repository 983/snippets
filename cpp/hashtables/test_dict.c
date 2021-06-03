#include "dict.h"

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

char* read_text(const char *path, size_t *n_bytes){
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    size_t n = ftell(f);
    rewind(f);
    char *text = malloc(n + 1);
    size_t m = fread(text, 1, n, f);
    if (n != m){
        free(text);
        fclose(f);
        return NULL;
    }
    *n_bytes = n;
    text[n] = '\0';
    fclose(f);
    return text;
}

int is_alpha(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

void test_text_file(){
    Dict dict;
    dict_init(&dict);

    double t = sec();

    size_t n = 0;
    size_t n_words = 0;
    char *text = read_text("/home/user/data/hackernews/HNI_2010-01", &n);
    for (size_t i = 0; i < n; i++){
        while (i < n && !is_alpha(text[i])) i++;

        if (i >= n) break;

        char *word = text + i;

        while (i < n && is_alpha(text[i])) i++;

        text[i] = '\0';

        dict_put(&dict, word, NULL);

        n_words++;
    }

    double dt = sec() - t;

    printf("%f seconds\n", dt);

    assert(n_words == 4608018);
    assert(dict.size == 93169);

    free(text);

    dict_free(&dict);
}

void test_count(){
    uint32_t x = 0x12345678;

    Dict dict;
    dict_init(&dict);
    size_t checksum1 = 0;
    size_t checksum2 = 0;

    for (int i = 0; i < 1000; i++){
        // Generate random key
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;

        uint32_t value = x & 0xff;
        // Dict does not do memory management for key! Allocate key dynamically.
        char *key = malloc(32);
        snprintf(key, 32, "%i", value);

        // Sum random key values for first checksum
        checksum1 += value;

        // Store key in dict
        int create_new = 1;
        DictEntry *entry = dict_entry_create(&dict, key, &create_new);

        if (create_new){
            entry->value = (void*)1;
        }else{
            // If key has already been stored in dict, increment its count.
            size_t n = (size_t)entry->value;
            n++;
            entry->value = (void*)n;
        }
    }

    // Iterate over entries of dict.
    for (size_t i = 0; i < dict.capacity; i++){
        DictEntry *entry = &dict.entries[i];

        // Iterate over existing entries in dict. They have a key != NULL.
        if (entry->key){
            uint32_t key = atoi(entry->key);

            // Sum keys times their values for second checksum.
            checksum2 += key * (size_t)entry->value;

            // Release keys. Do not use the dict after keys have been released!
            free((void*)entry->key);
        }
    }

    assert(checksum1 == checksum2);

    dict_free(&dict);
}

int main(){
    for (int k = 0; k < 10; k++){
        test_count();
    }
    for (int k = 0; k < 10; k++){
        test_text_file();
    }
    printf("Tests passed! :)\n");

    return 0;
}


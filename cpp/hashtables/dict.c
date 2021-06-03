#include "dict.h"

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

static inline uint32_t dict_fnv1a32_hash_string(const char *string){
    uint32_t h = 0x811c9dc5;
    uint32_t prime = 0x01000193;

    for (; *string != '\0'; string++){
        h = (h ^ *(uint8_t*)string) * prime;
    }

    return h;
}

static inline int dict_streq(const char *a, const char *b){
    for (; *a && *b; a++, b++){
        if (*a != *b){
            return 0;
        }
    }
    return *a == *b;
}

void dict_init(Dict *dict){
    dict->entries = NULL;
    dict->size = 0;
    dict->capacity = 0;
}

void dict_free(Dict *dict){
    free(dict->entries);
    dict->size = 0;
    dict->capacity = 0;
}

void dict_init_capacity(Dict *dict, size_t capacity){
    dict->entries = calloc(sizeof(*dict->entries), capacity);
    dict->size = 0;
    dict->capacity = capacity;
}

void dict_rebuild(Dict *dict, size_t new_capacity){
    Dict larger_dict[1];
    dict_init_capacity(larger_dict, new_capacity);

    for (size_t i = 0; i < dict->capacity; i++){
        DictEntry *entry = &dict->entries[i];

        if (entry->key){
            dict_put(larger_dict, entry->key, entry->value);
        }
    }

    dict_free(dict);
    *dict = *larger_dict;
}

DictEntry* dict_entry_create(Dict *dict, const char *key, int *create_new){
    assert(key != NULL);

    if (dict->size * 3 >= dict->capacity * 2){
        size_t new_capacity = dict->capacity < 2 ? 2 : (dict->capacity * 7 / 4);

        dict_rebuild(dict, new_capacity);
    }

    uint32_t key_hash = dict_fnv1a32_hash_string(key);
    size_t i = key_hash % dict->capacity;

    while (1){
        DictEntry *entry = &dict->entries[i];

        if (entry->key){
            if (entry->key_hash == key_hash && dict_streq(entry->key, key)){
                if (create_new){
                    *create_new = 0;
                }
                return entry;
            }
        }else{
            if (create_new && *create_new == 1){
                *create_new = 1;
                dict->size++;
                entry->key = key;
                entry->key_hash = key_hash;
                return entry;
            }

            return NULL;
        }

        // Current entry is occupied. Try next one.
        i++;
        // If the end has been reached, wrap around to beginning.
        if (i >= dict->capacity) i = 0;
    }
}

DictEntry* dict_entry(Dict *dict, const char *key){
    return dict_entry_create(dict, key, NULL);
}

DictEntry* dict_put(Dict *dict, const char *key, void *value){
    int create_new = 1;
    DictEntry *entry = dict_entry_create(dict, key, &create_new);

    if (entry){
        entry->value = value;
    }

    return entry;
}

void* dict_get(Dict *dict, const char *key){
    DictEntry *entry = dict_entry_create(dict, key, NULL);

    return entry ? entry->value : NULL;
}

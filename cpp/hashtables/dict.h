#include <stdlib.h>

typedef struct DictEntry {
    const char *key;
    void *value;
    size_t key_hash;
} DictEntry;

typedef struct Dict {
    DictEntry *entries;
    size_t size;
    size_t capacity;
} Dict;

// Initialize an empty dict. Usually the same as memset(dict, 0, sizeof(*dict)).
void dict_init(Dict *dict);

// Initialize a dict with a certain capacity. More capacity -> faster dicts.
void dict_init_capacity(Dict *dict, size_t capacity);

// Frees a dictionary. Only necessary if dict has been used after init.
void dict_free(Dict *dict);

// Store (key, value)-pair in dict and return corresponding DictEntry.
DictEntry* dict_put(Dict *dict, const char *key, void *value);

// Find DictEntry corresponding to key.
DictEntry* dict_entry(Dict *dict, const char *key);

// Initialize new DictEntry for given key if *create_new = 1.
// The value  of *create_new will set to 0 if DictEntry for key already existed.
// This can be used to check if a value has been seen for the first time to for
// example performe some initialization work.
//
// If create_new is NULL or *create_new is not 1, a new DictEntry
// will NOT be initialized but an existing DictEntry will still be returned if
// it already existed for that key.
DictEntry* dict_entry_create(Dict *dict, const char *key, int *create_new);

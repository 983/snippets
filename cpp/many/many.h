#ifndef MANY_INCLUDED
#define MANY_INCLUDED

typedef unsigned int many_index;

#define MANY_ALLOCATION_FAILED -1

#define MAKE_MANY_HEADER(name, T) \
\
struct many_##name {\
    T *values;\
    many_index n;\
    many_index capacity;\
};\
\
void        many_##name##_init       (struct many_##name *many);\
int         many_##name##_reserve    (struct many_##name *many, many_index capacity);\
int         many_##name##_push       (struct many_##name *many, T value);\
T           many_##name##_pop        (struct many_##name *many);\
void        many_##name##_clear      (struct many_##name *many);\
void        many_##name##_free       (struct many_##name *many);\
T*          many_##name##_at         (struct many_##name *many, many_index i);\
T           many_##name##_get        (struct many_##name *many, many_index i);\
void        many_##name##_set        (struct many_##name *many, many_index i, T value);\
int         many_##name##_resize     (struct many_##name *many, many_index n, T value);\
many_index  many_##name##_size       (struct many_##name *many);\
T           many_##name##_front      (struct many_##name *many);\
T           many_##name##_back       (struct many_##name *many);\
int         many_##name##_empty      (struct many_##name *many);\
T           many_##name##_erase_pop  (struct many_##name *many, many_index i);\

#define MAKE_MANY_SOURCE(name, T) \
\
void many_##name##_init(struct many_##name *many){\
    many->values = NULL;\
    many->n = 0;\
    many->capacity = 0;\
}\
\
int many_##name##_reserve(struct many_##name *many, many_index capacity){\
    T *temp;\
    if (many->capacity >= capacity) return 0;\
    temp = (T*)realloc(many->values, sizeof(*many->values)*capacity);\
    if (!temp) return MANY_ALLOCATION_FAILED;\
    many->values = temp;\
    many->capacity = capacity;\
    return 0;\
}\
\
int many_##name##_resize(struct many_##name *many, many_index n, T value){\
    int ret = many_##name##_reserve(many, n);\
    if (ret != 0) return ret;\
    while (many->n < n) many->values[many->n++] = value;\
    if (n < many->n) many->n = n;\
    return 0;\
}\
\
int many_##name##_push(struct many_##name *many, T value){\
    if (many->n >= many->capacity){\
        int ret = many_##name##_reserve(many, many->n + many->n/2 + 1);\
        if (ret != 0) return ret;\
    }\
    many->values[many->n++] = value;\
    return 0;\
}\
\
T many_##name##_pop(struct many_##name *many){\
    assert(many->n > 0);\
    return many->values[--many->n];\
}\
\
void many_##name##_clear(struct many_##name *many){\
    many->n = 0;\
}\
\
void many_##name##_free(struct many_##name *many){\
    free(many->values);\
    many->values = NULL;\
    many->n = 0;\
    many->capacity = 0;\
}\
\
T* many_##name##_at(struct many_##name *many, many_index i){\
    assert(i < many->n);\
    return many->values + i;\
}\
T many_##name##_get(struct many_##name *many, many_index i){\
    assert(i < many->n);\
    return many->values[i];\
}\
\
void many_##name##_set(struct many_##name *many, many_index i, T value){\
    assert(i < many->n);\
    many->values[i] = value;\
}\
\
many_index many_##name##_size(struct many_##name *many){\
    return many->n;\
}\
\
T many_##name##_front(struct many_##name *many){\
    assert(many->n > 0);\
    return many->values[0];\
}\
\
T many_##name##_back(struct many_##name *many){\
    assert(many->n > 0);\
    return many->values[many->n - 1];\
}\
\
int many_##name##_empty(struct many_##name *many){\
    return many->n == 0;\
}\
T many_##name##_erase_pop(struct many_##name *many, many_index i){\
    assert(i < many->n);\
    assert(many->n > 0);\
    T erased_value = many->values[i];\
    many->values[i] = many->values[--many->n];\
    return erased_value;\
}

#endif

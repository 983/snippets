#include "many.h"

/* Some struct for testing */
struct foot {
    float toes;
};

/* Generate struct and function declarations.          */
/* 'feet' will be inserted in every function name.     */
/* Since that won't work if the type contains a space, */
/* there's a second argument for that.                 */
MAKE_MANY_HEADER(feet, struct foot)

#include <stdio.h>
#include <stdlib.h>

int main(){
    /* Unsigned integers are harder to use, e.g. try iterating from n-1 to 0. */
    /* But the compiler can optimize many things on unsigned integers,        */
    /* e.g. division and multiplication by powers of two, as well as bound    */
    /* checks (>= 0 check can be omitted due to wraparound).                  */
    /* So choose your poison wisely. For now 'many_index' is an 'unsigned int'*/
    /* but you can typedef it to 'int' instead if you like.                   */
    many_index i;

    /* All 'many' functions accept a pointer as first parameter. */
    /* Since I am lazy and don't want to write '&' all the time, */
    /* you can use this convenient trick to get a pointer:       */
    struct many_feet feet[1];

    /* Make test object */
    struct foot someones_foot;
    /* Above average number of toes. */
    someones_foot.toes = 5.5f;

    /* 'many' objects must be initialized */
    many_feet_init(feet);

    for (i = 0; i < 10; i++){
        int err;

        /* 'push' will automatically resize. */
        err = many_feet_push(feet, someones_foot);

        /* If allocation failed, you may act upon it. But if that happens, */
        /* your system will probably crash anyway, so why bother.          */
        if (err){
            printf("Did you remember to save your work? Well, too bad!\n");
            return EXIT_FAILURE;
        }
    }

    /* Difference between 'resize' and 'reserve' similar to std::vector. */
    /* Namely, you may assign to index 10-14 now, unlike with 'reserve'. */
    /* Also all new values will be initialized with a default value.     */
    many_feet_resize(feet, 15, someones_foot);

    for (i = 10; i < 15; i++){
        struct foot someone_elses_foot;
        /* Still above average number of toes :> */
        someone_elses_foot.toes = 5.0f;

        /* 'set' may only be called if 'i' < 'size'        */
        /* 'assert' is used to make sure you don't forget. */
        many_feet_set(feet, i, someone_elses_foot);
    }

    /* You may also use 'feet->n' instead of 'size' if you feel lazy. */
    /* I won't tell you what to do, I'm not your mother.              */
    for (i = 0; i < many_feet_size(feet); i++){
        printf("foot %2i has %f toes\n", i, many_feet_get(feet, i).toes);
    }

    /* Free memory and zero all the things. */
    many_feet_free(feet);

    return EXIT_SUCCESS;
}

#include <assert.h>

/* Generate function implementations. Requires 'stdlib' and 'assert'. */
/* I'd include them for you, but the preprocessor won't let me.       */
MAKE_MANY_SOURCE(feet, struct foot)

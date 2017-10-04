#ifndef MY_LIST_INCLUDED
#define MY_LIST_INCLUDED

#include <stdlib.h>

struct my_list_node {
    void *data;
    struct my_list_node *prev;
    struct my_list_node *next;
};

struct my_list {
    struct my_list_node *head;
    struct my_list_node *tail;
};

void  my_list_init     (struct my_list *list);
int   my_list_empty    (struct my_list *list);
void  my_list_push_head(struct my_list *list, void *data);
void  my_list_push_tail(struct my_list *list, void *data);
void* my_list_pop_head (struct my_list *list);
void* my_list_pop_tail (struct my_list *list);
void  my_list_free     (struct my_list *list);

#endif


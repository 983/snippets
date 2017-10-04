#include "my_list.h"

#include <stdlib.h>

void my_list_init(struct my_list *list){
    list->head = NULL;
    list->tail = NULL;
}

int my_list_empty(struct my_list *list){
    return list->head == NULL;
}

void my_list_push_head(struct my_list *list, void *data){
    struct my_list_node *node = malloc(sizeof(*node));
    
    if (list->head == NULL){
        /* initialize node */
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
        
        /* initialize list head and tail */
        list->head = node;
        list->tail = node;
    }else{
        /* initialize node */
        node->data = data;
        node->next = list->head;
        node->prev = NULL;
        
        /* update list */
        list->head->prev = node;
        list->head = node;
    }
}

void my_list_push_tail(struct my_list *list, void *data){
    struct my_list_node *node = malloc(sizeof(*node));
    
    if (list->head == NULL){
        /* initialize node */
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
        
        /* initialize list head and tail */
        list->head = node;
        list->tail = node;
    }else{
        /* initialize node */
        node->data = data;
        node->next = NULL;
        node->prev = list->tail;
        
        /* update list */
        list->tail->next = node;
        list->tail = node;
    }
}

void* my_list_pop_head(struct my_list *list){
    struct my_list_node *node = list->head;
    
    if (node == NULL) return NULL;
    
    if (node->next == NULL){
        /* special case: node is last node in list */
        list->head = NULL;
        list->tail = NULL;
    }else{
        /* replace head node with next node */
        list->head = node->next;
        
        /* tell new head node that the previous node is gone */
        list->head->prev = NULL;
    }
    
    void *data = node->data;
    
    free(node);
    
    return data;
}

void* my_list_pop_tail(struct my_list *list){
    struct my_list_node *node = list->tail;
    
    if (node == NULL) return NULL;
    
    if (node->prev == NULL){
        /* special case: node is last node in list */
        list->head = NULL;
        list->tail = NULL;
    }else{
        /* replace head node with previous node */
        list->tail = node->prev;
        
        /* tell new tail node that the next node is gone */
        list->tail->next = NULL;
    }
    
    void *data = node->data;
    
    free(node);
    
    return data;
}

void my_list_free(struct my_list *list){
    while (!my_list_empty(list)){
        my_list_pop_head(list);
    }
}

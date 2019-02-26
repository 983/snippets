#ifndef AVL_TREE_INCLUDED
#define AVL_TREE_INCLUDED

typedef struct avl_node {
    int key;
    int height;
    struct avl_node *children[2];
} avl_node;

static inline int avl_height(avl_node *node){
    return node ? node->height : -1;
}

static inline void avl_update_height(avl_node *node){
    int h0 = avl_height(node->children[0]);
    int h1 = avl_height(node->children[1]);
    node->height = 1 + (h0 > h1 ? h0 : h1);
}

static inline int avl_calculate_balance(avl_node *node){
    return avl_height(node->children[0]) - avl_height(node->children[1]);
}

static inline avl_node* avl_rotate(avl_node *node, int child_index){
    int other_index = child_index ^ 1;
    avl_node *child = node->children[child_index];
    node->children[child_index] = child->children[other_index];
    child->children[other_index] = node;
    avl_update_height(node);
    return child;
}

static inline avl_node* avl_rebalance(avl_node *node){
    if (!node) return node;
    int b = avl_calculate_balance(node);
    if (b == -2){
        if (avl_calculate_balance(node->children[1]) == 1){
            node->children[1] = avl_rotate(node->children[1], 0);
        }
        node = avl_rotate(node, 1);
    }else if (b == 2){
        if (avl_calculate_balance(node->children[0]) == -1){
            node->children[0] = avl_rotate(node->children[0], 1);
        }
        node = avl_rotate(node, 0);
    }
    avl_update_height(node);
    return node;
}

static inline avl_node* avl_insert_node(avl_node *node, avl_node *new_node){
    if (!node) return new_node;
    int child_index = new_node->key > node->key;
    node->children[child_index] = avl_insert_node(node->children[child_index], new_node);
    return avl_rebalance(node);
}

static inline avl_node* avl_find_min(avl_node *node){
    if (!node) return node;
    while (node->children[0]) node = node->children[0];
    return node;
}

static inline avl_node* avl_find_max(avl_node *node){
    if (!node) return node;
    while (node->children[1]) node = node->children[1];
    return node;
}

static inline avl_node* avl_remove_node(avl_node *node, int key, avl_node **result){
    if (!node) return node;
    if (key == node->key){
        if (result) *result = node;
        if (node->children[0] && node->children[1]){
            avl_node *max_node = avl_find_max(node->children[0]);
            max_node->children[0] = avl_remove_node(node->children[0], max_node->key, &max_node);
            max_node->children[1] = node->children[1];
            node = max_node;
        }else{
            node = node->children[0] ? node->children[0] : node->children[1];
        }
    }else{
        int child_index = key > node->key;
        node->children[child_index] = avl_remove_node(node->children[child_index], key, result);
    }
    return avl_rebalance(node);
}

static inline avl_node* avl_find(avl_node *node, int key){
    if (!node) return node;
    if (node->key == key) return node;
    return avl_find(node->children[key > node->key], key);
}

#include <stdlib.h>

static inline avl_node* avl_insert(avl_node *root, int key){
    avl_node *node = avl_find(root, key);
    if (node) return root;

    node = (avl_node*)calloc(1, sizeof(*node));
    node->key = key;
    return avl_insert_node(root, node);
}

static inline avl_node* avl_remove(avl_node *root, int key){
    avl_node *node = NULL;
    root = avl_remove_node(root, key, &node);
    free(node);
    return root;
}

#endif

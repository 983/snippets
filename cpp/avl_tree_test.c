#include "avl_tree.h"
#include "avl_tree.h"
#include "avl_tree.h"
#include "avl_tree.h"

#include <stdio.h>
#include <assert.h>

void shuffle(int *values, int n){
    for (int i = n - 1; i > 0; i--){
        int j = rand() % (i + 1);
        int temp = values[i];
        values[i] = values[j];
        values[j] = temp;
    }
}

int calculate_height(avl_node *node){
    if (!node) return -1;
    int h0 = calculate_height(node->children[0]);
    int h1 = calculate_height(node->children[1]);
    return 1 + (h0 > h1 ? h0 : h1);
}

int check(avl_node *node){
    if (!node) return 0;
    
    if (node->children[0]){
        assert(node->children[0]->key < node->key);
    }
    
    if (node->children[1]){
        assert(node->children[1]->key > node->key);
    }
    
    assert(node->height == calculate_height(node));
    
    int balance = avl_calculate_balance(node);
    assert(-1 <= balance && balance <= 1);
    
    return 1 + check(node->children[0]) + check(node->children[1]);
}

int main(){
    for (int n = 0; n < 100; n++){
        avl_node *root = NULL;
    
        int *keys = (int*)malloc(n*sizeof(*keys));
        
        for (int i = 0; i < n; i++) keys[i] = i;
        
        shuffle(keys, n);
        
        for (int i = 0; i < n; i++){
            root = avl_insert(root, keys[i]);
            
            int n_nodes_in_tree = check(root);
            assert(i + 1 == n_nodes_in_tree);
        }
        
        shuffle(keys, n);
        
        for (int i = 0; i < n; i++){
            int key = keys[i];
            
            avl_node *found = avl_find(root, key);
            
            assert(found);
            assert(found->key == key);
        }
        
        shuffle(keys, n);
        
        for (int i = 0; i < n; i++){
            int key = keys[i];
            
            root = avl_remove(root, key);
            
            int n_nodes_in_tree = check(root);
            assert(n_nodes_in_tree == n - i - 1);
        }
        
        assert(root == NULL);
        
        free(keys);
    }
    
    printf("tests passed\n");
    
    return 0;
}

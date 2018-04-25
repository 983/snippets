#include <stdio.h>
#include <assert.h>

// mutable global state
// usually not a good idea, but makes it easier to understand
int pos = 0;
int n_subset = 0;
int subset[3];

const int values[] = {0, 1, 2, 3, 4};
const int n_values = sizeof(values)/sizeof(*values);
const int k = sizeof(subset)/sizeof(*subset);

void print_subsets_recursive(void){
    // part 0
    
    // if complete subset assembled, print it
    if (n_subset == k){
        for (int i = 0; i < k; i++){
            printf("%i ", subset[i]);
        }
        printf("\n");
        return;
    }
    
    // return if not enough values left to complete subset
    if (k - n_subset > n_values - pos) return;
    
    // part 1
    
    // add value to subset and recur
    subset[n_subset++] = values[pos++];
    print_subsets_recursive();
    
    // part 2
    
    // don't add value to subset and recur
    n_subset--;
    print_subsets_recursive();
    pos--;
    
    return;
}

void print_subsets_iterative(void){
    // stack keeps track of which part of recursive function we are in
    int stack[n_values + 1];
    int stack_size = 0;
    // add first recursive function call to stack, start at part 0
    stack[stack_size++] = 0;
    
    // while the first recursive call has not returned
    while (stack_size > 0){
        // check which part of the recursive function we are in
        switch (stack[stack_size - 1]){
            case 0:
                // part 0
                if (n_subset == k){
                    for (int i = 0; i < k; i++){
                        printf("%i ", subset[i]);
                    }
                    printf("\n");
                    
                    // pop current state from stack (return)
                    stack_size--;
                    break;
                }
                
                if (k - n_subset > n_values - pos){
                    // return
                    stack_size--;
                    break;
                }
                
                // add value to subset
                subset[n_subset++] = values[pos++];
                
                // go to part 1
                stack[stack_size - 1] = 1;
                // call recursively and start at part 0
                stack[stack_size++] = 0;
                break;
            
            case 1:
                // part 1
                n_subset--;
                
                // go to part 2
                stack[stack_size - 1] = 2;
                // call recursively and start at part 1
                stack[stack_size++] = 0;
                break;
            
            case 2:
                // part 2
                pos--;
                // return
                stack_size--;
                break;
        }
    }
}

int main(){
    printf("recursive result:\n");
    print_subsets_recursive();
    printf("\n");
    printf("iterative result:\n");
    print_subsets_iterative();
    return 0;
}

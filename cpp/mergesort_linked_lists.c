#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct node {
    int value;
    struct node *next;
};

struct node* mergesort(struct node *head){
    /*
     * Properties:
     * - O(n log n)
     * - Iterative (no recursion)
     *
     * How does this work?
     *
     * 1. Split list into sub-lists 'a' and 'b' of length list_size.
     * 2. merge 'a' and 'b'.
     * 3. append merged list to result.
     * 4. continue with result as input list as long as there are sub-lists.
     *
     * Example:
     *
     * input = [8, 3, 9, 5, 2, 6]
     *
     * list_size = 1
     *
     * a = [8]
     * b = [3]
     * Merge 'a' and 'b'.
     * merged = [3, 8]
     * Append 'merged' to 'result'.
     * result = [3, 8]
     *
     * a = [9]
     * b = [5]
     * Merge 'a' and 'b'.
     * merged = [5, 9]
     * Append 'merged' to 'result'.
     * result = [3, 8, 9, 5]
     *
     * a = [2]
     * b = [6]
     * Merge 'a' and 'b'.
     * merged = [2, 6]
     * Append 'merged' to 'result'.
     * result = [3, 8, 9, 5, 2, 6]
     *
     * Continue with 'result' as 'input'.
     * input = result = [3, 8, 9, 5, 2, 6]
     *
     * list_size = 2
     *
     * a = [3, 8]
     * b = [9, 5]
     * Merge 'a' and 'b'.
     * merged = [3, 5, 8, 9]
     * Append 'merged' to 'result'.
     * result = [3, 5, 8, 9]
     *
     * a = [2, 6]
     * b is empty
     * Append 'a' to 'result'.
     * result = [3, 5, 8, 9, 2, 6]
     *
     * continue with 'result' as 'input'
     * input = result = [3, 5, 8, 9, 2, 6]
     *
     * list_size = 4
     *
     * a = [3, 5, 8, 9]
     * b = [2, 6]
     * Merge 'a' and 'b'.
     * merged = [2, 3, 5, 6, 8, 9]
     * Append 'merged' to 'result'.
     * result = [2, 3, 5, 6, 8, 9]
     *
     * list_size = 8
     *
     * list_size is larger than number of nodes in list, so we are done.
     *
     */
    size_t list_size = 1;
    while (1){
        struct node *result_head = NULL;
        struct node *result_tail = NULL;
        size_t num_nodes = 0;

        while (head){
            /* Find first sub-list 'a' of at most length list_size. */
            size_t i;
            struct node *a, *b, *merge_head, *tail = head;
            for (i = 1; i < list_size && tail->next; i++) tail = tail->next;
            num_nodes += i;
            assert(head && tail);
            a = head;

            /* Append sub-list to result if there is no second sub-list 'b'. */
            if (!tail->next){
                if (result_tail){
                    result_tail->next = head;
                    result_tail = tail;
                }else{
                    result_head = head;
                    result_tail = tail;
                }

                break;
            }
            head = tail->next;
            /* Clear next pointer of tail of 'a' to terminate sub-list 'a'. */
            tail->next = NULL;

            /* Find second sub-list 'b' of at most length list_size. */
            /* TODO: Could be built into merge step, but would be less clear. */
            tail = head;
            for (i = 1; i < list_size && tail->next; i++) tail = tail->next;
            num_nodes += i;
            assert(head && tail);
            b = head;
            head = tail->next;
            /* Clear next pointer of tail of 'b' to terminate sub-list 'b'. */
            tail->next = NULL;

            assert(a && b);

            merge_head = NULL;

            /* Merge first node of 'a' or 'b' into merged list. */
            if (a->value <= b->value){
                merge_head = tail = a;
                a = a->next;
            }else{
                merge_head = tail = b;
                b = b->next;
            }

            /* Merge remaining nodes as long as 'a' and 'b' are not empty. */
            while (a && b){
                if (a->value <= b->value){
                    tail->next = a;
                    tail = a;
                    a = a->next;
                }else{
                    tail->next = b;
                    tail = b;
                    b = b->next;
                }
            }

            /* 'a' or 'b' is empty now. Merge remaining nodes. */
            if (a) tail->next = a;
            if (b) tail->next = b;
            /* Walk 'tail' until end of list to enable appending later on. */
            while (tail->next) tail = tail->next;

            /* Append to output. */
            if (result_tail){
                result_tail->next = merge_head;
                result_tail = tail;
            }else{
                result_head = merge_head;
                result_tail = tail;
            }
        }

        head = result_head;

        list_size *= 2;
        if (list_size >= num_nodes) break;
    }

    return head;
}

void free_list(struct node *head){
    while (head){
        struct node *temp = head->next;
        free(head);
        head = temp;
    }
}

int compare_int(const void *pa, const void *pb){
    int a = *(const int*)pa;
    int b = *(const int*)pb;
    return a < b ? -1 : a > b ? 1 : 0;
}

void test(size_t n){
    size_t i;
    struct node *temp, *head;
    int *values = (int*)malloc(n * sizeof(*values));

    for (i = 0; i < n; i++) values[i] = rand() % 100;

    head = NULL;

    for (i = 0; i < n; i++){
        temp = (struct node*)malloc(sizeof(*temp));
        temp->value = values[n - 1 - i];
        temp->next = head;
        head = temp;
    }

    head = mergesort(head);

    /* Check result. */
    qsort(values, n, sizeof(*values), compare_int);
    temp = head;
    for (i = 0; i < n; i++){
        assert(temp);
        assert(values[i] == temp->value);
        temp = temp->next;
    }

    /* Cleanup. */
    free_list(head);
    free(values);
}

int main(){
    /* Test sorting of lists of length 0 to 999. */
    size_t n;
    for (n = 0; n < 1000; n++){
        test(n);
    }

    printf("Tests passed.\n");

    return 0;
}

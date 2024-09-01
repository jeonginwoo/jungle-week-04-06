#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    rbtree *rb = new_rbtree();
    rbtree_insert(rb, 45);
    rbtree_insert(rb, 12);
    rbtree_insert(rb, 89);
    rbtree_insert(rb, 33);
    rbtree_insert(rb, 76);
    rbtree_insert(rb, 59);
    rbtree_insert(rb, 8);
    rbtree_insert(rb, 92);
    rbtree_insert(rb, 23);
    rbtree_insert(rb, 67);
    rbtree_insert(rb, 18);
    rbtree_insert(rb, 38);
    rbtree_insert(rb, 54);
    rbtree_insert(rb, 71);
    rbtree_insert(rb, 29);
    rbtree_insert(rb, 81);
    rbtree_insert(rb, 5);
    rbtree_insert(rb, 96);
    rbtree_insert(rb, 50);
    rbtree_insert(rb, 64);

    rbtree_find(rb, 9);
    rbtree_find(rb, 8);
    rbtree_find(rb, 12);
    rbtree_find(rb, 13);
    rbtree_min(rb);
    rbtree_max(rb);

    key_t *arr = calloc(20, sizeof(key_t));
    rbtree_to_array(rb, arr, (size_t)15);
    for (int i=0; i<20; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    delete_rbtree(rb);
    return 0;
}
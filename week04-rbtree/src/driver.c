#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    rbtree *rb = new_rbtree();

    key_t arr[] = {5, 17, 19, 7, 19, 19, 5, 11, 13, 7, 1, 5, 19};
    int cnt = sizeof(arr) / sizeof(arr[0]);

    for(int i=0; i<cnt; i++) {
        rbtree_insert(rb, arr[i]);
    }

    printf("\n");
    printf("\n");
    printf("\n");
    preorder_print(rb, rb->root);
    printf("\n");
    printf("\n");
    printf("\n");
    printf("\n");

    for(int i=0; i<cnt; i++) {
        node_t* del = rbtree_find(rb, arr[i]);
        rbtree_erase(rb, del);
    }

    delete_rbtree(rb);
    return 0;
}
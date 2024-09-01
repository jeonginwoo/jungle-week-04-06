#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>


void inorder(rbtree *t, node_t *node);
void inorder_arr(node_t *nil, node_t *node, int* arr, int* idx);
void postorder_free(rbtree *t, node_t *node);


rbtree *new_rbtree(void)
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // TODO: initialize struct if needed
    p->nil = (node_t *)calloc(1, sizeof(node_t));
    p->nil->color = RBTREE_BLACK;
    return p;
}

void delete_rbtree(rbtree *t)
{
    // TODO: reclaim the tree nodes's memory
    postorder_free(t, t->root);
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    // TODO: implement insert
    node_t *insert_node = (node_t *)calloc(1, sizeof(node_t));
    insert_node->key = key;
    insert_node->color = RBTREE_RED;
    insert_node->right = insert_node->left = t->nil;

    if (t->root == NULL){
        insert_node->color = RBTREE_BLACK;
        t->root = insert_node;
    } else {
        node_t *now_node = t->root;
        node_t *prev_node = NULL;
        while (now_node != t->nil) {
            prev_node = now_node;
            if (key <= now_node->key){
                now_node = now_node->left;
            } else{
                now_node = now_node->right;
            }
        }

        if (key <= prev_node->key)
            prev_node->left = insert_node;
        else
            prev_node->right = insert_node;
        insert_node->parent = prev_node;
    }

    printf("insert %d\t: ", key);
    inorder(t, t->root);
    printf("\n");

    return insert_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
    // TODO: implement find
    node_t *find_node = t->root;
    while (find_node != t->nil && find_node->key != key) {
        if (key <= find_node->key) {
            find_node = find_node->left;
        } else {
            find_node = find_node->right;
        }
    }

    if (find_node != t->nil) {
        printf("find %d\t\t: find it!\n", key);
    } else {
        printf("find %d\t\t: not exist\n", key);
    }

    return find_node;
}

node_t *rbtree_min(const rbtree *t)
{
    // TODO: implement find
    node_t *search = t->root;
    if (search == NULL) {
        return NULL;
    }

    node_t *min_node = NULL;
    while (search != t->nil) {
        min_node = search;
        search = search->left;
    }

    printf("min :\t%d\n", min_node->key);

    return min_node;
}

node_t *rbtree_max(const rbtree *t)
{
    // TODO: implement find
    node_t *search = t->root;
    if (search == NULL) {
        return NULL;
    }

    node_t *max_node = NULL;
    while (search != t->nil) {
        max_node = search;
        search = search->right;
    }

    printf("max :\t%d\n", max_node->key);

    return max_node;
}

int rbtree_erase(rbtree *t, node_t *p)
{
    // TODO: implement erase
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    // TODO: implement to_array.
    int idx = 0;
    inorder_arr(t->nil, t->root, arr, &idx);
    return 0;
}

//-----------------------------

void inorder(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    inorder(t, node->left);
    printf("%d ", node->key);
    inorder(t, node->right);
}

void inorder_arr(node_t *nil, node_t *node, int* arr, int* idx) {
    if (node == nil)
        return;
    inorder_arr(nil, node->left, arr, idx);
    arr[(*idx)++] = node->key;
    inorder_arr(nil, node->right, arr, idx);
}

void postorder_free(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    postorder_free(t, node->left);
    postorder_free(t, node->right);
    free(node);
}

int main() {
    rbtree *rb = new_rbtree();
    rbtree_insert(rb, 10);
    rbtree_insert(rb, 4);
    rbtree_insert(rb, 26);
    rbtree_insert(rb, 12);
    rbtree_insert(rb, 6);
    rbtree_insert(rb, 9);
    rbtree_insert(rb, 5);
    rbtree_insert(rb, 11);
    rbtree_insert(rb, 3);
    rbtree_insert(rb, 31);
    rbtree_insert(rb, 17);
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
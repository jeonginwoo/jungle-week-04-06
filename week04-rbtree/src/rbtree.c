#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void)
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // TODO: initialize struct if needed
    p->nil->color = RBTREE_BLACK;
    return p;
}

void delete_rbtree(rbtree *t)
{
    // TODO: reclaim the tree nodes's memory
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
        return t->root;
    }

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
    return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
    // TODO: implement find
    return t->root;
}

node_t *rbtree_min(const rbtree *t)
{
    // TODO: implement find
    return t->root;
}

node_t *rbtree_max(const rbtree *t)
{
    // TODO: implement find
    return t->root;
}

int rbtree_erase(rbtree *t, node_t *p)
{
    // TODO: implement erase
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    // TODO: implement to_array
    return 0;
}

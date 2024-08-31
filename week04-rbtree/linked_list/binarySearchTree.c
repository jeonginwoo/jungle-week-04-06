#include <stdio.h>
#include <stdlib.h>

typedef int key_t;

typedef struct Node {
    key_t key;
    struct Node *parent, *left, *right;
} node_t;

typedef struct BinarySearchTree {
    node_t *root;
} bst;

bst *new_bst() {
    bst *new_bst = (bst *)calloc(1, sizeof(bst));
    return new_bst;
}

node_t *bst_insert(bst *bst, key_t key) {
    node_t *insert_node = (node_t *)calloc(1, sizeof(node_t));
    insert_node->key = key;
    if (bst->root == NULL){
        bst->root = insert_node;
        return bst->root;
    }
    
    node_t *now_node = bst->root;
    node_t *prev_node = NULL;
    while (now_node != NULL) {
        prev_node = now_node;
        if (key <= now_node->key) {
            now_node = now_node->left;
        } else {
            now_node = now_node->right;
        }
    }
    if (key <= prev_node->key) {
        prev_node->left = insert_node;
    } else {
        prev_node->right = insert_node;
    }
    insert_node->parent = prev_node;

    printf("insert %d : ", key);
    preorder(bst->root);
    printf("\n");

    return bst->root;
}

node_t *bst_find(bst *bst, key_t key) {
    node_t *find_node = bst->root;
    while (find_node != NULL && find_node->key != key) {
        if (key <= find_node->key) {
            find_node = find_node->left;
        } else {
            find_node = find_node->right;
        }
    }

    if (find_node != NULL) {
        printf("find %d : find it!\n", key);
    } else {
        printf("find %d : not exist\n", key);
    }
    
    return find_node;
}

void preorder(node_t *node) {
    if (node == NULL)
        return;
    printf("%d ", node->key);
    preorder(node->left);
    preorder(node->right);
}

void postorder(node_t *node) {
    if (node == NULL)
        return;
    postorder(node->left);
    postorder(node->right);
    printf("%d ", node->key);
    free(node);
}

void delete_tree(bst *bst) {
    postorder(bst->root);
    free(bst);
}

int main() {
    bst *bst = new_bst();
    bst_insert(bst, 10);
    bst_insert(bst, 4);
    bst_insert(bst, 26);
    bst_insert(bst, 12);
    bst_insert(bst, 6);
    bst_insert(bst, 9);
    bst_insert(bst, 5);
    bst_insert(bst, 11);
    bst_insert(bst, 3);
    bst_insert(bst, 31);
    bst_insert(bst, 17);
    bst_find(bst, 9);
    bst_find(bst, 8);
    bst_find(bst, 12);
    bst_find(bst, 13);
    delete_tree(bst);
    return 0;
}
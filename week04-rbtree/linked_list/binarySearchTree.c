#include <stdio.h>
#include <stdlib.h>

typedef int key_t;
typedef struct Node {
    key_t key;
    struct Node *parent, *left, *right;
} node;

typedef struct BinarySearchTree {
    node *root;
} bst;

void preorder(node *node);

bst *new_bst() {
    bst *new_bst = (bst *)calloc(1, sizeof(bst));
    return new_bst;
}

node *bst_insert(bst *bst, key_t key) {
    node *insert_node = (node *)calloc(1, sizeof(node));
    insert_node->key = key;
    if (bst->root == NULL){
        bst->root = insert_node;
    } else {
        node *now_node = bst->root;
        node *prev_node = NULL;
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
    }

    printf("insert %d : ", key);
    preorder(bst->root);
    printf("\n");

    return insert_node;
}

node *bst_find(bst *bst, key_t key) {
    node *find_node = bst->root;
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

int bst_delete(bst *bst, key_t key) {
    node *delete_node = bst_find(bst, key);
    if (delete_node == NULL) {
        printf("delete : not exist\n");
        return -1;
    }

    node *left = delete_node->left;
    node *right = delete_node->right;
    node *parent = delete_node->parent;

    if (left == NULL && right == NULL) {    // 자식 0
        if (delete_node == bst->root) {
            bst->root = NULL;
        } else {
            if (parent->left == delete_node) {
                parent->left = NULL;
            } else {
                parent->right = NULL;
            }
        }
    } else if (left == NULL && right != NULL) { // 자식 1 (right)
        if (delete_node == bst->root) {
            bst->root = right;
            right->parent = NULL;
        } else {
            if (parent->left == delete_node) {
                parent->left = right;
            } else {
                parent->right = right;
            }
            right->parent = parent;
        }
    } else if (left != NULL && right == NULL) { // 자식 1 (left)
        if (delete_node == bst->root) {
            bst->root = left;
            left->parent = NULL;
        } else {
            if (parent->right == delete_node) {
                parent->right = left;
            } else {
                parent->left = left;
            }
            left->parent = parent;
        }
    } else {                                        // 자식 2
        node *successor = right;
        while (successor->left != NULL) {
            successor = successor->left;
        }
        node *s_parent = successor->parent;

        if (delete_node == bst->root) {
            bst->root = successor;
            successor->parent = NULL;
        } else {
            if (parent->left == delete_node) {
                parent->left = successor;
            } else {
                parent->right = successor;
            }
            successor->parent = parent;
        }

        if (successor != right) {
            s_parent->left = successor->right;
            if (successor->right != NULL) {
                successor->right->parent = s_parent;
            }
            successor->right = right;
            right->parent = successor;
        }
        
        successor->left = left;
        left->parent = successor;
    }

    int delete_key = delete_node->key;
    printf("delete %d : ", delete_key);
    preorder(bst->root);
    printf("\n");

    free(delete_node);
    return delete_key;
}

void preorder(node *node) {
    if (node == NULL)
        return;
    printf("%d ", node->key);
    preorder(node->left);
    preorder(node->right);
}

void postorder(node *node) {
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
    bst_insert(bst, 45);
    bst_insert(bst, 12);
    bst_insert(bst, 89);
    bst_insert(bst, 33);
    bst_insert(bst, 76);
    bst_insert(bst, 59);
    bst_insert(bst, 8);
    bst_insert(bst, 92);
    bst_insert(bst, 23);
    bst_insert(bst, 67);
    bst_insert(bst, 18);
    bst_insert(bst, 38);
    bst_insert(bst, 54);
    bst_insert(bst, 71);
    bst_insert(bst, 29);
    bst_insert(bst, 81);
    bst_insert(bst, 5);
    bst_insert(bst, 96);
    bst_insert(bst, 50);
    bst_insert(bst, 64);

    bst_find(bst, 9);
    bst_find(bst, 8);
    bst_find(bst, 12);
    bst_find(bst, 13);

    bst_delete(bst, 33);
    bst_delete(bst, 89);
    bst_delete(bst, 76);
    bst_delete(bst, 59);
    bst_delete(bst, 45);
    bst_delete(bst, 8);
    bst_delete(bst, 29);
    bst_delete(bst, 12);
    bst_delete(bst, 92);
    bst_delete(bst, 54);
    bst_delete(bst, 67);
    bst_delete(bst, 18);
    bst_delete(bst, 23);
    bst_delete(bst, 38);
    bst_delete(bst, 5);
    bst_delete(bst, 81);
    bst_delete(bst, 50);
    bst_delete(bst, 64);
    bst_delete(bst, 71);
    bst_delete(bst, 96);

    bst_find(bst, 9);
    bst_find(bst, 8);
    bst_find(bst, 12);
    bst_find(bst, 13);
    delete_tree(bst);
    printf("\n");
    return 0;
}
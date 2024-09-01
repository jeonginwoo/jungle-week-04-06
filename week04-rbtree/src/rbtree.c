#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>


node_t *bst_insert(rbtree *t, const key_t key);     // 이진탐색트리 삽입
void rbtree_check(rbtree *t, node_t *check);   // rbtree 규칙 확인
void rotateRight(rbtree *t, node_t *z);     // 트리 오른쪽으로 회전
void rotateLeft(rbtree *t, node_t *z);      // 트리 왼쪽으로 회전
void inorder_print(rbtree *t, node_t *node);        // inorder 형태로 트리 출력
void inorder_arr(node_t *nil, node_t *node, int* arr, int* idx);    // inorder 형태로 배열에 저장
void postorder_free(rbtree *t, node_t *node);   // postorder 형태로 트리의 모든 노드 메모리 해제


rbtree *new_rbtree(void)
{
    rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
    // TODO: initialize struct if needed
    p->nil = (node_t *)calloc(1, sizeof(node_t));
    p->nil->color = RBTREE_BLACK;
    p->root = p->nil;
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
    node_t *insert = bst_insert(t, key);
    rbtree_check(t, insert);

    printf("insert %d\t: ", key);
    inorder_print(t, t->root);
    printf("\n");

    return insert;
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
        printf("find %d\t: find it!\n", key);
    } else {
        printf("find %d\t: not exist\n", key);
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

node_t *bst_insert(rbtree *t, const key_t key) {
    node_t *insert_node = (node_t *)calloc(1, sizeof(node_t));
    insert_node->key = key;
    insert_node->color = RBTREE_RED;
    insert_node->right = insert_node->left = t->nil;

    if (t->root == t->nil){
        insert_node->color = RBTREE_BLACK;
        t->root = insert_node;
    } else {
        node_t *now_node = t->root;
        node_t *prev_node = t->nil;
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

    return insert_node;
}

void rbtree_check(rbtree *t, node_t *check) {
    if (check == t->root) {
        check->color = RBTREE_BLACK;
        return;
    }
    if (check->parent->color == RBTREE_BLACK){
        return;
    }
    
    node_t *parent = check->parent;
    node_t *grand = parent->parent;
    node_t *uncle = (grand->left == parent)? grand->right : grand->left;
    
    if (uncle->color == RBTREE_RED) {
        parent->color = uncle->color = RBTREE_BLACK;
        grand->color = RBTREE_RED;
        rbtree_check(t, grand);
    } else {
        grand->color = RBTREE_RED;
        if (grand->left == parent) {
            if (parent->left == check) {
                parent->color = RBTREE_BLACK;
                rotateRight(t, grand);
                rbtree_check(t, parent);
            } else {
                check->color = RBTREE_RED;
                rotateLeft(t, parent);
                rotateRight(t, grand);
                rbtree_check(t, check);
            }
        } else {
            if (parent->right == check) {
                parent->color = RBTREE_BLACK;
                rotateLeft(t, grand);
                rbtree_check(t, parent);
            } else {
                check->color = RBTREE_RED;
                rotateRight(t, parent);
                rotateLeft(t, grand);
                rbtree_check(t, check);
            }
        }
    }
}

void rotateRight(rbtree *t, node_t *z) {
    if (z == NULL) return;
    node_t *x = z->left;
    if (x == NULL) return;
    node_t *b = x->right;
    x->parent = z->parent;
    if (z->parent != NULL) {
        if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
    }
    x->right = z;
    z->parent = x;
    z->left = b;
    if (b != NULL) b->parent = z;
    if (z == t->root) t->root = x;
}

void rotateLeft(rbtree *t, node_t *z) {
    if (z==NULL) return;
    node_t *x = z->right;
    if (x==NULL) return;
    node_t *b = x->left;
    x->parent = z->parent;
    if (z->parent != NULL) {
        if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
    }
    x->left = z;
    z->parent = x;
    z->right = b;
    if (b != NULL) b->parent = z;
    if (z == t->root) t->root = x;
}

void inorder_print(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    inorder_print(t, node->left);
    printf("%d ", node->key);
    inorder_print(t, node->right);
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
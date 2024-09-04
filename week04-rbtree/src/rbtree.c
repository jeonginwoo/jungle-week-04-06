#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>


// 이진탐색트리 삽입
node_t *bst_insert(rbtree *t, const key_t key);
// 삽입 규칙 확인
void rbtree_insert_check(rbtree *t, node_t *check);
// 이진탐색트리 삭제
node_t *bst_delete(rbtree *t, node_t *delete_node, color_t *delete_color);
// 삭제 규칙 확인
void rbtree_delete_check(rbtree *t, node_t *node);
// 트리 오른쪽으로 회전
void rotateRight(rbtree *t, node_t *z);
// 트리 왼쪽으로 회전
void rotateLeft(rbtree *t, node_t *z);
// inorder 형태로 배열에 저장
void inorder_arr(node_t *nil, node_t *node, int* arr, int* idx, const size_t n);
// postorder 형태로 트리의 모든 노드 메모리 해제
void postorder_free(rbtree *t, node_t *node);


rbtree *new_rbtree(void)
{
    rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
    t->nil = (node_t *)calloc(1, sizeof(node_t));
    t->nil->color = RBTREE_BLACK;
    t->root = t->nil;
    return t;
}

void delete_rbtree(rbtree *t)
{
    postorder_free(t, t->root);
    free(t->nil);
    free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    node_t *insert = bst_insert(t, key);
    rbtree_insert_check(t, insert);

    // printf("insert %d\t: ", key);
    // preorder_print(t, t->root);
    // printf("\n");

    return insert;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
    node_t *find_node = t->root;
    while (find_node != t->nil && find_node->key != key) {        
        if (key <= find_node->key) {
            find_node = find_node->left;
        } else {
            find_node = find_node->right;
        }
    }

    return (find_node != t->nil) ? find_node : NULL;
}

node_t *rbtree_min(const rbtree *t)
{
    node_t *search = t->root;
    if (search == NULL) {
        return NULL;
    }

    node_t *min_node = NULL;
    while (search != t->nil) {
        min_node = search;
        search = search->left;
    }

    return min_node;
}

node_t *rbtree_max(const rbtree *t)
{
    node_t *search = t->root;
    if (search == NULL) {
        return NULL;
    }

    node_t *max_node = NULL;
    while (search != t->nil) {
        max_node = search;
        search = search->right;
    }

    return max_node;
}

int rbtree_erase(rbtree *t, node_t *p)
{
    key_t key = p->key;
    color_t delete_color;
    node_t *replace_node = bst_delete(t, p, &delete_color);
    if (delete_color == RBTREE_BLACK) {
        rbtree_delete_check(t, replace_node);
    }

    // printf("delete %d\t: ", key);
    // preorder_print(t, t->root);
    // printf("\n");
    
    return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    int idx = 0;
    inorder_arr(t->nil, t->root, arr, &idx, n);
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
        insert_node->parent = t->nil;
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

void rbtree_insert_check(rbtree *t, node_t *check) {
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
    
    // 삼촌이 red
    if (uncle->color == RBTREE_RED) {
        parent->color = uncle->color = RBTREE_BLACK;
        grand->color = RBTREE_RED;
        rbtree_insert_check(t, grand);
    // 삼촌이 black
    } else {
        grand->color = RBTREE_RED;
        if (grand->left == parent) {
            // 부모가 left, 체크 노드가 left
            if (parent->left == check) {
                parent->color = RBTREE_BLACK;
                rotateRight(t, grand);
            // 부모가 left, 체크 노드가 right
            } else {
                check->color = RBTREE_BLACK;
                rotateLeft(t, parent);
                rotateRight(t, grand);
            }
        } else {
            // 부모가 right, 체크 노드가 right
            if (parent->right == check) {
                parent->color = RBTREE_BLACK;
                rotateLeft(t, grand);
            // 부모가 right, 체크 노드가 left
            } else {
                check->color = RBTREE_BLACK;
                rotateRight(t, parent);
                rotateLeft(t, grand);
            }
        }
    }
}

node_t *bst_delete(rbtree *t, node_t *delete_node, color_t *delete_color) {
    node_t *replace_node;
    node_t *left = delete_node->left;
    node_t *right = delete_node->right;
    node_t *parent = delete_node->parent;
    *delete_color = delete_node->color;

    // case1 : 자식 0
    if (left == t->nil && right == t->nil) {    
        replace_node = t->nil;
        if (delete_node == t->root) {
            t->root = t->nil;
        } else {
            if (parent->left == delete_node) {
                parent->left = t->nil;
            } else {
                parent->right = t->nil;
            }
        }
        replace_node->parent = parent;
    // case2-1 : 자식 1 (right)
    } else if (left == t->nil) {
        replace_node = right;
        if (delete_node == t->root) {
            t->root = right;
            right->parent = t->nil;
        } else {
            if (parent->left == delete_node) {
                parent->left = right;
            } else {
                parent->right = right;
            }
            right->parent = parent;
        }
    // case2-2 : 자식 1 (left)
    } else if (right == t->nil) {
        replace_node = left;
        if (delete_node == t->root) {
            t->root = left;
            left->parent = t->nil;
        } else {
            if (parent->right == delete_node) {
                parent->right = left;
            } else {
                parent->left = left;
            }
            left->parent = parent;
        }
    // case3 : 자식 2 (successor의 color가 삭제됨)
    } else {
        node_t *successor = right;
        while (successor->left != t->nil) {         
            successor = successor->left;
        }

        // 삭제되는 노드는 successor
        *delete_color = successor->color;
        replace_node = successor->right;
        delete_node->key = successor->key;
        
        if (successor == right) {
            delete_node->right = replace_node;
            replace_node->parent = delete_node;
        } else {
            successor->parent->left = replace_node;
            replace_node->parent = successor->parent;
        }
        
        delete_node = successor;
    }

    free(delete_node);
    return replace_node;
}

void rbtree_delete_check(rbtree *t, node_t *x) {
    if (t->root == x || x->color == RBTREE_RED) {
        x->color = RBTREE_BLACK;
        return;
    }

    node_t *bro;
    if (x->parent->left == x) {
        bro = x->parent->right;

        // case1 : 형제가 red
        if (bro->color == RBTREE_RED) {
            bro->color = RBTREE_BLACK;
            x->parent->color = RBTREE_RED;
            rotateLeft(t, x->parent);
            bro = x->parent->right;
        }
        // case2 : 형제의 자녀가 모두 black
        if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
            bro->color = RBTREE_RED;
            rbtree_delete_check(t, x->parent);
            return;
        }
        // case3 : 형제의 왼쪽 자녀가 red
        if (bro->left->color == RBTREE_RED && bro->right->color == RBTREE_BLACK) {
            bro->left->color = RBTREE_BLACK;
            bro->color = RBTREE_RED;
            rotateRight(t, bro);
            bro = x->parent->right;
        }
        // case4 : 형제의 오른쪽 자녀가 red
        bro->color = x->parent->color;
        x->parent->color = bro->right->color = RBTREE_BLACK;
        rotateLeft(t, x->parent);
    } else {
        bro = x->parent->left;

        // case1 : 형제가 red
        if (bro->color == RBTREE_RED) {
            bro->color = RBTREE_BLACK;
            x->parent->color = RBTREE_RED;
            rotateRight(t, x->parent);
            bro = x->parent->left;
        }
        // case2 : 형제의 자녀가 모두 black
        if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK) {
            bro->color = RBTREE_RED;
            rbtree_delete_check(t, x->parent);
            return;
        }
        // case3 : 형제의 오른쪽 자녀가 red
        if (bro->right->color == RBTREE_RED && bro->left->color == RBTREE_BLACK) {
            bro->right->color = RBTREE_BLACK;
            bro->color = RBTREE_RED;
            rotateLeft(t, bro);
            bro = x->parent->left;
        }
        // case4 : 형제의 왼쪽 자녀가 red
        bro->color = x->parent->color;
        x->parent->color = bro->left->color = RBTREE_BLACK;
        rotateRight(t, x->parent);
    }
    x->color = RBTREE_BLACK;
    t->nil->parent = t->nil;
}

void rotateRight(rbtree *t, node_t *z) {
    if (z == t->nil) return;
    node_t *x = z->left;
    if (x == t->nil) return;
    node_t *b = x->right;
    x->parent = z->parent;
    if (z->parent != t->nil) {
        if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
    }
    x->right = z;
    z->parent = x;
    z->left = b;
    if (b != t->nil) b->parent = z;
    if (z == t->root) t->root = x;
}

void rotateLeft(rbtree *t, node_t *z) {
    if (z==t->nil) return;
    node_t *x = z->right;
    if (x==t->nil) return;
    node_t *b = x->left;
    x->parent = z->parent;
    if (z->parent != t->nil) {
        if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
    }
    x->left = z;
    z->parent = x;
    z->right = b;
    if (b != t->nil) b->parent = z;
    if (z == t->root) t->root = x;
}

void preorder_print(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    printf("%02d(%c) ", node->key, (node->color==RBTREE_BLACK)?'B':'R');
    preorder_print(t, node->left);
    preorder_print(t, node->right);
}

void inorder_print(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    inorder_print(t, node->left);
    printf("%02d(%c) ", node->key, (node->color==RBTREE_BLACK)?'B':'R');
    inorder_print(t, node->right);
}

void postorder_print(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    postorder_print(t, node->left);
    postorder_print(t, node->right);
    printf("%02d(%c) ", node->key, (node->color==RBTREE_BLACK)?'B':'R');
}

void inorder_arr(node_t *nil, node_t *node, int *arr, int *idx, const size_t n) {
    if (node == nil || *idx >= n)
        return;
    inorder_arr(nil, node->left, arr, idx, n);
    arr[(*idx)++] = node->key;
    inorder_arr(nil, node->right, arr, idx, n);
}

void postorder_free(rbtree *t, node_t *node) {
    if (node == t->nil)
        return;
    postorder_free(t, node->left);
    postorder_free(t, node->right);
    free(node);
}
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int key;
    struct node *next;
} node;

typedef struct SinglyLinkedList
{
    node *head;  // head는 node의 포인터
    int size;
} sll;

sll* newSll()
{
    sll* new_sll = (sll*)calloc(1, sizeof(sll));
    new_sll->size = 0;
    return new_sll;
}

void pushFront(sll* sll, int key)
{
    node* push_node = (node*)calloc(1, sizeof(node));
    push_node->key = key;
    push_node->next = sll->head;
    sll->head = push_node;
    sll->size++;
    printf("push front %d : ", key);
    print(sll);
}

void print(sll* sll)
{
    node* now = sll->head;
    while (now != NULL){
        printf("%d", now->key);
        now = now->next;
        if(now != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void deleteList(sll* s)
{
    free(s);
}


int main()
{
    sll* sll = newSll();
    pushFront(sll, 12);
    pushFront(sll, 17);
    pushFront(sll, 4);
    pushFront(sll, 7);
    pushFront(sll, 15);
    return 0;
}
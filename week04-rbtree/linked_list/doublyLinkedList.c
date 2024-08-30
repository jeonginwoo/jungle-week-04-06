#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int key;
    struct Node *prev;
    struct Node *next;
} node;

typedef struct DoublyLinkedList
{
    node *head;
    int size;
} dll;

dll *newDll()
{
    dll *new_dll = (dll *)calloc(1, sizeof(dll));
    node *head_node = (node *)calloc(1, sizeof(node));
    new_dll->head = head_node;
    new_dll->size = 0;
    return new_dll;
}

void pushFront(dll *dll, int key)
{
    node *push_node = (node *)calloc(1, sizeof(node));
    push_node->key = key;
    if (dll->size == 0){
        dll->head->next = dll->head->prev = push_node;
    } else {
        push_node->next = dll->head->next;
        dll->head->next->prev = push_node;
        dll->head->next = push_node;
        push_node->prev = dll->head;
    }
    dll->size++;
    printf("push front %d : ", key);
    print(dll);
}

// void pushBack(dll *dll, int key)
// {

// }

// int popFront(dll *dll)
// {
// }

// int popBack(dll *dll)
// {
// }

// node *search(dll *dll, int key)
// {
// }

// void removeIdx(dll *dll, int idx)
// {
// }

void print(dll *dll)
{
    if (dll->size == 0)
    {
        printf("[]\n");
        return;
    }

    node *now = dll->head->next;
    while (now != dll->head->prev)
    {
        printf("%d ←→ ", now->key);
        now = now->next;
    }
    printf("%d\n", now->key);
}

void deleteList(dll *dll)
{
}

int main()
{
    dll *dll = newDll();
    pushFront(dll, 12);
    pushFront(dll, 17);
    pushFront(dll, 4);
    pushFront(dll, 7);
    pushFront(dll, 15);
    return 0;
}
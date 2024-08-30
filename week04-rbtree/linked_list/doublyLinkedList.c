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
    head_node->next = head_node->prev = head_node;
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

void pushBack(dll *dll, int key)
{
    node *push_node = (node *)calloc(1, sizeof(node));
    push_node->key = key;
    if (dll->size == 0){
        dll->head->next = dll->head->prev = push_node;
    } else {
        push_node->prev = dll->head->prev;
        dll->head->prev->next = push_node;
        dll->head->prev = push_node;
        push_node->next = dll->head;
    }
    dll->size++;
    printf("push back %d : ", key);
    print(dll);
}

int popFront(dll *dll)
{
    if (dll->size == 0){
        printf("List is empty, cannot pop.\n");
        return -1;
    }
    int pop_key;
    node *pop_node = dll->head->next;
    pop_key = pop_node->key;

    dll->head->next = pop_node->next;
    pop_node->next->prev = dll->head;
    free(pop_node);

    dll->size--;

    printf("pop front : ");
    print(dll);
    return pop_key;
}

int popBack(dll *dll)
{
    if (dll->size == 0){
        printf("List is empty, cannot pop.\n");
        return -1;
    }
    int pop_key;
    node *pop_node = dll->head->prev;
    pop_key = pop_node->key;

    dll->head->prev = pop_node->prev;
    pop_node->prev->next = dll->head;
    free(pop_node);

    dll->size--;

    printf("pop back : ");
    print(dll);
    return pop_key;
}

node *search(dll *dll, int key)
{
    node *find_node = dll->head;
    while (find_node->next != dll->head) {
        find_node = find_node->next;
        if (find_node->key == key) {
            printf("search %d : find!\n", key);
            return find_node;
        }
    }
    printf("search %d : not exist\n", key);
    return NULL;
}

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
    pushBack(dll, 28);
    pushBack(dll, 21);
    pushBack(dll, 43);
    pushBack(dll, 35);
    pushBack(dll, 6);
    popFront(dll);
    popFront(dll);
    popFront(dll);
    popBack(dll);
    popBack(dll);
    popBack(dll);
    search(dll, 17);
    search(dll, 18);
    return 0;
}
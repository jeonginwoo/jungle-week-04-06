#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int key;
    struct node *next;
} node;

typedef struct SinglyLinkedList
{
    node *head; // head는 node의 포인터
    int size;
} sll;

sll *newSll()
{
    sll *new_sll = (sll *)calloc(1, sizeof(sll));
    new_sll->size = 0;
    return new_sll;
}

void pushFront(sll *sll, int key)
{
    node *push_node = (node *)calloc(1, sizeof(node));
    push_node->key = key;
    push_node->next = sll->head;
    sll->head = push_node;
    sll->size++;
    printf("push front %d : ", key);
    print(sll);
}

void pushBack(sll *sll, int key)
{
    node *push_node = (node *)calloc(1, sizeof(node));
    push_node->key = key;

    if (sll->head == NULL) {
        sll->head = push_node;
    } else {
        node *tail_node = sll->head;
        while (tail_node->next != NULL)
            tail_node = tail_node->next;
        tail_node->next = push_node;
    }
    
    sll->size++;
    printf("push back %d : ", key);
    print(sll);
}

int popFront(sll *sll)
{
    if (sll->head == NULL)
    {
        printf("List is empty, cannot pop.\n");
        return -1;
    }

    int pop_key;
    node *pop_node = sll->head;
    pop_key = pop_node->key;
    sll->head = pop_node->next;

    free(pop_node);
    sll->size--;

    printf("pop front : ");
    print(sll);
    return pop_key;
}

int popBack(sll *sll)
{
    if (sll->head == NULL)
    {
        printf("List is empty, cannot pop.\n");
        return -1;
    }

    int pop_key;
    node *tail_node = sll->head;
    node *prev_node = NULL;

    while (tail_node->next != NULL)
    {
        prev_node = tail_node;
        tail_node = tail_node->next;
    }
    pop_key = tail_node->key;

    if (prev_node == NULL) {
        sll->head = NULL;
    } else {
        prev_node->next = NULL;
    }

    free(tail_node);
    sll->size--;

    printf("pop back : ");
    print(sll);
    return pop_key;
}

void print(sll *sll)
{
    if (sll->head == NULL)
    {
        printf("[]\n");
        return;
    }

    node *now = sll->head;
    while (now != NULL)
    {
        printf("%d", now->key);
        now = now->next;
        if (now != NULL)
        {
            printf(" -> ");
        }
    }
    printf("\n");
}

void deleteList(sll *sll)
{
    node *current = sll->head;
    node *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(sll);  // 리스트 구조체 자체도 해제
}

int main()
{
    sll *sll = newSll();
    pushFront(sll, 12);
    pushFront(sll, 17);
    pushFront(sll, 4);
    pushFront(sll, 7);
    pushFront(sll, 15);
    pushBack(sll, 21);
    pushBack(sll, 1);
    pushBack(sll, 9);
    popFront(sll);
    popFront(sll);
    popBack(sll);
    popBack(sll);
    popBack(sll);
    popFront(sll);
    popFront(sll);
    popBack(sll);
    popBack(sll);
    pushBack(sll, 21);
    pushBack(sll, 1);
    pushBack(sll, 9);
    popBack(sll);
    popBack(sll);
    popBack(sll);
    popBack(sll);
    popFront(sll);
    deleteList(sll);
    return 0;
}
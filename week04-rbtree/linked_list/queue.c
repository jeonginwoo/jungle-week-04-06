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

void pushFront(sll *sll, int key)
{
    node *push_node = (node *)malloc(sizeof(node));  // 동적 메모리 할당
    push_node->key = key;
    push_node->next = sll->head;  // 새로운 노드의 next가 현재 head를 가리키게 함
    sll->head = push_node;  // head가 새로운 노드를 가리키게 함
    sll->size++;
}

int len(sll sll)
{
    return sll.size;
}

void print(sll sll)
{
    node *now = sll.head;
    while (now != NULL) {
        printf("%d", now->key);
        now = now->next;
        if (now != NULL) {
            printf(" -> ");
        }
    }
    printf("\n");
}

int main()
{
    sll sll = {NULL, 0};  // head를 NULL로 초기화
    pushFront(&sll, 5);
    pushFront(&sll, 10);
    pushFront(&sll, 15);
    print(sll);
    return 0;
}

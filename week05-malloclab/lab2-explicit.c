/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "jungle week05 team08",
    /* First member's full name */
    "Jeong inwoo",
    /* First member's email address */
    "jiw413@naver.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};


#define WSIZE       4           // 워드 크기 (bytes)
#define DSIZE       8           // 더블 워드 (bytes)
#define CHUNKSIZE   (1<<12)     // 초기 가용 블록과 힙 확장을 위한 기본 크기  (4KB)

#define MAX(x, y) ((x) > (y)? (x) : (y))

// 크기와 할당 비트를 통합해서 헤더와 풋터에 저장할 수 있는 값을 리턴
#define PACK(size, alloc)   ((size) | (alloc))      // alloc : 가용여부 (가용: 0, 할당: 1) / size : block size => 합치면 온전한 주소

// 각각 주소 p에 워드를 read, write 한다.
#define GET(p)      (*(unsigned int *)(p))          // 인자 p가 참조하는 워드(4byte)를 읽어서 리턴
#define PUT(p, val) (*(unsigned int *)(p) = (val))  // 인자 p가 가리키는 워드(4byte)에 val 저장

// 각각 주소 p에 있는 헤더 또는 풋터의 size와 할당 비트를 리턴
#define GET_SIZE(p)     (GET(p) & ~0x7)     // size 리턴
#define GET_ALLOC(p)    (GET(p) & 0x1)      // 할당 비트 리턴

// 각각 블록 헤더와 풋터를 가리키는 포인터를 리턴
#define HDRP(bp)        ((char *)(bp) - WSIZE)      // (헤더 주소) = (블록의 시작 주소) - (헤더 사이즈)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // (풋터 주소) = (블록의 시작 주소) + (블록 사이즈) - (헤더+풋터 사이즈)

// 다음과 이전 블록의 블록 포인터를 각각 리턴
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))   // (다음 블록 포인터) = (현재 블록 포인터) + (현재 블록 사이즈)
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))   // (이전 블록 포인터) = (현재 블록 포인터) - (이전 블록 사이즈)

/* explicit 추가 매크로 */
#define PREV(bp) (*(char **)(bp))
#define NEXT(bp) (*(char **)(bp + WSIZE))


static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void *heap_listp;    // 힙의 시작주소 pointer

/* explicit 추가 함수 */
static void removeBlock(void *bp);         // 할당 블록을 freelist에서 지운다
static void putFreeBlock(void *bp);        // 가용 블록을 freelist에 추가한다
static void *free_listp;    // free list head - 가용리스트 시작 부분


int mm_init(void)
{
    // 메모리 시스템에서 6워드를 가져와서 빈 가용 리스트를 만들 수 있도록 초기화
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1) // heap_list는 메모리 할당 전 heap의 top 위치 저장
        return -1;
    PUT(heap_listp, 0);             // 더블 워드 경계로 정렬된 미사용 패딩 워드 (경계는 bp 기준)
    PUT(heap_listp + (1*WSIZE), PACK(4*WSIZE, 1));      // 프롤로그 헤더
    PUT(heap_listp + (2*WSIZE), NULL);                  // 프롤로그 PRED 포인터 NULL로 초기화
    PUT(heap_listp + (3*WSIZE), NULL);                  // 프롤로그 SUCC 포인터 NULL로 초기화
    PUT(heap_listp + (4*WSIZE), PACK(4*WSIZE, 1));      // 프롤로그 풋터
    PUT(heap_listp + (5*WSIZE), PACK(0, 1));            // 에필로그 헤더
    free_listp = heap_listp + DSIZE;        // free_listp를 PRED 포인터를 가리키도록 초기화

    // 힙을 CHUNKSIZE 바이트로 확장하고 초기 가용 블록 생성
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}


/* 
 * extend_heap 호출
 * 1. 힙이 초기화될 때
 * 2. mm_malloc이 적당한 맞춤 fit을 찾지 못했을 때
 * 정렬을 유지하기 위해 요청한 크기를 인접 2워드 배수(8바이트)로 반올림하며, 그 후에 메모리 시스템으로부터 추가적인 힙 공간 요청
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // 확장할 블록의 크기를 짝수 워드로 반올림하여 mem_sbrk()로 요청
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)  // bp는 메모리 추가 할당하기 전 힙의 탑 주소. 힙의 마지막에는 에필로그 블록이 있기 때문에 해당 블록을 헤더로 사용하면 bp가 블록 포인터가 됨.
        return NULL;
    
    // 새로 확장된 힙에 대해 헤더와 풋터를 설정하고, 힙의 끝에 에필로그 헤더를 설정
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // 새로 확장된 가용 블록을 병합
    return coalesce(bp);
}

static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))) || PREV_BLKP(bp) == bp;     // 이전 블록이 할당 되었는지. 0 or 1
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));     // 다음 블록이 할당 되었는지. 0 or 1
    size_t size = GET_SIZE(HDRP(bp));                       // 현재 블록 사이즈

    // explicit에서는 가용블록이 없으면 조건을 추가할 필요가 없다
    // if (prev_alloc && next_alloc) {          // Case 1 : 이전 블록과 다음 블록 모두 할당됨 -> 병합 x
    //     return bp;
    // }

    if (prev_alloc && !next_alloc) {            // Case 2 : 이전 블록은 할당되고, 다음 블록은 가용됨 -> 현재 블록과 다음 블록 병합
        removeBlock(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {       // Case 3 : 이전 블록은 가용되고, 다음 블록은 할당됨 -> 현재 블록과 이전 블록 병합
        removeBlock(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    else if (!prev_alloc && !next_alloc) {      // Case 4 : 이전 블록과 다음 블록 모두 가용됨 -> 세 블록 모두 병합
        removeBlock(NEXT_BLKP(bp));
        removeBlock(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }

    putFreeBlock(bp);
    return bp;
}


/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    // 요청 크기가 0인 경우 NULL 반환
    if (size == 0)
        return NULL;
    
    if (size <= DSIZE)  // 요청 크기가 더블 워드보다 작을 경우 최소 블록 크기인 16바이트로 설정
        asize = 2 * DSIZE;
    else                // 더블 워드 경계에 맞게 정렬 (패딩 추가)
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

    // 적절한 가용 블록을 찾은 경우 그 블록에 메모리를 할당
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    // 적절한 블록을 찾지 못한 경우 힙을 확장하여 메모리 할당
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    
    place(bp, asize);
    return bp;
}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    if (bp == NULL)
        return;
    size_t size = GET_SIZE(HDRP(bp));   // 블록의 사이즈 가져오기
    PUT(HDRP(bp), PACK(size, 0));   // 헤더 alloc bit 0으로 설정
    PUT(FTRP(bp), PACK(size, 0));   // 풋터 alloc bit 0으로 설정
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    // 요청 크기가 0 이하인 경우 해당 포인터 해제 후 NULL 반환
    if (size <= 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t old_size = GET_SIZE(HDRP(ptr));  // 이전 블록의 크기를 가져온다
    size_t new_size = size + (2*WSIZE);     // 새로운 요청 크기를 설정한다

    // 새로운 크기가 이전 크기보다 작거나 같으면 기존 포인터 반환
    if (new_size <= old_size)
        return ptr;
    
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(ptr))); // 다음 블록의 할당 여부 확인
    size_t csize;

    // 다음 블록이 가용 상태이고, 합쳐진 크기가 새로운 크기보다 크거나 같다면 기존 포인터 반환
    if (!next_alloc && ((csize = old_size + GET_SIZE(HDRP(NEXT_BLKP(ptr))))) >= new_size) {
        removeBlock(NEXT_BLKP(ptr));
        PUT(HDRP(ptr), PACK(csize, 1));
        PUT(FTRP(ptr), PACK(csize, 1));
        return ptr;
    // 새로운 크기로 할당할 공간이 부족하면
    } else {
        void *new_ptr = mm_malloc(new_size);
        place(new_ptr, new_size);           // 새로운 메모리 블록 할당
        memcpy(new_ptr, ptr, old_size);     // 기존 데이터를 새로운 메모리 블록으로 복사
        mm_free(ptr);                       // 기존 메모리 블록을 해제
        return new_ptr;                     // 새로운 메모리 블록의 포인터 반환
    }
}

// 가용 리스트에서 적절한 크기의 블록을 찾는 역할 (first-fit 방식)
static void *find_fit(size_t asize)
{
    void *bp;

    // 가용리스트 내부의 유일한 할당블록인 프롤로그 블록을 만나면 종료
    for (bp = free_listp; !GET_ALLOC(HDRP(bp)); bp = NEXT(bp)) {
        // 할당 비트가 1이 아니고 찾는 사이즈 이상이면
        if ((asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;
}

// 찾은 가용 블록에 실제로 메모리를 할당
// 요청한 크기(asize)만큼의 메모리를 블록에 배치하고, 남는 공간이 충분히 크면 새로운 가용 블록으로 분할
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    removeBlock(bp);
    // 블록을 분할하는 경우
    if ((csize - asize) >= (2*DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));          // 헤더에 크기와 할당 상태 저장
        PUT(FTRP(bp), PACK(asize, 1));          // 풋터에 크기와 할당 상태 저장
        bp = NEXT_BLKP(bp);                     // 남은 공간의 시작 주소 계산
        PUT(HDRP(bp), PACK(csize-asize, 0));    // 나머지 가용 블록의 헤더 저장
        PUT(FTRP(bp), PACK(csize-asize, 0));    // 나머지 가용 블록의 풋터 저장
        putFreeBlock(bp);                       // free list 첫 번재에 분할된 블록을 넣는다
    }
    // 블록을 분할하지 않는 경우
    else {
        PUT(HDRP(bp), PACK(csize, 1));      // 전체 브록을 할달 상태로 설정
        PUT(FTRP(bp), PACK(csize, 1));      // 전체 블록을 할당 상태로 설정
    }
}

// 연결 리스트에 추가
static void putFreeBlock(void *bp)
{
    NEXT(bp) = free_listp;
    PREV(bp) = NULL;
    PREV(free_listp) = bp;
    free_listp = bp;
}

// 연결 리스트에서 제거
static void removeBlock(void *bp)
{
    // 다음 블록이 있다면
    if (PREV(bp)) {
        NEXT(PREV(bp)) = NEXT(bp);  // 다음 블록의 주소에 이전 블록의 주소 저장
    // 다음 블록이 없다면 (자신이 제일 앞 블록)
    } else {
        free_listp = NEXT(bp);      // 이전 노드를 제일 앞 블록으로 만든다
    }
    PREV(NEXT(bp)) = PREV(bp);
}
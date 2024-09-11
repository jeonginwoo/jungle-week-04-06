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
#include <limits.h>

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
    "정인우",
    /* First member's email address */
    "jiw413@naver.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

#define WSIZE               4
#define DSIZE               8
#define CHUNKSIZE           (1<<12)

#define MAX(x, y)           ((x) > (y)? (x) : (y))

#define PACK(size, alloc)   ((size) | (alloc))

#define GET(p)              (*(unsigned int *)(p))
#define PUT(p, val)         (*(unsigned int *)(p) = (unsigned int)(val))

#define GET_SIZE(p)         (GET(p) & ~0x7)
#define GET_ALLOC(p)        (GET(p) & 0x1)

#define HDRP(bp)            ((char *)(bp) - WSIZE)
#define FTRP(bp)            ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BP(bp)         ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BP(bp)         ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

#define PREV(bp)            (*(char **)bp)
#define NEXT(bp)            (*(char **)(bp + WSIZE))

static void *heap_listp;
static void *free_listp;

static void *extend_heap(size_t words);
static void *coalesce(char *bp);
// static void *best_fit(size_t asize);
static void *first_fit(size_t asize);
static void place(char *bp, size_t asize);
static void delete_block(char *bp);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1) {
        return -1;
    }
    PUT(heap_listp + 0*WSIZE, PACK(0, 0));          // 미사용 패딩
    PUT(heap_listp + 1*WSIZE, PACK(2*DSIZE, 1));    // 프롤로그 헤더
    PUT(heap_listp + 2*WSIZE, NULL);                // 프롤로그 prev
    PUT(heap_listp + 3*WSIZE, NULL);                // 프롤로그 next
    PUT(heap_listp + 4*WSIZE, PACK(2*DSIZE, 1));    // 프롤로그 풋터
    PUT(heap_listp + 5*WSIZE, PACK(0, 1));          // 에필로그 헤더
    free_listp = heap_listp = heap_listp + DSIZE;   // 

    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

static void *extend_heap(size_t words)
{   
    char *bp;
    size_t size = (words % 2 == 0)? words * WSIZE : (words + 1) * WSIZE;

    if ((bp = mem_sbrk(size)) == (void *)-1)
        return NULL;
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BP(bp)), PACK(0, 1));
    
    return coalesce(bp);
}

// LIFO 방식
static void *coalesce(char* bp)
{
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /*
     * CASE 2 - 오른쪽에 가용 블록이 있는 경우
     * 1. 오른쪽의 가용 블록을 가용 리스트에서 제거
     * 2. 블록 병합 (size, bp 갱신)
     *              ┌───────────┐
     *              └───────────┘
     *                (⇅) ↑│
     * ╔═══════════╗┌───────────┐
     * ╚═══════════╝└───────────┘
     *                (⇅) │↓
     *              ┌───────────┐
     *              └───────────┘
     */
    if (prev_alloc && !next_alloc) {
        char *next_bp = NEXT_BP(bp);
        delete_block(next_bp);
        size += GET_SIZE(HDRP(next_bp));
    }
    
    /* 
     * CASE 3 - 왼쪽에 가용 블록이 있는 경우
     * 1. 왼쪽의 가용 블록을 가용 리스트에서 제거
     * 2. 블록 병합 (size, bp 갱신)
     * ┌───────────┐
     * └───────────┘
     *   (⇅) ↑│
     * ┌───────────┐╔═══════════╗
     * └───────────┘╚═══════════╝
     *   (⇅) │↓
     * ┌───────────┐
     * └───────────┘
     */
    else if (!prev_alloc && next_alloc) {
        char *prev_bp = PREV_BP(bp);
        delete_block(prev_bp);
        size += GET_SIZE(HDRP(prev_bp));
        bp = prev_bp;
    }

    /* 
     * CASE 4 - 양쪽에 가용 블록이 있는 경우
     * 1. 양쪽의 가용 블록을 각각 가용 리스트에서 제거
     * 2. 블록 병합 (size, bp 갱신)
     * ┌───────────┐             ┌───────────┐
     * └───────────┘             └───────────┘
     *   (⇅) ↑│                    (⇅) ↑│
     * ┌───────────┐╔═══════════╗┌───────────┐
     * └───────────┘╚═══════════╝└───────────┘
     *   (⇅) │↓                    (⇅) │↓
     * ┌───────────┐             ┌───────────┐
     * └───────────┘             └───────────┘
     */
    else if (!prev_alloc && !next_alloc) {
        char *next_bp = NEXT_BP(bp);
        char *prev_bp = PREV_BP(bp);
        delete_block(next_bp);
        delete_block(prev_bp);
        size += GET_SIZE(HDRP(prev_bp)) + GET_SIZE(HDRP(next_bp));
        bp = prev_bp;
    }
    
    /* 
     * CASE 1 - 양쪽에 가용 블록이 없는 경우 (또는 위에서 병합된 블록)
     * 1. 가용 블록 리스트의 맨 위로 올라감
     * 2. 기존의 맨 위 블록(free_listp)과 포인터 연결
     * 3. free_listp를 새로 top이 된 가용블록으로 변경
     * ╔═══════════╗ ← free_listp
     * ╚═══════════╝
     *    ⇅
     * ┌───────────┐
     * └───────────┘
     *    ⇅
     * ┌───────────┐
     * └───────────┘
     *    ⇅
     * ┌───────────┐
     * └───────────┘
     */
    NEXT(bp) = free_listp;
    PREV(bp) = NULL;
    PREV(free_listp) = bp;
    free_listp = bp;

    // 병합한 가용 블록 사이즈 재설정
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    return bp;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    size_t asize = (size <= DSIZE)? 2 * DSIZE : DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    size_t extendsize = MAX(asize, CHUNKSIZE);
    char *bp;

    // if ((bp = best_fit(asize)) != NULL) {
    if ((bp = first_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    
    if ((bp = extend_heap(extendsize / WSIZE)) != NULL) {
        place(bp, asize);
        return bp;
    }
    
    return NULL;
}

// static void *best_fit(size_t asize)
// {
//     char *min_bp = NULL;
//     size_t min_size = ULONG_MAX;
//     for (char *bp = free_listp; !GET_ALLOC(HDRP(bp)); bp = NEXT(bp)) {
//         if (asize <= GET_SIZE(HDRP(bp)) && GET_SIZE(HDRP(bp)) < min_size) {
//             min_size = GET_SIZE(HDRP(bp));
//             min_bp = bp;
//         }
//     }
//     return min_bp;
// }

static void *first_fit(size_t asize)
{
    for (char *bp = free_listp; !GET_ALLOC(HDRP(bp)); bp = NEXT(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL;
}

static void place(char *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    delete_block(bp);
    if ((csize - asize) >= 4*WSIZE) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
        coalesce(bp);
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

static void delete_block(char *bp)
{
    if (PREV(bp)){  // 이전 블록이 있는 경우
        NEXT(PREV(bp)) = NEXT(bp);}
    else{           // 이전 블록이 없으면 free_listp를 업데이트
        free_listp = NEXT(bp);}
    if (NEXT(bp)){  // 다음 블록이 있는 경우
        PREV(NEXT(bp)) = PREV(bp);}
    NEXT(bp) = NULL;
    PREV(bp) = NULL;
}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size)
{
    void *old_bp = bp;
    void *new_bp = mm_malloc(size);
    size_t copy_size = GET_SIZE(HDRP(old_bp)) - DSIZE;
    
    if (new_bp == NULL)
        return NULL;

    if (size < copy_size)
        copy_size = size;
    
    memcpy(new_bp, old_bp, copy_size);
    mm_free(old_bp);
    return new_bp;
}

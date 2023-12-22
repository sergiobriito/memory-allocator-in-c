#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define CHUNK_SIZE 4096

void *initialChunk = NULL;

typedef struct Block {
    size_t size;
    int is_allocated;
    struct Block *nextBlock;
} Block;

void initialize_memory_chunk() {
    initialChunk = sbrk(CHUNK_SIZE);
    Block *firstBlock = (Block *)initialChunk;
    firstBlock->size = CHUNK_SIZE - sizeof(Block);
    firstBlock->is_allocated = 0;
    firstBlock->nextBlock = NULL;
}

void *alloc(size_t size) {
    Block *currentBlock = (Block *)initialChunk;

    while (currentBlock != NULL) {
        if (!currentBlock->is_allocated && currentBlock->size >= size) {
            if (currentBlock->size > size + sizeof(Block)) {
                Block *newBlock = (Block *)((char *)currentBlock + sizeof(Block) + size);
                newBlock->size = currentBlock->size - size - sizeof(Block);
                newBlock->is_allocated = 0;
                newBlock->nextBlock = currentBlock->nextBlock;

                currentBlock->size = size;
                currentBlock->is_allocated = 1;
                currentBlock->nextBlock = newBlock;
            } else {
                currentBlock->is_allocated = 1;
            }
            return (void *)(currentBlock + 1);
        }
        
        currentBlock = currentBlock->nextBlock; 
    }
    
    return NULL;
}

void mergeFreeBlocks() {
    Block *currentBlock = (Block *)initialChunk;
    while (currentBlock != NULL && currentBlock->nextBlock != NULL) {
        if (!currentBlock->is_allocated && !(currentBlock->nextBlock->is_allocated)) {
            currentBlock->size += currentBlock->nextBlock->size + sizeof(Block);
            currentBlock->nextBlock = currentBlock->nextBlock->nextBlock;
        } else {
            currentBlock = currentBlock->nextBlock;
        }
    }
}

void freeAlloc(void *ptr, int merge) {
    if (ptr == NULL) {
        return;
    }

    Block *block = ((Block *)ptr) - 1;

    if (!block->is_allocated) {
        return;
    }

    block->is_allocated = 0;
    
    if (merge == 1){
        mergeFreeBlocks();
    };
}

void printAlloc(void *ptr){
    if (ptr == NULL) {
        return;
    }
    Block *block = (((Block *)ptr) - 1);
    printf("Memory block -> size: %ld | is_allocated: %d\n", block->size, block->is_allocated); 
};

int main() {
    initialize_memory_chunk();

    printf("-> Alloc\n");
    void *ptr = alloc(15);
    void *ptr2 = alloc(25);
    
    printAlloc(ptr);
    printAlloc(ptr2);

    printf("-> Free Alloc\n");
    freeAlloc(ptr,0);
    freeAlloc(ptr2,0);

    printAlloc(ptr);
    printAlloc(ptr2);

    printf("------With MergeFreeBlocks function------\n");

    printf("-> Alloc\n");
    ptr = alloc(15);
    ptr2 = alloc(25);
    
    printAlloc(ptr);
    printAlloc(ptr2);

    printf("-> Free Alloc\n");
    freeAlloc(ptr,1);
    freeAlloc(ptr2,1);

    printAlloc(ptr);
    printAlloc(ptr2);

    return 0;
}

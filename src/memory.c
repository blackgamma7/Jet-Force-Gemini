#include "common.h"

/* Size: 0x14 bytes */
typedef struct MemoryPoolSlot {
/* 0x00 */ u8 *data;
/* 0x04 */ s32 size;
/* 0x08 */ s16 flags;
    // 0x00 = Slot is free
    // 0x01 = Slot is being used?
    // 0x02 = ???
    // 0x04 = ???
/* 0x0A */ s16 prevIndex;
/* 0x0C */ s16 nextIndex;
/* 0x0E */ s16 index;
/* 0x10 */ u32 colourTag;
} MemoryPoolSlot;

/* Size: 0x10 bytes */
typedef struct MemoryPool {
/* 0x00 */ s32 maxNumSlots;
/* 0x04 */ s32 curNumSlots;
/* 0x08 */ MemoryPoolSlot *slots;
/* 0x0C */ s32 size;
} MemoryPool;

MemoryPoolSlot *func_8004A7C4(MemoryPoolSlot *, s32, s32); // new_memory_pool
s32 func_8004B288_4BE88(s32 poolIndex, s32 slotIndex, s32 size, s32 slotIsTaken, s32 newSlotIsTaken, u32 colourTag);
void mmSetDelay(s32 arg0);
s32 *disableInterrupts(void);
void enableInterrupts(s32*);
void *mmAlloc(s32 size, u32 colourTag);
extern MemoryPool D_800FE310_FEF10[4]; //gMemoryPools
extern s32 D_800FE858_FF458; //gFreeQueueCount
extern s32 D_800FE350_FEF50; //gNumberOfMemoryPools
extern s32 D_800FE878_FF478; //mmEndRam
extern u8 D_800A3E50_A4A50; //mmExtendedRam = FALSE;
extern MemoryPoolSlot *D_80106470; //gMainMemoryPool
extern s32 FreeRAM;
extern s32 D_800FE868_FF468[4];
extern s32 mmDelay;

#define MAIN_POOL_SLOT_COUNT 1600
#define RAM_END 0x80400000
#define EXTENDED_RAM_END 0x80600000

#ifndef _ALIGN16
#define _ALIGN16(a) (((u32) (a) & ~0xF) + 0x10)
//#define _ALIGN16(val) ((val)&0xFFFFFFF0) + 0x10
#endif


void mmInit(void) {
    D_800FE350_FEF50 = -1;
    if (D_800A3E50_A4A50) {
        D_800FE878_FF478 = EXTENDED_RAM_END;
    } else {
        D_800FE878_FF478 = RAM_END;
    }
    func_8004A7C4((MemoryPoolSlot *)&D_80106470, D_800FE878_FF478 - (s32)&D_80106470, MAIN_POOL_SLOT_COUNT);
    mmSetDelay(2);
    D_800FE858_FF458 = 0;
}

u8 mmExtended(void) {
	return D_800A3E50_A4A50;
}

/**
 * Creates a new memory pool that is sectioned off the main one.
 */
MemoryPoolSlot *mmAllocRegion(s32 poolDataSize, s32 numSlots) {
    s32 size;
    MemoryPoolSlot *slots;
    UNUSED s32 unused_2;
    s32 *flags = disableInterrupts();
    MemoryPoolSlot *newPool;

    size = poolDataSize + (numSlots * sizeof(MemoryPoolSlot));
    slots = (MemoryPoolSlot *) mmAlloc(size, COLOUR_TAG_WHITE);
    newPool = func_8004A7C4(slots, size, numSlots);
    enableInterrupts(flags);
    return newPool;
}

/**
 * Create and initialise a memory pool in RAM that will act as the place where arbitrary allocations can go.
 * Will return the location of the first free slot in that pool.
*/
MemoryPoolSlot *func_8004A7C4(MemoryPoolSlot *slots, s32 poolSize, s32 numSlots) {
    MemoryPoolSlot *firstSlot;
    s32 poolCount;
    s32 i;
	s32 firstSlotSize;
    
    poolCount = ++D_800FE350_FEF50;
	firstSlotSize = poolSize - (numSlots * sizeof(MemoryPoolSlot));
    D_800FE310_FEF10[poolCount].maxNumSlots = numSlots;
    D_800FE310_FEF10[poolCount].curNumSlots = 0;
    D_800FE310_FEF10[poolCount].slots = slots;
    D_800FE310_FEF10[poolCount].size = poolSize;
    firstSlot = slots;
    for (i = 0; i < D_800FE310_FEF10[poolCount].maxNumSlots; i++) {
        firstSlot->index = i;
        firstSlot++;
    }
    firstSlot = &D_800FE310_FEF10[poolCount].slots[0];
    slots += numSlots;
    if ((s32) slots & 0xF) {
        firstSlot->data = (u8 *) _ALIGN16(slots);
    } else {
        firstSlot->data = (u8 *) slots;
    }
    firstSlot->size = firstSlotSize;
    firstSlot->flags = 0;
    firstSlot->prevIndex = -1;
    firstSlot->nextIndex = -1;
    D_800FE310_FEF10[poolCount].curNumSlots++;
	if (poolCount == 0) {
		FreeRAM = firstSlotSize;
	}
	D_800FE868_FF468[poolCount] = firstSlotSize;
    return D_800FE310_FEF10[poolCount].slots;
}

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlloc.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlloc2.s")

//allocate_from_memory_pool
MemoryPoolSlot *func_8004A9BC_4B5BC(s32 poolIndex, s32 size, u32 colourTag) {
    s32 slotSize;
    MemoryPoolSlot *curSlot;
    UNUSED s32 pad;
    MemoryPool *pool;
    MemoryPoolSlot *slots;
    s32 *flags;
    s32 nextIndex;
    s32 currIndex;

    flags = disableInterrupts();
    pool = &D_800FE310_FEF10[poolIndex];
    if ((pool->curNumSlots + 1) == (*pool).maxNumSlots) {
        enableInterrupts(flags);
        return 0;
    }
    currIndex = -1;
    if (size & 0xF) {
        size = (size & ~0xF);
		size += 0x10;
    }
    slots = pool->slots;
    slotSize = 0x7FFFFFFF;
    nextIndex = 0;
    do {
        curSlot = &slots[nextIndex];
        if (curSlot->flags == 0) {
            if ((curSlot->size >= size) && (curSlot->size < slotSize)) {
                slotSize = curSlot->size;
                currIndex = nextIndex;
            }
        }
        nextIndex = curSlot->nextIndex;
    } while (nextIndex != -1);
    if (currIndex != -1) {
        func_8004B288_4BE88(poolIndex, (s32) currIndex, size, 1, 0, colourTag);
        enableInterrupts(flags);
        return (MemoryPoolSlot *) (slots + currIndex)->data;
    }
    enableInterrupts(flags);
    return 0;
}

void *mmAllocR(MemoryPoolSlot *slots, s32 size) {
    s32 i;
    for (i = D_800FE350_FEF50; i != 0; i--) {
        if (slots == D_800FE310_FEF10[i].slots) {
            return func_8004A9BC_4B5BC(i, size, COLOUR_TAG_NONE);
        }
    }
    return (void *)NULL;
}

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAllocAtAddr.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmSetDelay.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmFree.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmFreeTick.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004AFC0_4BBC0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B05C_4BC5C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B098_4BC98.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B0F8_4BCF8.s")

UNUSED MemoryPoolSlot *mmGetSlotPtr(s32 poolIndex) {
    return D_800FE310_FEF10[poolIndex].slots;
}

s32 mmGetDelay(void) {
	return mmDelay;
}

//allocate_memory_pool_slot
s32 func_8004B288_4BE88(s32 poolIndex, s32 slotIndex, s32 size, s32 slotIsTaken, s32 newSlotIsTaken, u32 colourTag) {
    MemoryPool *pool;
    MemoryPoolSlot *poolSlots;
    s32 index;
    s32 nextIndex;
    s32 poolSize;

    if (slotIsTaken == TRUE) {
        if (poolIndex == 0) {
            FreeRAM -= size;
        }
        D_800FE868_FF468[poolIndex] -= size;
    }

    pool = &D_800FE310_FEF10[poolIndex];
    poolSlots = pool->slots;
    pool = pool; // Fakematch
    poolSlots[slotIndex].flags = slotIsTaken;
    poolSize = poolSlots[slotIndex].size;
    poolSlots[slotIndex].size = size;
    poolSlots[slotIndex].colourTag = colourTag;
    index = poolSlots[pool->curNumSlots].index;
    if (size < poolSize) {
        index = (pool->curNumSlots + poolSlots)->index;
        pool->curNumSlots++;
        poolSlots[index].data = &poolSlots[slotIndex].data[size];
        poolSlots[index].size = poolSize;
        poolSlots[index].size -= size;
        poolSlots[index].flags = newSlotIsTaken;
        poolSize = poolSlots[slotIndex].nextIndex;
        nextIndex = poolSize;
        poolSlots[index].prevIndex = slotIndex;
        poolSlots[index].nextIndex = nextIndex;
        poolSlots[slotIndex].nextIndex = index;
        if (nextIndex != -1) {
            poolSlots[nextIndex].prevIndex = index;
        }
        return index;
    }
    return slotIndex;
}

/**
 * Returns the passed in address aligned to the next 16-byte boundary.
 */
u8 *mmAlign16(u8 *address) {
    s32 remainder = (s32)address & 0xF;
    if (remainder > 0) {
        address = (u8 *)(((s32)address - remainder) + 16);
    }
    return address;
}

/**
 * Returns the passed in address aligned to the next 4-byte boundary.
 */
u8 *mmAlign4(u8 *address) {
    s32 remainder = (s32)address & 0x3;
    if (remainder > 0) {
        address = (u8 *)(((s32)address - remainder) + 4);
    }
    return address;
}

void mmSlotPrint(void) {
    s32 i;
    s32 skip;
    s32 index;
    s32 index2;
    s32 nextIndex;
    MemoryPoolSlot *slot;

    for (i = 0; (D_800FE350_FEF50 ^ 0) >= i; i++) {
        if (i && i) {} // Fakematch
        slot = &D_800FE310_FEF10[i].slots[0];
        index = 1;
        index2 = -index;
        do {
            index = slot->flags;
            nextIndex = slot->nextIndex;
            skip = nextIndex == index2;
            if (index) {
                if (((((((((!slot->nextIndex) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu) & 0xFFu) {
                    if (nextIndex && nextIndex) {} // Fakematch
                }
            }
            if (skip) {
                continue;
            }
            slot = &D_800FE310_FEF10[i].slots[slot->nextIndex];
        } while (nextIndex != (-1));
    }
}

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

void *func_8004A7C4(void *, s32, s32); // new_memory_pool
void mmSetDelay(s32 arg0);
s32 *disableInterrupts(void);
void enableInterrupts(s32*);
void *mmAlloc(s32 size, u32 colourTag);
extern MemoryPool D_800FE310[4]; //gMemoryPools
extern s32 D_800FE858; //gFreeQueueCount
extern s32 D_800FE350; //gNumberOfMemoryPools
extern s32 D_800FE878; //mmEndRam
extern u8 D_800A3E50; //mmExtendedRam = FALSE;
extern MemoryPoolSlot *D_80106470; //gMainMemoryPool

#define MAIN_POOL_SLOT_COUNT 1600
#define RAM_END 0x80400000
#define EXTENDED_RAM_END 0x80600000

void mmInit(void) {
    D_800FE350 = -1;
    if (D_800A3E50) {
        D_800FE878 = EXTENDED_RAM_END;
    } else {
        D_800FE878 = RAM_END;
    }
    func_8004A7C4(&D_80106470, D_800FE878 - (s32)&D_80106470, MAIN_POOL_SLOT_COUNT);
    mmSetDelay(2);
    D_800FE858 = 0;
}

u8 mmExtended(void) {
	return D_800A3E50;
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

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004A7C4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlloc.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlloc2.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004A9BC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAllocR.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAllocAtAddr.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmSetDelay.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmFree.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmFreeTick.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004AFC0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B05C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B098.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B0F8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmGetSlotPtr.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmGetDelay.s")

//allocate_memory_pool_slot?
#pragma GLOBAL_ASM("asm/nonmatchings/memory/func_8004B288.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlign16.s")

#pragma GLOBAL_ASM("asm/nonmatchings/memory/mmAlign4.s")

void mmSlotPrint(void) {
    s32 i;
    s32 skip;
    s32 index;
    s32 index2;
    s32 nextIndex;
    MemoryPoolSlot *slot;

    for (i = 0; (D_800FE350 ^ 0) >= i; i++) {
        if (i && i) {} // Fakematch
        slot = &D_800FE310[i].slots[0];
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
            slot = &D_800FE310[i].slots[slot->nextIndex];
        } while (nextIndex != (-1));
    }
}

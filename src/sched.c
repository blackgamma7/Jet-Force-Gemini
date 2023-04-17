#include "common.h"
#include "viint.h"

/*
 * private typedefs and defines
 */
#define UNK_MSG         99
#define VIDEO_MSG       666
#define RSP_DONE_MSG    667
#define RDP_DONE_MSG    668
#define PRE_NMI_MSG     669

static void __scMain(void *arg);
s32 __scSchedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP);
void func_80050670_51270(OSSched *sc);

extern OSViMode D_800AA460_AB060; //PAL
extern OSViMode D_800AA4B0_AB0B0; //MPAL
extern OSViMode D_800AA500_AB100; //NTSC
extern s32 D_800A4310_A4F10; //gCurRSPTaskCounter
extern s32 D_800A4314_A4F14; //gCurRDPTaskCounter
extern s32 D_800A4300_A4F00[];
// s32 D_800A4300_A4F00[] = { OSMESG_SWAP_BUFFER, OSMESG_SWAP_BUFFER };
OSTime D_800FF668_100268; //gYieldTime

void osCreateScheduler(OSSched *sc, void *stack, OSPri priority, u8 mode, u8 numFields) {
    sc->curRSPTask      = 0;
    sc->curRDPTask      = 0;
    sc->clientList      = 0;
    sc->audioListHead   = 0;
    sc->gfxListHead     = 0;
    sc->audioListTail   = 0;
    sc->gfxListTail     = 0;
    sc->frameCount      = 0;
    sc->unkTask         = 0;
    sc->retraceMsg.type = OS_SC_RETRACE_MSG;  /* sent to apps */
    sc->prenmiMsg.type  = OS_SC_PRE_NMI_MSG;
    
    /*
     * Set up video manager, listen for Video, RSP, and RDP interrupts
     */
    osCreateViManager(OS_PRIORITY_VIMGR);    
    switch (mode) {
        case OS_VI_PAL_LPN1:
            osViSetMode(&D_800AA460_AB060);
            break;
        case OS_VI_MPAL_LPN1:
            osViSetMode(&D_800AA4B0_AB0B0);
            break;
        default:
            osViSetMode(&D_800AA500_AB100);
            break;
    }
    osViBlack(TRUE);
    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, OS_SC_MAX_MESGS);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, OS_SC_MAX_MESGS);
    osSetEventMesg(OS_EVENT_SP, &sc->interruptQ, (OSMesg)RSP_DONE_MSG);
    osSetEventMesg(OS_EVENT_DP, &sc->interruptQ, (OSMesg)RDP_DONE_MSG);
    osSetEventMesg(OS_EVENT_PRENMI, &sc->interruptQ, (OSMesg)PRE_NMI_MSG);

    osViSetEvent(&sc->interruptQ, (OSMesg)VIDEO_MSG, numFields);

    osCreateThread(&sc->thread, 5, __scMain, (void *)sc, stack, priority);
    osStartThread(&sc->thread);
}

void osScAddClient(OSSched *sc, OSScClient *c, OSMesgQueue *msgQ, u8 id){
    OSIntMask mask;

    mask = osSetIntMask(OS_IM_NONE);

    c->msgQ = msgQ;
    c->next = sc->clientList;
    c->id = id;
    sc->clientList = c;
    
    osSetIntMask(mask);
}

void osScRemoveClient(OSSched *sc, OSScClient *c) {
    OSScClient *client = sc->clientList; 
    OSScClient *prev   = 0;
    OSIntMask  mask;

    mask = osSetIntMask(OS_IM_NONE);
    
    while (client != 0) {
        if (client == c) {
            if(prev)
                prev->next = c->next;
            else
                sc->clientList = c->next;
            break;
        }
        prev   = client;
        client = client->next;
    }

    osSetIntMask(mask);
}

OSMesgQueue *osScGetCmdQ(OSSched *sc) {
    return &sc->cmdQ;
}

OSMesgQueue *osScGetInterruptQ(OSSched *sc) {
    return &sc->interruptQ;
}

UNUSED void osScGetAudioSPStats(f32 *arg0, f32 *arg1, f32 *arg2) {
    *arg0 = 0.0f;
    *arg1 = 0.0f;
    *arg2 = 0.0f;
}

static void __scMain(void *arg) {
    OSMesg msg = NULL;
    OSSched *sc = (OSSched *)arg;
    OSScClient *client;
    s32 state = 0;
    OSScTask *sp = 0;
    OSScTask *dp = 0;

    while (1) {

        osRecvMesg(&sc->interruptQ, (OSMesg *)&msg, OS_MESG_BLOCK);

        switch ((int) msg) {
            case (VIDEO_MSG):
                __scHandleRetrace(sc);
                break;

            case (RSP_DONE_MSG):
                __scHandleRSP(sc);
                break;

            case (RDP_DONE_MSG):
                __scHandleRDP(sc);
                break;

            case (UNK_MSG):
                func_80050670_51270(sc);
                break;

            case (PRE_NMI_MSG):
                /*
                * notify audio and graphics threads to fade out
                */
                for (client = sc->clientList; client != 0; client = client->next) {
                    osSendMesg(client->msgQ, (OSMesg) &sc->prenmiMsg,
                                OS_MESG_NOBLOCK);
                };
                break;

            default:
                __scAppendList(sc, (OSScTask *) msg);
                state = ((sc->curRSPTask == 0) << 1) | (sc->curRDPTask == 0);
                if (__scSchedule(sc, &sp, &dp, state) != state)
                    __scExec(sc, sp, dp);
                break;
        }
    }
}

void func_80050670_51270(OSSched *sc) {
    s32 state;
    OSScTask *sp = 0;
    OSScTask *dp = 0;

    if (sc->audioListHead) {
        sc->doAudio = 1;
    }
    if (sc->doAudio && sc->curRSPTask) {
        __scYield(sc);
    } else {
        state = ((sc->curRSPTask == 0) << 1) | (sc->curRDPTask == 0);
        if (__scSchedule(sc, &sp, &dp, state) != state) {
            __scExec(sc, sp, dp);
        }
    }
}

#ifdef NON_MATCHING
//Needs RODATA migrated for the jump table to match
char *func_80050718_51318(s32 arg0) {
    switch (arg0) {
    case 1:
        return &D_800AD510_AE110;
    case 2:
        return &D_800AD520_AE120;
    case 3:
        return &D_800AD52C_AE12C;
    case 4:
        return &D_800AD538_AE138;
    case 5:
        return &D_800AD550_AE150;
    case 6:
        return &D_800AD560_AE160;
    case 7:
        return &D_800AD570_AE170;
    default:
        return &D_800AD580_AE180;
    }
}
#else
#pragma GLOBAL_ASM("asm/nonmatchings/sched/func_80050718_51318.s")
#endif

void func_800507A4_513A4(OSScTask *task) {}

#pragma GLOBAL_ASM("asm/nonmatchings/sched/func_800507AC_513AC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/sched/func_80050AA4_516A4.s")

#ifdef NON_EQUIVALENT
void __scHandleRetrace(OSSched *sc) {
    OSScTask *rspTask = NULL;
    OSScClient *client;
    s32 state;
    OSScTask *sp = 0;
    OSScTask *dp = 0;
    u8 set_curRSPTask_NULL = FALSE;
    u8 set_curRDPTask_NULL = FALSE;
    OSScTask *unkTask;
    Gfx *spGfx;
    Gfx *dpGfx;
    s32 spB4;
    s32 spA4;
    s32 spC4;
    s32 spB0;
    s32 spA0;
    s32 spC0;
    OSMesgQueue sp5C;
    s32 *sp74;
    OSScTask *curTask;
    Gfx *dlist;
    s32 yPos;
    s32 var_a1;
    s32 spAC;
    s32 sp9C;
    s32 spBC;
    s32 spA8;
    s32 sp98;
    s32 spB8;

    if (sc->curRSPTask) {
        D_800A4310_A4F10++;
    }

    if (sc->curRDPTask) {
        D_800A4314_A4F14++;
    }
    spGfx = 0;
    dpGfx = 0;
    if ((D_800A4310_A4F10 > 10) && (sc->curRSPTask)) {
        if (D_800FF660_100260 != 0) {
            func_80050718_51318(sc->curRSPTask->unk6C);
            func_800507A4_513A4(sc->curRSPTask);
            if (sc->curRSPTask->list.t.type == M_GFXTASK) {
                spGfx = (Gfx *) func_80050AA4_516A4(sc, &spB4, &spA4, &spC4, &spB0, &spA0, &spC0);
            }
            D_800FF660_100260 = 0;
        }
        D_800A4310_A4F10 = 0;
        set_curRSPTask_NULL = TRUE;
        __osSpSetStatus(SP_SET_HALT | SP_CLR_INTR_BREAK | SP_CLR_SIG0 |
            SP_CLR_SIG1 | SP_CLR_SIG2 | SP_CLR_SIG3 | SP_CLR_SIG4 |
            SP_CLR_SIG5 | SP_CLR_SIG6 | SP_CLR_SIG7);
    } else if (sc->curRSPTask) {
        D_800FF660_100260 = TRUE;
    }

    if ((D_800A4314_A4F14 > 10) && (sc->curRDPTask)) {
        if (sc->curRDPTask->unk68 == 0) {
            osSendMesg(sc->curRDPTask->msgQ, &D_800A4308_A4F08, OS_MESG_BLOCK);
        }
        if (D_800FF664_100264 != 0) {
            func_80050718_51318(sc->curRDPTask->unk6C);
            func_800507A4_513A4(sc->curRDPTask);
            if (sc->curRDPTask->list.t.type == M_GFXTASK) {
                dpGfx = (Gfx *) func_80050AA4_516A4(sc, &spAC, &sp9C, &spBC, &spA8, &sp98, &spB8);
            }
            D_800FF664_100264 = 0;
        }

        set_curRDPTask_NULL = TRUE;
        sc->frameCount = 0;
        D_800A4314_A4F14 = 0;

        __osSpSetStatus(SP_SET_HALT | SP_CLR_INTR_BREAK | SP_CLR_SIG0 |
            SP_CLR_SIG1 | SP_CLR_SIG2 | SP_CLR_SIG3 | SP_CLR_SIG4 |
            SP_CLR_SIG5 | SP_CLR_SIG6 | SP_CLR_SIG7);

        osDpSetStatus(DPC_SET_XBUS_DMEM_DMA | DPC_CLR_FREEZE | DPC_CLR_FLUSH |
            DPC_CLR_TMEM_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_CMD_CTR);
    } else if (sc->curRDPTask) {
        D_800FF664_100264 = TRUE;
    }

    if ((spGfx != 0) || (dpGfx != 0)) {
        osCreateMesgQueue(&sp5C, &sp74, 8);
        osSetEventMesg(OS_EVENT_SP, &sp5C, (OSMesg)RSP_DONE_MSG);
        osSetEventMesg(OS_EVENT_DP, &sp5C, (OSMesg)RDP_DONE_MSG);
        osViSetEvent(&sp5C, (OSMesg)VIDEO_MSG, 1U);
        if (sc->curRSPTask != NULL) {
            curTask = sc->curRSPTask;
        } else {
            curTask = sc->curRDPTask;
        }
        dlist = (Gfx *) curTask->list.t.data_ptr;
        segSetBase(&dlist, 0, 0);
        segSetBase(&dlist, 1, otherScreen);
        segSetBase(&dlist, 2, otherZbuf);
        segSetBase(&dlist, 4, currentScreen - 0x500);
        diPrintfSetBG(0, 0, 0, 128);
        if ((spGfx != 0) || (dpGfx != 0)) {
            mmSlotPrint();
        }
        if (spGfx != 0) {
            diPrintfSetXY(30, 30);
            //"SP CRASHED, gfx=%x"
            diPrintf(D_800AD7D4_AE3D4, spGfx);
        }
        if (dpGfx != 0) {
            diPrintfSetXY(30, 70);
            //"DP CRASHED, gfx=%x"
            diPrintf(D_800AD7E8_AE3E8, dpGfx);
        }
        yPos = 110;
        if (D_800A3B28_A4728 != 0) {
            diPrintfSetXY(30, yPos);
            //"** GFX overflow **"
            diPrintf(D_800AD7FC_AE3FC);
            yPos = 120;
        }
        if (D_800A3B2C_A472C != 0) {
            diPrintfSetXY(30, yPos);
            //"** MTX overflow **"
            diPrintf(D_800AD810_AE410);
            yPos += 10;
        }
        if (D_800A3B30_A4730 != 0) {
            diPrintfSetXY(30, yPos);
            //"** VTX overflow **"
            diPrintf(&D_800AD824_AE424);
            yPos += 10;
        }
        if (D_800A3B34_A4734 != 0) {
            diPrintfSetXY(30, yPos);
            //"** POL overflow **"
            diPrintf(D_800AD838_AE438);
            yPos += 10;
        }
        set_curRDPTask_NULL = 0;
        set_curRDPTask_NULL = 1;
        spGfx = 0;
        dpGfx = 0;
        diPrintfSetXY(30, yPos + 10);
        //"Version %s" - "7.2"
        diPrintf(D_800AD84C_AE44C, D_800AD7D0_AE3D0);
        diPrintfAll(&dlist);
        __osSpSetStatus(SP_SET_HALT | SP_CLR_INTR_BREAK | SP_CLR_SIG0 |
            SP_CLR_SIG1 | SP_CLR_SIG2 | SP_CLR_SIG3 | SP_CLR_SIG4 |
            SP_CLR_SIG5 | SP_CLR_SIG6 | SP_CLR_SIG7);
        osDpSetStatus(DPC_SET_XBUS_DMEM_DMA | DPC_CLR_FREEZE | DPC_CLR_FLUSH |
            DPC_CLR_TMEM_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_CMD_CTR);
        gDPFullSync(dlist++);
        gSPEndDisplayList(dlist++)
        osWritebackDCacheAll();
        osSpTaskLoad(&curTask->list);
        osSpTaskStartGo(&curTask->list);
        while (1) {}
    }

    if (set_curRSPTask_NULL) {
        sc->curRSPTask = NULL;
    }
    if (set_curRDPTask_NULL) {
        sc->curRDPTask = NULL;
    }

    while (osRecvMesg(&sc->cmdQ, (OSMesg *)&rspTask, OS_MESG_NOBLOCK) != -1)
        __scAppendList(sc, rspTask);

    state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
    if (__scSchedule(sc, &sp, &dp, state) != state)
        __scExec(sc, sp, dp);

    D_800A4320_A4F20++;
    sc->frameCount+=1; // If you want to make the game 60FPS, change this to 2.

    if ((sc->unkTask) && (sc->frameCount >= 2)) {
        unkTask = sc->unkTask;
        if (unkTask->msgQ) {
            if ((unkTask->unk68) || (unkTask->msg)) {
                osSendMesg(unkTask->msgQ, unkTask->msg, OS_MESG_BLOCK);
            } else {
                osSendMesg(unkTask->msgQ, &D_800A4300_A4F00, OS_MESG_BLOCK);
            }
        }
        sc->frameCount = 0;
        sc->unkTask = 0;
    }

    for (client = sc->clientList; client != 0; client = client->next) {
        if (client->id == OS_SC_ID_AUDIO) {
            //Only run this on even calls to this function
            D_800A4318_A4F18 -= 1;
            if (D_800A4318_A4F18 <= 0) {
                osSendMesg(client->msgQ, sc, OS_MESG_NOBLOCK);
                if (sc->audioListHead) {
                    func_80050670_51270(sc);
                }
                D_800A4318_A4F18 = amAudioMgrGetNextFrameCount();
            }
        } else if (client->id == OS_SC_ID_VIDEO) {
            osSendMesg(client->msgQ, sc, OS_MESG_NOBLOCK);
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/nonmatchings/sched/__scHandleRetrace.s")
#endif

void __scHandleRSP(OSSched *sc) {
    OSScTask *t, *sp = 0, *dp = 0;
    s32 state;

    t = sc->curRSPTask;
    sc->curRSPTask = NULL;

    if ((t->state & OS_SC_YIELD)) {
        if (osSpTaskYielded(&t->list)) {
            t->state |= OS_SC_YIELDED;
            if ((t->flags & OS_SC_TYPE_MASK) == OS_SC_XBUS) {
                /* push the task back on the list */
                t->next = sc->gfxListHead;
                sc->gfxListHead = t;
                if (sc->gfxListTail == 0)
                    sc->gfxListTail = t;
            }
        } else {
            t->state &= ~OS_SC_NEEDS_RSP;
    /* BEGIN TODO: This just feels wrong, but it matches */
            do{} while(0);
        }
        if ((t->flags & OS_SC_TYPE_MASK) != OS_SC_XBUS){}
    /* END TODO */
    } else {
        t->state &= ~OS_SC_NEEDS_RSP;
        __scTaskComplete(sc, t);
    }

    state = ((sc->curRSPTask == 0) << 1) | (sc->curRDPTask == 0);
    if ( (__scSchedule (sc, &sp, &dp, state)) != state)
        __scExec(sc, sp, dp);
}

void __scHandleRDP(OSSched *sc) {
    OSScTask *t, *sp = 0, *dp = 0; 
    s32 state;
    
    t = sc->curRDPTask;
    sc->curRDPTask = 0;

    t->state &= ~OS_SC_NEEDS_RDP;

    __scTaskComplete(sc, t);

    state = ((sc->curRSPTask == 0) << 1) | (sc->curRDPTask == 0);
    if ( (__scSchedule(sc, &sp, &dp, state)) != state)
        __scExec(sc, sp, dp);
}

OSScTask *__scTaskReady(OSScTask *t) {
    if (t) {    
        /*
         * If there is a pending swap bail out til later (next
         * retrace).
         */
        if (osViGetCurrentFramebuffer() != osViGetNextFramebuffer()) {           
            return 0;
        }

        return t;
    }

    return 0;
}

/*
 * __scTaskComplete checks to see if the task is complete (all RCP
 * operations have been performed) and sends the done message to the
 * client if it is.
 */
s32 __scTaskComplete(OSSched *sc, OSScTask *t) {
    if ((t->state & OS_SC_RCP_MASK) == 0) {
        if (t->msgQ) {
            if (t->flags & OS_SC_LAST_TASK) {
                if (sc->frameCount <= 1) {
                    sc->unkTask = t;
                    return 1;
                }
                if (t->unk68 || t->msg) {
                    osSendMesg(t->msgQ, t->msg, OS_MESG_BLOCK);
                } else {
                    osSendMesg(t->msgQ, &D_800A4300_A4F00, OS_MESG_BLOCK);
                }
                sc->frameCount = 0;
                return 1;
            }
            if (t->unk68 || t->msg) {
                osSendMesg(t->msgQ, t->msg, OS_MESG_BLOCK);
                return 1;
            }
            osSendMesg(t->msgQ, &D_800A4300_A4F00, OS_MESG_BLOCK);
        }
        return 1;
    }
    return 0;
}

void __scAppendList(OSSched *sc, OSScTask *t) {
    long type = t->list.t.type;

    if (type == M_AUDTASK) {
        if(sc->audioListTail)
            sc->audioListTail->next = t;
        else
            sc->audioListHead = t;

        sc->audioListTail = t;
    } else {
        if(sc->gfxListTail)
            sc->gfxListTail->next = t;
        else
            sc->gfxListHead = t;

        sc->gfxListTail = t;
    }

    t->next = NULL;
    t->state = t->flags & OS_SC_RCP_MASK;
}

void __scExec(OSSched *sc, OSScTask *sp, OSScTask *dp) {
    if (sp) {
        if (sp->list.t.type == M_AUDTASK) {
            osWritebackDCacheAll();  /* flush the cache */
            //gRSPAudTaskFlushTime = osGetCount();
        }
        
        sp->state &= ~(OS_SC_YIELD | OS_SC_YIELDED);
        osSpTaskLoad(&sp->list);
        osSpTaskStartGo(&sp->list); 
        D_800A4310_A4F10 = 0;
        D_800A4314_A4F14 = 0;
        sc->curRSPTask = sp;

        if (sp == dp)
            sc->curRDPTask = dp;
    }

    if (dp && (dp != sp)) {
        osDpSetNextBuffer(dp->list.t.output_buff, *dp->list.t.output_buff_size);
        sc->curRDPTask = dp;
    }
}

void __scYield(OSSched *sc) {
    if (sc->curRSPTask->list.t.type == M_GFXTASK) {
        sc->curRSPTask->state |= OS_SC_YIELD;
        D_800FF668_100268 = osGetTime();
        osSpTaskYield();
    } 
}

#ifdef NON_MATCHING
//Needs RODATA migrated for the jump table to match
#define assert(EX) ((void)0)
/*
 * Schedules the tasks to be run on the RCP
 */
s32 __scSchedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP)  {
    s32 avail = availRCP;
    OSScTask *gfx = sc->gfxListHead;
    OSScTask *audio = sc->audioListHead;

#ifdef SC_LOGGING
    osLogEvent(l, 517, 3, *sp, *dp, availRCP);
#endif

    if (sc->doAudio && (avail & OS_SC_SP)) {

        if (gfx && (gfx->flags & OS_SC_PARALLEL_TASK)) {
            *sp = gfx;
            avail &= ~OS_SC_SP;
        } else {
            *sp = audio;
            avail &= ~OS_SC_SP;
            sc->doAudio = 0;
            sc->audioListHead = sc->audioListHead->next;
            if (sc->audioListHead == NULL)
                sc->audioListTail = NULL;
        }
    } else {
#ifdef SC_LOGGING
        osLogEvent(l, 520, 1, gfx);
#endif
        if (__scTaskReady(gfx)) {

#ifdef SC_LOGGING
            osLogEvent(l, 522, 3, gfx, gfx->state, gfx->flags);
#endif
            switch (gfx->flags & OS_SC_TYPE_MASK) {
              case (OS_SC_XBUS):
                  if (gfx->state & OS_SC_YIELDED) {
#ifdef SC_LOGGING
                      osLogEvent(l, 518, 0);
#endif
		      /* can hit this if RDP finishes at yield req */
                      /* assert(gfx->state & OS_SC_DP); */

                      if (avail & OS_SC_SP) {   /* if SP is available */
#ifdef SC_LOGGING
                      osLogEvent(l, 519, 0);
#endif
                          *sp = gfx;
                          avail &= ~OS_SC_SP;

                          if (gfx->state & OS_SC_DP) {  /* if it needs DP */
                              *dp = gfx;
                              avail &= ~OS_SC_DP;

                              if ((avail & OS_SC_DP) == 0)
                                  assert(sc->curRDPTask == gfx);

                          }

                          sc->gfxListHead = sc->gfxListHead->next;
                          if (sc->gfxListHead == NULL)
                              sc->gfxListTail = NULL;

                      }
                  } else {
                      if (avail == (OS_SC_SP | OS_SC_DP)) {
                          *sp = *dp = gfx;
                          avail &= ~(OS_SC_SP | OS_SC_DP);
                          sc->gfxListHead = sc->gfxListHead->next;
                          if (sc->gfxListHead == NULL)
                              sc->gfxListTail = NULL;
                      }
                  }

                  break;

              case (OS_SC_DRAM):
              case (OS_SC_DP_DRAM):
              case (OS_SC_DP_XBUS):
                  if (gfx->state & OS_SC_SP) {  /* if needs SP */
                      if (avail & OS_SC_SP) {   /* if SP is available */
                          *sp = gfx;
                          avail &= ~OS_SC_SP;
                      }
                  }
                  //This is else if in libreultra, and it's the only difference
                  if (gfx->state & OS_SC_DP) {   /* if needs DP */
                      if (avail & OS_SC_DP) {        /* if DP available */
                          *dp = gfx;
                          avail &= ~OS_SC_DP;
                          sc->gfxListHead = sc->gfxListHead->next;
                          if (sc->gfxListHead == NULL)
                              sc->gfxListTail = NULL;
                      }
                  }
                  break;

              case (OS_SC_SP_DRAM):
              case (OS_SC_SP_XBUS):
              default:
                  break;
            }
        }
    }

    if (avail != availRCP)
        avail = __scSchedule(sc, sp, dp, avail);

    return avail;
}
#else
#pragma GLOBAL_ASM("asm/nonmatchings/sched/__scSchedule.s")
#endif

#include "common.h"

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxInitLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxAddLevelEffect.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxClearLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxLevelFreeLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxLevelAllocateLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_80068CD4_698D4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_80068CDC_698DC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_80068D34_69934.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxUpdateLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawLevelEffects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxFreeCone.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxAllocateCone.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_80069320_69F20.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_80069510_6A110.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxMakeConeTextureCoords.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxMakeConeLength.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawCone.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006A224_6AE24.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeAllocate.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeSetupRipple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeFree.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeFreeRipple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeUpdate.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeUpdateRipple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeDraw.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/wakeDrawRipple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxInit.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/disableClearScreen.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/enableClearScreen.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxFadeOn.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/setupClearScreen.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/updateClearScreen.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/drawClearScreen.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006C1B8_6CDB8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006C1CC_6CDCC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006C280_6CE80.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006C454_6D054.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006C560_6D160.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006CAAC_6D6AC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006CD60_6D960.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006CE58_6DA58.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006CF50_6DB50.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006D048_6DC48.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006D1A8_6DDA8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006D2F0_6DEF0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxInitLines.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawLine.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawLineInWindow.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxSetClipWindow.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxTinyPrint.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxInttostr.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/ClipPoint.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawLine3D.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/PlotAddRGB.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/PlotAddRG.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006E1D8_6EDD8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006E38C_6EF8C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006E5FC_6F1FC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawDigitalNumber.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxOutputLines.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxInitNightVision.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxFreeNightVision.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxNightVision.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxDrawNightVision.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxNightVisionSkip.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxSPDPRipple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxQueueScreenEffect.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxUnQueueScreenEffect.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxScreenEffect.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/func_8006FFF8_70BF8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxGenerateTextures.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxCpuTextureRequired.s")

#pragma GLOBAL_ASM("asm/nonmatchings/fx/fxCpuTextureFlush.s")

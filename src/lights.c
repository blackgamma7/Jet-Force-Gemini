#include "common.h"

#pragma GLOBAL_ASM("asm/nonmatchings/lights/freeLights.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/setupLights.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/func_80020D94_21994.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/addRomdefLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/addObjectLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/turnLightOff.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/turnLightOn.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/toggleLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/changeLightColour.s")

void changeLightColourCycle(s32 arg0, s32 arg1) {
    initColourCycle(arg0 + 0x48, arg1);
}

#pragma GLOBAL_ASM("asm/nonmatchings/lights/changeLightIntensity.s")

void lightUpdateLights(s32 arg0) {
    s32 i;
    for (i = 0; i < D_800A1894_A2494; i++) {
        func_80021444_22044(D_800A1898_A2498[i], arg0);
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/lights/func_80021444_22044.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/killLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightGetLights.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightGetStrongestEffect.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightUpdateObjects.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/func_80021B9C_2279C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightDistanceCalc.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightDirectionCalc.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightObject.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightDefaultObjectLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightSetObjectLight.s")

void lightSetupLightSources(Object *obj) { 
    s32 i;
    for(i = 0; i < obj->segment.header->unk25; i++) { 
        obj->unk74[i] = addObjectLight(obj, &obj->segment.header->unk48[i]);
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightSetupFlareSources.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightInitObjectLighting.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightAdjustGlowingLight.s")

#pragma GLOBAL_ASM("asm/nonmatchings/lights/lightKillGlowingLight.s")

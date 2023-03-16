#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "structs.h"

//objects.cs
extern s32 ObjListCount;
extern Object **ObjList; // Not sure about the number of elements
extern VertexPosition gEnvmapPos;
extern s32 *Ftables;
extern s32 *Findex;
extern s32 Fmax;

//controllers.cs
extern OSMesgQueue joyMessageQueue;
extern OSMesg joyMessageBuf;
extern OSMesg joyMessage;
extern OSContStatus joyStatus;
extern s32 connected;
extern s32 numberOfJoypads;

#endif
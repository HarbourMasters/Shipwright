#pragma once
#define dgGerudoCellDoorTex "__OTR__objects/object_door_gerudo/gGerudoCellDoorTex"
#ifdef _WIN32
static const __declspec(align(2)) char gGerudoCellDoorTex[] = dgGerudoCellDoorTex;
#else
static const char gGerudoCellDoorTex[] __attribute__((aligned (2))) = dgGerudoCellDoorTex;
#endif
            
#define dgGerudoCellDoorDL "__OTR__objects/object_door_gerudo/gGerudoCellDoorDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGerudoCellDoorDL[] = dgGerudoCellDoorDL;
#else
static const char gGerudoCellDoorDL[] __attribute__((aligned (2))) = dgGerudoCellDoorDL;
#endif
            
#define dgGerudoCellDoorCol "__OTR__objects/object_door_gerudo/gGerudoCellDoorCol"
#ifdef _WIN32
static const __declspec(align(2)) char gGerudoCellDoorCol[] = dgGerudoCellDoorCol;
#else
static const char gGerudoCellDoorCol[] __attribute__((aligned (2))) = dgGerudoCellDoorCol;
#endif
            


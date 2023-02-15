#pragma once
#define dgSpiritDoorDL "__OTR__objects/object_jya_door/gSpiritDoorDL"
#ifdef _WIN32
static const __declspec(align(2)) char gSpiritDoorDL[] = dgSpiritDoorDL;
#else
static const char gSpiritDoorDL[] __attribute__((aligned (2))) = dgSpiritDoorDL;
#endif
            
#define dgJyaDoorMetalBarsDL "__OTR__objects/object_jya_door/gJyaDoorMetalBarsDL"
#ifdef _WIN32
static const __declspec(align(2)) char gJyaDoorMetalBarsDL[] = dgJyaDoorMetalBarsDL;
#else
static const char gJyaDoorMetalBarsDL[] __attribute__((aligned (2))) = dgJyaDoorMetalBarsDL;
#endif
            
#define dgSpiritDoorTex "__OTR__objects/object_jya_door/gSpiritDoorTex"
#ifdef _WIN32
static const __declspec(align(2)) char gSpiritDoorTex[] = dgSpiritDoorTex;
#else
static const char gSpiritDoorTex[] __attribute__((aligned (2))) = dgSpiritDoorTex;
#endif
            
#define dgSpiritDoorMetalBarsTex "__OTR__objects/object_jya_door/gSpiritDoorMetalBarsTex"
#ifdef _WIN32
static const __declspec(align(2)) char gSpiritDoorMetalBarsTex[] = dgSpiritDoorMetalBarsTex;
#else
static const char gSpiritDoorMetalBarsTex[] __attribute__((aligned (2))) = dgSpiritDoorMetalBarsTex;
#endif
            


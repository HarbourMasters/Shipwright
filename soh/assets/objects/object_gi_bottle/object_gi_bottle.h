#pragma once
#define dgGiBottleStopperDL "__OTR__objects/object_gi_bottle/gGiBottleStopperDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBottleStopperDL[] = dgGiBottleStopperDL;
#else
static const char gGiBottleStopperDL[] __attribute__((aligned (2))) = dgGiBottleStopperDL;
#endif
            
#define dgGiBottleDL "__OTR__objects/object_gi_bottle/gGiBottleDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBottleDL[] = dgGiBottleDL;
#else
static const char gGiBottleDL[] __attribute__((aligned (2))) = dgGiBottleDL;
#endif
            


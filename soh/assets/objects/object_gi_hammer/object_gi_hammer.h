#pragma once
#define dgGiHammerDL "__OTR__objects/object_gi_hammer/gGiHammerDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiHammerDL[] = dgGiHammerDL;
#else
static const char gGiHammerDL[] __attribute__((aligned (2))) = dgGiHammerDL;
#endif
            


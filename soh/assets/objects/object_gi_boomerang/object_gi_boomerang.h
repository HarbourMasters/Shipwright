#pragma once
#define dgGiBoomerangDL "__OTR__objects/object_gi_boomerang/gGiBoomerangDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBoomerangDL[] = dgGiBoomerangDL;
#else
static const char gGiBoomerangDL[] __attribute__((aligned (2))) = dgGiBoomerangDL;
#endif
            


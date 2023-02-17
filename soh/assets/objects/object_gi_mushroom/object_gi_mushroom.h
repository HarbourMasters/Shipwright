#pragma once
#define dgGiOddMushroomDL "__OTR__objects/object_gi_mushroom/gGiOddMushroomDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiOddMushroomDL[] = dgGiOddMushroomDL;
#else
static const char gGiOddMushroomDL[] __attribute__((aligned (2))) = dgGiOddMushroomDL;
#endif
            


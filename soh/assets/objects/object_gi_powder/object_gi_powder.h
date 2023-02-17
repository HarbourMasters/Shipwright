#pragma once
#define dgGiOddPotionDL "__OTR__objects/object_gi_powder/gGiOddPotionDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiOddPotionDL[] = dgGiOddPotionDL;
#else
static const char gGiOddPotionDL[] __attribute__((aligned (2))) = dgGiOddPotionDL;
#endif
            


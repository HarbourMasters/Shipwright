#pragma once
#define dgGiSeedDL "__OTR__objects/object_gi_seed/gGiSeedDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSeedDL[] = dgGiSeedDL;
#else
static const char gGiSeedDL[] __attribute__((aligned (2))) = dgGiSeedDL;
#endif
            


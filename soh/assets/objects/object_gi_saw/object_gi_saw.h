#pragma once
#define dgGiSawDL "__OTR__objects/object_gi_saw/gGiSawDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSawDL[] = dgGiSawDL;
#else
static const char gGiSawDL[] __attribute__((aligned (2))) = dgGiSawDL;
#endif
            


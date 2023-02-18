#pragma once
#define dgGiFishDL "__OTR__objects/object_gi_fish/gGiFishDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiFishDL[] = dgGiFishDL;
#else
static const char gGiFishDL[] __attribute__((aligned (2))) = dgGiFishDL;
#endif
            


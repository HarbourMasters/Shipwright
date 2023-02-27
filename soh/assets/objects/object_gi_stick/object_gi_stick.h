#pragma once
#define dgGiStickDL "__OTR__objects/object_gi_stick/gGiStickDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiStickDL[] = dgGiStickDL;
#else
static const char gGiStickDL[] __attribute__((aligned (2))) = dgGiStickDL;
#endif
            


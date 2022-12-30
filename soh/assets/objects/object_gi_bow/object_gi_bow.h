#pragma once
#define dgGiBowDL "__OTR__objects/object_gi_bow/gGiBowDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBowDL[] = dgGiBowDL;
#else
static const char gGiBowDL[] __attribute__((aligned (2))) = dgGiBowDL;
#endif
            


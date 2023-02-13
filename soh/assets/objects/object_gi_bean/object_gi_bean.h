#pragma once
#define dgGiBeanDL "__OTR__objects/object_gi_bean/gGiBeanDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBeanDL[] = dgGiBeanDL;
#else
static const char gGiBeanDL[] __attribute__((aligned (2))) = dgGiBeanDL;
#endif
            


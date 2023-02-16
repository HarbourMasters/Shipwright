#pragma once
#define dgGiSmallKeyDL "__OTR__objects/object_gi_key/gGiSmallKeyDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSmallKeyDL[] = dgGiSmallKeyDL;
#else
static const char gGiSmallKeyDL[] __attribute__((aligned (2))) = dgGiSmallKeyDL;
#endif
            


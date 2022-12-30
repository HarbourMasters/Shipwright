#pragma once
#define dgGiHookshotDL "__OTR__objects/object_gi_hookshot/gGiHookshotDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiHookshotDL[] = dgGiHookshotDL;
#else
static const char gGiHookshotDL[] __attribute__((aligned (2))) = dgGiHookshotDL;
#endif
            
#define dgGiLongshotDL "__OTR__objects/object_gi_hookshot/gGiLongshotDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiLongshotDL[] = dgGiLongshotDL;
#else
static const char gGiLongshotDL[] __attribute__((aligned (2))) = dgGiLongshotDL;
#endif
            


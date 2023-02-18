#pragma once
#define dgGiBossKeyDL "__OTR__objects/object_gi_bosskey/gGiBossKeyDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBossKeyDL[] = dgGiBossKeyDL;
#else
static const char gGiBossKeyDL[] __attribute__((aligned (2))) = dgGiBossKeyDL;
#endif
            
#define dgGiBossKeyGemDL "__OTR__objects/object_gi_bosskey/gGiBossKeyGemDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBossKeyGemDL[] = dgGiBossKeyGemDL;
#else
static const char gGiBossKeyGemDL[] __attribute__((aligned (2))) = dgGiBossKeyGemDL;
#endif
            


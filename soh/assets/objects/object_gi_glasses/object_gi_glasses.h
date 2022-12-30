#pragma once
#define dgGiLensDL "__OTR__objects/object_gi_glasses/gGiLensDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiLensDL[] = dgGiLensDL;
#else
static const char gGiLensDL[] __attribute__((aligned (2))) = dgGiLensDL;
#endif
            
#define dgGiLensGlassDL "__OTR__objects/object_gi_glasses/gGiLensGlassDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiLensGlassDL[] = dgGiLensGlassDL;
#else
static const char gGiLensGlassDL[] __attribute__((aligned (2))) = dgGiLensGlassDL;
#endif
            


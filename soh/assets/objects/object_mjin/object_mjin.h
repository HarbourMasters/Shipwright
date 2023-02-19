#pragma once
#define dgWarpPadBaseDL "__OTR__objects/object_mjin/gWarpPadBaseDL"
#ifdef _WIN32
static const __declspec(align(2)) char gWarpPadBaseDL[] = dgWarpPadBaseDL;
#else
static const char gWarpPadBaseDL[] __attribute__((aligned (2))) = dgWarpPadBaseDL;
#endif
            
#define dgWarpPadCol "__OTR__objects/object_mjin/gWarpPadCol"
#ifdef _WIN32
static const __declspec(align(2)) char gWarpPadCol[] = dgWarpPadCol;
#else
static const char gWarpPadCol[] __attribute__((aligned (2))) = dgWarpPadCol;
#endif
            
#define dgWarpPadSideTex "__OTR__objects/object_mjin/gWarpPadSideTex"
#ifdef _WIN32
static const __declspec(align(2)) char gWarpPadSideTex[] = dgWarpPadSideTex;
#else
static const char gWarpPadSideTex[] __attribute__((aligned (2))) = dgWarpPadSideTex;
#endif
            
#define dgWarpPadTopTex "__OTR__objects/object_mjin/gWarpPadTopTex"
#ifdef _WIN32
static const __declspec(align(2)) char gWarpPadTopTex[] = dgWarpPadTopTex;
#else
static const char gWarpPadTopTex[] __attribute__((aligned (2))) = dgWarpPadTopTex;
#endif
            


#pragma once
#define dgTimeWarpSkel "__OTR__objects/object_efc_tw/gTimeWarpSkel"
#ifdef _WIN32
static const __declspec(align(2)) char gTimeWarpSkel[] = dgTimeWarpSkel;
#else
static const char gTimeWarpSkel[] __attribute__((aligned (2))) = dgTimeWarpSkel;
#endif
            
#define dgTimeWarpAnim "__OTR__objects/object_efc_tw/gTimeWarpAnim"
#ifdef _WIN32
static const __declspec(align(2)) char gTimeWarpAnim[] = dgTimeWarpAnim;
#else
static const char gTimeWarpAnim[] __attribute__((aligned (2))) = dgTimeWarpAnim;
#endif
            
#define dgTimeWarpVtx "__OTR__objects/object_efc_tw/gTimeWarpVtx"
#ifdef _WIN32
static const __declspec(align(2)) char gTimeWarpVtx[] = dgTimeWarpVtx;
#else
static const char gTimeWarpVtx[] __attribute__((aligned (2))) = dgTimeWarpVtx;
#endif
            
#define dgTimeWarpDL "__OTR__objects/object_efc_tw/gTimeWarpDL"
#ifdef _WIN32
static const __declspec(align(2)) char gTimeWarpDL[] = dgTimeWarpDL;
#else
static const char gTimeWarpDL[] __attribute__((aligned (2))) = dgTimeWarpDL;
#endif
            
#define dgTimeWarpTex "__OTR__objects/object_efc_tw/gTimeWarpTex"
#ifdef _WIN32
static const __declspec(align(2)) char gTimeWarpTex[] = dgTimeWarpTex;
#else
static const char gTimeWarpTex[] __attribute__((aligned (2))) = dgTimeWarpTex;
#endif
            


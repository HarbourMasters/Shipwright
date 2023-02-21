#pragma once
#define dgMantTex "__OTR__overlays/ovl_En_Ganon_Mant/gMantTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMantTex[] = dgMantTex;
#else
static const char gMantTex[] __attribute__((aligned (2))) = dgMantTex;
#endif
            
#define dgMantUnusedTex "__OTR__overlays/ovl_En_Ganon_Mant/gMantUnusedTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMantUnusedTex[] = dgMantUnusedTex;
#else
static const char gMantUnusedTex[] __attribute__((aligned (2))) = dgMantUnusedTex;
#endif
            
#define dgMant1Vtx "__OTR__overlays/ovl_En_Ganon_Mant/gMant1Vtx"
#ifdef _WIN32
static const __declspec(align(2)) char gMant1Vtx[] = dgMant1Vtx;
#else
static const char gMant1Vtx[] __attribute__((aligned (2))) = dgMant1Vtx;
#endif
            
#define dgMantMaterialDL "__OTR__overlays/ovl_En_Ganon_Mant/gMantMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char gMantMaterialDL[] = dgMantMaterialDL;
#else
static const char gMantMaterialDL[] __attribute__((aligned (2))) = dgMantMaterialDL;
#endif
            
#define dgMantUnusedMaterialDL "__OTR__overlays/ovl_En_Ganon_Mant/gMantUnusedMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char gMantUnusedMaterialDL[] = dgMantUnusedMaterialDL;
#else
static const char gMantUnusedMaterialDL[] __attribute__((aligned (2))) = dgMantUnusedMaterialDL;
#endif
            
#define dgMantDL "__OTR__overlays/ovl_En_Ganon_Mant/gMantDL"
#ifdef _WIN32
static const __declspec(align(2)) char gMantDL[] = dgMantDL;
#else
static const char gMantDL[] __attribute__((aligned (2))) = dgMantDL;
#endif
            
#define dgMant2Vtx "__OTR__overlays/ovl_En_Ganon_Mant/gMant2Vtx"
#ifdef _WIN32
static const __declspec(align(2)) char gMant2Vtx[] = dgMant2Vtx;
#else
static const char gMant2Vtx[] __attribute__((aligned (2))) = dgMant2Vtx;
#endif
            


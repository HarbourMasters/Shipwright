#pragma once
#define dgGiNutDL "__OTR__objects/object_gi_nuts/gGiNutDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiNutDL[] = dgGiNutDL;
#else
static const char gGiNutDL[] __attribute__((aligned (2))) = dgGiNutDL;
#endif
            
#define dobject_gi_nutsTex_000000 "__OTR__objects/object_gi_nuts/object_gi_nutsTex_000000"
#ifdef _WIN32
static const __declspec(align(2)) char object_gi_nutsTex_000000[] = dobject_gi_nutsTex_000000;
#else
static const char object_gi_nutsTex_000000[] __attribute__((aligned (2))) = dobject_gi_nutsTex_000000;
#endif
            


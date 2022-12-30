#pragma once
#define dsTex "__OTR__overlays/ovl_Magic_Fire/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsSphereVtx "__OTR__overlays/ovl_Magic_Fire/sSphereVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sSphereVtx[] = dsSphereVtx;
#else
static const char sSphereVtx[] __attribute__((aligned (2))) = dsSphereVtx;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Magic_Fire/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsModelDL "__OTR__overlays/ovl_Magic_Fire/sModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sModelDL[] = dsModelDL;
#else
static const char sModelDL[] __attribute__((aligned (2))) = dsModelDL;
#endif
            


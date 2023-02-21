#pragma once
#define dsDiamondTex "__OTR__overlays/ovl_Magic_Dark/sDiamondTex"
#ifdef _WIN32
static const __declspec(align(2)) char sDiamondTex[] = dsDiamondTex;
#else
static const char sDiamondTex[] __attribute__((aligned (2))) = dsDiamondTex;
#endif
            
#define dsDiamondVtx "__OTR__overlays/ovl_Magic_Dark/sDiamondVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sDiamondVtx[] = dsDiamondVtx;
#else
static const char sDiamondVtx[] __attribute__((aligned (2))) = dsDiamondVtx;
#endif
            
#define dsDiamondMaterialDL "__OTR__overlays/ovl_Magic_Dark/sDiamondMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sDiamondMaterialDL[] = dsDiamondMaterialDL;
#else
static const char sDiamondMaterialDL[] __attribute__((aligned (2))) = dsDiamondMaterialDL;
#endif
            
#define dsDiamondModelDL "__OTR__overlays/ovl_Magic_Dark/sDiamondModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sDiamondModelDL[] = dsDiamondModelDL;
#else
static const char sDiamondModelDL[] __attribute__((aligned (2))) = dsDiamondModelDL;
#endif
            


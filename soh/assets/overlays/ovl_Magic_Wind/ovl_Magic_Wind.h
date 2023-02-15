#pragma once
#define dsSkel "__OTR__overlays/ovl_Magic_Wind/sSkel"
#ifdef _WIN32
static const __declspec(align(2)) char sSkel[] = dsSkel;
#else
static const char sSkel[] __attribute__((aligned (2))) = dsSkel;
#endif
            
#define dsAnim "__OTR__overlays/ovl_Magic_Wind/sAnim"
#ifdef _WIN32
static const __declspec(align(2)) char sAnim[] = dsAnim;
#else
static const char sAnim[] __attribute__((aligned (2))) = dsAnim;
#endif
            
#define dsCylinderVtx "__OTR__overlays/ovl_Magic_Wind/sCylinderVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderVtx[] = dsCylinderVtx;
#else
static const char sCylinderVtx[] __attribute__((aligned (2))) = dsCylinderVtx;
#endif
            
#define dsTex "__OTR__overlays/ovl_Magic_Wind/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsInnerCylinderDL "__OTR__overlays/ovl_Magic_Wind/sInnerCylinderDL"
#ifdef _WIN32
static const __declspec(align(2)) char sInnerCylinderDL[] = dsInnerCylinderDL;
#else
static const char sInnerCylinderDL[] __attribute__((aligned (2))) = dsInnerCylinderDL;
#endif
            
#define dsOuterCylinderDL "__OTR__overlays/ovl_Magic_Wind/sOuterCylinderDL"
#ifdef _WIN32
static const __declspec(align(2)) char sOuterCylinderDL[] = dsOuterCylinderDL;
#else
static const char sOuterCylinderDL[] __attribute__((aligned (2))) = dsOuterCylinderDL;
#endif
            


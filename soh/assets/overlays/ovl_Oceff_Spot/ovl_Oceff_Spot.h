#pragma once
#define dsTex "__OTR__overlays/ovl_Oceff_Spot/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsCylinderVtx "__OTR__overlays/ovl_Oceff_Spot/sCylinderVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderVtx[] = dsCylinderVtx;
#else
static const char sCylinderVtx[] __attribute__((aligned (2))) = dsCylinderVtx;
#endif
            
#define dsCylinderMaterialDL "__OTR__overlays/ovl_Oceff_Spot/sCylinderMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderMaterialDL[] = dsCylinderMaterialDL;
#else
static const char sCylinderMaterialDL[] __attribute__((aligned (2))) = dsCylinderMaterialDL;
#endif
            
#define dsCylinderModelDL "__OTR__overlays/ovl_Oceff_Spot/sCylinderModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderModelDL[] = dsCylinderModelDL;
#else
static const char sCylinderModelDL[] __attribute__((aligned (2))) = dsCylinderModelDL;
#endif
            


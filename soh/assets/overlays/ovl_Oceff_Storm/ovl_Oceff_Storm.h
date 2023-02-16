#pragma once
#define dsTex "__OTR__overlays/ovl_Oceff_Storm/sTex"
#ifdef _WIN32
static const __declspec(align(2)) char sTex[] = dsTex;
#else
static const char sTex[] __attribute__((aligned (2))) = dsTex;
#endif
            
#define dsMaterialDL "__OTR__overlays/ovl_Oceff_Storm/sMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sMaterialDL[] = dsMaterialDL;
#else
static const char sMaterialDL[] __attribute__((aligned (2))) = dsMaterialDL;
#endif
            
#define dsCylinderVtx "__OTR__overlays/ovl_Oceff_Storm/sCylinderVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderVtx[] = dsCylinderVtx;
#else
static const char sCylinderVtx[] __attribute__((aligned (2))) = dsCylinderVtx;
#endif
            
#define dsCylinderMaterialDL "__OTR__overlays/ovl_Oceff_Storm/sCylinderMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderMaterialDL[] = dsCylinderMaterialDL;
#else
static const char sCylinderMaterialDL[] __attribute__((aligned (2))) = dsCylinderMaterialDL;
#endif
            
#define dsCylinderModelDL "__OTR__overlays/ovl_Oceff_Storm/sCylinderModelDL"
#ifdef _WIN32
static const __declspec(align(2)) char sCylinderModelDL[] = dsCylinderModelDL;
#else
static const char sCylinderModelDL[] __attribute__((aligned (2))) = dsCylinderModelDL;
#endif
            


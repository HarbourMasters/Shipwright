#pragma once
#define dsPlaneVtx "__OTR__overlays/ovl_En_Holl/sPlaneVtx"
#ifdef _WIN32
static const __declspec(align(2)) char sPlaneVtx[] = dsPlaneVtx;
#else
static const char sPlaneVtx[] __attribute__((aligned (2))) = dsPlaneVtx;
#endif
            
#define dsPlaneDL "__OTR__overlays/ovl_En_Holl/sPlaneDL"
#ifdef _WIN32
static const __declspec(align(2)) char sPlaneDL[] = dsPlaneDL;
#else
static const char sPlaneDL[] __attribute__((aligned (2))) = dsPlaneDL;
#endif
            


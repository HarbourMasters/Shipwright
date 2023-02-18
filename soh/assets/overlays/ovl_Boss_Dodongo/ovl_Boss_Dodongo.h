#pragma once
#define dsLavaFloorLavaTex "__OTR__overlays/ovl_Boss_Dodongo/sLavaFloorLavaTex"
#ifdef _WIN32
static const __declspec(align(2)) char sLavaFloorLavaTex[] = dsLavaFloorLavaTex;
#else
static const char sLavaFloorLavaTex[] __attribute__((aligned (2))) = dsLavaFloorLavaTex;
#endif
            
#define dsLavaFloorRockTex "__OTR__overlays/ovl_Boss_Dodongo/sLavaFloorRockTex"
#ifdef _WIN32
static const __declspec(align(2)) char sLavaFloorRockTex[] = dsLavaFloorRockTex;
#else
static const char sLavaFloorRockTex[] __attribute__((aligned (2))) = dsLavaFloorRockTex;
#endif
            


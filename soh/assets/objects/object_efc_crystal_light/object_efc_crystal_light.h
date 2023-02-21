#pragma once
#define dgCrystalLightTex "__OTR__objects/object_efc_crystal_light/gCrystalLightTex"
#ifdef _WIN32
static const __declspec(align(2)) char gCrystalLightTex[] = dgCrystalLightTex;
#else
static const char gCrystalLightTex[] __attribute__((aligned (2))) = dgCrystalLightTex;
#endif
            
#define dgCrystalLightDL "__OTR__objects/object_efc_crystal_light/gCrystalLightDL"
#ifdef _WIN32
static const __declspec(align(2)) char gCrystalLightDL[] = dgCrystalLightDL;
#else
static const char gCrystalLightDL[] __attribute__((aligned (2))) = dgCrystalLightDL;
#endif
            


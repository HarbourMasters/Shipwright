#pragma once
#define dgGiEggMaterialDL "__OTR__objects/object_gi_egg/gGiEggMaterialDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiEggMaterialDL[] = dgGiEggMaterialDL;
#else
static const char gGiEggMaterialDL[] __attribute__((aligned (2))) = dgGiEggMaterialDL;
#endif
            
#define dgGiEggDL "__OTR__objects/object_gi_egg/gGiEggDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiEggDL[] = dgGiEggDL;
#else
static const char gGiEggDL[] __attribute__((aligned (2))) = dgGiEggDL;
#endif
            


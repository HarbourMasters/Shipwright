#pragma once
#define dgEnliveningLightTex "__OTR__objects/object_efc_lgt_shower/gEnliveningLightTex"
#ifdef _WIN32
static const __declspec(align(2)) char gEnliveningLightTex[] = dgEnliveningLightTex;
#else
static const char gEnliveningLightTex[] __attribute__((aligned (2))) = dgEnliveningLightTex;
#endif
            
#define dgEnliveningLightDL "__OTR__objects/object_efc_lgt_shower/gEnliveningLightDL"
#ifdef _WIN32
static const __declspec(align(2)) char gEnliveningLightDL[] = dgEnliveningLightDL;
#else
static const char gEnliveningLightDL[] __attribute__((aligned (2))) = dgEnliveningLightDL;
#endif
            


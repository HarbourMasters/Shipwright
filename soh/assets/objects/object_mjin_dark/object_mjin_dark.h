#pragma once
#define dgShadowMedallionPlatformTex "__OTR__objects/object_mjin_dark/gShadowMedallionPlatformTex"
#ifdef _WIN32
static const __declspec(align(2)) char gShadowMedallionPlatformTex[] = dgShadowMedallionPlatformTex;
#else
static const char gShadowMedallionPlatformTex[] __attribute__((aligned (2))) = dgShadowMedallionPlatformTex;
#endif
            


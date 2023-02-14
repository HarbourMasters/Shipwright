#pragma once
#define dgWaterMedallionPlatformTex "__OTR__objects/object_mjin_ice/gWaterMedallionPlatformTex"
#ifdef _WIN32
static const __declspec(align(2)) char gWaterMedallionPlatformTex[] = dgWaterMedallionPlatformTex;
#else
static const char gWaterMedallionPlatformTex[] __attribute__((aligned (2))) = dgWaterMedallionPlatformTex;
#endif
            


#pragma once
#define dgLightMedallionPlatformTex "__OTR__objects/object_mjin_flash/gLightMedallionPlatformTex"
#ifdef _WIN32
static const __declspec(align(2)) char gLightMedallionPlatformTex[] = dgLightMedallionPlatformTex;
#else
static const char gLightMedallionPlatformTex[] __attribute__((aligned (2))) = dgLightMedallionPlatformTex;
#endif
            


#pragma once
#define dgSpiritMedallionPlatformTex "__OTR__objects/object_mjin_soul/gSpiritMedallionPlatformTex"
#ifdef _WIN32
static const __declspec(align(2)) char gSpiritMedallionPlatformTex[] = dgSpiritMedallionPlatformTex;
#else
static const char gSpiritMedallionPlatformTex[] __attribute__((aligned (2))) = dgSpiritMedallionPlatformTex;
#endif
            


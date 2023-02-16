#pragma once
#define dgFireMedallionPlatformTex "__OTR__objects/object_mjin_flame/gFireMedallionPlatformTex"
#ifdef _WIN32
static const __declspec(align(2)) char gFireMedallionPlatformTex[] = dgFireMedallionPlatformTex;
#else
static const char gFireMedallionPlatformTex[] __attribute__((aligned (2))) = dgFireMedallionPlatformTex;
#endif
            


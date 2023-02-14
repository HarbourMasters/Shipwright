#pragma once
#define dgSunsetSkyboxTLUT "__OTR__textures/vr_fine2_pal_static/gSunsetSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gSunsetSkyboxTLUT[] = dgSunsetSkyboxTLUT;
#else
static const char gSunsetSkyboxTLUT[] __attribute__((aligned (2))) = dgSunsetSkyboxTLUT;
#endif
            


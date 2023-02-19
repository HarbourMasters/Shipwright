#pragma once
#define dgNightSkyboxTLUT "__OTR__textures/vr_fine3_pal_static/gNightSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gNightSkyboxTLUT[] = dgNightSkyboxTLUT;
#else
static const char gNightSkyboxTLUT[] __attribute__((aligned (2))) = dgNightSkyboxTLUT;
#endif
            


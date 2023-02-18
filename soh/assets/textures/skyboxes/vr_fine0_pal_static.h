#pragma once
#define dgSunriseSkyboxTLUT "__OTR__textures/vr_fine0_pal_static/gSunriseSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gSunriseSkyboxTLUT[] = dgSunriseSkyboxTLUT;
#else
static const char gSunriseSkyboxTLUT[] __attribute__((aligned (2))) = dgSunriseSkyboxTLUT;
#endif
            


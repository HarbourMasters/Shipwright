#pragma once
#define dgSunriseOvercastSkyboxTLUT "__OTR__textures/vr_cloud0_pal_static/gSunriseOvercastSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gSunriseOvercastSkyboxTLUT[] = dgSunriseOvercastSkyboxTLUT;
#else
static const char gSunriseOvercastSkyboxTLUT[] __attribute__((aligned (2))) = dgSunriseOvercastSkyboxTLUT;
#endif
            


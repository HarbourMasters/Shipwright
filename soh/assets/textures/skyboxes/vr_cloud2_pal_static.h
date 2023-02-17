#pragma once
#define dgSunsetOvercastSkyboxTLUT "__OTR__textures/vr_cloud2_pal_static/gSunsetOvercastSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gSunsetOvercastSkyboxTLUT[] = dgSunsetOvercastSkyboxTLUT;
#else
static const char gSunsetOvercastSkyboxTLUT[] __attribute__((aligned (2))) = dgSunsetOvercastSkyboxTLUT;
#endif
            


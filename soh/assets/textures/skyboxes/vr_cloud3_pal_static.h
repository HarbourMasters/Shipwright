#pragma once
#define dgNightOvercastSkyboxTLUT "__OTR__textures/vr_cloud3_pal_static/gNightOvercastSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gNightOvercastSkyboxTLUT[] = dgNightOvercastSkyboxTLUT;
#else
static const char gNightOvercastSkyboxTLUT[] __attribute__((aligned (2))) = dgNightOvercastSkyboxTLUT;
#endif
            


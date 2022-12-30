#pragma once
#define dgDayOvercastSkyboxTLUT "__OTR__textures/vr_cloud1_pal_static/gDayOvercastSkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gDayOvercastSkyboxTLUT[] = dgDayOvercastSkyboxTLUT;
#else
static const char gDayOvercastSkyboxTLUT[] __attribute__((aligned (2))) = dgDayOvercastSkyboxTLUT;
#endif
            


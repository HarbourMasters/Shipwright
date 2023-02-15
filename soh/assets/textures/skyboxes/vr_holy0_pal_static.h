#pragma once
#define dgHoly0SkyboxTLUT "__OTR__textures/vr_holy0_pal_static/gHoly0SkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gHoly0SkyboxTLUT[] = dgHoly0SkyboxTLUT;
#else
static const char gHoly0SkyboxTLUT[] __attribute__((aligned (2))) = dgHoly0SkyboxTLUT;
#endif
            


#pragma once
#define dgHoly1SkyboxTLUT "__OTR__textures/vr_holy1_pal_static/gHoly1SkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gHoly1SkyboxTLUT[] = dgHoly1SkyboxTLUT;
#else
static const char gHoly1SkyboxTLUT[] __attribute__((aligned (2))) = dgHoly1SkyboxTLUT;
#endif
            


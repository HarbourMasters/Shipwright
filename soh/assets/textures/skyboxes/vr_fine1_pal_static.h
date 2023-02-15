#pragma once
#define dgDaySkyboxTLUT "__OTR__textures/vr_fine1_pal_static/gDaySkyboxTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gDaySkyboxTLUT[] = dgDaySkyboxTLUT;
#else
static const char gDaySkyboxTLUT[] __attribute__((aligned (2))) = dgDaySkyboxTLUT;
#endif
            


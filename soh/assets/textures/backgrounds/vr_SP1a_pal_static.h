#pragma once
#define dgBazaarBgTLUT "__OTR__textures/vr_SP1a_pal_static/gBazaarBgTLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gBazaarBgTLUT[] = dgBazaarBgTLUT;
#else
static const char gBazaarBgTLUT[] __attribute__((aligned (2))) = dgBazaarBgTLUT;
#endif
            
#define dgBazaarBg2TLUT "__OTR__textures/vr_SP1a_pal_static/gBazaarBg2TLUT"
#ifdef _WIN32
static const __declspec(align(2)) char gBazaarBg2TLUT[] = dgBazaarBg2TLUT;
#else
static const char gBazaarBg2TLUT[] __attribute__((aligned (2))) = dgBazaarBg2TLUT;
#endif
            


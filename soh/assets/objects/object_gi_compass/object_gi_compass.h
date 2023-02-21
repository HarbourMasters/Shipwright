#pragma once
#define dgGiCompassDL "__OTR__objects/object_gi_compass/gGiCompassDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiCompassDL[] = dgGiCompassDL;
#else
static const char gGiCompassDL[] __attribute__((aligned (2))) = dgGiCompassDL;
#endif
            
#define dgGiCompassGlassDL "__OTR__objects/object_gi_compass/gGiCompassGlassDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiCompassGlassDL[] = dgGiCompassGlassDL;
#else
static const char gGiCompassGlassDL[] __attribute__((aligned (2))) = dgGiCompassGlassDL;
#endif
            


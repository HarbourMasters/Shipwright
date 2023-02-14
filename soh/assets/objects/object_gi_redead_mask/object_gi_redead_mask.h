#pragma once
#define dgGiSpookyMaskDL "__OTR__objects/object_gi_redead_mask/gGiSpookyMaskDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSpookyMaskDL[] = dgGiSpookyMaskDL;
#else
static const char gGiSpookyMaskDL[] __attribute__((aligned (2))) = dgGiSpookyMaskDL;
#endif
            


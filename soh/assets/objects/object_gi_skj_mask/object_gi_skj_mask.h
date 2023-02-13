#pragma once
#define dgGiSkullMaskDL "__OTR__objects/object_gi_skj_mask/gGiSkullMaskDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSkullMaskDL[] = dgGiSkullMaskDL;
#else
static const char gGiSkullMaskDL[] __attribute__((aligned (2))) = dgGiSkullMaskDL;
#endif
            


#pragma once
#define dgGiGrassDL "__OTR__objects/object_gi_grass/gGiGrassDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiGrassDL[] = dgGiGrassDL;
#else
static const char gGiGrassDL[] __attribute__((aligned (2))) = dgGiGrassDL;
#endif
            


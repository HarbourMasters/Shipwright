#pragma once
#define dgGiSlingshotDL "__OTR__objects/object_gi_pachinko/gGiSlingshotDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiSlingshotDL[] = dgGiSlingshotDL;
#else
static const char gGiSlingshotDL[] __attribute__((aligned (2))) = dgGiSlingshotDL;
#endif
            


#pragma once
#define dgGiBombDL "__OTR__objects/object_gi_bomb_1/gGiBombDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiBombDL[] = dgGiBombDL;
#else
static const char gGiBombDL[] __attribute__((aligned (2))) = dgGiBombDL;
#endif
            


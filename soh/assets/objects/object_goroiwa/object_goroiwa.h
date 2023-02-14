#pragma once
#define dgRollingRockTex "__OTR__objects/object_goroiwa/gRollingRockTex"
#ifdef _WIN32
static const __declspec(align(2)) char gRollingRockTex[] = dgRollingRockTex;
#else
static const char gRollingRockTex[] __attribute__((aligned (2))) = dgRollingRockTex;
#endif
            
#define dgRollingRockDL "__OTR__objects/object_goroiwa/gRollingRockDL"
#ifdef _WIN32
static const __declspec(align(2)) char gRollingRockDL[] = dgRollingRockDL;
#else
static const char gRollingRockDL[] __attribute__((aligned (2))) = dgRollingRockDL;
#endif
            


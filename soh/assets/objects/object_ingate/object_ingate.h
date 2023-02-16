#pragma once
#define dgIngoGateTex "__OTR__objects/object_ingate/gIngoGateTex"
#ifdef _WIN32
static const __declspec(align(2)) char gIngoGateTex[] = dgIngoGateTex;
#else
static const char gIngoGateTex[] __attribute__((aligned (2))) = dgIngoGateTex;
#endif
            
#define dgIngoGateDL "__OTR__objects/object_ingate/gIngoGateDL"
#ifdef _WIN32
static const __declspec(align(2)) char gIngoGateDL[] = dgIngoGateDL;
#else
static const char gIngoGateDL[] __attribute__((aligned (2))) = dgIngoGateDL;
#endif
            
#define dgIngoGateCol "__OTR__objects/object_ingate/gIngoGateCol"
#ifdef _WIN32
static const __declspec(align(2)) char gIngoGateCol[] = dgIngoGateCol;
#else
static const char gIngoGateCol[] __attribute__((aligned (2))) = dgIngoGateCol;
#endif
            


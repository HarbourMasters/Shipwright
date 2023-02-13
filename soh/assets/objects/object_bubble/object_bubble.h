#pragma once
#define dgBubbleTex "__OTR__objects/object_bubble/gBubbleTex"
#ifdef _WIN32
static const __declspec(align(2)) char gBubbleTex[] = dgBubbleTex;
#else
static const char gBubbleTex[] __attribute__((aligned (2))) = dgBubbleTex;
#endif
            
#define dgBubbleDL "__OTR__objects/object_bubble/gBubbleDL"
#ifdef _WIN32
static const __declspec(align(2)) char gBubbleDL[] = dgBubbleDL;
#else
static const char gBubbleDL[] __attribute__((aligned (2))) = dgBubbleDL;
#endif
            


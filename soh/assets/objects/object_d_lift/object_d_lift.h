#pragma once
#define dgCollapsingPlatformDL "__OTR__objects/object_d_lift/gCollapsingPlatformDL"
#ifdef _WIN32
static const __declspec(align(2)) char gCollapsingPlatformDL[] = dgCollapsingPlatformDL;
#else
static const char gCollapsingPlatformDL[] __attribute__((aligned (2))) = dgCollapsingPlatformDL;
#endif
            
#define dgCollapsingPlatformCol "__OTR__objects/object_d_lift/gCollapsingPlatformCol"
#ifdef _WIN32
static const __declspec(align(2)) char gCollapsingPlatformCol[] = dgCollapsingPlatformCol;
#else
static const char gCollapsingPlatformCol[] __attribute__((aligned (2))) = dgCollapsingPlatformCol;
#endif
            
#define dgCollapsingPlatformSideTex "__OTR__objects/object_d_lift/gCollapsingPlatformSideTex"
#ifdef _WIN32
static const __declspec(align(2)) char gCollapsingPlatformSideTex[] = dgCollapsingPlatformSideTex;
#else
static const char gCollapsingPlatformSideTex[] __attribute__((aligned (2))) = dgCollapsingPlatformSideTex;
#endif
            
#define dgCollapsingPlatformTopTex "__OTR__objects/object_d_lift/gCollapsingPlatformTopTex"
#ifdef _WIN32
static const __declspec(align(2)) char gCollapsingPlatformTopTex[] = dgCollapsingPlatformTopTex;
#else
static const char gCollapsingPlatformTopTex[] __attribute__((aligned (2))) = dgCollapsingPlatformTopTex;
#endif
            


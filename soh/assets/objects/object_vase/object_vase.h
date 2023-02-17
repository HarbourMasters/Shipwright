#pragma once
#define dgUnusedVaseDL "__OTR__objects/object_vase/gUnusedVaseDL"
#ifdef _WIN32
static const __declspec(align(2)) char gUnusedVaseDL[] = dgUnusedVaseDL;
#else
static const char gUnusedVaseDL[] __attribute__((aligned (2))) = dgUnusedVaseDL;
#endif
            
#define dgUnusedVaseTopTex "__OTR__objects/object_vase/gUnusedVaseTopTex"
#ifdef _WIN32
static const __declspec(align(2)) char gUnusedVaseTopTex[] = dgUnusedVaseTopTex;
#else
static const char gUnusedVaseTopTex[] __attribute__((aligned (2))) = dgUnusedVaseTopTex;
#endif
            
#define dgUnusedVaseBodyTex "__OTR__objects/object_vase/gUnusedVaseBodyTex"
#ifdef _WIN32
static const __declspec(align(2)) char gUnusedVaseBodyTex[] = dgUnusedVaseBodyTex;
#else
static const char gUnusedVaseBodyTex[] __attribute__((aligned (2))) = dgUnusedVaseBodyTex;
#endif
            


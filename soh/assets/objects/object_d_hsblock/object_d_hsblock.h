#pragma once
#define dgHookshotTargetCol "__OTR__objects/object_d_hsblock/gHookshotTargetCol"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotTargetCol[] = dgHookshotTargetCol;
#else
static const char gHookshotTargetCol[] __attribute__((aligned (2))) = dgHookshotTargetCol;
#endif
            
#define dgHookshotPostCol "__OTR__objects/object_d_hsblock/gHookshotPostCol"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotPostCol[] = dgHookshotPostCol;
#else
static const char gHookshotPostCol[] __attribute__((aligned (2))) = dgHookshotPostCol;
#endif
            
#define dgHookshotPostDL "__OTR__objects/object_d_hsblock/gHookshotPostDL"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotPostDL[] = dgHookshotPostDL;
#else
static const char gHookshotPostDL[] __attribute__((aligned (2))) = dgHookshotPostDL;
#endif
            
#define dgHookshotTargetDL "__OTR__objects/object_d_hsblock/gHookshotTargetDL"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotTargetDL[] = dgHookshotTargetDL;
#else
static const char gHookshotTargetDL[] __attribute__((aligned (2))) = dgHookshotTargetDL;
#endif
            
#define dgHookshotTargetTex "__OTR__objects/object_d_hsblock/gHookshotTargetTex"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotTargetTex[] = dgHookshotTargetTex;
#else
static const char gHookshotTargetTex[] __attribute__((aligned (2))) = dgHookshotTargetTex;
#endif
            
#define dgHookshotPostTex "__OTR__objects/object_d_hsblock/gHookshotPostTex"
#ifdef _WIN32
static const __declspec(align(2)) char gHookshotPostTex[] = dgHookshotPostTex;
#else
static const char gHookshotPostTex[] __attribute__((aligned (2))) = dgHookshotPostTex;
#endif
            


#pragma once
#define dgCreationFireBallDL "__OTR__objects/object_efc_fire_ball/gCreationFireBallDL"
#ifdef _WIN32
static const __declspec(align(2)) char gCreationFireBallDL[] = dgCreationFireBallDL;
#else
static const char gCreationFireBallDL[] __attribute__((aligned (2))) = dgCreationFireBallDL;
#endif
            
#define dgCreationFireBallMaskTex "__OTR__objects/object_efc_fire_ball/gCreationFireBallMaskTex"
#ifdef _WIN32
static const __declspec(align(2)) char gCreationFireBallMaskTex[] = dgCreationFireBallMaskTex;
#else
static const char gCreationFireBallMaskTex[] __attribute__((aligned (2))) = dgCreationFireBallMaskTex;
#endif
            
#define dgCreationFireBallFlameEffectTex "__OTR__objects/object_efc_fire_ball/gCreationFireBallFlameEffectTex"
#ifdef _WIN32
static const __declspec(align(2)) char gCreationFireBallFlameEffectTex[] = dgCreationFireBallFlameEffectTex;
#else
static const char gCreationFireBallFlameEffectTex[] __attribute__((aligned (2))) = dgCreationFireBallFlameEffectTex;
#endif
            


#pragma once
#define dgGiFrogDL "__OTR__objects/object_gi_frog/gGiFrogDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiFrogDL[] = dgGiFrogDL;
#else
static const char gGiFrogDL[] __attribute__((aligned (2))) = dgGiFrogDL;
#endif
            
#define dgGiFrogEyesDL "__OTR__objects/object_gi_frog/gGiFrogEyesDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiFrogEyesDL[] = dgGiFrogEyesDL;
#else
static const char gGiFrogEyesDL[] __attribute__((aligned (2))) = dgGiFrogEyesDL;
#endif
            
#define dobject_gi_frogTex_000000 "__OTR__objects/object_gi_frog/object_gi_frogTex_000000"
#ifdef _WIN32
static const __declspec(align(2)) char object_gi_frogTex_000000[] = dobject_gi_frogTex_000000;
#else
static const char object_gi_frogTex_000000[] __attribute__((aligned (2))) = dobject_gi_frogTex_000000;
#endif
            


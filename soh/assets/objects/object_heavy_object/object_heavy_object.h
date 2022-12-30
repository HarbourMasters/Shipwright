#pragma once
#define dgHeavyBlockTex "__OTR__objects/object_heavy_object/gHeavyBlockTex"
#ifdef _WIN32
static const __declspec(align(2)) char gHeavyBlockTex[] = dgHeavyBlockTex;
#else
static const char gHeavyBlockTex[] __attribute__((aligned (2))) = dgHeavyBlockTex;
#endif
            
#define dgHeavyBlockEntirePillarDL "__OTR__objects/object_heavy_object/gHeavyBlockEntirePillarDL"
#ifdef _WIN32
static const __declspec(align(2)) char gHeavyBlockEntirePillarDL[] = dgHeavyBlockEntirePillarDL;
#else
static const char gHeavyBlockEntirePillarDL[] __attribute__((aligned (2))) = dgHeavyBlockEntirePillarDL;
#endif
            
#define dgHeavyBlockBigPieceDL "__OTR__objects/object_heavy_object/gHeavyBlockBigPieceDL"
#ifdef _WIN32
static const __declspec(align(2)) char gHeavyBlockBigPieceDL[] = dgHeavyBlockBigPieceDL;
#else
static const char gHeavyBlockBigPieceDL[] __attribute__((aligned (2))) = dgHeavyBlockBigPieceDL;
#endif
            
#define dgHeavyBlockSmallPieceDL "__OTR__objects/object_heavy_object/gHeavyBlockSmallPieceDL"
#ifdef _WIN32
static const __declspec(align(2)) char gHeavyBlockSmallPieceDL[] = dgHeavyBlockSmallPieceDL;
#else
static const char gHeavyBlockSmallPieceDL[] __attribute__((aligned (2))) = dgHeavyBlockSmallPieceDL;
#endif
            
#define dgHeavyBlockCol "__OTR__objects/object_heavy_object/gHeavyBlockCol"
#ifdef _WIN32
static const __declspec(align(2)) char gHeavyBlockCol[] = dgHeavyBlockCol;
#else
static const char gHeavyBlockCol[] __attribute__((aligned (2))) = dgHeavyBlockCol;
#endif
            


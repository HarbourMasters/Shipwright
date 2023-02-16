#pragma once
#define dgSongOfTimeBlockDL "__OTR__objects/object_timeblock/gSongOfTimeBlockDL"
#ifdef _WIN32
static const __declspec(align(2)) char gSongOfTimeBlockDL[] = dgSongOfTimeBlockDL;
#else
static const char gSongOfTimeBlockDL[] __attribute__((aligned (2))) = dgSongOfTimeBlockDL;
#endif
            
#define dgSongOfTimeBlockTex "__OTR__objects/object_timeblock/gSongOfTimeBlockTex"
#ifdef _WIN32
static const __declspec(align(2)) char gSongOfTimeBlockTex[] = dgSongOfTimeBlockTex;
#else
static const char gSongOfTimeBlockTex[] __attribute__((aligned (2))) = dgSongOfTimeBlockTex;
#endif
            
#define dgSongOfTimeBlockCol "__OTR__objects/object_timeblock/gSongOfTimeBlockCol"
#ifdef _WIN32
static const __declspec(align(2)) char gSongOfTimeBlockCol[] = dgSongOfTimeBlockCol;
#else
static const char gSongOfTimeBlockCol[] __attribute__((aligned (2))) = dgSongOfTimeBlockCol;
#endif
            


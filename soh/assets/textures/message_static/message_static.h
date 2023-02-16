#pragma once
#define dgDefaultMessageBackgroundTex "__OTR__textures/message_static/gDefaultMessageBackgroundTex"
#ifdef _WIN32
static const __declspec(align(2)) char gDefaultMessageBackgroundTex[] = dgDefaultMessageBackgroundTex;
#else
static const char gDefaultMessageBackgroundTex[] __attribute__((aligned (2))) = dgDefaultMessageBackgroundTex;
#endif
            
#define dgSignMessageBackgroundTex "__OTR__textures/message_static/gSignMessageBackgroundTex"
#ifdef _WIN32
static const __declspec(align(2)) char gSignMessageBackgroundTex[] = dgSignMessageBackgroundTex;
#else
static const char gSignMessageBackgroundTex[] __attribute__((aligned (2))) = dgSignMessageBackgroundTex;
#endif
            
#define dgNoteStaffMessageBackgroundTex "__OTR__textures/message_static/gNoteStaffMessageBackgroundTex"
#ifdef _WIN32
static const __declspec(align(2)) char gNoteStaffMessageBackgroundTex[] = dgNoteStaffMessageBackgroundTex;
#else
static const char gNoteStaffMessageBackgroundTex[] __attribute__((aligned (2))) = dgNoteStaffMessageBackgroundTex;
#endif
            
#define dgFadingMessageBackgroundTex "__OTR__textures/message_static/gFadingMessageBackgroundTex"
#ifdef _WIN32
static const __declspec(align(2)) char gFadingMessageBackgroundTex[] = dgFadingMessageBackgroundTex;
#else
static const char gFadingMessageBackgroundTex[] __attribute__((aligned (2))) = dgFadingMessageBackgroundTex;
#endif
            
#define dgMessageContinueTriangleTex "__OTR__textures/message_static/gMessageContinueTriangleTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMessageContinueTriangleTex[] = dgMessageContinueTriangleTex;
#else
static const char gMessageContinueTriangleTex[] __attribute__((aligned (2))) = dgMessageContinueTriangleTex;
#endif
            
#define dgMessageEndSquareTex "__OTR__textures/message_static/gMessageEndSquareTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMessageEndSquareTex[] = dgMessageEndSquareTex;
#else
static const char gMessageEndSquareTex[] __attribute__((aligned (2))) = dgMessageEndSquareTex;
#endif
            
#define dgMessageArrowTex "__OTR__textures/message_static/gMessageArrowTex"
#ifdef _WIN32
static const __declspec(align(2)) char gMessageArrowTex[] = dgMessageArrowTex;
#else
static const char gMessageArrowTex[] __attribute__((aligned (2))) = dgMessageArrowTex;
#endif
            


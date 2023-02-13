#pragma once
#define dgRedMessageXLeftTex "__OTR__textures/message_texture_static/gRedMessageXLeftTex"
#ifdef _WIN32
static const __declspec(align(2)) char gRedMessageXLeftTex[] = dgRedMessageXLeftTex;
#else
static const char gRedMessageXLeftTex[] __attribute__((aligned (2))) = dgRedMessageXLeftTex;
#endif
            
#define dgRedMessageXRightTex "__OTR__textures/message_texture_static/gRedMessageXRightTex"
#ifdef _WIN32
static const __declspec(align(2)) char gRedMessageXRightTex[] = dgRedMessageXRightTex;
#else
static const char gRedMessageXRightTex[] __attribute__((aligned (2))) = dgRedMessageXRightTex;
#endif
            


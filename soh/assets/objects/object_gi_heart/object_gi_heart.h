#pragma once
#define dgGiRecoveryHeartDL "__OTR__objects/object_gi_heart/gGiRecoveryHeartDL"
#ifdef _WIN32
static const __declspec(align(2)) char gGiRecoveryHeartDL[] = dgGiRecoveryHeartDL;
#else
static const char gGiRecoveryHeartDL[] __attribute__((aligned (2))) = dgGiRecoveryHeartDL;
#endif
            


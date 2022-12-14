#include "ultra64.h"
#include "global.h"

struct LinkPuppet;

typedef struct LinkPuppet {
    Actor actor;
    SkelAnime linkSkeleton;
    ColliderCylinder collider;
    PuppetPacketZ64 puppetPacket;
    ColliderQuad swordQuads[2];
    ColliderQuad shieldQuad;
    uint8_t damageTimer;
    uint8_t initialized;
    DamagePacketZ64 damagePacket;
    uint8_t ocarina_playing;
} LinkPuppet;
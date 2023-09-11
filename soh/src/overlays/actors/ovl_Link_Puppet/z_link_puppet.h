#ifdef ENABLE_REMOTE_CONTROL
#ifndef Z_LINK_PUPPET_H
#define Z_LINK_PUPPET_H

#include <libultraship/libultra.h>
#include "soh/Enhancements/nametag.h"
#include "global.h"

struct LinkPuppet;

typedef struct LinkPuppet {
    Actor actor;
    SkelAnime linkSkeleton;
    ColliderCylinder collider;
    ColliderQuad swordQuads[2];
    ColliderQuad shieldQuad;
    uint8_t damageTimer;
    uint8_t puppetAge;
    NameTagOptions nameTagOptions;
    Vec3f leftHandPos;
} LinkPuppet;

#ifdef __cplusplus
extern "C" {
#endif
void LinkPuppet_Init(Actor* thisx, PlayState* play);
void LinkPuppet_Destroy(Actor* thisx, PlayState* play);
void LinkPuppet_Update(Actor* thisx, PlayState* play);
void LinkPuppet_Draw(Actor* thisx, PlayState* play);
#ifdef __cplusplus
}
#endif

#endif
#endif

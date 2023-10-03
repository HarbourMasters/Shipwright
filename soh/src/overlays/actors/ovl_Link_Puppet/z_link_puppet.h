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

typedef enum {
    PUPPET_DMGEFF_NONE,
    PUPPET_DMGEFF_NORMAL,
    PUPPET_DMGEFF_ICE,
    PUPPET_DMGEFF_FIRE,
    PUPPET_DMGEFF_THUNDER,
    PUPPET_DMGEFF_KNOCKBACK,
    PUPPET_DMGEFF_STUN,
} PuppetDamageEffect;

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

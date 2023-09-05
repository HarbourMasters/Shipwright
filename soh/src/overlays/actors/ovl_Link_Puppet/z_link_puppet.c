#ifdef ENABLE_REMOTE_CONTROL

#include "vt.h"
#include "z_link_puppet.h"
#include <objects/gameplay_keep/gameplay_keep.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Anchor.h"
#include <string.h>

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void LinkPuppet_Init(Actor* thisx, PlayState* play);
void LinkPuppet_Destroy(Actor* thisx, PlayState* play);
void LinkPuppet_Update(Actor* thisx, PlayState* play);
void LinkPuppet_Draw(Actor* thisx, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK1,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 50, 0, { 0, 0, 0 } },
};

typedef enum {
    PUPPET_DMGEFF_NONE,
    PUPPET_DMGEFF_NORMAL,
    PUPPET_DMGEFF_ICE,
    PUPPET_DMGEFF_FIRE,
    PUPPET_DMGEFF_THUNDER,
    PUPPET_DMGEFF_KNOCKBACK,
    PUPPET_DMGEFF_STUN,
} PuppetDamageEffect;

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, PUPPET_DMGEFF_KNOCKBACK),
    /* Hookshot      */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, PUPPET_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, PUPPET_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, PUPPET_DMGEFF_THUNDER),
    /* Unk arrow 1   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(2, PUPPET_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, PUPPET_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(3, PUPPET_DMGEFF_THUNDER),
    /* Shield        */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, PUPPET_DMGEFF_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, PUPPET_DMGEFF_KNOCKBACK),
    /* Unknown 2     */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
};

void LinkPuppet_Init(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    this->actor.room = -1;
    this->actor.targetMode = 1;

    s32 playerAge = Anchor_GetClientPlayerData(this->actor.params - 3).playerAge;

    this->puppetAge = playerAge;

    SkelAnime_InitLink(play, &this->linkSkeleton, gPlayerSkelHeaders[((void)0, playerAge)],
           gPlayerAnim_link_normal_wait, 9, this->linkSkeleton.jointTable, this->linkSkeleton.morphTable,
           PLAYER_LIMB_MAX);

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.colChkInfo.damageTable = sDamageTable;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    NameTag_RemoveAllForActor(thisx);

    Color_RGB8 clientColor = Anchor_GetClientColor(this->actor.params - 3);
    Color_RGBA8 nameTagColor = { clientColor.r, clientColor.g, clientColor.b, 255 };
    const char* playerName = Anchor_GetClientName(this->actor.params - 3);
    this->nameTagOptions.yOffset = playerData.playerAge == LINK_AGE_ADULT ? 56.0f : 32.0f;
    this->nameTagOptions.textColor = nameTagColor;
    NameTag_RegisterForActorWithOptions(&this->actor, playerName, this->nameTagOptions);
}

void LinkPuppet_Destroy(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void LinkPuppet_Update(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 60.0f, 0x1D);

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    if (this->puppetAge != playerData.playerAge) {
        LinkPuppet_Init(this, play);
        return;
    }

    this->actor.shape.yOffset = playerData.yOffset;

    if (this->damageTimer > 0) {
        this->damageTimer--;
    }

    if (playerData.damageEffect > 0 && GET_PLAYER(play)->invincibilityTimer <= 0 &&
        !Player_InBlockingCsMode(play, GET_PLAYER(play))) {
        if (playerData.damageEffect == PUPPET_DMGEFF_NORMAL) {
            Player_InflictDamage(play, playerData.damageValue * -4);
            func_80837C0C(play, GET_PLAYER(play), 0, 0, 0, 0, 0);
            GET_PLAYER(play)->invincibilityTimer = 18;
            GET_PLAYER(play)->actor.freezeTimer = 0;
        } else if (playerData.damageEffect == PUPPET_DMGEFF_ICE) {
            GET_PLAYER(play)->stateFlags1 &= ~(PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_ITEM_OVER_HEAD);
            func_80837C0C(play, GET_PLAYER(play), 3, 0.0f, 0.0f, 0, 20);
            GET_PLAYER(play)->invincibilityTimer = 18;
            GET_PLAYER(play)->actor.freezeTimer = 0;
        } else if (playerData.damageEffect == PUPPET_DMGEFF_FIRE) {
            for (int i = 0; i < 18; i++) {
                GET_PLAYER(play)->flameTimers[i] = Rand_S16Offset(0, 200);
            }
            GET_PLAYER(play)->isBurning = true;
            func_80837C0C(play, GET_PLAYER(play), 0, 0, 0, 0, 0);
            GET_PLAYER(play)->invincibilityTimer = 18;
            GET_PLAYER(play)->actor.freezeTimer = 0;
        } else if (playerData.damageEffect == PUPPET_DMGEFF_THUNDER) {
            func_80837C0C(play, GET_PLAYER(play), 4, 0.0f, 0.0f, 0, 20);
            GET_PLAYER(play)->invincibilityTimer = 18;
            GET_PLAYER(play)->actor.freezeTimer = 0;
        } else if (playerData.damageEffect == PUPPET_DMGEFF_KNOCKBACK) {
            func_8002F71C(play, &this->actor, 100.0f * 0.04f + 4.0f, this->actor.world.rot.y, 8.0f);
            GET_PLAYER(play)->invincibilityTimer = 28;
            GET_PLAYER(play)->actor.freezeTimer = 0;
        } else if (playerData.damageEffect == PUPPET_DMGEFF_STUN) {
            GET_PLAYER(play)->actor.freezeTimer = 20;
            Actor_SetColorFilter(&GET_PLAYER(play)->actor, 0, 0xFF, 0, 10);
        }
    }

    if (this->collider.base.acFlags & AC_HIT && this->damageTimer <= 0) {
        this->collider.base.acFlags &= ~AC_HIT;
        gSaveContext.playerData.damageEffect = this->actor.colChkInfo.damageEffect;
        gSaveContext.playerData.damageValue = this->actor.colChkInfo.damage;

        if (gSaveContext.playerData.damageEffect == PUPPET_DMGEFF_STUN) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Actor_SetColorFilter(&this->actor, 0, 0xFF, 0, 40);
        } else if (gSaveContext.playerData.damageEffect != PUPPET_DMGEFF_NONE) {
            Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 24);
        }

        this->damageTimer = 18;
    }

    Collider_UpdateCylinder(thisx, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    if (this->actor.params >= 3) {
        if (Anchor_GetClientScene(this->actor.params - 3) == play->sceneNum) {
            PosRot playerPosRot = Anchor_GetClientPosition(this->actor.params - 3);
            this->actor.world.pos = playerPosRot.pos;
            this->actor.shape.rot = playerPosRot.rot;
        } else {
            this->actor.world.pos.x = -9999.0f;
            this->actor.world.pos.y = -9999.0f;
            this->actor.world.pos.z = -9999.0f;
        }
    }

    Vec3s* jointTable = Anchor_GetClientJointTable(this->actor.params - 3);

    this->linkSkeleton.jointTable = jointTable;

    if (playerData.playerSound != 0) {
        Audio_PlaySoundGeneral(playerData.playerSound, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

Vec3f FEET_POS[] = {
    { 200.0f, 300.0f, 0.0f },
    { 200.0f, 200.0f, 0.0f },
};

void Puppet_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    Vec3f* vec = &FEET_POS[((void)0, playerData.playerAge)];
    Actor_SetFeetPos(&this->actor, limbIndex, PLAYER_LIMB_L_FOOT, vec, PLAYER_LIMB_R_FOOT, vec);
}

void LinkPuppet_Draw(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    if (this->puppetAge == playerData.playerAge) {
        DrawAnchorPuppet(play, this->linkSkeleton.skeleton, this->linkSkeleton.jointTable,
                         this->linkSkeleton.dListCount, 0, playerData.tunicType, playerData.bootsType,
                         playerData.faceType, PuppetOverrideDraw, Puppet_PostLimbDraw, this, playerData);
    }
}
#endif
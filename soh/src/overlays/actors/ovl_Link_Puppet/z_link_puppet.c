#ifdef ENABLE_REMOTE_CONTROL

#include "vt.h"
#include "z_link_puppet.h"
#include <objects/gameplay_keep/gameplay_keep.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Anchor.h"
#include <string.h>
#include <objects/object_link_child/object_link_child.h>

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
    const char* playerName = Anchor_GetClientName(this->actor.params - 3);
    this->nameTagOptions.yOffset = 0;
    NameTag_RegisterForActorWithOptions(&this->actor, playerName, this->nameTagOptions);
}

void LinkPuppet_Destroy(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void LinkPuppet_Update(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    if (this->puppetAge != playerData.playerAge) {
        LinkPuppet_Init(this, play);
        return;
    }

    this->actor.shape.yOffset = playerData.yOffset;

    if (this->damageTimer > 0) {
        this->damageTimer--;
    }

    if (this->collider.base.acFlags & AC_HIT && this->damageTimer <= 0) {
        this->collider.base.acFlags &= ~AC_HIT;
        gSaveContext.playerData.damageEffect = this->actor.colChkInfo.damageEffect;
        gSaveContext.playerData.damageValue = this->actor.colChkInfo.damage;
        Anchor_DamagePlayer(this->actor.params - 3, this->actor.colChkInfo.damageEffect, this->actor.colChkInfo.damage);

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

Vec3f D_808547A4 = { 0.0f, 0.5f, 0.0f };
Vec3f D_808547B0 = { 0.0f, 0.5f, 0.0f };

Color_RGBA8 D_808547BC = { 255, 255, 100, 255 };
Color_RGBA8 D_808547C0 = { 255, 50, 0, 0 };

void Puppet_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    Vec3f* vec = &FEET_POS[((void)0, playerData.playerAge)];
    Actor_SetFeetPos(&this->actor, limbIndex, PLAYER_LIMB_L_FOOT, vec, PLAYER_LIMB_R_FOOT, vec);

    if (limbIndex == PLAYER_LIMB_L_HAND && playerData.itemAction == PLAYER_IA_DEKU_STICK) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Translate(-428.26f, 267.2f, -33.82f, MTXMODE_APPLY);
        Matrix_RotateZYX(-0x8000, 0, 0x4000, MTXMODE_APPLY);
        Matrix_Scale(1.0f, playerData.unk_85C, 1.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gLinkChildLinkDekuStickDL);

        CLOSE_DISPS(play->state.gfxCtx);

        if (playerData.unk_860 != 0) {
            f32 temp = 1.0f;

            if (playerData.unk_860 > 200) {
                temp = (210 - playerData.unk_860) / 10.0f;
            } else if (playerData.unk_860 < 20) {
                temp = playerData.unk_860 / 20.0f;
            }

            func_8002836C(play, &playerData.stickWeaponTip, &D_808547A4, &D_808547B0, &D_808547BC, &D_808547C0,
                          temp * 200.0f, 0, 8);
        }
    }
}

void LinkPuppet_Draw(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    PlayerData playerData = Anchor_GetClientPlayerData(this->actor.params - 3);

    if (this->puppetAge == playerData.playerAge) {
        DrawAnchorPuppet(play, this->linkSkeleton.skeleton, this->linkSkeleton.jointTable,
                         this->linkSkeleton.dListCount, 0, playerData.tunicType, playerData.bootsType,
                         playerData.faceType, PuppetOverrideDraw, Puppet_PostLimbDraw, this, playerData, this->actor.params - 3);

        if ((playerData.unk_862 - 1) > -1) {
            OPEN_DISPS(play->state.gfxCtx);

            Matrix_Translate(this->leftHandPos.x + (3.3f * Math_SinS(this->actor.shape.rot.y)), this->leftHandPos.y,
                             this->leftHandPos.z + ((3.3f + (IREG(90) / 10.0f)) * Math_CosS(this->actor.shape.rot.y)),
                             MTXMODE_NEW);
            Matrix_RotateZYX(0, play->gameplayFrames * 1000, 0, MTXMODE_APPLY);
            Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);

            GetItem_Draw(play, ABS(playerData.unk_862 - 1));

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}
#endif
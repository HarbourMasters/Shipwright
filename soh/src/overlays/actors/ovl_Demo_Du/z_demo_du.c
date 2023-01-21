#include "z_demo_du.h"
#include "objects/object_du/object_du.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

typedef void (*DemoDuActionFunc)(DemoDu*, PlayState*);
typedef void (*DemoDuDrawFunc)(Actor*, PlayState*);

void DemoDu_Init(Actor* thisx, PlayState* play);
void DemoDu_Destroy(Actor* thisx, PlayState* play);
void DemoDu_Update(Actor* thisx, PlayState* play);
void DemoDu_Draw(Actor* thisx, PlayState* play);
void DemoDu_Reset(void);

//static s32 sUnused = 0;

#include "z_demo_du_cutscene_data.c" EARLY

static void* sEyeTextures[] = { gDaruniaEyeOpenTex, gDaruniaEyeOpeningTex, gDaruniaEyeShutTex, gDaruniaEyeClosingTex };
static void* sMouthTextures[] = { gDaruniaMouthSeriousTex, gDaruniaMouthGrinningTex, gDaruniaMouthOpenTex,
                                  gDaruniaMouthHappyTex };

/**
 * Cs => Cutscene
 *
 * FM => Fire Medallion
 * GR => Goron's Ruby
 * AG => In the chamber of sages, just After the final blow on Ganon.
 * CR => Credits
 *
 */

// Each macro maps its argument to an index of sUpdateFuncs.
#define CS_FIREMEDALLION_SUBSCENE(x) (0 + (x))      // DEMO_DU_CS_FIREMEDALLION
#define CS_GORONSRUBY_SUBSCENE(x) (7 + (x))         // DEMO_DU_CS_GORONS_RUBY
#define CS_CHAMBERAFTERGANON_SUBSCENE(x) (21 + (x)) // DEMO_DU_CS_CHAMBER_AFTER_GANON
#define CS_CREDITS_SUBSCENE(x) (24 + (x))           // DEMO_DU_CS_CREDITS

void DemoDu_Destroy(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void DemoDu_UpdateEyes(DemoDu* this) {
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;
    s32 pad[3];

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void DemoDu_SetEyeTexIndex(DemoDu* this, s16 eyeTexIndex) {
    this->eyeTexIndex = eyeTexIndex;
}

void DemoDu_SetMouthTexIndex(DemoDu* this, s16 mouthTexIndex) {
    this->mouthTexIndex = mouthTexIndex;
}

// Resets all the values used in this cutscene.
void DemoDu_CsAfterGanon_Reset(DemoDu* this) {
    this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
    this->drawIndex = 0;
    this->shadowAlpha = 0;
    this->demo6KSpawned = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_1A4 = 0.0f;
}

s32 D_8096CE94 = false;
void DemoDu_CsAfterGanon_CheckIfShouldReset(DemoDu* this, PlayState* play) {

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_8096CE94) {
            if (this->actor.params == DEMO_DU_CS_CHAMBER_AFTER_GANON) {
                DemoDu_CsAfterGanon_Reset(this);
            }
            D_8096CE94 = false;
            return;
        }
    } else if (!D_8096CE94) {
        D_8096CE94 = true;
    }
}

s32 DemoDu_UpdateSkelAnime(DemoDu* this) {
    return SkelAnime_Update(&this->skelAnime);
}

void DemoDu_UpdateBgCheckInfo(DemoDu* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 5);
}

CsCmdActorAction* DemoDu_GetNpcAction(PlayState* play, s32 idx) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[idx];
    }
    return NULL;
}

s32 DemoDu_IsNpcDoingThisAction(DemoDu* this, PlayState* play, u16 action, s32 idx) {
    CsCmdActorAction* npcAction = DemoDu_GetNpcAction(play, idx);

    if ((npcAction != NULL) && (npcAction->action == action)) {
        return true;
    }
    return false;
}

s32 DemoDu_IsNpcNotDoingThisAction(DemoDu* this, PlayState* play, u16 action, s32 idx) {
    CsCmdActorAction* npcAction = DemoDu_GetNpcAction(play, idx);

    if ((npcAction != NULL) && (npcAction->action != action)) {
        return true;
    }
    return false;
}

void DemoDu_MoveToNpcPos(DemoDu* this, PlayState* play, s32 idx) {
    CsCmdActorAction* npcAction = DemoDu_GetNpcAction(play, idx);
    s32 pad;

    if (npcAction != NULL) {
        this->actor.world.pos.x = npcAction->startPos.x;
        this->actor.world.pos.y = npcAction->startPos.y;
        this->actor.world.pos.z = npcAction->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = npcAction->rot.y;
    }
}

void func_80969DDC(DemoDu* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 arg4) {
    f32 startFrame;
    s16 lastFrame = Animation_GetLastFrame(animation);
    f32 endFrame;
    f32 playSpeed;

    if (arg4 == 0) {
        startFrame = 0.0f;
        endFrame = lastFrame;
        playSpeed = 1.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.0f;
        startFrame = lastFrame;
    }
    Animation_Change(&this->skelAnime, animation, playSpeed, startFrame, endFrame, mode, morphFrames);
}

void DemoDu_InitCs_FireMedallion(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaIdleAnim, NULL, NULL, 0);
    this->actor.shape.yOffset = -10000.0f;
    DemoDu_SetEyeTexIndex(this, 1);
    DemoDu_SetMouthTexIndex(this, 3);
}

// A.k.a Warp portal
void DemoDu_CsFireMedallion_SpawnDoorWarp(DemoDu* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

// Gives the Fire Medallion to Link.
void func_80969F38(DemoDu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 80.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       DEMO_EFFECT_MEDAL_FIRE);
    Item_Give(play, ITEM_MEDALLION_FIRE);
}

void func_80969FB4(DemoDu* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

// Gives the Fire Medallion to Link too.
void DemoDu_CsFireMedallion_AdvanceTo01(DemoDu* this, PlayState* play) {
    s32 pad[2];

    if ((gSaveContext.chamberCutsceneNum == 1) && (gSaveContext.sceneSetupIndex < 4)) {
        Player* player = GET_PLAYER(play);

        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(1);
        play->csCtx.segment = D_8096C1A4;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_FIRE);

        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void DemoDu_CsFireMedallion_AdvanceTo02(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 1)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(2);
            this->drawIndex = 1;
            DemoDu_CsFireMedallion_SpawnDoorWarp(this, play);
        }
    }
}

void DemoDu_CsFireMedallion_AdvanceTo03(DemoDu* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(3);
        this->actor.shape.yOffset = 0.0f;
    }
}

void DemoDu_CsFireMedallion_AdvanceTo04(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 2)) {
            Animation_Change(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(4);
        }
    }
}

void DemoDu_CsFireMedallion_AdvanceTo05(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_FIREMEDALLION_SUBSCENE(5);
    }
}

void DemoDu_CsFireMedallion_AdvanceTo06(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[6];

        if ((npcAction != NULL) && (npcAction->action == 2)) {
            this->updateIndex = CS_FIREMEDALLION_SUBSCENE(6);
            func_80969F38(this, play);
        }
    }
}

void DemoDu_UpdateCs_FM_00(DemoDu* this, PlayState* play) {
    DemoDu_CsFireMedallion_AdvanceTo01(this, play);
}

void DemoDu_UpdateCs_FM_01(DemoDu* this, PlayState* play) {
    DemoDu_CsFireMedallion_AdvanceTo02(this, play);
}

void DemoDu_UpdateCs_FM_02(DemoDu* this, PlayState* play) {
    func_80969FB4(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo03(this);
}

void DemoDu_UpdateCs_FM_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo04(this, play);
}

void DemoDu_UpdateCs_FM_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo05(this, animFinished);
}

void DemoDu_UpdateCs_FM_05(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsFireMedallion_AdvanceTo06(this, play);
}

void DemoDu_UpdateCs_FM_06(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
}

void DemoDu_InitCs_GoronsRuby(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(0);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia lands at the floor at the start of the cutscene.
void DemoDu_CsPlaySfx_GoronLanding(DemoDu* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_EN_GOLON_LAND_BIG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Darunia is falling at the start of the cutscene.
void DemoDu_CsPlaySfx_DaruniaFalling(PlayState* play) {
    if (play->csCtx.frames == 160) {
        func_800788CC(NA_SE_EV_OBJECT_FALL);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void DemoDu_CsPlaySfx_DaruniaHitsLink(PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    func_80078914(&player->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_LINK);
    Audio_PlaySoundGeneral(NA_SE_VO_LI_DAMAGE_S_KID, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
void DemoDu_CsPlaySfx_HitBreast(DemoDu* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_EN_DARUNIA_HIT_BREAST - SFX_FLAG);
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is escaping from the gorons at the end of the scene.
void DemoDu_CsPlaySfx_LinkEscapeFromGorons(PlayState* play) {
    if (play->csCtx.frames == 1400) {
        Player* player = GET_PLAYER(play);

        Audio_PlaySoundGeneral(NA_SE_VO_LI_FALL_L_KID, &player->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

// Cutscene: Darunia gives Link the Goron's Ruby.
// Sfx played when Link is surprised by Darunia falling from the sky.
void DemoDu_CsPlaySfx_LinkSurprised(PlayState* play) {
    if (play->csCtx.frames == 174) {
        Player* player = GET_PLAYER(play);

        Audio_PlaySoundGeneral(NA_SE_VO_LI_SURPRISE_KID, &player->actor.projectedPos, 4U, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void DemoDu_CsGoronsRuby_UpdateFaceTextures(DemoDu* this, PlayState* play) {
    u16* frames = &play->csCtx.frames;

    if (*frames < 260) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 0);
    } else if (*frames < 335) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 3);
    } else if (*frames < 365) {
        DemoDu_SetEyeTexIndex(this, 3);
        DemoDu_SetMouthTexIndex(this, 1);
    } else if (*frames < 395) {
        DemoDu_SetEyeTexIndex(this, 0);
        DemoDu_SetMouthTexIndex(this, 3);
    } else if (*frames < 410) {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 0);
    } else {
        DemoDu_UpdateEyes(this);
        DemoDu_SetMouthTexIndex(this, 3);
    }
}

void func_8096A630(DemoDu* this, PlayState* play) {
    s32 pad;
    Vec3f pos = this->actor.world.pos;

    pos.y += kREG(5);
    func_80033480(play, &pos, kREG(1) + 100.0f, kREG(2) + 10, kREG(3) + 300, kREG(4), 0);
    DemoDu_CsPlaySfx_GoronLanding(this);
}

void DemoDu_CsGoronsRuby_SpawnDustWhenHittingLink(DemoDu* this, PlayState* play) {
    static Vec3f dustPosOffsets[] = {
        { 11.0f, -11.0f, -6.0f }, { 0.0f, 14.0f, -13.0f },  { 14.0f, -2.0f, -10.0f }, { 10.0f, -6.0f, -8.0f },
        { 8.0f, 6.0f, 8.0f },     { 13.0f, 8.0f, -10.0f },  { -14.0f, 1.0f, -14.0f }, { 5.0f, 12.0f, -9.0f },
        { 11.0f, 6.0f, -7.0f },   { 14.0f, 14.0f, -14.0f },
    };

    if (Animation_OnFrame(&this->skelAnime, 31.0f) || Animation_OnFrame(&this->skelAnime, 41.0f)) {
        s32 pad[2];
        s32 i;
        Player* player = GET_PLAYER(play);
        Vec3f* headPos = &player->bodyPartsPos[PLAYER_LIMB_HEAD];
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.3f, 0.0f };
        s32 pad2;

        for (i = 4; i >= 0; --i) {
            Color_RGBA8 primColor = { 190, 150, 110, 255 };
            Color_RGBA8 envColor = { 120, 80, 40, 255 };
            s32 colorDelta;
            Vec3f position;

            if (Animation_OnFrame(&this->skelAnime, 31.0f)) {
                position.x = dustPosOffsets[i + 5].x + headPos->x;
                position.y = dustPosOffsets[i + 5].y + headPos->y;
                position.z = dustPosOffsets[i + 5].z + headPos->z;
            } else {
                position.x = dustPosOffsets[i + 0].x + headPos->x;
                position.y = dustPosOffsets[i + 0].y + headPos->y;
                position.z = dustPosOffsets[i + 0].z + headPos->z;
            }

            colorDelta = Rand_ZeroOne() * 20.0f - 10.0f;

            primColor.r += colorDelta;
            primColor.g += colorDelta;
            primColor.b += colorDelta;
            envColor.r += colorDelta;
            envColor.g += colorDelta;
            envColor.b += colorDelta;

            func_8002829C(play, &position, &velocity, &accel, &primColor, &envColor,
                          Rand_ZeroOne() * 40.0f + 200.0f, 0);
        }

        DemoDu_CsPlaySfx_DaruniaHitsLink(play);
    }
}

void DemoDu_CsGoronsRuby_DaruniaFalling(DemoDu* this, PlayState* play) {
    s32 pad;
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = csCtx->npcActions[2];
        Vec3f startPos;
        Vec3f endPos;
        Vec3f* pos = &this->actor.world.pos;

        if (npcAction != NULL) {
            f32 traveledPercent = Environment_LerpWeight(npcAction->endFrame, npcAction->startFrame, csCtx->frames);

            startPos.x = npcAction->startPos.x;
            startPos.y = npcAction->startPos.y;
            startPos.z = npcAction->startPos.z;

            endPos.x = npcAction->endPos.x;
            endPos.y = npcAction->endPos.y;
            endPos.z = npcAction->endPos.z;

            pos->x = ((endPos.x - startPos.x) * traveledPercent) + startPos.x;
            pos->y = ((endPos.y - startPos.y) * traveledPercent) + startPos.y;
            pos->z = ((endPos.z - startPos.z) * traveledPercent) + startPos.z;
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo01(DemoDu* this, PlayState* play) {
    this->updateIndex = CS_GORONSRUBY_SUBSCENE(1);
}

void DemoDu_CsGoronsRuby_AdvanceTo02(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 1)) {
            Animation_Change(&this->skelAnime, &gDaruniaStandUpAfterFallingAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaStandUpAfterFallingAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(2);
            this->drawIndex = 1;
            DemoDu_CsGoronsRuby_DaruniaFalling(this, play);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo03(DemoDu* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = csCtx->npcActions[2];

        if ((npcAction != NULL) && (csCtx->frames >= npcAction->endFrame)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(3);
            func_8096A630(this, play);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo04(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 2)) {
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(4);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo05(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(5);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo06(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 3)) {
            Animation_Change(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaHitBreastAnim), 2, -4.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(6);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo07(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(7);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo08(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 4)) {
            Animation_Change(&this->skelAnime, &gDaruniaHitLinkAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaHitLinkAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(8);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo09(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaHitBreastAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaHitBreastAnim), 2, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(9);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo10(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                         ANIMMODE_LOOP, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(10);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo11(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 5)) {
            Animation_Change(&this->skelAnime, &gDaruniaItemGiveAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gDaruniaItemGiveAnim), 2, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(11);
        }
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo12(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        Animation_Change(&this->skelAnime, &gDaruniaItemGiveIdleAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDaruniaItemGiveIdleAnim), 0, 0.0f);
        this->updateIndex = CS_GORONSRUBY_SUBSCENE(12);
    }
}

void DemoDu_CsGoronsRuby_AdvanceTo13(DemoDu* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorAction* npcAction = play->csCtx.npcActions[2];

        if ((npcAction != NULL) && (npcAction->action != 6)) {
            Animation_Change(&this->skelAnime, &gDaruniaIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gDaruniaIdleAnim),
                             ANIMMODE_LOOP, 0.0f);
            this->updateIndex = CS_GORONSRUBY_SUBSCENE(13);
        }
    }
}

void DemoDu_UpdateCs_GR_00(DemoDu* this, PlayState* play) {
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsGoronsRuby_AdvanceTo01(this, play);
}

void DemoDu_UpdateCs_GR_01(DemoDu* this, PlayState* play) {
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo02(this, play);
}

void DemoDu_UpdateCs_GR_02(DemoDu* this, PlayState* play) {
    DemoDu_CsGoronsRuby_DaruniaFalling(this, play);
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_CsPlaySfx_DaruniaFalling(play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo03(this, play);
}

void DemoDu_UpdateCs_GR_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_CsPlaySfx_LinkSurprised(play);
    DemoDu_CsGoronsRuby_AdvanceTo04(this, play);
}

void DemoDu_UpdateCs_GR_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo05(this, animFinished);
}

void DemoDu_UpdateCs_GR_05(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo06(this, play);
}

void DemoDu_UpdateCs_GR_06(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsPlaySfx_HitBreast(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo07(this, animFinished);
}

void DemoDu_UpdateCs_GR_07(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo08(this, play);
}

void DemoDu_UpdateCs_GR_08(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_SpawnDustWhenHittingLink(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo09(this, animFinished);
}

void DemoDu_UpdateCs_GR_09(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsPlaySfx_HitBreast(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo10(this, animFinished);
}

void DemoDu_UpdateCs_GR_10(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo11(this, play);
}

void DemoDu_UpdateCs_GR_11(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo12(this, animFinished);
}

void DemoDu_UpdateCs_GR_12(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsGoronsRuby_AdvanceTo13(this, play);
}

void DemoDu_UpdateCs_GR_13(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_CsGoronsRuby_UpdateFaceTextures(this, play);
    DemoDu_CsPlaySfx_LinkEscapeFromGorons(play);
}

void DemoDu_InitCs_AfterGanon(DemoDu* this, PlayState* play) {
    s32 pad[3];
    f32 lastFrame = Animation_GetLastFrame(&gDaruniaSageFormationAnim);

    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, NULL, NULL, NULL, 0);
    Animation_Change(&this->skelAnime, &gDaruniaSageFormationAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
    this->actor.shape.shadowAlpha = 0;
}

void DemoDu_CsPlaySfx_WhiteOut() {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void DemoDu_CsAfterGanon_SpawnDemo6K(DemoDu* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(16) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 3);
}

void DemoDu_CsAfterGanon_AdvanceTo01(DemoDu* this, PlayState* play) {
    if (DemoDu_IsNpcDoingThisAction(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->shadowAlpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1A4 = 0.0f;
        DemoDu_CsPlaySfx_WhiteOut();
    }
}

void DemoDu_CsAfterGanon_AdvanceTo02(DemoDu* this, PlayState* play) {
    f32* unk_1A4 = &this->unk_1A4;
    s32 shadowAlpha = 255;

    if (DemoDu_IsNpcDoingThisAction(this, play, 4, 2)) {
        *unk_1A4 += 1.0f;
        if (*unk_1A4 >= kREG(5) + 10.0f) {
            this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(2);
            this->drawIndex = 1;
            *unk_1A4 = kREG(5) + 10.0f;
            this->shadowAlpha = shadowAlpha;
            this->actor.shape.shadowAlpha = shadowAlpha;
            return;
        }
    } else {
        *unk_1A4 -= 1.0f;
        if (*unk_1A4 <= 0.0f) {
            this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
            this->drawIndex = 0;
            *unk_1A4 = 0.0f;
            this->shadowAlpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    shadowAlpha = (*unk_1A4 / (kREG(5) + 10.0f)) * 255.0f;
    this->shadowAlpha = shadowAlpha;
    this->actor.shape.shadowAlpha = shadowAlpha;
}

void DemoDu_CsAfterGanon_BackTo01(DemoDu* this, PlayState* play) {
    if (DemoDu_IsNpcNotDoingThisAction(this, play, 4, 2)) {
        this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(1);
        this->drawIndex = 2;
        this->unk_1A4 = kREG(5) + 10.0f;
        this->shadowAlpha = 255;
        if (!this->demo6KSpawned) {
            DemoDu_CsAfterGanon_SpawnDemo6K(this, play);
            this->demo6KSpawned = 1;
        }
        this->actor.shape.shadowAlpha = 255;
    }
}

void DemoDu_UpdateCs_AG_00(DemoDu* this, PlayState* play) {
    DemoDu_CsAfterGanon_AdvanceTo01(this, play);
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
}

void DemoDu_UpdateCs_AG_01(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsAfterGanon_AdvanceTo02(this, play);
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
}

void DemoDu_UpdateCs_AG_02(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsAfterGanon_BackTo01(this, play);
    DemoDu_CsAfterGanon_CheckIfShouldReset(this, play);
}

// Similar to DemoDu_Draw_01, but this uses POLY_XLU_DISP. Also uses this->shadowAlpha for setting the env color.
void DemoDu_Draw_02(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoDu* this = (DemoDu*)thisx;
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = sEyeTextures[eyeTexIndex];
    s32 pad;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTexture = sMouthTextures[mouthTexIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTexture));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gDaruniaNoseSeriousTex));

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->shadowAlpha);

    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, 0,
                                       0, 0, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void DemoDu_InitCs_Credits(DemoDu* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gDaruniaSkel, &gDaruniaCreditsIdleAnim, NULL, NULL, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(0);
    this->drawIndex = 0;
    this->actor.shape.shadowAlpha = 0;
    DemoDu_SetMouthTexIndex(this, 3);
}

void DemoDu_CsCredits_UpdateShadowAlpha(DemoDu* this) {
    s32 shadowAlpha = 255;
    f32 temp_f0;
    f32* unk_1A4;

    this->unk_1A4 += 1.0f;
    temp_f0 = kREG(17) + 10.0f;
    unk_1A4 = &this->unk_1A4;

    if (temp_f0 <= *unk_1A4) {
        this->shadowAlpha = shadowAlpha;
        this->actor.shape.shadowAlpha = shadowAlpha;
    } else {
        shadowAlpha = *unk_1A4 / temp_f0 * 255.0f;
        this->shadowAlpha = shadowAlpha;
        this->actor.shape.shadowAlpha = shadowAlpha;
    }
}

void DemoDu_CsCredits_AdvanceTo01(DemoDu* this, PlayState* play) {
    DemoDu_MoveToNpcPos(this, play, 2);
    this->updateIndex = CS_CREDITS_SUBSCENE(1);
    this->drawIndex = 2;
}

void DemoDu_CsCredits_AdvanceTo02(DemoDu* this) {
    if (this->unk_1A4 >= kREG(17) + 10.0f) {
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
        this->drawIndex = 1;
    }
}

void DemoDu_CsCredits_AdvanceTo03(DemoDu* this) {
    func_80969DDC(this, &gDaruniaLookingUpToSariaAnim, ANIMMODE_ONCE, -8.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(3);
}

void DemoDu_CsCredits_AdvanceTo04(DemoDu* this) {
    func_80969DDC(this, &gDaruniaCreditsHitBreastAnim, ANIMMODE_ONCE, 0.0f, 0);
    this->updateIndex = CS_CREDITS_SUBSCENE(4);
}

void DemoDu_CsCredits_BackTo02(DemoDu* this, s32 animFinished) {
    if (animFinished) {
        func_80969DDC(this, &gDaruniaCreditsIdleAnim, ANIMMODE_LOOP, 0.0f, 0);
        this->updateIndex = CS_CREDITS_SUBSCENE(2);
    }
}

void DemoDu_CsCredits_HandleSubscenesByNpcAction(DemoDu* this, PlayState* play) {
    CsCmdActorAction* npcAction = DemoDu_GetNpcAction(play, 2);

    if (npcAction != NULL) {
        s32 action = npcAction->action;
        s32 lastAction = this->lastAction;

        if (action != lastAction) {
            switch (action) {
                case 9:
                    DemoDu_CsCredits_AdvanceTo01(this, play);
                    break;
                case 10:
                    DemoDu_CsCredits_AdvanceTo03(this);
                    break;
                case 11:
                    DemoDu_CsCredits_AdvanceTo04(this);
                    break;
                default:
                    // "Demo_Du_inEnding_Check_DemoMode:There is no such operation!!!!!!!!"
                    osSyncPrintf("Demo_Du_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->lastAction = action;
        }
    }
}

void DemoDu_UpdateCs_CR_00(DemoDu* this, PlayState* play) {
    DemoDu_CsCredits_HandleSubscenesByNpcAction(this, play);
}

void DemoDu_UpdateCs_CR_01(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_UpdateShadowAlpha(this);
    DemoDu_CsCredits_AdvanceTo02(this);
}

void DemoDu_UpdateCs_CR_02(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_HandleSubscenesByNpcAction(this, play);
}

void DemoDu_UpdateCs_CR_03(DemoDu* this, PlayState* play) {
    DemoDu_UpdateBgCheckInfo(this, play);
    DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_HandleSubscenesByNpcAction(this, play);
}

void DemoDu_UpdateCs_CR_04(DemoDu* this, PlayState* play) {
    s32 animFinished;

    DemoDu_UpdateBgCheckInfo(this, play);
    animFinished = DemoDu_UpdateSkelAnime(this);
    DemoDu_UpdateEyes(this);
    DemoDu_CsCredits_BackTo02(this, animFinished);
}

static DemoDuActionFunc sUpdateFuncs[] = {
    DemoDu_UpdateCs_FM_00, DemoDu_UpdateCs_FM_01, DemoDu_UpdateCs_FM_02, DemoDu_UpdateCs_FM_03, DemoDu_UpdateCs_FM_04,
    DemoDu_UpdateCs_FM_05, DemoDu_UpdateCs_FM_06, DemoDu_UpdateCs_GR_00, DemoDu_UpdateCs_GR_01, DemoDu_UpdateCs_GR_02,
    DemoDu_UpdateCs_GR_03, DemoDu_UpdateCs_GR_04, DemoDu_UpdateCs_GR_05, DemoDu_UpdateCs_GR_06, DemoDu_UpdateCs_GR_07,
    DemoDu_UpdateCs_GR_08, DemoDu_UpdateCs_GR_09, DemoDu_UpdateCs_GR_10, DemoDu_UpdateCs_GR_11, DemoDu_UpdateCs_GR_12,
    DemoDu_UpdateCs_GR_13, DemoDu_UpdateCs_AG_00, DemoDu_UpdateCs_AG_01, DemoDu_UpdateCs_AG_02, DemoDu_UpdateCs_CR_00,
    DemoDu_UpdateCs_CR_01, DemoDu_UpdateCs_CR_02, DemoDu_UpdateCs_CR_03, DemoDu_UpdateCs_CR_04,
};

void DemoDu_Update(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    if (this->updateIndex < 0 || this->updateIndex >= 29 || sUpdateFuncs[this->updateIndex] == NULL) {
        // "The main mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sUpdateFuncs[this->updateIndex](this, play);
}

void DemoDu_Init(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    switch (this->actor.params) {
        case DEMO_DU_CS_GORONS_RUBY:
            DemoDu_InitCs_GoronsRuby(this, play);
            break;

        case DEMO_DU_CS_CHAMBER_AFTER_GANON:
            DemoDu_InitCs_AfterGanon(this, play);
            break;

        case DEMO_DU_CS_CREDITS:
            DemoDu_InitCs_Credits(this, play);
            break;

        default:
            DemoDu_InitCs_FireMedallion(this, play);
            break;
    }
}

void DemoDu_Draw_NoDraw(Actor* thisx, PlayState* play2) {
}

// Similar to DemoDu_Draw_02, but this uses POLY_OPA_DISP. Sets the env color to 255.
void DemoDu_Draw_01(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoDu* this = (DemoDu*)thisx;
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = sEyeTextures[eyeTexIndex];
    s32 pad;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTexture = sMouthTextures[mouthTexIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTexture));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gDaruniaNoseSeriousTex));

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          this);

    CLOSE_DISPS(play->state.gfxCtx);
}

static DemoDuDrawFunc sDrawFuncs[] = {
    DemoDu_Draw_NoDraw,
    DemoDu_Draw_01,
    DemoDu_Draw_02,
};

void DemoDu_Draw(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    if (this->drawIndex < 0 || this->drawIndex >= 3 || sDrawFuncs[this->drawIndex] == NULL) {
        // "The drawing mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawIndex](thisx, play);
}

const ActorInit Demo_Du_InitVars = {
    ACTOR_DEMO_DU,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_DU,
    sizeof(DemoDu),
    (ActorFunc)DemoDu_Init,
    (ActorFunc)DemoDu_Destroy,
    (ActorFunc)DemoDu_Update,
    (ActorFunc)DemoDu_Draw,
    (ActorResetFunc)DemoDu_Reset,
};

void DemoDu_Reset(void) {
    D_8096CE94 = 0;
}
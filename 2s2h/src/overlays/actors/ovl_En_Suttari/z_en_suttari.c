/*
 * File: z_en_suttari.c
 * Overlay: ovl_En_Suttari
 * Description: Sakon
 */

#include "z_en_suttari.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnSuttari_Init(Actor* thisx, PlayState* play);
void EnSuttari_Destroy(Actor* thisx, PlayState* play);
void EnSuttari_Update(Actor* thisx, PlayState* play);
void EnSuttari_Draw(Actor* thisx, PlayState* play);

void func_80BACA14(EnSuttari* this, PlayState* play);
void func_80BACEE0(EnSuttari* this, PlayState* play);
void func_80BAD004(EnSuttari* this, PlayState* play);
void func_80BAD5F8(EnSuttari* this, PlayState* play);
void func_80BAD7F8(EnSuttari* this, PlayState* play);
void func_80BAD380(EnSuttari* this, PlayState* play);
void func_80BACBB0(EnSuttari* this, PlayState* play);
void func_80BADA9C(EnSuttari* this, PlayState* play);
void func_80BADDB4(EnSuttari* this, PlayState* play);
void func_80BAD130(EnSuttari* this, PlayState* play);
void func_80BACD2C(EnSuttari* this, PlayState* play);
void func_80BACE4C(EnSuttari* this, PlayState* play);
void func_80BADD0C(EnSuttari* this, PlayState* play);
void func_80BADA08(EnSuttari* this, PlayState* play);
void func_80BAD2B4(EnSuttari* this, PlayState* play);
void func_80BADE14(EnSuttari* this, PlayState* play);
void func_80BADE8C(EnSuttari* this, PlayState* play);
void func_80BADF3C(EnSuttari* this, PlayState* play);

ActorProfile En_Suttari_Profile = {
    /**/ ACTOR_EN_SUTTARI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BOJ,
    /**/ sizeof(EnSuttari),
    /**/ EnSuttari_Init,
    /**/ EnSuttari_Destroy,
    /**/ EnSuttari_Update,
    /**/ EnSuttari_Draw,
};

typedef enum SuttariAnimation {
    /* -1 */ SUTTARI_ANIM_NONE = -1,
    /*  0 */ SUTTARI_ANIM_0,
    /*  1 */ SUTTARI_ANIM_1,
    /*  2 */ SUTTARI_ANIM_2,
    /*  3 */ SUTTARI_ANIM_3,
    /*  4 */ SUTTARI_ANIM_4,
    /*  5 */ SUTTARI_ANIM_5,
    /*  6 */ SUTTARI_ANIM_6,
    /*  7 */ SUTTARI_ANIM_7,
    /*  8 */ SUTTARI_ANIM_8,
    /*  9 */ SUTTARI_ANIM_MAX
} SuttariAnimation;

static AnimationInfo sAnimationInfo[SUTTARI_ANIM_MAX] = {
    { &object_boj_Anim_00071C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_0
    { &object_boj_Anim_0128F4, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_1
    { &object_boj_Anim_011F84, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_2
    { &object_boj_Anim_012E84, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f }, // SUTTARI_ANIM_3
    { &object_boj_Anim_01139C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_4
    { &object_boj_Anim_00071C, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_5
    { &object_boj_Anim_011F84, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_6
    { &object_boj_Anim_011C38, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },  // SUTTARI_ANIM_7
    { &object_boj_Anim_010BDC, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // SUTTARI_ANIM_8
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    0, 18, 64, 0, MASS_IMMOVABLE,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xE),
    /* Zora boomerang */ DMG_ENTRY(1, 0xE),
    /* Normal arrow   */ DMG_ENTRY(1, 0xE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0xE),
    /* Goron punch    */ DMG_ENTRY(1, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0xE),
    /* Ice arrow      */ DMG_ENTRY(1, 0xE),
    /* Light arrow    */ DMG_ENTRY(1, 0xE),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(1, 0xE),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xE),
};

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
};

static u8 D_80BAE820[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x80 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG, 0x7F - 0x08),
    /* 0x08 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_ICHIBA, 0x3D - 0x0C),
    /* 0x0C */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 0, 0, 25, 0x37 - 0x12),
    /* 0x12 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 25, 0, 30, 0x31 - 0x18),
    /* 0x18 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(1, 15, 1, 20, 0x2B - 0x1E),
    /* 0x1E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(1, 20, 1, 45, 0x25 - 0x24),
    /* 0x24 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x25 */ SCHEDULE_CMD_RET_TIME(1, 20, 1, 45, 14),
    /* 0x2B */ SCHEDULE_CMD_RET_TIME(1, 15, 1, 20, 7),
    /* 0x31 */ SCHEDULE_CMD_RET_TIME(0, 25, 0, 30, 6),
    /* 0x37 */ SCHEDULE_CMD_RET_TIME(0, 0, 0, 25, 13),
    /* 0x3D */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_AYASHIISHOP, 0x7E - 0x41),
    /* 0x41 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 25, 0, 30, 0x78 - 0x47),
    /* 0x47 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 30, 0, 45, 0x72 - 0x4D),
    /* 0x4D */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 45, 1, 0, 0x6C - 0x53),
    /* 0x53 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(1, 0, 1, 15, 0x66 - 0x59),
    /* 0x59 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(1, 15, 1, 20, 0x60 - 0x5F),
    /* 0x5F */ SCHEDULE_CMD_RET_NONE(),
    /* 0x60 */ SCHEDULE_CMD_RET_TIME(1, 15, 1, 20, 9),
    /* 0x66 */ SCHEDULE_CMD_RET_TIME(1, 0, 1, 15, 11),
    /* 0x6C */ SCHEDULE_CMD_RET_TIME(0, 45, 1, 0, 2),
    /* 0x72 */ SCHEDULE_CMD_RET_TIME(0, 30, 0, 45, 10),
    /* 0x78 */ SCHEDULE_CMD_RET_TIME(0, 25, 0, 30, 8),
    /* 0x7E */ SCHEDULE_CMD_RET_NONE(),
    /* 0x7F */ SCHEDULE_CMD_RET_NONE(),
    /* 0x80 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(1, 0xA9 - 0x84),
    /* 0x84 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_BACKTOWN, 0xA8 - 0x88),
    /* 0x88 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 0, 20, 0xA2 - 0x8E),
    /* 0x8E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 20, 0, 30, 0x9C - 0x94),
    /* 0x94 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(0, 30, 0x9B - 0x98),
    /* 0x98 */ SCHEDULE_CMD_RET_VAL_L(4),
    /* 0x9B */ SCHEDULE_CMD_RET_NONE(),
    /* 0x9C */ SCHEDULE_CMD_RET_TIME(0, 20, 0, 30, 15),
    /* 0xA2 */ SCHEDULE_CMD_RET_TIME(18, 0, 0, 20, 3),
    /* 0xA8 */ SCHEDULE_CMD_RET_NONE(),
    /* 0xA9 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0xD4 - 0xAD),
    /* 0xAD */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_IKANA, 0xD3 - 0xB1),
    /* 0xB1 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 19, 0, 0xCD - 0xB7),
    /* 0xB7 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(19, 0, 19, 25, 0xCA - 0xBD),
    /* 0xBD */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(19, 25, 0, 0, 0xC4 - 0xC3),
    /* 0xC3 */ SCHEDULE_CMD_RET_NONE(),
    /* 0xC4 */ SCHEDULE_CMD_RET_TIME(19, 25, 0, 0, 1),
    /* 0xCA */ SCHEDULE_CMD_RET_VAL_L(5),
    /* 0xCD */ SCHEDULE_CMD_RET_TIME(18, 0, 19, 0, 12),
    /* 0xD3 */ SCHEDULE_CMD_RET_NONE(),
    /* 0xD4 */ SCHEDULE_CMD_RET_NONE(),
};

static s32 D_80BAE8F8[] = {
    -1, -1, -1, -1, -1, -1, 1, 2, 0, 3, 1, 2, 0, 0, 3, 0,
};

void EnSuttari_UpdateCollider(EnSuttari* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;

    if (this->flags1 & 0x40) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect == 0xF) {
                if (GameInteractor_Should(VB_SAKON_TAKE_DAMAGE, true, this)) {
                    this->flags1 |= 0x100;
                    this->flags1 &= ~0x40;
                    Enemy_StartFinishingBlow(play, &this->actor);
                }
            } else if (this->actor.colChkInfo.damageEffect == 0xE) {
                this->flags1 |= 0x200;
                this->flags1 &= ~0x40;
            }
        }
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if ((this->flags1 & 1) && (this->actionFunc != func_80BADE14)) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    } else {
        Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_4);
    }
}

Actor* EnSuttari_GetActorById(PlayState* play, s16 actorId) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if (actor->id == actorId) {
            return actor;
        }
        actor = actor->next;
    }
    return NULL;
}

void EnSuttari_TriggerTransition(PlayState* play, u16 entrance) {
    play->nextEntrance = entrance;
    play->transitionType = TRANS_TYPE_64;
    gSaveContext.nextTransitionType = TRANS_TYPE_64;
    play->transitionTrigger = TRANS_TRIGGER_START;
}

void EnSuttari_AdvanceTime(void) {
    gSaveContext.save.time = CURRENT_TIME + (u16)R_TIME_SPEED;
    gSaveContext.save.time = CURRENT_TIME + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
}

s32 func_80BAA904(EnSuttari* this, PlayState* play) {
    Vec3f pos;
    s32 pad;
    CollisionPoly* poly;

    pos.x = (Math_SinS(this->actor.world.rot.y) * 50.0f) + this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 100.0f;
    pos.z = (Math_CosS(this->actor.world.rot.y) * 50.0f) + this->actor.world.pos.z;
    if (BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &pos) > -500.0f) {
        return false;
    }
    return true;
}

void func_80BAA9B4(EnSuttari* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    switch (this->textId) {
        case 0x1454:
        case 0x1455:
        case 0x1456:
        case 0x1457:
        case 0x1458:
        case 0x1459:
        case 0x145A:
        case 0x145B:
            if ((this->animIndex != SUTTARI_ANIM_8) && (curFrame == endFrame)) {
                this->animIndex = SUTTARI_ANIM_8;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            }
            break;

        default:
            if ((this->animIndex != SUTTARI_ANIM_1) && (curFrame == endFrame)) {
                this->animIndex = SUTTARI_ANIM_1;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            }
    }
}

void func_80BAAA94(EnSuttari* this) {
    switch (this->textId) {
        case 0x29E5:
            this->animIndex = SUTTARI_ANIM_1;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            break;

        case 0x29E9:
            this->enFsn->flags |= ENFSN_ANGRY;
            break;

        case 0x29EA:
            this->enFsn->flags |= ENFSN_CALM_DOWN;
            break;

        case 0x29EB:
            this->enFsn->flags |= ENFSN_OFFER_FINAL_PRICE;
            break;

        case 0x29EC:
            this->animIndex = SUTTARI_ANIM_7;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            break;

        case 0x29ED:
            this->animIndex = SUTTARI_ANIM_1;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            break;

        default:
            break;
    }
}

void func_80BAAB78(EnSuttari* this, PlayState* play) {
    if (this->flags1 & 4) {
        switch (this->textId) {
            case 0:
                this->flags1 |= 0x20;
                this->csIdIndex = 0;
                this->textId = 0x29E5;
                break;

            case 0x29E5:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 1;
                this->textId = 0x29E6;
                break;

            case 0x29E6:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 0;
                this->textId = 0x29E7;
                break;

            case 0x29E7:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 1;
                this->textId = 0x29E8;
                break;

            case 0x29E8:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 0;
                this->textId = 0x29E9;
                break;

            case 0x29E9:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 1;
                this->textId = 0x29EA;
                break;

            case 0x29EA:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 0;
                this->textId = 0x29EB;
                break;

            case 0x29EB:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 1;
                this->textId = 0x29EC;
                break;

            case 0x29EC:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->csIdIndex = 0;
                this->textId = 0x29ED;
                break;

            case 0x29ED:
                CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                this->flags1 |= 0x400;
                this->csIdIndex = 1;
                this->textId = 0x29EE;
                break;

            default:
                break;
        }
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        this->actionFunc = func_80BADD0C;
    } else if (this->flags1 & 1) {
        switch (this->textId) {
            case 0:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_81_01)) {
                    this->textId = 0x1455;
                    ((EnElf*)GET_PLAYER(play)->tatlActor)->unk_264 |= 8;
                    this->flags2 |= 1;
                } else {
                    this->textId = 0x1450;
                }
                break;

            case 0x1450:
                this->textId = 0x1451;
                break;

            case 0x1451:
                if (this->flags1 & 0x800) {
                    this->flags1 &= ~0x800;
                    this->textId = 0x1453;
                } else {
                    this->flags1 |= 0x400;
                    this->textId = 0x1452;
                    SET_WEEKEVENTREG(WEEKEVENTREG_81_01);
                }
                break;

            case 0x1453:
                this->flags1 |= 0x400;
                SET_WEEKEVENTREG(WEEKEVENTREG_81_01);
                ((EnElf*)GET_PLAYER(play)->tatlActor)->unk_264 |= 8;
                this->flags2 |= 1;
                this->textId = 0x1454;
                break;

            case 0x1455:
                this->textId = 0x1456;
                break;

            case 0x1456:
                this->textId = 0x1457;
                break;

            case 0x1457:
                this->textId = 0x1458;
                break;

            case 0x1458:
                this->textId = 0x1459;
                break;

            case 0x1459:
                this->textId = 0x145A;
                break;

            case 0x145A:
                this->flags1 |= 0x400;
                this->textId = 0x145B;
                break;

            default:
                break;
        }
    }
    Message_StartTextbox(play, this->textId, &this->actor);
}

void func_80BAAF1C(EnSuttari* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex == SUTTARI_ANIM_5) {
        this->animIndex = SUTTARI_ANIM_3;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
    } else if ((this->animIndex == SUTTARI_ANIM_3) && (curFrame == endFrame)) {
        this->animIndex = SUTTARI_ANIM_6;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->flags1 &= ~0x100;
    }
}

void func_80BAAFDC(EnSuttari* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 500.0f) && (this->actor.playerHeightRel < 100.0f)) {
        Vec3f effectVelocityOffset = { 0.0f, 0.0f, 0.0f };
        Vec3f effectPos;
        Vec3f effectVelocity;

        Math_Vec3f_Copy(&effectPos, &this->actor.world.pos);
        effectPos.x += Math_SinS(this->actor.world.rot.y + this->unk3F4) * 10.0f;
        effectPos.y += 60.0f;
        effectPos.z += Math_CosS(this->actor.world.rot.y + this->unk3F4) * 10.0f;
        Matrix_Push();
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        effectVelocityOffset.z = 20.0f;
        Matrix_MultVec3f(&effectVelocityOffset, &effectVelocity);
        Matrix_Pop();
        if (!this->playerDetected) {
            EffectSsSolderSrchBall_Spawn(play, &effectPos, &effectVelocity, &gZeroVec3f, 50, &this->playerDetected,
                                         SOLDERSRCHBALL_INVISIBLE);
        }
        if (this->playerDetected == true) {
            Audio_PlaySfx(NA_SE_SY_FOUND);
            this->playerDetected = false;
            this->actor.speed = 0.0f;
            if (this->unk1F4[0] != 0) {
                this->unk1F4[0]--;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_61_08);
            this->unk3F6 = 20;
            this->actionFunc = func_80BADE8C;
        }
    }
}

void func_80BAB1A0(EnSuttari* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 500.0f) && (this->actor.playerHeightRel < 100.0f)) {
        Vec3f effectVelocityOffset = { 0.0f, 0.0f, 0.0f };
        Vec3f effectPos;
        Vec3f effectVelocity;

        Math_Vec3f_Copy(&effectPos, &this->actor.world.pos);
        effectPos.x += Math_SinS(this->actor.world.rot.y + this->unk3F4) * 350.0f;
        effectPos.y += 60.0f;
        effectPos.z += Math_CosS(this->actor.world.rot.y + this->unk3F4) * 350.0f;
        Matrix_Push();
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        effectVelocityOffset.z = 20.0f;
        Matrix_MultVec3f(&effectVelocityOffset, &effectVelocity);
        Matrix_Pop();
        if (!this->playerDetected) {
            EffectSsSolderSrchBall_Spawn(play, &effectPos, &effectVelocity, &gZeroVec3f, 50, &this->playerDetected,
                                         SOLDERSRCHBALL_INVISIBLE);
        }
        if (this->playerDetected == true) {
            Audio_PlaySfx(NA_SE_SY_FOUND);
            this->playerDetected = false;
            this->actor.speed = 0.0f;
            if (this->unk1F4[0] != 0) {
                this->unk1F4[0]--;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_61_08);
            this->unk3F6 = 20;
            this->actionFunc = func_80BADE8C;
        } else {
            func_80BAAFDC(this, play);
        }
    }
}

void func_80BAB374(EnSuttari* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    Vec3f sp38;

    if ((curFrame % 3) == 0) {
        sp38.x = Rand_CenteredFloat(15.0f) + this->actor.world.pos.x;
        sp38.y = this->actor.world.pos.y;
        sp38.z = Rand_CenteredFloat(15.0f) + this->actor.world.pos.z;
        Actor_SpawnFloorDustRing(play, &this->actor, &sp38, 10.0f, 0, 2.0f, 0, 0, false);
    }
}

void func_80BAB434(EnSuttari* this) {
    this->unk3F2 = Math_SinS(this->unk3F4) * 60.0f * (0x10000 / 360.0f);
    this->unk3F4 += 0x400;
}

void EnSuttari_GetPaths(EnSuttari* this, PlayState* play) {
    this->paths[0] = SubS_GetPathByIndex(play, ENSUTTARI_GET_PATH_INDEX(&this->actor), ENSUTTARI_PATH_INDEX_NONE);
    //! @bug: the additional path shouldn't use `ENSUTTARI_PATH_INDEX_NONE`, but `ADDITIONAL_PATH_INDEX_NONE`
    this->paths[1] = SubS_GetPathByIndex(play, this->paths[0]->additionalPathIndex, ENSUTTARI_PATH_INDEX_NONE);
}

void func_80BAB4F0(EnSuttari* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f point;

    if (this->flags1 & 0x80) {
        EnSuttari_UpdateCollider(this, play);
    }
    SkelAnime_Update(&this->skelAnime);
    if (!(this->flags1 & 4) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
        if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x36B0, this->actor.yawTowardsPlayer)) {
            point.x = player->actor.world.pos.x;
            point.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
            point.z = player->actor.world.pos.z;
            SubS_TrackPoint(&point, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                            &this->torsoRot, &sTrackOptions);
        } else {
            Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->torsoRot.x, 0, 4, 0x3E8, 1);
            Math_SmoothStepToS(&this->torsoRot.y, 0, 4, 0x3E8, 1);
        }
    }
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, OBJECT_BOJ_LIMB_MAX);
}

s16 EnSuttari_GetDistSqAndOrient(Path* path, s32 index, Vec3f* pos, f32* distSq) {
    Vec3s* points;
    f32 diffX;
    f32 diffZ;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[index];
        diffX = points->x - pos->x;
        diffZ = points->z - pos->z;
    } else {
        diffX = 0.0f;
        diffZ = 0.0f;
    }

    *distSq = SQ(diffX) + SQ(diffZ);
    return RAD_TO_BINANG(Math_Atan2F_XY(diffZ, diffX));
}

s32 EnSuttari_HasReachedPointForward(EnSuttari* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }
    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

s32 EnSuttari_HasReachedPointReverse(EnSuttari* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[0].x - points[1].x;
        diffZ = points[0].z - points[1].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 2].x - points[count - 1].x;
        diffZ = points[count - 2].z - points[count - 1].z;
    } else {
        diffX = points[index - 1].x - points[index + 1].x;
        diffZ = points[index - 1].z - points[index + 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

void func_80BABA90(EnSuttari* this, s32 arg1, u8 arg2) {
    s16 target;
    f32 dist;

    if (this->paths[arg1] != NULL) {
        target = EnSuttari_GetDistSqAndOrient(this->paths[arg1], this->unk1F4[arg1], &this->actor.world.pos, &dist);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if (arg2 == 2) {
                this->unk1F4[arg1] = -0x63;
            } else {
                target = this->actor.wallYaw;
            }
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, target, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (EnSuttari_HasReachedPointForward(this, this->paths[arg1], this->unk1F4[arg1])) {
            if (this->unk1F4[arg1] >= (this->paths[arg1]->count - 1)) {
                if (arg2 == 0) {
                    this->unk1F4[arg1] = 0;
                } else {
                    this->unk1F4[arg1] = -0x63;
                }
            } else {
                this->unk1F4[arg1]++;
            }
        }
    }
}

void func_80BABB90(EnSuttari* this, s32 arg1) {
    s16 target;
    f32 sp30;

    if (this->paths[arg1] != NULL) {
        target = EnSuttari_GetDistSqAndOrient(this->paths[arg1], this->unk1F4[arg1], &this->actor.world.pos, &sp30);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            target = this->actor.wallYaw;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, target, 1, 0xBB8, 0);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (EnSuttari_HasReachedPointReverse(this, this->paths[arg1], this->unk1F4[arg1])) {
            if (this->unk1F4[arg1] <= 0) {
                this->unk1F4[arg1] = -0x63;
            } else {
                this->unk1F4[arg1]--;
            }
        }
    }
}

s32 func_80BABC48(EnSuttari* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 sp26 = SCRIPT_TIME_NOW;
    u16 phi_a0;
    u8 pathIndex = ENSUTTARI_GET_PATH_INDEX(&this->actor);
    u16 tmp;

    if (D_80BAE8F8[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80BAE8F8[scheduleOutput->result]);
    }
    if (this->timePath == NULL) {
        return 0;
    }
    if ((this->scheduleResult != 0) && (this->scheduleResult < 0xC) && (this->timePathTimeSpeed >= 0)) {
        phi_a0 = sp26;
    } else {
        phi_a0 = scheduleOutput->time0;
    }
    if (scheduleOutput->time1 < phi_a0) {
        this->timePathTotalTime = (phi_a0 - scheduleOutput->time1) + 0xFFFF;
    } else {
        this->timePathTotalTime = scheduleOutput->time1 - phi_a0;
    }
    this->timePathElapsedTime = sp26 - phi_a0;
    tmp = phi_a0 = this->timePath->count - (SUBS_TIME_PATHING_ORDER - 1);
    this->unk42C = 0;
    this->timePathWaypointTime = this->timePathTotalTime / tmp;
    this->timePathWaypoint = (this->timePathElapsedTime / this->timePathWaypointTime) + (SUBS_TIME_PATHING_ORDER - 1);
    this->unk430 = 0;
    return 1;
}

s32 func_80BABDD8(EnSuttari* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 pad;
    EnDoor* door;
    u8 pathIndex = ENSUTTARI_GET_PATH_INDEX(&this->actor);
    u16 sp44 = SCRIPT_TIME_NOW;
    Vec3f sp38;
    Vec3f sp2C;
    Vec3s* sp28;
    s32 sp24;

    if ((this->scheduleResult == 10) || (this->scheduleResult == 11) || (this->scheduleResult == 2)) {
        return false;
    }
    door = (EnDoor*)SubS_FindNearestActor(&this->actor, play, ACTORCAT_DOOR, ACTOR_EN_DOOR);
    sp24 = D_80BAE8F8[scheduleOutput->result];
    if ((door != NULL) && (sp24 >= 0)) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, sp24);
    }
    if ((door == NULL) || (this->timePath == NULL)) {
        return false;
    }
    sp28 = Lib_SegmentedToVirtual(this->timePath->points);
    Math_Vec3s_ToVec3f(&sp38, &sp28[0]);
    Math_Vec3s_ToVec3f(&sp2C, &sp28[1]);
    this->unk434 = sp44 - scheduleOutput->time0;
    this->unk436 = scheduleOutput->time1 - scheduleOutput->time0;
    if ((scheduleOutput->result != 10) && (scheduleOutput->result != 11)) {
        door->openTimer = 75;
    }
    Math_Vec3f_Copy(&this->unk438, &sp38);
    Math_Vec3f_Copy(&this->unk444, &sp2C);
    this->actor.world.rot.y = Math_Vec3f_Yaw(&sp38, &sp2C);
    this->actor.world.pos = sp38;
    return true;
}

s32 func_80BABF64(EnSuttari* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret;

    switch (scheduleOutput->result) {
        case 15:
        case 14:
        case 13:
        case 12:
            ret = func_80BABC48(this, play, scheduleOutput);
            break;

        case 11:
        case 10:
        case 9:
        case 8:
        case 7:
        case 6:
            ret = func_80BABDD8(this, play, scheduleOutput);
            break;

        case 5:
        case 4:
        case 3:
        case 2:
        case 1:
            ret = true;
            break;

        default:
            ret = false;
            break;
    }
    return ret;
}

s32 func_80BABFD4(EnSuttari* this, PlayState* play) {
    f32 knots[265];
    Vec3f sp70;
    Vec3f sp64;
    Vec3f timePathTargetPos;
    s32 sp54 = 0;
    s32 sp50 = 0;
    s32 pad;

    SubS_TimePathing_FillKnots(knots, SUBS_TIME_PATHING_ORDER, this->timePath->count + SUBS_TIME_PATHING_ORDER);
    if (this->unk42C == 0) {
        timePathTargetPos = gZeroVec3f;
        SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &timePathTargetPos, this->timePathTimeSpeed);
        SubS_TimePathing_ComputeInitialY(play, this->timePath, this->timePathWaypoint, &timePathTargetPos);
        this->actor.world.pos.y = timePathTargetPos.y;
        this->unk42C = 1;
    } else {
        timePathTargetPos = this->timePathTargetPos;
    }
    this->actor.world.pos.x = timePathTargetPos.x;
    this->actor.world.pos.z = timePathTargetPos.z;
    if (SubS_InCsMode(play)) {
        sp54 = this->timePathElapsedTime;
        sp50 = this->timePathWaypoint;
        timePathTargetPos = this->actor.world.pos;
    }
    this->timePathTargetPos = gZeroVec3f;
    if (SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &this->timePathTargetPos, this->timePathTimeSpeed)) {
        this->unk430 = 1;
    } else {
        sp70 = this->actor.world.pos;
        sp64 = this->timePathTargetPos;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp70, &sp64);
    }
    if (SubS_InCsMode(play)) {
        this->timePathElapsedTime = sp54;
        this->timePathWaypoint = sp50;
        this->timePathTargetPos = timePathTargetPos;
    }
    return 0;
}

s32 func_80BAC220(EnSuttari* this, PlayState* play) {
    Vec3f sp3C;
    Vec3f sp30;
    s32 tmp2;
    f32 tmp;

    if (this->unk434 < 0) {
        this->unk434 = 0;
    } else {
        tmp2 = (this->unk436 < this->unk434) ? this->unk436 : this->unk434;
        this->unk434 = tmp2;
    }
    tmp = Math_Vec3f_DistXZ(&this->unk438, &this->unk444) / this->unk436;
    sp3C.x = sp3C.y = 0.0f;
    sp3C.z = this->unk434 * tmp;
    Lib_Vec3f_TranslateAndRotateY(&this->unk438, this->actor.world.rot.y, &sp3C, &sp30);
    this->actor.world.pos = sp30;
    this->unk434 += this->timePathTimeSpeed;
    return 0;
}

void func_80BAC2FC(EnSuttari* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    switch (this->scheduleResult) {
        case 12:
        case 13:
            this->flags1 |= 0x80;
            func_80BABFD4(this, play);
            break;

        case 15:
            if ((this->animIndex == SUTTARI_ANIM_1) && (curFrame == endFrame)) {
                this->animIndex = SUTTARI_ANIM_2;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            }
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_83_04) && !(this->flags1 & 0x1000)) {
                if (CutsceneManager_IsNext(this->csIdList[0])) {
                    CutsceneManager_Start(this->csIdList[0], &this->actor);
                    if (!(player->stateFlags1 & PLAYER_STATE1_10000000)) {
                        this->flags2 |= 0x10;
                        player->stateFlags1 |= PLAYER_STATE1_10000000;
                    }
                    this->flags1 |= 0x1000;
                    this->flags2 |= 2;
                } else {
                    CutsceneManager_Queue(this->csIdList[0]);
                }
            }
            func_80BABFD4(this, play);
            break;

        case 6:
        case 8:
            this->flags1 |= 0x80;
            func_80BAC220(this, play);
            break;

        case 10:
            this->flags1 |= 0x80;
            func_80BAC220(this, play);
            break;

        case 9:
        case 11:
            this->flags1 |= 0x80;
            this->flags1 |= 0x20;
            this->enFsn->flags &= ~ENFSN_HAGGLE;
            if (this->animIndex != SUTTARI_ANIM_2) {
                this->animIndex = SUTTARI_ANIM_2;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            }
            func_80BAC220(this, play);
            break;

        case 7:
            this->flags1 |= 0x20;
            this->flags1 |= 0x80;
            func_80BAC220(this, play);
            break;

        case 14:
            this->flags1 |= 0x20;
            this->flags1 |= 0x80;
            func_80BABFD4(this, play);
            break;

        case 1:
            Actor_Kill(&this->actor);
            break;

        case 2:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_81_04)) {
                this->flags1 |= 0x80;
                this->actor.world.pos.x = -16.0f;
                this->actor.world.pos.z = -16.0f;
                this->actor.world.pos.y = 12.0f;
                this->enFsn->flags |= ENFSN_HAGGLE;
                this->actionFunc = func_80BADA08;
            }
            break;

        case 4:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
                if ((this->animIndex == SUTTARI_ANIM_2) || (this->animIndex == SUTTARI_ANIM_1)) {
                    this->animIndex = SUTTARI_ANIM_5;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
                }
                this->flags1 |= 0x10;
                if (this->flags2 & 2) {
                    this->flags2 &= ~2;
                }
                if (!(player->stateFlags1 & PLAYER_STATE1_10000000)) {
                    this->flags2 |= 0x10;
                    player->stateFlags1 |= PLAYER_STATE1_10000000;
                }
                this->textId = 0x2A30;
                Message_StartTextbox(play, this->textId, &this->actor);
                this->actionFunc = func_80BAD2B4;
            }
            break;

        default:
            break;
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 4, 0x1554);
}

void func_80BAC6E8(EnSuttari* this, PlayState* play) {
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_boj_Skel_00C240, &object_boj_Anim_00071C, this->jointTable,
                       this->morphTable, OBJECT_BOJ_LIMB_MAX);
    this->actor.draw = EnSuttari_Draw;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    if (play->sceneId == SCENE_IKANA) {
        this->flags1 |= 1;
        if ((gSaveContext.save.day == 1) || (gSaveContext.save.day == 2)) {
            this->animIndex = SUTTARI_ANIM_2;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->flags1 |= 0x80;
            this->actionFunc = func_80BACA14;
            return;
        } else if ((gSaveContext.save.day == 3) && (CURRENT_TIME <= CLOCK_TIME(19, 0)) &&
                   !CHECK_WEEKEVENTREG(WEEKEVENTREG_61_08) &&
                   !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) &&
                   CHECK_WEEKEVENTREG(WEEKEVENTREG_51_08)) {
            this->animIndex = SUTTARI_ANIM_2;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = func_80BACEE0;
            return;
        }
    } else if (play->sceneId == SCENE_BACKTOWN) {
        if ((CURRENT_TIME >= CLOCK_TIME(0, 20)) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
            Actor_Kill(&this->actor);
        }

        if ((gSaveContext.save.entrance == ENTRANCE(NORTH_CLOCK_TOWN, 7)) || CHECK_WEEKEVENTREG(WEEKEVENTREG_58_40)) {
            Actor_Kill(&this->actor);
        }
        this->csIdList[0] = this->actor.csId;
        this->csIdList[1] = CutsceneManager_GetAdditionalCsId(this->csIdList[0]);
        this->flags1 |= 0x80;
        this->flags1 |= 8;
        this->animIndex = SUTTARI_ANIM_1;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = func_80BAD004;
        return;
    } else if (play->sceneId == SCENE_ICHIBA) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
            Actor_Kill(&this->actor);
            return;
        }
        this->animIndex = SUTTARI_ANIM_0;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->flags1 |= 2;
        this->actionFunc = func_80BAD5F8;
        return;
    } else if (play->sceneId == SCENE_AYASHIISHOP) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
            Actor_Kill(&this->actor);
            return;
        }
        this->animIndex = SUTTARI_ANIM_0;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->csIdList[0] = this->actor.csId;
        this->csIdList[1] = CutsceneManager_GetAdditionalCsId(this->csIdList[0]);
        this->flags1 |= 4;
        this->actionFunc = func_80BAD7F8;
        return;
    } else {
        this->flags1 |= 8;
        this->flags1 |= 0x10;
        this->actionFunc = func_80BACA14;
        return;
    }
    Actor_Kill(&this->actor);
}

void func_80BACA14(EnSuttari* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->animIndex == SUTTARI_ANIM_1) || (this->animIndex == SUTTARI_ANIM_8)) {
        this->animIndex = SUTTARI_ANIM_2;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
    func_80BABA90(this, 0, 0);
    func_80BAB434(this);
    if ((player->transformation == PLAYER_FORM_GORON) || (player->transformation == PLAYER_FORM_ZORA)) {
        if ((this->actor.playerHeightRel < 60.0f) && (this->actor.xzDistToPlayer < 500.0f)) {
            this->unk3F2 = this->headRot.y;
            this->actionFunc = func_80BACBB0;
        }
    } else if ((player->transformation == PLAYER_FORM_HUMAN) &&
               (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) != EQUIP_VALUE_SWORD_NONE)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->unk3F2 = this->headRot.y;
            func_80BAAB78(this, play);
            this->actionFunc = func_80BADA9C;
        } else if (this->actor.xzDistToPlayer < 200.0f) {
            Actor_OfferTalk(&this->actor, play, 200.0f);
        }
    }
    Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 0.1f);
    Actor_MoveWithGravity(&this->actor);
}

void func_80BACBB0(EnSuttari* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 target;

    this->unk3F2 = this->headRot.y;
    if ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_DEKU)) {
        this->actionFunc = func_80BACA14;
    }
    if ((this->actor.playerHeightRel < 60.0f) && (this->actor.xzDistToPlayer < 500.0f)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            target = this->actor.wallYaw;
        } else if (func_80BAA904(this, play)) {
            target = -this->actor.world.rot.y;
        } else {
            target = -this->actor.yawTowardsPlayer;
        }
        Math_SmoothStepToS(&this->actor.world.rot.y, target, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 0.1f);
    } else {
        this->actionFunc = func_80BACD2C;
        this->actor.speed = 0.0f;
    }
    Actor_MoveWithGravity(&this->actor);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.world.pos = this->actor.prevPos;
        this->actor.world.rot.y = -this->actor.world.rot.y;
        this->actionFunc = func_80BACE4C;
    }
}

void func_80BACD2C(EnSuttari* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk3F2 = this->headRot.y;
    if ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_DEKU)) {
        this->actionFunc = func_80BACA14;
    }
    if ((this->actor.playerHeightRel < 60.0f) && (this->actor.xzDistToPlayer < 500.0f)) {
        this->actionFunc = func_80BACBB0;
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 0.1f);
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
    Actor_MoveWithGravity(&this->actor);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.world.pos = this->actor.prevPos;
        this->actor.world.rot.y = -this->actor.world.rot.y;
        this->actionFunc = func_80BACE4C;
    }
}

void func_80BACE4C(EnSuttari* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 100.0f) {
        this->actionFunc = func_80BACBB0;
    }
    Actor_MoveWithGravity(&this->actor);
    if (this->actor.world.pos.y != this->actor.floorHeight) {
        this->actor.world.pos = this->actor.prevPos;
        this->actor.world.rot.y = -this->actor.world.rot.y;
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

void func_80BACEE0(EnSuttari* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
    if (!Schedule_RunScript(play, D_80BAE820, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BABF64(this, play, &scheduleOutput))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    func_80BAC2FC(this, play);
    func_80BAB434(this);
    if (this->scheduleResult == 5) {
        SET_WEEKEVENTREG(WEEKEVENTREG_58_80);
        this->actionFunc = func_80BADDB4;
        this->actor.speed = 0.0f;
    } else if (Player_GetMask(play) != PLAYER_MASK_STONE) {
        func_80BAB1A0(this, play);
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80BAD004(EnSuttari* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
    if (!Schedule_RunScript(play, D_80BAE820, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BABF64(this, play, &scheduleOutput))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    this->scheduleResult = scheduleOutput.result;
    func_80BAC2FC(this, play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x2A3A, &this->actor);
        this->actionFunc = func_80BAD130;
    } else if ((this->actor.xzDistToPlayer < 200.0f) || this->actor.isLockedOn) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }

    Actor_MoveWithGravity(&this->actor);
}

void func_80BAD130(EnSuttari* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        if (this->flags1 & 8) {
            this->actionFunc = func_80BAD004;
        } else if (this->flags1 & 4) {
            this->actionFunc = func_80BAD7F8;
        } else if (this->flags1 & 2) {
            this->actionFunc = func_80BAD5F8;
        } else if (this->flags1 & 1) {
            this->unk3F6 = 20;
            this->actionFunc = func_80BADF3C;
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
}

void func_80BAD230(EnSuttari* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csIdList[1])) {
        CutsceneManager_Start(this->csIdList[1], &this->actor);
        this->textId = 0x2A31;
        Message_StartTextbox(play, this->textId, &this->actor);
        this->flags1 |= 0x4000;
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CHASE | SEQ_FLAG_ASYNC);
        this->actionFunc = func_80BAD380;
    } else {
        CutsceneManager_Queue(this->csIdList[1]);
    }
}

void func_80BAD2B4(EnSuttari* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->textId == 0x2A30) {
            CutsceneManager_Stop(this->csIdList[0]);
            CutsceneManager_Queue(this->csIdList[1]);
            this->actionFunc = func_80BAD230;
        } else {
            CutsceneManager_Stop(this->csIdList[1]);
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->flags1 |= 0x40;
            this->actionFunc = func_80BAD380;
        }
    }
    func_80BAB374(this, play);
}

void func_80BAD380(EnSuttari* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_TALKING) && (play->msgCtx.currentTextId != 0x2A31)) {
        this->flags1 |= 0x8000;
        this->actor.speed = 0.0f;
    } else {
        this->flags1 &= ~0x8000;
        func_80BABA90(this, 1, 1);
        if ((this->flags1 & 0x4000) && (talkState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            this->flags2 &= ~0x10;
            player->stateFlags1 &= ~PLAYER_STATE1_10000000;
            this->flags1 &= ~0x4000;
            CutsceneManager_Stop(this->csIdList[1]);
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->flags1 |= 0x40;
        }
        if (this->flags1 & 0x100) {
            this->flags2 |= 8;
            func_80BAAF1C(this);
        } else if (this->flags1 & 0x200) {
            SET_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD);
            this->flags2 |= 4;
            this->actor.speed = 0.0f;
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_EXPLOSION));
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_IT_BOMB_EXPLOSION);
            Actor_Kill(&this->actor);
            return;
        }
        if (this->unk1F4[1] == -0x63) {
            if (this->flags2 & 8) {
                SET_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG);
            }
            this->actor.speed = 0.0f;
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
            this->flags2 |= 4;
            EnSuttari_TriggerTransition(play, ENTRANCE(NORTH_CLOCK_TOWN, 7));
        } else {
            this->unk3F2 = this->headRot.y;
            Math_ApproachF(&this->actor.speed, 4.0f, 0.2f, 0.5f);
            Actor_MoveWithGravity(&this->actor);
            func_80BAB374(this, play);
        }
    }
}

void func_80BAD5F8(EnSuttari* this, PlayState* play) {
    ScheduleOutput scheduleOutput;
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((curFrame == endFrame) && (this->animIndex == SUTTARI_ANIM_0) && (this->flags1 & 0x20)) {
        this->animIndex = SUTTARI_ANIM_2;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
    if (!Schedule_RunScript(play, D_80BAE820, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BABF64(this, play, &scheduleOutput))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    func_80BAC2FC(this, play);
    if ((this->unk430 == 1) && (this->timePath->additionalPathIndex == ADDITIONAL_PATH_INDEX_NONE)) {
        Actor_Kill(&this->actor);
        return;
    }
    func_80BAB434(this);
    if ((this->flags1 & 0x20) && (this->unk430 == 0) && (scheduleOutput.result != 7)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            Message_StartTextbox(play, 0x2A02, &this->actor);
            this->actionFunc = func_80BAD130;
        } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80BAD7F8(EnSuttari* this, PlayState* play) {
    ScheduleOutput scheduleOutput;
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->enFsn == NULL) {
        this->enFsn = (EnFsn*)EnSuttari_GetActorById(play, ACTOR_EN_FSN);
    } else {
        if ((this->flags1 & 0x2000) && (this->animIndex == SUTTARI_ANIM_1) && (curFrame == endFrame)) {
            this->animIndex = SUTTARI_ANIM_2;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
        this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
        if (!Schedule_RunScript(play, D_80BAE820, &scheduleOutput) ||
            ((this->scheduleResult != scheduleOutput.result) && !func_80BABF64(this, play, &scheduleOutput))) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            scheduleOutput.result = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        }
        this->scheduleResult = scheduleOutput.result;
        func_80BAC2FC(this, play);
        if ((this->unk430 == 1) && (this->timePath->additionalPathIndex == ADDITIONAL_PATH_INDEX_NONE)) {
            Actor_Kill(&this->actor);
            return;
        }
        if ((this->flags1 & 0x20) && (scheduleOutput.result != 9)) {
            if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                Message_StartTextbox(play, 0x2A02, &this->actor);
                this->actionFunc = func_80BAD130;
            } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
                Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            }
        }
        Actor_MoveWithGravity(&this->actor);
    }
}

void func_80BADA08(EnSuttari* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80BAAB78(this, play);
        SET_WEEKEVENTREG(WEEKEVENTREG_81_04);
    } else if (this->actor.xzDistToPlayer < 500.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 500.0f);
    }
}

void func_80BADA9C(EnSuttari* this, PlayState* play) {
    u8 talkstate = Message_GetState(&play->msgCtx);
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->flags1 & 1) {
        this->unk3F2 = this->headRot.y;
        func_80BAA9B4(this);
    } else if ((this->animIndex == SUTTARI_ANIM_7) && (curFrame == endFrame)) {
        this->animIndex = SUTTARI_ANIM_1;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
    if (talkstate == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if (this->flags1 & 0x400) {
                if (this->textId == 0x29EE) {
                    CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
                }
                this->flags1 &= ~0x400;
                if (this->flags2 & 1) {
                    ((EnElf*)GET_PLAYER(play)->tatlActor)->unk_264 |= 0x10;
                    this->flags2 &= ~1;
                }
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->textId = 0;
                if (this->flags1 & 1) {
                    this->actionFunc = func_80BACA14;
                } else {
                    this->actionFunc = func_80BAD7F8;
                }
                return;
            }
            func_80BAAB78(this, play);
        }
    } else if ((talkstate == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Audio_PlaySfx_MessageDecide();
                this->flags1 |= 0x800;
                func_80BAAB78(this, play);
                break;

            case 1:
                Audio_PlaySfx_MessageCancel();
                func_80BAAB78(this, play);
                break;

            default:
                break;
        }
    }
    if (!(this->flags1 & 4)) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void func_80BADD0C(EnSuttari* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
        if (this->csIdIndex == 0) {
            CutsceneManager_Start(this->csIdList[this->csIdIndex], &this->enFsn->actor);
        } else {
            CutsceneManager_Start(this->csIdList[this->csIdIndex], &this->actor);
        }
        func_80BAAA94(this);
        this->actionFunc = func_80BADA9C;
    } else {
        CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
    }
}

void func_80BADDB4(EnSuttari* this, PlayState* play) {
    func_80BABA90(this, 1, 1);
    func_80BAB434(this);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_51_10)) {
        this->actionFunc = func_80BADE14;
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80BADE14(EnSuttari* this, PlayState* play) {
    func_80BABA90(this, 1, 2);
    if (this->unk1F4[1] == -0x63) {
        this->actor.speed = 0.0f;
    } else {
        this->unk3F2 = this->headRot.y;
        Math_ApproachF(&this->actor.speed, 6.0f, 0.2f, 0.5f);
    }
    Actor_MoveWithGravity(&this->actor);
}

void func_80BADE8C(EnSuttari* this, PlayState* play) {
    this->unk3F2 = this->headRot.y;
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Message_StartTextbox(play, 0x2A3A, &this->actor);
        this->actionFunc = func_80BAD130;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->actor, play, 500.0f);
    }
}

void func_80BADF3C(EnSuttari* this, PlayState* play) {
    func_80BABB90(this, 0);
    if (this->unk1F4[0] == -0x63) {
        Actor_Kill(&this->actor);
    }
    this->unk3F2 = this->headRot.y;
    if (DECR(this->unk3F6) == 0) {
        Math_ApproachF(&this->actor.speed, 6.0f, 0.2f, 0.5f);
    }
    Actor_MoveWithGravity(&this->actor);
}

void EnSuttari_Init(Actor* thisx, PlayState* play) {
    EnSuttari* this = (EnSuttari*)thisx;
    s32 pad;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
        Actor_Kill(&this->actor);
        return;
    }
    Collider_InitCylinder(play, &this->collider);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    EnSuttari_GetPaths(this, play);
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = func_80BAC6E8;
    this->actor.gravity = -4.0f;
}

void EnSuttari_Destroy(Actor* thisx, PlayState* play) {
    EnSuttari* this = (EnSuttari*)thisx;

    if ((play->sceneId == SCENE_BACKTOWN) && !(this->flags2 & 4)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
    }
    Collider_DestroyCylinder(play, &this->collider);
}

void EnSuttari_Update(Actor* thisx, PlayState* play) {
    EnSuttari* this = (EnSuttari*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    this->actionFunc(this, play);
    if ((this->flags1 & 8) && (this->flags2 & 0x10) && (player->stateFlags1 & PLAYER_STATE1_10000000)) {
        player->actor.freezeTimer = 3;
    }
    if (!(this->flags1 & 0x8000)) {
        func_80BAB4F0(this, play);
    }
    if (this->flags2 & 2) {
        EnSuttari_AdvanceTime();
    }
    if (this->scheduleResult != 0) {
        if ((this->animIndex == SUTTARI_ANIM_2) || (this->animIndex == SUTTARI_ANIM_6)) {
            if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 16.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
            }
        } else if ((this->animIndex == SUTTARI_ANIM_0) || (this->animIndex == SUTTARI_ANIM_5)) {
            if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 17.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
            }
        }
    }
}

s32 EnSuttari_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSuttari* this = (EnSuttari*)thisx;

    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        *dList = object_boj_DL_00AF90;
        if (!(this->flags1 & 4)) {
            Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateXS(this->unk3F2, MTXMODE_APPLY);
            Matrix_RotateZS(-this->headRot.x, MTXMODE_APPLY);
            Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        }
    }
    if (limbIndex == OBJECT_BOJ_LIMB_08) {
        Matrix_RotateXS(-this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->torsoRot.x, MTXMODE_APPLY);
    }
    if ((limbIndex == OBJECT_BOJ_LIMB_08) || (limbIndex == OBJECT_BOJ_LIMB_09) || (limbIndex == OBJECT_BOJ_LIMB_0C)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex]) * 200.0f);
    }
    return false;
}

void EnSuttari_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80BAE950 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_80BAE95C = { 2000.0f, -1000.0f, 0.0f };
    EnSuttari* this = (EnSuttari*)thisx;
    s32 pad;
    MtxF* curState;
    Actor* bombBag;

    if (((this->flags1 & 8) && (this->flags1 & 0x10)) || ((this->flags1 & 2) && !(this->flags1 & 0x20)) ||
        ((this->flags1 & 4) && !(this->flags1 & 0x20))) {
        if (limbIndex == OBJECT_BOJ_LIMB_08) {
            curState = Matrix_GetCurrent();
            Matrix_MultVec3f(&D_80BAE95C, &this->unk3F8);
            if (this->actor.child == NULL) {
                if (this->flags1 & 0x100) {
                    bombBag =
                        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_NIMOTSU, curState->mf[3][0],
                                           curState->mf[3][1], curState->mf[3][2], 0, 0, 0, -1);
                    if (bombBag != NULL) {
                        Matrix_MtxFToYXZRot(curState, &bombBag->shape.rot, false);
                    }
                } else {
                    Gfx_SetupDL25_Opa(play->state.gfxCtx);

                    OPEN_DISPS(play->state.gfxCtx);

                    gSPDisplayList(POLY_OPA_DISP++, gBombShopBagDL);

                    CLOSE_DISPS(play->state.gfxCtx);
                }
            }
        }
    }
    if (limbIndex == OBJECT_BOJ_LIMB_0F) {
        Matrix_MultVec3f(&D_80BAE950, &this->actor.focus.pos);
    }
}

void EnSuttari_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void EnSuttari_Draw(Actor* thisx, PlayState* play) {
    EnSuttari* this = (EnSuttari*)thisx;
    s32 pad;
    Vec3f pos;
    Vec3f scale;

    if (this->flags1 & 0x80) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, Gfx_EnvColor(play->state.gfxCtx, 255, 255, 255, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 55, 55, 255, 0));
        gDPPipeSync(POLY_OPA_DISP++);
        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnSuttari_OverrideLimbDraw, EnSuttari_PostLimbDraw,
                                       EnSuttari_TransformLimbDraw, &this->actor);
        if (this->flags1 & 0x80) {
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);
            pos = this->actor.world.pos;
            scale.x = 0.2f;
            scale.y = 0.2f;
            scale.z = 0.2f;
            func_800BC620(&pos, &scale, 255, play);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

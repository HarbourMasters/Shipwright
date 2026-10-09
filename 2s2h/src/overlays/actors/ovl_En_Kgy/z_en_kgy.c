/*
 * File: z_en_kgy.c
 * Overlay: ovl_En_Kgy
 * Description: Blacksmith - Gabora
 */

#include "z_en_kgy.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnKgy_Init(Actor* thisx, PlayState* play);
void EnKgy_Destroy(Actor* thisx, PlayState* play);
void EnKgy_Update(Actor* thisx, PlayState* play);
void EnKgy_Draw(Actor* thisx, PlayState* play);

void EnKgy_ChangeAnim(EnKgy* this, s16 animIndex, u8 animMode, f32 morphFrames);
EnKbt* EnKgy_FindZubora(PlayState* play);
ObjIcePoly* EnKgy_FindIceBlock(PlayState* play);
void func_80B40D30(PlayState* play);
s32 func_80B40D64(PlayState* play);
s32 func_80B40DB4(PlayState* play);
void func_80B419B0(EnKgy* this, PlayState* play);
void func_80B41A48(EnKgy* this, PlayState* play);
void func_80B41E18(EnKgy* this, PlayState* play);
void func_80B42508(EnKgy* this, PlayState* play);
void func_80B425A0(EnKgy* this, PlayState* play);
void func_80B42714(EnKgy* this, PlayState* play);
void func_80B42D28(EnKgy* this, PlayState* play);

typedef enum EnKgyAnimation {
    /* -1 */ ENKGY_ANIM_NONE = -1,
    /*  0 */ ENKGY_ANIM_0,
    /*  1 */ ENKGY_ANIM_1,
    /*  2 */ ENKGY_ANIM_2,
    /*  3 */ ENKGY_ANIM_3,
    /*  4 */ ENKGY_ANIM_4,
    /*  5 */ ENKGY_ANIM_5,
    /*  6 */ ENKGY_ANIM_6,
    /*  7 */ ENKGY_ANIM_7,
    /*  8 */ ENKGY_ANIM_8,
    /*  9 */ ENKGY_ANIM_9,
    /* 10 */ ENKGY_ANIM_MAX
} EnKgyAnimation;

ActorProfile En_Kgy_Profile = {
    /**/ ACTOR_EN_KGY,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KGY,
    /**/ sizeof(EnKgy),
    /**/ EnKgy_Init,
    /**/ EnKgy_Destroy,
    /**/ EnKgy_Update,
    /**/ EnKgy_Draw,
};

void EnKgy_Init(Actor* thisx, PlayState* play) {
    EnKgy* this = (EnKgy*)thisx;
    s16 csId;
    s32 i;

    Actor_SetScale(&this->actor, 0.01f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_kgy_Skel_00F910, &object_kgy_Anim_004B98, this->jointTable,
                       this->morphTable, OBJECT_KGY_LIMB_MAX);
    this->animIndex = ENKGY_ANIM_NONE;
    this->unk_29C = 0;
    this->unk_2E4 = 0;
    this->animIndex2 = ENKGY_ANIM_NONE;
    this->zubora = EnKgy_FindZubora(play);
    this->iceBlock = EnKgy_FindIceBlock(play);
    Flags_UnsetSwitch(play, ENKGY_GET_SWITCH_FLAG(&this->actor) + 1);
    if (Flags_GetSwitch(play, ENKGY_GET_SWITCH_FLAG(&this->actor)) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
        Flags_SetSwitch(play, ENKGY_GET_SWITCH_FLAG(&this->actor) + 1);
        play->envCtx.lightSettingOverride = 1;
        SET_WEEKEVENTREG(WEEKEVENTREG_21_01);
        if (!func_80B40D64(play)) {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_4, ANIMMODE_LOOP, 0);
            this->actionFunc = func_80B425A0;
            this->actor.textId = 0xC35;
        } else if (!func_80B40DB4(play)) {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_6, ANIMMODE_ONCE, 0);
            this->actionFunc = func_80B419B0;
            this->actor.textId = 0xC4E;
            this->unk_29C |= 1;
            this->getItemId = GI_RUPEE_10;
        } else {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_4, ANIMMODE_LOOP, 0);
            this->actionFunc = func_80B42714;
            this->actor.textId = 0xC50;
        }
    } else {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_80)) {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_4, ANIMMODE_LOOP, 0);
        } else {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_0, ANIMMODE_LOOP, 0);
        }
        this->actionFunc = func_80B42D28;
    }

    csId = this->actor.csId;
    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        }
    }

    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 255, 64, 64, -1);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    this->unk_300 = -1;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnKgy_Destroy(Actor* thisx, PlayState* play) {
    EnKgy* this = (EnKgy*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

static AnimationHeader* sAnimations[ENKGY_ANIM_MAX] = {
    &object_kgy_Anim_004B98, // ENKGY_ANIM_0
    &object_kgy_Anim_0008FC, // ENKGY_ANIM_1
    &object_kgy_Anim_00292C, // ENKGY_ANIM_2
    &object_kgy_Anim_0042E4, // ENKGY_ANIM_3
    &object_kgy_Anim_0101F0, // ENKGY_ANIM_4
    &object_kgy_Anim_001764, // ENKGY_ANIM_5
    &object_kgy_Anim_003334, // ENKGY_ANIM_6
    &object_kgy_Anim_010B84, // ENKGY_ANIM_7
    &object_kgy_Anim_001EA4, // ENKGY_ANIM_8
    &object_kgy_Anim_003D88, // ENKGY_ANIM_9
};

void EnKgy_ChangeAnim(EnKgy* this, s16 animIndex, u8 animMode, f32 morphFrames) {
    Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimations[animIndex]), animMode, morphFrames);
    this->animIndex = animIndex;
}

void EnKgy_ChangeAnimLoopMorph(EnKgy* this, s16 animIndex) {
    if (this->animIndex != animIndex) {
        EnKgy_ChangeAnim(this, animIndex, ANIMMODE_LOOP, -5.0f);
    }
}

EnKbt* EnKgy_FindZubora(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_KBT) {
            return (EnKbt*)actor;
        }
        actor = actor->next;
    }
    return NULL;
}

ObjIcePoly* EnKgy_FindIceBlock(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_OBJ_ICE_POLY) {
            return (ObjIcePoly*)actor;
        }
        actor = actor->next;
    }
    return NULL;
}

void func_80B40C74(PlayState* play) {
    gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= 1;
    if (CURRENT_DAY == 1) {
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= 2;
    } else {
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 &= ~2;
    }
}

void func_80B40D00(PlayState* play) {
    gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= 4;
}

void func_80B40D30(PlayState* play) {
    gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 &= ~7;
}

s32 func_80B40D64(PlayState* play) {
    return gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 1;
}

s32 func_80B40D8C(PlayState* play) {
    return gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 4;
}

s32 func_80B40DB4(PlayState* play) {
    if ((CURRENT_DAY == 3) ||
        ((CURRENT_DAY == 2) && (gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 2))) {
        return true;
    }
    return false;
}

void func_80B40E18(EnKgy* this, u16 textId) {
    if (this->zubora != NULL) {
        this->zubora->actor.textId = textId;
    }
}

void func_80B40E38(EnKgy* this) {
    if (this->zubora != NULL) {
        this->zubora->actor.home.rot.z = 1;
    }
}

s32 func_80B40E54(EnKgy* this) {
    if (this->zubora != NULL) {
        return this->zubora->actor.home.rot.z;
    }
    return 0;
}

void func_80B40E74(EnKgy* this, PlayState* play, u16 textId) {
    Message_ContinueTextbox(play, textId);
    this->actor.textId = textId;
    func_80B40E18(this, this->actor.textId);
}

void func_80B40EBC(EnKgy* this, PlayState* play, u16 arg2) {
    func_80B40E74(this, play, ++arg2);
}

void func_80B40EE8(EnKgy* this, PlayState* play) {
    s32 pad;

    if (this->unk_2E4 > 0) {
        this->unk_2E4--;
        if ((this->unk_2E4 == 0) && (this->animIndex2 > ENKGY_ANIM_NONE)) {
            if (this->animIndex2 == ENKGY_ANIM_3) {
                EnKgy_ChangeAnim(this, ENKGY_ANIM_3, ANIMMODE_ONCE, -5.0f);
            } else {
                EnKgy_ChangeAnimLoopMorph(this, this->animIndex2);
            }
            this->animIndex2 = ENKGY_ANIM_NONE;
        }
    }

    if ((this->csIdIndex != -1) && (CutsceneManager_GetCurrentCsId() != this->csIdList[this->csIdIndex])) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        } else if (CutsceneManager_IsNext(this->csIdList[this->csIdIndex])) {
            CutsceneManager_StartWithPlayerCs(this->csIdList[this->csIdIndex], &this->actor);
        } else {
            CutsceneManager_Queue(this->csIdList[this->csIdIndex]);
        }
    }

    if (this->unk_29C & 2) {
        Vec3f projectedPos;
        f32 invW;
        f32 temp_f0;

        Actor_GetProjectedPos(play, &this->unk_2B4, &projectedPos, &invW);
        temp_f0 = projectedPos.x * invW;
        if (this->unk_2E6 > 0) {
            this->unk_2E6--;
        }

        if (((temp_f0 < 0.15f) && (temp_f0 > -0.15f)) || (this->unk_2E6 == 0)) {
            Message_ContinueTextbox(play, this->actor.textId);
            this->unk_29C &= ~0x2;
            func_80B40E18(this, this->actor.textId);

            switch (this->actor.textId) {
                case 0xC22:
                    this->unk_2E4 = 20;
                    this->animIndex2 = ENKGY_ANIM_3;
                    break;

                case 0xC43:
                    if (func_80B40D64(play)) {
                        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_7);
                    } else {
                        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
                    }
                    this->unk_2E4 = 30;
                    this->actionFunc = func_80B41A48;
                    break;

                case 0xC52:
                case 0xC54:
                    EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
                    break;

                default:
                    break;
            }
        }
    }

    if (this->unk_29C & 4) {
        if (this->unk_2E8 > 0) {
            this->unk_2E8--;
        } else {
            this->unk_2E8 = 0;
        }
        if (this->unk_2E8 == 0) {
            if ((this->actor.textId == 0xC1D) || (this->actor.textId == 0xC2D)) {
                this->unk_2E4 = 20;
                this->animIndex2 = ENKGY_ANIM_3;
                func_80B40EBC(this, play, this->actor.textId);
            } else {
                func_80B40EBC(this, play, this->actor.textId);
            }
            this->unk_29C &= ~4;
        }
    }
}

void func_80B411DC(EnKgy* this, PlayState* play, s32 arg2) {
    Player* player = GET_PLAYER(play);

    switch (arg2) {
        case 0:
            this->unk_2B4 = this->unk_2A8;
            this->actor.focus.pos = this->unk_2A8;
            this->csIdIndex = 0;
            break;

        case 1:
            if (this->zubora != NULL) {
                this->unk_2B4 = this->zubora->actor.focus.pos;
                this->actor.focus.pos = this->zubora->actor.focus.pos;
            }
            this->csIdIndex = 1;
            break;

        case 2:
            if (this->iceBlock != NULL) {
                this->unk_2B4 = this->iceBlock->actor.world.pos;
                this->actor.focus.pos = this->iceBlock->actor.focus.pos;
            }
            this->csIdIndex = 2;
            break;

        case 3:
            this->unk_2B4 = player->actor.world.pos;
            if (this->zubora != NULL) {
                this->actor.focus.pos = this->zubora->actor.focus.pos;
            }
            this->csIdIndex = 3;
            break;

        case 4:
            if (this->zubora != NULL) {
                this->unk_2B4 = this->zubora->actor.focus.pos;
                this->actor.focus.pos = this->zubora->actor.focus.pos;
            }
            this->csIdIndex = 4;
            break;

        default:
            break;
    }
}

void func_80B41368(EnKgy* this, PlayState* play, s32 arg2) {
    CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
    func_80B411DC(this, play, arg2);
    this->unk_2E6 = 20;
    this->unk_29C |= 2;
}

void func_80B413C8(EnKgy* this) {
    Vec3f sp1C = this->unk_2A8;

    this->unk_2B4 = sp1C;
    this->actor.focus.pos = sp1C;
    CutsceneManager_Stop(this->csIdList[this->csIdIndex]);
    this->csIdIndex = -1;
    this->unk_2E6 = 0;
    this->unk_29C &= ~2;
}

s32 func_80B41460(void) {
    if ((GET_PLAYER_FORM != PLAYER_FORM_HUMAN) ||
        GameInteractor_Should(VB_SMITHY_CHECK_FOR_SWORD, ((CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_KOKIRI) &&
                                                          (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_RAZOR) &&
                                                          (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_GILDED)))) {
        return 0xC38;
    }

    if (CURRENT_DAY == 3) {
        return 0xC39;
    }

    return 0xC3A;
}

s32 func_80B41528(PlayState* play) {
    if (GameInteractor_Should(VB_SMITHY_CHECK_FOR_GILDED_SWORD, CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_SWORD_GILDED)) {
        return 0xC4C;
    }

    if (GameInteractor_Should(VB_SMITHY_CHECK_FOR_RAZOR_SWORD, CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_SWORD_RAZOR)) {
        return 0xC45;
    }

    return 0xC3B;
}

void func_80B415A8(PlayState* play, Vec3f* arg1) {
    static EffectShieldParticleInit D_80B43298 = {
        16,
        { 0, 0, 0 },
        { 0, 200, 255, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 128, 255 },
        { 255, 255, 0, 255 },
        { 255, 64, 0, 200 },
        { 255, 0, 0, 255 },
        2.0f,
        15.0f,
        10.0f,
        4,
        { 0, 0, 0, { 0, 128, 255 }, 0, 300 },
        0,
    };
    s32 effectIndex;

    D_80B43298.position.x = arg1->x;
    D_80B43298.position.y = arg1->y;
    D_80B43298.position.z = arg1->z;
    D_80B43298.lightPoint.x = D_80B43298.position.x;
    D_80B43298.lightPoint.y = D_80B43298.position.y;
    D_80B43298.lightPoint.z = D_80B43298.position.z;

    Effect_Add(play, &effectIndex, EFFECT_SHIELD_PARTICLE, 0, 1, &D_80B43298);
}

void func_80B4163C(EnKgy* this, PlayState* play) {
    this->actor.focus.pos = this->unk_2A8;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->animIndex == ENKGY_ANIM_6) {
            if (this->getItemId > GI_NONE) {
                EnKgy_ChangeAnim(this, ENKGY_ANIM_6, ANIMMODE_ONCE, 0.0f);
                this->getItemId--;
            } else {
                EnKgy_ChangeAnim(this, ENKGY_ANIM_9, ANIMMODE_ONCE, -5.0f);
                this->getItemId = (s32)Rand_ZeroFloat(GI_RUPEE_PURPLE - GI_RUPEE_BLUE) + GI_RUPEE_BLUE;
            }
        } else {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_6, ANIMMODE_ONCE, -5.0f);
        }
        SkelAnime_Update(&this->skelAnime);
    }

    switch ((s32)this->skelAnime.curFrame) {
        case 1:
        case 7:
        case 13:
        case 19:
        case 25:
            func_80B415A8(play, &this->unk_2C0);
            this->lightInfo.params.point.x = this->unk_2C0.x;
            this->lightInfo.params.point.y = this->unk_2C0.y;
            this->lightInfo.params.point.z = this->unk_2C0.z;
            this->unk_300 = 200;
            Actor_PlaySfx(&this->actor, NA_SE_EV_SWORD_FORGE);
            break;

        default:
            break;
    }

    this->lightInfo.params.point.radius = this->unk_300;
    if (this->unk_300 > 0) {
        this->unk_300 -= 50;
    }
}

void func_80B417B8(EnKgy* this, PlayState* play) {
    func_80B4163C(this, play);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_80B413C8(this);
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        this->actionFunc = func_80B419B0;
        func_80B40E18(this, 7);
    }
    func_80B40EE8(this, play);
}

void func_80B41858(EnKgy* this, PlayState* play) {
    func_80B4163C(this, play);
    if (CutsceneManager_IsNext(this->csIdList[5])) {
        CutsceneManager_StartWithPlayerCs(this->csIdList[5], &this->actor);
        this->actionFunc = func_80B419B0;
        func_80B40E18(this, 7);
    } else {
        CutsceneManager_Queue(this->csIdList[5]);
    }
}

void func_80B418C4(EnKgy* this, PlayState* play) {
    func_80B4163C(this, play);
    if ((this->unk_2E4 <= 0) && !(this->unk_29C & 2) && (func_80B40E54(this) == 0) &&
        (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play) &&
        ((play->msgCtx.currentTextId == 0xC4E) || (play->msgCtx.currentTextId == 0xC4F))) {
        Message_CloseTextbox(play);
        this->actor.textId = 0xC4F;
        func_80B413C8(this);
        CutsceneManager_Queue(this->csIdList[5]);
        this->actionFunc = func_80B41858;
        this->actor.flags &= ~ACTOR_FLAG_TALK;
    }
    func_80B40EE8(this, play);
}

void func_80B419B0(EnKgy* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    func_80B4163C(this, play);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state) || (&this->actor == player->talkActor)) {
        func_80B411DC(this, play, 4);
        func_80B40E18(this, this->actor.textId);
        if (this->actor.textId == 0xC37) {
            this->actionFunc = func_80B417B8;
        } else {
            this->actionFunc = func_80B418C4;
        }
    }
}

void func_80B41A48(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_2E4 > 0) {
        this->unk_2E4--;
    } else {
        play->nextEntrance = play->setupExitList[ENKGY_GET_1F(&this->actor)];
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void func_80B41ACC(EnKgy* this, PlayState* play) {
    PlayerItemAction itemAction;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_PAUSE_MENU) {
        itemAction = func_80123810(play);
        if (itemAction != PLAYER_IA_NONE) {
            this->actionFunc = func_80B41E18;
        }

        if (itemAction > PLAYER_IA_NONE) {
            Message_CloseTextbox(play);
            if (itemAction == PLAYER_IA_BOTTLE_GOLD_DUST) {
                if (this->unk_29C & 0x10) {
                    this->actor.textId = 0xC55;
                    player->actor.textId = 0xC55;
                } else {
                    this->actor.textId = 0xC46;
                    player->actor.textId = 0xC46;
                    func_80B40D00(play);
                }
            } else if (this->unk_29C & 0x10) {
                this->actor.textId = 0xC57;
                player->actor.textId = 0xC57;
            } else {
                this->actor.textId = 0xC47;
                player->actor.textId = 0xC47;
            }
            this->unk_29C |= 8;
        } else if (itemAction <= PLAYER_IA_MINUS1) {
            if (this->unk_29C & 0x10) {
                this->actor.textId = 0xC57;
            } else {
                this->actor.textId = 0xC47;
            }
            player->actor.textId = 0;
            play->msgCtx.msgLength = 0;
            func_80B41368(this, play, 4);
        }
    }
    func_80B40EE8(this, play);
}

void func_80B41C30(EnKgy* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (&this->actor != player->talkActor) {
        this->actionFunc = func_80B42508;
    }
}

void func_80B41C54(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = func_80B41C30;
        this->actor.flags &= ~ACTOR_FLAG_TALK;
    }
    func_80B40EE8(this, play);
}

void func_80B41CBC(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        func_80B40E18(this, this->actor.textId);
        this->actionFunc = func_80B41E18;
        func_80B411DC(this, play, 4);
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void func_80B41D64(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Actor_HasParent(&this->actor, play)) {
        this->actionFunc = func_80B41CBC;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->getItemId, 2000.0f, 1000.0f);
    }
    func_80B40EE8(this, play);
}

void func_80B41E18(EnKgy* this, PlayState* play) {
    u16 textId;
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (SkelAnime_Update(&this->skelAnime) && (this->animIndex == ENKGY_ANIM_3)) {
        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
    }

    if ((this->unk_2E4 <= 0) && !(this->unk_29C & 2) && (func_80B40E54(this) == 0)) {
        switch (Message_GetState(&play->msgCtx)) {
            case TEXT_STATE_CHOICE:
                if (Message_ShouldAdvance(play)) {
                    textId = play->msgCtx.currentTextId;

                    switch (textId) {
                        case 0xC3B:
                            switch (play->msgCtx.choiceIndex) {
                                case 0:
                                    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
                                        Audio_PlaySfx(NA_SE_SY_ERROR);
                                        func_80B40E74(this, play, 0xC3F);
                                    } else {
                                        Audio_PlaySfx_MessageDecide();
                                        func_80B40E74(this, play, 0xC42);
                                        Rupees_ChangeBy(-play->msgCtx.unk1206C);
                                    }
                                    break;

                                case 1:
                                    Audio_PlaySfx_MessageCancel();
                                    func_80B40EBC(this, play, textId);
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case 0xC3E:
                            switch (play->msgCtx.choiceIndex) {
                                case 0:
                                    Audio_PlaySfx_MessageDecide();
                                    func_80B40E74(this, play, func_80B41460());
                                    break;

                                case 1:
                                    Audio_PlaySfx_MessageCancel();
                                    func_80B40E74(this, play, 0xC3C);
                                    break;

                                default:
                                    break;
                            }
                            break;

                        default:
                            break;
                    }
                }
                break;

            case TEXT_STATE_EVENT:
                if (Message_ShouldAdvance(play)) {
                    textId = play->msgCtx.currentTextId;

                    switch (textId) {
                        case 0xC35:
                            play->msgCtx.msgLength = 0;
                            this->actor.textId = textId;
                            this->unk_29C |= 4;
                            this->unk_2E8 = 3;
                            func_80B40E38(this);
                            break;

                        case 0xC36:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 4);
                            this->actor.textId = func_80B41460();
                            break;

                        case 0xC38:
                        case 0xC39:
                        case 0xC52:
                        case 0xC54:
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B425A0;
                            func_80B413C8(this);
                            func_80B40E18(this, 5);
                            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
                            this->actor.textId = 0xC35;
                            break;

                        case 0xC3A:
                            func_80B40E74(this, play, func_80B41528(play));
                            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
                            break;

                        case 0xC3C:
                        case 0xC3F:
                        case 0xC4C:
                            func_80B40EBC(this, play, textId);
                            break;

                        case 0xC3D:
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B41C54;
                            this->actor.textId = 0xC3E;
                            func_80B413C8(this);
                            func_80B40E18(this, 2);
                            break;

                        case 0xC40:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 0);
                            this->actor.textId = 0xC43;
                            break;

                        case 0xC42:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 0);
                            this->actor.textId = 0xC43;
                            if (GameInteractor_Should(VB_SMITHY_START_UPGRADING_SWORD, true, this)) {
                                CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_NONE;
                                SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_NONE);
                                Interface_LoadItemIconImpl(play, EQUIP_SLOT_B);
                                func_80B40C74(play);
                            }
                            break;

                        case 0xC4D:
                        case 0xC58:
                            this->unk_29C |= 0x10;

                        case 0xC45:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 3);
                            this->actor.textId = 0xFF;
                            this->actionFunc = func_80B41ACC;
                            break;

                        case 0xC57:
                            this->unk_29C &= ~0x8;
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B41C54;
                            this->actor.textId = 0xC58;
                            func_80B413C8(this);
                            func_80B40E18(this, 3);
                            break;

                        case 0xC46:
                        case 0xC55:
                            Player_UpdateBottleHeld(play, GET_PLAYER(play), ITEM_BOTTLE, PLAYER_IA_BOTTLE_EMPTY);
                            player->exchangeItemAction = PLAYER_IA_NONE;
                            this->unk_29C &= ~0x8;
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 4);
                            if (this->unk_29C & 0x10) {
                                this->actor.textId = 0xC56;
                                Rupees_ChangeBy(play->msgCtx.unk1206C);
                            } else {
                                this->actor.textId = 0xC42;
                            }
                            break;

                        case 0xC47:
                            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
                            if (this->unk_29C & 8) {
                                player->exchangeItemAction = PLAYER_IA_NONE;
                                this->unk_29C &= ~8;
                            }
                            func_80B40EBC(this, play, textId);
                            break;

                        case 0xC48:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 0);
                            this->actor.textId = textId + 1;
                            break;

                        case 0xC49:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 4);
                            this->actor.textId = textId + 1;
                            break;

                        case 0xC4A:
                        case 0xC4B:
                            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B41C54;
                            this->actor.textId = 0xC4B;
                            func_80B413C8(this);
                            func_80B40E18(this, 4);
                            break;

                        case 0xC50:
                            if (func_80B40D8C(play)) {
                                this->getItemId = GI_SWORD_GILDED;
                                this->actor.textId = 0xC53;
                            } else {
                                this->getItemId = GI_SWORD_RAZOR;
                                this->actor.textId = 0xC51;
                            }
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B41D64;
                            func_80B413C8(this);
                            Actor_OfferGetItem(&this->actor, play, this->getItemId, 2000.0f, 1000.0f);
                            break;

                        case 0xC51:
                        case 0xC53:
                            play->msgCtx.msgLength = 0;
                            func_80B41368(this, play, 0);
                            this->actor.textId = textId + 1;
                            func_80B40D30(play);
                            break;

                        case 0xC56:
                            Message_CloseTextbox(play);
                            this->actionFunc = func_80B41C54;
                            this->actor.textId = 0xC56;
                            func_80B413C8(this);
                            func_80B40E18(this, 8);
                            break;

                        default:
                            break;
                    }
                }
                break;

            default:
                break;
        }
    }

    func_80B40EE8(this, play);
}

void func_80B42508(EnKgy* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->actor.focus.pos = this->unk_2A8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state) || (&this->actor == player->talkActor)) {
        this->actionFunc = func_80B41E18;
        func_80B411DC(this, play, 4);
        func_80B40E18(this, this->actor.textId);
    }
}

void func_80B425A0(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.focus.pos = this->unk_2A8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B41E18;
        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
        func_80B411DC(this, play, 0);
        func_80B40E18(this, this->actor.textId);
    } else if (this->actor.xzDistToPlayer < 200.0f) {
        Actor_OfferTalk(&this->actor, play, 210.0f);
    }
}

void func_80B42660(EnKgy* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->actor.focus.pos = this->unk_2A8;
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_80B413C8(this);
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        this->actionFunc = func_80B42714;
        func_80B40E18(this, 7);
    }
    func_80B40EE8(this, play);
}

void func_80B42714(EnKgy* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->actor.focus.pos = this->unk_2A8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state) || (&this->actor == player->talkActor)) {
        func_80B411DC(this, play, 4);
        func_80B40E18(this, this->actor.textId);
        if (this->actor.textId == 0xC37) {
            this->actionFunc = func_80B42660;
        } else {
            this->actionFunc = func_80B41E18;
        }
    }
}

void func_80B427C8(EnKgy* this, PlayState* play) {
    s32 pad;
    u16 textId;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->animIndex == ENKGY_ANIM_5) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_7);
        }
        if (this->animIndex == ENKGY_ANIM_3) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
        }
    }

    if ((this->unk_2E4 <= 0) && !(this->unk_29C & 2) && (func_80B40E54(this) == 0) &&
        (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        textId = play->msgCtx.currentTextId;

        switch (textId) {
            case 0xC30:
                play->msgCtx.msgLength = 0;
                this->actor.textId = textId;
                this->unk_29C |= 4;
                this->unk_2E8 = 3;
                break;

            case 0xC31:
                play->msgCtx.msgLength = 0;
                func_80B41368(this, play, 4);
                this->actor.textId = textId + 1;
                break;

            case 0xC32:
            case 0xC33:
                func_80B40EBC(this, play, textId);
                break;

            case 0xC34:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B41C54;
                func_80B413C8(this);
                func_80B40E18(this, 6);
                EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
                this->actor.textId = 0xC3A;
                break;

            default:
                break;
        }
    }
    func_80B40EE8(this, play);
}

void func_80B4296C(EnKgy* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->animIndex == ENKGY_ANIM_8)) {
        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_2);
    }

    this->actor.focus.pos = this->unk_2A8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B427C8;
        if (this->animIndex == ENKGY_ANIM_4) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_7);
        } else {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_5, ANIMMODE_ONCE, -5.0f);
        }
        func_80B411DC(this, play, 0);
        func_80B40E18(this, this->actor.textId);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_NONE);
    }
}

void func_80B42A8C(EnKgy* this, PlayState* play) {
    u16 textId;
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->animIndex == ENKGY_ANIM_5) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
        }

        if (this->animIndex == ENKGY_ANIM_3) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_4);
        }
    }

    if ((this->unk_2E4 <= 0) && !(this->unk_29C & 2) && (func_80B40E54(this) == 0) &&
        (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        textId = play->msgCtx.currentTextId;
        switch (textId) {
            case 0xC1D:
            case 0xC2D:
                play->msgCtx.msgLength = 0;
                this->actor.textId = textId;
                this->unk_29C |= 4;
                this->unk_2E8 = 3;
                break;

            case 0xC1E:
                play->msgCtx.msgLength = 0;
                func_80B41368(this, play, 1);
                this->actor.textId = textId + 1;
                func_80B40E38(this);
                break;

            case 0xC1F:
            case 0xC23:
            case 0xC24:
            case 0xC27:
                func_80B40EBC(this, play, textId);
                break;

            case 0xC20:
            case 0xC28:
                EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
                play->msgCtx.msgLength = 0;
                this->unk_29C |= 4;
                this->unk_2E8 = 3;
                this->actor.textId = textId;
                func_80B40E38(this);
                break;

            case 0xC21:
            case 0xC29:
                play->msgCtx.msgLength = 0;
                func_80B41368(this, play, 0);
                this->actor.textId = textId + 1;
                break;

            case 0xC22:
            case 0xC26:
            case 0xC2B:
            case 0xC2E:
                play->msgCtx.msgLength = 0;
                func_80B41368(this, play, 1);
                this->actor.textId = textId + 1;
                break;

            case 0xC25:
                play->msgCtx.msgLength = 0;
                func_80B41368(this, play, 2);
                this->actor.textId = textId + 1;
                break;

            case 0xC2A:
                this->unk_2E4 = 20;
                this->animIndex2 = ENKGY_ANIM_3;
                func_80B40EBC(this, play, this->actor.textId);
                break;

            case 0xC2C:
            case 0xC2F:
                Message_CloseTextbox(play);
                this->actionFunc = func_80B42D28;
                func_80B413C8(this);
                func_80B40E18(this, 1);
                break;

            default:
                break;
        }
    }
    func_80B40EE8(this, play);
}

void func_80B42D28(EnKgy* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->animIndex == ENKGY_ANIM_8)) {
        EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_2);
    }

    this->actor.focus.pos = this->unk_2A8;
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B42A8C;
        if (this->actor.textId == 0xC2D) {
            EnKgy_ChangeAnimLoopMorph(this, ENKGY_ANIM_1);
        } else {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_5, ANIMMODE_ONCE, -5.0f);
            SET_WEEKEVENTREG(WEEKEVENTREG_20_80);
        }
        func_80B411DC(this, play, 0);
        func_80B40E18(this, this->actor.textId);
    } else {
        if (Flags_GetSwitch(play, ENKGY_GET_SWITCH_FLAG(&this->actor))) {
            this->actor.textId = 0xC30;
            this->actionFunc = func_80B4296C;
            SET_WEEKEVENTREG(WEEKEVENTREG_21_01);
        } else if (this->actor.xzDistToPlayer < 200.0f) {
            if (this->animIndex == ENKGY_ANIM_4) {
                this->actor.textId = 0xC2D;
            } else {
                this->actor.textId = 0xC1D;
            }
            Actor_OfferTalk(&this->actor, play, 210.0f);
        }

        if ((this->animIndex == ENKGY_ANIM_0) && (this->actor.xzDistToPlayer < 200.0f)) {
            EnKgy_ChangeAnim(this, ENKGY_ANIM_8, ANIMMODE_ONCE, 5.0f);
        }
    }
}

void EnKgy_Update(Actor* thisx, PlayState* play) {
    EnKgy* this = (EnKgy*)thisx;
    s32 pad;
    Vec3s torsoRot;

    this->actionFunc(this, play);
    if (this->animIndex == ENKGY_ANIM_2) {
        torsoRot.z = 0;
        torsoRot.y = 0;
        torsoRot.x = 0;
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
    }
}

s32 EnKgy_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKgy* this = (EnKgy*)thisx;

    if (!(this->unk_29C & 1)) {
        if (limbIndex == OBJECT_KGY_LIMB_11) {
            *dList = NULL;
        }
    } else if (limbIndex == OBJECT_KGY_LIMB_10) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_KGY_LIMB_0B) {
        rot->x += this->headRot.y;
    }

    return false;
}

void EnKgy_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80B432D8 = { 1000.0f, 2000.0f, 0.0f };
    static Vec3f D_80B432E4 = { 3000.0f, 4000.0f, 300.0f };
    EnKgy* this = (EnKgy*)thisx;

    if (limbIndex == OBJECT_KGY_LIMB_0B) {
        Matrix_MultVec3f(&D_80B432D8, &this->unk_2A8);
    }

    if (limbIndex == OBJECT_KGY_LIMB_10) {
        Matrix_MultVec3f(&D_80B432E4, &this->unk_2C0);
    }
}

void func_80B43074(EnKgy* this, PlayState* play) {
    s32 pad;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);
    Matrix_Push();
    Matrix_Translate(-800.0f, 3100.0f, 8400.0f, MTXMODE_APPLY);
    Matrix_RotateXS(0x4000, MTXMODE_APPLY);

    if (func_80B40D8C(play)) {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_kgy_Matanimheader_00F6A0));
    } else {
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_kgy_Matanimheader_00EE58));
    }

    gfx = POLY_OPA_DISP;
    MATRIX_FINALIZE_AND_LOAD(gfx, play->state.gfxCtx);

    if (func_80B40D8C(play)) {
        gSPDisplayList(&gfx[1], gRazorSwordHandleDL);
        gSPDisplayList(&gfx[2], object_kgy_DL_00F180);
    } else {
        gSPDisplayList(&gfx[1], gKokiriSwordHandleDL);
        gSPDisplayList(&gfx[2], object_kgy_DL_00E8F0);
    }
    POLY_OPA_DISP = &gfx[3];

    CLOSE_DISPS(play->state.gfxCtx);

    Matrix_Pop();
}

void EnKgy_Draw(Actor* thisx, PlayState* play) {
    EnKgy* this = (EnKgy*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->unk_29C & 1) {
        func_80B43074(this, play);
    }
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnKgy_OverrideLimbDraw, EnKgy_PostLimbDraw, &this->actor);
}

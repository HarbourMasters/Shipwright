/*
 * File: z_en_zoraegg.c
 * Overlay: ovl_En_Zoraegg
 * Description: Zora Egg
 */

#include "z_en_zoraegg.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnZoraegg_Init(Actor* thisx, PlayState* play);
void EnZoraegg_Destroy(Actor* thisx, PlayState* play);
void EnZoraegg_Update(Actor* thisx, PlayState* play);
void EnZoraegg_Draw(Actor* thisx, PlayState* play);

s32 func_80B319A8(PlayState* play);
void func_80B32084(EnZoraegg* this, PlayState* play);
void func_80B32094(EnZoraegg* this, PlayState* play);
void func_80B320E0(EnZoraegg* this, PlayState* play);
void func_80B321D0(EnZoraegg* this, PlayState* play);
void func_80B32228(EnZoraegg* this, PlayState* play);
void func_80B322BC(EnZoraegg* this, PlayState* play);
void func_80B32390(EnZoraegg* this, PlayState* play);
void func_80B324B0(EnZoraegg* this, PlayState* play);
void func_80B32644(EnZoraegg* this, PlayState* play);
void func_80B326F4(EnZoraegg* this, PlayState* play);
void func_80B32820(EnZoraegg* this, PlayState* play);
void func_80B32928(EnZoraegg* this, PlayState* play);
void func_80B32A88(EnZoraegg* this, PlayState* play);
void func_80B32B10(EnZoraegg* this, PlayState* play);
void func_80B32B3C(EnZoraegg* this, PlayState* play);
void func_80B32B70(EnZoraegg* this, PlayState* play);
void func_80B32BB8(EnZoraegg* this, PlayState* play);
void func_80B32C34(EnZoraegg* this, PlayState* play);
void func_80B32D08(EnZoraegg* this, PlayState* play);

ActorProfile En_Zoraegg_Profile = {
    /**/ ACTOR_EN_ZORAEGG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_ZORAEGG,
    /**/ sizeof(EnZoraegg),
    /**/ EnZoraegg_Init,
    /**/ EnZoraegg_Destroy,
    /**/ EnZoraegg_Update,
    /**/ EnZoraegg_Draw,
};

void func_80B31590(EnZoraegg* this) {
    this->actor.shape.yOffset = 0.0f;

    if (this->actor.home.rot.x == 0) {
        this->actor.scale.x = 0.4f;
    } else {
        this->actor.scale.x = this->actor.home.rot.x * 0.04f;
    }

    if (this->actor.home.rot.z == 0) {
        this->actor.scale.z = 0.4f;
    } else {
        this->actor.scale.z = this->actor.home.rot.z * 0.04f;
    }

    this->actor.shape.rot.z = 0;
    this->actor.scale.y = 0.4f;
    this->actor.draw = NULL;
    this->actor.shape.rot.y = this->actor.shape.rot.z;
    this->actor.shape.rot.x = this->actor.shape.rot.z;
}

void EnZoraegg_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZoraegg* this = (EnZoraegg*)thisx;
    u16 cueTypes[] = {
        CS_CMD_ACTOR_CUE_457, CS_CMD_ACTOR_CUE_458, CS_CMD_ACTOR_CUE_459, CS_CMD_ACTOR_CUE_460,
        CS_CMD_ACTOR_CUE_461, CS_CMD_ACTOR_CUE_462, CS_CMD_ACTOR_CUE_464,
    };

    Actor_SetScale(&this->actor, 0.006f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gZoraBabySkel, &gZoraBabySwimAnim, this->jointTable, this->morphTable,
                       ZORA_BABY_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gZoraBabySwimAnim);
    ActorShape_Init(&this->actor.shape, 1100.0f, NULL, 0.0f);

    this->actionFunc = func_80B32084;

    this->unk_1ED = 255;
    this->unk_1EC = 0;
    this->unk_1EA = 0;

    this->unk_1E0 = 1.0f;
    this->unk_1E4 = 0.0f;
    this->actor.velocity.y = -10.0f;
    this->actor.terminalVelocity = -10.0f;
    this->actor.gravity = -5.0f;

    switch (ZORA_EGG_GET_TYPE(&this->actor)) {
        case ZORA_EGG_TYPE_00:
            if (Flags_GetSwitch(play, ZORA_EGG_GET_SWITCH_FLAG(&this->actor))) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case ZORA_EGG_TYPE_11:
            if (func_80B319A8(play) >= 7) {
                Actor_Kill(&this->actor);
                this->actor.home.rot.z = 1;
                return;
            }
            break;

        case ZORA_EGG_TYPE_03:
        case ZORA_EGG_TYPE_04:
        case ZORA_EGG_TYPE_05:
        case ZORA_EGG_TYPE_06:
        case ZORA_EGG_TYPE_07:
        case ZORA_EGG_TYPE_08:
        case ZORA_EGG_TYPE_09:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_19_40)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case ZORA_EGG_TYPE_0A:
        case ZORA_EGG_TYPE_0B:
        case ZORA_EGG_TYPE_0C:
        case ZORA_EGG_TYPE_0D:
        case ZORA_EGG_TYPE_0E:
        case ZORA_EGG_TYPE_0F:
        case ZORA_EGG_TYPE_10:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_19_40)) {
                Actor_Kill(&this->actor);
                return;
            }
            break;
    }

    switch (ZORA_EGG_GET_TYPE(&this->actor)) {
        case ZORA_EGG_TYPE_00:
            this->actionFunc = func_80B320E0;
            this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
            this->actor.attentionRangeType = ATTENTION_RANGE_3;
            break;

        case ZORA_EGG_TYPE_01:
            this->actionFunc = func_80B322BC;
            func_80B31590(this);
            break;

        case ZORA_EGG_TYPE_02:
            this->actionFunc = func_80B32390;
            func_80B31590(this);
            break;

        case ZORA_EGG_TYPE_03:
        case ZORA_EGG_TYPE_04:
        case ZORA_EGG_TYPE_05:
        case ZORA_EGG_TYPE_06:
        case ZORA_EGG_TYPE_07:
        case ZORA_EGG_TYPE_08:
        case ZORA_EGG_TYPE_09:
            this->cueType = cueTypes[ZORA_EGG_GET_TYPE(&this->actor) - ZORA_EGG_TYPE_03];
            Animation_PlayOnce(&this->skelAnime, &gZoraBabyHatchAnim);
            this->unk_1EC = 1;
            this->unk_1EE = 0;
            this->unk_1EF = 0;
            this->actionFunc = func_80B32B10;
            if ((ZORA_EGG_GET_TYPE(&this->actor) - ZORA_EGG_TYPE_03) >= func_80B319A8(play)) {
                this->actionFunc = func_80B32B3C;
                this->actor.draw = NULL;
            }
            break;

        case ZORA_EGG_TYPE_0A:
        case ZORA_EGG_TYPE_0B:
        case ZORA_EGG_TYPE_0C:
        case ZORA_EGG_TYPE_0D:
        case ZORA_EGG_TYPE_0E:
        case ZORA_EGG_TYPE_0F:
        case ZORA_EGG_TYPE_10:
            this->cueType = cueTypes[ZORA_EGG_GET_TYPE(&this->actor) - ZORA_EGG_TYPE_0A];
            this->unk_1EC = 2;
            this->actionFunc = func_80B324B0;
            Animation_PlayLoop(&this->skelAnime, &gZoraBabyNoteAnim);
            this->unk_1EE = 0;
            this->unk_1ED = 0;
            this->unk_1EA |= 3;
            break;

        case ZORA_EGG_TYPE_11:
            Actor_SetScale(&this->actor, 0.0006f);
            this->actionFunc = func_80B32D08;
            this->actor.world.pos.y -= this->actor.shape.yOffset * this->actor.scale.y;
            this->actor.shape.rot.x = 0;
            this->actor.velocity.y = 0.0f;
            this->actor.terminalVelocity = -10.0f;
            this->actor.gravity = -1.0f;
            break;
    }
}

void EnZoraegg_Destroy(Actor* thisx, PlayState* play) {
}

s32 func_80B319A8(PlayState* play) {
    return gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 7;
}

void func_80B319D0(PlayState* play, s32 arg1) {
    if ((arg1 < 8) && (arg1 >= 0)) {
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 &= ~7;
        gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 |= arg1;
    }
}

void func_80B31A34(EnZoraegg* this) {
    s32 curFrame;

    if (this->unk_1EA & 1) {
        this->unk_1EE = this->unk_1EF = 100;
        return;
    }

    curFrame = this->skelAnime.curFrame;
    if (curFrame < 6) {
        this->unk_1EE = 0;
        this->unk_1EF = 0;
    } else if (curFrame < 93) {
        this->unk_1EE = Math_SinS((curFrame * 0xBC) - 0x468) * 100.0f;
        if (curFrame >= 89) {
            this->unk_1EF = Math_SinS((curFrame * 0x1000) - (0x59 * 0x1000)) * 100.0f;
        } else {
            this->unk_1EF = 0;
        }
    } else {
        this->unk_1EE = this->unk_1EF = 100;
    }
}

void func_80B31C40(EnZoraegg* this, PlayState* play) {
    Actor_MoveWithGravity(&this->actor);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
    this->actor.focus.pos.y += 10.0f;
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

Actor* func_80B31CB4(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_ZORAEGG) && (ZORA_EGG_GET_TYPE(actor) == ZORA_EGG_TYPE_11) &&
            (actor->home.rot.z == 0)) {
            actor->home.rot.z = 1;
            return actor;
        }
        actor = actor->next;
    }
    return NULL;
}

Actor* func_80B31D14(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_ZORAEGG) && (ZORA_EGG_GET_TYPE(actor) == ZORA_EGG_TYPE_03)) {
            return actor;
        }
        actor = actor->next;
    }
    return NULL;
}

void func_80B31D64(EnZoraegg* this, PlayState* play, s32 arg2, f32 arg3) {
    Vec3f sp24 = this->actor.world.pos;

    sp24.y += arg3;

    if ((play->gameplayFrames & arg2) == 0) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
    }
}

void func_80B31E00(EnZoraegg* this) {
    f32 sp20[] = {
        1.0f,
        1.075f,
        1.15f,
        1.075f,
    };

    if (this->unk_1E8 < 112) {
        if (this->unk_1E8 > 10) {
            this->unk_1ED = (s32)((112.0f - this->unk_1E8) * 2.5f) & 0xFF;
        }
    } else {
        this->unk_1ED = 0;
    }

    if (this->unk_1E8 < 10) {
        this->unk_1E0 = sp20[this->unk_1E8 & 3];
    } else if (this->unk_1E8 < 22) {
        this->unk_1E0 = (Math_SinS((this->unk_1E8 - 10.0f) * (4096.0f / 3)) * 0.8f) + 1.0f;
    } else if (this->unk_1E8 < 26) {
        this->unk_1E0 = (Math_CosS((this->unk_1E8 - 22.0f) * 4096.0f) * 0.8f) + 1.0f;
    } else if (this->unk_1E8 < 32) {
        this->unk_1E0 = sp20[(this->unk_1E8 - 1) & 3];
    } else if (this->unk_1E8 < 47) {
        this->unk_1E0 = 1.0f;
    } else if (this->unk_1E8 < 53) {
        this->unk_1E0 = sp20[(this->unk_1E8 - 2) & 3];
    } else if (this->unk_1E8 < 74) {
        this->unk_1E0 = 1.0f - ((this->unk_1E8 - 53.0f) * (1.0f / 105));
    } else if (this->unk_1E8 < 112) {
        this->unk_1E0 = (this->unk_1E0 * 0.9f) + 0.15f;
    }

    if ((this->unk_1E8 == 111) || (this->unk_1E8 == 32)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_BORN);
    }
}

void func_80B32084(EnZoraegg* this, PlayState* play) {
}

void func_80B32094(EnZoraegg* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actionFunc = func_80B320E0;
    }
    func_80B31C40(this, play);
}

void func_80B320E0(EnZoraegg* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        Flags_SetSwitch(play, ZORA_EGG_GET_SWITCH_FLAG(&this->actor));
        Actor_Kill(&this->actor);
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B32094;
        Message_StartTextbox(play, 0x24B, &this->actor);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MAX, 80.0f, 60.0f);
        if (this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 110.0f);
        }
    }

    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    func_80B31C40(this, play);
}

void func_80B321D0(EnZoraegg* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() != this->actor.csId) {
        this->actionFunc = func_80B322BC;
        func_80B319D0(play, func_80B319A8(play) + 1);
    }
}

void func_80B32228(EnZoraegg* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, this->unk_1DC);
        this->actionFunc = func_80B321D0;
    } else {
        if (this->unk_1E8 > 0) {
            this->unk_1E8--;
        } else if (this->unk_1E8 == 0) {
            CutsceneManager_Stop(player->csId);
            player->csId = CS_ID_NONE;
            this->unk_1E8 = -1;
        }

        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80B322BC(EnZoraegg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.csId != CS_ID_NONE) {
        this->unk_1DC = func_80B31CB4(play);
        if (this->unk_1DC != NULL) {
            this->unk_1E8 = 3;
            this->actionFunc = func_80B32228;
            return;
        }
    }

    if ((fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < (100.0f * this->actor.scale.x)) &&
        (fabsf(player->actor.world.pos.z - this->actor.world.pos.z) < (100.0f * this->actor.scale.z))) {
        func_8012300C(play, 25);
    }
}

void func_80B32390(EnZoraegg* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_IsNext(this->actor.csId)) {
        Actor* temp_v0 = func_80B31D14(play);

        if (temp_v0 != NULL) {
            CutsceneManager_StartWithPlayerCs(this->actor.csId, temp_v0);
            SET_EVENTINF(EVENTINF_33);
            Actor_Kill(&this->actor);
        }
    } else if ((func_80B319A8(play) >= 7) &&
               (fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < (100.0f * this->actor.scale.x)) &&
               (fabsf(player->actor.world.pos.z - this->actor.world.pos.z) < (100.0f * this->actor.scale.z)) &&
               (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 30.0f)) {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80B324B0(EnZoraegg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, this->cueType)) {
        if (this->unk_1EA & 4) {
            if (Cutscene_IsCueInChannel(play, this->cueType) &&
                (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 3)) {
                Animation_PlayLoop(&this->skelAnime, &gZoraBabyNoteAnim);
                this->unk_1EA &= ~4;
            }
        } else {
            if (Cutscene_IsCueInChannel(play, this->cueType) &&
                (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 4)) {
                Animation_PlayLoop(&this->skelAnime, &gZoraBabySongLearnedAnim);
                this->unk_1EA |= 4;
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

        if ((this->unk_1EA & 4) && Animation_OnFrame(&this->skelAnime, this->unk_1E4)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_SWIM_1);
            this->unk_1E4 = Rand_ZeroFloat(5.0f);
        }
    } else if (this->unk_1EA & 4) {
        Animation_PlayLoop(&this->skelAnime, &gZoraBabyNoteAnim);
        this->unk_1EA &= ~4;
    }
}

void func_80B32644(EnZoraegg* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayLoop(&this->skelAnime, &gZoraBabyNoteAnim);
        this->unk_1EE = 0;
        this->unk_1EA |= 2;
    }

    if (!Cutscene_IsCueInChannel(play, this->cueType)) {
        this->actionFunc = func_80B324B0;
    } else {
        Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

        if (this->unk_1EE > 25) {
            this->unk_1EE -= 25;
        } else {
            this->unk_1EE = 0;
        }
    }
}

void func_80B326F4(EnZoraegg* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 3)) {
        Animation_Change(&this->skelAnime, &gZoraBabyFormNoteAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gZoraBabyFormNoteAnim), ANIMMODE_ONCE, 5.0f);
        this->unk_1E8 = 0;
        this->actionFunc = func_80B32644;
        SET_WEEKEVENTREG(WEEKEVENTREG_19_40);
        this->unk_1EC = 2;
        this->unk_1EE = 100;
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_SWIM_2);
    }

    Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_SWIM_1);
    }
}

void func_80B32820(EnZoraegg* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_1E8 >= 2) {
            this->actionFunc = func_80B326F4;
            Animation_Change(&this->skelAnime, &gZoraBabySwimAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gZoraBabySwimAnim), ANIMMODE_LOOP, 10.0f);
            this->unk_1E8 = 0;
            SkelAnime_Update(&this->skelAnime);
            return;
        }

        Animation_PlayOnce(&this->skelAnime, &gZoraBabySwimUpAnim);
        this->unk_1E8 += 1;
        SkelAnime_Update(&this->skelAnime);
    }

    Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

    if (Animation_OnFrame(&this->skelAnime, 16.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_SWIM_0);
    }
}

void func_80B32928(EnZoraegg* this, PlayState* play) {
    if (this->unk_1E8 < 112) {
        this->unk_1E8++;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayOnce(&this->skelAnime, &gZoraBabySwimUpAnim);
        this->unk_1EA |= 1;
        this->actionFunc = func_80B32820;
        this->unk_1E8 = 0;
        this->actor.velocity.y = 0.0f;
        return;
    }

    func_80B31A34(this);
    func_80B31E00(this);

    if (this->unk_1E8 < 112) {
        if ((this->unk_1E8 >= 69) && (this->unk_1E8 < 72)) {
            func_80B31D64(this, play, 0, 0.0f);
            func_80B31D64(this, play, 0, 0.0f);
            func_80B31D64(this, play, 0, 0.0f);
        } else {
            func_80B31D64(this, play, 0xD, 0.0f);
        }
    }

    Cutscene_ActorTranslateAndYaw(&this->actor, play, Cutscene_GetCueChannel(play, this->cueType));

    if (Animation_OnFrame(&this->skelAnime, 97.0f) || Animation_OnFrame(&this->skelAnime, 101.0f) ||
        Animation_OnFrame(&this->skelAnime, 105.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_ZORA_KIDS_SWIM_1);
    }
}

void func_80B32A88(EnZoraegg* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 2)) {
        this->unk_1E8 = 0;
        this->actionFunc = func_80B32928;
    }
    func_80B31D64(this, play, 13, 0.0f);
}

void func_80B32B10(EnZoraegg* this, PlayState* play) {
    if (CHECK_EVENTINF(EVENTINF_33)) {
        this->actionFunc = func_80B32A88;
    }
}

void func_80B32B3C(EnZoraegg* this, PlayState* play) {
    if (CHECK_EVENTINF(EVENTINF_33)) {
        this->actionFunc = func_80B32A88;
        this->actor.draw = EnZoraegg_Draw;
    }
}

void func_80B32B70(EnZoraegg* this, PlayState* play) {
    func_80B31C40(this, play);
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_457)) {
        Actor_Kill(&this->actor);
    }
}

void func_80B32BB8(EnZoraegg* this, PlayState* play) {
    func_80B31C40(this, play);
    func_80B31D64(this, play, 13, 0.0f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actionFunc = func_80B32B70;
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_457)) {
        Actor_Kill(&this->actor);
    }
}

void func_80B32C34(EnZoraegg* this, PlayState* play) {
    WaterBox* waterBox;
    f32 sp30;
    s32 pad;

    Actor_MoveWithGravity(&this->actor);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
    this->actor.focus.pos.y += 10.0f;
    sp30 = this->actor.world.pos.y;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp30, &waterBox)) {
        if ((this->actor.world.pos.y + 50.0f) < sp30) {
            this->actionFunc = func_80B32BB8;
        }

        func_80B31D64(this, play, 0, -20.0f);
        func_80B31D64(this, play, 0, -20.0f);
    }
}

void func_80B32D08(EnZoraegg* this, PlayState* play) {
    WaterBox* waterBox;
    f32 sp40;
    Vec3f sp34;
    s32 pad;

    Actor_MoveWithGravity(&this->actor);
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
    this->actor.focus.pos.y += 10.0f;
    sp40 = this->actor.world.pos.y;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp40, &waterBox)) {
        if (this->actor.world.pos.y < sp40) {
            sp34.x = this->actor.world.pos.x;
            sp34.y = sp40;
            sp34.z = this->actor.world.pos.z;

            EffectSsGRipple_Spawn(play, &sp34, 150, 500, 0);
            EffectSsGSplash_Spawn(play, &sp34, NULL, NULL, 0, 200);
            Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_INTO_WATER_L);

            this->actionFunc = func_80B32C34;
            this->actor.velocity.y = -1.0f;
            this->actor.terminalVelocity = -1.0f;
            this->actor.gravity = -1.0f;
        }
    }

    if (this->actor.scale.x < 0.006f) {
        Actor_SetScale(&this->actor, this->actor.scale.x + 0.0012f);
    }

    if (this->actor.scale.x > 0.006f) {
        Actor_SetScale(&this->actor, 0.006f);
    }
}

void EnZoraegg_Update(Actor* thisx, PlayState* play) {
    EnZoraegg* this = (EnZoraegg*)thisx;

    this->actionFunc(this, play);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

void func_80B32F04(Actor* thisx, PlayState* play) {
    f32 sp7C;
    f32 sp78;
    f32 sp74;
    EnZoraegg* this = (EnZoraegg*)thisx;
    Vec3f pos;
    s16 sp62;
    s16 sp60;
    s32 pad;
    Gfx* gfx;
    Vec3f sp4C;
    s32 pad2;

    Matrix_Push();

    sp4C = GET_ACTIVE_CAM(play)->eye;

    sp62 = Math_Vec3f_Yaw(&sp4C, &this->actor.focus.pos);
    sp60 = -Math_Vec3f_Pitch(&sp4C, &this->actor.focus.pos);

    sp74 = -((15.0f * Math_SinS(sp62)) * Math_CosS(sp60));
    sp78 = -(15.0f * Math_SinS(sp60));
    sp7C = -((15.0f * Math_CosS(sp62)) * Math_CosS(sp60));

    pos.x = this->actor.world.pos.x + sp74;
    pos.y = this->actor.world.pos.y + sp78 + 6.0f;
    pos.z = this->actor.world.pos.z + sp7C;

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);

    sp7C = Math_SinS(play->gameplayFrames * 0x4000);

    Matrix_Scale(this->actor.scale.x * (((sp7C + 1.0f) * 0.1f) + 9.0f),
                 this->actor.scale.y * (((sp7C + 1.0f) * 0.1f) + 9.0f),
                 this->actor.scale.z * (((sp7C + 1.0f) * 0.1f) + 9.0f), MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_XLU_DISP;
    gfx = Gfx_SetupDL20_NoCD(gfx);

    gDPSetDither(gfx++, G_CD_NOISE);
    gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
    gSPDisplayList(gfx++, gameplay_keep_DL_029CB0);

    gDPSetPrimColor(gfx++, 0, 0, 120, 180, 200, (s32)(this->unk_1ED * (20.0f / 51)));
    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gSPDisplayList(gfx++, gameplay_keep_DL_029CF0);

    POLY_XLU_DISP = gfx;

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B331C8(Actor* thisx, PlayState* play) {
    EnZoraegg* this = (EnZoraegg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    Matrix_Scale(this->unk_1E0, this->unk_1E0, this->unk_1E0, MTXMODE_APPLY);

    if (this->unk_1ED >= 254) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
        gSPDisplayList(POLY_OPA_DISP++, gZoraEggDL);
    } else {
        Gfx_SetupDL72(POLY_XLU_DISP++);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->unk_1ED);
        gSPDisplayList(POLY_XLU_DISP++, gZoraEggDL);
    }

    Matrix_Pop();

    func_80B32F04(thisx, play);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnZoraegg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnZoraegg* this = (EnZoraegg*)thisx;

    switch (this->unk_1EC) {
        case 1:
            if ((limbIndex != 1) && (limbIndex != 3) && (limbIndex != 4)) {
                *dList = NULL;
            }
            break;

        case 2:
            if ((limbIndex != 1) && (limbIndex != 3)) {
                *dList = NULL;
            }
            break;
    }
    return false;
}

void func_80B333DC(PlayState* play, Gfx** dList, f32 arg2) {
    Matrix_Push();
    Matrix_Scale(arg2, arg2, arg2, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, *dList);

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZoraegg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnZoraegg* this = (EnZoraegg*)thisx;
    f32 temp_f20;
    f32 temp_f2;

    switch (this->unk_1EC) {
        case 1:
            switch (limbIndex) {
                case ZORA_BABY_LIMB_TAIL1:
                    temp_f20 = this->unk_1EE * 0.01f;
                    Matrix_Push();
                    Matrix_Scale(temp_f20, temp_f20, temp_f20, MTXMODE_APPLY);

                    OPEN_DISPS(play->state.gfxCtx);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_OPA_DISP++, *dList);

                    CLOSE_DISPS(play->state.gfxCtx);
                    break;

                case ZORA_BABY_LIMB_TAIL_FIN:
                    Matrix_Pop();
                    break;

                case ZORA_BABY_LIMB_RIGHT_PECTORAL_FIN:
                case ZORA_BABY_LIMB_LEFT_PECTORAL_FIN:
                    temp_f20 = this->unk_1EF * 0.01f;
                    func_80B333DC(play, dList, temp_f20);
                    break;
            }
            break;

        case 2:
            switch (limbIndex) {
                case ZORA_BABY_LIMB_TAIL1:
                    temp_f20 = (this->unk_1EE * 0.005f) + 0.5f;
                    Matrix_Push();
                    Matrix_Scale(1.0f, temp_f20, temp_f20, MTXMODE_APPLY);

                    OPEN_DISPS(play->state.gfxCtx);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_OPA_DISP++, *dList);

                    CLOSE_DISPS(play->state.gfxCtx);
                    break;

                case ZORA_BABY_LIMB_TAIL_FIN:
                    temp_f2 = 1.0f / ((this->unk_1EE * 0.005f) + 0.5f);
                    temp_f20 = (this->unk_1EE * 0.0035f) + 0.65f;
                    Matrix_Scale(1.0f, temp_f20 * temp_f2, temp_f2, MTXMODE_APPLY);

                    OPEN_DISPS(play->state.gfxCtx);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_OPA_DISP++, *dList);

                    CLOSE_DISPS(play->state.gfxCtx);

                    Matrix_Pop();
                    break;

                case ZORA_BABY_LIMB_RIGHT_PECTORAL_FIN:
                case ZORA_BABY_LIMB_LEFT_PECTORAL_FIN:
                    temp_f20 = this->unk_1EE * 0.01f;
                    func_80B333DC(play, dList, temp_f20);
                    break;
            }
            break;
    }
}

void func_80B33818(Actor* thisx, PlayState* play) {
    static TexturePtr sZoraBabyEyeTextures[] = { gZoraBabyEyeOpenTex, gZoraBabyEyeHalfTex, gZoraBabyEyeClosedTex };
    EnZoraegg* this = (EnZoraegg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sZoraBabyEyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnZoraegg_OverrideLimbDraw, EnZoraegg_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnZoraegg_Draw(Actor* thisx, PlayState* play) {
    EnZoraegg* this = (EnZoraegg*)thisx;

    if (this->unk_1ED > 0) {
        func_80B331C8(thisx, play);
    }

    if (this->unk_1ED != 255) {
        func_80B33818(thisx, play);
    }
}

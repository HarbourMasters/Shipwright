#include "z_en_heishi4.h"
#include "objects/object_sd/object_sd.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY)

void EnHeishi4_Init(Actor* thisx, PlayState* play);
void EnHeishi4_Destroy(Actor* thisx, PlayState* play);
void EnHeishi4_Update(Actor* thisx, PlayState* play);
void EnHeishi4_Draw(Actor* thisx, PlayState* play);

void func_80A56544(EnHeishi4* this, PlayState* play);
void func_80A5673C(EnHeishi4* this, PlayState* play);
void func_80A56328(EnHeishi4* this, PlayState* play);
void func_80A563BC(EnHeishi4* this, PlayState* play);
void func_80A56B40(EnHeishi4* this, PlayState* play);
void func_80A56614(EnHeishi4* this, PlayState* play);
void func_80A56874(EnHeishi4* this, PlayState* play);
void func_80A56900(EnHeishi4* this, PlayState* play);
void func_80A56994(EnHeishi4* this, PlayState* play);
void func_80A56A50(EnHeishi4* this, PlayState* play);
void func_80A56ACC(EnHeishi4* this, PlayState* play);
void EnHeishi4_MarketSneak(EnHeishi4* this, PlayState* play);

const ActorInit En_Heishi4_InitVars = {
    ACTOR_EN_HEISHI4,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SD,
    sizeof(EnHeishi4),
    (ActorFunc)EnHeishi4_Init,
    (ActorFunc)EnHeishi4_Destroy,
    (ActorFunc)EnHeishi4_Update,
    (ActorFunc)EnHeishi4_Draw,
    NULL,
};

static u32 sFaceReactionSets[] = { 6, 7 };

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 33, 40, 0, { 0, 0, 0 } },
};

void EnHeishi4_Init(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    Actor_SetScale(thisx, 0.01f);
    this->type = thisx->params & 0xFF;
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->pos = thisx->world.pos;
    thisx->targetMode = 6;
    if (this->type == HEISHI4_AT_MARKET_DYING) {
        this->height = 30.0f;
        ActorShape_Init(&thisx->shape, 0.0f, NULL, 30.0f);
        SkelAnime_Init(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiDyingGuardAnim_00C444, this->jointTable,
                       this->morphTable, 17);
    } else {
        this->height = 60.0f;
        ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        SkelAnime_Init(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, this->jointTable,
                       this->morphTable, 17);
    }
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    this->collider.dim.yShift = 0;
    this->collider.dim.radius = 15;
    this->collider.dim.height = 70;
    switch (this->type) {
        case HEISHI4_AT_KAKRIKO_ENTRANCE:
        case HEISHI4_AT_IMPAS_HOUSE:
            this->actionFunc = func_80A56328;
            break;
        case HEISHI4_AT_MARKET_DYING:
            this->collider.dim.radius = 28;
            this->collider.dim.height = 5;
            this->actionFunc = func_80A5673C;
            break;
        case HEISHI4_AT_MARKET_NIGHT:
            this->actionFunc = func_80A56544;
            break;
    }
    this->unk_27C = ((thisx->params >> 8) & 0xFF);
    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 兵士２セット完了！ ☆☆☆☆☆ %d\n" VT_RST, thisx->params);
    osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 識別完了！\t    ☆☆☆☆☆ %d\n" VT_RST, this->type);
    osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ メッセージ完了！   ☆☆☆☆☆ %x\n\n" VT_RST, (thisx->params >> 8) & 0xF);
    osSyncPrintf("\n\n");
}

void EnHeishi4_Destroy(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_80A56328(EnHeishi4* this, PlayState* play) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = func_80A563BC;
}

void func_80A563BC(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    this->unk_2B4 = 0;
    reactionOffset = this->type - 4;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(play, sFaceReactionSets[reactionOffset]) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, sFaceReactionSets[reactionOffset]);
        this->unk_2B4 = 1;
        this->actionFunc = func_80A56B40;
    } else {
        if (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) {
            this->actor.textId = 0x5065;
            this->actionFunc = func_80A56B40;
            return;
        }
        if (Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
            this->actor.textId = 0x5068;
            this->actionFunc = func_80A56B40;
            return;
        }
        if (this->type == HEISHI4_AT_IMPAS_HOUSE) {
            if (this->unk_284 == 0) {
                this->actor.textId = 0x5079;
            } else {
                this->actor.textId = 0x507A;
            }
        } else if (IS_DAY) {
            if (this->unk_284 == 0) {
                this->actor.textId = 0x5063;
            } else {
                this->actor.textId = 0x5064;
            }
        } else if (this->unk_284 == 0) {
            this->actor.textId = 0x5066;
        } else {
            this->actor.textId = 0x5067;
        }

        this->actionFunc = func_80A56B40;
    }
}

void func_80A56544(EnHeishi4* this, PlayState* play) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ ぎゃぁ！オトナだー ☆☆☆☆☆ \n" VT_RST);
        Actor_Kill(&this->actor);
    } else {
        this->actionFunc = func_80A56614;
    }
}

void func_80A56614(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    reactionOffset = this->type - 4;
    this->unk_2B4 = 0;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(play, sFaceReactionSets[reactionOffset]) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, sFaceReactionSets[reactionOffset]);
        this->unk_2B4 = 1;
        this->actionFunc = func_80A56B40;
        return;
    }
    if (play->sceneNum == SCENE_MARKET_GUARD_HOUSE) {
        if (IS_DAY) {
            this->actor.textId = 0x7004;
        } else {
            this->actor.textId = 0x709A;
        }
    } else if (play->sceneNum != SCENE_MARKET_NIGHT) {
        if (IS_DAY) {
            this->actor.textId = 0x7002;
        } else {
            this->actor.textId = 0x7003;
        }
    } else {
        this->actor.textId = 0x7003;
    }

    this->actionFunc = func_80A56B40;
}

void func_80A5673C(EnHeishi4* this, PlayState* play) {
    if (Flags_GetEventChkInf(EVENTCHKINF_PULLED_MASTER_SWORD_FROM_PEDESTAL)) {
        osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ マスターソード祝入手！ ☆☆☆☆☆ \n" VT_RST);
        Actor_Kill(&this->actor);
        return;
    }
    this->unk_284 = 0;
    if (Flags_GetEventChkInf(EVENTCHKINF_ZELDA_FLED_HYRULE_CASTLE)) {
        if (!Flags_GetInfTable(INFTABLE_6C)) {
            f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardAnim_00C444);
            Animation_Change(&this->skelAnime, &gEnHeishiDyingGuardAnim_00C444, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP,
                             -10.0f);
            this->actor.textId = 0x7007;
            this->unk_282 = TEXT_STATE_EVENT;
            this->unk_284 = 1;
            osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ デモ開始！ ☆☆☆☆☆ \n" VT_RST);
        } else {
            this->actor.textId = 0x7008;
            this->unk_282 = TEXT_STATE_DONE;
            osSyncPrintf(VT_FGCOL(BLUE) " ☆☆☆☆☆ 返事なし ☆☆☆☆☆ \n" VT_RST);
        }
        this->actionFunc = func_80A56874;
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80A56874(EnHeishi4* this, PlayState* play) {
    if (this->unk_284 != 0) {
        SkelAnime_Update(&this->skelAnime);
    }
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->unk_284 == 0) {
            this->actionFunc = func_80A5673C;

        } else {
            this->actionFunc = func_80A56900;
        }
    } else {
        func_8002F2CC(&this->actor, play, 100.0f);
    }
}

void func_80A56900(EnHeishi4* this, PlayState* play) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardTalkAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiDyingGuardTalkAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = func_80A56994;
}

void func_80A56994(EnHeishi4* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    func_80038290(play, &this->actor, &this->unk_260, &this->unk_266, this->actor.focus.pos);
    if ((this->unk_282 == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        Flags_SetInfTable(INFTABLE_6C);
        func_8002DF54(play, NULL, 8);
        this->actionFunc = func_80A56A50;
    }
}

void func_80A56A50(EnHeishi4* this, PlayState* play) {
    f32 frames = Animation_GetLastFrame(&gEnHeishiDyingGuardDieAnim);

    this->unk_288 = frames;
    Animation_Change(&this->skelAnime, &gEnHeishiDyingGuardDieAnim, 1.0f, 0.0f, frames, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = func_80A56ACC;
}

void func_80A56ACC(EnHeishi4* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    SkelAnime_Update(&this->skelAnime);
    if (this->unk_288 <= currentFrame) {
        func_8002DF54(play, NULL, 7);
        this->actionFunc = func_80A5673C;
    }
}

void func_80A56B40(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    SkelAnime_Update(&this->skelAnime);
    reactionOffset = (this->type - 4);
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (Text_GetFaceReaction(play, sFaceReactionSets[reactionOffset]) != 0) {
        if (this->unk_2B4 == 0) {
            if ((this->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
                this->actionFunc = func_80A563BC;
                return;
            }
            if (this->type == HEISHI4_AT_MARKET_NIGHT) {
                this->actionFunc = func_80A56614;
                return;
            }
        }
    } else {
        if (this->unk_2B4 != 0) {
            if ((this->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
                this->actionFunc = func_80A563BC;
                return;
            }
            if (this->type == HEISHI4_AT_MARKET_NIGHT) {
                this->actionFunc = func_80A56614;
                return;
            }
        }
    }
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if ((this->type == HEISHI4_AT_KAKRIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
            this->unk_284 = 1;
            this->actionFunc = func_80A563BC;
            return;
        }
        if (this->type == HEISHI4_AT_MARKET_NIGHT) {
            Player* player = GET_PLAYER(play);
            // Only allow sneaking when not wearing a mask as that triggers different dialogue. MM Bunny hood disables
            // these interactions, so bunny hood is fine in that case.
            if (CVarGetInteger("gMarketSneak", 0) &&
                (player->currentMask == PLAYER_MASK_NONE ||
                 (player->currentMask == PLAYER_MASK_BUNNY && CVarGetInteger("gMMBunnyHood", 0)))) {
                this->actionFunc = EnHeishi4_MarketSneak;
            } else {
                this->actionFunc = func_80A56614;
                return;
            }
        }
    }
    func_8002F2F4(&this->actor, play);
}

/*Function that allows child Link to exit from Market entrance to Hyrule Field
at night.
*/
void EnHeishi4_MarketSneak(EnHeishi4* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: //yes
                if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES) != RO_GENERIC_OFF){
                    play->nextEntranceIndex = Entrance_OverrideNextIndex(ENTR_HYRULE_FIELD_7); // Market Entrance -> HF
                } else {
                    play->nextEntranceIndex = ENTR_HYRULE_FIELD_0; // HF Near bridge (OoT cutscene entrance) to not fall in the water
                } 
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
                gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
                this->actionFunc = func_80A56614;
                break;
            case 1: //no
                this->actionFunc = func_80A56614;
                break;
        }
    }
}

void EnHeishi4_Update(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    thisx->world.pos.x = this->pos.x;
    thisx->world.pos.y = this->pos.y;
    thisx->world.pos.z = this->pos.z;
    Actor_SetFocus(thisx, this->height);
    if (this->type != HEISHI4_AT_MARKET_DYING) {
        this->interactInfo.trackPos = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            this->interactInfo.trackPos.y = player->actor.world.pos.y - 10.0f;
        }
        Npc_TrackPoint(thisx, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
        this->unk_260 = this->interactInfo.headRot;
        this->unk_266 = this->interactInfo.torsoRot;
    }
    this->unk_27E += 1;
    this->actionFunc(this, play);
    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(play, thisx, 10.0f, 10.0f, 30.0f, 0x1D);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnHeishi_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                              void* thisx) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    if (limbIndex == 9) {
        rot->x += this->unk_266.y;
    }
    if (limbIndex == 16) {
        rot->x += this->unk_260.y;
        rot->z += this->unk_260.z;
    }
    return false;
}

void EnHeishi4_Draw(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnHeishi_OverrideLimbDraw, NULL, this);
}

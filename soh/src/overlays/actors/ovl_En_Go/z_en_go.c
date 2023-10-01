#include "z_en_go.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnGo_Init(Actor* thisx, PlayState* play);
void EnGo_Destroy(Actor* thisx, PlayState* play);
void EnGo_Update(Actor* thisx, PlayState* play);
void EnGo_Draw(Actor* thisx, PlayState* play);

void func_80A3FEB4(EnGo* this, PlayState* play);
void EnGo_StopRolling(EnGo* this, PlayState* play);
void func_80A4008C(EnGo* this, PlayState* play);
void EnGo_GoronLinkRolling(EnGo* this, PlayState* play);
void EnGo_FireGenericActionFunc(EnGo* this, PlayState* play);
void EnGo_CurledUp(EnGo* this, PlayState* play);
void EnGo_WakeUp(EnGo* this, PlayState* play);

void func_80A40494(EnGo* this, PlayState* play);
void func_80A405CC(EnGo* this, PlayState* play);
void EnGo_BiggoronActionFunc(EnGo* this, PlayState* play);
void func_80A408D8(EnGo* this, PlayState* play);

void func_80A40B1C(EnGo* this, PlayState* play);
void EnGo_GetItem(EnGo* this, PlayState* play);
void func_80A40C78(EnGo* this, PlayState* play);
void EnGo_Eyedrops(EnGo* this, PlayState* play);
void func_80A40DCC(EnGo* this, PlayState* play);

void EnGo_AddDust(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep);
void EnGo_UpdateDust(EnGo* this);
void EnGo_DrawDust(EnGo* this, PlayState* play);

const ActorInit En_Go_InitVars = {
    ACTOR_EN_GO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGo),
    (ActorFunc)EnGo_Init,
    (ActorFunc)EnGo_Destroy,
    (ActorFunc)EnGo_Update,
    (ActorFunc)EnGo_Draw,
    NULL,
};

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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum {
    /* 0 */ ENGO_ANIM_0,
    /* 1 */ ENGO_ANIM_1,
    /* 2 */ ENGO_ANIM_2,
    /* 3 */ ENGO_ANIM_3
} EnGoAnimation;

static AnimationSpeedInfo sAnimationInfo[] = {
    { &gGoronAnim_004930, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f },
    { &gGoronAnim_004930, 0.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gGoronAnim_0029A8, 1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gGoronAnim_010590, 1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
};

void EnGo_SetupAction(EnGo* this, EnGoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

u16 EnGo_GetTextID(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);

    switch (thisx->params & 0xF0) {
        case 0x90:
            if (!IS_RANDO && gSaveContext.bgsFlag) {
                return 0x305E;
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_CLAIM_CHECK) {
                if (Environment_GetBgsDayCount() >= CVarGetInteger("gForgeTime", 3)) {
                    return 0x305E;
                } else {
                    return 0x305D;
                }
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_EYEDROPS) {
                player->exchangeItemId = EXCH_ITEM_EYEDROPS;
                return 0x3059;
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) {
                return 0x3058;
            } else {
                player->exchangeItemId = EXCH_ITEM_SWORD_BROKEN;
                return 0x3053;
            }
        case 0x00:
            if ((!IS_RANDO && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) ||
                (IS_RANDO && Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE))) {
                if (Flags_GetInfTable(INFTABLE_10F)) {
                    return 0x3042;
                } else {
                    return 0x3041;
                }
            } else if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1) || (Flags_GetInfTable(INFTABLE_10D))) {
                if (Flags_GetInfTable(INFTABLE_SPOKE_TO_GORON_LINK)) {
                    return 0x3038;
                } else {
                    return 0x3037;
                }
            } else {
                if (Flags_GetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED)) {
                    if (Flags_GetInfTable(INFTABLE_10A)) {
                        return 0x3033;
                    } else {
                        return 0x3032;
                    }
                } else {
                    return 0x3030;
                }
            }
        case 0x10:
            if (Flags_GetSwitch(play, thisx->params >> 8)) {
                return 0x3052;
            } else {
                return 0x3051;
            }
        case 0x20:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                return 0x3021;
            } else if (Flags_GetInfTable(INFTABLE_E0)) {
                return 0x302A;
            } else {
                return 0x3008;
            }
        case 0x30:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                return 0x3026;
            } else {
                return 0x3009;
            }
        case 0x40:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_BOMBED_DODONGOS_CAVERN_ENTRANCE)) {
                return 0x3026;
            } else {
                return 0x300A;
            }
        case 0x50:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetInfTable(INFTABLE_F0)) {
                return 0x3015;
            } else {
                return 0x3014;
            }
        case 0x60:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetInfTable(INFTABLE_F4)) {
                return 0x3017;
            } else {
                return 0x3016;
            }
        case 0x70:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (Flags_GetInfTable(INFTABLE_F8)) {
                return 0x3019;
            } else {
                return 0x3018;
            }
        default:
            return 0x0000;
    }
}

s16 EnGo_UpdateTalkState(PlayState* play, Actor* thisx) {
    s16 unkState = NPC_TALK_STATE_TALKING;
    f32 xzRange;
    f32 yRange = fabsf(thisx->yDistToPlayer) + 1.0f;

    xzRange = thisx->xzDistToPlayer + 1.0f;
    switch (Message_GetState(&play->msgCtx)) {
        if (play) {}
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x3008:
                    Flags_SetInfTable(INFTABLE_E0);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x300B:
                    Flags_SetInfTable(INFTABLE_EB);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3014:
                    Flags_SetInfTable(INFTABLE_F0);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3016:
                    Flags_SetInfTable(INFTABLE_F4);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3018:
                    Flags_SetInfTable(INFTABLE_F8);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3036:
                    func_8002F434(thisx, play, GI_TUNIC_GORON, xzRange, yRange);
                    Flags_SetInfTable(INFTABLE_10D); // EnGo exclusive flag
                    unkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x3037:
                    Flags_SetInfTable(INFTABLE_SPOKE_TO_GORON_LINK);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3041:
                    Flags_SetInfTable(INFTABLE_10F);
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3059:
                    unkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x3052:
                case 0x3054:
                case 0x3055:
                case 0x305A:
                    unkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x305E:
                    unkState = NPC_TALK_STATE_ACTION;
                    break;
                default:
                    unkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (thisx->textId) {
                    case 0x300A:
                        if (play->msgCtx.choiceIndex == 0) {
                            if (CUR_UPG_VALUE(UPG_STRENGTH) != 0 || (Flags_GetInfTable(INFTABLE_EB))) {
                                thisx->textId = 0x300B;
                            } else {
                                thisx->textId = 0x300C;
                            }
                        } else {
                            thisx->textId = 0x300D;
                        }
                        Message_ContinueTextbox(play, thisx->textId);
                        unkState = NPC_TALK_STATE_TALKING;
                        break;
                    case 0x3034:
                        if (play->msgCtx.choiceIndex == 0) {
                            if (Flags_GetInfTable(INFTABLE_10B)) {
                                thisx->textId = 0x3033;
                            } else {
                                thisx->textId = 0x3035;
                            }
                        } else if (Flags_GetInfTable(INFTABLE_10B)) {
                            thisx->textId = 0x3036;
                        } else {
                            thisx->textId = 0x3033;
                        }
                        Message_ContinueTextbox(play, thisx->textId);
                        unkState = NPC_TALK_STATE_TALKING;
                        break;
                    case 0x3054:
                    case 0x3055:
                        if (play->msgCtx.choiceIndex == 0) {
                            unkState = NPC_TALK_STATE_ACTION;
                        } else {
                            thisx->textId = 0x3056;
                            Message_ContinueTextbox(play, thisx->textId);
                            unkState = NPC_TALK_STATE_TALKING;
                        }
                        Flags_SetInfTable(INFTABLE_B4);
                        break;
                }
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (thisx->textId) {
                    case 0x3035:
                        Flags_SetInfTable(INFTABLE_10B);
                    case 0x3032:
                    case 0x3033:
                        thisx->textId = 0x3034;
                        Message_ContinueTextbox(play, thisx->textId);
                        unkState = NPC_TALK_STATE_TALKING;
                        break;
                    default:
                        unkState = NPC_TALK_STATE_ACTION;
                        break;
                }
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                unkState = NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_9:
            break;
    }
    return unkState;
}

s32 func_80A3ED24(PlayState* play, EnGo* this, NpcInteractInfo* interactInfo, f32 arg3, NpcGetTextIdFunc getTextId,
                  NpcUpdateTalkStateFunc updateTalkState) {
    if (interactInfo->talkState != NPC_TALK_STATE_IDLE) {
        interactInfo->talkState = updateTalkState(play, &this->actor);
        return false;
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        interactInfo->talkState = NPC_TALK_STATE_TALKING;
        return true;
    } else if (!func_8002F2CC(&this->actor, play, arg3)) {
        return false;
    } else {
        this->actor.textId = getTextId(play, &this->actor);
        return false;
    }
}

void EnGo_ChangeAnim(EnGo* this, s32 index) {
    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation,
                     sAnimationInfo[index].playSpeed * ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f), 0.0f,
                     Animation_GetLastFrame(sAnimationInfo[index].animation), sAnimationInfo[index].mode,
                     sAnimationInfo[index].morphFrames);
}

s32 EnGo_IsActorSpawned(EnGo* this, PlayState* play) {
    if (((this->actor.params) & 0xF0) == 0x90) {
        return true;
    } else if (play->sceneNum == SCENE_FIRE_TEMPLE && !Flags_GetSwitch(play, (this->actor.params) >> 8) &&
               LINK_IS_ADULT && (this->actor.params & 0xF0) == 0x10) {
        return true;
    } else if (play->sceneNum == SCENE_GORON_CITY && LINK_IS_ADULT && (this->actor.params & 0xF0) == 0x00) {
        return true;
    } else if (play->sceneNum == SCENE_DEATH_MOUNTAIN_TRAIL && LINK_IS_CHILD &&
               ((this->actor.params & 0xF0) == 0x20 || (this->actor.params & 0xF0) == 0x30 ||
                (this->actor.params & 0xF0) == 0x40)) {
        return true;
    } else if (play->sceneNum == SCENE_GORON_CITY && LINK_IS_CHILD &&
               ((this->actor.params & 0xF0) == 0x50 || (this->actor.params & 0xF0) == 0x60 ||
                (this->actor.params & 0xF0) == 0x70)) {
        return true;
    } else {
        return false;
    }
}

f32 EnGo_GetGoronSize(EnGo* this) {
    switch (this->actor.params & 0xF0) {
        case 0x00:
            return 10.0f;
        case 0x20:
        case 0x30:
        case 0x50:
        case 0x60:
        case 0x70:
            return 20.0f;
        case 0x40:
            return 60.0f;
        default:
            return 20.0f;
    }
}

void func_80A3F060(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 npcTrackingMode;

    if (this->actionFunc != EnGo_BiggoronActionFunc && this->actionFunc != EnGo_FireGenericActionFunc &&
        this->actionFunc != func_80A40B1C) {
        npcTrackingMode = NPC_TRACKING_NONE;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = EnGo_GetGoronSize(this);
    Npc_TrackPoint(&this->actor, &this->interactInfo, 4, npcTrackingMode);
}

void func_80A3F0E4(EnGo* this) {
    if (DECR(this->unk_214) == 0) {
        this->unk_216++;
        if (this->unk_216 >= 3) {
            this->unk_214 = Rand_S16Offset(30, 30);
            this->unk_216 = 0;
        }
    }
}

s32 EnGo_IsCameraModified(EnGo* this, PlayState* play) {
    f32 xyzDist;
    s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    Camera* camera = play->cameraPtrs[MAIN_CAM];

    if (fabsf(yawDiff) > 10920.0f) {
        return 0;
    }

    xyzDist = (this->actor.scale.x / 0.01f) * 10000.0f;
    if ((this->actor.params & 0xF0) == 0x90) {
        Camera_ChangeSetting(camera, CAM_SET_DIRECTED_YAW);
        xyzDist *= 4.8f;
    }

    if (fabsf(this->actor.xyzDistToPlayerSq) > xyzDist) {
        if (camera->setting == CAM_SET_DIRECTED_YAW) {
            Camera_ChangeSetting(camera, CAM_SET_NORMAL0);
        }
        return 0;
    } else {
        return 1;
    }
}

void EnGo_ReverseAnimation(EnGo* this) {
    f32 startFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = startFrame;
}

void EnGo_UpdateShadow(EnGo* this) {
    s16 shadowAlpha;
    f32 currentFrame = this->skelAnime.curFrame;
    s16 shadowAlphaTarget = (this->skelAnime.animation == &gGoronAnim_004930 && currentFrame > 32.0f) ||
                                    this->skelAnime.animation != &gGoronAnim_004930
                                ? 255
                                : 0;

    shadowAlpha = this->actor.shape.shadowAlpha;
    Math_SmoothStepToS(&shadowAlpha, shadowAlphaTarget, 10, 60, 1);
    this->actor.shape.shadowAlpha = shadowAlpha;
}

s32 EnGo_FollowPath(EnGo* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;
    f32 xDist;
    f32 zDist;

    if ((this->actor.params & 0xF) == 15) {
        return false;
    }

    path = &play->setupPathList[this->actor.params & 0xF];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->unk_218;
    xDist = pointPos->x - this->actor.world.pos.x;
    zDist = pointPos->z - this->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, (s16)(Math_FAtan2F(xDist, zDist) * ((f32)0x8000 / M_PI)), 10, 1000, 1);

    if ((SQ(xDist) + SQ(zDist)) < 600.0f) {
        this->unk_218++;
        if (this->unk_218 >= path->count) {
            this->unk_218 = 0;
        }

        if ((this->actor.params & 0xF0) != 0x00) {
            return true;
        } else if (Flags_GetSwitch(play, this->actor.params >> 8)) {
            return true;
        } else if (this->unk_218 >= this->actor.shape.rot.z) {
            this->unk_218 = 0;
        }

        return true;
    }

    return false;
}

s32 EnGo_SetMovedPos(EnGo* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;

    if ((this->actor.params & 0xF) == 0xF) {
        return false;
    } else {
        path = &play->setupPathList[this->actor.params & 0xF];
        pointPos = SEGMENTED_TO_VIRTUAL(path->points);
        pointPos += (path->count - 1);
        this->actor.world.pos.x = pointPos->x;
        this->actor.world.pos.y = pointPos->y;
        this->actor.world.pos.z = pointPos->z;
        this->actor.home.pos = this->actor.world.pos;
        return true;
    }
}

s32 EnGo_SpawnDust(EnGo* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius, f32 xzAccel) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s16 angle;
    s32 i;

    pos = this->actor.world.pos; // Overwrites pos
    pos.y = this->actor.floorHeight;
    angle = (Rand_ZeroOne() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.x = (Rand_ZeroOne() - 0.5f) * xzAccel;
        accel.z = (Rand_ZeroOne() - 0.5f) * xzAccel;
        pos.x = (Math_SinS(angle) * radius) + this->actor.world.pos.x;
        pos.z = (Math_CosS(angle) * radius) + this->actor.world.pos.z;
        EnGo_AddDust(this, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}

s32 EnGo_IsRollingOnGround(EnGo* this, s16 unkArg1, f32 unkArg2) {
    if ((this->actor.bgCheckFlags & 1) == 0 || this->actor.velocity.y > 0.0f) {
        return false;
    } else if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return true;
    } else if (DECR(this->unk_21C)) {
        if ((this->unk_21C & 1)) {
            this->actor.world.pos.y += 1.5f;
        } else {
            this->actor.world.pos.y -= 1.5f;
        }
        return true;
    } else {
        this->unk_21A--;
        if (this->unk_21A <= 0) {
            if (this->unk_21A == 0) {
                this->unk_21C = Rand_S16Offset(60, 30);
                this->unk_21A = 0;
                this->actor.velocity.y = 0.0f;
                return true;
            }
            this->unk_21A = unkArg1;
        }
        this->actor.velocity.y = ((f32)this->unk_21A / (f32)unkArg1) * unkArg2;
        return true;
    }
}

void func_80A3F908(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 float1;
    s32 isUnkCondition;

    if (this->actionFunc == EnGo_BiggoronActionFunc || this->actionFunc == EnGo_GoronLinkRolling ||
        this->actionFunc == EnGo_FireGenericActionFunc || this->actionFunc == EnGo_Eyedrops ||
        this->actionFunc == func_80A40DCC || this->actionFunc == EnGo_GetItem || this->actionFunc == func_80A40C78 ||
        this->actionFunc == func_80A40B1C) {

        float1 = (this->collider.dim.radius + 30.0f);
        float1 *= (this->actor.scale.x / 0.01f);
        if ((this->actor.params & 0xF0) == 0x90) {
            float1 *= 4.8f;
        }

        if ((this->actor.params & 0xF0) == 0x90) {
            isUnkCondition =
                func_80A3ED24(play, this, &this->interactInfo, float1, EnGo_GetTextID, EnGo_UpdateTalkState);
        } else {
            isUnkCondition = Npc_UpdateTalking(play, &this->actor, &this->interactInfo.talkState, float1,
                                               EnGo_GetTextID, EnGo_UpdateTalkState);
        }

        if (((this->actor.params & 0xF0) == 0x90) && (isUnkCondition == true)) {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_SWORD_BROKEN) {
                if (func_8002F368(play) == EXCH_ITEM_SWORD_BROKEN) {
                    if (Flags_GetInfTable(INFTABLE_B4)) {
                        this->actor.textId = 0x3055;
                    } else {
                        this->actor.textId = 0x3054;
                    }
                } else {
                    this->actor.textId = 0x3053;
                }
                player->actor.textId = this->actor.textId;
            }

            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYEDROPS) {
                if (func_8002F368(play) == EXCH_ITEM_EYEDROPS) {
                    this->actor.textId = 0x3059;
                } else {
                    this->actor.textId = 0x3058;
                }
                player->actor.textId = this->actor.textId;
            }
        }
    }
}

void EnGo_Init(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;
    s32 pad;
    Vec3f D_80A41B9C = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f D_80A41BA8 = { 0.0f, 0.0f, 0.0f }; // unused

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, NULL, 0, 0, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);

    if (!EnGo_IsActorSpawned(this, play)) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((this->actor.params & 0xF0) && ((this->actor.params & 0xF0) != 0x90)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_WHILE_CULLED;
        this->actor.flags &= ~ACTOR_FLAG_DRAW_WHILE_CULLED;
    }

    EnGo_ChangeAnim(this, ENGO_ANIM_0);
    this->actor.targetMode = 6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->actor.gravity = -1.0f;

    switch (this->actor.params & 0xF0) {
        case 0x00:
            Actor_SetScale(&this->actor, 0.008f);
            if (CHECK_OWNED_EQUIP(EQUIP_TUNIC, 1)) {
                EnGo_SetMovedPos(this, play);
                EnGo_SetupAction(this, EnGo_CurledUp);
            } else {
                this->actor.shape.yOffset = 1400.0f;
                this->actor.speedXZ = 3.0f;
                EnGo_SetupAction(this, EnGo_GoronLinkRolling);
            }
            break;
        case 0x10:
            this->skelAnime.curFrame = Animation_GetLastFrame(&gGoronAnim_004930);
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, EnGo_FireGenericActionFunc);
            break;
        case 0x40:
            if (Flags_GetInfTable(INFTABLE_EB)) {
                EnGo_SetMovedPos(this, play);
            }
            Actor_SetScale(&this->actor, 0.015f);
            EnGo_SetupAction(this, EnGo_CurledUp);
            break;
        case 0x30:
            this->actor.shape.yOffset = 1400.0f;
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, func_80A3FEB4);
            break;
        case 0x90:
            this->actor.targetMode = 5;
            Actor_SetScale(&this->actor, 0.16f);
            EnGo_SetupAction(this, EnGo_CurledUp);
            break;
        case 0x20:
        case 0x50:
        case 0x60:
        case 0x70:
            Actor_SetScale(&this->actor, 0.01f);
            EnGo_SetupAction(this, EnGo_CurledUp);
            break;
        default:
            Actor_Kill(&this->actor);
    }
}

void EnGo_Destroy(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80A3FEB4(EnGo* this, PlayState* play) {
    if (!(this->actor.xyzDistToPlayerSq > SQ(1200.0f))) {
        EnGo_SetupAction(this, EnGo_StopRolling);
    }
}

void EnGo_StopRolling(EnGo* this, PlayState* play) {
    EnBom* bomb;

    if (DECR(this->unk_20E) == 0) {
        if (this->collider.base.ocFlags2 & 1) {
            this->collider.base.ocFlags2 &= ~1;
            play->damagePlayer(play, -4);
            func_8002F71C(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
            this->unk_20E = 0x10;
        }
    }

    this->actor.speedXZ = 3.0f;
    if ((EnGo_FollowPath(this, play) == true) && (this->unk_218 == 0)) {
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
        if (bomb != NULL) {
            bomb->timer = 0;
        }

        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, func_80A4008C);
    }

    this->actor.shape.rot = this->actor.world.rot;

    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        EnGo_SpawnDust(this, 12, 0.16f, 0.1f, 1, 10.0f, 20.0f);
    }
}

void func_80A4008C(EnGo* this, PlayState* play) {
    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        if (this->unk_21A == 0) {
            this->actor.shape.yOffset = 0.0f;
            EnGo_SetupAction(this, EnGo_CurledUp);
        } else {
            EnGo_SpawnDust(this, 12, 0.16f, 0.1f, 1, 10.0f, 20.0f);
        }
    }
}

void EnGo_GoronLinkRolling(EnGo* this, PlayState* play) {
    if ((EnGo_FollowPath(this, play) == true) && Flags_GetSwitch(play, this->actor.params >> 8) &&
        (this->unk_218 == 0)) {
        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, func_80A4008C);
        Flags_SetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED);
    }

    this->actor.shape.rot = this->actor.world.rot;

    if (EnGo_IsRollingOnGround(this, 3, 6.0f)) {
        EnGo_SpawnDust(this, 12, 0.18f, 0.2f, 2, 13.0f, 20.0f);
    }
}

void EnGo_FireGenericActionFunc(EnGo* this, PlayState* play) {
}

void EnGo_CurledUp(EnGo* this, PlayState* play) {
    if ((DECR(this->unk_210) == 0) && EnGo_IsCameraModified(this, play)) {
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_WAKE_UP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);

        this->skelAnime.playSpeed = 0.1f;
        this->skelAnime.playSpeed *= (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f;

        EnGo_SetupAction(this, EnGo_WakeUp);
        if ((this->actor.params & 0xF0) == 0x90) {
            OnePointCutscene_Init(play, 4200, -99, &this->actor, MAIN_CAM);
        }
    }
}

void EnGo_WakeUp(EnGo* this, PlayState* play) {
    f32 frame;

    if (this->skelAnime.playSpeed != 0.0f) {
        Math_SmoothStepToF(&this->skelAnime.playSpeed, ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * 0.5f, 0.1f,
                           1000.0f, 0.1f);
        frame = this->skelAnime.curFrame;
        frame += this->skelAnime.playSpeed;

        if (frame <= 12.0f) {
            return;
        } else {
            this->skelAnime.curFrame = 12.0f;
            this->skelAnime.playSpeed = 0.0f;
            if ((this->actor.params & 0xF0) != 0x90) {
                this->unk_212 = 30;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnGo_SetupAction(this, func_80A405CC);
    } else if (!EnGo_IsCameraModified(this, play)) {
        EnGo_ReverseAnimation(this);
        this->skelAnime.playSpeed = 0.0f;
        EnGo_SetupAction(this, func_80A40494);
    }
}

void func_80A40494(EnGo* this, PlayState* play) {
    f32 frame;

    Math_SmoothStepToF(&this->skelAnime.playSpeed, ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * -0.5f, 0.1f,
                       1000.0f, 0.1f);
    frame = this->skelAnime.curFrame;
    frame += this->skelAnime.playSpeed;

    if (!(frame >= 0.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EN_DODO_M_GND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnGo_SpawnDust(this, 10, 0.4f, 0.1f, 16, 26.0f, 2.0f);
        EnGo_ReverseAnimation(this);
        this->skelAnime.playSpeed = 0.0f;
        this->skelAnime.curFrame = 0.0f;
        this->unk_210 = Rand_S16Offset(30, 30);
        EnGo_SetupAction(this, EnGo_CurledUp);
    }
}

void func_80A405CC(EnGo* this, PlayState* play) {
    f32 lastFrame;
    f32 frame;

    lastFrame = Animation_GetLastFrame(&gGoronAnim_004930);
    Math_SmoothStepToF(&this->skelAnime.playSpeed, (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f, 0.1f, 1000.0f,
                       0.1f);

    frame = this->skelAnime.curFrame;
    frame += this->skelAnime.playSpeed;

    if (!(frame < lastFrame)) {
        this->skelAnime.curFrame = lastFrame;
        this->skelAnime.playSpeed = 0.0f;
        this->unk_212 = Rand_S16Offset(30, 30);
        if (((this->actor.params & 0xF0) == 0x40) && ((Flags_GetInfTable(INFTABLE_EB)) == 0)) {
            EnGo_SetupAction(this, func_80A40B1C);
        } else {
            EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
        }
    }
}

void EnGo_BiggoronActionFunc(EnGo* this, PlayState* play) {
    if (((this->actor.params & 0xF0) == 0x90) && (this->interactInfo.talkState == NPC_TALK_STATE_ACTION)) {
        if (!IS_RANDO && gSaveContext.bgsFlag) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        } else {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYEDROPS) {
                EnGo_ChangeAnim(this, ENGO_ANIM_2);
                this->unk_21E = 100;
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
                EnGo_SetupAction(this, EnGo_Eyedrops);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                gSaveContext.timer2State = 0;
                OnePointCutscene_Init(play, 4190, -99, &this->actor, MAIN_CAM);
            } else {
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
                EnGo_SetupAction(this, EnGo_GetItem);
                Message_CloseTextbox(play);
                EnGo_GetItem(this, play);
            }
        }
    } else if (((this->actor.params & 0xF0) == 0) && (this->interactInfo.talkState == NPC_TALK_STATE_ACTION)) {
        EnGo_SetupAction(this, EnGo_GetItem);
        play->msgCtx.stateTimer = 4;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    } else {
        if ((DECR(this->unk_212) == 0) && !EnGo_IsCameraModified(this, play)) {
            EnGo_ReverseAnimation(this);
            this->skelAnime.playSpeed = -0.1f;
            this->skelAnime.playSpeed *= (this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f;
            EnGo_SetupAction(this, func_80A408D8);
        }
    }
}

void func_80A408D8(EnGo* this, PlayState* play) {
    f32 frame;

    if (this->skelAnime.playSpeed != 0.0f) {
        Math_SmoothStepToF(&this->skelAnime.playSpeed, ((this->actor.params & 0xF0) == 0x90 ? 0.5f : 1.0f) * -1.0f,
                           0.1f, 1000.0f, 0.1f);
        frame = this->skelAnime.curFrame;
        frame += this->skelAnime.playSpeed;
        if (frame >= 12.0f) {
            return;
        } else {
            this->skelAnime.curFrame = 12.0f;
            this->skelAnime.playSpeed = 0.0f;
            if ((this->actor.params & 0xF0) != 0x90) {
                this->unk_212 = 30;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        EnGo_SetupAction(this, func_80A40494);
    } else if (EnGo_IsCameraModified(this, play)) {
        EnGo_ReverseAnimation(this);
        Audio_PlaySoundGeneral(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        this->skelAnime.playSpeed = 0.0f;
        EnGo_SetupAction(this, func_80A405CC);
    }
}

void func_80A40A54(EnGo* this, PlayState* play) {
    f32 float1 = ((f32)0x8000 / Animation_GetLastFrame(&gGoronAnim_010590));
    f32 float2 = this->skelAnime.curFrame * float1;

    this->actor.speedXZ = Math_SinS((s16)float2);
    if (EnGo_FollowPath(this, play) && this->unk_218 == 0) {
        EnGo_ChangeAnim(this, ENGO_ANIM_1);
        this->skelAnime.curFrame = Animation_GetLastFrame(&gGoronAnim_004930);
        this->actor.speedXZ = 0.0f;
        EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
    }
}

void func_80A40B1C(EnGo* this, PlayState* play) {
    if (Flags_GetInfTable(INFTABLE_EB)) {
        EnGo_ChangeAnim(this, ENGO_ANIM_3);
        EnGo_SetupAction(this, func_80A40A54);
    } else {
        EnGo_BiggoronActionFunc(this, play);
    }
}

void EnGo_GetItem(EnGo* this, PlayState* play) {
    f32 xzDist;
    f32 yDist;
    GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    s32 getItemId;

    if (Actor_HasParent(&this->actor, play)) {
        this->interactInfo.talkState = NPC_TALK_STATE_ACTION;
        this->actor.parent = NULL;
        EnGo_SetupAction(this, func_80A40C78);
    } else {
        this->unk_20C = 0;
        if ((this->actor.params & 0xF0) == 0x90) {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK) {
                if (!IS_RANDO) {
                    getItemId = GI_SWORD_BGS;
                } else {
                    getItemEntry = Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_CLAIM_CHECK, GI_SWORD_BGS);
                    getItemId = getItemEntry.getItemId;
                }
                this->unk_20C = 1;
            }
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYEDROPS) {
                if (IS_RANDO) {
                    getItemEntry = Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_EYEDROPS, GI_CLAIM_CHECK);
                    getItemId = getItemEntry.getItemId;
                    Randomizer_ConsumeAdultTradeItem(play, ITEM_EYEDROPS);
                } else {
                    getItemId = GI_CLAIM_CHECK;
                }
            }
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_SWORD_BROKEN) {
                if (IS_RANDO) {
                    getItemEntry = Randomizer_GetItemFromKnownCheck(RC_DMT_TRADE_BROKEN_SWORD, GI_PRESCRIPTION);
                    Randomizer_ConsumeAdultTradeItem(play, ITEM_SWORD_BROKEN);
                    getItemId = getItemEntry.getItemId;
                } else {
                    getItemId = GI_PRESCRIPTION;
                }
            }
        }

        if ((this->actor.params & 0xF0) == 0) {
            getItemId = GI_TUNIC_GORON;
        }

        yDist = fabsf(this->actor.yDistToPlayer) + 1.0f;
        xzDist = this->actor.xzDistToPlayer + 1.0f;
        if (!IS_RANDO || getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, getItemId, xzDist, yDist);
        } else {
            GiveItemEntryFromActor(&this->actor, play, getItemEntry, xzDist, yDist);
        }
    }
}

void func_80A40C78(EnGo* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        EnGo_SetupAction(this, EnGo_BiggoronActionFunc);
        if ((this->actor.params & 0xF0) != 0x90) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        } else if (this->unk_20C) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
            gSaveContext.bgsFlag = true;
        } else if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_PRESCRIPTION) {
            this->actor.textId = 0x3058;
            Message_ContinueTextbox(play, this->actor.textId);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        } else if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK) {
            this->actor.textId = 0x305C;
            Message_ContinueTextbox(play, this->actor.textId);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
            Environment_ClearBgsDayCount();
        }
    }
}

void EnGo_Eyedrops(EnGo* this, PlayState* play) {
    if (DECR(this->unk_21E) == 0) {
        this->actor.textId = 0x305A;
        Message_ContinueTextbox(play, this->actor.textId);
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        EnGo_SetupAction(this, func_80A40DCC);
    }
}

void func_80A40DCC(EnGo* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        EnGo_ChangeAnim(this, ENGO_ANIM_1);
        this->skelAnime.curFrame = Animation_GetLastFrame(&gGoronAnim_004930);
        Message_CloseTextbox(play);
        EnGo_SetupAction(this, EnGo_GetItem);
        EnGo_GetItem(this, play);
    }
}

void EnGo_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGo* this = (EnGo*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);

    if (this->actionFunc == EnGo_BiggoronActionFunc || this->actionFunc == EnGo_FireGenericActionFunc ||
        this->actionFunc == func_80A40B1C) {
        func_80034F54(play, this->jointTable, this->morphTable, 18);
    }

    EnGo_UpdateShadow(this);

    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        Actor_MoveForward(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    func_80A3F0E4(this);
    func_80A3F908(this, play);
    this->actionFunc(this, play);
    func_80A3F060(this, play);
}

void EnGo_DrawCurledUp(EnGo* this, PlayState* play) {
    Vec3f D_80A41BB4 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00BD80);

    Matrix_MultVec3f(&D_80A41BB4, &this->actor.focus.pos);
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnGo_DrawRolling(EnGo* this, PlayState* play) {
    Vec3f D_80A41BC0 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Matrix_RotateZYX((s16)(play->state.frames * ((s16)this->actor.speedXZ * 1400)), 0, this->actor.shape.rot.z,
                     MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00C140);
    Matrix_MultVec3f(&D_80A41BC0, &this->actor.focus.pos);
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnGo_OverrideLimbDraw(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo* this = (EnGo*)thisx;
    Vec3s vec1;
    f32 float1;

    if (limb == 17) {
        Matrix_Translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec1 = this->interactInfo.headRot;
        float1 = (vec1.y / (f32)0x8000) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * M_PI;
        Matrix_RotateZ(float1, MTXMODE_APPLY);
        Matrix_Translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limb == 10) {
        vec1 = this->interactInfo.torsoRot;
        float1 = (vec1.y / (f32)0x8000) * M_PI;
        Matrix_RotateY(float1, MTXMODE_APPLY);
        float1 = (vec1.x / (f32)0x8000) * M_PI;
        Matrix_RotateX(float1, MTXMODE_APPLY);
    }

    if ((limb == 10) || (limb == 11) || (limb == 14)) {
        float1 = Math_SinS(this->jointTable[limb]);
        rot->y += float1 * 200.0f;
        float1 = Math_CosS(this->morphTable[limb]);
        rot->z += float1 * 200.0f;
    }

    return 0;
}

void EnGo_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo* this = (EnGo*)thisx;
    Vec3f D_80A41BCC = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 17) {
        Matrix_MultVec3f(&D_80A41BCC, &this->actor.focus.pos);
    }
}

void EnGo_Draw(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    EnGo_UpdateDust(this);
    Matrix_Push();
    EnGo_DrawDust(this, play);
    Matrix_Pop();

    if (this->actionFunc == EnGo_CurledUp) {
        EnGo_DrawCurledUp(this, play);
    } else if (this->actionFunc == EnGo_GoronLinkRolling || this->actionFunc == func_80A3FEB4 ||
               this->actionFunc == EnGo_StopRolling || this->actionFunc == func_80A3FEB4) {
        EnGo_DrawRolling(this, play);
    } else {
        Gfx_SetupDL_37Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGoronCsEyeOpenTex));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));

        SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnGo_OverrideLimbDraw, EnGo_PostLimbDraw, &this->actor);
        EnGo_DrawDust(this, play);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnGo_AddDust(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale, f32 scaleStep) {
    EnGoEffect* dustEffect = this->dustEffects;
    s16 i;
    s16 timer;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->epoch++;
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            timer = initialTimer;
            dustEffect->timer = timer;
            dustEffect->type = 1;
            dustEffect->initialTimer = initialTimer;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            return;
        }
    }
}

void EnGo_UpdateDust(EnGo* this) {
    EnGoEffect* dustEffect = this->dustEffects;
    f32 randomNumber;
    s16 i;

    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;
            if (dustEffect->timer == 0) {
                dustEffect->type = 0;
            }

            dustEffect->accel.x = (Rand_ZeroOne() * 0.4f) - 0.2f;
            randomNumber = Rand_ZeroOne() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

void EnGo_DrawDust(EnGo* this, PlayState* play) {
    static void* dustTex[] = { gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex };
    EnGoEffect* dustEffect = this->dustEffects;
    s16 alpha;
    s16 firstDone;
    s16 index;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx);
    firstDone = false;
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->dustEffects); i++, dustEffect++) {
        if (dustEffect->type) {
            if (!firstDone) {
                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0);
                gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD40);
                gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
                firstDone = true;
            }

            FrameInterpolation_RecordOpenChild(dustEffect, dustEffect->epoch);
            alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
            gDPPipeSync(POLY_XLU_DISP++);
            Matrix_Translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dustTex[index]));
            gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD50);
            FrameInterpolation_RecordCloseChild();
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

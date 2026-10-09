/*
 * File: z_en_mnk.c
 * Overlay: ovl_En_Mnk
 * Description: Monkey
 */

#include "z_en_mnk.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnMnk_Init(Actor* thisx, PlayState* play);
void EnMnk_Destroy(Actor* thisx, PlayState* play);
void EnMnk_Update(Actor* thisx, PlayState* play);
void EnMnk_Draw(Actor* thisx, PlayState* play);

// Draw funcs
void EnMnk_MonkeyTiedUp_Draw(Actor* thisx, PlayState* play);
void EnMnk_MonkeyHanging_Draw(Actor* thisx, PlayState* play);

// Action funcs
void EnMnk_Monkey_WaitToRunAndWaitAtEachPoint(EnMnk* this, PlayState* play);
void EnMnk_MonkeyTiedUp_Wait(EnMnk* this, PlayState* play);
void EnMnk_MonkeyHanging_StruggleBeforeDunk(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitToTalkAfterRun(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitForPlayerApproach(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitToFollowPath(EnMnk* this, PlayState* play);
void EnMnk_MonkeyTiedUp_WaitUnused(EnMnk* this, PlayState* play);
void EnMnk_MonkeyTiedUp_WaitForInstrument(EnMnk* this, PlayState* play);
void EnMnk_MonkeyTiedUp_WaitForCutRope(EnMnk* this, PlayState* play);
void EnMnk_MonkeyHanging_WaitAfterDunk(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitToTalkAfterSaved(EnMnk* this, PlayState* play);

void EnMnk_DoNothing(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitOutsideWoods(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitOutsideChamber(EnMnk* this, PlayState* play);
void EnMnk_Monkey_WaitToGuideThroughWoods(EnMnk* this, PlayState* play);
void func_80AB9084(EnMnk* this, PlayState* play);
void func_80AB92CC(EnMnk* this, PlayState* play);

s32 EnMnk_ValidatePictograph(PlayState* play, Actor* thisx);
s32 EnMnk_AlreadyExists(EnMnk* this, PlayState* play);

ActorProfile En_Mnk_Profile = {
    /**/ ACTOR_EN_MNK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MNK,
    /**/ sizeof(EnMnk),
    /**/ EnMnk_Init,
    /**/ EnMnk_Destroy,
    /**/ EnMnk_Update,
    /**/ EnMnk_Draw,
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
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01000200, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 30, 0, { 0, 0, 0 } },
};

typedef enum {
    /* 0 */ MONKEY_TIEDUP_ANIM_NONE = -1,
    /* 0 */ MONKEY_TIEDUP_ANIM_KICKAROUND,
    /* 1 */ MONKEY_TIEDUP_ANIM_KICKUPANDDOWN,
    /* 2 */ MONKEY_TIEDUP_ANIM_SHH,
    /* 3 */ MONKEY_TIEDUP_ANIM_SING,
    /* 4 */ MONKEY_TIEDUP_ANIM_SMALLCLAPFEET,
    /* 5 */ MONKEY_TIEDUP_ANIM_LARGECLAPFEET,
    /* 6 */ MONKEY_TIEDUP_ANIM_SHAKEHEAD,
    /* 7 */ MONKEY_TIEDUP_ANIM_WAIT,
    /* 8 */ MONKEY_TIEDUP_ANIM_CONFUSED
} MonkeyTiedUpAnim;

typedef enum {
    /* 0 */ MONKEY_HANGING_ANIM_FREEZE,
    /* 1 */ MONKEY_HANGING_ANIM_RISE,
    /* 2 */ MONKEY_HANGING_ANIM_STRUGGLE
} MonkeyHangingAnim;

typedef enum {
    /* 0 */ MONKEY_FOLLOWPATH_RESULT_FOLLOWING,
    /* 1 */ MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_REACHEDPOINT,
    /* 2 */ MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_FINISHEDPATH,
    /* 3 */ MONKEY_FOLLOWPATH_RESULT_LONGRANGE_REACHEDPOINT,
    /* 4 */ MONKEY_FOLLOWPATH_RESULT_LONGRANGE_FINISHEDPATH
} MonkeyFollowPathResult;

#define MONKEY_FOLLOWPATH_FLAGS_REACHNEXTPOINT (1 << 0)
#define MONKEY_FOLLOWPATH_FLAGS_LONGRANGE (1 << 1)

AnimationHeader* sMonkeyTiedUpAnimations[] = {
    &object_mnk_Anim_00D1C8, &object_mnk_Anim_00CD4C, &object_mnk_Anim_00C23C,
    &object_mnk_Anim_00F248, &object_mnk_Anim_00F9A4, &object_mnk_Anim_00FE34,
    &object_mnk_Anim_00EC44, &object_mnk_Anim_010298, &object_mnk_Anim_00CAE8,
};

AnimationHeader* sMonkeyTiedUpPoleAnimations[] = {
    &object_mnk_Anim_003584, &object_mnk_Anim_005194, &object_mnk_Anim_0052C4,
    &object_mnk_Anim_003854, &object_mnk_Anim_005390, &object_mnk_Anim_005414,
    &object_mnk_Anim_0037E8, &object_mnk_Anim_0054B4, &object_mnk_Anim_003504,
};

AnimationHeader* sMonkeyHangingAnimations[] = {
    &object_mnk_Anim_0062D8,
    &object_mnk_Anim_007380,
    &gMonkeyHangingStruggleAnim,
};

AnimationHeader* sMonkeyHangingRopeAnimations[] = {
    &object_mnk_Anim_01C1B8,
    &object_mnk_Anim_01C1F8,
    &gMonkeyHangingRopeStruggleAnim,
};

AnimationHeader* sMonkeyAnimations[] = {
    &object_mnk_Anim_0105DC, &object_mnk_Anim_009FE0, &object_mnk_Anim_0099B0, &object_mnk_Anim_0095B4,
    &object_mnk_Anim_0095B4, &object_mnk_Anim_00B864, &object_mnk_Anim_00B864, &object_mnk_Anim_009CC0,
    &object_mnk_Anim_009CC0, &object_mnk_Anim_008814, &object_mnk_Anim_005A08, &object_mnk_Anim_01A4F8,
    &object_mnk_Anim_01B468, &object_mnk_Anim_01BB0C, &object_mnk_Anim_01C17C,
};

u32 D_80AB9DD8[] = { 2, 2, 3, 4, 5, 6, 100 };

Color_RGBA8 sMonkeyHangingSplashPrimColor = { 255, 255, 155, 255 };
Color_RGBA8 sMonkeyHangingSplashEnvColor = { 255, 100, 0, 255 };

static Vec3f sMonkeyFocusPosOffset = { 300.0f, 400.0f, 0.0f };

TexturePtr sMonkeyFaceTextures[] = {
    object_mnk_Tex_015020, object_mnk_Tex_015920, object_mnk_Tex_016120, object_mnk_Tex_016920,
    object_mnk_Tex_017120, object_mnk_Tex_017920, object_mnk_Tex_018120,
};

void EnMnk_MonkeyTiedUp_ChangeAnim(EnMnk* this, s32 animIndex, u8 animMode, f32 morphFrames) {
    Animation_Change(&this->skelAnime, sMonkeyTiedUpAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(&sMonkeyTiedUpAnimations[animIndex]->common), animMode, morphFrames);
    Animation_Change(&this->propSkelAnime, sMonkeyTiedUpPoleAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(&sMonkeyTiedUpPoleAnimations[animIndex]->common), animMode, morphFrames);

    switch (animIndex) {
        case MONKEY_TIEDUP_ANIM_KICKAROUND:
            this->unk_3E0 = 5;
            break;

        case MONKEY_TIEDUP_ANIM_SHAKEHEAD:
            this->unk_3E0 = 4;
            break;

        case MONKEY_TIEDUP_ANIM_SING:
        case MONKEY_TIEDUP_ANIM_CONFUSED:
            this->unk_3E0 = 6;
            break;

        default:
            this->unk_3E0 = 0;
            break;
    }
    this->animIndex = animIndex;
}

void EnMnk_MonkeyTiedUp_SetAnim(EnMnk* this, s32 animIndex) {
    EnMnk_MonkeyTiedUp_ChangeAnim(this, animIndex, ANIMMODE_LOOP, -5.0f);
}

void EnMnk_MonkeyTiedUp_SetNewAnim(EnMnk* this, s32 animIndex) {
    if (this->animIndex != animIndex) {
        EnMnk_MonkeyTiedUp_ChangeAnim(this, animIndex, ANIMMODE_LOOP, -5.0f);
    }
}

void EnMnk_MonkeyHanging_ChangeAnim(EnMnk* this, s32 animIndex, u8 animMode, f32 morphFrames) {
    Animation_Change(&this->skelAnime, sMonkeyHangingAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(&sMonkeyHangingAnimations[animIndex]->common), animMode, morphFrames);
    Animation_Change(&this->propSkelAnime, sMonkeyHangingRopeAnimations[animIndex], 1.0f, 0.0f,
                     Animation_GetLastFrame(&sMonkeyHangingRopeAnimations[animIndex]->common), animMode, morphFrames);
    this->animIndex = animIndex;
}

void EnMnk_MonkeyHanging_SetAnim(EnMnk* this, s32 animIndex) {
    EnMnk_MonkeyHanging_ChangeAnim(this, animIndex, ANIMMODE_LOOP, -5.0f);
}

void EnMnk_Monkey_ChangeAnim(EnMnk* this, s32 animIndex, u8 animMode, f32 morphFrames) {
    s32 pad;

    if (animIndex == 100) {
        animIndex = (s32)Rand_ZeroFloat(3.99f) + 7;
    }
    if ((animIndex == 4) || (animIndex == 6)) {
        Animation_Change(&this->skelAnime, sMonkeyAnimations[animIndex], -1.0f,
                         Animation_GetLastFrame(&sMonkeyAnimations[animIndex]->common), 0.0f, animMode, morphFrames);
    } else {
        Animation_Change(&this->skelAnime, sMonkeyAnimations[animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(&sMonkeyAnimations[animIndex]->common), animMode, morphFrames);
    }
    this->unk_3E0 = 0;
    this->animIndex = animIndex;
}

void EnMnk_Monkey_SetAnim(EnMnk* this, s32 animIndex) {
    EnMnk_Monkey_ChangeAnim(this, animIndex, ANIMMODE_LOOP, -5.0f);
}

void EnMnk_Monkey_SetupWaitToRunAndWaitAtEachPoint(EnMnk* this, PlayState* play) {
    this->picto.actor.textId = 0x7D8;
    this->actionFunc = EnMnk_Monkey_WaitToRunAndWaitAtEachPoint;
    this->destPointIndex = 0;
    this->flags |= MONKEY_FLAGS_1 | MONKEY_FLAGS_2;
    this->flags &= ~MONKEY_FLAGS_40;
    Animation_PlayLoop(&this->skelAnime, &object_mnk_Anim_008814);
    this->picto.actor.velocity.y = 0.0f;
    this->picto.actor.terminalVelocity = 0.0f;
    this->picto.actor.gravity = 0.0f;
}

void EnMnk_Monkey_StartInvisible(EnMnk* this, PlayState* play) {
    this->picto.actor.draw = NULL;
    this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->collider.dim.radius = 100;
    this->flags |= MONKEY_FLAGS_8;
    this->flags |= MONKEY_FLAGS_20;
    if ((MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) &&
        Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
        Actor_Kill(&this->picto.actor);
        return;
    }
}

void EnMnk_MonkeyTiedUp_Init(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;
    s16 csId;
    s32 i;

    this->actionFunc = EnMnk_MonkeyTiedUp_Wait;
    this->picto.actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
    SkelAnime_InitFlex(play, &this->propSkelAnime, &gMonkeyTiedUpPoleSkel, &object_mnk_Anim_003584,
                       this->propJointTable, this->propMorphTable, OBJECT_MNK_1_LIMB_MAX);
    this->cueId = 4;
    this->animIndex = MONKEY_TIEDUP_ANIM_NONE;
    EnMnk_MonkeyTiedUp_ChangeAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND, ANIMMODE_ONCE, 0.0f);
    this->picto.actor.draw = EnMnk_MonkeyTiedUp_Draw;
    this->picto.actor.shape.shadowDraw = NULL;
    this->flags |= MONKEY_FLAGS_1 | MONKEY_FLAGS_2 | MONKEY_FLAGS_20 | MONKEY_FLAGS_200;
    csId = this->picto.actor.csId;

    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        this->csIdList[i] = csId;
        if (csId != CS_ID_NONE) {
            this->picto.actor.csId = csId;
            csId = CutsceneManager_GetAdditionalCsId(this->picto.actor.csId);
        }
    }

    this->csId = CS_ID_NONE;
    this->picto.actor.csId = this->csIdList[0];
}

void EnMnk_MonkeyHanging_Init(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    Actor_ChangeCategory(play, &play->actorCtx, &this->picto.actor, ACTORCAT_PROP);
    this->actionFunc = EnMnk_MonkeyHanging_StruggleBeforeDunk;
    this->picto.actor.textId = 0x8E8;
    SkelAnime_InitFlex(play, &this->propSkelAnime, &gMonkeyHangingRopeSkel, &gMonkeyHangingStruggleAnim,
                       this->propJointTable, this->propMorphTable, OBJECT_MNK_3_LIMB_MAX);
    EnMnk_MonkeyHanging_ChangeAnim(this, MONKEY_HANGING_ANIM_STRUGGLE, ANIMMODE_LOOP, 0.0f);
    this->unk_3E0 = 5;
    this->picto.actor.draw = EnMnk_MonkeyHanging_Draw;
    this->picto.actor.shape.shadowDraw = NULL;
    this->flags |= MONKEY_FLAGS_1 | MONKEY_FLAGS_2 | MONKEY_FLAGS_20;
    this->approachPlayerRadius = -0.3f;
    this->unk_3C8 = 60;
    play->envCtx.lightSettingOverride = 1;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_08);
}

void EnMnk_Init(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;
    s32 pad;

    Actor_SetScale(&this->picto.actor, 0.012f);
    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
    this->actionFunc = EnMnk_DoNothing;
    SkelAnime_InitFlex(play, &this->skelAnime, &gMonkeySkel, &object_mnk_Anim_0105DC, this->jointTable,
                       this->morphTable, OBJECT_MNK_2_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &object_mnk_Anim_0105DC);
    this->flags = 0;
    this->unk_3D4 = 0;
    this->unk_3E0 = 0;
    this->unk_3CC = 0;
    this->unk_3CE = 0;
    this->cueId = 0;
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->picto.actor.colChkInfo.cylRadius = 40;
    this->picto.actor.velocity.y = -9.0f;
    this->picto.actor.terminalVelocity = -9.0f;
    this->picto.actor.gravity = -1.0f;
    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    this->pathIndex = MONKEY_GET_PATH_INDEX(thisx);
    this->approachPlayerRadius = 0.0f;
    if (this->pathIndex == MONKEY_PATH_INDEX_NONE) {
        this->pathIndex = PATH_INDEX_NONE;
    }

    switch (MONKEY_GET_TYPE(thisx)) {
        case MONKEY_0:
            if (!Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor)) ||
                Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor) + 1) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80)) {
                Actor_Kill(&this->picto.actor);
                return;
            }
            break;

        case MONKEY_OUTSIDEWOODS:
        case MONKEY_OUTSIDEPALACE:
            if (Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_OUTSIDECHAMBER:
            if (Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor)) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_29_80)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_TIED_UP:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80) || CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_HANGING:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_09_80) || CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_8:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_20) || CHECK_WEEKEVENTREG(WEEKEVENTREG_29_80)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_AFTER_SAVED:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_29_80)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_BY_WITCH:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_02) || CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME) ||
                Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
                Actor_Kill(thisx);
                return;
            }
            break;

        case MONKEY_WOODS_GUIDE:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_02) || CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) {
                Actor_Kill(thisx);
                return;
            }
            if (EnMnk_AlreadyExists(this, play)) {
                Actor_Kill(thisx);
                return;
            }
            break;

        default:
            break;
    }

    switch (MONKEY_GET_TYPE(thisx)) {
        case MONKEY_0:
            EnMnk_Monkey_SetupWaitToRunAndWaitAtEachPoint(this, play);
            break;

        case MONKEY_OUTSIDEWOODS:
            this->actionFunc = EnMnk_Monkey_WaitOutsideWoods;
            CLEAR_EVENTINF(EVENTINF_25);
            CLEAR_EVENTINF(EVENTINF_26);
            this->picto.actor.textId = 0x7D0;
            EnMnk_Monkey_StartInvisible(this, play);
            break;

        case MONKEY_OUTSIDECHAMBER:
            this->picto.actor.textId = 0x7D3;
            this->actionFunc = EnMnk_Monkey_WaitOutsideChamber;
            EnMnk_Monkey_StartInvisible(this, play);
            this->collider.dim.radius = 120;
            break;

        case MONKEY_BY_WITCH:
            EnMnk_Monkey_SetAnim(this, 0);
            this->actionFunc = EnMnk_Monkey_WaitToFollowPath;
            this->unk_3C8 = 0;
            this->flags |= MONKEY_FLAGS_2;
            this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->picto.actor.velocity.y = 0.0f;
            this->picto.actor.terminalVelocity = 0.0f;
            this->picto.actor.gravity = 0.0f;
            break;

        case MONKEY_OUTSIDEPALACE:
            EnMnk_Monkey_SetAnim(this, 0);
            this->actionFunc = EnMnk_Monkey_WaitToFollowPath;
            this->unk_3C8 = 0;
            break;

        case MONKEY_TIED_UP:
            EnMnk_MonkeyTiedUp_Init(thisx, play);
            break;

        case MONKEY_HANGING:
            EnMnk_MonkeyHanging_Init(thisx, play);
            break;

        case MONKEY_WOODS_GUIDE:
            this->picto.actor.room = -1;
            EnMnk_Monkey_SetAnim(this, 0);
            this->actionFunc = EnMnk_Monkey_WaitToGuideThroughWoods;
            this->unk_3C8 = 0;
            this->destPointIndex = 0;

            switch (this->picto.actor.home.rot.x) {
                case 1:
                    this->picto.actor.textId = 0x7DD;
                    break;

                case 2:
                    this->picto.actor.textId = 0x7DE;
                    break;

                default:
                    this->picto.actor.textId = 0x7DC;
                    break;
            }
            break;

        case MONKEY_8:
            this->actionFunc = func_80AB9084;
            this->cueId = 0;
            break;

        case MONKEY_AFTER_SAVED:
            EnMnk_Monkey_SetAnim(this, 0);
            this->unk_3C8 = 0;
            this->actionFunc = EnMnk_Monkey_WaitToTalkAfterSaved;
            this->picto.actor.textId = 0x8E5;
            break;

        case MONKEY_10:
            this->actionFunc = func_80AB92CC;
            this->cueId = 0;
            break;

        default:
            this->actionFunc = EnMnk_DoNothing;
            break;
    }

    this->picto.actor.world.rot.x = this->picto.actor.world.rot.z = 0;
    this->picto.actor.shape.rot.x = this->picto.actor.shape.rot.z = 0;
    this->picto.validationFunc = EnMnk_ValidatePictograph;
}

void EnMnk_Destroy(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    if ((MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_TIED_UP) && (this->flags & MONKEY_FLAGS_2000)) {
        Item_Give(play, ITEM_SONG_SONATA);
        CLEAR_EVENTINF(EVENTINF_24);
    }
}

s32 EnMnk_AlreadyExists(EnMnk* this, PlayState* play) {
    Actor* thisActor = &this->picto.actor;
    Actor* nextActor = NULL;
    Actor* curActor;

    while (true) {
        curActor = SubS_FindActor(play, nextActor, ACTORCAT_NPC, ACTOR_EN_MNK);
        if ((curActor != NULL) && (curActor != thisActor)) {
            if ((thisActor->home.pos.x == curActor->home.pos.x) && (thisActor->home.pos.y == curActor->home.pos.y) &&
                (thisActor->home.pos.z == curActor->home.pos.z)) {
                return true;
            }
            nextActor = curActor->next;
            continue;
        }

        if ((curActor != NULL) && (curActor->next != NULL)) {
            nextActor = curActor->next;
            continue;
        }

        return false;
    };
}

s32 EnMnk_ValidatePictograph(PlayState* play, Actor* thisx) {
    return Snap_ValidatePictograph(play, thisx, PICTO_VALID_MONKEY, &thisx->focus.pos, &thisx->shape.rot, 50.0f, 500.0f,
                                   -1);
}

void EnMnk_PlayWalkSfx(EnMnk* this) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_VO_WALK);
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_WALK);
    }
}

void EnMnk_PlayWaterWalkSfx(EnMnk* this) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_VO_WALK);
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_WALK_WATER);
    }
}

void EnMnk_SpawnWaterEffects(EnMnk* this, PlayState* play) {
    Vec3f* effSpawnPos = &this->picto.actor.world.pos;

    EffectSsGRipple_Spawn(play, effSpawnPos, 150, 500, 0);
    EffectSsGSplash_Spawn(play, effSpawnPos, NULL, NULL, 0, 200);
}

void EnMnk_StartNextPath(EnMnk* this, PlayState* play) {
    s32 pathIndex;

    EnMnk_Monkey_SetAnim(this, 1);
    pathIndex = this->pathIndex;
    this->picto.actor.speed = 0.0f;
    if (pathIndex != PATH_INDEX_NONE) {
        this->path = &play->setupPathList[pathIndex];
    } else {
        this->path = NULL;
    }
}

s32 EnMnk_FollowPath(EnMnk* this, u16 flags) {
    Path* path;
    Vec3s* destPoint;
    f32 dx;
    f32 dz;
    f32 distSq;
    s16 arcTan;

    if (this->path == NULL) {
        return MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_REACHEDPOINT;
    }

    path = this->path;
    destPoint = &((Vec3s*)Lib_SegmentedToVirtual(path->points))[this->destPointIndex];
    dx = destPoint->x - this->picto.actor.world.pos.x;
    dz = destPoint->z - this->picto.actor.world.pos.z;
    arcTan = Math_Atan2S_XY(dz, dx);
    Math_SmoothStepToS(&this->picto.actor.world.rot.y, arcTan, 2, 0xBB8, 0xC8);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);
    arcTan -= this->picto.actor.world.rot.y;

    if (ABS_ALT(arcTan) < 0x3E8) {
        if (this->picto.actor.speed < 7.5f) {
            this->picto.actor.speed += 1.5f;
        }
    } else {
        if (this->picto.actor.speed > 4.0f) {
            this->picto.actor.speed -= 1.5f;
        } else if (this->picto.actor.speed < 2.0f) {
            this->picto.actor.speed += 1.5f;
        }
    }

    distSq = SQ(dx) + SQ(dz);
    if (distSq < SQ(8.0f)) {
        this->destPointIndex++;
        this->picto.actor.world.pos.x = destPoint->x;
        this->picto.actor.world.pos.z = destPoint->z;
        if (this->destPointIndex >= path->count) {
            return MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_FINISHEDPATH;
        }
        if (flags & MONKEY_FOLLOWPATH_FLAGS_REACHNEXTPOINT) {
            return MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_REACHEDPOINT;
        }
    } else if ((flags & MONKEY_FOLLOWPATH_FLAGS_LONGRANGE) && (distSq < SQ(20.0f))) {
        this->destPointIndex++;
        if (this->destPointIndex >= path->count) {
            return MONKEY_FOLLOWPATH_RESULT_LONGRANGE_FINISHEDPATH;
        }
        if (flags & MONKEY_FOLLOWPATH_FLAGS_REACHNEXTPOINT) {
            return MONKEY_FOLLOWPATH_RESULT_LONGRANGE_REACHEDPOINT;
        }
    }
    return MONKEY_FOLLOWPATH_RESULT_FOLLOWING;
}

void func_80AB5F6C(EnMnk* this) {
    s32 pad;
    u32 animIndex;

    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    SkelAnime_Update(&this->skelAnime);

    if ((s32)this->skelAnime.curFrame == 0) {
        if (this->unk_3C8 < 0) {
            EnMnk_Monkey_ChangeAnim(this, 100, ANIMMODE_LOOP, 0.0f);
            this->unk_3C8++;
        } else if (this->unk_3C8 > 0) {
            this->unk_3C8--;
        } else {

            do {
                animIndex = D_80AB9DD8[(s32)Rand_ZeroFloat(7.0f)];
            } while (animIndex == this->unk_3D4);

            EnMnk_Monkey_ChangeAnim(this, animIndex, ANIMMODE_LOOP, 0.0f);

            switch (animIndex) {
                case 100:
                    this->unk_3C8 = -(s32)(Rand_ZeroFloat(6.0f) + 6.0f);
                    break;

                case 2:
                    this->unk_3C8 = Rand_ZeroFloat(6.0f) + 6.0f;
                    break;

                default:
                    this->unk_3C8 = Rand_ZeroFloat(2.0f) + 1.0f;
                    break;
            }

            this->unk_3D4 = animIndex;
        }
    }
}

void func_80AB60FC(EnMnk* this, PlayState* play) {
    f32 depthInWater;

    SkelAnime_Update(&this->skelAnime);
    if (EnMnk_FollowPath(this, 0) != MONKEY_FOLLOWPATH_RESULT_FOLLOWING) {
        this->pathIndex = this->path->additionalPathIndex;
        if (this->pathIndex == ADDITIONAL_PATH_INDEX_NONE) {
            Flags_SetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor) + 1);
            Actor_Kill(&this->picto.actor);
            return;
        }
    }

    depthInWater = this->picto.actor.depthInWater;
    if (depthInWater > 0.0f) {
        this->picto.actor.world.pos.y += depthInWater;
        if ((s32)this->skelAnime.curFrame == 0) {
            EnMnk_SpawnWaterEffects(this, play);
        }
        EnMnk_PlayWaterWalkSfx(this);
        return;
    }
    if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnMnk_PlayWalkSfx(this);
    }
}

void func_80AB61E8(EnMnk* this, PlayState* play) {
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x7D9:
                Message_ContinueTextbox(play, 0x7DA);
                EnMnk_Monkey_SetAnim(this, 3);
                break;

            case 0x7DA:
                Message_ContinueTextbox(play, 0x7DB);
                EnMnk_Monkey_SetAnim(this, 2);
                break;

            case 0x7DB:
                Message_CloseTextbox(play);
                EnMnk_StartNextPath(this, play);
                this->destPointIndex = 0;
                this->actionFunc = func_80AB60FC;
                this->picto.actor.velocity.y = -20.0f;
                this->picto.actor.terminalVelocity = -20.0f;
                break;

            default:
                break;
        }
    }
    SkelAnime_Update(&this->skelAnime);
}

void func_80AB6304(EnMnk* this, PlayState* play) {
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->flags |= MONKEY_FLAGS_4;
    }

    SkelAnime_Update(&this->skelAnime);
    if (this->flags & MONKEY_FLAGS_4) {
        Message_ContinueTextbox(play, 0x7D9);
        EnMnk_Monkey_SetAnim(this, 7);
        this->actionFunc = func_80AB61E8;
        this->flags &= ~MONKEY_FLAGS_4;
    }
}

void func_80AB63CC(EnMnk* this, PlayState* play) {
    s32 pad;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->flags |= MONKEY_FLAGS_4;
    }

    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);

    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_PlayLoop(&this->skelAnime, &object_mnk_Anim_008814);
        this->actionFunc = func_80AB6304;
    }

    if (this->picto.actor.velocity.y < 0.0f) {
        this->flags &= ~MONKEY_FLAGS_2;
    }

    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_VO_JOY);
    }
}

void func_80AB64B8(EnMnk* this, PlayState* play) {
    if (!(this->flags & MONKEY_FLAGS_8)) {
        SkelAnime_Update(&this->skelAnime);
        this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
        Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);
    }

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->picto.actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = func_80AB63CC;
        EnMnk_Monkey_ChangeAnim(this, 9, ANIMMODE_ONCE, -5.0f);
        this->picto.actor.velocity.y = 3.6f;
        this->picto.actor.terminalVelocity = -9.0f;
        this->picto.actor.gravity = -0.4f;
        this->flags &= ~MONKEY_FLAGS_1;
        this->flags &= ~MONKEY_FLAGS_4;
    } else if (this->picto.actor.xzDistToPlayer < 100.0f) {
        this->picto.actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    } else {
        this->picto.actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void EnMnk_Monkey_RunAndWaitAtEachPoint(EnMnk* this, PlayState* play) {
    MonkeyFollowPathResult followPathResult;

    SkelAnime_Update(&this->skelAnime);
    followPathResult =
        EnMnk_FollowPath(this, MONKEY_FOLLOWPATH_FLAGS_REACHNEXTPOINT | MONKEY_FOLLOWPATH_FLAGS_LONGRANGE);
    if (followPathResult != MONKEY_FOLLOWPATH_RESULT_FOLLOWING) {
        if ((followPathResult == MONKEY_FOLLOWPATH_RESULT_SHORTRANGE_FINISHEDPATH) ||
            (followPathResult == MONKEY_FOLLOWPATH_RESULT_LONGRANGE_FINISHEDPATH)) {
            this->pathIndex = this->path->additionalPathIndex;
            if (this->pathIndex == ADDITIONAL_PATH_INDEX_NONE) {
                this->pathIndex = PATH_INDEX_NONE;
            }
            this->path = &play->setupPathList[this->pathIndex];
            this->destPointIndex = 0;
            EnMnk_Monkey_SetAnim(this, 0);
            this->actionFunc = func_80AB64B8;
            this->picto.actor.terminalVelocity = -9.0f;
            this->picto.actor.velocity.y = 0.0f;
            this->picto.actor.speed = 0.0f;
            this->picto.actor.gravity = -1.0f;
        } else if (this->picto.actor.xzDistToPlayer > 160.0f) {
            this->actionFunc = EnMnk_Monkey_WaitToRunAndWaitAtEachPoint;
            this->flags |= MONKEY_FLAGS_1 | MONKEY_FLAGS_2;
            this->picto.actor.speed = 0.0f;
            EnMnk_Monkey_SetAnim(this, 9);
            return;
        }
    }

    if (this->flags & MONKEY_FLAGS_40) {
        EnMnk_PlayWalkSfx(this);
    } else {
        if ((s32)this->skelAnime.curFrame == 0) {
            EnMnk_SpawnWaterEffects(this, play);
        }
        EnMnk_PlayWaterWalkSfx(this);
    }
}

void EnMnk_Monkey_WaitToRunAndWaitAtEachPoint(EnMnk* this, PlayState* play) {
    s32 pad[2];

    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    SkelAnime_Update(&this->skelAnime);

    if (this->picto.actor.xzDistToPlayer < 160.0f) {
        this->flags |= MONKEY_FLAGS_80;
        EnMnk_StartNextPath(this, play);
        this->actionFunc = EnMnk_Monkey_RunAndWaitAtEachPoint;
        this->flags &= ~MONKEY_FLAGS_1;
        this->flags &= ~MONKEY_FLAGS_2;
    }

    if (!(this->flags & MONKEY_FLAGS_40)) {
        if (((s32)this->skelAnime.curFrame == 0) || Animation_OnFrame(&this->skelAnime, 14.0f)) {
            EnMnk_SpawnWaterEffects(this, play);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        if ((this->unk_3D4 % 4) != 0) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_VO_JOY);
        }
        this->unk_3D4++;
    }
}

void EnMnk_Monkey_TalkAfterRun(EnMnk* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->flags |= MONKEY_FLAGS_4;
        Message_CloseTextbox(play);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->flags & MONKEY_FLAGS_4) {
            EnMnk_Monkey_SetAnim(this, 0);
            this->actionFunc = EnMnk_Monkey_WaitToTalkAfterRun;
        } else {
            Animation_PlayOnce(&this->skelAnime, &object_mnk_Anim_009CC0);
        }
    }
}

void EnMnk_Monkey_WaitToTalkAfterRun(EnMnk* this, PlayState* play) {
    s32 pad[2];

    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    SkelAnime_Update(&this->skelAnime);
    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        Animation_PlayOnce(&this->skelAnime, &object_mnk_Anim_009CC0);
        this->actionFunc = EnMnk_Monkey_TalkAfterRun;
        this->flags &= ~MONKEY_FLAGS_4;
    } else if (this->picto.actor.xzDistToPlayer < 100.0f) {
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }
}

void EnMnk_Monkey_Run(EnMnk* this, PlayState* play) {
    s32 switchFlag;

    SkelAnime_Update(&this->skelAnime);
    if (EnMnk_FollowPath(this, MONKEY_FOLLOWPATH_FLAGS_LONGRANGE) != MONKEY_FOLLOWPATH_RESULT_FOLLOWING) {
        EnMnk_Monkey_SetAnim(this, 0);
        this->picto.actor.velocity.y = -9.0f;
        this->picto.actor.terminalVelocity = -9.0f;
        this->picto.actor.gravity = -1.0f;
        this->picto.actor.speed = 0.0f;
        if (MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDECHAMBER) {
            switchFlag = MONKEY_GET_SWITCH_FLAG(&this->picto.actor);
            if (switchFlag != MONKEY_SWITCH_FLAG_NONE) {
                Flags_SetSwitch(play, switchFlag + 1);
            }
            Actor_Kill(&this->picto.actor);
        } else {
            this->picto.actor.textId = 0x7D2;
            this->actionFunc = EnMnk_Monkey_WaitToTalkAfterRun;
        }
    }

    if (this->picto.actor.depthInWater > 0.0f) {
        this->picto.actor.world.pos.y += this->picto.actor.depthInWater;
        if ((s32)this->skelAnime.curFrame == 0) {
            EnMnk_SpawnWaterEffects(this, play);
        }
        EnMnk_PlayWaterWalkSfx(this);
    } else if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnMnk_PlayWalkSfx(this);
    }
}

void EnMnk_Monkey_WaitToRun(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (CHECK_EVENTINF(EVENTINF_25)) {
        EnMnk_StartNextPath(this, play);
        this->destPointIndex = 0;
        this->actionFunc = EnMnk_Monkey_Run;
        this->collider.dim.radius = 15;
        Collider_UpdateCylinder(&this->picto.actor, &this->collider);
        this->flags &= ~MONKEY_FLAGS_8;
    }
}

void EnMnk_Monkey_SetupRunAfterTalk(EnMnk* this, PlayState* play) {
    Message_CloseTextbox(play);
    EnMnk_StartNextPath(this, play);
    this->destPointIndex = 0;
    this->actionFunc = EnMnk_Monkey_Run;
    this->collider.dim.radius = 15;
    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    this->unk_3E0 = 0;
    this->flags &= ~MONKEY_FLAGS_8;
}

void EnMnk_Monkey_TalkAfterApproach(EnMnk* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        EnMnk_Monkey_ChangeAnim(this, 100, ANIMMODE_ONCE, 0.0f);
        SkelAnime_Update(&this->skelAnime);
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.choiceIndex) {
                    case 1:
                        Audio_PlaySfx_MessageDecide();
                        EnMnk_Monkey_ChangeAnim(this, 10, ANIMMODE_ONCE, -5.0f);
                        Message_ContinueTextbox(play, 0x7D7);
                        break;

                    case 0:
                        Audio_PlaySfx_MessageCancel();
                        EnMnk_Monkey_SetAnim(this, 7);
                        Message_ContinueTextbox(play, 0x7D6);
                        this->unk_3E0 = 6;
                        break;
                }
            }
            break;

        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x7D0:
                        EnMnk_Monkey_SetAnim(this, 3);
                        Message_ContinueTextbox(play, 0x7D1);
                        this->unk_3E0 = 5;
                        break;

                    case 0x7D3:
                    case 0x7D6:
                        EnMnk_Monkey_SetAnim(this, 3);
                        Message_ContinueTextbox(play, 0x7D4);
                        break;

                    case 0x7D1:
                        EnMnk_Monkey_SetAnim(this, 7);
                        Message_ContinueTextbox(play, 0x7D2);
                        this->unk_3E0 = 4;
                        break;

                    case 0x7D4:
                        EnMnk_Monkey_SetAnim(this, 5);
                        Message_ContinueTextbox(play, 0x7D5);
                        this->unk_3E0 = 6;
                        break;

                    case 0x7D2:
                        SET_EVENTINF(EVENTINF_25);
                        EnMnk_Monkey_SetupRunAfterTalk(this, play);
                        SET_WEEKEVENTREG(WEEKEVENTREG_79_02);
                        break;

                    case 0x7D7:
                        EnMnk_Monkey_SetupRunAfterTalk(this, play);
                        SET_WEEKEVENTREG(WEEKEVENTREG_08_02);
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

void EnMnk_Monkey_MoveRelativeToPlayer(EnMnk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 angleToPlayer = Math_Atan2S_XY(player->actor.world.pos.z - this->picto.actor.home.pos.z,
                                       player->actor.world.pos.x - this->picto.actor.home.pos.x);

    this->picto.actor.shape.rot.y = angleToPlayer;

    if (this->flags & MONKEY_FLAGS_10) {
        this->picto.actor.shape.rot.y = (this->picto.actor.shape.rot.y + 0x8000) & 0xFFFF;
    }

    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    this->picto.actor.world.pos.x =
        (Math_SinS(angleToPlayer) * this->approachPlayerRadius) + this->picto.actor.home.pos.x;
    this->picto.actor.world.pos.z =
        (Math_CosS(angleToPlayer) * this->approachPlayerRadius) + this->picto.actor.home.pos.z;
}

void EnMnk_Monkey_UnapproachPlayer(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->approachPlayerRadius -= 5.0f;
    EnMnk_Monkey_MoveRelativeToPlayer(this, play);
    if (this->approachPlayerRadius < 4.0f) {
        EnMnk_Monkey_SetAnim(this, 0);
        this->actionFunc = EnMnk_Monkey_WaitForPlayerApproach;
        this->flags &= ~MONKEY_FLAGS_10;
        this->approachPlayerRadius = 0.0f;
    }
    EnMnk_PlayWalkSfx(this);
    if ((MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDEWOODS) &&
        (MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) &&
        Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
        EnMnk_Monkey_SetAnim(this, 2);
        this->actionFunc = EnMnk_Monkey_WaitToRun;
    }
}

void EnMnk_Monkey_WaitToTalkAfterApproach(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnMnk_Monkey_MoveRelativeToPlayer(this, play);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        if (MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDECHAMBER) {
            EnMnk_Monkey_SetAnim(this, 2);
        } else {
            EnMnk_Monkey_SetAnim(this, 5);
        }
        this->unk_3E0 = 6;
        this->actionFunc = EnMnk_Monkey_TalkAfterApproach;
        if (MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) {
            Flags_SetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor));
        }
    } else if ((MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDEWOODS) &&
               (MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) &&
               Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
        EnMnk_Monkey_SetAnim(this, 2);
        this->actionFunc = EnMnk_Monkey_WaitToRun;
    } else if (this->picto.actor.xzDistToPlayer > 150.0f) {
        EnMnk_Monkey_SetAnim(this, 1);
        this->flags |= MONKEY_FLAGS_10;
        this->actionFunc = EnMnk_Monkey_UnapproachPlayer;
    } else if (this->picto.actor.xzDistToPlayer < 100.0f) {
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }
}

void EnMnk_Monkey_ApproachPlayer(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->approachPlayerRadius += 5.0f;
    EnMnk_Monkey_MoveRelativeToPlayer(this, play);

    if (this->approachPlayerRadius > 84.0f) {
        if (MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDECHAMBER) {
            EnMnk_Monkey_SetAnim(this, 7);
        } else {
            EnMnk_Monkey_SetAnim(this, 2);
        }
        if (GameInteractor_Should(VB_MONKEY_WAIT_TO_TALK_AFTER_APPROACH, true, this)) {
            this->actionFunc = EnMnk_Monkey_WaitToTalkAfterApproach;
        }
    }

    EnMnk_PlayWalkSfx(this);

    if ((MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDEWOODS) &&
        (MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) &&
        Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
        EnMnk_Monkey_SetAnim(this, 2);
        this->actionFunc = EnMnk_Monkey_WaitToRun;
    }
}

void EnMnk_Monkey_WaitForPlayerApproach(EnMnk* this, PlayState* play) {
    EnMnk_Monkey_MoveRelativeToPlayer(this, play);
    SkelAnime_Update(&this->skelAnime);

    if (this->picto.actor.xzDistToPlayer < 200.0f) {
        EnMnk_Monkey_SetAnim(this, 1);
        this->actionFunc = EnMnk_Monkey_ApproachPlayer;
    }

    if ((MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDEWOODS) &&
        (MONKEY_GET_SWITCH_FLAG(&this->picto.actor) != 0x7F) &&
        Flags_GetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor))) {
        EnMnk_Monkey_SetAnim(this, 2);
        this->actionFunc = EnMnk_Monkey_WaitToRun;
    }
}

void EnMnk_Monkey_Drop(EnMnk* this, PlayState* play) {
    if (((this->picto.actor.world.pos.y - this->picto.actor.home.pos.y) < 40.0f) &&
        SkelAnime_Update(&this->skelAnime)) {
        EnMnk_Monkey_SetAnim(this, 0);
        this->actionFunc = EnMnk_Monkey_WaitForPlayerApproach;

        if (MONKEY_GET_TYPE(&this->picto.actor) == MONKEY_OUTSIDEWOODS) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_WALK);
        }
    }
}

void EnMnk_Monkey_SetupDrop(EnMnk* this) {
    this->flags &= ~MONKEY_FLAGS_20;
    this->picto.actor.world.pos.y = this->picto.actor.home.pos.y + 200.0f;
    this->actionFunc = EnMnk_Monkey_Drop;
    this->picto.actor.velocity.y = -10.0f;
    this->picto.actor.terminalVelocity = -10.0f;
    this->picto.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->picto.actor.draw = EnMnk_Draw;
    Animation_Change(&this->skelAnime, &object_mnk_Anim_008814, 1.0f, 10.0f,
                     Animation_GetLastFrame(&object_mnk_Anim_008814), ANIMMODE_ONCE, 0.0f);
}

void EnMnk_Monkey_WaitOutsideWoods(EnMnk* this, PlayState* play) {
    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;

    if (((this->picto.actor.xzDistToPlayer < 200.0f) && CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) ||
        CHECK_EVENTINF(EVENTINF_26)) {
        SET_EVENTINF(EVENTINF_26);
        EnMnk_Monkey_SetupDrop(this);
    }
}

void EnMnk_Monkey_WaitOutsideChamber(EnMnk* this, PlayState* play) {
    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    if ((this->picto.actor.xzDistToPlayer < 200.0f) &&
        ((CHECK_WEEKEVENTREG(WEEKEVENTREG_18_01) || CHECK_WEEKEVENTREG(WEEKEVENTREG_88_01)) && (play->curSpawn != 0))) {
        EnMnk_Monkey_SetupDrop(this);
    }
}

void EnMnk_Monkey_FollowPathAndWait(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (EnMnk_FollowPath(this, MONKEY_FOLLOWPATH_FLAGS_LONGRANGE) != MONKEY_FOLLOWPATH_RESULT_FOLLOWING) {
        this->pathIndex = this->path->additionalPathIndex;
        if (this->pathIndex == ADDITIONAL_PATH_INDEX_NONE) {
            Flags_SetSwitch(play, MONKEY_GET_SWITCH_FLAG(&this->picto.actor));
            Actor_Kill(&this->picto.actor);
            return;
        }
        EnMnk_Monkey_SetAnim(this, 0);
        this->actionFunc = EnMnk_Monkey_WaitToFollowPath;
        this->picto.actor.speed = 0.0f;
    }
    EnMnk_PlayWalkSfx(this);
}

void EnMnk_Monkey_WaitToFollowPath(EnMnk* this, PlayState* play) {
    if ((this->picto.actor.playerHeightRel > -10.0f) && (this->picto.actor.xzDistToPlayer < 160.0f)) {
        EnMnk_StartNextPath(this, play);
        this->destPointIndex = 0;
        this->actionFunc = EnMnk_Monkey_FollowPathAndWait;
    } else {
        func_80AB5F6C(this);
    }
}

s32 EnMnk_PlayerIsInTalkRange(EnMnk* this, PlayState* play) {
    s32 absYawTowardsPlayer;

    if ((this->picto.actor.xzDistToPlayer < 100.0f) && (this->picto.actor.xzDistToPlayer > 20.0f)) {
        absYawTowardsPlayer = ABS_ALT(this->picto.actor.yawTowardsPlayer);
        if ((absYawTowardsPlayer > 0x4000) && Player_IsFacingActor(&this->picto.actor, 0x3000, play)) {
            return true;
        }
    }
    return false;
}

void EnMnk_MonkeyTiedUp_TeachSong(EnMnk* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    switch (play->msgCtx.currentTextId) {
        case 0x8D9:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
            break;

        case 0x8DC:
        case 0x8DF:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND);
            break;

        case 0x8DD:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_SMALLCLAPFEET);
            this->unk_3E0 = 5;
            break;

        case 0x8DE:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
            break;

        case 0x8E0:
        case 0x1B66:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_LARGECLAPFEET);
            break;

        case 0x8E1:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
            break;

        case 0x1B5C:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_SING);
            break;

        case 0x8E2:
            EnMnk_MonkeyTiedUp_SetNewAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND);
            this->unk_3E0 = 0;
            break;

        default:
            break;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && !(this->flags & MONKEY_FLAGS_4000) &&
        Message_ShouldAdvance(play)) {

        switch (play->msgCtx.choiceIndex) {
            case 1:
                this->picto.actor.csId = this->csIdList[5];
                this->csId = 5;
                SET_WEEKEVENTREG(WEEKEVENTREG_09_80);
                this->flags |= MONKEY_FLAGS_2000;
                Audio_PlaySfx_MessageDecide();
                break;

            case 0:
                Audio_PlaySfx_MessageCancel();
                this->picto.actor.csId = this->csIdList[4];
                this->csId = 4;
                break;

            default:
                break;
        }
        this->flags |= MONKEY_FLAGS_4000;
    }

    if (this->picto.actor.csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(this->picto.actor.csId)) {
            this->flags &= ~MONKEY_FLAGS_4000;
            CutsceneManager_StartWithPlayerCs(this->picto.actor.csId, &this->picto.actor);

            switch (this->csId) {
                case 2:
                    this->csId = 3;
                    break;

                case 4:
                    this->csId = 3;
                    break;

                case 5:
                    this->csId = 6;
                    break;

                default:
                    this->picto.actor.csId = CS_ID_NONE;
                    return;
            }
            this->picto.actor.csId = this->csIdList[this->csId];
        } else {
            CutsceneManager_Queue(this->picto.actor.csId);
        }
    }
}

void EnMnk_MonkeyTiedUp_ReactToWrongInstrument(EnMnk* this, PlayState* play) {
    Actor* actor = &this->picto.actor;

    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x8D4:
            case 0x8DA:
            case 0x8DB:
                Message_CloseTextbox(play);
                this->actionFunc = EnMnk_MonkeyTiedUp_WaitForInstrument;
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_WAIT);
                if (this->csId != CS_ID_NONE) {
                    CutsceneManager_Stop(this->csId);
                    this->picto.actor.csId = this->csIdList[0];
                    this->csId = CS_ID_NONE;
                    return;
                }
                break;

            case 0x8D5:
                Message_ContinueTextbox(play, 0x8DA);
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
                break;

            case 0x8D6:
                Message_ContinueTextbox(play, 0x8DB);
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
                break;

            default:
                break;
        }
    }

    if (actor->csId != CS_ID_NONE) {
        if (CutsceneManager_IsNext(actor->csId)) {
            CutsceneManager_Start(actor->csId, actor);

            this->csId = actor->csId;
            actor->csId = (actor->csId == this->csIdList[0]) ? this->csIdList[1] : CS_ID_NONE;
        } else {
            CutsceneManager_Queue(actor->csId);
        }
    }
}

void EnMnk_MonkeyTiedUp_TalkUnused(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);
    if (Actor_TextboxIsClosing(&this->picto.actor, play)) {
        this->actionFunc = EnMnk_MonkeyTiedUp_WaitUnused;
    }
}

void EnMnk_MonkeyTiedUp_TransitionAfterTalk(EnMnk* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);
    if (play->msgCtx.currentTextId == 0x8CD) {
        if (this->skelAnime.curFrame <= 17.0f) {
            this->unk_3E0 = 0;
        } else {
            this->unk_3E0 = 2;
        }
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x8CA:
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                if (play->curSpawn != 1) {
                    Message_ContinueTextbox(play, 0x8CB);
                    SET_WEEKEVENTREG(WEEKEVENTREG_88_01);
                } else {
                    Message_ContinueTextbox(play, 0x8EB);
                    this->flags |= MONKEY_FLAGS_1000;
                }
                break;

            case 0x8CB:
            case 0x8EB:
            case 0x8EC:
                Message_CloseTextbox(play);
                this->actionFunc = EnMnk_MonkeyTiedUp_Wait;
                this->cueId = 4;
                EnMnk_MonkeyTiedUp_ChangeAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND, ANIMMODE_ONCE, -5.0f);
                break;

            case 0x8CC:
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHH);
                this->unk_3E0 = 2;
                Message_ContinueTextbox(play, 0x8CD);
                break;

            case 0x8CD:
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                Message_ContinueTextbox(play, 0x8CE);
                break;

            case 0x8CE:
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
                Message_ContinueTextbox(play, 0x8CF);
                break;

            case 0x8CF:
            case 0x8D0:
            case 0x8D1:
                Message_CloseTextbox(play);
                this->actionFunc = EnMnk_MonkeyTiedUp_WaitForCutRope;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
                break;

            case 0x8D3:
                if ((gSaveContext.save.playerForm == PLAYER_FORM_FIERCE_DEITY) ||
                    (gSaveContext.save.playerForm == PLAYER_FORM_HUMAN)) {
                    Message_CloseTextbox(play);
                    this->actionFunc = EnMnk_MonkeyTiedUp_WaitForInstrument;
                    EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_WAIT);
                } else {
                    EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
                    Message_ContinueTextbox(play, 0x8D7);
                }
                break;

            case 0x8D2:
            case 0x8D7:
                Message_CloseTextbox(play);
                this->actionFunc = EnMnk_MonkeyTiedUp_WaitForInstrument;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_WAIT);
                break;

            default:
                break;
        }
    }
}

void EnMnk_MonkeyTiedUp_WaitUnused(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_MonkeyTiedUp_TalkUnused;
        this->unk_3E0 = 0;
    } else if (EnMnk_PlayerIsInTalkRange(this, play)) {
        this->picto.actor.textId = 0x8E2;
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }
}

void EnMnk_MonkeyTiedUp_WaitForInstrument(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_OcarinaInteractionAccepted(&this->picto.actor, &play->state)) {
        switch (gSaveContext.save.playerForm) {
            case PLAYER_FORM_HUMAN:
            case PLAYER_FORM_FIERCE_DEITY:
                this->picto.actor.textId = 0x8D4;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                break;

            case PLAYER_FORM_GORON:
                this->picto.actor.textId = 0x8D5;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                break;

            case PLAYER_FORM_ZORA:
                this->picto.actor.textId = 0x8D6;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                break;

            case PLAYER_FORM_DEKU:
                this->picto.actor.textId = 0x8D8;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
                this->actionFunc = EnMnk_MonkeyTiedUp_TeachSong;
                this->csId = 2;
                SET_EVENTINF(EVENTINF_24);
                this->picto.actor.csId = this->csIdList[2];
                play->msgCtx.ocarinaMode = OCARINA_MODE_END;
                CutsceneManager_Queue(this->csIdList[2]);
                return;

            default:
                this->picto.actor.textId = 0x8D4;
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
                break;
        }
        this->actionFunc = EnMnk_MonkeyTiedUp_ReactToWrongInstrument;
        this->picto.actor.csId = this->csIdList[0];
        Message_StartTextbox(play, this->picto.actor.textId, NULL);
        CutsceneManager_Queue(this->picto.actor.csId);
    } else if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_MonkeyTiedUp_TransitionAfterTalk;
        EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND);
    } else if (EnMnk_PlayerIsInTalkRange(this, play)) {
        this->picto.actor.textId = 0x8D3;
        Actor_OfferTalk(&this->picto.actor, play, 100.0f);
        Actor_OfferOcarinaInteraction(&this->picto.actor, play, 100.0f, 100.0f);
    }
}

void EnMnk_MonkeyTiedUp_TalkAfterCutRope(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_MonkeyTiedUp_TransitionAfterTalk;
        this->picto.actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        Actor_OfferTalk(&this->picto.actor, play, 150.0f);
    }
}

void EnMnk_MonkeyTiedUp_WaitForCutRope(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        if ((gSaveContext.save.playerForm == PLAYER_FORM_FIERCE_DEITY) ||
            (gSaveContext.save.playerForm == PLAYER_FORM_HUMAN)) {
            EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHH);
            this->unk_3E0 = 5;
        } else {
            EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_CONFUSED);
        }
        this->actionFunc = EnMnk_MonkeyTiedUp_TransitionAfterTalk;
    } else if (this->collider.base.acFlags & AC_HIT) {
        this->actionFunc = EnMnk_MonkeyTiedUp_TalkAfterCutRope;
        this->picto.actor.textId = 0x8D2;
        EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
        this->picto.actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (EnMnk_PlayerIsInTalkRange(this, play)) {
        if ((gSaveContext.save.playerForm != PLAYER_FORM_FIERCE_DEITY) &&
            (gSaveContext.save.playerForm != PLAYER_FORM_HUMAN)) {
            this->picto.actor.textId = 0x8D1;
        } else {
            this->picto.actor.textId = 0x8D0;
        }
        Actor_OfferTalk(&this->picto.actor, play, 100.0f);
    }
}

void EnMnk_MonkeyTiedUp_Wait(EnMnk* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    if (SkelAnime_Update(&this->propSkelAnime)) {
        this->cueId--;
        if (this->cueId < 0) {
            this->cueId = 4;
            EnMnk_MonkeyTiedUp_ChangeAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND, ANIMMODE_ONCE, -5.0f);
        } else if (this->cueId == 0) {
            EnMnk_MonkeyTiedUp_ChangeAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD, ANIMMODE_ONCE, -5.0f);
        } else {
            EnMnk_MonkeyTiedUp_ChangeAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND, ANIMMODE_ONCE, 0.0f);
            SkelAnime_Update(&this->skelAnime);
            SkelAnime_Update(&this->propSkelAnime);
        }
    }
    if (Actor_OcarinaInteractionAccepted(&this->picto.actor, &play->state)) {
        this->picto.actor.textId = 0x8D8;
        EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
        this->actionFunc = EnMnk_MonkeyTiedUp_TeachSong;
        this->csId = 2;
        SET_EVENTINF(EVENTINF_24);
        this->picto.actor.csId = this->csIdList[2];
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        CutsceneManager_Queue(this->csIdList[2]);
    } else if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        if (gSaveContext.save.playerForm == PLAYER_FORM_DEKU) {
            if (this->picto.actor.textId == 0x8EC) {
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_SHAKEHEAD);
            } else {
                EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKAROUND);
            }
        } else {
            EnMnk_MonkeyTiedUp_SetAnim(this, MONKEY_TIEDUP_ANIM_KICKUPANDDOWN);
        }
        this->actionFunc = EnMnk_MonkeyTiedUp_TransitionAfterTalk;
    } else if (EnMnk_PlayerIsInTalkRange(this, play)) {
        if (gSaveContext.save.playerForm == PLAYER_FORM_DEKU) {
            if (this->flags & MONKEY_FLAGS_1000) {
                this->picto.actor.textId = 0x8EC;
            } else {
                this->picto.actor.textId = 0x8CA;
            }
            Actor_OfferOcarinaInteraction(&this->picto.actor, play, 100.0f, 100.0f);
        } else {
            this->picto.actor.textId = 0x8CC;
        }
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    } else if ((play->curSpawn != 1) && (this->picto.actor.xzDistToPlayer < 140.0f) &&
               Player_IsFacingActor(&this->picto.actor, 0x3000, play)) {
        this->picto.actor.textId = 0x8CA;
        Actor_OfferTalk(&this->picto.actor, play, 140.0f);
    }
}

void EnMnk_MonkeyHanging_WaitForTextboxAfterDunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->flags & MONKEY_FLAGS_800) {
            this->flags &= ~MONKEY_FLAGS_800;
            CutsceneManager_Stop(this->picto.actor.csId);
        }
        Message_CloseTextbox(play);
        this->actionFunc = EnMnk_MonkeyHanging_WaitAfterDunk;
        Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_END);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_08);
    }
}

void EnMnk_MonkeyHanging_RaiseFromDunk2(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (this->unk_3C8 > 0) {
        this->unk_3D0 -= 0x320;
        this->approachPlayerRadius -= 0.01f;
        this->unk_3C8--;
    } else {
        this->unk_3C8 = 240;
        this->actionFunc = EnMnk_MonkeyHanging_WaitForTextboxAfterDunk;
        EnMnk_MonkeyHanging_SetAnim(this, MONKEY_HANGING_ANIM_FREEZE);
        Message_ContinueTextbox(play, 0x8EA);
    }
}

void EnMnk_MonkeyHanging_PauseDuringDunk2(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);
    if (this->unk_3C8 > 0) {
        this->unk_3C8--;
    } else {
        this->unk_3C8 = 60;
        this->actionFunc = EnMnk_MonkeyHanging_RaiseFromDunk2;
        EnMnk_MonkeyHanging_ChangeAnim(this, MONKEY_HANGING_ANIM_RISE, ANIMMODE_ONCE, -5.0f);
        this->unk_3E0 = 3;
    }
}

void EnMnk_MonkeyHanging_MakeSplash(EnMnk* this, PlayState* play) {
    Vec3f effSpawnPos;

    Math_Vec3f_Copy(&effSpawnPos, &this->picto.actor.focus.pos);
    effSpawnPos.y += 20.0f;
    EffectSsGSplash_Spawn(play, &effSpawnPos, &sMonkeyHangingSplashPrimColor, &sMonkeyHangingSplashEnvColor, 0, 300);
}

void EnMnk_MonkeyHanging_Dunk2(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (this->unk_3C8 > 0) {
        this->unk_3D0 += 0x320;
        this->approachPlayerRadius += 0.01f;
        this->unk_3C8--;
        if (this->unk_3C8 == 0xA) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_BOMB_DROP_WATER);
            EnMnk_MonkeyHanging_MakeSplash(this, play);
        }
    } else {
        this->unk_3C8 = 20;
        this->actionFunc = EnMnk_MonkeyHanging_PauseDuringDunk2;
    }
}

void EnMnk_MonkeyHanging_Plead(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x8E8:
                Message_ContinueTextbox(play, 0x8E9);
                break;

            case 0x8E9:
                this->actionFunc = EnMnk_MonkeyHanging_Dunk2;
                Message_CloseTextbox(play);
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_WAIT);
                this->unk_3C8 = 60;
                break;

            default:
                break;
        }
    }
    if (!(this->flags & MONKEY_FLAGS_800)) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->picto.actor.csId);
        } else if (CutsceneManager_IsNext(this->picto.actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->picto.actor.csId, &this->picto.actor);
            this->flags |= MONKEY_FLAGS_800;
        } else {
            CutsceneManager_Queue(this->picto.actor.csId);
        }
    }
}

void EnMnk_MonkeyHanging_WaitAfterDunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_MonkeyHanging_Plead;
        CutsceneManager_Queue(this->picto.actor.csId);
        SET_WEEKEVENTREG(WEEKEVENTREG_83_08);
        EnMnk_MonkeyHanging_SetAnim(this, MONKEY_HANGING_ANIM_STRUGGLE);
        this->unk_3E0 = 5;
    } else if (this->unk_3C8 > 0) {
        this->unk_3C8--;
        if (this->picto.actor.isLockedOn && (this->picto.actor.csId != CS_ID_NONE)) {
            Actor_OfferTalk(&this->picto.actor, play, 1000.0f);
        }
    } else {
        this->unk_3C8 = 240;
        this->actionFunc = EnMnk_MonkeyHanging_StruggleBeforeDunk;
        EnMnk_MonkeyHanging_SetAnim(this, MONKEY_HANGING_ANIM_STRUGGLE);
        this->unk_3E0 = 5;
    }
}

void EnMnk_MonkeyHanging_RaiseFromDunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (this->unk_3C8 > 0) {
        this->unk_3D0 -= 0x320;
        this->approachPlayerRadius -= 0.01f;
        this->unk_3C8--;
    } else {
        this->unk_3C8 = 240;
        this->actionFunc = EnMnk_MonkeyHanging_WaitAfterDunk;
        EnMnk_MonkeyHanging_SetAnim(this, MONKEY_HANGING_ANIM_FREEZE);
    }
}

void EnMnk_MonkeyHanging_PauseDuringDunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (this->unk_3C8 > 0) {
        this->unk_3C8--;
    } else {
        this->unk_3C8 = 60;
        this->actionFunc = EnMnk_MonkeyHanging_RaiseFromDunk;
        EnMnk_MonkeyHanging_ChangeAnim(this, MONKEY_HANGING_ANIM_RISE, ANIMMODE_ONCE, -5.0f);
        this->unk_3E0 = 3;
    }
}

void EnMnk_MonkeyHanging_Dunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (this->unk_3C8 > 0) {
        this->unk_3D0 += 0x320;
        this->approachPlayerRadius += 0.01f;
        this->unk_3C8--;
        if (this->unk_3C8 == 0xA) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_BOMB_DROP_WATER);
            EnMnk_MonkeyHanging_MakeSplash(this, play);
        }
    } else {
        this->unk_3C8 = 20;
        this->actionFunc = EnMnk_MonkeyHanging_PauseDuringDunk;
    }
}

void EnMnk_MonkeyHanging_StruggleBeforeDunk(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    SkelAnime_Update(&this->propSkelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_MonkeyHanging_Plead;
        CutsceneManager_Queue(this->picto.actor.csId);
        SET_WEEKEVENTREG(WEEKEVENTREG_83_08);
    } else if (this->unk_3C8 > 0) {
        this->unk_3C8--;
        if (this->picto.actor.isLockedOn) {
            Actor_OfferTalk(&this->picto.actor, play, 1000.0f);
        }
    } else {
        this->unk_3C8 = 60;
        this->actionFunc = EnMnk_MonkeyHanging_Dunk;
    }
}

void EnMnk_Monkey_GuideThroughWoods(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (EnMnk_FollowPath(this, 0) != MONKEY_FOLLOWPATH_RESULT_FOLLOWING) {
        this->pathIndex = this->path->additionalPathIndex;
        if (this->pathIndex == ADDITIONAL_PATH_INDEX_NONE) {
            Actor_Kill(&this->picto.actor);
            return;
        }
        this->path = &play->setupPathList[this->pathIndex];
        this->destPointIndex = 0;
    }
    EnMnk_PlayWalkSfx(this);
}

void EnMnk_Monkey_TalkBeforeGuideThroughWoods(EnMnk* this, PlayState* play) {
    Math_SmoothStepToS(&this->picto.actor.world.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0xBB8, 0xC8);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0xBB8, 0xC8);
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TextboxIsClosing(&this->picto.actor, play)) {
        EnMnk_StartNextPath(this, play);
        this->actionFunc = EnMnk_Monkey_GuideThroughWoods;
        this->flags |= MONKEY_FLAGS_8000;
    }
}

void EnMnk_Monkey_SetupTalkBeforeGuideThroughWoods(EnMnk* this) {
    switch ((u32)Rand_ZeroFloat(6.0f)) {
        case 0:
            EnMnk_Monkey_SetAnim(this, 3);
            break;

        case 1:
            EnMnk_Monkey_SetAnim(this, 4);
            break;

        case 2:
            EnMnk_Monkey_SetAnim(this, 5);
            break;

        case 3:
            EnMnk_Monkey_SetAnim(this, 6);
            break;

        case 4:
            EnMnk_Monkey_SetAnim(this, 2);
            break;

        default:
            EnMnk_Monkey_SetAnim(this, 7);
            break;
    }
    this->actionFunc = EnMnk_Monkey_TalkBeforeGuideThroughWoods;
}

void EnMnk_Monkey_WaitToGuideThroughWoods(EnMnk* this, PlayState* play) {
    func_80AB5F6C(this);
    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        EnMnk_Monkey_SetupTalkBeforeGuideThroughWoods(this);
    } else if (this->picto.actor.isLockedOn || (this->picto.actor.xzDistToPlayer < 100.0f)) {
        Actor_OfferTalk(&this->picto.actor, play, 120.0f);
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAVED_KOUME)) {
        Actor_Kill(&this->picto.actor);
    }
}

void EnMnk_Monkey_TalkAfterSaved(EnMnk* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x8E5:
                Message_ContinueTextbox(play, 0x8E6);
                EnMnk_Monkey_SetAnim(this, 3);
                break;

            case 0x8E6:
                Message_ContinueTextbox(play, 0x8E7);
                EnMnk_Monkey_SetAnim(this, 5);
                break;

            case 0x8E7:
                Message_CloseTextbox(play);
                this->actionFunc = EnMnk_Monkey_WaitToTalkAfterSaved;
                EnMnk_Monkey_SetAnim(this, 0);
                break;

            default:
                break;
        }
    }
}

void EnMnk_Monkey_WaitToTalkAfterSaved(EnMnk* this, PlayState* play) {
    this->picto.actor.shape.rot.y = this->picto.actor.yawTowardsPlayer;
    this->picto.actor.world.rot.y = this->picto.actor.yawTowardsPlayer;
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnMnk_Monkey_TalkAfterSaved;
        EnMnk_Monkey_SetAnim(this, 7);
    } else if ((this->picto.actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->picto.actor, 0x3000, play)) {
        Actor_OfferTalk(&this->picto.actor, play, 110.0f);
    }
}

void func_80AB9084(EnMnk* this, PlayState* play) {
    s16 cueId = 0;

    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->cueId) {
            case 2:
                EnMnk_Monkey_SetAnim(this, 0xD);
                break;

            case 3:
                EnMnk_Monkey_SetAnim(this, 0);
                break;

            case 4:
                EnMnk_Monkey_SetAnim(this, 0);
                break;

            default:
                break;
        }
        SkelAnime_Update(&this->skelAnime);
        SET_WEEKEVENTREG(WEEKEVENTREG_29_80);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_106)) {
        Cutscene_ActorTranslateAndYaw(&this->picto.actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_106));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_106)]->id;
    }

    if ((cueId == 0) && (this->animIndex != 0)) {
        EnMnk_Monkey_SetAnim(this, 0);
    } else if (this->cueId != cueId) {
        switch (cueId) {
            case 1:
                EnMnk_Monkey_ChangeAnim(this, 0, ANIMMODE_LOOP, 0.0f);
                break;

            case 2:
                EnMnk_Monkey_ChangeAnim(this, 12, ANIMMODE_ONCE, 0.0f);
                this->unk_3E0 = 6;
                break;

            case 3:
                EnMnk_Monkey_ChangeAnim(this, 11, ANIMMODE_ONCE, 0.0f);
                break;

            case 4:
                EnMnk_Monkey_ChangeAnim(this, 14, ANIMMODE_ONCE, -5.0f);
                break;

            default:
                break;
        }
        this->cueId = cueId;
    }

    if (this->animIndex == 11) {
        if ((this->skelAnime.curFrame >= 8.0f) && (this->skelAnime.curFrame < 14.0f)) {
            this->unk_3E0 = 2;
        } else {
            this->unk_3E0 = 0;
        }
        if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_MONKEY_VO_REWARD);
        }
    }
}

void func_80AB92CC(EnMnk* this, PlayState* play) {
    s16 cueId = 0;

    if (SkelAnime_Update(&this->skelAnime)) {
        switch (this->cueId) {
            case 2:
                EnMnk_Monkey_SetAnim(this, 0xD);
                break;

            case 3:
                EnMnk_Monkey_SetAnim(this, 0);
                break;

            case 4:
                EnMnk_Monkey_SetAnim(this, 0);
                break;

            default:
                break;
        }
        SkelAnime_Update(&this->skelAnime);
    }

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_106)) {
        Cutscene_ActorTranslateAndYaw(&this->picto.actor, play, Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_106));
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_106)]->id;
    }

    if ((cueId == 0) && (this->animIndex != 0)) {
        EnMnk_Monkey_SetAnim(this, 0);
    } else if (this->cueId != cueId) {
        switch (cueId) {
            case 1:
                EnMnk_Monkey_ChangeAnim(this, 0, ANIMMODE_LOOP, 0.0f);
                break;

            case 2:
                EnMnk_Monkey_ChangeAnim(this, 12, ANIMMODE_ONCE, 0.0f);
                this->unk_3E0 = 6;
                break;

            case 3:
                EnMnk_Monkey_ChangeAnim(this, 11, ANIMMODE_ONCE, 0.0f);
                break;

            case 4:
                EnMnk_Monkey_ChangeAnim(this, 14, ANIMMODE_ONCE, -5.0f);
                break;

            default:
                break;
        }
        this->cueId = cueId;
    }
    if (this->animIndex == 11) {
        if ((this->skelAnime.curFrame >= 8.0f) && (this->skelAnime.curFrame < 14.0f)) {
            this->unk_3E0 = 2;
        } else {
            this->unk_3E0 = 0;
        }
    }
}

void EnMnk_DoNothing(EnMnk* this, PlayState* play) {
}

void EnMnk_Update(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    if (!(this->flags & MONKEY_FLAGS_1)) {
        Actor_MoveWithGravity(&this->picto.actor);
    }
    if (!(this->flags & MONKEY_FLAGS_2)) {
        Actor_UpdateBgCheckInfo(play, &this->picto.actor, 20.0f, 25.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        if (this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->flags |= MONKEY_FLAGS_40;
        }
    }

    this->actionFunc(this, play);

    if (!(this->flags & MONKEY_FLAGS_20)) {
        if (!(this->flags & MONKEY_FLAGS_8)) {
            Collider_UpdateCylinder(&this->picto.actor, &this->collider);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->flags & MONKEY_FLAGS_200) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }

    this->blinkFrame = this->blinkTimer;
    if (this->blinkFrame >= 3) {
        this->blinkFrame = 0;
    }

    if (this->flags & MONKEY_FLAGS_80) {
        s8 csId = this->picto.actor.csId;

        if (csId == CS_ID_NONE) {
            this->flags &= ~MONKEY_FLAGS_80;
        } else if (CutsceneManager_IsNext(csId)) {
            CutsceneManager_StartWithPlayerCs(this->picto.actor.csId, &this->picto.actor);
            this->picto.actor.csId = CutsceneManager_GetAdditionalCsId(this->picto.actor.csId);
        } else {
            CutsceneManager_Queue(this->picto.actor.csId);
        }
    }
}

s32 EnMnk_Monkey_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnMnk* this = (EnMnk*)thisx;

    if (limbIndex == OBJECT_MNK_2_LIMB_03) {
        rot->x += this->unk_3CC;
        rot->z += this->unk_3CE;
    }
    return false;
}

s32 EnMnk_MonkeyTiedUp_PropOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                            Actor* thisx) {
    if (limbIndex == OBJECT_MNK_1_LIMB_01) {
        *dList = NULL;
    }
    return false;
}

s32 EnMnk_MonkeyHanging_PropOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                             Actor* thisx) {
    if ((limbIndex == OBJECT_MNK_3_LIMB_01) || (limbIndex == OBJECT_MNK_3_LIMB_02) ||
        (limbIndex == OBJECT_MNK_3_LIMB_03)) {
        *dList = NULL;
    }
    return false;
}

void EnMnk_Monkey_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMnk* this = (EnMnk*)thisx;

    if (limbIndex == OBJECT_MNK_2_LIMB_04) {
        Matrix_MultVec3f(&sMonkeyFocusPosOffset, &this->picto.actor.focus.pos);
    }
}

void EnMnk_MonkeyTiedUp_PropPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMnk* this = (EnMnk*)thisx;

    if (limbIndex == OBJECT_MNK_1_LIMB_04) {
        Matrix_Get(&this->unk_36C);
    }
}

void EnMnk_MonkeyHanging_PropPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMnk* this = (EnMnk*)thisx;

    switch (limbIndex) {
        case OBJECT_MNK_3_LIMB_01:
            if (*dList != NULL) {
                OPEN_DISPS(play->state.gfxCtx);

                Matrix_Push();
                Matrix_RotateZS(this->cueId, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, *dList);
                Matrix_Pop();

                CLOSE_DISPS(play->state.gfxCtx);
            }
            break;

        case OBJECT_MNK_3_LIMB_02:
            if (*dList != NULL) {
                OPEN_DISPS(play->state.gfxCtx);

                Matrix_Scale(this->approachPlayerRadius + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, *dList);

                CLOSE_DISPS(play->state.gfxCtx);
            }
            break;

        case OBJECT_MNK_3_LIMB_03:
            if (*dList != NULL) {
                OPEN_DISPS(play->state.gfxCtx);

                Matrix_Scale(1.0f, 1.0f / (this->approachPlayerRadius + 1.0f), 1.0f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, *dList);

                CLOSE_DISPS(play->state.gfxCtx);
            }
            Matrix_Get(&this->unk_36C);
            break;

        default:
            break;
    }
}

void EnMnk_Monkey_DrawFace(EnMnk* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);

    switch (this->unk_3E0) {
        case 4:
        case 5:
        case 6:
            if (this->blinkFrame != 2) {
                gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sMonkeyFaceTextures[this->unk_3E0]));
            } else {
                gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sMonkeyFaceTextures[this->blinkFrame]));
            }
            break;
        case 2:
        case 3:
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sMonkeyFaceTextures[this->unk_3E0]));
            break;

        default:
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sMonkeyFaceTextures[this->blinkFrame]));
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnMnk_Draw(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    EnMnk_Monkey_DrawFace(this, play);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMnk_Monkey_OverrideLimbDraw, EnMnk_Monkey_PostLimbDraw, &this->picto.actor);
}

void EnMnk_MonkeyTiedUp_Draw(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    SkelAnime_DrawFlexOpa(play, this->propSkelAnime.skeleton, this->propSkelAnime.jointTable,
                          this->propSkelAnime.dListCount, EnMnk_MonkeyTiedUp_PropOverrideLimbDraw,
                          EnMnk_MonkeyTiedUp_PropPostLimbDraw, &this->picto.actor);
    EnMnk_Monkey_DrawFace(this, play);
    Matrix_Mult(&this->unk_36C, MTXMODE_NEW);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMnk_Monkey_OverrideLimbDraw, EnMnk_Monkey_PostLimbDraw, &this->picto.actor);
}

void EnMnk_MonkeyHanging_Draw(Actor* thisx, PlayState* play) {
    EnMnk* this = (EnMnk*)thisx;

    SkelAnime_DrawFlexOpa(play, this->propSkelAnime.skeleton, this->propSkelAnime.jointTable,
                          this->propSkelAnime.dListCount, EnMnk_MonkeyHanging_PropOverrideLimbDraw,
                          EnMnk_MonkeyHanging_PropPostLimbDraw, &this->picto.actor);
    EnMnk_Monkey_DrawFace(this, play);
    Matrix_Mult(&this->unk_36C, MTXMODE_NEW);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMnk_Monkey_OverrideLimbDraw, EnMnk_Monkey_PostLimbDraw, &this->picto.actor);
}

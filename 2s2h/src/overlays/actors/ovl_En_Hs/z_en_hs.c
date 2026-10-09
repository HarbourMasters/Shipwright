/*
 * File: z_en_hs.c
 * Overlay: ovl_En_Hs
 * Description: Grog
 */

#include "z_en_hs.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnHs_Init(Actor* thisx, PlayState* play);
void EnHs_Destroy(Actor* thisx, PlayState* play);
void EnHs_Update(Actor* thisx, PlayState* play);
void EnHs_Draw(Actor* thisx, PlayState* play);

void func_80952FE0(EnHs* this, PlayState* play);
void func_80953098(EnHs* this, PlayState* play);
void func_80953180(EnHs* this, PlayState* play);
void EnHs_DoNothing(EnHs* this, PlayState* play);
void EnHs_SceneTransitToBunnyHoodDialogue(EnHs* this, PlayState* play);
void func_80953354(EnHs* this, PlayState* play);
void func_8095345C(EnHs* this, PlayState* play);

ActorProfile En_Hs_Profile = {
    /**/ ACTOR_EN_HS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_HS,
    /**/ sizeof(EnHs),
    /**/ EnHs_Init,
    /**/ EnHs_Destroy,
    /**/ EnHs_Update,
    /**/ EnHs_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 40, 40, 0, { 0, 0, 0 } },
};

Vec3f D_8095393C = { 300.0f, 1000.0f, 0.0f };

void func_80952C50(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->nwcPos); i++) {
        Math_Vec3f_Copy(&this->nwcPos[i], &player->actor.world.pos);
    }

    this->actor.home.rot.x = 0; // reset adult transformed count
    this->actor.home.rot.z = 0; // reset chick count
}

void EnHs_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHs* this = (EnHs*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gHsSkel, &gHsIdleAnim, this->jointTable, this->morphTable,
                       OBJECT_HS_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gHsIdleAnim);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = func_8095345C;

    if (play->curSpawn == 1) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }

    this->stateFlags = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    func_80952C50(this, play);
}

void EnHs_Destroy(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80952DFC(PlayState* play) {
    if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD);
    }
    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_GROG);
}

void EnHs_UpdateChickPos(Vec3f* dst, Vec3f src, f32 offset) {
    Vec3f diff;
    f32 distance;

    Math_Vec3f_Diff(&src, dst, &diff);

    distance = SQXZ(diff); // gets un-squared after we check if we are too close

    if (SQ(offset) > distance) {
        return;
    }

    distance = sqrtf(distance);
    diff.x *= (distance - offset) / distance;
    diff.z *= (distance - offset) / distance;

    dst->x += diff.x;
    dst->z += diff.z;
}

void func_80952F00(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    f32 offset;

    if (this->actor.home.rot.z >= 20) { // current chick count >= 10
        offset = 15.0f;
    } else {
        offset = 10.0f;
    }

    EnHs_UpdateChickPos(&this->nwcPos[0], player->actor.world.pos, offset);

    for (i = 1; i < ARRAY_COUNT(this->nwcPos); i++) {
        EnHs_UpdateChickPos(&this->nwcPos[i], this->nwcPos[i - 1], offset);
    }
}

void func_80952FE0(EnHs* this, PlayState* play) {
    if (this->stateTimer < 40) {
        Math_SmoothStepToS(&this->headRot.y, 0x1F40, 6, 0x1838, 0x64);
    } else if (this->stateTimer < 80) {
        Math_SmoothStepToS(&this->headRot.y, -0x1F40, 6, 0x1838, 0x64);
    } else {
        this->actionFunc = func_80953180;
        this->stateFlags &= ~4;
        Message_ContinueTextbox(play, 0x33F6);
        func_80952DFC(play);
    }
    this->stateTimer++;
}

void func_80953098(EnHs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_8095345C;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->stateFlags |= 0x10;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    } else {
        this->stateFlags |= 8;
        if (INV_CONTENT(ITEM_MASK_BUNNY) == ITEM_MASK_BUNNY) {
            Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 10000.0f, 50.0f);
        } else {
            Actor_OfferGetItem(&this->actor, play, GI_MASK_BUNNY, 10000.0f, 50.0f);
        }
    }
}

void func_80953180(EnHs* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x33F4: // laughing that they are all roosters (!)
            case 0x33F6: // Grog regrets not being able to see his chicks reach adult hood
                Message_CloseTextbox(play);
                this->actionFunc = func_8095345C;
                break;

            case 0x33F7: // notice his chicks are grown up, happy, wants to give you bunny hood
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Message_CloseTextbox(play);
                this->actionFunc = func_80953098;
                func_80953098(this, play);
                break;

            case 0x33F9: // laughing that they are all roosters (.)
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Message_CloseTextbox(play);
                this->actionFunc = func_8095345C;
                break;

            case 0x33F5: // He heard from his gramps (?) the moon is going to fall
                play->msgCtx.msgLength = 0;
                this->actionFunc = func_80952FE0;
                this->stateTimer = 0;
                this->headRot.z = 0;
                this->stateFlags |= 4;
                break;

            default:
                Message_CloseTextbox(play);
                this->actionFunc = func_8095345C;
                break;
        }
    }
}

void EnHs_DoNothing(EnHs* this, PlayState* play) {
}

void EnHs_SceneTransitToBunnyHoodDialogue(EnHs* this, PlayState* play) {
    if (DECR(this->stateTimer) == 0) {
        play->nextEntrance = play->setupExitList[HS_GET_EXIT_INDEX(&this->actor)];
        play->transitionTrigger = TRANS_TRIGGER_START;
        SET_WEEKEVENTREG(WEEKEVENTREG_25_08);
        this->actionFunc = EnHs_DoNothing;
    }
}

void func_80953354(EnHs* this, PlayState* play) {
    if (!Play_InCsMode(play)) {
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
        this->actionFunc = EnHs_SceneTransitToBunnyHoodDialogue;
    }
}

void func_809533A0(EnHs* this, PlayState* play) {
    u16 sp1E;

    if ((play->curSpawn == 1) && !(this->stateFlags & 0x20)) {
        sp1E = 0x33F7;
        this->stateFlags |= 0x20;
    } else if (this->stateFlags & 0x10) {
        sp1E = 0x33F9;
        this->stateFlags &= ~0x10;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_25_08)) {
        sp1E = 0x33F4;
    } else {
        sp1E = 0x33F5;
    }

    Message_StartTextbox(play, sp1E, &this->actor);

    if (sp1E == 0x33F4) {
        func_80952DFC(play);
    }
}

void func_8095345C(EnHs* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80953180;
        func_809533A0(this, play);
        if (this->stateFlags & 8) {
            func_80952DFC(play);
            this->stateFlags &= ~8;
        }
    } else if (this->actor.home.rot.x >= 20) { // chicks turned adult >= 10
        this->actionFunc = func_80953354;
        this->stateTimer = 40;
    } else if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
        this->stateFlags |= 1;
    } else if ((this->actor.xzDistToPlayer < 120.0f) && Player_IsFacingActor(&this->actor, 0x2000, play)) {
        Actor_OfferTalk(&this->actor, play, 130.0f);
        this->stateFlags |= 1;
    } else {
        this->stateFlags &= ~1;
    }
}

void EnHs_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHs* this = (EnHs*)thisx;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (SkelAnime_Update(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }

    this->actionFunc(this, play);

    func_80952F00(this, play);

    if (this->stateFlags & 4) {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    } else if (this->stateFlags & 1) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0x3200, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 6, 0x1838, 0x64);
    }
}

s32 EnHs_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnHs* this = (EnHs*)thisx;

    switch (limbIndex) {
        case HS_LIMB_HEAD:
            rot->x += this->headRot.y;
            rot->z += this->headRot.x;
            break;

        case HS_LIMB_HAIR_SPIKES:
            rot->x += this->headRot.y;
            rot->z += this->headRot.x;
            break;

        case HS_LIMB_HIDDEN_HAIR:
            // for some reason this hair is always removed here in the Override limb
            // if you do re-enable, make sure you add the head rot like above
            *dList = NULL;
            return false;

        // these two limbs both have empty enddisplaylist, they do nothing
        // at the same time (params == HS_TYPE_UNK1) is always false, because vanilla params is 0xFE01
        case OBJECT_HS_LIMB_0C:
            if (this->actor.params == HS_TYPE_UNK1) {
                *dList = NULL;
                return false;
            }
            break;

        case OBJECT_HS_LIMB_0D:
            if (this->actor.params == HS_TYPE_UNK1) {
                *dList = NULL;
                return false;
            }
            break;

        default:
            break;
    }
    return false;
}

void EnHs_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnHs* this = (EnHs*)thisx;

    if (limbIndex == HS_LIMB_HEAD) {
        Matrix_MultVec3f(&D_8095393C, &this->actor.focus.pos);
    }
}

void EnHs_Draw(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnHs_OverrideLimbDraw, EnHs_PostLimbDraw, &this->actor);
}

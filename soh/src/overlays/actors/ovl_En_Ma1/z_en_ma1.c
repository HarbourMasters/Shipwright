/*
 * File: z_en_ma1.c
 * Overlay: En_Ma1
 * Description: Child Malon
 */

#include "z_en_ma1.h"
#include "objects/object_ma1/object_ma1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_5 | ACTOR_FLAG_25)

void EnMa1_Init(Actor* thisx, GlobalContext* globalCtx);
void EnMa1_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnMa1_Update(Actor* thisx, GlobalContext* globalCtx);
void EnMa1_Draw(Actor* thisx, GlobalContext* globalCtx);

u16 EnMa1_GetText(GlobalContext* globalCtx, Actor* this);
s16 func_80AA0778(GlobalContext* globalCtx, Actor* this);

void func_80AA0D88(EnMa1* this, GlobalContext* globalCtx);
void func_80AA0EA0(EnMa1* this, GlobalContext* globalCtx);
void func_80AA0EFC(EnMa1* this, GlobalContext* globalCtx);
void func_80AA0F44(EnMa1* this, GlobalContext* globalCtx);
void func_80AA106C(EnMa1* this, GlobalContext* globalCtx);
void func_80AA10EC(EnMa1* this, GlobalContext* globalCtx);
void func_80AA1150(EnMa1* this, GlobalContext* globalCtx);
void EnMa1_DoNothing(EnMa1* this, GlobalContext* globalCtx);
void EnMa1_WaitForSongGive(EnMa1* this, GlobalContext* globalCtx);

const ActorInit En_Ma1_InitVars = {
    ACTOR_EN_MA1,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_MA1,
    sizeof(EnMa1),
    (ActorFunc)EnMa1_Init,
    (ActorFunc)EnMa1_Destroy,
    (ActorFunc)EnMa1_Update,
    (ActorFunc)EnMa1_Draw,
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* 0 */ ENMA1_ANIM_0,
    /* 1 */ ENMA1_ANIM_1,
    /* 2 */ ENMA1_ANIM_2,
    /* 3 */ ENMA1_ANIM_3
} EnMa1Animation;

static AnimationFrameCountInfo sAnimationInfo[] = {
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildIdleAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gMalonChildSingAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
};

static Vec3f D_80AA16B8 = { 800.0f, 0.0f, 0.0f };

static void* sMouthTextures[] = {
    gMalonChildNeutralMouthTex,
    gMalonChildSmilingMouthTex,
    gMalonChildTalkingMouthTex,
};

static void* sEyeTextures[] = {
    gMalonChildEyeOpenTex,
    gMalonChildEyeHalfTex,
    gMalonChildEyeClosedTex,
};

u16 EnMa1_GetText(GlobalContext* globalCtx, Actor* thisx) {
    u16 faceReaction = Text_GetFaceReaction(globalCtx, 0x17);

    if (faceReaction != 0) {
        return faceReaction;
    }
    if (!gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
            return 0x204A;
        }
    }
    if (gSaveContext.eventChkInf[1] & 0x40) {
        return 0x2049;
    }
    if (gSaveContext.eventChkInf[1] & 0x20) {
        if ((gSaveContext.infTable[8] & 0x20)) {
            return 0x2049;
        } else {
            return 0x2048;
        }
    }
    if (gSaveContext.eventChkInf[1] & 0x10) {
        return 0x2047;
    }
    if (gSaveContext.eventChkInf[1] & 4) {
        return 0x2044;
    }
    if (gSaveContext.infTable[8] & 0x10) {
        if (gSaveContext.infTable[8] & 0x800) {
            return 0x2043;
        } else {
            return 0x2042;
        }
    }
    return 0x2041;
}

s16 func_80AA0778(GlobalContext* globalCtx, Actor* thisx) {
    s16 ret = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x2041:
                    gSaveContext.infTable[8] |= 0x10;
                    gSaveContext.eventChkInf[1] |= 1;
                    ret = 0;
                    break;
                case 0x2043:
                    ret = 1;
                    break;
                case 0x2047:
                    gSaveContext.eventChkInf[1] |= 0x20;
                    ret = 0;
                    break;
                case 0x2048:
                    gSaveContext.infTable[8] |= 0x20;
                    ret = 0;
                    break;
                case 0x2049:
                    gSaveContext.eventChkInf[1] |= 0x40;
                    ret = 0;
                    break;
                case 0x2061:
                    ret = 2;
                    break;
                default:
                    ret = 0;
                    break;
            }
            break;
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(globalCtx)) {
                ret = 2;
            }
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                ret = 3;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            ret = 1;
            break;
    }
    return ret;
}

s32 func_80AA08C4(EnMa1* this, GlobalContext* globalCtx) {
    if ((this->actor.shape.rot.z == 3) && (gSaveContext.sceneSetupIndex == 5)) {
        return 1;
    }
    if (!LINK_IS_CHILD) {
        return 0;
    }
    if (((globalCtx->sceneNum == SCENE_MARKET_NIGHT) || (globalCtx->sceneNum == SCENE_MARKET_DAY)) &&
        !(gSaveContext.eventChkInf[1] & 0x10) && !(gSaveContext.infTable[8] & 0x800)) {
        return 1;
    }
    if ((globalCtx->sceneNum == SCENE_SPOT15) && !(gSaveContext.eventChkInf[1] & 0x10)) {
        if (gSaveContext.infTable[8] & 0x800) {
            return 1;
        } else {
            gSaveContext.infTable[8] |= 0x800;
            return 0;
        }
    }
    if ((globalCtx->sceneNum == SCENE_SOUKO) && IS_NIGHT && (gSaveContext.eventChkInf[1] & 0x10)) {
        return 1;
    }
    if (globalCtx->sceneNum != SCENE_SPOT20) {
        return 0;
    }
    if ((this->actor.shape.rot.z == 3) && IS_DAY && (gSaveContext.eventChkInf[1] & 0x10)) {
        return 1;
    }
    return 0;
}

void EnMa1_UpdateEyes(EnMa1* this) {
    if (DECR(this->blinkTimer) == 0) {
        this->eyeIndex += 1;
        if (this->eyeIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeIndex = 0;
        }
    }
}

void EnMa1_ChangeAnim(EnMa1* this, s32 index) {
    f32 frameCount = Animation_GetLastFrame(sAnimationInfo[index].animation);

    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, frameCount,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

void func_80AA0AF4(EnMa1* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 phi_a3;

    if ((this->unk_1E8.unk_00 == 0) && (this->skelAnime.animation == &gMalonChildSingAnim)) {
        phi_a3 = 1;
    } else {
        phi_a3 = 0;
    }

    this->unk_1E8.unk_18 = player->actor.world.pos;
    this->unk_1E8.unk_18.y -= -10.0f;

    func_80034A14(&this->actor, &this->unk_1E8, 0, phi_a3);
}

void func_80AA0B74(EnMa1* this) {
    if (this->skelAnime.animation == &gMalonChildSingAnim) {
        if (this->unk_1E8.unk_00 == 0) {
            if (this->unk_1E0 != 0) {
                this->unk_1E0 = 0;
                func_800F6584(0);
            }
        } else {
            if (this->unk_1E0 == 0) {
                this->unk_1E0 = 1;
                func_800F6584(1);
            }
        }
    }
}

void EnMa1_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(globalCtx, &this->skelAnime, &gMalonChildSkel, NULL, NULL, NULL, 0);
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(22), &sColChkInfoInit);

    if (gSaveContext.n64ddFlag) { // Skip Malon's multiple textboxes before getting an item
        gSaveContext.infTable[8] |= 0x800;
        gSaveContext.infTable[8] |= 0x10;
        gSaveContext.eventChkInf[1] |= 1;
    }

    if (!func_80AA08C4(this, globalCtx)) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_UpdateBgCheckInfo(globalCtx, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.targetMode = 6;
    this->unk_1E8.unk_00 = 0;

    if (!(gSaveContext.eventChkInf[1] & 0x10) || (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && !gSaveContext.n64ddFlag)) {
        this->actionFunc = func_80AA0D88;
        EnMa1_ChangeAnim(this, ENMA1_ANIM_2);
    } else {
        if (gSaveContext.n64ddFlag) { // Skip straight to "let's sing it together" textbox in the ranch
            gSaveContext.eventChkInf[1] |= 0x40;
        }

        this->actionFunc = func_80AA0F44;
        EnMa1_ChangeAnim(this, ENMA1_ANIM_2);
    }
}

void EnMa1_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnMa1* this = (EnMa1*)thisx;

    SkelAnime_Free(&this->skelAnime, globalCtx);
    Collider_DestroyCylinder(globalCtx, &this->collider);
}

void func_80AA0D88(EnMa1* this, GlobalContext* globalCtx) {
    if (this->unk_1E8.unk_00 != 0) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_1);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_3);
        }
    }

    if ((globalCtx->sceneNum == SCENE_SPOT15) && (gSaveContext.eventChkInf[1] & 0x10)) {
        Actor_Kill(&this->actor);
    } else if (!(gSaveContext.eventChkInf[1] & 0x10) || (CHECK_QUEST_ITEM(QUEST_SONG_EPONA) && !gSaveContext.n64ddFlag)) {
        if (this->unk_1E8.unk_00 == 2) {
            this->actionFunc = func_80AA0EA0;
            globalCtx->msgCtx.stateTimer = 4;
            globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        }
    }
}

void func_80AA0EA0(EnMa1* this, GlobalContext* globalCtx) {
    if (Actor_HasParent(&this->actor, globalCtx)) {
        this->actor.parent = NULL;
        this->actionFunc = func_80AA0EFC;
    } else {
        if (gSaveContext.n64ddFlag) {
            GetItemID getItemId = GetRandomizedItemIdFromKnownCheck(RC_HC_MALON_EGG, GI_WEIRD_EGG);
            func_8002F434(&this->actor, globalCtx, getItemId, 120.0f, 10.0f);
        } else {
            func_8002F434(&this->actor, globalCtx, GI_WEIRD_EGG, 120.0f, 10.0f);
        }
    }
}

void func_80AA0EFC(EnMa1* this, GlobalContext* globalCtx) {
    if (this->unk_1E8.unk_00 == 3) {
        this->unk_1E8.unk_00 = 0;
        this->actionFunc = func_80AA0D88;
        gSaveContext.eventChkInf[1] |= 4;
        globalCtx->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    }
}

void GivePlayerRandoRewardMalon(EnMa1* malon, GlobalContext* globalCtx, RandomizerCheck check) {
    GetItemID getItemId = GetRandomizedItemIdFromKnownCheck(check, GI_EPONAS_SONG);
    // Prevents flag from getting set if we weren't able to get the item (i.e. Player is holding shield
    // when closing the textbox).
    if (malon->actor.parent != NULL && malon->actor.parent->id == GET_PLAYER(globalCtx)->actor.id &&
        !Flags_GetTreasure(globalCtx, 0x1F)) {
        Flags_SetTreasure(globalCtx, 0x1F);
        // puts malon in the action that vanilla has her in after learning the song
        // (confirmed via breakpoints in a vanilla save).
        malon->actionFunc = func_80AA0D88;
    } else if (!Flags_GetTreasure(globalCtx, 0x1F)) {
        func_8002F434(&malon->actor, globalCtx, getItemId, 10000.0f, 100.0f);
    }
    // make malon sing again after giving the item.
    malon->unk_1E8.unk_00 = 0;
    malon->unk_1E0 = 1;
}

void func_80AA0F44(EnMa1* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->unk_1E8.unk_00 != 0) {
        if (this->skelAnime.animation != &gMalonChildIdleAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_1);
        }
    } else {
        if (this->skelAnime.animation != &gMalonChildSingAnim) {
            EnMa1_ChangeAnim(this, ENMA1_ANIM_3);
        }
    }

    if (gSaveContext.eventChkInf[1] & 0x40) {
        // When the player pulls out the Ocarina while close to Malon
        if (player->stateFlags2 & 0x1000000) {
            player->stateFlags2 |= 0x2000000;
            player->unk_6A8 = &this->actor;
            this->actor.textId = 0x2061;
            Message_StartTextbox(globalCtx, this->actor.textId, NULL);
            this->unk_1E8.unk_00 = 1;
            this->actor.flags |= ACTOR_FLAG_16;
            // when rando'ed, skip to the Item Giving. Otherwise go to the song teaching code.
            this->actionFunc = gSaveContext.n64ddFlag ? func_80AA1150 : func_80AA106C;
        } else if (this->actor.xzDistToPlayer < 30.0f + (f32)this->collider.dim.radius) {
            // somehow flags that the player is close to malon so that pulling out the Ocarina
            // triggers the code above this.
            player->stateFlags2 |= 0x800000;
        }
        // If rando'ed, a textbox is closing, it's malon's 'my mom wrote this song' text, AND we do have an ocarina
        // in our inventory. This allows us to grant the check when talking to malon with the ocarina in our inventory.
        if (gSaveContext.n64ddFlag && (Actor_TextboxIsClosing(&this->actor, globalCtx) && globalCtx->msgCtx.textId == 0x2049) &&
            (INV_CONTENT(ITEM_OCARINA_FAIRY) != ITEM_NONE || INV_CONTENT(ITEM_OCARINA_TIME) != ITEM_NONE)) {
            this->actionFunc = EnMa1_WaitForSongGive;
        }
    }
}

void func_80AA106C(EnMa1* this, GlobalContext* globalCtx) {
    GET_PLAYER(globalCtx)->stateFlags2 |= 0x800000;
    if (this->unk_1E8.unk_00 == 2) {
        Audio_OcaSetInstrument(2);
        func_8010BD58(globalCtx, OCARINA_ACTION_TEACH_EPONA);
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = func_80AA10EC;
    }
}

void func_80AA10EC(EnMa1* this, GlobalContext* globalCtx) {
    GET_PLAYER(globalCtx)->stateFlags2 |= 0x800000;
    if (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_SONG_DEMO_DONE) {
        func_8010BD58(globalCtx, OCARINA_ACTION_PLAYBACK_EPONA);
        this->actionFunc = func_80AA1150;
    }
}

void EnMa1_WaitForSongGive(EnMa1* this, GlobalContext* globalCtx) {
    // Actually give the song check.
    GivePlayerRandoRewardMalon(this, globalCtx, RC_SONG_FROM_MALON);
}

// Sets an Ocarina State necessary to not softlock in rando.
// This function should only be called in rando.
void EnMa1_EndTeachSong(EnMa1* this, GlobalContext* globalCtx) {
    if (globalCtx->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = func_80AA0F44;
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_04;
    }

    if (gSaveContext.n64ddFlag) {
        // Transition to the giving the song check on the next update run.
        this->actionFunc = EnMa1_WaitForSongGive;
    }
}

void func_80AA1150(EnMa1* this, GlobalContext* globalCtx) {
    GET_PLAYER(globalCtx)->stateFlags2 |= 0x800000;

    // When rando'ed, trigger the "song learned" Ocarina mode.
    if (gSaveContext.n64ddFlag && (Message_GetState(&globalCtx->msgCtx) == TEXT_STATE_CLOSING)) {
        globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_03;
    }

    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_03) {
        if (!gSaveContext.n64ddFlag) {
            globalCtx->nextEntranceIndex = 0x157;
            gSaveContext.nextCutsceneIndex = 0xFFF1;
            globalCtx->fadeTransition = 42;
            globalCtx->sceneLoadFlag = 0x14;
            this->actionFunc = EnMa1_DoNothing;
        } else {
            // When rando'ed, skip the cutscene, play the chime, reset some flags,
            // and give the song on next update.
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            this->actionFunc = EnMa1_EndTeachSong;
            this->actor.flags &= ~ACTOR_FLAG_16;
            globalCtx->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }
    }
}

void EnMa1_DoNothing(EnMa1* this, GlobalContext* globalCtx) {
}

void EnMa1_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnMa1* this = (EnMa1*)thisx;
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    SkelAnime_Update(&this->skelAnime);
    EnMa1_UpdateEyes(this);
    this->actionFunc(this, globalCtx);
    if (this->actionFunc != EnMa1_DoNothing) {
        func_800343CC(globalCtx, &this->actor, &this->unk_1E8.unk_00, (f32)this->collider.dim.radius + 30.0f,
                      EnMa1_GetText, func_80AA0778);
    }
    func_80AA0B74(this);
    func_80AA0AF4(this, globalCtx);
}

s32 EnMa1_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3s vec;

    if ((limbIndex == 2) || (limbIndex == 5)) {
        *dList = NULL;
    }
    if (limbIndex == 15) {
        Matrix_Translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        vec = this->unk_1E8.unk_08;
        Matrix_RotateX((vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_Translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limbIndex == 8) {
        vec = this->unk_1E8.unk_0E;
        Matrix_RotateX((-vec.y / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_RotateZ((-vec.x / 32768.0f) * M_PI, MTXMODE_APPLY);
    }
    return false;
}

void EnMa1_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnMa1* this = (EnMa1*)thisx;
    Vec3f vec = D_80AA16B8;

    if (limbIndex == 15) {
        Matrix_MultVec3f(&vec, &this->actor.focus.pos);
    }
}

void EnMa1_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnMa1* this = (EnMa1*)thisx;
    Camera* camera;
    f32 distFromCamera;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    camera = GET_ACTIVE_CAM(globalCtx);
    distFromCamera = Math_Vec3f_DistXZ(&this->actor.world.pos, &camera->eye);
    func_800F6268(distFromCamera, NA_BGM_LONLON);
    func_80093D18(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthTextures[this->mouthIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[this->eyeIndex]));

    SkelAnime_DrawFlexOpa(globalCtx, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMa1_OverrideLimbDraw, EnMa1_PostLimbDraw, this);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

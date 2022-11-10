/*
 * File: z_en_cow.c
 * Overlay: ovl_En_Cow
 * Description: Cow
 */

#include "z_en_cow.h"
#include "objects/object_cow/object_cow.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnCow_Init(Actor* thisx, PlayState* play);
void EnCow_Destroy(Actor* thisx, PlayState* play);
void EnCow_Update(Actor* thisx, PlayState* play);
void EnCow_Draw(Actor* thisx, PlayState* play);
void func_809DFE98(Actor* thisx, PlayState* play);
void func_809E0070(Actor* thisx, PlayState* play);

void func_809DF494(EnCow* this, PlayState* play);
void func_809DF6BC(EnCow* this, PlayState* play);
void EnCow_MoveForRandomizer(EnCow* this, PlayState* play);
void func_809DF778(EnCow* this, PlayState* play);
void func_809DF7D8(EnCow* this, PlayState* play);
void func_809DF870(EnCow* this, PlayState* play);
void func_809DF8FC(EnCow* this, PlayState* play);
void func_809DF96C(EnCow* this, PlayState* play);
void func_809DFA84(EnCow* this, PlayState* play);

const ActorInit En_Cow_InitVars = {
    ACTOR_EN_COW,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_COW,
    sizeof(EnCow),
    (ActorFunc)EnCow_Init,
    (ActorFunc)EnCow_Destroy,
    (ActorFunc)EnCow_Update,
    (ActorFunc)EnCow_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static Vec3f D_809E010C = { 0.0f, -1300.0f, 1100.0f };

void func_809DEE00(Vec3f* vec, s16 rotY) {
    f32 xCalc;
    f32 rotCalcTemp;

    rotCalcTemp = Math_CosS(rotY);
    xCalc = (Math_SinS(rotY) * vec->z) + (rotCalcTemp * vec->x);
    rotCalcTemp = Math_SinS(rotY);
    vec->z = (Math_CosS(rotY) * vec->z) + (-rotCalcTemp * vec->x);
    vec->x = xCalc;
}

void func_809DEE9C(EnCow* this) {
    Vec3f vec;

    vec.y = 0.0f;
    vec.x = 0.0f;
    vec.z = 30.0f;
    func_809DEE00(&vec, this->actor.shape.rot.y);
    this->colliders[0].dim.pos.x = this->actor.world.pos.x + vec.x;
    this->colliders[0].dim.pos.y = this->actor.world.pos.y;
    this->colliders[0].dim.pos.z = this->actor.world.pos.z + vec.z;

    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = -20.0f;
    func_809DEE00(&vec, this->actor.shape.rot.y);
    this->colliders[1].dim.pos.x = this->actor.world.pos.x + vec.x;
    this->colliders[1].dim.pos.y = this->actor.world.pos.y;
    this->colliders[1].dim.pos.z = this->actor.world.pos.z + vec.z;
}

void func_809DEF94(EnCow* this) {
    Vec3f vec;

    VEC_SET(vec, 0.0f, 57.0f, -36.0f);

    func_809DEE00(&vec, this->actor.shape.rot.y);
    this->actor.world.pos.x += vec.x;
    this->actor.world.pos.y += vec.y;
    this->actor.world.pos.z += vec.z;
}

void EnCow_Init(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;
    s32 pad;

    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_COWS)) {
        EnCow_MoveForRandomizer(thisx, play);
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 72.0f);
    switch (this->actor.params) {
        case 0:
            SkelAnime_InitFlex(play, &this->skelAnime, &gCowBodySkel, NULL, this->jointTable, this->morphTable, 6);
            Animation_PlayLoop(&this->skelAnime, &gCowBodyChewAnim);
            Collider_InitCylinder(play, &this->colliders[0]);
            Collider_SetCylinder(play, &this->colliders[0], &this->actor, &sCylinderInit);
            Collider_InitCylinder(play, &this->colliders[1]);
            Collider_SetCylinder(play, &this->colliders[1], &this->actor, &sCylinderInit);
            func_809DEE9C(this);
            this->actionFunc = func_809DF96C;
            if (play->sceneNum == SCENE_LINK_HOME) {
                if (!LINK_IS_ADULT && !CVar_GetS32("gCowOfTime", 0)) {
                    Actor_Kill(&this->actor);
                    return;
                }
                if (!(gSaveContext.eventChkInf[1] & 0x4000)) {
                    Actor_Kill(&this->actor);
                    return;
                }
            }
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_COW, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 1);
            this->unk_278 = Rand_ZeroFloat(1000.0f) + 40.0f;
            this->unk_27A = 0;
            this->actor.targetMode = 6;
            DREG(53) = 0;
            break;
        case 1:
            SkelAnime_InitFlex(play, &this->skelAnime, &gCowTailSkel, NULL, this->jointTable, this->morphTable, 6);
            Animation_PlayLoop(&this->skelAnime, &gCowTailIdleAnim);
            this->actor.update = func_809DFE98;
            this->actor.draw = func_809E0070;
            this->actionFunc = func_809DFA84;
            func_809DEF94(this);
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->unk_278 = ((u32)(Rand_ZeroFloat(1000.0f)) & 0xFFFF) + 40.0f;
            break;
    }
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->actor, 0.01f);
    this->unk_276 = 0;
}

void EnCow_Destroy(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    if (this->actor.params == 0) {
        Collider_DestroyCylinder(play, &this->colliders[0]);
        Collider_DestroyCylinder(play, &this->colliders[1]);
    }
}

void func_809DF494(EnCow* this, PlayState* play) {
    if (this->unk_278 > 0) {
        this->unk_278 -= 1;
    } else {
        this->unk_278 = Rand_ZeroFloat(500.0f) + 40.0f;
        Animation_Change(&this->skelAnime, &gCowBodyChewAnim, 1.0f, this->skelAnime.curFrame,
                         Animation_GetLastFrame(&gCowBodyChewAnim), ANIMMODE_ONCE, 1.0f);
    }

    if ((this->actor.xzDistToPlayer < 150.0f) && (!(this->unk_276 & 2))) {
        this->unk_276 |= 2;
        if (this->skelAnime.animation == &gCowBodyChewAnim) {
            this->unk_278 = 0;
        }
    }

    this->unk_27A += 1;
    if (this->unk_27A >= 0x31) {
        this->unk_27A = 0;
    }

    // (1.0f / 100.0f) instead of 0.01f below is necessary so 0.01f doesn't get reused mistakenly
    if (this->unk_27A < 0x20) {
        this->actor.scale.x = ((Math_SinS(this->unk_27A << 0xA) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        this->actor.scale.x = 0.01f;
    }

    if (this->unk_27A >= 0x11) {
        this->actor.scale.y = ((Math_SinS((this->unk_27A << 0xA) - 0x4000) * (1.0f / 100.0f)) + 1.0f) * 0.01f;
    } else {
        this->actor.scale.y = 0.01f;
    }
}

void func_809DF6BC(EnCow* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        Message_CloseTextbox(play);
        this->actionFunc = func_809DF96C;
    }
}

void func_809DF730(EnCow* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        this->actionFunc = func_809DF96C;
    }
}

void EnCow_MoveForRandomizer(EnCow* this, PlayState* play) {
    // Only move the cow body (the tail will be moved with the body)
    if (this->actor.params != 0) {
        return;
    }

    // Move left cow in lon lon tower
    if (play->sceneNum == SCENE_SOUKO && this->actor.world.pos.x == -108 && this->actor.world.pos.z == -65) {
        this->actor.world.pos.x = -229.0f;
        this->actor.world.pos.z = 157.0f;
        this->actor.shape.rot.y = 15783.0f;
    // Move right cow in lon lon stable
    } else if (play->sceneNum == SCENE_MALON_STABLE && this->actor.world.pos.x == -3 && this->actor.world.pos.z == -254) {
        this->actor.world.pos.x += 119.0f;
    }
}

void EnCow_SetCowMilked(EnCow* this, PlayState* play) {
    CowIdentity cowIdentity = Randomizer_IdentifyCow(play->sceneNum, this->actor.world.pos.x, this->actor.world.pos.z);
    Player* player = GET_PLAYER(play);
    player->pendingFlag.flagID = cowIdentity.randomizerInf;
    player->pendingFlag.flagType = FLAG_RANDOMIZER_INF;
}

void func_809DF778(EnCow* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_809DF730;
    } else {
        func_8002F434(&this->actor, play, GI_MILK, 10000.0f, 100.0f);
    }
}

void func_809DF7D8(EnCow* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actor.flags &= ~ACTOR_FLAG_16;
        Message_CloseTextbox(play);
        this->actionFunc = func_809DF778;
        func_8002F434(&this->actor, play, GI_MILK, 10000.0f, 100.0f);
    }
}

void func_809DF870(EnCow* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (Inventory_HasEmptyBottle()) {
            Message_ContinueTextbox(play, 0x2007);
            this->actionFunc = func_809DF7D8;
        } else {
            Message_ContinueTextbox(play, 0x2013);
            this->actionFunc = func_809DF6BC;
        }
    }
}

void func_809DF8FC(EnCow* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = func_809DF870;
    } else {
        this->actor.flags |= ACTOR_FLAG_16;
        func_8002F2CC(&this->actor, play, 170.0f);
        this->actor.textId = 0x2006;
    }
    func_809DF494(this, play);
}

bool EnCow_HasBeenMilked(EnCow* this, PlayState* play) {
    CowIdentity cowIdentity = Randomizer_IdentifyCow(play->sceneNum, this->actor.world.pos.x, this->actor.world.pos.z);
    return Flags_GetRandomizerInf(cowIdentity.randomizerInf);
}

void EnCow_GivePlayerRandomizedItem(EnCow* this, PlayState* play) {
    if (!EnCow_HasBeenMilked(this, play)) {
        CowIdentity cowIdentity = Randomizer_IdentifyCow(play->sceneNum, this->actor.world.pos.x, this->actor.world.pos.z);
        GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(cowIdentity.randomizerCheck, GI_MILK);
        GiveItemEntryFromActor(&this->actor, play, itemEntry, 10000.0f, 100.0f);
    } else {
        // once we've gotten the rando reward from the cow,
        // return them to the their default action function
        this->actionFunc = func_809DF96C;
    }
}

void func_809DF96C(EnCow* this, PlayState* play) {
    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_00) || (play->msgCtx.ocarinaMode == OCARINA_MODE_04)) {
        if (DREG(53) != 0) {
            if (this->unk_276 & 4) {
                this->unk_276 &= ~0x4;
                DREG(53) = 0;
            } else {
                if ((this->actor.xzDistToPlayer < 150.0f) &&
                    (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x61A8)) {
                    DREG(53) = 0;
                    // when randomized with cowsanity, if we haven't gotten the
                    // reward from this cow yet, give that, otherwise use the
                    // vanilla cow behavior
                    if (gSaveContext.n64ddFlag &&
                        Randomizer_GetSettingValue(RSK_SHUFFLE_COWS) &&
                        !EnCow_HasBeenMilked(this, play)) {
                        EnCow_SetCowMilked(this, play);
                        // setting the ocarina mode here prevents intermittent issues
                        // with the item get not triggering until walking away
                        play->msgCtx.ocarinaMode = OCARINA_MODE_00;
                        this->actionFunc = EnCow_GivePlayerRandomizedItem;
                        return;
                    }
                    this->actionFunc = func_809DF8FC;
                    this->actor.flags |= ACTOR_FLAG_16;
                    func_8002F2CC(&this->actor, play, 170.0f);
                    this->actor.textId = 0x2006;
                } else {
                    this->unk_276 |= 4;
                }
            }
        } else {
            this->unk_276 &= ~0x4;
        }
    }
    func_809DF494(this, play);
}

void func_809DFA84(EnCow* this, PlayState* play) {
    if (this->unk_278 > 0) {
        this->unk_278--;
    } else {
        this->unk_278 = Rand_ZeroFloat(200.0f) + 40.0f;
        Animation_Change(&this->skelAnime, &gCowTailIdleAnim, 1.0f, this->skelAnime.curFrame,
                         Animation_GetLastFrame(&gCowTailIdleAnim), ANIMMODE_ONCE, 1.0f);
    }

    if ((this->actor.xzDistToPlayer < 150.0f) &&
        (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) >= 0x61A9) && (!(this->unk_276 & 2))) {
        this->unk_276 |= 2;
        if (this->skelAnime.animation == &gCowTailIdleAnim) {
            this->unk_278 = 0;
        }
    }
}

void EnCow_Update(Actor* thisx, PlayState* play2) {
    EnCow* this = (EnCow*)thisx;
    PlayState* play = play2;
    s16 targetX;
    s16 targetY;
    Player* player = GET_PLAYER(play);

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliders[0].base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliders[1].base);
    Actor_MoveForward(thisx);
    Actor_UpdateBgCheckInfo(play, thisx, 0.0f, 0.0f, 0.0f, 4);
    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->skelAnime.animation == &gCowBodyChewAnim) {
            Audio_PlayActorSound2(thisx, NA_SE_EV_COW_CRY);
            Animation_Change(&this->skelAnime, &gCowBodyMoveHeadAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gCowBodyMoveHeadAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Animation_Change(&this->skelAnime, &gCowBodyChewAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCowBodyChewAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }
    this->actionFunc(this, play);
    if ((thisx->xzDistToPlayer < 150.0f) &&
        (ABS(Math_Vec3f_Yaw(&thisx->world.pos, &player->actor.world.pos)) < 0xC000)) {
        targetX = Math_Vec3f_Pitch(&thisx->focus.pos, &player->actor.focus.pos);
        targetY = Math_Vec3f_Yaw(&thisx->focus.pos, &player->actor.focus.pos) - thisx->shape.rot.y;

        if (targetX > 0x1000) {
            targetX = 0x1000;
        } else if (targetX < -0x1000) {
            targetX = -0x1000;
        }

        if (targetY > 0x2500) {
            targetY = 0x2500;
        } else if (targetY < -0x2500) {
            targetY = -0x2500;
        }

    } else {
        targetY = 0;
        targetX = 0;
    }
    Math_SmoothStepToS(&this->someRot.x, targetX, 0xA, 0xC8, 0xA);
    Math_SmoothStepToS(&this->someRot.y, targetY, 0xA, 0xC8, 0xA);
}

void func_809DFE98(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;
    s32 pad;

    if (SkelAnime_Update(&this->skelAnime) != 0) {
        if (this->skelAnime.animation == &gCowTailIdleAnim) {
            Animation_Change(&this->skelAnime, &gCowTailSwishAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gCowTailSwishAnim), ANIMMODE_ONCE, 1.0f);
        } else {
            Animation_Change(&this->skelAnime, &gCowTailIdleAnim, 1.0f, 0.0f, Animation_GetLastFrame(&gCowTailIdleAnim),
                             ANIMMODE_LOOP, 1.0f);
        }
    }
    this->actionFunc(this, play);
}

s32 EnCow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCow* this = (EnCow*)thisx;

    if (limbIndex == 2) {
        rot->y += this->someRot.y;
        rot->x += this->someRot.x;
    }
    if (limbIndex == 5) {
        *dList = NULL;
    }
    return false;
}

void EnCow_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnCow* this = (EnCow*)thisx;

    if (limbIndex == 2) {
        Matrix_MultVec3f(&D_809E010C, &this->actor.focus.pos);
    }
}

void EnCow_Draw(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    func_800943C8(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnCow_OverrideLimbDraw, EnCow_PostLimbDraw, this);
}

void func_809E0070(Actor* thisx, PlayState* play) {
    EnCow* this = (EnCow*)thisx;

    func_800943C8(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, this);
}

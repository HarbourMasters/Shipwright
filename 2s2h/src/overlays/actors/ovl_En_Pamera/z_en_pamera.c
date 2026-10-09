/*
 * File: z_en_pamera.c
 * Overlay: ovl_En_Pamera
 * Description: Pamela
 */

#include "z_en_pamera.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPamera_Init(Actor* thisx, PlayState* play);
void EnPamera_Destroy(Actor* thisx, PlayState* play);
void EnPamera_Update(Actor* thisx, PlayState* play);
void EnPamera_Draw(Actor* thisx, PlayState* play);

s32 func_80BD84F0(EnPamera* this, PlayState* play);
void func_80BD8588(EnPamera* this, PlayState* play);
void func_80BD8658(EnPamera* this);
void func_80BD8700(EnPamera* this);
void func_80BD8758(EnPamera* this, PlayState* play);
void func_80BD8908(EnPamera* this);
void func_80BD8964(EnPamera* this, PlayState* play);
void func_80BD8A38(EnPamera* this);
void func_80BD8A7C(EnPamera* this, PlayState* play);
void func_80BD8B50(EnPamera* this);
void func_80BD8B70(EnPamera* this, PlayState* play);
void func_80BD8CCC(EnPamera* this);
void func_80BD8D1C(EnPamera* this, PlayState* play);
void func_80BD8D80(EnPamera* this);
void func_80BD8DB0(EnPamera* this, PlayState* play);
void EnPamera_LookDownWell(EnPamera* this);
void func_80BD8F60(EnPamera* this, PlayState* play);
void func_80BD8FF0(EnPamera* this);
void func_80BD909C(EnPamera* this, PlayState* play);
s32 func_80BD9234(EnPamera* this, PlayState* play);
void func_80BD92D0(EnPamera* this, PlayState* play);
void func_80BD9338(EnPamera* this, PlayState* play);
void func_80BD9384(EnPamera* this, PlayState* play);
void func_80BD93CC(EnPamera* this, s16 arg1, s16 arg2);
void func_80BD93F4(EnPamera* this, PlayState* play);
void func_80BD94E0(EnPamera* this, PlayState* play);
void func_80BD9840(EnPamera* this, PlayState* play);
void func_80BD9904(EnPamera* this);
void func_80BD9928(EnPamera* this, PlayState* play);
void func_80BD9938(EnPamera* this);
void func_80BD994C(EnPamera* this, PlayState* play);
void func_80BD9A9C(EnPamera* this);
void EnPamera_HandleDialogue(EnPamera* this, PlayState* play);
void func_80BD9B4C(EnPamera* this, PlayState* play);
void func_80BD9C70(EnPamera* this, PlayState* play);
s32 func_80BD9CB8(EnPamera* this, PlayState* play);
void func_80BD9E60(EnPamera* this);
void func_80BD9E78(EnPamera* this, PlayState* play);
void func_80BD9E88(EnPamera* this);
void func_80BD9ED0(EnPamera* this, PlayState* play);
void func_80BD9EE0(EnPamera* this);
void func_80BD9F3C(EnPamera* this, PlayState* play);
void func_80BDA038(EnPamera* this);
void func_80BDA090(EnPamera* this, PlayState* play);
void func_80BDA0A0(EnPamera* this);
void func_80BDA0FC(EnPamera* this, PlayState* play);
void func_80BDA170(EnPamera* this);
void func_80BDA1C8(EnPamera* this, PlayState* play);
void func_80BDA288(EnPamera* this);
void func_80BDA2E0(EnPamera* this, PlayState* play);
void func_80BDA344(Actor* thisx, PlayState* play);

ActorProfile En_Pamera_Profile = {
    /**/ ACTOR_EN_PAMERA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_PAMERA,
    /**/ sizeof(EnPamera),
    /**/ EnPamera_Init,
    /**/ EnPamera_Destroy,
    /**/ EnPamera_Update,
    /**/ EnPamera_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 12, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum PamelaAnimation {
    /*  -1 */ PAMELA_ANIM_NONE = -1,
    /* 0x0 */ PAMELA_ANIM_0,
    /* 0x1 */ PAMELA_ANIM_1,
    /* 0x2 */ PAMELA_ANIM_2,
    /* 0x3 */ PAMELA_ANIM_3,
    /* 0x4 */ PAMELA_ANIM_4,
    /* 0x5 */ PAMELA_ANIM_5,
    /* 0x6 */ PAMELA_ANIM_6,
    /* 0x7 */ PAMELA_ANIM_7,
    /* 0x8 */ PAMELA_ANIM_8,
    /* 0x9 */ PAMELA_ANIM_9,
    /* 0xA */ PAMELA_ANIM_10,
    /* 0xB */ PAMELA_ANIM_11,
    /* 0xC */ PAMELA_ANIM_12,
    /* 0xD */ PAMELA_ANIM_MAX
} PamelaAnimation;

static AnimationInfo sAnimationInfo[PAMELA_ANIM_MAX] = {
    { &object_pamera_Anim_0005BC, 1.0f, 0, 0.0f, ANIMMODE_LOOP, -4.0f }, // PAMELA_ANIM_0
    { &object_pamera_Anim_008AE0, 1.0f, 0, 0.0f, ANIMMODE_LOOP, -4.0f }, // PAMELA_ANIM_1
    { &object_pamera_Anim_008E38, 1.0f, 0, 0.0f, ANIMMODE_LOOP, -4.0f }, // PAMELA_ANIM_2
    { &object_pamera_Anim_00A844, 1.0f, 0, 0.0f, ANIMMODE_LOOP, -4.0f }, // PAMELA_ANIM_3
    { &object_pamera_Anim_00B0C4, 1.0f, 0, 0.0f, ANIMMODE_LOOP, -4.0f }, // PAMELA_ANIM_4
    { &object_pamera_Anim_009870, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_5
    { &object_pamera_Anim_009F54, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_6
    { &object_pamera_Anim_00B5B0, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_7
    { &object_pamera_Anim_00BCC4, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_8
    { &object_pamera_Anim_00D9DC, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_9
    { &object_pamera_Anim_00E16C, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_10
    { &object_pamera_Anim_00C9F4, 1.0f, 0, 0.0f, ANIMMODE_ONCE, 0.0f },  // PAMELA_ANIM_11
    { &object_pamera_Anim_00D0F0, 1.0f, 0, 0.0f, ANIMMODE_LOOP, 0.0f },  // PAMELA_ANIM_12
};

static Vec3f D_80BDA5F0 = { 1000.0f, 0.0f, 0.0f };

static TexturePtr D_80BDA5FC[] = { object_pamera_Tex_0074E8, object_pamera_Tex_0078E8 };

static TexturePtr D_80BDA604[] = { object_pamera_Tex_0066E8, object_pamera_Tex_006AE8, object_pamera_Tex_006EE8 };

static TexturePtr D_80BDA610[] = { object_pamera_Tex_0072E8, object_pamera_Tex_0073E8 };

void EnPamera_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPamera* this = (EnPamera*)thisx;
    Vec3f sp44;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 15.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gPamelaSkel, &object_pamera_Anim_0005BC, this->jointTable,
                       this->morphTable, PAMELA_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &sColChkInfoInit2);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->unk_312 = 0;
    this->unk_310 = 0;
    this->unk_314 = 0;
    this->unk_316 = 0;
    this->hideInisdeTimer = 0;
    this->unk_322 = 0;
    this->unk_324 = 0;
    if (PAMELA_GET_F000(&this->actor)) {
        func_80BD9840(this, play);
    } else {
        func_80BD8588(this, play);
        func_80BD8658(this);

        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) ||
            CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20) || (gSaveContext.save.entrance == ENTRANCE(IKANA_CANYON, 9))) {
            Actor_Kill(&this->actor);
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_61_04)) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_59_01) || (gSaveContext.save.entrance != ENTRANCE(IKANA_CANYON, 2))) {
                if ((gSaveContext.save.entrance != ENTRANCE(IKANA_CANYON, 2)) &&
                    CHECK_WEEKEVENTREG(WEEKEVENTREG_59_01)) {
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_59_01);
                }
                func_80BD8700(this);
            } else {
                this->waypointIndex = this->pathCount - 1;
                Math_Vec3s_ToVec3f(&this->actor.world.pos, &this->pathPoints[this->waypointIndex]);
                Math_Vec3s_ToVec3f(&sp44, &this->pathPoints[this->waypointIndex - 1]);
                this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&sp44, &this->actor.world.pos);
                func_80BD8CCC(this);
            }
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_59_01);
            func_80BD8FF0(this);
        }
    }
}

s32 func_80BD84F0(EnPamera* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_DOOR].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_DOOR) && (Math_Vec3f_DistXZ(&this->actor.world.pos, &actor->world.pos) < 200.0f)) {
            this->actor.child = actor;
            return true;
        }
        actor = actor->next;
    }
    this->actor.child = NULL;
    return false;
}

void func_80BD8588(EnPamera* this, PlayState* play) {
    Path* path = &play->setupPathList[PAMELA_GET_PATH_INDEX(&this->actor)];
    Vec3f sp28;

    if (path == NULL) {
        Actor_Kill(&this->actor);
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_61_04)) {
        path = &play->setupPathList[path->additionalPathIndex];
    }
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->waypointIndex = 0;
    this->pathCount = path->count;
    this->additionalPathIndex = path->additionalPathIndex;
    Math_Vec3s_ToVec3f(&sp28, this->pathPoints);
    this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &sp28);
}

void func_80BD8658(EnPamera* this) {
    s32 i;
    s16 csId = this->actor.csId;

    for (i = 0; i < ARRAY_COUNT(this->csIdList); i++) {
        if (csId == CS_ID_NONE) {
            break;
        }
        this->csIdList[i] = csId;
        csId = CutsceneManager_GetAdditionalCsId(csId);
    }
}

void EnPamera_Destroy(Actor* thisx, PlayState* play) {
    EnPamera* this = (EnPamera*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BD8700(EnPamera* this) {
    this->hideInisdeTimer = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_0);
    this->actionFunc = func_80BD8758;
}

void func_80BD8758(EnPamera* this, PlayState* play) {
    if (this->hideInisdeTimer++ > 1800) {
        if (CutsceneManager_IsNext(this->csIdList[0]) && (this->csIdList[0] != CS_ID_NONE)) {
            CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->actor);
            Camera_SetFocalActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIdList[0])),
                                 &this->actor);
            this->actor.speed = 1.5f;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
            this->actor.shape.rot.y = this->actor.home.rot.y;
            this->actor.world.rot.y = this->actor.home.rot.y;
            func_80BD9338(this, play);
            func_80BD8908(this);
        } else if ((this->csIdList[0] != CS_ID_NONE) && (this->actor.xzDistToPlayer < 1000.0f)) {
            CutsceneManager_Queue(this->csIdList[0]);
        } else {
            this->actor.speed = 1.5f;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
            this->actor.shape.rot.y = this->actor.home.rot.y;
            this->actor.world.rot.y = this->actor.home.rot.y;
            func_80BD9338(this, play);
            func_80BD8908(this);
        }
    }
    if ((this->actor.draw != NULL) && (this->actionFunc != func_80BD8964)) {
        if ((this->actor.child != NULL) && (this->actor.child->id == ACTOR_EN_DOOR) &&
            (this->actor.child->world.rot.y == 0)) {
            this->actor.draw = NULL;
        }
    }
    if (func_80BD9234(this, play)) {
        // this is 1760 frames, which is ~90sec
        this->hideInisdeTimer = 88 * 20;
    }
}

void func_80BD8908(EnPamera* this) {
    this->actor.draw = EnPamera_Draw;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
    this->actionFunc = func_80BD8964;
}

void func_80BD8964(EnPamera* this, PlayState* play) {
    Vec3f vec;

    vec.x = this->pathPoints->x;
    vec.y = this->pathPoints->y;
    vec.z = this->pathPoints->z;

    if (Math_Vec3f_StepTo(&this->actor.world.pos, &vec, 1.0f) < 5.0f) {
        this->actor.speed = 1.5f;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
        SET_WEEKEVENTREG(WEEKEVENTREG_59_01);
        func_80BD8B50(this);
    }
}

void func_80BD8A38(EnPamera* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
    this->actionFunc = func_80BD8A7C;
}

void func_80BD8A7C(EnPamera* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos), 0xA,
                       0x3000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Math_Vec3f_StepTo(&this->actor.world.pos, &this->actor.home.pos, 1.5f) < 10.0f) {
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_59_01);
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_61_04)) {
            func_80BD92D0(this, play);
            SET_WEEKEVENTREG(WEEKEVENTREG_61_04);
        }
        func_80BD8700(this);
    }
}

void func_80BD8B50(EnPamera* this) {
    this->actionFunc = func_80BD8B70;
    this->actor.gravity = -2.0f;
}

void func_80BD8B70(EnPamera* this, PlayState* play) {
    Vec3f vec;
    s16 sp32;

    vec.x = this->pathPoints[this->waypointIndex].x;
    vec.y = this->pathPoints[this->waypointIndex].y;
    vec.z = this->pathPoints[this->waypointIndex].z;
    sp32 = Math_Vec3f_Yaw(&this->actor.world.pos, &vec);
    if (Math_Vec3f_StepToXZ(&this->actor.world.pos, &vec, this->actor.speed) > 10.0f) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, sp32, 0xA, 0x3000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (this->waypointIndex < (this->pathCount - 1)) {
        this->waypointIndex++;
    } else {
        func_80BD8CCC(this);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->actor.world.pos.y += this->actor.gravity;
}

void func_80BD8CCC(EnPamera* this) {
    this->hideInisdeTimer = 0;
    this->actor.speed = 0.0f;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_3);
    this->actionFunc = func_80BD8D1C;
}

void func_80BD8D1C(EnPamera* this, PlayState* play) {
    if (this->hideInisdeTimer++ > 200) {
        this->actor.speed = 1.5f;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_1);
        func_80BD8D80(this);
    }
}

void func_80BD8D80(EnPamera* this) {
    if (this->waypointIndex > 0) {
        this->waypointIndex--;
    }
    this->actor.gravity = -2.0f;
    this->actionFunc = func_80BD8DB0;
}

void func_80BD8DB0(EnPamera* this, PlayState* play) {
    Vec3f vec;
    s16 sp32;

    vec.x = this->pathPoints[this->waypointIndex].x;
    vec.y = this->pathPoints[this->waypointIndex].y;
    vec.z = this->pathPoints[this->waypointIndex].z;
    sp32 = Math_Vec3f_Yaw(&this->actor.world.pos, &vec);
    if (Math_Vec3f_StepToXZ(&this->actor.world.pos, &vec, this->actor.speed) > 10.0f) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, sp32, 0xA, 0x3000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (this->waypointIndex > 0) {
        this->waypointIndex--;
    } else {
        func_80BD9338(this, play);
        func_80BD8A38(this);
    }
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    this->actor.world.pos.y += this->actor.gravity;
}

void EnPamera_LookDownWell(EnPamera* this) {
    func_80BD93CC(this, 1, 1);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_4);
    this->actionFunc = func_80BD8F60;
}

void func_80BD8F60(EnPamera* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x3000, 0x1000);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_2);
        this->actor.speed = 3.0f;
        func_80BD93CC(this, 0, 0);
        func_80BD8D80(this);
    }
}

void func_80BD8FF0(EnPamera* this) {
    Vec3f pameraPos;
    s16 pameraYaw;

    this->waypointIndex = this->pathCount - 1;
    Math_Vec3s_ToVec3f(&this->actor.world.pos, &this->pathPoints[this->pathCount - 1]);
    Math_Vec3s_ToVec3f(&pameraPos, &this->pathPoints[this->pathCount - 2]);
    pameraYaw = Math_Vec3f_Yaw(&pameraPos, &this->actor.world.pos);
    this->actor.shape.rot.y = pameraYaw;
    this->actor.world.rot.y = pameraYaw;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_3);
    this->actionFunc = func_80BD909C;
}

void func_80BD909C(EnPamera* this, PlayState* play) {
}

void func_80BD90AC(EnPamera* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->actionFunc != func_80BD8758) &&
        (this->actionFunc != func_80BD8964) && (this->actionFunc != func_80BD8A7C) &&
        (this->actionFunc != func_80BD8F60) && ((this->actionFunc != func_80BD8B70) || (this->waypointIndex != 0)) &&
        ((this->actionFunc != func_80BD8DB0) || (this->actor.speed != 3.0f)) &&
        ((this->actor.xzDistToPlayer < 150.0f) ||
         ((this->actionFunc == func_80BD909C) &&
          (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 200.0f)))) {
        if (CutsceneManager_IsNext(this->csIdList[1]) && (this->csIdList[1] != CS_ID_NONE)) {
            CutsceneManager_StartWithPlayerCs(this->csIdList[1], &this->actor);
            Camera_SetFocalActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIdList[1])),
                                 &this->actor);
            EnPamera_LookDownWell(this);
        } else if (this->csIdList[1] != CS_ID_NONE) {
            CutsceneManager_Queue(this->csIdList[1]);
        } else {
            EnPamera_LookDownWell(this);
        }
    }
}

s32 func_80BD9234(EnPamera* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;

    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_BOM) && (Math_Vec3f_DistXZ(&this->actor.world.pos, &actor->world.pos) < 500.0f) &&
            (((EnBom*)actor)->timer == 0)) {
            return true;
        } else {
            actor = actor->next;
        }
    }
    return false;
}

void func_80BD92D0(EnPamera* this, PlayState* play) {
    Path* path;
    s32 pathIndex = this->additionalPathIndex;

    path = &play->setupPathList[pathIndex];
    if (pathIndex > PATH_INDEX_NONE) {
        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        this->waypointIndex = 0;
        this->pathCount = path->count;
        this->additionalPathIndex = path->additionalPathIndex;
    }
}

void func_80BD9338(EnPamera* this, PlayState* play) {
    Actor* actor;

    func_80BD84F0(this, play);
    actor = this->actor.child;
    if ((actor != NULL) && (actor->id == ACTOR_EN_DOOR)) {
        ((EnDoor*)actor)->openTimer = -50;
    }
}

void func_80BD9384(EnPamera* this, PlayState* play) {
    if (this->unk_316 < 40) {
        this->unk_316++;
    } else {
        this->unk_312++;
        if (this->unk_312 >= 3) {
            this->unk_312 = 0;
            this->unk_316 = 0;
        }
    }
}

void func_80BD93CC(EnPamera* this, s16 arg1, s16 arg2) {
    this->unk_310 = arg1;
    this->unk_314 = arg2;
}

void func_80BD93F4(EnPamera* this, PlayState* play) {
    if ((this->actionFunc == func_80BD8B70) || (this->actionFunc == func_80BD8DB0) ||
        (this->actionFunc == func_80BD8964) || (this->actionFunc == func_80BD8A7C)) {
        if (this->skelAnime.animation == &object_pamera_Anim_008AE0) {
            if (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 18.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
            }
        } else if ((this->skelAnime.animation == &object_pamera_Anim_008E38) &&
                   (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
        }
    }
}

void func_80BD94E0(EnPamera* this, PlayState* play) {
    if ((this->actionFunc != func_80BD8B70) && (this->actionFunc != func_80BD8964) &&
        (this->actionFunc != func_80BD909C) && (this->actionFunc != func_80BD8D1C) &&
        ((this->actionFunc != func_80BD8DB0) || (this->actor.speed == 3.0f))) {
        Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        Actor_TrackNone(&this->headRot, &this->torsoRot);
    }
}

void EnPamera_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPamera* this = (EnPamera*)thisx;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    func_80BD90AC(this, play);
    func_80BD9384(this, play);
    func_80BD94E0(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    func_80BD93F4(this, play);
}

s32 EnPamera_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPamera* this = (EnPamera*)thisx;

    if (limbIndex == PAMELA_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    return false;
}

void EnPamera_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnPamera* this = (EnPamera*)thisx;

    if (limbIndex == PAMELA_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80BDA5F0, &this->actor.focus.pos);
    }
}

void EnPamera_Draw(Actor* thisx, PlayState* play) {
    EnPamera* this = (EnPamera*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(D_80BDA604[this->unk_312]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_K0(D_80BDA610[this->unk_314]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_K0(D_80BDA5FC[this->unk_310]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnPamera_OverrideLimbDraw, EnPamera_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BD9840(EnPamera* this, PlayState* play) {
    this->actor.update = func_80BDA344;
    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
    this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
        func_80BD9E60(this);
        func_80BD9938(this);
    } else {
        this->actor.draw = NULL;
        func_80BD9E60(this);
        func_80BD9904(this);
    }
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
        func_801A0204(NA_BGM_MUSIC_BOX_HOUSE);
    } else {
        func_801A0204(NA_BGM_INSIDE_A_HOUSE);
    }
}

void func_80BD9904(EnPamera* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_80BD9928;
}

void func_80BD9928(EnPamera* this, PlayState* play) {
}

void func_80BD9938(EnPamera* this) {
    this->actionFunc = func_80BD994C;
}

void func_80BD994C(EnPamera* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (Player_GetMask(play) == PLAYER_MASK_GIBDO) {
            func_80BD93CC(this, 0, 1);
            Message_StartTextbox(play, 0x15A8, &this->actor);

            this->unk_324 = 0x15A8;
        } else if ((GET_PLAYER_FORM != PLAYER_FORM_HUMAN) ||
                   (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) &&
                    !CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20))) {
            func_80BD93CC(this, 1, 0);
            Message_StartTextbox(play, 0x158E, &this->actor);
            this->unk_324 = 0x158E;
        } else if (!(this->unk_322 & 1)) {
            this->unk_322 |= 1;
            Message_StartTextbox(play, 0x1587, &this->actor);
            this->unk_324 = 0x1587;
        } else {
            Message_StartTextbox(play, 0x158C, &this->actor);
            this->unk_324 = 0x158C;
        }
        func_80BD9A9C(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void func_80BD9A9C(EnPamera* this) {
    this->actionFunc = EnPamera_HandleDialogue;
}

void EnPamera_HandleDialogue(EnPamera* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        case TEXT_STATE_CHOICE:
            break;

        case TEXT_STATE_EVENT:
            func_80BD9B4C(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                func_80BD9938(this);
            }
            break;

        default:
            break;
    }
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x71C, 0xB6);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void func_80BD9B4C(EnPamera* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->unk_324) {
            case 0x1587:
                Message_StartTextbox(play, 0x1588, &this->actor);
                this->unk_324 = 0x1588;
                break;

            case 0x1588:
                Message_StartTextbox(play, 0x1589, &this->actor);
                this->unk_324 = 0x1589;
                break;

            case 0x1589:
                Message_StartTextbox(play, 0x158A, &this->actor);
                this->unk_324 = 0x158A;
                break;

            case 0x158A:
                Message_StartTextbox(play, 0x158B, &this->actor);
                this->unk_324 = 0x158B;
                break;

            case 0x158C:
                Message_StartTextbox(play, 0x158D, &this->actor);
                this->unk_324 = 0x158D;
                break;

            case 0x158E:
            case 0x15A8:
                Message_CloseTextbox(play);
                func_80BD9C70(this, play);
                break;

            default:
                break;
        }
    }
}

void func_80BD9C70(EnPamera* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(IKANA_CANYON, 2);
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_70;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
}

s32 func_80BD9CB8(EnPamera* this, PlayState* play) {
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_485)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_485);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;

            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    func_80BD9E88(this);
                    break;

                case 2:
                    if (this->actor.draw == NULL) {
                        this->actor.draw = EnPamera_Draw;
                        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    }
                    func_80BD9EE0(this);
                    break;

                case 3:
                    func_80BDA038(this);
                    break;

                case 4:
                    func_80BDA0A0(this);
                    break;

                case 5:
                    func_80BDA170(this);
                    break;

                case 6:
                    func_80BDA288(this);
                    break;

                default:
                    break;
            }
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        this->setupFunc(this, play);
        return true;
    }
    if ((play->csCtx.state == CS_STATE_IDLE) && CHECK_WEEKEVENTREG(WEEKEVENTREG_75_20)) {
        if ((this->actionFunc != func_80BD994C) && (this->actionFunc != EnPamera_HandleDialogue)) {
            this->actor.shape.rot.y = this->actor.world.rot.y;
            func_80BD9904(this);
            func_80BD9E60(this);
        }
    }
    this->cueId = 99;
    return false;
}

void func_80BD9E60(EnPamera* this) {
    this->unk_31E = 0;
    this->setupFunc = func_80BD9E78;
}

void func_80BD9E78(EnPamera* this, PlayState* play) {
}

void func_80BD9E88(EnPamera* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_0);
    this->unk_31E = 0;
    this->setupFunc = func_80BD9ED0;
}

void func_80BD9ED0(EnPamera* this, PlayState* play) {
}

void func_80BD9EE0(EnPamera* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_5);
    func_80BD93CC(this, 1, 0);
    this->unk_31E = 1;
    this->setupFunc = func_80BD9F3C;
}

void func_80BD9F3C(EnPamera* this, PlayState* play) {
    if (this->unk_31E == 1) {
        if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
            Animation_OnFrame(&this->skelAnime, 10.0f) || Animation_OnFrame(&this->skelAnime, 14.0f) ||
            Animation_OnFrame(&this->skelAnime, 18.0f) || Animation_OnFrame(&this->skelAnime, 22.0f) ||
            Animation_OnFrame(&this->skelAnime, 25.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
        }
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_31E = 0;
            func_80BD93CC(this, 0, 0);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_6);
        }
    }
}

void func_80BDA038(EnPamera* this) {
    func_80BD93CC(this, 0, 1);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_7);
    this->unk_31E = 0;
    this->setupFunc = func_80BDA090;
}

void func_80BDA090(EnPamera* this, PlayState* play) {
}

void func_80BDA0A0(EnPamera* this) {
    func_80BD93CC(this, 0, 1);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_8);
    this->unk_31E = 1;
    this->setupFunc = func_80BDA0FC;
}

void func_80BDA0FC(EnPamera* this, PlayState* play) {
    if (this->unk_31E == 1) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_31E = 0;
            func_80BD93CC(this, 0, 0);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_6);
        }
    }
}

void func_80BDA170(EnPamera* this) {
    this->unk_31E = 1;
    func_80BD93CC(this, 0, 1);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_9);
    this->setupFunc = func_80BDA1C8;
}

void func_80BDA1C8(EnPamera* this, PlayState* play) {
    if (this->unk_31E == 1) {
        if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
            Animation_OnFrame(&this->skelAnime, 10.0f) || Animation_OnFrame(&this->skelAnime, 14.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_PAMERA_WALK);
        }
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_31E = 0;
            func_80BD93CC(this, 0, 0);
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_10);
        }
    }
}

void func_80BDA288(EnPamera* this) {
    this->unk_31E = 1;
    func_80BD93CC(this, 0, 0);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_11);
    this->setupFunc = func_80BDA2E0;
}

void func_80BDA2E0(EnPamera* this, PlayState* play) {
    if (this->unk_31E == 1) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->unk_31E = 0;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, PAMELA_ANIM_12);
        }
    }
}

void func_80BDA344(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPamera* this = (EnPamera*)thisx;

    this->actionFunc(this, play);
    SkelAnime_Update(&this->skelAnime);
    func_80BD9384(this, play);
    if (func_80BD9CB8(this, play)) {
        // Pamela is outside
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_59_01)) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_59_01);
        }
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_61_04)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_61_04);
        }
        Actor_TrackNone(&this->headRot, &this->torsoRot);
    } else {
        func_80BD94E0(this, play);
        if (this->actionFunc == func_80BD994C) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
            play->roomCtx.unk7A[0]++;
        }
    }
}

/*
 * File: z_en_yabusame_mark.c
 * Overlay: ovl_En_Yabusame_Mark
 * Description: Horseback Archery Target (arrow hitbox)
 */

#include "z_en_yabusame_mark.h"
#include "vt.h"

#define FLAGS 0

void EnYabusameMark_Init(Actor* thisx, PlayState* play);
void EnYabusameMark_Destroy(Actor* thisx, PlayState* play);
void EnYabusameMark_Update(Actor* thisx, PlayState* play);
void func_80B42F74(EnYabusameMark* this, PlayState* play);

static ColliderQuadInit sQuadInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0001F824, 0x00, 0x00 },
        TOUCH_NONE | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

const ActorInit En_Yabusame_Mark_InitVars = {
    ACTOR_EN_YABUSAME_MARK,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnYabusameMark),
    (ActorFunc)EnYabusameMark_Init,
    (ActorFunc)EnYabusameMark_Destroy,
    (ActorFunc)EnYabusameMark_Update,
    NULL,
    NULL,
};

static Vec3f sCollisionVertices[] = {
    { 70.0f, 70.0f, 0.0f },      { 70.0f, -70.0f, 0.0f },      { -70.0f, 70.0f, 0.0f },    { -70.0f, -70.0f, 0.0f },
    { 90.0f, 130.0f, -120.0f },  { -25.0f, -80.0f, -130.0f },  { 90.0f, 130.0f, 120.0f },  { -25.0f, -80.0, 130.0f },
    { 115.0f, 160.0f, -150.0f }, { -50.0f, -140.0f, -160.0f }, { 115.0f, 160.0f, 150.0f }, { -50.0f, -140.0f, 160.0f },
};

static Vec3f sTargetPos[] = {
    { 3382.0f, 1734.0f, -4946.0f }, // small, furthest from entrance
    { 3360.0f, 1734.0f, 495.0f },   // small, closest to entrance
    { 4517.0f, 1682.0f, -1779.0f }, // medium, on the right
    { 4517.0f, 1682.0f, -2813.0f }, // medium, on the left
    { 4522.0f, 1727.0f, -2296.0f }, // large in the center
};

// 0: first ring
// 1: second ring
// 2: outside edge
static f32 sRingDistance[] = {
    20.0f, 40.0f,  60.0f,  777.0f, // small
    40.0f, 80.0f,  120.0f, 777.0f, // medium
    40.0f, 120.0f, 160.0f, 777.0f, // large
};

void EnYabusameMark_Destroy(Actor* thisx, PlayState* play) {
    EnYabusameMark* this = (EnYabusameMark*)thisx;

    Collider_DestroyQuad(play, &this->collider);
}

void EnYabusameMark_Init(Actor* thisx, PlayState* play) {
    EnYabusameMark* this = (EnYabusameMark*)thisx;

    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ やぶさめまと ☆☆☆☆☆ %x\n" VT_RST, this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->typeIndex = this->actor.params;
    this->actor.targetMode = 5;
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ 種類インデックス \t   ☆☆☆☆☆ %d\n" VT_RST, this->typeIndex);
    switch (this->typeIndex) {
        case 0:
            this->subTypeIndex = 0;
            if (this->actor.world.pos.z > 0.0f) {
                this->subTypeIndex = 1;
            }
            break;
        case 1:
            this->subTypeIndex = 2;
            if (this->actor.world.pos.z < -2000.0f) {
                this->subTypeIndex = 3;
            }
            break;
        case 2:
            this->subTypeIndex = 4;
            break;
    }
    Collider_InitQuad(play, &this->collider);
    Collider_SetQuad(play, &this->collider, &this->actor, &sQuadInit);
    this->worldPos = this->actor.world.pos;
    this->actor.flags |= ACTOR_FLAG_4;
    if (gSaveContext.sceneSetupIndex != 4) {
        Actor_Kill(&this->actor);
        return;
    }
    osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 種類       ☆☆☆☆☆ %d\n" VT_RST, this->typeIndex);
    osSyncPrintf(VT_FGCOL(CYAN) "☆☆☆☆☆ さらに分類 ☆☆☆☆☆ %d\n" VT_RST, this->subTypeIndex);
    this->actionFunc = func_80B42F74;
}

void func_80B42F74(EnYabusameMark* this, PlayState* play) {
    Vec3f effectAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f arrowHitPos;
    Vec3f distanceFromCenter;
    s32 pad;
    s32 scoreIndex;
    f32 scoreDistance100;
    f32 scoreDistance60;
    f32 scoreDistance30;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        arrowHitPos.x = this->collider.info.bumper.hitPos.x;
        arrowHitPos.y = this->collider.info.bumper.hitPos.y;
        arrowHitPos.z = this->collider.info.bumper.hitPos.z;

        effectVelocity.y = 15.0f;

        EffectSsHitMark_SpawnCustomScale(play, 0, 700, &arrowHitPos);

        scoreIndex = 2;

        scoreDistance100 = sRingDistance[this->typeIndex * 4 + 0];
        scoreDistance60 = sRingDistance[this->typeIndex * 4 + 1];
        scoreDistance30 = sRingDistance[this->typeIndex * 4 + 2];

        distanceFromCenter.x = fabsf(sTargetPos[this->subTypeIndex].x - arrowHitPos.x);
        distanceFromCenter.y = fabsf(sTargetPos[this->subTypeIndex].y - arrowHitPos.y);
        distanceFromCenter.z = fabsf(sTargetPos[this->subTypeIndex].z - arrowHitPos.z);

        if (distanceFromCenter.x > scoreDistance100 || distanceFromCenter.y > scoreDistance100 ||
            distanceFromCenter.z > scoreDistance100) {
            scoreIndex = 1;
            if (distanceFromCenter.x > scoreDistance60 || distanceFromCenter.y > scoreDistance60 ||
                distanceFromCenter.z > scoreDistance60) {
                scoreIndex = 0;
            }
            if (distanceFromCenter.x > scoreDistance30 || distanceFromCenter.y > scoreDistance30 ||
                distanceFromCenter.z > scoreDistance30) {
                return;
            }
        }

        osSyncPrintf("\n\n");
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ posＸ ☆☆☆☆☆ %f\n" VT_RST, arrowHitPos.x);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ posＹ ☆☆☆☆☆ %f\n" VT_RST, arrowHitPos.y);
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ posＺ ☆☆☆☆☆ %f\n" VT_RST, arrowHitPos.z);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ hitＸ ☆☆☆☆☆ %f\n" VT_RST, sTargetPos[this->subTypeIndex].x);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ hitＹ ☆☆☆☆☆ %f\n" VT_RST, sTargetPos[this->subTypeIndex].y);
        osSyncPrintf(VT_FGCOL(YELLOW) "☆☆☆☆☆ hitＺ ☆☆☆☆☆ %f\n" VT_RST, sTargetPos[this->subTypeIndex].z);
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 小    ☆☆☆☆☆ %f\n" VT_RST, scoreDistance100);
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ 大    ☆☆☆☆☆ %f\n" VT_RST, scoreDistance60);
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆☆☆ point ☆☆☆☆☆ %d\n" VT_RST, scoreIndex);
        osSyncPrintf("\n\n");

        if (scoreIndex == 2) {
            Audio_PlayFanfare(NA_BGM_ITEM_GET | 0x900);
        }
        if (scoreIndex == 1) {
            Audio_StopSfxById(NA_SE_SY_TRE_BOX_APPEAR);
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        }
        if (scoreIndex == 0) {
            func_80078884(NA_SE_SY_DECIDE);
        }
        EffectSsExtra_Spawn(play, &arrowHitPos, &effectVelocity, &effectAccel, 5, scoreIndex);
    }
}

void EnYabusameMark_Update(Actor* thisx, PlayState* play) {
    EnYabusameMark* this = (EnYabusameMark*)thisx;
    Vec3f* vertexArray;
    u32 arrayIndex;

    this->actionFunc(this, play);
    arrayIndex = this->typeIndex * 4;
    vertexArray = &sCollisionVertices[arrayIndex];

    this->vertexA.x = vertexArray[0].x + this->actor.world.pos.x;
    this->vertexA.y = vertexArray[0].y + this->actor.world.pos.y;
    this->vertexA.z = vertexArray[0].z + this->actor.world.pos.z;

    this->vertexB.x = vertexArray[1].x + this->actor.world.pos.x;
    this->vertexB.y = vertexArray[1].y + this->actor.world.pos.y;
    this->vertexB.z = vertexArray[1].z + this->actor.world.pos.z;

    this->vertexC.x = vertexArray[2].x + this->actor.world.pos.x;
    this->vertexC.y = vertexArray[2].y + this->actor.world.pos.y;
    this->vertexC.z = vertexArray[2].z + this->actor.world.pos.z;

    this->vertexD.x = vertexArray[3].x + this->actor.world.pos.x;
    this->vertexD.y = vertexArray[3].y + this->actor.world.pos.y;
    this->vertexD.z = vertexArray[3].z + this->actor.world.pos.z;

    Collider_SetQuadVertices(&this->collider, &this->vertexA, &this->vertexB, &this->vertexC, &this->vertexD);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    if (BREG(0)) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0, 0xFF, 0, 0xFF, 4, play->state.gfxCtx);
    }
}

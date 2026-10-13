/*
 * File: z_en_warp_uzu.c
 * Overlay: ovl_En_Warp_Uzu
 * Description: Pirates Fortress Telescope
 */

#include "z_en_warp_uzu.h"
#include "objects/object_warp_uzu/object_warp_uzu.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnWarpUzu_Init(Actor* thisx, PlayState* play);
void EnWarpUzu_Destroy(Actor* thisx, PlayState* play);
void EnWarpUzu_Update(Actor* thisx, PlayState* play);
void EnWarpUzu_Draw(Actor* thisx, PlayState* play);

void func_80A66208(EnWarpUzu* this, PlayState* play);
void func_80A66278(EnWarpUzu* this, PlayState* play);
void func_80A66384(EnWarpUzu* this, PlayState* play);
void EnWarpUzu_DoNothing(EnWarpUzu* this, PlayState* play);

ActorProfile En_Warp_Uzu_Profile = {
    /**/ ACTOR_EN_WARP_UZU,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_WARP_UZU,
    /**/ sizeof(EnWarpUzu),
    /**/ EnWarpUzu_Init,
    /**/ EnWarpUzu_Destroy,
    /**/ EnWarpUzu_Update,
    /**/ EnWarpUzu_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 43, -20, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void EnWarpUzu_Init(Actor* thisx, PlayState* play) {
    EnWarpUzu* this = (EnWarpUzu*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, thisx, &sCylinderInit);
    thisx->attentionRangeType = ATTENTION_RANGE_0;
    func_80A66208(this, play);
}

void EnWarpUzu_Destroy(Actor* thisx, PlayState* play) {
    EnWarpUzu* this = (EnWarpUzu*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80A66208(EnWarpUzu* this, PlayState* play) {
    static Vec3f D_80A664FC = { 0.0f, 53.0f, -29.0f };
    Vec3f sp24;

    this->actor.textId = 0;
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVec3f(&D_80A664FC, &sp24);
    Math_Vec3f_Sum(&this->actor.world.pos, &sp24, &this->actor.focus.pos);
    Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.shape.rot);
    this->actionFunc = func_80A66278;
}

void func_80A66278(EnWarpUzu* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        func_80A66384(this, play);
    } else {
        s16 phi_a0 = ABS((s16)(Actor_WorldYawTowardActor(&this->actor, &player->actor) - this->actor.shape.rot.y));
        s16 temp_v0 = player->actor.shape.rot.y - this->actor.shape.rot.y;
        s16 phi_v1 = ABS(temp_v0);

        if ((phi_a0 >= 0x2AAB) && (phi_v1 < 0x238E)) {
            Actor_OfferTalk(&this->actor, play, 70.0f);
        }
    }
}

void func_80A66384(EnWarpUzu* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(PIRATES_FORTRESS, 10);
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = play->nextEntrance;
    func_80169EFC(play);
    gSaveContext.respawnFlag = -2;
    this->actionFunc = EnWarpUzu_DoNothing;
}

void EnWarpUzu_DoNothing(EnWarpUzu* this, PlayState* play) {
}

void EnWarpUzu_Update(Actor* thisx, PlayState* play) {
    EnWarpUzu* this = (EnWarpUzu*)thisx;
    s32 pad;

    this->actor.cullingVolumeDistance = 1000.0f;
    this->actionFunc(this, play);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnWarpUzu_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_warp_uzu_DL_000EC0);
}

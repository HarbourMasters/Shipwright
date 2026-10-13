/*
 * File: z_obj_wind.c
 * Overlay: ovl_Obj_Wind
 * Description: Updraft Current (STT) and Water Current (PFInterior)
 */

#include "z_obj_wind.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjWind_Init(Actor* thisx, PlayState* play);
void ObjWind_Destroy(Actor* thisx, PlayState* play);
void ObjWind_Update(Actor* thisx, PlayState* play);
void ObjWind_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Wind_Profile = {
    /**/ ACTOR_OBJ_WIND,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjWind),
    /**/ ObjWind_Init,
    /**/ ObjWind_Destroy,
    /**/ ObjWind_Update,
    /**/ ObjWind_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 4000, ICHAIN_STOP),
};

void ObjWind_Init(Actor* thisx, PlayState* play) {
    ObjWind* this = (ObjWind*)thisx;
    s32 pad;
    WaterBox* waterbox;
    f32 ySurface;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &ySurface,
                             &waterbox) &&
        (this->actor.world.pos.y < ySurface)) {
        this->isUnderWater = true;
    }
}

void ObjWind_Destroy(Actor* thisx, PlayState* play) {
}

typedef struct ObjWindStruct {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s16 unk_2;
    /* 0x4 */ s16 unk_4;
    /* 0x6 */ s16 unk_6;
    /* 0x8 */ s16 unk_8;
} ObjWindStruct; // size = 0xA

static ObjWindStruct D_80B2448C[] = {
    { 530, 90, 3000, 2000, 0 },   { 1120, 100, 2000, 1000, 0 }, { 620, 100, 2000, 1000, 0 },
    { 1620, 100, 2000, 1000, 0 }, { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },  { 400, 100, 2000, 1000, 0 },
    { 400, 100, 2000, 1000, 0 },  { 600, 120, 3000, 2000, 0 },  { 500, 80, 3000, 2000, 0 },
    { 400, 60, 2000, 2000, 0 },   { 470, 70, 2000, 1000, 0 },
};

static s32 D_80B245CC = -1;

void ObjWind_Update(Actor* thisx, PlayState* play) {
    ObjWind* this = (ObjWind*)thisx;
    ObjWindStruct* entry = &D_80B2448C[OBJ_WIND_GET_TYPE(thisx)];
    Player* player;
    InfiniteLine line;
    Vec3f nearestPoint;
    Vec3f sp54;
    f32 upXZ; // reused temp
    f32 distToNearestPoint;

    if (D_80B245CC != DREG(85)) {
        if ((DREG(85) >= 0) && (DREG(85) < ARRAY_COUNT(D_80B2448C))) {
            DREG(80) = D_80B2448C[DREG(85)].unk_0;
            DREG(81) = D_80B2448C[DREG(85)].unk_2;
            DREG(82) = D_80B2448C[DREG(85)].unk_4;
            DREG(83) = D_80B2448C[DREG(85)].unk_6;
            DREG(84) = D_80B2448C[DREG(85)].unk_8;
            D_80B245CC = DREG(85);
        }
    } else {
        D_80B2448C[DREG(85)].unk_0 = DREG(80);
        D_80B2448C[DREG(85)].unk_2 = DREG(81);
        D_80B2448C[DREG(85)].unk_4 = DREG(82);
        D_80B2448C[DREG(85)].unk_6 = DREG(83);
        D_80B2448C[DREG(85)].unk_8 = DREG(84);
    }

    if ((OBJ_WIND_GET_SWITCH_FLAG(thisx) == 0x7F) || !Flags_GetSwitch(play, OBJ_WIND_GET_SWITCH_FLAG(thisx))) {
        player = GET_PLAYER(play);
        Math_Vec3f_Copy(&line.point, &this->actor.world.pos);
        line.dir.y = Math_CosS(this->actor.shape.rot.x);
        upXZ = Math_SinS(this->actor.shape.rot.x);
        line.dir.x = Math_SinS(this->actor.shape.rot.y) * upXZ;
        line.dir.z = Math_CosS(this->actor.shape.rot.y) * upXZ;
        distToNearestPoint = Math3D_LineClosestToPoint(&line, &player->actor.world.pos, &nearestPoint);
        if ((distToNearestPoint >= 0.0f) && (distToNearestPoint < entry->unk_0)) {
            upXZ = Math_Vec3f_DistXYZAndStoreDiff(&player->actor.world.pos, &nearestPoint, &sp54);
            if (upXZ < entry->unk_2) {
                f32 var_fa0;
                f32 windSpeedXZ = 1.0f; // reused temp
                f32 windMagnitude;
                f32 temp_ft0;
                f32 windSpeedX;
                f32 windSpeedY;
                f32 windSpeedZ;

                var_fa0 = 1.0f - distToNearestPoint / entry->unk_0;
                windMagnitude = ((f32)entry->unk_4 / 100.0f) *
                                ((var_fa0 * (1.0f - upXZ / entry->unk_2)) + ((f32)entry->unk_8 / 100.0f));
                if ((distToNearestPoint / entry->unk_0) > 0.8f) {
                    var_fa0 = windSpeedXZ - 1.0f;
                }
                temp_ft0 = ((f32)entry->unk_6 / 100.0f) * (upXZ / entry->unk_2 * var_fa0);
                if (upXZ != 0.0f) {
                    //! FAKE:
                    if (1) {}
                    upXZ = 1.0f / upXZ;
                }
                temp_ft0 *= upXZ;
                windSpeedX = (line.dir.x * windMagnitude) + (sp54.x * temp_ft0);
                windSpeedY = (line.dir.y * windMagnitude) + (sp54.y * temp_ft0);
                windSpeedZ = (line.dir.z * windMagnitude) + (sp54.z * temp_ft0);
                player->windSpeed = sqrtf(SQ(windSpeedX) + SQ(windSpeedY) + SQ(windSpeedZ));
                player->windAngleY = Math_Atan2S_XY(windSpeedZ, windSpeedX);

                windSpeedXZ = sqrtf(SQ(windSpeedX) + SQ(windSpeedZ));
                player->windAngleX = Math_Atan2S_XY(windSpeedXZ, windSpeedY);
            }
        }
    }
    this->actor.scale.x = (f32)entry->unk_2 / 50.0f;
    this->actor.scale.z = (f32)entry->unk_2 / 50.0f;
    this->actor.scale.y = (f32)entry->unk_0 / 400.0f;
}

void ObjWind_Draw(Actor* thisx, PlayState* play) {
    ObjWind* this = (ObjWind*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(&gameplay_keep_Matanimheader_07F218));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_07E8C0);

    CLOSE_DISPS(play->state.gfxCtx);
}

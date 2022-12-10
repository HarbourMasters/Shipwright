/*
 * File: z_bg_mizu_movebg.c
 * Overlay: ovl_Bg_Mizu_Movebg
 * Description: Kakariko Village Well Water
 */

#include "z_bg_mizu_movebg.h"
#include "overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.h"
#include "objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_4

#define MOVEBG_TYPE(params) (((u16)(params) >> 0xC) & 0xF)
#define MOVEBG_FLAGS(params) ((u16)(params)&0x3F)
#define MOVEBG_PATH_ID(params) (((u16)(params) >> 0x8) & 0xF)
#define MOVEBG_POINT_ID(params) ((u16)(params)&0xF)
#define MOVEBG_SPEED(params) (((u16)(params) >> 0x4) & 0xF)

void BgMizuMovebg_Init(Actor* thisx, PlayState* play);
void BgMizuMovebg_Destroy(Actor* thisx, PlayState* play);
void BgMizuMovebg_Update(Actor* thisx, PlayState* play);
void BgMizuMovebg_Draw(Actor* thisx, PlayState* play);

void func_8089E318(BgMizuMovebg* this, PlayState* play);
void func_8089E650(BgMizuMovebg* this, PlayState* play);
s32 func_8089E108(Path* pathList, Vec3f* pos, s32 pathId, s32 pointId);

const ActorInit Bg_Mizu_Movebg_InitVars = {
    ACTOR_BG_MIZU_MOVEBG,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_MIZU_OBJECTS,
    sizeof(BgMizuMovebg),
    (ActorFunc)BgMizuMovebg_Init,
    (ActorFunc)BgMizuMovebg_Destroy,
    (ActorFunc)BgMizuMovebg_Update,
    (ActorFunc)BgMizuMovebg_Draw,
    NULL,
};

static f32 D_8089EB40[] = { -115.200005f, -115.200005f, -115.200005f, 0.0f };

static Gfx* D_8089EB50[] = {
    gObjectMizuObjectsMovebgDL_000190, gObjectMizuObjectsMovebgDL_000680, gObjectMizuObjectsMovebgDL_000C20,
    gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_002E10,
    gObjectMizuObjectsMovebgDL_002E10, gObjectMizuObjectsMovebgDL_0011F0,
};

static CollisionHeader* D_8089EB70[] = {
    &gObjectMizuObjectsMovebgCol_0003F0, &gObjectMizuObjectsMovebgCol_000998, &gObjectMizuObjectsMovebgCol_000ED0,
    &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_003590,
    &gObjectMizuObjectsMovebgCol_003590, &gObjectMizuObjectsMovebgCol_0015F8,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static Vec3f D_8089EBA0 = { 0.0f, 80.0f, 23.0f };
static Vec3f D_8089EBAC = { 0.0f, 80.0f, 23.0f };

static u8 D_8089EE40;

s32 func_8089DC30(PlayState* play) {
    s32 result;

    if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F1_FLAG)) {
        result = 1;
    } else if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F2_FLAG)) {
        result = 2;
    } else if (Flags_GetSwitch(play, WATER_TEMPLE_WATER_F3_FLAG)) {
        result = 3;
    } else {
        result = 1;
    }
    return result;
}

void BgMizuMovebg_Init(Actor* thisx, PlayState* play) {
    s32 type;
    s32 waypointId;
    WaterBox* waterBoxes = play->colCtx.colHeader->waterBoxes;
    f32 temp;
    CollisionHeader* colHeader = NULL;
    Vec3f sp48;

    Actor_ProcessInitChain(thisx, sInitChain);
    ((BgMizuMovebg*)thisx)->homeY = thisx->world.pos.y;
    ((BgMizuMovebg*)thisx)->dlist = D_8089EB50[MOVEBG_TYPE(thisx->params)];
    DynaPolyActor_Init(&((BgMizuMovebg*)thisx)->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(D_8089EB70[MOVEBG_TYPE(thisx->params)], &colHeader);
    ((BgMizuMovebg*)thisx)->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);

    type = MOVEBG_TYPE(thisx->params);
    switch (type) {
        case 0:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisx)->homeY - 700.0f) {
                thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY - 700.0f;
            } else {
                thisx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E318;
            break;
        case 1:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisx)->homeY - 710.0f) {
                thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY - 710.0f;
            } else {
                thisx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E318;
            break;
        case 2:
            temp = waterBoxes[2].ySurface + 15.0f;
            if (temp < ((BgMizuMovebg*)thisx)->homeY - 700.0f) {
                thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY - 700.0f;
            } else {
                thisx->world.pos.y = temp;
            }
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E318;
            break;
        case 3:
            thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY + D_8089EB40[func_8089DC30(play)];
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E318;
            break;
        case 4:
        case 5:
        case 6:
            if (Flags_GetSwitch(play, MOVEBG_FLAGS(thisx->params))) {
                thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY + 115.19999999999999;
            } else {
                thisx->world.pos.y = ((BgMizuMovebg*)thisx)->homeY;
            }
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E318;
            break;
        case 7:
            ((BgMizuMovebg*)thisx)->scrollAlpha1 = 160;
            ((BgMizuMovebg*)thisx)->scrollAlpha2 = 160;
            ((BgMizuMovebg*)thisx)->scrollAlpha3 = 160;
            ((BgMizuMovebg*)thisx)->scrollAlpha4 = 160;
            waypointId = MOVEBG_POINT_ID(thisx->params);
            ((BgMizuMovebg*)thisx)->waypointId = waypointId;
            func_8089E108(play->setupPathList, &thisx->world.pos, MOVEBG_PATH_ID(thisx->params), waypointId);
            ((BgMizuMovebg*)thisx)->actionFunc = func_8089E650;
            break;
    }

    type = MOVEBG_TYPE(thisx->params);
    switch (type) {
        case 3:
        case 4:
        case 5:
        case 6:
            Matrix_RotateY(thisx->world.rot.y * (M_PI / 32768), MTXMODE_NEW);
            Matrix_MultVec3f(&D_8089EBA0, &sp48);

            if (Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_OBJ_HSBLOCK,
                                   thisx->world.pos.x + sp48.x, thisx->world.pos.y + sp48.y,
                                   thisx->world.pos.z + sp48.z, thisx->world.rot.x, thisx->world.rot.y,
                                   thisx->world.rot.z, 2) == NULL) {
                Actor_Kill(thisx);
            }
            break;
    }
}

void BgMizuMovebg_Destroy(Actor* thisx, PlayState* play) {
    BgMizuMovebg* this = (BgMizuMovebg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    switch (MOVEBG_TYPE(thisx->params)) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (this->sfxFlags & 2) {
                D_8089EE40 &= ~2;
            }
            break;
        case 7:
            if (this->sfxFlags & 1) {
                D_8089EE40 &= ~1;
            }
            break;
    }
}

s32 func_8089E108(Path* pathList, Vec3f* pos, s32 pathId, s32 pointId) {
    Path* path = pathList;
    Vec3s* point;

    path += pathId;
    point = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[pointId];

    pos->x = point->x;
    pos->y = point->y;
    pos->z = point->z;

    return 0;
}

void func_8089E198(BgMizuMovebg* this, PlayState* play) {
    f32 waterLevel = play->colCtx.colHeader->waterBoxes[2].ySurface;

    if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        this->scrollAlpha1 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        this->scrollAlpha1 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F1_Y) /
                                         (WATER_TEMPLE_WATER_F2_Y - WATER_TEMPLE_WATER_F1_Y) * (255 - 160));
    } else {
        this->scrollAlpha1 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        this->scrollAlpha2 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F3_Y) {
        this->scrollAlpha2 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F2_Y) /
                                         (WATER_TEMPLE_WATER_F3_Y - WATER_TEMPLE_WATER_F2_Y) * (255 - 160));
    } else {
        this->scrollAlpha2 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_B1_Y) {
        this->scrollAlpha3 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        this->scrollAlpha3 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_B1_Y) /
                                         (WATER_TEMPLE_WATER_F1_Y - WATER_TEMPLE_WATER_B1_Y) * (255 - 160));
    } else {
        this->scrollAlpha3 = 160;
    }

    this->scrollAlpha4 = this->scrollAlpha3;
}

void func_8089E318(BgMizuMovebg* this, PlayState* play) {
    WaterBox* waterBoxes = play->colCtx.colHeader->waterBoxes;
    f32 phi_f0;
    s32 type;
    Vec3f sp28;

    func_8089E198(this, play);

    type = MOVEBG_TYPE(this->dyna.actor.params);
    switch (type) {
        case 0:
        case 2:
            phi_f0 = waterBoxes[2].ySurface + 15.0f;
            if (phi_f0 < this->homeY - 700.0f) {
                this->dyna.actor.world.pos.y = this->homeY - 700.0f;
            } else {
                this->dyna.actor.world.pos.y = phi_f0;
            }
            break;
        case 1:
            phi_f0 = waterBoxes[2].ySurface + 15.0f;
            if (phi_f0 < this->homeY - 710.0f) {
                this->dyna.actor.world.pos.y = this->homeY - 710.0f;
            } else {
                this->dyna.actor.world.pos.y = phi_f0;
            }
            break;
        case 3:
            phi_f0 = this->homeY + D_8089EB40[func_8089DC30(play)];
            if (!Math_StepToF(&this->dyna.actor.world.pos.y, phi_f0, 1.0f)) {
                if (!(D_8089EE40 & 2) && MOVEBG_SPEED(this->dyna.actor.params) != 0) {
                    D_8089EE40 |= 2;
                    this->sfxFlags |= 2;
                }
                if (this->sfxFlags & 2) {
                    if (this->dyna.actor.room == 0) {
                        func_8002F974(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                    } else {
                        func_8002F948(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                    }
                }
            }
            break;
        case 4:
        case 5:
        case 6:
            if (Flags_GetSwitch(play, MOVEBG_FLAGS(this->dyna.actor.params))) {
                phi_f0 = this->homeY + 115.200005f;
            } else {
                phi_f0 = this->homeY;
            }
            if (!Math_StepToF(&this->dyna.actor.world.pos.y, phi_f0, 1.0f)) {
                if (!(D_8089EE40 & 2) && MOVEBG_SPEED(this->dyna.actor.params) != 0) {
                    D_8089EE40 |= 2;
                    this->sfxFlags |= 2;
                }
                if (this->sfxFlags & 2) {
                    func_8002F948(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
                }
            }
            break;
    }

    type = MOVEBG_TYPE(this->dyna.actor.params);
    switch (type) {
        case 3:
        case 4:
        case 5:
        case 6:
            if (play->roomCtx.curRoom.num == this->dyna.actor.room) {
                Matrix_RotateY(this->dyna.actor.world.rot.y * (M_PI / 32768), MTXMODE_NEW);
                Matrix_MultVec3f(&D_8089EBAC, &sp28);
                this->dyna.actor.child->world.pos.x = this->dyna.actor.world.pos.x + sp28.x;
                this->dyna.actor.child->world.pos.y = this->dyna.actor.world.pos.y + sp28.y;
                this->dyna.actor.child->world.pos.z = this->dyna.actor.world.pos.z + sp28.z;
                this->dyna.actor.child->flags &= ~ACTOR_FLAG_0;
            }
            break;
    }
}

void func_8089E650(BgMizuMovebg* this, PlayState* play) {
    Vec3f waypoint;
    f32 dist;
    f32 dx;
    f32 dy;
    f32 dz;

    this->dyna.actor.speedXZ = MOVEBG_SPEED(this->dyna.actor.params) * 0.1f;
    func_8089E108(play->setupPathList, &waypoint, MOVEBG_PATH_ID(this->dyna.actor.params), this->waypointId);
    dist = Actor_WorldDistXYZToPoint(&this->dyna.actor, &waypoint);
    if (dist < this->dyna.actor.speedXZ) {
        this->dyna.actor.speedXZ = dist;
    }
    func_80035844(&this->dyna.actor.world.pos, &waypoint, &this->dyna.actor.world.rot, 1);
    func_8002D97C(&this->dyna.actor);
    dx = waypoint.x - this->dyna.actor.world.pos.x;
    dy = waypoint.y - this->dyna.actor.world.pos.y;
    dz = waypoint.z - this->dyna.actor.world.pos.z;
    if (fabsf(dx) < 2.0f && fabsf(dy) < 2.0f && fabsf(dz) < 2.0f) {
        this->waypointId++;
        if (this->waypointId >= play->setupPathList[MOVEBG_PATH_ID(this->dyna.actor.params)].count) {
            this->waypointId = 0;
            func_8089E108(play->setupPathList, &this->dyna.actor.world.pos,
                          MOVEBG_PATH_ID(this->dyna.actor.params), 0);
        }
    }
    if (!(D_8089EE40 & 1) && MOVEBG_SPEED(this->dyna.actor.params) != 0) {
        D_8089EE40 |= 1;
        this->sfxFlags |= 1;
    }
    if (this->sfxFlags & 1) {
        func_8002F948(&this->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
    }
}

void BgMizuMovebg_Update(Actor* thisx, PlayState* play) {
    BgMizuMovebg* this = (BgMizuMovebg*)thisx;

    this->actionFunc(this, play);
}

void BgMizuMovebg_Draw(Actor* thisx, PlayState* play2) {
    BgMizuMovebg* this = (BgMizuMovebg*)thisx;
    PlayState* play = play2;
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx);

    frames = play->gameplayFrames;
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScrollEnvColor(play->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        this->scrollAlpha1));

    gSPSegment(POLY_OPA_DISP++, 0x09,
               Gfx_TwoTexScrollEnvColor(play->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        this->scrollAlpha2));

    gSPSegment(POLY_OPA_DISP++, 0x0A,
               Gfx_TwoTexScrollEnvColor(play->state.gfxCtx, 0, frames * 1, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        this->scrollAlpha3));

    gSPSegment(POLY_OPA_DISP++, 0x0B,
               Gfx_TwoTexScrollEnvColor(play->state.gfxCtx, 0, frames * 3, 0, 32, 32, 1, 0, 0, 32, 32, 0, 0, 0,
                                        this->scrollAlpha4));

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->dlist != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, this->dlist);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/*
 * File: z_obj_oshihiki.c
 * Overlay: ovl_Obj_Oshihiki
 * Description: Pushable Block
 */

#include "z_obj_oshihiki.h"
#include "overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjOshihiki_Init(Actor* thisx, PlayState* play);
void ObjOshihiki_Destroy(Actor* thisx, PlayState* play);
void ObjOshihiki_Update(Actor* thisx, PlayState* play);
void ObjOshihiki_Draw(Actor* thisx, PlayState* play);

void ObjOshihiki_OnScene(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_SetupOnActor(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_OnActor(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_SetupPush(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_Push(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_SetupFall(ObjOshihiki* this, PlayState* play);
void ObjOshihiki_Fall(ObjOshihiki* this, PlayState* play);

ActorProfile Obj_Oshihiki_Profile = {
    /**/ ACTOR_OBJ_OSHIHIKI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(ObjOshihiki),
    /**/ ObjOshihiki_Init,
    /**/ ObjOshihiki_Destroy,
    /**/ ObjOshihiki_Update,
    /**/ ObjOshihiki_Draw,
};

static f32 sScales[] = { 0.1f, 0.2f, 0.4f, 0.1f, 0.2f, 0.4f };

static Color_RGB8 sColors[] = {
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

// The vertices and center of the bottom face
static Vec3f sColCheckPoints[] = {
    { 29.99f, 1.01f, -29.99f }, { -29.99f, 1.01f, -29.99f }, { -29.99f, 1.01f, 29.99f },
    { 29.99f, 1.01f, 29.99f },  { 0.0f, 1.01f, 0.0f },
};

static Vec2f sFaceVtx[] = {
    { -30.0f, 0.0f }, { 30.0f, 0.0f }, { -30.0f, 60.0f }, { 30.0f, 60.0f }, { -30.0f, 20.0f }, { 30.0f, 20.0f },
};

static Vec2f sFaceDirection[] = {
    { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f }, { -1.0f, -1.0f }, { 1.0f, 3.0f }, { -1.0f, 3.0f },
};

s8 D_80918940[] = { 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void ObjOshihiki_RotateXZ(Vec3f* out, Vec3f* in, f32 sn, f32 cs) {
    out->x = (in->z * sn) + (in->x * cs);
    out->y = in->y;
    out->z = (in->z * cs) - (in->x * sn);
}

s32 ObjOshihiki_StrongEnough(ObjOshihiki* this, PlayState* play) {
    s32 strength = OBJOSHIHIKI_GET_F(&this->dyna.actor);

    if (this->cantMove) {
        return false;
    }

    if ((strength == OBJOSHIHIKI_F_0) || (strength == OBJOSHIHIKI_F_3)) {
        return true;
    }

    if (Player_GetStrength() >= PLAYER_STRENGTH_ZORA) {
        return true;
    }
    return false;
}

void ObjOshihiki_ResetFloors(ObjOshihiki* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->floorBgIds); i++) {
        this->floorBgIds[i] = BGCHECK_SCENE;
    }
}

ObjOshihiki* ObjOshihiki_GetBlockUnder(ObjOshihiki* this, PlayState* play) {
    DynaPolyActor* dyna;

    if ((this->floorBgIds[this->highestFloor] != BGCHECK_SCENE) &&
        (fabsf(this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) < 0.001f)) {
        dyna = DynaPoly_GetActor(&play->colCtx, this->floorBgIds[this->highestFloor]);
        if ((dyna != NULL) && (dyna->actor.id == ACTOR_OBJ_OSHIHIKI)) {
            return (ObjOshihiki*)dyna;
        }
    }
    return NULL;
}

void ObjOshihiki_UpdateInitPos(ObjOshihiki* this) {
    if (this->dyna.actor.home.pos.x < this->dyna.actor.world.pos.x) {
        while ((this->dyna.actor.world.pos.x - this->dyna.actor.home.pos.x) >= 60.0f) {
            this->dyna.actor.home.pos.x += 60.0f;
        }
    } else {
        while ((this->dyna.actor.home.pos.x - this->dyna.actor.world.pos.x) >= 60.0f) {
            this->dyna.actor.home.pos.x -= 60.0f;
        }
    }

    if (this->dyna.actor.home.pos.z < this->dyna.actor.world.pos.z) {
        while ((this->dyna.actor.world.pos.z - this->dyna.actor.home.pos.z) >= 60.0f) {
            this->dyna.actor.home.pos.z += 60.0f;
        }
    } else {
        while ((this->dyna.actor.home.pos.z - this->dyna.actor.world.pos.z) >= 60.0f) {
            this->dyna.actor.home.pos.z -= 60.0f;
        }
    }
}

s32 ObjOshihiki_NoSwitchPress(ObjOshihiki* this, DynaPolyActor* dyna, PlayState* play) {
    s16 switchFlag;

    if (dyna == NULL) {
        return true;
    }

    if (dyna->actor.id == ACTOR_OBJ_SWITCH) {
        switchFlag = OBJSWITCH_GET_7F00(&dyna->actor);

        switch (OBJSWITCH_GET_33(&dyna->actor)) {
            case OBJSWITCH_NORMAL_BLUE:
                if ((switchFlag == OBJOSHIHIKI_GET_SWITCH_FLAG(&this->dyna.actor)) &&
                    Flags_GetSwitch(play, switchFlag)) {
                    return false;
                }
                break;

            case OBJSWITCH_INVERSE_BLUE:
                if ((switchFlag == OBJOSHIHIKI_GET_SWITCH_FLAG(&this->dyna.actor)) &&
                    !Flags_GetSwitch(play, switchFlag)) {
                    return false;
                }
                break;
        }
    }
    return true;
}

void ObjOshihiki_SetScale(ObjOshihiki* this, PlayState* play) {
    Actor_SetScale(&this->dyna.actor, sScales[OBJOSHIHIKI_GET_F(&this->dyna.actor)]);
}

void ObjOshihiki_SetTextureStep(ObjOshihiki* this, PlayState* play) {
    switch (OBJOSHIHIKI_GET_F(&this->dyna.actor)) {
        case OBJOSHIHIKI_F_0:
        case OBJOSHIHIKI_F_3:
            this->textureStep = 0;
            break;

        case OBJOSHIHIKI_F_1:
        case OBJOSHIHIKI_F_4:
            this->textureStep = 1;
            break;

        case OBJOSHIHIKI_F_2:
        case OBJOSHIHIKI_F_5:
            this->textureStep = 2;
            break;
    }
}

void ObjOshihiki_SetColor(ObjOshihiki* this, PlayState* play) {
    Color_RGB8* src = &sColors[OBJOSHIHIKI_GET_F0(&this->dyna.actor)];

    this->color.r = src->r;
    this->color.g = src->g;
    this->color.b = src->b;
}

void ObjOshihiki_Init(Actor* thisx, PlayState* play) {
    ObjOshihiki* this = (ObjOshihiki*)thisx;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    if ((OBJOSHIHIKI_GET_FF00(&this->dyna.actor) >= OBJOSHIHIKI_FF00_0) &&
        (OBJOSHIHIKI_GET_FF00(&this->dyna.actor) < OBJOSHIHIKI_FF00_80)) {
        if (Flags_GetSwitch(play, OBJOSHIHIKI_GET_SWITCH_FLAG(&this->dyna.actor))) {
            switch (OBJOSHIHIKI_GET_F(&this->dyna.actor)) {
                case OBJOSHIHIKI_F_0:
                case OBJOSHIHIKI_F_1:
                case OBJOSHIHIKI_F_2:
                    Actor_Kill(&this->dyna.actor);
                    return;

                default:
                    break;
            }
        } else {
            switch (OBJOSHIHIKI_GET_F(&this->dyna.actor)) {
                case OBJOSHIHIKI_F_3:
                case OBJOSHIHIKI_F_4:
                case OBJOSHIHIKI_F_5:
                    Actor_Kill(&this->dyna.actor);
                    return;

                default:
                    break;
            }
        }
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gameplay_dangeon_keep_Colheader_007498);
    this->texture = Lib_SegmentedToVirtual(gameplay_dangeon_keep_Matanimheader_01B370);
    ObjOshihiki_SetScale(this, play);
    ObjOshihiki_SetTextureStep(this, play);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    ObjOshihiki_SetColor(this, play);
    ObjOshihiki_ResetFloors(this);
    ObjOshihiki_SetupOnActor(this, play);
}

void ObjOshihiki_Destroy(Actor* thisx, PlayState* play) {
    ObjOshihiki* this = (ObjOshihiki*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void ObjOshihiki_SetFloors(ObjOshihiki* this, PlayState* play) {
    s32 pad;
    Vec3f colCheckPoint;
    Vec3f colCheckOffset;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sColCheckPoints); i++) {
        colCheckOffset.x = sColCheckPoints[i].x * (this->dyna.actor.scale.x * 10.0f);
        colCheckOffset.y = sColCheckPoints[i].y * (this->dyna.actor.scale.y * 10.0f);
        colCheckOffset.z = sColCheckPoints[i].z * (this->dyna.actor.scale.z * 10.0f);

        ObjOshihiki_RotateXZ(&colCheckPoint, &colCheckOffset, this->yawSin, this->yawCos);

        colCheckPoint.x += this->dyna.actor.world.pos.x;
        colCheckPoint.y += this->dyna.actor.prevPos.y;
        colCheckPoint.z += this->dyna.actor.world.pos.z;

        this->floorHeights[i] = BgCheck_EntityRaycastFloor6(&play->colCtx, &this->floorPolys[i], &this->floorBgIds[i],
                                                            &this->dyna.actor, &colCheckPoint, 0.0f);
    }
}

s16 ObjOshihiki_GetHighestFloor(ObjOshihiki* this) {
    s32 i;
    s16 highestFloor = 0;

    for (i = 1; i < ARRAY_COUNT(this->floorHeights); i++) {
        if (this->floorHeights[i] > this->floorHeights[highestFloor]) {
            highestFloor = i;
        } else if ((this->floorBgIds[i] == BGCHECK_SCENE) &&
                   ((this->floorHeights[i] - this->floorHeights[highestFloor]) > -0.001f)) {
            highestFloor = i;
        }
    }

    return highestFloor;
}

void ObjOshihiki_SetGround(ObjOshihiki* this, PlayState* play) {
    ObjOshihiki_ResetFloors(this);
    ObjOshihiki_SetFloors(this, play);
    this->highestFloor = ObjOshihiki_GetHighestFloor(this);
    this->dyna.actor.floorHeight = this->floorHeights[this->highestFloor];
}

s32 ObjOshihiki_CheckFloor(ObjOshihiki* this, PlayState* play) {
    ObjOshihiki_SetGround(this, play);

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >= -0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        return true;
    }
    return false;
}

s32 ObjOshihiki_CheckGround(ObjOshihiki* this, PlayState* play) {
    if (this->dyna.actor.world.pos.y <= BGCHECK_Y_MIN + 10.0f) {
        Actor_Kill(&this->dyna.actor);
        return false;
    }

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >= -0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        return true;
    }
    return false;
}

s32 ObjOshihiki_CheckWall(PlayState* play, s16 angle, f32 direction, ObjOshihiki* this) {
    f32 maxDists[2];
    f32 maxDist;
    f32 sn;
    f32 cs;
    s32 i;
    Vec3f faceVtx;
    Vec3f faceVtxNext;
    Vec3f posResult;
    Vec3f faceVtxOffset;
    s32 bgId;
    CollisionPoly* outPoly;

    sn = Math_SinS(angle);
    cs = Math_CosS(angle);

    maxDists[0] = ((this->dyna.actor.scale.x * 300.0f) + 60.0f) - 0.5f;
    if (direction > 0.0f) {
        maxDists[1] = maxDists[0];
    } else {
        maxDists[0] = -maxDists[0];
        maxDists[1] = -(((this->dyna.actor.scale.x * 300.0f) + 90.0f) - 0.5f);
    }

    for (i = 0; i < ARRAY_COUNT(sFaceDirection); i++) {
        maxDist = maxDists[D_80918940[i]];

        faceVtxOffset.x = (sFaceVtx[i].x * this->dyna.actor.scale.x * 10.0f) + sFaceDirection[i].x;
        faceVtxOffset.y = (sFaceVtx[i].z * this->dyna.actor.scale.y * 10.0f) + sFaceDirection[i].z;
        faceVtxOffset.z = 0.0f;

        ObjOshihiki_RotateXZ(&faceVtx, &faceVtxOffset, sn, cs);

        faceVtx.x += this->dyna.actor.world.pos.x;
        faceVtx.y += this->dyna.actor.world.pos.y;
        faceVtx.z += this->dyna.actor.world.pos.z;

        faceVtxNext.x = (maxDist * sn) + faceVtx.x;
        faceVtxNext.y = faceVtx.y;
        faceVtxNext.z = (maxDist * cs) + faceVtx.z;

        if (BgCheck_EntityLineTest3(&play->colCtx, &faceVtx, &faceVtxNext, &posResult, &outPoly, true, false, false,
                                    true, &bgId, &this->dyna.actor, 0.0f)) {
            return true;
        }
    }
    return false;
}

s32 ObjOshihiki_MoveWithBlockUnder(ObjOshihiki* this, PlayState* play) {
    s32 pad;
    ObjOshihiki* blockUnder = ObjOshihiki_GetBlockUnder(this, play);

    if ((blockUnder != NULL) && (blockUnder->stateFlags & PUSHBLOCK_SETUP_PUSH)) {
        if (!ObjOshihiki_CheckWall(play, blockUnder->dyna.yRotation, blockUnder->direction, this)) {
            this->blockUnder = blockUnder;
        }
    }

    if (this->stateFlags & PUSHBLOCK_MOVE_UNDER) {
        if (this->blockUnder != NULL) {
            if (this->blockUnder->stateFlags & PUSHBLOCK_PUSH) {
                this->underDistX = this->blockUnder->dyna.actor.world.pos.x - this->blockUnder->dyna.actor.prevPos.x;
                this->underDistZ = this->blockUnder->dyna.actor.world.pos.z - this->blockUnder->dyna.actor.prevPos.z;
                this->dyna.actor.world.pos.x += this->underDistX;
                this->dyna.actor.world.pos.z += this->underDistZ;
                ObjOshihiki_UpdateInitPos(this);
                return true;
            }

            if (!(this->blockUnder->stateFlags & PUSHBLOCK_SETUP_PUSH)) {
                this->blockUnder = NULL;
            }
        }
    }
    return false;
}

void ObjOshihiki_SetupOnScene(ObjOshihiki* this, PlayState* play) {
    this->stateFlags |= PUSHBLOCK_SETUP_ON_SCENE;
    this->dyna.actor.gravity = 0.0f;
    this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.velocity.x = 0.0f;
    this->actionFunc = ObjOshihiki_OnScene;
}

void ObjOshihiki_OnScene(ObjOshihiki* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    this->stateFlags |= PUSHBLOCK_ON_SCENE;

    if (this->timer <= 0) {
        if (fabsf(this->dyna.pushForce) > 0.001f) {
            if (ObjOshihiki_StrongEnough(this, play)) {
                if (!ObjOshihiki_CheckWall(play, this->dyna.yRotation, this->dyna.pushForce, this)) {
                    this->direction = this->dyna.pushForce;
                    ObjOshihiki_SetupPush(this, play);
                    return;
                }
            }
            player->stateFlags2 &= ~PLAYER_STATE2_10;
            this->dyna.pushForce = 0.0f;
        }
    } else if (fabsf(this->dyna.pushForce) > 0.001f) {
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void ObjOshihiki_SetupOnActor(ObjOshihiki* this, PlayState* play) {
    this->stateFlags |= PUSHBLOCK_SETUP_ON_ACTOR;
    this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.velocity.x = 0.0f;
    this->dyna.actor.gravity = -1.0f;
    this->actionFunc = ObjOshihiki_OnActor;
}

void ObjOshihiki_OnActor(ObjOshihiki* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    DynaPolyActor* dyna;
    s32 sp20 = false;

    this->stateFlags |= PUSHBLOCK_ON_ACTOR;
    Actor_MoveWithGravity(&this->dyna.actor);

    if (ObjOshihiki_CheckFloor(this, play)) {
        if (this->floorBgIds[this->highestFloor] == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(this, play);
        } else {
            dyna = DynaPoly_GetActor(&play->colCtx, this->floorBgIds[this->highestFloor]);
            if (dyna != NULL) {
                DynaPolyActor_SetActorOnTop(dyna);
                DynaPolyActor_SetSwitchPressed(dyna);
                if ((this->timer <= 0) && (fabsf(this->dyna.pushForce) > 0.001f) &&
                    ObjOshihiki_StrongEnough(this, play) && ObjOshihiki_NoSwitchPress(this, dyna, play) &&
                    !ObjOshihiki_CheckWall(play, this->dyna.yRotation, this->dyna.pushForce, this)) {
                    this->direction = this->dyna.pushForce;
                    ObjOshihiki_SetupPush(this, play);
                    sp20 = true;
                }
            } else {
                ObjOshihiki_SetupOnScene(this, play);
            }
        }
    } else if (this->floorBgIds[this->highestFloor] == BGCHECK_SCENE) {
        ObjOshihiki_SetupFall(this, play);
    } else {
        dyna = DynaPoly_GetActor(&play->colCtx, this->floorBgIds[this->highestFloor]);
        if ((dyna != NULL) && (dyna->transformFlags & DYNA_TRANSFORM_POS)) {
            DynaPolyActor_SetActorOnTop(dyna);
            DynaPolyActor_SetSwitchPressed(dyna);
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        } else {
            ObjOshihiki_SetupFall(this, play);
        }
    }

    if (!sp20 && (fabsf(this->dyna.pushForce) > 0.001f)) {
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void ObjOshihiki_SetupPush(ObjOshihiki* this, PlayState* play) {
    this->stateFlags |= PUSHBLOCK_SETUP_PUSH;
    this->dyna.actor.gravity = 0.0f;
    if (GameInteractor_Should(VB_PUSH_BLOCK_SET_SPEED, true, this)) {
        this->pushSpeed = 2.0f;
    }
    this->actionFunc = ObjOshihiki_Push;
}

void ObjOshihiki_Push(ObjOshihiki* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    f32 pushDistSigned;
    s32 stopFlag;

    this->stateFlags |= PUSHBLOCK_PUSH;
    stopFlag = Math_StepToF(&this->pushDist, 60.0f, this->pushSpeed);

    pushDistSigned = ((this->direction >= 0.0f) ? 1.0f : -1.0f) * this->pushDist;
    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + (pushDistSigned * this->yawSin);
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + (pushDistSigned * this->yawCos);

    if (!ObjOshihiki_CheckFloor(this, play)) {
        this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
        this->pushDist = 0.0f;
        this->pushSpeed = 0.0f;
        ObjOshihiki_SetupFall(this, play);
    } else if (stopFlag) {
        player = GET_PLAYER(play);

        if (ObjOshihiki_CheckWall(play, this->dyna.yRotation, this->dyna.pushForce, this)) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }

        this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
        this->pushDist = 0.0f;
        this->pushSpeed = 0.0f;
        if (GameInteractor_Should(VB_PUSH_BLOCK_SET_TIMER, true, this)) {
            this->timer = 10;
        }

        if (this->floorBgIds[this->highestFloor] == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(this, play);
        } else {
            ObjOshihiki_SetupOnActor(this, play);
        }
    }

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void ObjOshihiki_SetupFall(ObjOshihiki* this, PlayState* play) {
    this->stateFlags |= PUSHBLOCK_SETUP_FALL;
    this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->dyna.actor.velocity.x = 0.0f;
    this->dyna.actor.gravity = -1.0f;
    ObjOshihiki_SetGround(this, play);
    this->actionFunc = ObjOshihiki_Fall;
}

void ObjOshihiki_Fall(ObjOshihiki* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp_t4;

    this->stateFlags |= PUSHBLOCK_FALL;

    if (fabsf(this->dyna.pushForce) > 0.001f) {
        this->dyna.pushForce = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
    }

    Actor_MoveWithGravity(&this->dyna.actor);

    if (ObjOshihiki_CheckGround(this, play)) {
        if (this->floorBgIds[this->highestFloor] == BGCHECK_SCENE) {
            ObjOshihiki_SetupOnScene(this, play);
        } else {
            ObjOshihiki_SetupOnActor(this, play);
        }
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        Actor_PlaySfx(&this->dyna.actor, NA_SE_PL_WALK_GROUND + SurfaceType_GetSfxOffset(
                                                                    &play->colCtx, this->floorPolys[this->highestFloor],
                                                                    this->floorBgIds[this->highestFloor]));
    }
}

void ObjOshihiki_Update(Actor* thisx, PlayState* play) {
    ObjOshihiki* this = (ObjOshihiki*)thisx;

    this->stateFlags &=
        ~(PUSHBLOCK_SETUP_FALL | PUSHBLOCK_FALL | PUSHBLOCK_SETUP_PUSH | PUSHBLOCK_PUSH | PUSHBLOCK_SETUP_ON_ACTOR |
          PUSHBLOCK_ON_ACTOR | PUSHBLOCK_SETUP_ON_SCENE | PUSHBLOCK_ON_SCENE);
    this->stateFlags |= PUSHBLOCK_MOVE_UNDER;
    if (this->timer > 0) {
        this->timer--;
    }

    this->dyna.actor.world.rot.y = this->dyna.yRotation;
    this->yawSin = Math_SinS(this->dyna.actor.world.rot.y);
    this->yawCos = Math_CosS(this->dyna.actor.world.rot.y);

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void ObjOshihiki_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjOshihiki* this = (ObjOshihiki*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (ObjOshihiki_MoveWithBlockUnder(this, play)) {
        Matrix_Translate(this->underDistX * 10.0f, 0.0f, this->underDistZ * 10.0f, MTXMODE_APPLY);
    }

    this->stateFlags &= ~PUSHBLOCK_MOVE_UNDER;
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    AnimatedMat_DrawStep(play, this->texture, this->textureStep);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0xFF, 0xFF, this->color.r, this->color.g, this->color.b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_dangeon_keep_DL_0182A8);

    CLOSE_DISPS(play->state.gfxCtx);
}

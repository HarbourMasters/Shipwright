/*
 * File: z_bg_heavy_block.c
 * Overlay: ovl_Bg_Heavy_Block
 * Description: Large block that can only be lifted with Golden Gauntlets
 */

#include "z_bg_heavy_block.h"
#include "objects/object_heavy_object/object_heavy_object.h"
#include "vt.h"

#define FLAGS 0

#define PIECE_FLAG_HIT_FLOOR (1 << 0)

void BgHeavyBlock_Init(Actor* thisx, PlayState* play);
void BgHeavyBlock_Destroy(Actor* thisx, PlayState* play);
void BgHeavyBlock_Update(Actor* thisx, PlayState* play);
void BgHeavyBlock_Draw(Actor* thisx, PlayState* play);

void BgHeavyBlock_DrawPiece(Actor* thisx, PlayState* play);

void BgHeavyBlock_MovePiece(BgHeavyBlock* this, PlayState* play);
void BgHeavyBlock_Wait(BgHeavyBlock* this, PlayState* play);
void BgHeavyBlock_LiftedUp(BgHeavyBlock* this, PlayState* play);
void BgHeavyBlock_Fly(BgHeavyBlock* this, PlayState* play);
void BgHeavyBlock_Land(BgHeavyBlock* this, PlayState* play);
void BgHeavyBlock_DoNothing(BgHeavyBlock* this, PlayState* play);

const ActorInit Bg_Heavy_Block_InitVars = {
    ACTOR_BG_HEAVY_BLOCK,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HEAVY_OBJECT,
    sizeof(BgHeavyBlock),
    (ActorFunc)BgHeavyBlock_Init,
    (ActorFunc)BgHeavyBlock_Destroy,
    (ActorFunc)BgHeavyBlock_Update,
    (ActorFunc)BgHeavyBlock_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 400, ICHAIN_STOP),
};

void BgHeavyBlock_SetPieceRandRot(BgHeavyBlock* this, f32 scale) {
    this->dyna.actor.world.rot.x = Rand_CenteredFloat(1024.0f) * scale;
    this->dyna.actor.world.rot.y = Rand_CenteredFloat(1024.0f) * scale;
    this->dyna.actor.world.rot.z = Rand_CenteredFloat(1024.0f) * scale;
}

void BgHeavyBlock_InitPiece(BgHeavyBlock* this, f32 scale) {
    f32 rand;
    f32 yawSinCos;
    f32 randChoice;

    this->dyna.actor.gravity = -0.6f;
    this->dyna.actor.minVelocityY = -12.0f;
    randChoice = Rand_CenteredFloat(12.0f * scale);
    rand = (randChoice < 0.0f) ? randChoice - 2.0f : randChoice + 2.0f;
    this->dyna.actor.velocity.y = (Rand_ZeroFloat(8.0f) + 4.0f) * scale;
    this->dyna.actor.velocity.z = Rand_ZeroFloat(-8.0f * scale);
    yawSinCos = Math_CosS(this->dyna.actor.world.rot.y);
    this->dyna.actor.velocity.x =
        (Math_SinS(this->dyna.actor.world.rot.y) * this->dyna.actor.velocity.z + (yawSinCos * rand));
    yawSinCos = Math_SinS(this->dyna.actor.world.rot.y);
    this->dyna.actor.velocity.z =
        (Math_CosS(this->dyna.actor.world.rot.y) * this->dyna.actor.velocity.z) + (-yawSinCos * rand);
    BgHeavyBlock_SetPieceRandRot(this, scale);
    Actor_SetScale(&this->dyna.actor, Rand_CenteredFloat(0.2f) + 1.0f);
}

void BgHeavyBlock_SetupDynapoly(BgHeavyBlock* this, PlayState* play) {
    s32 pad[2];
    CollisionHeader* colHeader = NULL;
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED | ACTOR_FLAG_PILLAR_PICKUP;
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gHeavyBlockCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void BgHeavyBlock_Init(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    this->pieceFlags = 0;

    if (play->sceneNum == SCENE_OUTSIDE_GANONS_CASTLE) {
        thisx->params &= 0xFF00;
        thisx->params |= 4;
    }

    switch (thisx->params & 0xFF) {
        case HEAVYBLOCK_BIG_PIECE:
            thisx->draw = BgHeavyBlock_DrawPiece;
            this->actionFunc = BgHeavyBlock_MovePiece;
            BgHeavyBlock_InitPiece(this, 1.0f);
            this->timer = 120;
            thisx->flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->unk_164.y = -50.0f;
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            thisx->draw = BgHeavyBlock_DrawPiece;
            this->actionFunc = BgHeavyBlock_MovePiece;
            BgHeavyBlock_InitPiece(this, 2.0f);
            this->timer = 120;
            thisx->flags |= ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->unk_164.y = -20.0f;
            break;
        case HEAVYBLOCK_BREAKABLE:
            BgHeavyBlock_SetupDynapoly(this, play);

            if (Flags_GetSwitch(play, (thisx->params >> 8) & 0x3F)) {
                Actor_Kill(thisx);
                return;
            }

            this->actionFunc = BgHeavyBlock_Wait;
            break;
        case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
            BgHeavyBlock_SetupDynapoly(this, play);

            if (Flags_GetSwitch(play, (thisx->params >> 8) & 0x3F)) {
                this->actionFunc = BgHeavyBlock_DoNothing;
                thisx->shape.rot.x = thisx->world.rot.x = 0x8AD0;
                thisx->shape.rot.y = thisx->world.rot.y = 0xC000;
                thisx->shape.rot.z = thisx->world.rot.z = 0x0;
                thisx->world.pos.x = 1704.0f;
                thisx->world.pos.y = 1504.0f;
                thisx->world.pos.z = 516.0f;
            }

            this->actionFunc = BgHeavyBlock_Wait;
            break;
        case HEAVYBLOCK_UNBREAKABLE:
            BgHeavyBlock_SetupDynapoly(this, play);
            this->actionFunc = BgHeavyBlock_Wait;
            break;
        default:
            BgHeavyBlock_SetupDynapoly(this, play);
            this->actionFunc = BgHeavyBlock_Wait;
            break;
    }
    // "Largest Block Save Bit %x"
    osSyncPrintf(VT_FGCOL(CYAN) " 最大 ブロック セーブビット %x\n" VT_RST, thisx->params);
}

void BgHeavyBlock_Destroy(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;
    switch (this->dyna.actor.params & 0xFF) {
        case HEAVYBLOCK_BIG_PIECE:
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            break;
        default:
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void BgHeavyBlock_MovePiece(BgHeavyBlock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += thisx->gravity;

    if (thisx->velocity.y < thisx->minVelocityY) {
        thisx->velocity.y = thisx->minVelocityY;
    }

    thisx->velocity.x *= 0.98f;
    thisx->velocity.z *= 0.98f;
    func_8002D7EC(thisx);
    thisx->shape.rot.x += thisx->world.rot.x;
    thisx->shape.rot.y += thisx->world.rot.y;
    thisx->shape.rot.z += thisx->world.rot.z;

    if (!(this->pieceFlags & PIECE_FLAG_HIT_FLOOR)) {
        thisx->world.pos.y += this->unk_164.y;
        thisx->prevPos.y += this->unk_164.y;
        Actor_UpdateBgCheckInfo(play, thisx, 50.0f, 50.0f, 0.0f, 5);
        thisx->world.pos.y -= this->unk_164.y;
        thisx->prevPos.y -= this->unk_164.y;
        if (thisx->bgCheckFlags & 1) {
            this->pieceFlags |= PIECE_FLAG_HIT_FLOOR;
            thisx->velocity.y = Rand_ZeroFloat(4.0f) + 2.0f;
            thisx->velocity.x = Rand_CenteredFloat(8.0f);
            thisx->velocity.z = Rand_CenteredFloat(8.0f);
            BgHeavyBlock_SetPieceRandRot(this, 1.0f);
            Audio_PlayActorSound2(thisx, NA_SE_EV_ROCK_BROKEN);
            func_800AA000(thisx->xzDistToPlayer, 0x96, 0xA, 8);
        }
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        Actor_Kill(thisx);
    }
}

void BgHeavyBlock_SpawnDust(PlayState* play, f32 posX, f32 posY, f32 posZ, f32 velX, f32 velY, f32 velZ,
                            u8 dustParams) {
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    Vec3f eye;
    Vec3f at;
    s16 sp6E;
    s16 sp6C;
    Vec3f accel;
    Vec3f velocity;
    Vec3f pos;
    f32 sp44;
    s16 scaleStep;
    s16 scale;

    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;

    if (dustParams & 1) {
        // red dust, landed in fire
        primColor.r = 150;
        primColor.g = primColor.b = envColor.g = envColor.b = 0;
        envColor.r = 80;
        primColor.a = envColor.a = 0;
    } else {
        // brown dust
        // clang-format off
        primColor.r = 170; primColor.g = 130; primColor.b = 90; primColor.a = 255;
        envColor.r = 100; envColor.g = 60; envColor.b = 20; envColor.a = 255;
        // clang-format on
    }

    accel.z = 0.0f;
    accel.x = 0.0f;
    accel.y = (dustParams & 8) ? 0.0f : 0.5f;

    eye = GET_ACTIVE_CAM(play)->eye;
    at = GET_ACTIVE_CAM(play)->at;

    scale = 1000;
    scaleStep = 160;

    switch (dustParams & 6) {
        case 4:
        case 6:
            velocity.x = velX;
            velocity.y = velY;
            velocity.z = velZ;
            scale = 300;
            scaleStep = 50;
            break;
        case 2:
            sp44 = Rand_ZeroFloat(5.0f) + 5.0f;
            sp6E = Rand_CenteredFloat(65280.0f);

            velocity.x = (Math_SinS(sp6E) * sp44) + velX;
            velocity.y = velY;
            velocity.z = (Math_CosS(sp6E) * sp44) + velZ;
            break;
        case 0:
            sp6E = Math_Vec3f_Yaw(&eye, &at);
            sp6C = -Math_Vec3f_Pitch(&eye, &at);

            velocity.x = ((5.0f * Math_SinS(sp6E)) * Math_CosS(sp6C)) + velX;
            velocity.y = (Math_SinS(sp6C) * 5.0f) + velY;
            velocity.z = ((5.0f * Math_CosS(sp6E)) * Math_CosS(sp6C)) + velZ;

            pos.x -= (velocity.x * 20.0f);
            pos.y -= (velocity.y * 20.0f);
            pos.z -= (velocity.z * 20.0f);
            break;
    }

    func_8002843C(play, &pos, &velocity, &accel, &primColor, &envColor, scale, scaleStep,
                  (s32)Rand_ZeroFloat(10.0f) + 20);
}

void BgHeavyBlock_SpawnPieces(BgHeavyBlock* this, PlayState* play) {
    s32 i;
    Vec3f spA4[] = {
        { 0.0f, 300.0f, -20.0f }, { 50.0f, 200.0f, -20.0f }, { -50.0f, 200.0f, -20.0f },
        { 0.0f, 100.0f, 30.0f },  { 0.0f, 100.0f, -70.0f },  { 0.0f, 0.0f, -20.0f },
    };
    s32 pad;
    Vec3f pos;
    f32 sinPitch;
    f32 cosPitch;
    f32 sinYaw;
    f32 cosYaw;

    sinPitch = Math_SinS(this->dyna.actor.world.rot.x);
    cosPitch = Math_CosS(this->dyna.actor.world.rot.x);
    sinYaw = Math_SinS(this->dyna.actor.world.rot.y);
    cosYaw = Math_CosS(this->dyna.actor.world.rot.y);

    for (i = 0; i < ARRAY_COUNT(spA4); i++) {
        pos.z = (spA4[i].y * sinPitch) + (spA4[i].z * cosPitch);

        pos.x = this->dyna.actor.world.pos.x + (spA4[i].x * cosYaw) + (sinYaw * pos.z);
        pos.y = this->dyna.actor.world.pos.y + (spA4[i].y * cosPitch) + (-spA4[i].z * sinPitch);
        pos.z = this->dyna.actor.world.pos.z + (spA4[i].x * -sinYaw) + (cosYaw * pos.z);

        Actor_Spawn(&play->actorCtx, play, ACTOR_BG_HEAVY_BLOCK, pos.x, pos.y, pos.z,
                    this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, 0, 2, true);
        Actor_Spawn(&play->actorCtx, play, ACTOR_BG_HEAVY_BLOCK, pos.x, pos.y, pos.z,
                    this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, 0, 3, true);

        BgHeavyBlock_SpawnDust(play, pos.x, pos.y, pos.z, 0.0f, 0.0f, 0.0f, 0);
    }
}

void BgHeavyBlock_Wait(BgHeavyBlock* this, PlayState* play) {
    s32 quakeIndex;

    // if block has a parent link has lifted it, start one point cutscene and quake
    if (Actor_HasParent(&this->dyna.actor, play)) {
        this->timer = 0;

        if (!CVarGetInteger("gFasterHeavyBlockLift", 0)) {
            switch (this->dyna.actor.params & 0xFF) {
                case HEAVYBLOCK_BREAKABLE:
                    OnePointCutscene_Init(play, 4020, 270, &this->dyna.actor, MAIN_CAM);
                    break;
                case HEAVYBLOCK_UNBREAKABLE:
                    OnePointCutscene_Init(play, 4021, 220, &this->dyna.actor, MAIN_CAM);
                    break;
                case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                    OnePointCutscene_Init(play, 4022, 210, &this->dyna.actor, MAIN_CAM);
                    break;
            }
        }

        quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
        Quake_SetSpeed(quakeIndex, 25000);
        Quake_SetQuakeValues(quakeIndex, 1, 1, 5, 0);
        Quake_SetCountdown(quakeIndex, 10);
        this->actionFunc = BgHeavyBlock_LiftedUp;
    }
}

void BgHeavyBlock_LiftedUp(BgHeavyBlock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 cosYaw;
    f32 zOffset;
    f32 sinYaw;
    f32 xOffset;

    if (this->timer == 11) {
        func_800AA000(0.0f, 0xFF, 0x14, 0x14);
        func_8002F7DC(&player->actor, NA_SE_PL_PULL_UP_BIGROCK);
        LOG_STRING("NA_SE_PL_PULL_UP_BIGROCK");
    }

    if (this->timer < 40) {
        xOffset = Rand_CenteredFloat(110.0f);
        sinYaw = Math_SinS(this->dyna.actor.shape.rot.y);
        zOffset = Rand_CenteredFloat(110.0f);
        cosYaw = Math_CosS(this->dyna.actor.shape.rot.y);

        BgHeavyBlock_SpawnDust(play, (sinYaw * -70.0f) + (this->dyna.actor.world.pos.x + xOffset),
                               this->dyna.actor.world.pos.y + 10.0f,
                               (cosYaw * -70.0f) + (this->dyna.actor.world.pos.z + zOffset), 0.0f, -1.0f, 0.0f, 0xC);
    }

    this->timer++;

    if (!CVarGetInteger("gFasterHeavyBlockLift", 0)) {
        func_8002DF54(play, &player->actor, 8);
    }

    // if parent is NULL, link threw it
    if (Actor_HasNoParent(&this->dyna.actor, play)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_HEAVY_THROW);
        this->actionFunc = BgHeavyBlock_Fly;
    }
}

void BgHeavyBlock_Fly(BgHeavyBlock* this, PlayState* play) {
    s32 bgId;
    s32 quakeIndex;
    Vec3f pos;
    f32 raycastResult;

    Actor_MoveForward(&this->dyna.actor);
    pos.x = this->dyna.actor.home.pos.x;
    pos.y = this->dyna.actor.home.pos.y + 1000.0f;
    pos.z = this->dyna.actor.home.pos.z;
    raycastResult =
        BgCheck_EntityRaycastFloor4(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &pos);
    this->dyna.actor.floorHeight = raycastResult;

    if (this->dyna.actor.home.pos.y <= raycastResult) {
        func_800AA000(0.0f, 0xFF, 0x3C, 4);

        switch (this->dyna.actor.params & 0xFF) {
            case HEAVYBLOCK_BREAKABLE:
                BgHeavyBlock_SpawnPieces(this, play);
                Flags_SetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
                Actor_Kill(&this->dyna.actor);

                quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
                Quake_SetSpeed(quakeIndex, 28000);
                Quake_SetQuakeValues(quakeIndex, 14, 2, 100, 0);
                Quake_SetCountdown(quakeIndex, 30);

                // We don't want this arbitrarily long quake with the enhancement enabled
                if (!CVarGetInteger("gFasterHeavyBlockLift", 0)) {
                    quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 2);
                    Quake_SetSpeed(quakeIndex, 12000);
                    Quake_SetQuakeValues(quakeIndex, 5, 0, 0, 0);
                    Quake_SetCountdown(quakeIndex, 999);
                }

                SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 30,
                                                   NA_SE_EV_ELECTRIC_EXPLOSION);
                return;
            case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_STONE_BOUND);

                quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
                Quake_SetSpeed(quakeIndex, 28000);
                Quake_SetQuakeValues(quakeIndex, 16, 2, 120, 0);
                Quake_SetCountdown(quakeIndex, 40);

                this->actionFunc = BgHeavyBlock_Land;
                Flags_SetSwitch(play, (this->dyna.actor.params >> 8) & 0x3F);
                break;
            case HEAVYBLOCK_UNBREAKABLE:
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BUYOSTAND_STOP_U);

                quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
                Quake_SetSpeed(quakeIndex, 28000);
                Quake_SetQuakeValues(quakeIndex, 14, 2, 100, 0);
                Quake_SetCountdown(quakeIndex, 40);

                this->actionFunc = BgHeavyBlock_Land;
                break;
            default:
                quakeIndex = Quake_Add(GET_ACTIVE_CAM(play), 3);
                Quake_SetSpeed(quakeIndex, 28000);
                Quake_SetQuakeValues(quakeIndex, 14, 2, 100, 0);
                Quake_SetCountdown(quakeIndex, 40);

                this->actionFunc = BgHeavyBlock_Land;
        }
    }
    this->dyna.actor.shape.rot.x = Math_Atan2S(this->dyna.actor.velocity.y, this->dyna.actor.speedXZ);
}

void BgHeavyBlock_DoNothing(BgHeavyBlock* this, PlayState* play) {
}

void BgHeavyBlock_Land(BgHeavyBlock* this, PlayState* play) {
    s32 pad;

    if (Math_SmoothStepToS(&this->dyna.actor.shape.rot.x, 0x8AD0, 6, 2000, 100) != 0) {
        Math_StepToF(&this->dyna.actor.speedXZ, 0.0f, 20.0f);
        Math_StepToF(&this->dyna.actor.velocity.y, 0.0f, 3.0f);
        this->dyna.actor.gravity = 0.0f;
        this->dyna.actor.world.pos = this->dyna.actor.home.pos;
        Actor_MoveForward(&this->dyna.actor);
        this->dyna.actor.home.pos = this->dyna.actor.world.pos;
        switch (this->dyna.actor.params & 0xFF) {
            case HEAVYBLOCK_UNBREAKABLE_OUTSIDE_CASTLE:
                BgHeavyBlock_SpawnDust(play, Rand_CenteredFloat(30.0f) + 1678.0f, Rand_ZeroFloat(100.0f) + 1286.0f,
                                       Rand_CenteredFloat(30.0f) + 552.0f, 0.0f, 0.0f, 0.0f, 0);
                BgHeavyBlock_SpawnDust(play, Rand_CenteredFloat(30.0f) + 1729.0f, Rand_ZeroFloat(80.0f) + 1269.0f,
                                       Rand_CenteredFloat(30.0f) + 600.0f, 0.0f, 0.0f, 0.0f, 0);
                break;
            case HEAVYBLOCK_UNBREAKABLE:
                BgHeavyBlock_SpawnDust(play, Rand_CenteredFloat(100.0f) + -735.0f, 29.0f,
                                       Rand_CenteredFloat(100.0f) + -3418.0f, 0.0f, 0.0f, 0.0f, 3);
                break;
        }
    } else {
        this->dyna.actor.flags &= ~(ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED);
        this->actionFunc = BgHeavyBlock_DoNothing;
    }
}

void BgHeavyBlock_Update(Actor* thisx, PlayState* play) {
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;

    this->actionFunc(this, play);
}

void BgHeavyBlock_Draw(Actor* thisx, PlayState* play) {
    static Vec3f D_80884EC8 = { 0.0f, 0.0f, 0.0f };
    static Vec3f D_80884ED4 = { 0.0f, 400.0f, 0.0f };
    BgHeavyBlock* this = (BgHeavyBlock*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx);

    if (BgHeavyBlock_LiftedUp == this->actionFunc) {
        Matrix_SetTranslateRotateYXZ(player->leftHandPos.x, player->leftHandPos.y, player->leftHandPos.z,
                                     &thisx->shape.rot);
        Matrix_Translate(-this->unk_164.x, -this->unk_164.y, -this->unk_164.z, MTXMODE_APPLY);
    } else if ((thisx->gravity == 0.0f) && (BgHeavyBlock_Land == this->actionFunc)) {
        Matrix_SetTranslateRotateYXZ(thisx->home.pos.x, thisx->home.pos.y, thisx->home.pos.z, &thisx->shape.rot);
        Matrix_Translate(-D_80884ED4.x, -D_80884ED4.y, -D_80884ED4.z, MTXMODE_APPLY);
    }

    Matrix_MultVec3f(&D_80884EC8, &thisx->world.pos);
    Matrix_MultVec3f(&D_80884ED4, &thisx->home.pos);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gHeavyBlockEntirePillarDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgHeavyBlock_DrawPiece(Actor* thisx, PlayState* play) {
    switch (thisx->params & 0xFF) {
        case HEAVYBLOCK_BIG_PIECE:
            Matrix_Translate(50.0f, -260.0f, -20.0f, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, gHeavyBlockBigPieceDL);
            break;
        case HEAVYBLOCK_SMALL_PIECE:
            Matrix_Translate(45.0f, -280.0f, -5.0f, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, gHeavyBlockSmallPieceDL);
            break;
    }
}

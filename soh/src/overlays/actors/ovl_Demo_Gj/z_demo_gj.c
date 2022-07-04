/*
 * File: z_demo_gj.c
 * Overlay: Demo_Gj
 * Description: Ganon battle rubble.
 */

#include "z_demo_gj.h"
#include "objects/object_gj/object_gj.h"
#include "objects/object_geff/object_geff.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoGj_Init(Actor* thisx, GlobalContext* globalCtx);
void DemoGj_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DemoGj_Update(Actor* thisx, GlobalContext* globalCtx);
void DemoGj_Draw(Actor* thisx, GlobalContext* globalCtx);

static ColliderCylinderInitType1 sCylinderInit1 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 30, 100, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit2 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 25, 110, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 sCylinderInit3 = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 25, 200, 0, { 0, 0, 0 } },
};

typedef void (*DemoGjUpdateFunc)(DemoGj*, GlobalContext*);
typedef void (*DemoGjDrawFunc)(DemoGj*, GlobalContext*);

// bits 11-15
s32 DemoGj_GetCollectibleType(DemoGj* this) {
    s32 params = this->dyna.actor.params >> 0xB;

    return params & 0x1F;
}

// bits 8-10
s32 DemoGj_GetCollectibleAmount(DemoGj* this) {
    s32 params = this->dyna.actor.params >> 0x8;

    return params & 7;
}

// bits 0-7
s32 DemoGj_GetType(DemoGj* this) {
    s32 params = this->dyna.actor.params;

    return params & 0xFF;
}

void DemoGj_InitCylinder(DemoGj* this, GlobalContext* globalCtx, ColliderCylinder* cylinder,
                         ColliderCylinderInitType1* cylinderInit) {
    Collider_InitCylinder(globalCtx, cylinder);
    Collider_SetCylinderType1(globalCtx, cylinder, &this->dyna.actor, cylinderInit);
}

s32 DemoGj_HitByExplosion(DemoGj* this, GlobalContext* globalCtx, ColliderCylinder* cylinder) {
    if (Actor_GetCollidedExplosive(globalCtx, &cylinder->base) != NULL) {
        return true;
    }
    return false;
}

void DemoGj_DestroyCylinder(DemoGj* this, GlobalContext* globalCtx) {
    switch (DemoGj_GetType(this)) {
        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1:
            Collider_DestroyCylinder(globalCtx, &this->cylinders[0]);
            Collider_DestroyCylinder(globalCtx, &this->cylinders[1]);
            Collider_DestroyCylinder(globalCtx, &this->cylinders[2]);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2:
            Collider_DestroyCylinder(globalCtx, &this->cylinders[0]);
            Collider_DestroyCylinder(globalCtx, &this->cylinders[1]);
            Collider_DestroyCylinder(globalCtx, &this->cylinders[2]);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL:
            Collider_DestroyCylinder(globalCtx, &this->cylinders[0]);
            break;
    }
}

void DemoGj_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DemoGj* this = (DemoGj*)thisx;

    DemoGj_DestroyCylinder(this, globalCtx);
    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void DemoGj_PlayExplosionSfx(DemoGj* this, GlobalContext* globalCtx) {
    SoundSource_PlaySfxAtFixedWorldPos(globalCtx, &this->dyna.actor.world.pos, 50, NA_SE_EV_GRAVE_EXPLOSION);
}

void DemoGj_SpawnSmoke(GlobalContext* globalCtx, Vec3f* pos, f32 arg2) {
    static Vec3f velocity = { 0.0f, 6.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 primColor = { 0, 0, 0, 0 };
    static Color_RGBA8 envColor = { 0, 0, 0, 0 };
    f32 temp = arg2 * 0.2f;

    func_800283D4(globalCtx, pos, &velocity, &accel, &primColor, &envColor, temp * Rand_ZeroOne() + arg2, 15, 90);
}

void DemoGj_DropCollectible(DemoGj* this, GlobalContext* globalCtx) {
    Vec3f* pos = &this->dyna.actor.world.pos;
    s16 collectible = DemoGj_GetCollectibleType(this);
    s32 amount = DemoGj_GetCollectibleAmount(this);
    s32 i;

    for (i = 0; i < amount; i++) {
        Item_DropCollectible(globalCtx, pos, collectible);
    }
}

void DemoGj_Explode(DemoGj* this, GlobalContext* globalCtx, Vec3f* initialPos, Vec3f* direction) {
    Vec3f explosionPos;
    Vec3f velocity;
    s32 phi_s0;
    f32 aux;
    s16 theta = 0;
    s32 i;

    for (i = 0; i < 6; i++) {
        explosionPos.x = Math_SinS(theta) * 16.0f;
        explosionPos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        explosionPos.z = Math_CosS(theta) * 16.0f;

        velocity.x = (explosionPos.x * 0.6f) + (12.0f * direction->x);
        velocity.y = (Rand_ZeroOne() * 36.0f) + 6.0f;
        velocity.z = (explosionPos.z * 0.6f) + (12.0f * direction->z);

        explosionPos.x += initialPos->x;
        explosionPos.y += initialPos->y;
        explosionPos.z += initialPos->z;

        aux = Rand_ZeroOne();
        if (aux < 0.1f) {
            phi_s0 = 0x61;
        } else if (aux < 0.7f) {
            phi_s0 = 0x41;
        } else {
            phi_s0 = 0x21;
        }

        Gfx* gfx = ResourceMgr_LoadGfxByName(gGanonRubbleDL);

        EffectSsKakera_Spawn(globalCtx, &explosionPos, &velocity, initialPos, -200, phi_s0, 10, 10, 0,
                             Rand_ZeroOne() * 20.0f + 20.0f, 20, 300, (s32)(Rand_ZeroOne() * 30.0f) + 30, -1,
                             OBJECT_GEFF, gfx);

        theta += 0x2AAA;
    }

    DemoGj_PlayExplosionSfx(this, globalCtx);
}

s32 DemoGj_IsSceneInvalid(void) {
    if (gSaveContext.sceneSetupIndex < 4) {
        return false;
    }
    return true;
}

s32 DemoGj_FindGanon(DemoGj* this, GlobalContext* globalCtx) {
    Actor* actor;

    if (this->ganon == NULL) {
        actor = globalCtx->actorCtx.actorLists[ACTORCAT_BOSS].head;

        while (actor != NULL) {
            if (actor->id == ACTOR_BOSS_GANON2) {
                this->ganon = (BossGanon2*)actor;

                // "Demo_Gj_Search_Boss_Ganon %d: Discover Ganon !!!!"
                osSyncPrintf("Demo_Gj_Search_Boss_Ganon %d:ガノン発見!!!!\n", this->dyna.actor.params);
                return true;
            }
            actor = actor->next;
        }

        // "Demo_Gj_Search_Boss_Ganon %d: I couldn't find Ganon"
        osSyncPrintf("Demo_Gj_Search_Boss_Ganon %d:ガノン発見出来ず\n", this->dyna.actor.params);
        return false;
    }
    //! @bug: Missing return value when `this->ganon` is already set.
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void DemoGj_InitCommon(DemoGj* this, GlobalContext* globalCtx, CollisionHeader* header) {
    s32 pad[3];
    CollisionHeader* newHeader;

    if (header != NULL) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        newHeader = NULL;
        CollisionHeader_GetVirtual(header, &newHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, newHeader);
    }
}

// TODO: find a better name
s32 DemoGj_InitSetIndexes(DemoGj* this, GlobalContext* globalCtx, s32 updateMode, s32 drawConfig,
                          CollisionHeader* header) {
    if (!DemoGj_IsSceneInvalid()) {
        this->updateMode = updateMode;
        this->drawConfig = drawConfig;
        DemoGj_InitCommon(this, globalCtx, header);
        return true;
    }
    Actor_Kill(&this->dyna.actor);
    return false;
}

void DemoGj_DrawCommon(DemoGj* this, GlobalContext* globalCtx, Gfx* displayList) {
    if (kREG(0) == 0) {
        GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;

        OPEN_DISPS(gfxCtx);

        func_80093D18(gfxCtx);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                  G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, displayList);
        gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

        CLOSE_DISPS(gfxCtx);
    }
}

void DemoGj_DrawRotated(DemoGj* this, GlobalContext* globalCtx, Gfx* displayList) {
    s32 pad;
    GraphicsContext* gfxCtx;
    s16 x = this->rotationVec.x;
    s16 y = this->rotationVec.y;
    s16 z = this->rotationVec.z;
    s32 pad2;
    Mtx* matrix;

    gfxCtx = globalCtx->state.gfxCtx;
    matrix = Graph_Alloc(gfxCtx, sizeof(Mtx));

    OPEN_DISPS(gfxCtx);

    Matrix_Push();
    Matrix_RotateZYX(x, y, z, MTXMODE_APPLY);
    MATRIX_TOMTX(matrix);
    Matrix_Pop();

    func_80093D18(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, matrix, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, displayList);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void DemoGj_SetupRotation(DemoGj* this, GlobalContext* globalCtx) {
    f32 yPosition = this->dyna.actor.world.pos.y;
    f32* yVelocity = &this->dyna.actor.velocity.y;
    f32* speedXZ = &this->dyna.actor.speedXZ;
    Vec3s* unk_172 = &this->unk_172;
    f32 verticalTranslation;
    Vec3f vec;
    f32 verticalFactor;
    f32 xzPlaneFactor;

    switch (DemoGj_GetType(this)) {
        case DEMOGJ_TYPE_RUBBLE_PILE_1:
            verticalTranslation = kREG(23);
            vec.x = kREG(24) * 0.01f + 1.0f;
            vec.y = kREG(25) * 0.01f + 1.0f;
            vec.z = kREG(26) * 0.01f + 1.0f;
            verticalFactor = kREG(27) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(28) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_2:
            verticalTranslation = kREG(36);
            vec.x = kREG(37) * 0.01f + 1.0f;
            vec.y = kREG(38) * 0.01f + 1.0f;
            vec.z = kREG(39) * 0.01f + 1.0f;
            verticalFactor = kREG(40) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(41) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_3:
            verticalTranslation = kREG(49);
            vec.x = kREG(50) * 0.01f + 1.0f;
            vec.y = kREG(51) * 0.01f + 1.0f;
            vec.z = kREG(52) * 0.01f + 1.0f;
            verticalFactor = kREG(53) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(54) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_4:
            verticalTranslation = kREG(62);
            vec.x = kREG(63) * 0.01f + 1.0f;
            vec.y = kREG(64) * 0.01f + 1.0f;
            vec.z = kREG(65) * 0.01f + 1.0f;
            verticalFactor = kREG(66) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(67) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_5:
            verticalTranslation = kREG(75);
            vec.x = kREG(76) * 0.01f + 1.0f;
            vec.y = kREG(77) * 0.01f + 1.0f;
            vec.z = kREG(78) * 0.01f + 1.0f;
            verticalFactor = kREG(79) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(80) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_6:
            verticalTranslation = kREG(88);
            vec.x = kREG(89) * 0.01f + 1.0f;
            vec.y = kREG(90) * 0.01f + 1.0f;
            vec.z = kREG(91) * 0.01f + 1.0f;
            verticalFactor = kREG(92) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(93) * 0.01f + 1.0f;
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_7:
            verticalTranslation = kREG(10) + -190.0f;
            vec.x = kREG(9) * 0.01f + 1.0f;
            vec.y = kREG(8) * 0.01f + 1.0f;
            vec.z = kREG(7) * 0.01f + 1.0f;
            verticalFactor = kREG(6) * 0.01f + -1.0f;
            xzPlaneFactor = kREG(5) * 0.01f + 1.0f;
            break;

        default:
            // "Demo_Gj_common_Reflect : This arg_data is not supported = %d"
            osSyncPrintf(VT_FGCOL(RED) "Demo_Gj_common_Reflect : そんなarg_dataには対応していない = %d\n" VT_RST,
                         this->dyna.actor.params);
            return;
    }

    yPosition += verticalTranslation;
    if (yPosition <= 1086.0f && (*yVelocity < 0.0f)) {
        if (!this->isRotated) {
            *yVelocity *= verticalFactor;
            *speedXZ *= xzPlaneFactor;

            unk_172->x *= vec.x;
            unk_172->y *= vec.y;
            unk_172->z *= vec.z;

            if (*yVelocity <= -this->dyna.actor.gravity) {
                *yVelocity = 0.0f;
                *speedXZ = 0.0f;

                unk_172->x = 0;
                unk_172->y = 0;
                unk_172->z = 0;
            }

            this->isRotated = true;
        }
    }
}

/*
 * Returns true if `ganon->unk_314` is equals to `arg1`.
 * `ganon->unk_314` can have the following values:
 *  0: Before the battle has started.
 *  1: When is set: Ganondorf starts rising from the rubble.
 *     What is happening: Ganondorf is moving vertically and has vertical velocity.
 *     Proposed name: BOSSGANON2_MODE_GANONDORF_RISING
 *  2: When is set: Ganondorf has stopped rising in air.
 *     What is happening: The camera is in front of him, focusing the clouds and going down to focus him.
 *     Proposed name: BOSSGANON2_MODE_GANONDORF_FLOATING
 *  3: When is set: The camera has stopped moving and is focusing him.
 *     What is happening: Ganondorf raises his hand, shows the triforce and transforms into Ganon. The battle starts.
 *     This value is set during the whole real fight against Ganon. Without and with Master Sword.
 *     Proposed name: BOSSGANON2_MODE_GANON_FIGHTING
 *  4: When is set: Link has hit Ganon's tail for last time with Master Sword.
 *     What is happening: Ganon falls to the floor, Zelda uses her magic and tells Link to kill him.
 *     Proposed name: BOSSGANON2_MODE_GANON_DEFEATED
 *
 * Those values should probably be defined as macros or enums in `ovl_Boss_Ganon2/z_boss_ganon2.h`.
 * Proposed name for the function: `s32 DemoGj_CheckGanonMode(DemoGj* this, u8 mode)`
 */
s32 func_809797E4(DemoGj* this, u8 arg1) {
    BossGanon2* ganon = this->ganon;

    if ((ganon != NULL) && (ganon->unk_314 == arg1)) {
        return true;
    }
    return false;
}

s32 DemoGj_IsGanondorfRisingFromRubble(DemoGj* this, GlobalContext* globalCtx) {
    return func_809797E4(this, 1);
}

// Ganondorf has stopped rising into the air and is just floating. Just before he transforms.
s32 DemoGj_IsGanondorfFloatingInAir(DemoGj* this, GlobalContext* globalCtx) {
    return func_809797E4(this, 2);
}

void DemoGj_SetupMovement(DemoGj* this, GlobalContext* globalCtx) {
    Actor* actor = &this->dyna.actor;
    Player* player;
    Vec3f* pos = &actor->world.pos;
    Vec3s* unk_172;
    f32 xDistance;
    f32 zDistance;

    if (this->ganon != NULL) {
        xDistance = actor->world.pos.x - this->ganon->actor.world.pos.x;
        zDistance = actor->world.pos.z - this->ganon->actor.world.pos.z;
        unk_172 = &this->unk_172;

        switch (DemoGj_GetType(this)) {
            case DEMOGJ_TYPE_RUBBLE_PILE_1:
                actor->speedXZ = kREG(16) + 10.0f;
                actor->velocity.y = kREG(17) + 40.0f;
                unk_172->x = kREG(18);
                unk_172->y = kREG(19) + 0x3E8;
                unk_172->z = kREG(20) + 0xBB8;
                actor->minVelocityY = kREG(21) * 0.01f + -29.0f;
                actor->gravity = kREG(22) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_2:
                actor->speedXZ = kREG(29) + 10.0f;
                actor->velocity.y = kREG(30) + 40.0f;
                unk_172->x = kREG(31);
                unk_172->y = kREG(32) + 0x3E8;
                unk_172->z = kREG(33) + 0xBB8;
                actor->minVelocityY = kREG(34) * 0.01f + -29.0f;
                actor->gravity = kREG(35) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_3:
                actor->speedXZ = kREG(42) + 10.0f;
                actor->velocity.y = kREG(43) + 40.0f;
                unk_172->x = kREG(44);
                unk_172->y = kREG(45) + 0x3E8;
                unk_172->z = kREG(46) + 0xBB8;
                actor->minVelocityY = kREG(47) * 0.01f + -29.0f;
                actor->gravity = kREG(48) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_4:
                actor->speedXZ = kREG(55) + 10.0f;
                actor->velocity.y = kREG(56) + 40.0f;
                unk_172->x = kREG(57);
                unk_172->y = kREG(58) + 0x3E8;
                unk_172->z = kREG(59) + 0xBB8;
                actor->minVelocityY = kREG(60) * 0.01f + -29.0f;
                actor->gravity = kREG(61) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_5:
                actor->speedXZ = kREG(68) + 10.0f;
                actor->velocity.y = kREG(69) + 40.0f;
                unk_172->x = kREG(70);
                unk_172->y = kREG(71) + 0x3E8;
                unk_172->z = kREG(72) + 0xBB8;
                actor->minVelocityY = kREG(73) * 0.01f + -29.0f;
                actor->gravity = kREG(74) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_6:
                actor->speedXZ = kREG(81) + 10.0f;
                actor->velocity.y = kREG(82) + 40.0f;
                unk_172->x = kREG(83);
                unk_172->y = kREG(84) + 0x3E8;
                unk_172->z = kREG(85) + 0xBB8;
                actor->minVelocityY = kREG(86) * 0.01f + -29.0f;
                actor->gravity = kREG(87) * 0.01f + -5.0f;
                break;

            case DEMOGJ_TYPE_RUBBLE_PILE_7:
                actor->speedXZ = kREG(94) + 10.0f;
                actor->velocity.y = kREG(95) + 70.0f;
                unk_172->x = kREG(15);
                unk_172->y = kREG(14) + 0x3E8;
                unk_172->z = kREG(13) + 0xBB8;
                actor->minVelocityY = kREG(12) * 0.01f + -29.0f;
                actor->gravity = kREG(11) * 0.01f + -5.0f;
                break;

            default:
                // "Demo_Gj_Setup_Move_common : This arg_data is not supported = %d"
                osSyncPrintf(VT_FGCOL(RED) "Demo_Gj_Setup_Move_common : そんなarg_dataには対応していない = %d\n" VT_RST,
                             actor->params);
                break;
        }

        if (xDistance == 0.0f && zDistance == 0.0f) {
            player = GET_PLAYER(globalCtx);
            xDistance = player->actor.world.pos.x - pos->x;
            zDistance = player->actor.world.pos.z - pos->z;

            if (xDistance != 0.0f || zDistance != 0.0f) {
                actor->world.rot.y = (Math_FAtan2F(xDistance, zDistance) * (0x8000 / M_PI));
            }
        } else {
            actor->world.rot.y = (Math_FAtan2F(xDistance, zDistance) * (0x8000 / M_PI));
        }
    }
}

void DemoGj_CheckIfTransformedIntoGanon(DemoGj* this) {
    if (func_809797E4(this, 3)) {
        this->isTransformedIntoGanon = true;
    }
}

void DemoGj_InitRubblePile1(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 1, 2, &gGanonsCastleRubble2Col);
}

void func_8097A000(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(18));
    this->rotationVec.y += (s16)(kREG(19) + 1000);
    this->rotationVec.z += (s16)(kREG(20) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void DemoGj_SpawnSmokePreBattle1(DemoGj* this, GlobalContext* globalCtx) {
    static Vec3f pos = { -371.0f, 1188.0f, -303.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = globalCtx->gameplayFrames % 3;

        if (gameplayFrames == 0) {
            if (!globalCtx->gameplayFrames) {}
            DemoGj_SpawnSmoke(globalCtx, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097A0E4(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 8;
        this->drawConfig = 9;
    }
}

void func_8097A130(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A160
void DemoGj_Update01(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097A0E4(this, globalCtx);
}

// func_8097A190
void DemoGj_Update08(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A000(this, globalCtx);
    func_8097A130(this, globalCtx);
}

void DemoGj_DrawRubble2(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble2DL);
}

void DemoGj_DrawRotatedRubble2(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble2DL);
}

void DemoGj_InitRubblePile2(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 2, 3, &gGanonsCastleRubble3Col);
}

void func_8097A238(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(31));
    this->rotationVec.y += (s16)(kREG(32) + 1000);
    this->rotationVec.z += (s16)(kREG(33) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void DemoGj_SpawnSmokePreBattle2(DemoGj* this, GlobalContext* globalCtx) {
    static Vec3f pos = { -119.0f, 1056.0f, -147.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = globalCtx->gameplayFrames % 3;

        if (gameplayFrames == 1) {
            if (!globalCtx->gameplayFrames) {}
            DemoGj_SpawnSmoke(globalCtx, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097A320(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 9;
        this->drawConfig = 10;
    }
}

void func_8097A36C(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A39C
void DemoGj_Update02(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097A320(this, globalCtx);
}

// func_8097A3CC
void DemoGj_Update09(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A238(this, globalCtx);
    func_8097A36C(this, globalCtx);
}

void DemoGj_DrawRubble3(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble3DL);
}

void DemoGj_DrawRotatedRubble3(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble3DL);
}

void DemoGj_InitRubblePile3(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 3, 4, &gGanonsCastleRubble4Col);
}

void func_8097A474(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(44));
    this->rotationVec.y += (s16)(kREG(45) + 1000);
    this->rotationVec.z += (s16)(kREG(46) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void func_8097A4F0(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 10;
        this->drawConfig = 11;
    }
}

void func_8097A53C(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A56C
void DemoGj_Update03(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097A4F0(this, globalCtx);
}

// func_8097A59C
void DemoGj_Update10(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A474(this, globalCtx);
    func_8097A53C(this, globalCtx);
}

void DemoGj_DrawRubble4(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble4DL);
}

void DemoGj_DrawRotatedRubble4(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble4DL);
}

void DemoGj_InitRubblePile4(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 4, 5, &gGanonsCastleRubble5Col);
}

void func_8097A644(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(57));
    this->rotationVec.y += (s16)(kREG(58) + 1000);
    this->rotationVec.z += (s16)(kREG(59) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void func_8097A6C0(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 11;
        this->drawConfig = 12;
    }
}

void func_8097A70C(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A73C
void DemoGj_Update04(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097A6C0(this, globalCtx);
}

// func_8097A76C
void DemoGj_Update11(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A644(this, globalCtx);
    func_8097A70C(this, globalCtx);
}

void DemoGj_DrawRubble5(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble5DL);
}

void DemoGj_DrawRotatedRubble5(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble5DL);
}

void DemoGj_InitRubblePile5(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 5, 6, &gGanonsCastleRubble6Col);
}

void func_8097A814(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(70));
    this->rotationVec.y += (s16)(kREG(71) + 1000);
    this->rotationVec.z += (s16)(kREG(72) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void func_8097A890(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 12;
        this->drawConfig = 13;
    }
}

void func_8097A8DC(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097A90C
void DemoGj_Update05(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097A890(this, globalCtx);
}

// func_8097A93C
void DemoGj_Update12(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A814(this, globalCtx);
    func_8097A8DC(this, globalCtx);
}

void DemoGj_DrawRubble6(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble6DL);
}

void DemoGj_DrawRotatedRubble6(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble6DL);
}

void DemoGj_InitRubblePile6(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 6, 7, &gGanonsCastleRubble7Col);
}

void func_8097A9E4(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(83));
    this->rotationVec.y += (s16)(kREG(84) + 1000);
    this->rotationVec.z += (s16)(kREG(85) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void func_8097AA60(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 13;
        this->drawConfig = 14;
    }
}

void func_8097AAAC(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097AADC
void DemoGj_Update06(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097AA60(this, globalCtx);
}

// func_8097AB0C
void DemoGj_Update13(DemoGj* this, GlobalContext* globalCtx) {
    func_8097A9E4(this, globalCtx);
    func_8097AAAC(this, globalCtx);
}

void DemoGj_DrawRubble7(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble7DL);
}

void DemoGj_DrawRotatedRubble7(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubble7DL);
}

void DemoGj_InitRubblePile7(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 7, 8, &gGanonsCastleRubbleTallCol);
}

void func_8097ABB4(DemoGj* this, GlobalContext* globalCtx) {
    Actor_MoveForward(&this->dyna.actor);

    this->rotationVec.x += (s16)(kREG(15));
    this->rotationVec.y += (s16)(kREG(14) + 1000);
    this->rotationVec.z += (s16)(kREG(13) + 3000);

    DemoGj_SetupRotation(this, globalCtx);
}

void DemoGj_SpawnSmokePreBattle3(DemoGj* this, GlobalContext* globalCtx) {
    static Vec3f pos = { -6.0f, 1053.0f, -473.0f };
    u32 gameplayFrames;

    if (!this->isTransformedIntoGanon) {
        gameplayFrames = globalCtx->gameplayFrames % 3;

        if (gameplayFrames == 2) {
            if (!globalCtx->gameplayFrames) {}
            DemoGj_SpawnSmoke(globalCtx, &pos, 300.0f);
        }

        DemoGj_CheckIfTransformedIntoGanon(this);
    }
}

void func_8097AC9C(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfRisingFromRubble(this, globalCtx)) {
        DemoGj_SetupMovement(this, globalCtx);
        this->updateMode = 14;
        this->drawConfig = 15;
    }
}

void func_8097ACE8(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Actor_Kill(&this->dyna.actor);
    }
}

// func_8097AD18
void DemoGj_Update07(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097AC9C(this, globalCtx);
}

// func_8097AD48
void DemoGj_Update14(DemoGj* this, GlobalContext* globalCtx) {
    func_8097ABB4(this, globalCtx);
    func_8097ACE8(this, globalCtx);
}

void DemoGj_DrawRubbleTall(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubbleTallDL);
}

void DemoGj_DrawRotatedRubbleTall(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawRotated(this, globalCtx, gGanonsCastleRubbleTallDL);
}

void DemoGj_InitRubbleAroundArena(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 0, 1, &gGanonsCastleRubbleAroundArenaCol);
}

// func_8097ADF0
void DemoGj_UpdateRubbleAroundArena(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    DemoGj_SpawnSmokePreBattle1(this, globalCtx);
    DemoGj_SpawnSmokePreBattle2(this, globalCtx);
    DemoGj_SpawnSmokePreBattle3(this, globalCtx);
}

void DemoGj_DrawRubbleAroundArena(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubbleAroundArenaDL);
}

// Inits the three cylinders with `sCylinderInit1`
void DemoGj_InitDestructableRubble1(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 15, 0, NULL);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[0], &sCylinderInit1);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[1], &sCylinderInit1);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[2], &sCylinderInit1);
}

void DemoGj_DoNothing1(DemoGj* this, GlobalContext* globalCtx) {
}

/*
 * Moves the ColliderCylinder's relative to the actor's position.
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1
 */
void func_8097AEE8(DemoGj* this, GlobalContext* globalCtx) {
    ColliderCylinder* cylinder0 = &this->cylinders[0];
    ColliderCylinder* cylinder1 = &this->cylinders[1];
    ColliderCylinder* cylinder2 = &this->cylinders[2];
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 pad;
    s16 theta = this->dyna.actor.world.rot.y;
    f32 cos_theta = Math_CosS(theta);
    f32 sin_theta = Math_SinS(theta);

    cylinder0->dim.pos.z = actorPos->z + (20.0f * cos_theta) - (-20.0f * sin_theta);
    cylinder0->dim.pos.x = actorPos->x + (20.0f * sin_theta) + (-20.0f * cos_theta);
    cylinder0->dim.pos.y = actorPos->y;

    cylinder1->dim.pos.z = actorPos->z + (-20.0f * cos_theta) - (20.0f * sin_theta);
    cylinder1->dim.pos.x = actorPos->x + (-20.0f * sin_theta) + (20.0f * cos_theta);
    cylinder1->dim.pos.y = actorPos->y;

    cylinder2->dim.pos.z = actorPos->z + (-60.0f * cos_theta) - (60.0f * sin_theta);
    cylinder2->dim.pos.x = actorPos->x + (-60.0f * sin_theta) + (60.0f * cos_theta);
    cylinder2->dim.pos.y = actorPos->y;
}

void DemoGj_SetCylindersAsAC(DemoGj* this, GlobalContext* globalCtx) {
    s32 pad[2];
    Collider* cylinder0 = &this->cylinders[0].base;
    Collider* cylinder1 = &this->cylinders[1].base;
    Collider* cylinder2 = &this->cylinders[2].base;
    s32 pad2[3];

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder0);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder1);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder2);
}

void DemoGj_DirectedExplosion(DemoGj* this, GlobalContext* globalCtx, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;

    DemoGj_Explode(this, globalCtx, &pos, direction);
}

void func_8097B128(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, globalCtx, &gGanonsCastleRubble2Col);
        this->updateMode = 18;
        this->drawConfig = 16;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

s32 DemoGj_HasCylinderAnyExploded(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[0])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[1])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[2])) {
        return true;
    }
    return false;
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1
 */
void func_8097B22C(DemoGj* this, GlobalContext* globalCtx) {
    Actor* thisx = &this->dyna.actor;

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HasCylinderAnyExploded(this, globalCtx)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedExplosion(this, globalCtx, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedExplosion(this, globalCtx, &vec2);

        Actor_Kill(thisx);
    }

    func_8097AEE8(this, globalCtx);
    DemoGj_SetCylindersAsAC(this, globalCtx);
}

// func_8097B340
void DemoGj_Update15(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097B128(this, globalCtx);
}

// func_8097B370
void DemoGj_Update18(DemoGj* this, GlobalContext* globalCtx) {
    func_8097B22C(this, globalCtx);
    DemoGj_DoNothing1(this, globalCtx);
}

void DemoGj_DrawDestructableRubble1(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble2DL);
}

// Inits the three cylinders with `sCylinderInit2`
void DemoGj_InitDestructableRubble2(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 16, 0, NULL);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[0], &sCylinderInit2);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[1], &sCylinderInit2);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[2], &sCylinderInit2);
}

void DemoGj_DoNothing2(DemoGj* this, GlobalContext* globalCtx) {
}

// Moves the ColliderCylinder's relative to the actor's position.
void func_8097B450(DemoGj* this, GlobalContext* globalCtx) {
    ColliderCylinder* cylinder0 = &this->cylinders[0];
    ColliderCylinder* cylinder1 = &this->cylinders[1];
    ColliderCylinder* cylinder2 = &this->cylinders[2];
    Vec3f* actorPos = &this->dyna.actor.world.pos;
    s32 pad;
    s16 theta = this->dyna.actor.world.rot.y;
    f32 cos_theta = Math_CosS(theta);
    f32 sin_theta = Math_SinS(theta);

    cylinder0->dim.pos.z = actorPos->z - (35.0f * sin_theta);
    cylinder0->dim.pos.x = actorPos->x + (35.0f * cos_theta);
    cylinder0->dim.pos.y = actorPos->y;

    cylinder1->dim.pos.z = actorPos->z - (-10.0f * sin_theta);
    cylinder1->dim.pos.x = actorPos->x + (-10.0f * cos_theta);
    cylinder1->dim.pos.y = actorPos->y;

    cylinder2->dim.pos.z = actorPos->z - (-55.0f * sin_theta);
    cylinder2->dim.pos.x = actorPos->x + (-55.0f * cos_theta);
    cylinder2->dim.pos.y = actorPos->y;
}

void DemoGj_SetCylindersAsAC2(DemoGj* this, GlobalContext* globalCtx) {
    s32 pad[2];
    Collider* cylinder0 = &this->cylinders[0].base;
    Collider* cylinder1 = &this->cylinders[1].base;
    Collider* cylinder2 = &this->cylinders[2].base;
    s32 pad2[3];

    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder0);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder1);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, cylinder2);
}

// Does the same as `DemoGj_HasCylinderAnyExploded`
s32 DemoGj_HasCylinderAnyExploded2(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[0])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[1])) {
        return true;
    }
    if (DemoGj_HitByExplosion(this, globalCtx, &this->cylinders[2])) {
        return true;
    }
    return false;
}

void DemoGj_DirectedExplosion2(DemoGj* this, GlobalContext* globalCtx, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;

    DemoGj_Explode(this, globalCtx, &pos, direction);
}

void func_8097B6C4(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, globalCtx, &gGanonsCastleRubble3Col);
        this->updateMode = 19;
        this->drawConfig = 17;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2
 */
void func_8097B750(DemoGj* this, GlobalContext* globalCtx) {
    Actor* thisx = &this->dyna.actor;

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HasCylinderAnyExploded2(this, globalCtx)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedExplosion2(this, globalCtx, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedExplosion2(this, globalCtx, &vec2);

        Actor_Kill(thisx);
    }

    func_8097B450(this, globalCtx);
    DemoGj_SetCylindersAsAC2(this, globalCtx);
}

// func_8097B864
void DemoGj_Update16(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097B6C4(this, globalCtx);
}

// func_8097B894
void DemoGj_Update19(DemoGj* this, GlobalContext* globalCtx) {
    func_8097B750(this, globalCtx);
    DemoGj_DoNothing2(this, globalCtx);
}

void DemoGj_DemoGj_InitDestructableRubble2(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubble3DL);
}

// Inits the first cylinder (only that one) with `sCylinderInit3`
void DemoGj_InitDestructableRubbleTall(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_InitSetIndexes(this, globalCtx, 17, 0, NULL);
    DemoGj_InitCylinder(this, globalCtx, &this->cylinders[0], &sCylinderInit3);
}

void DemoGj_DoNothing3(DemoGj* this, GlobalContext* globalCtx) {
}

void DemoGj_DirectedDoubleExplosion(DemoGj* this, GlobalContext* globalCtx, Vec3f* direction) {
    Vec3f pos;

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    pos.z = this->dyna.actor.world.pos.z;
    DemoGj_Explode(this, globalCtx, &pos, direction);

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y + 100.0f;
    pos.z = this->dyna.actor.world.pos.z;
    DemoGj_Explode(this, globalCtx, &pos, direction);
}

void func_8097B9BC(DemoGj* this, GlobalContext* globalCtx) {
    if (DemoGj_IsGanondorfFloatingInAir(this, globalCtx)) {
        Vec3f* scale = &this->dyna.actor.scale;

        DemoGj_InitCommon(this, globalCtx, &gGanonsCastleRubbleTallCol);
        this->updateMode = 20;
        this->drawConfig = 18;
        scale->x *= 0.8f;
        scale->y *= 0.8f;
        scale->z *= 0.8f;
    }
}

/*
 * Checks if should kill the actor and drop collectibles
 * Kills the actor if ganon->unk_314==4 (Ganon killed), this rubble was hit by an explosion or killFlag==true
 * Used by DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL
 */
void func_8097BA48(DemoGj* this, GlobalContext* globalCtx) {
    Actor* thisx = &this->dyna.actor;
    ColliderCylinder* cylinder = &this->cylinders[0];
    s32 pad[2];

    if (func_809797E4(this, 4)) {
        Actor_Kill(thisx);
    } else if (DemoGj_HitByExplosion(this, globalCtx, cylinder)) {
        Vec3f vec1 = { 0.0f, 0.0f, 0.0f };

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedDoubleExplosion(this, globalCtx, &vec1);

        Actor_Kill(thisx);
    } else if (this->killFlag) {
        Vec3f vec2 = this->unk_26C;
        vec2.y = 0.0f;

        DemoGj_DropCollectible(this, globalCtx);
        DemoGj_DirectedDoubleExplosion(this, globalCtx, &vec2);

        Actor_Kill(thisx);
    }

    Collider_UpdateCylinder(thisx, cylinder);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &cylinder->base);
}

// func_8097BB78
void DemoGj_Update17(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_FindGanon(this, globalCtx);
    func_8097B9BC(this, globalCtx);
}

// func_8097BBA8
void DemoGj_Update20(DemoGj* this, GlobalContext* globalCtx) {
    func_8097BA48(this, globalCtx);
    DemoGj_DoNothing3(this, globalCtx);
}

void DemoGj_DemoGj_InitDestructableRubbleTall(DemoGj* this, GlobalContext* globalCtx) {
    DemoGj_DrawCommon(this, globalCtx, gGanonsCastleRubbleTallDL);
}

static DemoGjUpdateFunc sUpdateFuncs[] = {
    DemoGj_UpdateRubbleAroundArena,
    DemoGj_Update01,
    DemoGj_Update02,
    DemoGj_Update03,
    DemoGj_Update04,
    DemoGj_Update05,
    DemoGj_Update06,
    DemoGj_Update07,
    DemoGj_Update08,
    DemoGj_Update09,
    DemoGj_Update10,
    DemoGj_Update11,
    DemoGj_Update12,
    DemoGj_Update13,
    DemoGj_Update14,
    DemoGj_Update15,
    DemoGj_Update16,
    DemoGj_Update17,
    DemoGj_Update18,
    DemoGj_Update19,
    DemoGj_Update20,
};

void DemoGj_Update(Actor* thisx, GlobalContext* globalCtx) {
    DemoGj* this = (DemoGj*)thisx;

    if (this->updateMode < 0 || this->updateMode >= ARRAY_COUNT(sUpdateFuncs) ||
        sUpdateFuncs[this->updateMode] == NULL) {
        // "The main mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sUpdateFuncs[this->updateMode](this, globalCtx);
}

void DemoGj_Init(Actor* thisx, GlobalContext* globalCtx) {
    DemoGj* this = (DemoGj*)thisx;

    switch (DemoGj_GetType(this)) {
        case DEMOGJ_TYPE_AROUNDARENA:
            DemoGj_InitRubbleAroundArena(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_1:
            DemoGj_InitRubblePile1(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_2:
            DemoGj_InitRubblePile2(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_3:
            DemoGj_InitRubblePile3(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_4:
            DemoGj_InitRubblePile4(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_5:
            DemoGj_InitRubblePile5(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_6:
            DemoGj_InitRubblePile6(this, globalCtx);
            break;

        case DEMOGJ_TYPE_RUBBLE_PILE_7:
            DemoGj_InitRubblePile7(this, globalCtx);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_1:
            DemoGj_InitDestructableRubble1(this, globalCtx);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_2:
            DemoGj_InitDestructableRubble2(this, globalCtx);
            break;

        case DEMOGJ_TYPE_DESTRUCTABLE_RUBBLE_TALL:
            DemoGj_InitDestructableRubbleTall(this, globalCtx);
            break;

        default:
            // "Demo_Gj_Actor_ct There is no such argument!!!!!!!!!!!!!!!!!!!!!!"
            osSyncPrintf(VT_FGCOL(RED) "Demo_Gj_Actor_ct そんな引数は無い!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST, globalCtx,
                         this);
            Actor_Kill(&this->dyna.actor);
    }
}

void DemoGj_DrawNothing(DemoGj* this, GlobalContext* globalCtx) {
}

static DemoGjDrawFunc sDrawFuncs[] = {
    DemoGj_DrawNothing,
    DemoGj_DrawRubbleAroundArena,
    DemoGj_DrawRubble2,
    DemoGj_DrawRubble3,
    DemoGj_DrawRubble4,
    DemoGj_DrawRubble5,
    DemoGj_DrawRubble6,
    DemoGj_DrawRubble7,
    DemoGj_DrawRubbleTall,
    DemoGj_DrawRotatedRubble2,
    DemoGj_DrawRotatedRubble3,
    DemoGj_DrawRotatedRubble4,
    DemoGj_DrawRotatedRubble5,
    DemoGj_DrawRotatedRubble6,
    DemoGj_DrawRotatedRubble7,
    DemoGj_DrawRotatedRubbleTall,
    DemoGj_DrawDestructableRubble1,
    DemoGj_DemoGj_InitDestructableRubble2,
    DemoGj_DemoGj_InitDestructableRubbleTall,
};

void DemoGj_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DemoGj* this = (DemoGj*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= ARRAY_COUNT(sDrawFuncs) || sDrawFuncs[this->drawConfig] == NULL) {
        // "The drawing mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sDrawFuncs[this->drawConfig](this, globalCtx);
}

const ActorInit Demo_Gj_InitVars = {
    ACTOR_DEMO_GJ,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GJ,
    sizeof(DemoGj),
    (ActorFunc)DemoGj_Init,
    (ActorFunc)DemoGj_Destroy,
    (ActorFunc)DemoGj_Update,
    (ActorFunc)DemoGj_Draw,
    NULL,
};

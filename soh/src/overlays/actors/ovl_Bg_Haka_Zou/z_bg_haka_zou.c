/*
 * File: z_bg_haka_zou.c
 * Overlay: ovl_Bg_Haka_Zou
 * Description: Statue and Wall (Shadow Temple)
 */

#include "z_bg_haka_zou.h"
#include "objects/object_hakach_objects/object_hakach_objects.h"
#include "objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_WHILE_CULLED

typedef enum {
    /* 0x0 */ STA_GIANT_BIRD_STATUE,
    /* 0x1 */ STA_BOMBABLE_SKULL_WALL,
    /* 0x2 */ STA_BOMBABLE_RUBBLE,
    /* 0x3 */ STA_UNKNOWN
} ShadowTempleAssetsType;

void BgHakaZou_Init(Actor* thisx, PlayState* play);
void BgHakaZou_Destroy(Actor* thisx, PlayState* play);
void BgHakaZou_Update(Actor* thisx, PlayState* play);
void BgHakaZou_Draw(Actor* thisx, PlayState* play);

void BgHakaZou_Wait(BgHakaZou* this, PlayState* play);
void func_80882BDC(BgHakaZou* this, PlayState* play);
void func_80883000(BgHakaZou* this, PlayState* play);
void func_80883104(BgHakaZou* this, PlayState* play);
void func_80883144(BgHakaZou* this, PlayState* play);
void func_80883254(BgHakaZou* this, PlayState* play);
void func_80883328(BgHakaZou* this, PlayState* play);
void func_808834D8(BgHakaZou* this, PlayState* play);
void BgHakaZou_DoNothing(BgHakaZou* this, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
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
    { 5, 60, 0, { 0, 0, 0 } },
};

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

const ActorInit Bg_Haka_Zou_InitVars = {
    ACTOR_BG_HAKA_ZOU,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgHakaZou),
    (ActorFunc)BgHakaZou_Init,
    (ActorFunc)BgHakaZou_Destroy,
    (ActorFunc)BgHakaZou_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgHakaZou_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaZou* this = (BgHakaZou*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);

    this->switchFlag = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    if (thisx->params == STA_UNKNOWN) {
        Actor_SetScale(thisx, (Rand_ZeroOne() * 0.005f) + 0.025f);

        thisx->speedXZ = Rand_ZeroOne();
        thisx->world.rot.y = thisx->shape.rot.y * ((Rand_ZeroOne() < 0.5f) ? -1 : 1) + Rand_CenteredFloat(0x1000);
        this->timer = 20;
        thisx->world.rot.x = Rand_S16Offset(0x100, 0x300) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
        thisx->world.rot.z = Rand_S16Offset(0x400, 0x800) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1);
    } else {
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
        Collider_UpdateCylinder(thisx, &this->collider);

        DynaPolyActor_Init(&this->dyna, 0);

        if (thisx->params == STA_GIANT_BIRD_STATUE) {
            thisx->uncullZoneForward = 2000.0f;
            thisx->uncullZoneScale = 3000.0f;
            thisx->uncullZoneDownward = 3000.0f;
        }
    }

    this->requiredObjBankIndex = (thisx->params == STA_BOMBABLE_RUBBLE)
                                     ? Object_GetIndex(&play->objectCtx, OBJECT_HAKACH_OBJECTS)
                                     : Object_GetIndex(&play->objectCtx, OBJECT_HAKA_OBJECTS);

    if (this->requiredObjBankIndex < 0) {
        Actor_Kill(thisx);
    } else if ((thisx->params != STA_UNKNOWN) && Flags_GetSwitch(play, this->switchFlag)) {
        if (thisx->params != STA_GIANT_BIRD_STATUE) {
            Actor_Kill(thisx);
        } else {
            thisx->shape.rot.x = -0x4000;
            thisx->world.pos.z -= 80.0f;
            thisx->world.pos.y -= 54.0f;
        }
    }

    this->actionFunc = BgHakaZou_Wait;
}

void BgHakaZou_Destroy(Actor* thisx, PlayState* play) {
    BgHakaZou* this = (BgHakaZou*)thisx;

    if (this->dyna.actor.params != STA_UNKNOWN) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_808828F4(BgHakaZou* this, PlayState* play) {
    Vec3f effectPos;
    Vec3f effectVelocity;
    f32 rand;
    s32 i;

    effectVelocity.x = 0.0f;
    effectVelocity.y = 1.0f;
    effectVelocity.z = 2.0f;

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            effectPos.x = this->dyna.actor.world.pos.x - (Rand_CenteredFloat(10.0f) + 112.0f);
        } else {
            effectPos.x = Rand_CenteredFloat(10.0f) + this->dyna.actor.world.pos.x;
        }

        rand = Rand_ZeroOne();
        effectPos.y = this->dyna.actor.world.pos.y + (60.0f * rand);
        effectPos.z = this->dyna.actor.world.pos.z + (112.0f * rand);

        func_800286CC(play, &effectPos, &effectVelocity, &sZeroVec, (Rand_ZeroOne() * 200.0f) + 1000.0f, 100);
    }
}

void BgHakaZou_Wait(BgHakaZou* this, PlayState* play) {
    CollisionHeader* colHeader;

    if (Object_IsLoaded(&play->objectCtx, this->requiredObjBankIndex)) {
        this->dyna.actor.objBankIndex = this->requiredObjBankIndex;
        this->dyna.actor.draw = BgHakaZou_Draw;

        if (this->dyna.actor.params == STA_UNKNOWN) {
            this->actionFunc = func_80882BDC;
        } else {
            Actor_SetObjectDependency(play, &this->dyna.actor);

            colHeader = NULL;

            if (this->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
                CollisionHeader_GetVirtual(&object_haka_objects_Col_006F70, &colHeader);
                this->collider.dim.radius = 80;
                this->collider.dim.height = 100;
                this->collider.dim.yShift = -30;
                this->collider.dim.pos.x -= 56;
                this->collider.dim.pos.z += 56;
                this->dyna.actor.uncullZoneScale = 1500.0f;
            } else if (this->dyna.actor.params == STA_BOMBABLE_SKULL_WALL) {
                CollisionHeader_GetVirtual(&object_haka_objects_Col_005E30, &colHeader);
                this->collider.dim.yShift = -50;
            } else {
                CollisionHeader_GetVirtual(&gBotwBombSpotCol, &colHeader);
                this->collider.dim.radius = 55;
                this->collider.dim.height = 20;
            }

            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

            if ((this->dyna.actor.params == STA_GIANT_BIRD_STATUE) && Flags_GetSwitch(play, this->switchFlag)) {
                this->actionFunc = BgHakaZou_DoNothing;
            } else {
                this->actionFunc = func_80883000;
            }
        }
    }
}
void func_80882BDC(BgHakaZou* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.shape.rot.x += this->dyna.actor.world.rot.x;
    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;

    if (this->dyna.actor.bgCheckFlags & 2) {
        if (this->dyna.actor.velocity.y < -8.0f) {
            this->dyna.actor.velocity.y *= -0.6f;
            this->dyna.actor.velocity.y = CLAMP_MAX(this->dyna.actor.velocity.y, 10.0f);
            this->dyna.actor.bgCheckFlags &= ~3;
            this->dyna.actor.speedXZ = 2.0f;
        } else {
            Actor_Kill(&this->dyna.actor);
        }
    }

    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80882CC4(BgHakaZou* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f actorSpawnPos;
    f32 sin;
    f32 cos;
    s32 pad;

    sin = Math_SinS(this->dyna.actor.shape.rot.y - 0x4000) * 40.0f;
    cos = Math_CosS(this->dyna.actor.shape.rot.y - 0x4000) * 40.0f;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            actorSpawnPos.x = this->dyna.actor.world.pos.x + (j - 1) * sin;
            actorSpawnPos.z = this->dyna.actor.world.pos.z + (j - 1) * cos;
            actorSpawnPos.y = this->dyna.actor.world.pos.y + (i - 1) * 55;

            Actor_Spawn(&play->actorCtx, play, ACTOR_BG_HAKA_ZOU, actorSpawnPos.x, actorSpawnPos.y,
                        actorSpawnPos.z, 0, this->dyna.actor.shape.rot.y, 0, this->dyna.actor.params + 2, true);
            func_800286CC(play, &actorSpawnPos, &sZeroVec, &sZeroVec, 1000, 50);
        }
    }
}

void func_80882E54(BgHakaZou* this, PlayState* play) {
    Vec3f fragmentPos;
    s32 i;
    s32 j;
    s32 num = 25;

    fragmentPos.x = this->collider.dim.pos.x;
    fragmentPos.y = this->collider.dim.pos.y;
    fragmentPos.z = this->collider.dim.pos.z;

    EffectSsHahen_SpawnBurst(play, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            fragmentPos.x = this->collider.dim.pos.x + (((j * 2) - 1) * num);
            fragmentPos.z = this->collider.dim.pos.z + (((i * 2) - 1) * num);
            EffectSsHahen_SpawnBurst(play, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);
            func_800286CC(play, &fragmentPos, &sZeroVec, &sZeroVec, 1000, 50);
        }
    }
}

void func_80883000(BgHakaZou* this, PlayState* play) {
    // #region SOH [Co-op]
    if ((this->collider.base.acFlags & AC_HIT) || Flags_GetSwitch(play, this->switchFlag)) {
    // #endregion
        Flags_SetSwitch(play, this->switchFlag);

        if (this->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
            this->timer = 20;
            this->actionFunc = func_80883144;
            OnePointCutscene_Init(play, 3400, 999, &this->dyna.actor, MAIN_CAM);
        } else if (this->dyna.actor.params == 2) {
            func_80882E54(this, play);
            this->dyna.actor.draw = NULL;
            this->timer = 1;
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_EXPLOSION);
            this->actionFunc = func_80883104;
        } else {
            func_80882CC4(this, play);
            this->timer = 1;
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
            this->actionFunc = func_80883104;
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80883104(BgHakaZou* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_80883144(BgHakaZou* this, PlayState* play) {
    Vec3f explosionPos;

    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        explosionPos.x = Rand_CenteredFloat(200.0f) + (this->dyna.actor.world.pos.x - 56.0f);
        explosionPos.y = (Rand_ZeroOne() * 80.0f) + this->dyna.actor.world.pos.y;
        explosionPos.z = Rand_CenteredFloat(200.0f) + (this->dyna.actor.world.pos.z + 56.0f);

        EffectSsBomb2_SpawnLayered(play, &explosionPos, &sZeroVec, &sZeroVec, 150, 70);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (this->timer == 0) {
        this->timer = 20;
        this->actionFunc = func_80883254;
    }
}

void func_80883254(BgHakaZou* this, PlayState* play) {
    f32 moveDist = (Rand_ZeroOne() * 0.5f) + 0.5f;

    Math_StepToF(&this->dyna.actor.world.pos.z, this->dyna.actor.home.pos.z - 80.0f, 2.0f * moveDist);

    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 40.0f, moveDist)) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            this->timer = 60;
            this->dyna.actor.world.rot.x = 8;
            this->actionFunc = func_80883328;
        }
    } else {
        func_808828F4(this, play);
    }
}

void func_80883328(BgHakaZou* this, PlayState* play) {
    Vec3f effectPos;
    s32 i;
    s32 j;

    this->dyna.actor.world.rot.x += this->dyna.actor.world.rot.x / 8.0f;

    if (Math_ScaledStepToS(&this->dyna.actor.shape.rot.x, -0x4000, this->dyna.actor.world.rot.x)) {
        effectPos.x = this->dyna.actor.world.pos.x;
        effectPos.y = this->dyna.actor.world.pos.y;

        for (j = 0; j < 2; j++) {
            effectPos.z = this->dyna.actor.world.pos.z;

            for (i = 0; i < 4; i++) {
                effectPos.z -= (i == 2) ? 550.0f : 50.0f;
                func_800286CC(play, &effectPos, &sZeroVec, &sZeroVec, (Rand_ZeroOne() * 200.0f) + 1000.0f, 200);
            }

            effectPos.x -= 112.0f;
        }

        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
        this->timer = 25;
        this->actionFunc = func_808834D8;
    }
}

void func_808834D8(BgHakaZou* this, PlayState* play) {
    f32 moveDist;

    if (this->timer != 0) {
        this->timer--;
    }

    moveDist = (this->timer % 2) ? 15.0f : -15.0f;
    this->dyna.actor.world.pos.y += ((this->timer & 0xFE) * 0.04f * moveDist);

    if (this->timer == 0) {
        this->actionFunc = BgHakaZou_DoNothing;
    }
}

void BgHakaZou_DoNothing(BgHakaZou* this, PlayState* play) {
}

void BgHakaZou_Update(Actor* thisx, PlayState* play) {
    BgHakaZou* this = (BgHakaZou*)thisx;

    this->actionFunc(this, play);

    if (this->dyna.actor.params == 3) {
        Actor_MoveForward(&this->dyna.actor);
    }
}

void BgHakaZou_Draw(Actor* thisx, PlayState* play) {
    static Gfx* dLists[] = {
        object_haka_objects_DL_0064E0,
        object_haka_objects_DL_005CE0,
        gBotwBombSpotDL,
        object_haka_objects_DL_005CE0,
    };

    Gfx_DrawDListOpa(play, dLists[thisx->params]);
}

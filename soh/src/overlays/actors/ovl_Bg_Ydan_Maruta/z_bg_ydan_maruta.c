/*
 * File: z_bg_ydan_maruta.c
 * Overlay: ovl_Bg_Ydan_Maruta
 * Description: Rotating spike log and falling ladder in Deku Tree
 */

#include "z_bg_ydan_maruta.h"
#include "objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS 0

void BgYdanMaruta_Init(Actor* thisx, PlayState* play);
void BgYdanMaruta_Destroy(Actor* thisx, PlayState* play);
void BgYdanMaruta_Update(Actor* thisx, PlayState* play);
void BgYdanMaruta_Draw(Actor* thisx, PlayState* play);

void func_808BEFF4(BgYdanMaruta* this, PlayState* play);
void BgYdanMaruta_DoNothing(BgYdanMaruta* this, PlayState* play);
void func_808BF078(BgYdanMaruta* this, PlayState* play);
void func_808BF108(BgYdanMaruta* this, PlayState* play);
void func_808BF1EC(BgYdanMaruta* this, PlayState* play);

const ActorInit Bg_Ydan_Maruta_InitVars = {
    ACTOR_BG_YDAN_MARUTA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_YDAN_OBJECTS,
    sizeof(BgYdanMaruta),
    (ActorFunc)BgYdanMaruta_Init,
    (ActorFunc)BgYdanMaruta_Destroy,
    (ActorFunc)BgYdanMaruta_Update,
    (ActorFunc)BgYdanMaruta_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_WOOD,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 220.0f, -10.0f, 0.0f }, { 220.0f, 10.0f, 0.0f }, { -220.0f, 10.0f, 0.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000004, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_WOOD,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 16.0f, 0.0f, 0.0f }, { 16.0f, 135.0f, 0.0f }, { -16.0f, 135.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgYdanMaruta_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;
    Vec3f sp4C[3];
    s32 i;
    f32 sinRotY;
    f32 cosRotY;
    CollisionHeader* colHeader = NULL;
    ColliderTrisElementInit* triInit;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Collider_InitTris(play, &this->collider);
    Collider_SetTris(play, &this->collider, &this->dyna.actor, &sTrisInit, this->elements);

    this->switchFlag = this->dyna.actor.params & 0xFFFF;
    thisx->params = (thisx->params >> 8) & 0xFF; // thisx is required to match here

    if (this->dyna.actor.params == 0) {
        triInit = &sTrisElementsInit[0];
        this->actionFunc = func_808BEFF4;
    } else {
        triInit = &sTrisElementsInit[1];
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        CollisionHeader_GetVirtual(&gDTFallingLadderCol, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
        thisx->home.pos.y += -280.0f;
        if (Flags_GetSwitch(play, this->switchFlag)) {
            thisx->world.pos.y = thisx->home.pos.y;
            this->actionFunc = BgYdanMaruta_DoNothing;
        } else {
            this->actionFunc = func_808BF078;
        }
    }

    sinRotY = Math_SinS(this->dyna.actor.shape.rot.y);
    cosRotY = Math_CosS(this->dyna.actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        sp4C[i].x = (triInit->dim.vtx[i].x * cosRotY) + this->dyna.actor.world.pos.x;
        sp4C[i].y = triInit->dim.vtx[i].y + this->dyna.actor.world.pos.y;
        sp4C[i].z = this->dyna.actor.world.pos.z - (triInit->dim.vtx[i].x * sinRotY);
    }

    Collider_SetTrisVertices(&this->collider, 0, &sp4C[0], &sp4C[1], &sp4C[2]);

    sp4C[1].x = (triInit->dim.vtx[2].x * cosRotY) + this->dyna.actor.world.pos.x;
    sp4C[1].y = triInit->dim.vtx[0].y + this->dyna.actor.world.pos.y;
    sp4C[1].z = this->dyna.actor.world.pos.z - (triInit->dim.vtx[2].x * sinRotY);

    Collider_SetTrisVertices(&this->collider, 1, &sp4C[0], &sp4C[2], &sp4C[1]);
}

void BgYdanMaruta_Destroy(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    Collider_DestroyTris(play, &this->collider);
    if (this->dyna.actor.params == 1) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_808BEFF4(BgYdanMaruta* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_HIT) {
        func_8002F71C(play, &this->dyna.actor, 7.0f, this->dyna.actor.shape.rot.y, 6.0f);
    }
    this->dyna.actor.shape.rot.x += 0x360;
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    func_8002F974(&this->dyna.actor, NA_SE_EV_TOGE_STICK_ROLLING - SFX_FLAG);
}

void func_808BF078(BgYdanMaruta* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->unk_16A = 20;
        Flags_SetSwitch(play, this->switchFlag);
        func_80078884(NA_SE_SY_CORRECT_CHIME);
        this->actionFunc = func_808BF108;
        OnePointCutscene_Init(play, 3010, 50, &this->dyna.actor, MAIN_CAM);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_808BF108(BgYdanMaruta* this, PlayState* play) {
    s16 temp;

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }
    if (this->unk_16A == 0) {
        this->actionFunc = func_808BF1EC;
    }

    temp = (this->unk_16A % 4) - 2;
    if (temp == -2) {
        temp = 0;
    } else {
        temp *= 2;
    }

    this->dyna.actor.world.pos.x = (Math_CosS(this->dyna.actor.shape.rot.y) * temp) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (Math_SinS(this->dyna.actor.shape.rot.y) * temp) + this->dyna.actor.home.pos.z;

    func_8002F974(&this->dyna.actor, NA_SE_EV_TRAP_OBJ_SLIDE - SFX_FLAG);
}

void func_808BF1EC(BgYdanMaruta* this, PlayState* play) {
    this->dyna.actor.velocity.y += 1.0f;
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_LADDER_DOUND);
        this->actionFunc = BgYdanMaruta_DoNothing;
    }
}

void BgYdanMaruta_DoNothing(BgYdanMaruta* this, PlayState* play) {
}

void BgYdanMaruta_Update(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    this->actionFunc(this, play);
}

void BgYdanMaruta_Draw(Actor* thisx, PlayState* play) {
    BgYdanMaruta* this = (BgYdanMaruta*)thisx;

    if (this->dyna.actor.params == 0) {
        Gfx_DrawDListOpa(play, gDTRollingSpikeTrapDL);
    } else {
        Gfx_DrawDListOpa(play, gDTFallingLadderDL);
    }
}

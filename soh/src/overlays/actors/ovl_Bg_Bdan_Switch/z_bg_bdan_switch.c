/*
 * File: z_bg_bdan_switch.c
 * Overlay: ovl_Bg_Bdan_Switch
 * Description: Switches Inside Lord Jabu-Jabu
 */

#include "z_bg_bdan_switch.h"
#include "objects/object_bdan_objects/object_bdan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgBdanSwitch_Init(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Update(Actor* thisx, GlobalContext* globalCtx);
void BgBdanSwitch_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_8086D5C4(BgBdanSwitch* this);
void func_8086D5E0(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D67C(BgBdanSwitch* this);
void func_8086D694(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D730(BgBdanSwitch* this);
void func_8086D754(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D7FC(BgBdanSwitch* this);
void func_8086D80C(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D86C(BgBdanSwitch* this);
void func_8086D888(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D8BC(BgBdanSwitch* this);
void func_8086D8CC(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D95C(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086D9F8(BgBdanSwitch* this);
void func_8086DA1C(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DAB4(BgBdanSwitch* this);
void func_8086DAC4(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DB24(BgBdanSwitch* this);
void func_8086DB40(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DB4C(BgBdanSwitch* this);
void func_8086DB68(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DC30(BgBdanSwitch* this);
void func_8086DC48(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DCCC(BgBdanSwitch* this);
void func_8086DCE8(BgBdanSwitch* this, GlobalContext* globalCtx);
void func_8086DDA8(BgBdanSwitch* this);
void func_8086DDC0(BgBdanSwitch* this, GlobalContext* globalCtx);

const ActorInit Bg_Bdan_Switch_InitVars = {
    ACTOR_BG_BDAN_SWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_BDAN_OBJECTS,
    sizeof(BgBdanSwitch),
    (ActorFunc)BgBdanSwitch_Init,
    (ActorFunc)BgBdanSwitch_Destroy,
    (ActorFunc)BgBdanSwitch_Update,
    (ActorFunc)BgBdanSwitch_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 120, 0 }, 370 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1200, ICHAIN_STOP),
};

static Vec3f D_8086E0E0 = { 0.0f, 140.0f, 0.0f };

void BgBdanSwitch_InitDynaPoly(BgBdanSwitch* this, GlobalContext* globalCtx, CollisionHeader* collision, s32 flag) {
    s16 pad1;
    CollisionHeader* colHeader = NULL;
    s16 pad2;

    DynaPolyActor_Init(&this->dyna, flag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void BgBdanSwitch_InitCollision(BgBdanSwitch* this, GlobalContext* globalCtx) {
    Actor* actor = &this->dyna.actor;
    Collider_InitJntSph(globalCtx, &this->collider);
    Collider_SetJntSph(globalCtx, &this->collider, actor, &sJntSphInit, this->colliderItems);
}

void func_8086D0EC(BgBdanSwitch* this) {
    if (this->unk_1CC > 0) {
        this->unk_1CC += 0x5DC;
    } else {
        this->unk_1CC += 0xFA0;
    }

    switch (this->dyna.actor.params & 0xFF) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            this->unk_1D4 = ((Math_CosS(this->unk_1CC) * 0.5f) + (53.000004f / 6.0f)) * 0.012f;
            this->unk_1D0 = ((Math_CosS(this->unk_1CC) * 0.5f) + 20.5f) * (this->unk_1C8 * 0.0050000004f);
            this->dyna.actor.scale.y = this->unk_1C8 * 0.1f;
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            this->unk_1D4 = ((Math_CosS(this->unk_1CC) * 0.5f) + (43.0f / 6.0f)) * 0.0075000003f;
            this->unk_1D0 = ((Math_CosS(this->unk_1CC) * 0.5f) + 20.5f) * (this->unk_1C8 * 0.0050000004f);
            this->dyna.actor.scale.y = this->unk_1C8 * 0.1f;
    }
    this->dyna.actor.shape.yOffset = 1.2f / this->unk_1D0;
}

void BgBdanSwitch_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;
    s32 pad;
    s16 type;
    s32 flag;

    type = this->dyna.actor.params & 0xFF;
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    if (type == YELLOW_TALL_1 || type == YELLOW_TALL_2) {
        this->dyna.actor.scale.z = 0.05f;
        this->dyna.actor.scale.x = 0.05f;
    } else {
        this->dyna.actor.scale.z = 0.1f;
        this->dyna.actor.scale.x = 0.1f;
    }
    this->dyna.actor.scale.y = 0.0f;
    Actor_SetFocus(&this->dyna.actor, 10.0f);

    switch (type) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            BgBdanSwitch_InitDynaPoly(this, globalCtx, &gJabuFloorSwitchCol, DPM_PLAYER);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            BgBdanSwitch_InitCollision(this, globalCtx);
            this->dyna.actor.flags |= ACTOR_FLAG_0;
            this->dyna.actor.targetMode = 4;
            break;
    }

    flag = Flags_GetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F);

    switch (type) {
        case BLUE:
        case YELLOW:
            if (flag) {
                func_8086D730(this);
            } else {
                func_8086D5C4(this);
            }
            break;
        case YELLOW_HEAVY:
            if (flag) {
                func_8086DB24(this);
            } else {
                func_8086D86C(this);
            }
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            if (flag) {
                func_8086DCCC(this);
            } else {
                func_8086DB4C(this);
            }
            break;
        default:
            osSyncPrintf("不正な ARG_DATA(arg_data 0x%04x)(%s %d)\n", this->dyna.actor.params, __FILE__,
                         __LINE__);
            Actor_Kill(&this->dyna.actor);
            return;
    }
    osSyncPrintf("(巨大魚ダンジョン 専用スイッチ)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void BgBdanSwitch_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;

    switch (this->dyna.actor.params & 0xFF) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            Collider_DestroyJntSph(globalCtx, &this->collider);
            break;
    }
}

void func_8086D4B4(BgBdanSwitch* this, GlobalContext* globalCtx) {
    s32 pad;
    s32 type;

    if (!Flags_GetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F)) {
        type = this->dyna.actor.params & 0xFF;
        Flags_SetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F);
        if (type == BLUE || type == YELLOW_TALL_2) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            OnePointCutscene_AttentionSetSfx(globalCtx, &this->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        }
    }
}

void func_8086D548(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if (Flags_GetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F)) {
        Flags_UnsetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F);
        if ((this->dyna.actor.params & 0xFF) == YELLOW_TALL_2) {
            OnePointCutscene_AttentionSetSfx(globalCtx, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void func_8086D5C4(BgBdanSwitch* this) {
    this->actionFunc = func_8086D5E0;
    this->unk_1C8 = 1.0f;
}

void func_8086D5E0(BgBdanSwitch* this, GlobalContext* globalCtx) {
    switch (this->dyna.actor.params & 0xFF) {
        case BLUE:
            if (func_800435B4(&this->dyna)) {
                func_8086D67C(this);
                func_8086D4B4(this, globalCtx);
            }
            break;
        case YELLOW:
            if (func_8004356C(&this->dyna)) {
                func_8086D67C(this);
                func_8086D4B4(this, globalCtx);
            }
            break;
    }
}

void func_8086D67C(BgBdanSwitch* this) {
    this->actionFunc = func_8086D694;
    this->unk_1DA = 0x64;
}

void func_8086D694(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if ((func_8005B198() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.2f;
        if (this->unk_1C8 <= 0.1f) {
            func_8086D730(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
        }
    }
}

void func_8086D730(BgBdanSwitch* this) {
    this->unk_1C8 = 0.1f;
    this->actionFunc = func_8086D754;
    this->unk_1D8 = 6;
}

void func_8086D754(BgBdanSwitch* this, GlobalContext* globalCtx) {
    switch (this->dyna.actor.params & 0xFF) {
        case BLUE:
            if (!func_800435B4(&this->dyna)) {
                if (this->unk_1D8 <= 0) {
                    func_8086D7FC(this);
                    func_8086D548(this, globalCtx);
                }
            } else {
                this->unk_1D8 = 6;
            }
            break;
        case YELLOW:
            if (!Flags_GetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F)) {
                func_8086D7FC(this);
            }
            break;
    }
}

void func_8086D7FC(BgBdanSwitch* this) {
    this->actionFunc = func_8086D80C;
}

void func_8086D80C(BgBdanSwitch* this, GlobalContext* globalCtx) {
    this->unk_1C8 += 0.2f;
    if (this->unk_1C8 >= 1.0f) {
        func_8086D5C4(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void func_8086D86C(BgBdanSwitch* this) {
    this->actionFunc = func_8086D888;
    this->unk_1C8 = 1.0f;
}

void func_8086D888(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if (func_8004356C(&this->dyna)) {
        func_8086D8BC(this);
    }
}

void func_8086D8BC(BgBdanSwitch* this) {
    this->actionFunc = func_8086D8CC;
}

void func_8086D8CC(BgBdanSwitch* this, GlobalContext* globalCtx) {
    this->unk_1C8 -= 0.2f;
    if (this->unk_1C8 <= 0.6f) {
        func_8086D9F8(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
    }
}

void func_8086D944(BgBdanSwitch* this) {
    this->actionFunc = func_8086D95C;
    this->unk_1DA = 0x64;
}

void func_8086D95C(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if ((func_8005B198() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.2f;
        if (this->unk_1C8 <= 0.1f) {
            func_8086DB24(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 0x78, 0x14, 0xA);
        }
    }
}

void func_8086D9F8(BgBdanSwitch* this) {
    this->unk_1C8 = 0.6f;
    this->actionFunc = func_8086DA1C;
    this->unk_1D8 = 6;
}

void func_8086DA1C(BgBdanSwitch* this, GlobalContext* globalCtx) {
    Actor* heldActor = GET_PLAYER(globalCtx)->heldActor;

    if (func_8004356C(&this->dyna)) {
        if (heldActor != NULL && heldActor->id == ACTOR_EN_RU1) {
            if (this->unk_1D8 <= 0) {
                func_8086D944(this);
                func_8086D4B4(this, globalCtx);
            }
        } else {
            this->unk_1D8 = 6;
        }
    } else {
        if (this->unk_1D8 <= 0) {
            func_8086DAB4(this);
        }
    }
}

void func_8086DAB4(BgBdanSwitch* this) {
    this->actionFunc = func_8086DAC4;
}

void func_8086DAC4(BgBdanSwitch* this, GlobalContext* globalCtx) {
    this->unk_1C8 += 0.2f;
    if (this->unk_1C8 >= 1.0f) {
        func_8086D86C(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void func_8086DB24(BgBdanSwitch* this) {
    this->unk_1C8 = 0.1f;
    this->actionFunc = func_8086DB40;
}

void func_8086DB40(BgBdanSwitch* this, GlobalContext* globalCtx) {
}

void func_8086DB4C(BgBdanSwitch* this) {
    this->actionFunc = func_8086DB68;
    this->unk_1C8 = 2.0f;
}

void func_8086DB68(BgBdanSwitch* this, GlobalContext* globalCtx) {
    switch (this->dyna.actor.params & 0xFF) {
        default:
            return;
        case YELLOW_TALL_1:
            if (((this->collider.base.acFlags & AC_HIT) != 0) && this->unk_1D8 <= 0) {
                this->unk_1D8 = 0xA;
                func_8086DC30(this);
                func_8086D4B4(this, globalCtx);
            }
            break;
        case YELLOW_TALL_2:
            if (((this->collider.base.acFlags & AC_HIT) != 0) && ((this->unk_1DC & 2) == 0) && this->unk_1D8 <= 0) {
                this->unk_1D8 = 0xA;
                func_8086DC30(this);
                func_8086D4B4(this, globalCtx);
            }
            break;
    }
}

void func_8086DC30(BgBdanSwitch* this) {
    this->actionFunc = func_8086DC48;
    this->unk_1DA = 0x64;
}

void func_8086DC48(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if ((func_8005B198() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.3f;
        if (this->unk_1C8 <= 1.0f) {
            func_8086DCCC(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void func_8086DCCC(BgBdanSwitch* this) {
    this->actionFunc = func_8086DCE8;
    this->unk_1C8 = 1.0f;
}

void func_8086DCE8(BgBdanSwitch* this, GlobalContext* globalCtx) {
    switch (this->dyna.actor.params & 0xFF) {
        case YELLOW_TALL_1:
            if (!Flags_GetSwitch(globalCtx, (this->dyna.actor.params >> 8) & 0x3F)) {
                func_8086DDA8(this);
            }
            break;
        case YELLOW_TALL_2:
            if (((this->collider.base.acFlags & AC_HIT) != 0) && ((this->unk_1DC & 2) == 0) && (this->unk_1D8 <= 0)) {
                this->unk_1D8 = 0xA;
                func_8086DDA8(this);
                func_8086D548(this, globalCtx);
            }
            break;
    }
}

void func_8086DDA8(BgBdanSwitch* this) {
    this->actionFunc = func_8086DDC0;
    this->unk_1DA = 0x64;
}

void func_8086DDC0(BgBdanSwitch* this, GlobalContext* globalCtx) {
    if ((((this->dyna.actor.params & 0xFF) != YELLOW_TALL_2) || (func_8005B198() == this->dyna.actor.category)) ||
        (this->unk_1DA <= 0)) {
        this->unk_1C8 += 0.3f;
        if (this->unk_1C8 >= 2.0f) {
            func_8086DB4C(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void BgBdanSwitch_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;
    s32 type;

    if (this->unk_1DA > 0) {
        this->unk_1DA--;
    }
    this->actionFunc(this, globalCtx);
    func_8086D0EC(this);
    type = this->dyna.actor.params & 0xFF;
    if (type != 3 && type != 4) {
        this->unk_1D8--;
    } else {
        if (!Player_InCsMode(globalCtx) && this->unk_1D8 > 0) {
            this->unk_1D8--;
        }
        this->unk_1DC = this->collider.base.acFlags;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.elements[0].dim.modelSphere.radius = this->unk_1D4 * 370.0f;
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
}

void func_8086DF58(BgBdanSwitch* this, GlobalContext* globalCtx, Gfx* dlist) {
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.y + (this->dyna.actor.shape.yOffset * this->unk_1D0),
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(this->unk_1D4, this->unk_1D0, this->unk_1D4, MTXMODE_APPLY);
    Gfx_DrawDListOpa(globalCtx, dlist);
}

void BgBdanSwitch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;

    switch (this->dyna.actor.params & 0xFF) {
        case YELLOW_HEAVY:
        case YELLOW:
            func_8086DF58(this, globalCtx, gJabuYellowFloorSwitchDL);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            func_8086DF58(this, globalCtx, gJabuYellowFloorSwitchDL);
            Collider_UpdateSpheres(0, &this->collider);
            Matrix_MultVec3f(&D_8086E0E0, &this->dyna.actor.focus.pos);
            break;
        case BLUE:
            func_8086DF58(this, globalCtx, gJabuBlueFloorSwitchDL);
            break;
    }
}

/*
 * File: z_obj_switch.c
 * Overlay: ovl_Obj_Switch
 * Description: Switches
 */

#include "z_obj_switch.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

// type:        (this->dyna.actor.params & 7)
// subtype:     (this->dyna.actor.params >> 4 & 7)
// switch flag: (this->dyna.actor.params >> 8 & 0x3F)
// frozen:      this->dyna.actor.params >> 7 & 1

void ObjSwitch_Init(Actor* thisx, PlayState* play);
void ObjSwitch_Destroy(Actor* thisx, PlayState* play);
void ObjSwitch_Update(Actor* thisx, PlayState* play);
void ObjSwitch_Draw(Actor* thisx, PlayState* play);

void ObjSwitch_FloorUpInit(ObjSwitch* this);
void ObjSwitch_FloorUp(ObjSwitch* this, PlayState* play);
void ObjSwitch_FloorPressInit(ObjSwitch* this);
void ObjSwitch_FloorPress(ObjSwitch* this, PlayState* play);
void ObjSwitch_FloorDownInit(ObjSwitch* this);
void ObjSwitch_FloorDown(ObjSwitch* this, PlayState* play);
void ObjSwitch_FloorReleaseInit(ObjSwitch* this);
void ObjSwitch_FloorRelease(ObjSwitch* this, PlayState* play);

void ObjSwitch_EyeFrozenInit(ObjSwitch* this);
void ObjSwitch_EyeInit(ObjSwitch* this, PlayState* play);
void ObjSwitch_EyeOpenInit(ObjSwitch* this);
void ObjSwitch_EyeOpen(ObjSwitch* this, PlayState* play);
void ObjSwitch_EyeClosingInit(ObjSwitch* this);
void ObjSwitch_EyeClosing(ObjSwitch* this, PlayState* play);
void ObjSwitch_EyeClosedInit(ObjSwitch* this);
void ObjSwitch_EyeClosed(ObjSwitch* this, PlayState* play);
void ObjSwitch_EyeOpeningInit(ObjSwitch* this);
void ObjSwitch_EyeOpening(ObjSwitch* this, PlayState* play);

void ObjSwitch_CrystalOffInit(ObjSwitch* this);
void ObjSwitch_CrystalOff(ObjSwitch* this, PlayState* play);
void ObjSwitch_CrystalTurnOnInit(ObjSwitch* this);
void ObjSwitch_CrystalTurnOn(ObjSwitch* this, PlayState* play);
void ObjSwitch_CrystalOnInit(ObjSwitch* this);
void ObjSwitch_CrystalOn(ObjSwitch* this, PlayState* play);
void ObjSwitch_CrystalTurnOffInit(ObjSwitch* this);
void ObjSwitch_CrystalTurnOff(ObjSwitch* this, PlayState* play);

const ActorInit Obj_Switch_InitVars = {
    ACTOR_OBJ_SWITCH,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(ObjSwitch),
    (ActorFunc)ObjSwitch_Init,
    (ActorFunc)ObjSwitch_Destroy,
    (ActorFunc)ObjSwitch_Update,
    (ActorFunc)ObjSwitch_Draw,
    NULL,
};

static f32 sHeights[] = { 10, 10, 0, 30, 30 };

static ColliderTrisElementInit D_80B9EC34[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, 20.0f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, -20.0f } } },
    },
};

static ColliderTrisInit sRustyFloorTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    D_80B9EC34,
};

static ColliderTrisElementInit D_80B9ECBC[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit trisColliderEye = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    D_80B9ECBC,
};

static ColliderJntSphElementInit D_80B9ED44[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 300, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sCyrstalJntSphereInit = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    D_80B9ED44,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2000, ICHAIN_STOP),
};

void ObjSwitch_RotateY(Vec3f* dest, Vec3f* src, s16 angle) {
    f32 s = Math_SinS(angle);
    f32 c = Math_CosS(angle);

    dest->x = src->z * s + src->x * c;
    dest->y = src->y;
    dest->z = src->z * c - src->x * s;
}

void ObjSwitch_InitDynapoly(ObjSwitch* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, moveFlag);
    CollisionHeader_GetVirtual(collision, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.bgId == BG_ACTOR_MAX) {
        // "Warning : move BG registration failure"
        osSyncPrintf("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", __FILE__, __LINE__,
                     this->dyna.actor.id, this->dyna.actor.params);
    }
}

void ObjSwitch_InitJntSphCollider(ObjSwitch* this, PlayState* play, ColliderJntSphInit* colliderJntSphInit) {
    ColliderJntSph* colliderJntSph = &this->jntSph.col;

    Collider_InitJntSph(play, colliderJntSph);
    Collider_SetJntSph(play, colliderJntSph, &this->dyna.actor, colliderJntSphInit, this->jntSph.items);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.y +
                                     this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    Collider_UpdateSpheres(0, colliderJntSph);
}

void ObjSwitch_InitTrisCollider(ObjSwitch* this, PlayState* play, ColliderTrisInit* colliderTrisInit) {
    ColliderTris* colliderTris = &this->tris.col;
    s32 i;
    s32 j;
    Vec3f pos[3];

    Collider_InitTris(play, colliderTris);
    Collider_SetTris(play, colliderTris, &this->dyna.actor, colliderTrisInit, this->tris.items);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            ObjSwitch_RotateY(&pos[j], &colliderTrisInit->elements[i].dim.vtx[j], this->dyna.actor.home.rot.y);
            Math_Vec3f_Sum(&pos[j], &this->dyna.actor.world.pos, &pos[j]);
        }

        Collider_SetTrisVertices(colliderTris, i, &pos[0], &pos[1], &pos[2]);
    }
}

Actor* ObjSwitch_SpawnIce(ObjSwitch* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    return Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_OBJ_ICE_POLY, thisx->world.pos.x,
                              thisx->world.pos.y, thisx->world.pos.z, thisx->world.rot.x, thisx->world.rot.y,
                              thisx->world.rot.z, (this->dyna.actor.params >> 8 & 0x3F) << 8);
}

void ObjSwitch_SetOn(ObjSwitch* this, PlayState* play) {
    s32 pad;
    s32 subType;

    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
        this->cooldownOn = false;
    } else {
        subType = (this->dyna.actor.params >> 4 & 7);
        Flags_SetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F));

        if (subType == 0 || subType == 4) {
            OnePointCutscene_AttentionSetSfx(play, &this->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        } else {
            OnePointCutscene_AttentionSetSfx(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }

        this->cooldownOn = true;
    }
}

void ObjSwitch_SetOff(ObjSwitch* this, PlayState* play) {
    this->cooldownOn = false;

    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
        Flags_UnsetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F));

        if ((this->dyna.actor.params >> 4 & 7) == 1) {
            OnePointCutscene_AttentionSetSfx(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
            this->cooldownOn = true;
        }
    }
}

void ObjSwitch_UpdateTwoTexScrollXY(ObjSwitch* this) {
    this->x1TexScroll = (this->x1TexScroll - 1) & 0x7F;
    this->y1TexScroll = (this->y1TexScroll + 1) & 0x7F;
    this->x2TexScroll = (this->x2TexScroll + 1) & 0x7F;
    this->y2TexScroll = (this->y2TexScroll - 1) & 0x7F;
}

void ObjSwitch_Init(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;
    s32 switchFlagSet;
    s32 type;

    switchFlagSet = Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F));
    type = (this->dyna.actor.params & 7);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        ObjSwitch_InitDynapoly(this, play, &gFloorSwitchCol, DPM_PLAYER);
    }

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 1.0f;
    }

    Actor_SetFocus(&this->dyna.actor, sHeights[type]);

    if (type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        ObjSwitch_InitTrisCollider(this, play, &sRustyFloorTrisInit);
    } else if (type == OBJSWITCH_TYPE_EYE) {
        ObjSwitch_InitTrisCollider(this, play, &trisColliderEye);
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        ObjSwitch_InitJntSphCollider(this, play, &sCyrstalJntSphereInit);
    }

    if (type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        this->dyna.actor.flags |= ACTOR_FLAG_0;
        this->dyna.actor.targetMode = 4;
    }

    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    if ((this->dyna.actor.params >> 7 & 1) && (ObjSwitch_SpawnIce(this, play) == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Error : 氷発生失敗 (%s %d)\n", __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
        this->dyna.actor.params &= ~0x80;
    }

    if (this->dyna.actor.params >> 7 & 1) {
        ObjSwitch_EyeFrozenInit(this);
    } else if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        if (switchFlagSet) {
            ObjSwitch_FloorDownInit(this);
        } else {
            ObjSwitch_FloorUpInit(this);
        }
    } else if (type == OBJSWITCH_TYPE_EYE) {
        if (switchFlagSet) {
            ObjSwitch_EyeClosedInit(this);
        } else {
            ObjSwitch_EyeOpenInit(this);
        }
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        if (switchFlagSet) {
            ObjSwitch_CrystalOnInit(this);
        } else {
            ObjSwitch_CrystalOffInit(this);
        }
    }

    osSyncPrintf("(Dungeon switch)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void ObjSwitch_Destroy(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;

    switch ((this->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
            break;
    }

    switch ((this->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
        case OBJSWITCH_TYPE_EYE:
            Collider_DestroyTris(play, &this->tris.col);
            break;
        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            Collider_DestroyJntSph(play, &this->jntSph.col);
            break;
    }
}

void ObjSwitch_FloorUpInit(ObjSwitch* this) {
    this->dyna.actor.scale.y = 33.0f / 200.0f;
    this->actionFunc = ObjSwitch_FloorUp;
}

void ObjSwitch_FloorUp(ObjSwitch* this, PlayState* play) {
    if ((this->dyna.actor.params & 7) == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        if (this->tris.col.base.acFlags & AC_HIT) {
            ObjSwitch_FloorPressInit(this);
            ObjSwitch_SetOn(this, play);
            this->tris.col.base.acFlags &= ~AC_HIT;
        } else {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->tris.col.base);
        }
    } else {
        switch ((this->dyna.actor.params >> 4 & 7)) {
            case OBJSWITCH_SUBTYPE_FLOOR_0:
                if (func_8004356C(&this->dyna)) {
                    ObjSwitch_FloorPressInit(this);
                    ObjSwitch_SetOn(this, play);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_1:
                if ((this->dyna.unk_160 & 2) && !(this->unk_17F & 2)) {
                    ObjSwitch_FloorPressInit(this);
                    ObjSwitch_SetOn(this, play);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_2:
                if (func_800435B4(&this->dyna)) {
                    ObjSwitch_FloorPressInit(this);
                    ObjSwitch_SetOn(this, play);
                }
                break;
            case OBJSWITCH_SUBTYPE_FLOOR_3:
                if (func_800435B4(&this->dyna)) {
                    ObjSwitch_FloorPressInit(this);
                    ObjSwitch_SetOff(this, play);
                }
                break;
        }
    }
}

void ObjSwitch_FloorPressInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_FloorPress;
    this->cooldownTimer = 100;
}

void ObjSwitch_FloorPress(ObjSwitch* this, PlayState* play) {
    if ((this->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_FLOOR_3 || !this->cooldownOn ||
        func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->dyna.actor.scale.y -= 99.0f / 2000.0f;
        if (this->dyna.actor.scale.y <= 33.0f / 2000.0f) {
            ObjSwitch_FloorDownInit(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
        }
    }
}

void ObjSwitch_FloorDownInit(ObjSwitch* this) {
    this->dyna.actor.scale.y = 33.0f / 2000.0f;
    this->releaseTimer = 6;
    this->actionFunc = ObjSwitch_FloorDown;
}

void ObjSwitch_FloorDown(ObjSwitch* this, PlayState* play) {
    switch ((this->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_FLOOR_0:
            if (!Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_FloorReleaseInit(this);
            }
            break;
        case OBJSWITCH_SUBTYPE_FLOOR_1:
            if ((this->dyna.unk_160 & 2) && !(this->unk_17F & 2)) {
                ObjSwitch_FloorReleaseInit(this);
                ObjSwitch_SetOff(this, play);
            }
            break;
        case OBJSWITCH_SUBTYPE_FLOOR_2:
        case OBJSWITCH_SUBTYPE_FLOOR_3:
            if (!func_800435B4(&this->dyna) && !Player_InCsMode(play)) {
                if (this->releaseTimer <= 0) {
                    ObjSwitch_FloorReleaseInit(this);
                    if ((this->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_FLOOR_2) {
                        ObjSwitch_SetOff(this, play);
                    } else {
                        ObjSwitch_SetOn(this, play);
                    }
                }
            } else {
                this->releaseTimer = 6;
            }
            break;
    }
}

void ObjSwitch_FloorReleaseInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_FloorRelease;
    this->cooldownTimer = 100;
}

void ObjSwitch_FloorRelease(ObjSwitch* this, PlayState* play) {
    s16 subType = (this->dyna.actor.params >> 4 & 7);

    if (((subType != OBJSWITCH_SUBTYPE_FLOOR_1) && (subType != OBJSWITCH_SUBTYPE_FLOOR_3)) || !this->cooldownOn ||
        func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->dyna.actor.scale.y += 99.0f / 2000.0f;
        if (this->dyna.actor.scale.y >= 33.0f / 200.0f) {
            ObjSwitch_FloorUpInit(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            if (subType == OBJSWITCH_SUBTYPE_FLOOR_1) {
                func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
            }
        }
    }
}

s32 ObjSwitch_EyeIsHit(ObjSwitch* this) {
    Actor* collidingActor;
    s16 yawDiff;

    if ((this->tris.col.base.acFlags & AC_HIT) && !(this->unk_17F & 2)) {
        collidingActor = this->tris.col.base.ac;
        if (collidingActor != NULL) {
            yawDiff = collidingActor->world.rot.y - this->dyna.actor.shape.rot.y;
            if (ABS(yawDiff) > 0x5000) {
                return 1;
            }
        }
    }
    return 0;
}

void ObjSwitch_EyeFrozenInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_EyeInit;
}

void ObjSwitch_EyeInit(ObjSwitch* this, PlayState* play) {
    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
        ObjSwitch_EyeClosedInit(this);
    } else {
        ObjSwitch_EyeOpenInit(this);
    }
}

void ObjSwitch_EyeOpenInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_EyeOpen;
    this->eyeTexIndex = 0;
}

void ObjSwitch_EyeOpen(ObjSwitch* this, PlayState* play) {
    if (ObjSwitch_EyeIsHit(this) || (this->dyna.actor.params >> 7 & 1)) {
        ObjSwitch_EyeClosingInit(this);
        ObjSwitch_SetOn(this, play);
        this->dyna.actor.params &= ~0x80;
    }
}

void ObjSwitch_EyeClosingInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_EyeClosing;
    this->cooldownTimer = 100;
}

void ObjSwitch_EyeClosing(ObjSwitch* this, PlayState* play) {
    if (!this->cooldownOn || func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            ObjSwitch_EyeClosedInit(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjSwitch_EyeClosedInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_EyeClosed;
    this->eyeTexIndex = 3;
}

void ObjSwitch_EyeClosed(ObjSwitch* this, PlayState* play) {
    switch ((this->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_EYE_0:
            if (!Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_EyeOpeningInit(this);
                this->dyna.actor.params &= ~0x80;
            }
            break;
        case OBJSWITCH_SUBTYPE_EYE_1:
            if (ObjSwitch_EyeIsHit(this) || (this->dyna.actor.params >> 7 & 1)) {
                ObjSwitch_EyeOpeningInit(this);
                ObjSwitch_SetOff(this, play);
                this->dyna.actor.params &= ~0x80;
            }
            break;
    }
}

void ObjSwitch_EyeOpeningInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_EyeOpening;
    this->cooldownTimer = 100;
}

void ObjSwitch_EyeOpening(ObjSwitch* this, PlayState* play) {
    if ((this->dyna.actor.params >> 4 & 7) != OBJSWITCH_SUBTYPE_EYE_1 || !this->cooldownOn ||
        func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->eyeTexIndex--;
        if (this->eyeTexIndex <= 0) {
            ObjSwitch_EyeOpenInit(this);
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void ObjSwitch_CrystalOffInit(ObjSwitch* this) {
    this->crystalColor.r = 0;
    this->crystalColor.g = 0;
    this->crystalColor.b = 0;
    this->crystalSubtype1texture = gCrstalSwitchRedTex;
    this->actionFunc = ObjSwitch_CrystalOff;
}

void ObjSwitch_CrystalOff(ObjSwitch* this, PlayState* play) {
    switch ((this->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_CRYSTAL_0:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                ObjSwitch_SetOn(this, play);
                ObjSwitch_CrystalTurnOnInit(this);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_4:
            if (((this->jntSph.col.base.acFlags & AC_HIT) && this->disableAcTimer <= 0) ||
                Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
                this->disableAcTimer = 10;
                ObjSwitch_SetOn(this, play);
                ObjSwitch_CrystalTurnOnInit(this);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_1:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && !(this->unk_17F & 2) && this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                ObjSwitch_SetOn(this, play);
                ObjSwitch_CrystalTurnOnInit(this);
            }
            ObjSwitch_UpdateTwoTexScrollXY(this);
            break;
    }
}

void ObjSwitch_CrystalTurnOnInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_CrystalTurnOn;
    this->cooldownTimer = 100;
}

void ObjSwitch_CrystalTurnOn(ObjSwitch* this, PlayState* play) {
    if (!this->cooldownOn || func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        ObjSwitch_CrystalOnInit(this);
        if ((this->dyna.actor.params >> 4 & 7) == OBJSWITCH_SUBTYPE_CRYSTAL_1) {
            ObjSwitch_UpdateTwoTexScrollXY(this);
        }
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void ObjSwitch_CrystalOnInit(ObjSwitch* this) {
    this->crystalColor.r = 255;
    this->crystalColor.g = 255;
    this->crystalColor.b = 255;
    this->crystalSubtype1texture = gCrstalSwitchBlueTex;
    this->actionFunc = ObjSwitch_CrystalOn;
}

void ObjSwitch_CrystalOn(ObjSwitch* this, PlayState* play) {
    switch ((this->dyna.actor.params >> 4 & 7)) {
        case OBJSWITCH_SUBTYPE_CRYSTAL_0:
        case OBJSWITCH_SUBTYPE_CRYSTAL_4:
            if (!Flags_GetSwitch(play, (this->dyna.actor.params >> 8 & 0x3F))) {
                ObjSwitch_CrystalTurnOffInit(this);
            }
            break;
        case OBJSWITCH_SUBTYPE_CRYSTAL_1:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && !(this->unk_17F & 2) && this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                play = play;
                ObjSwitch_CrystalTurnOffInit(this);
                ObjSwitch_SetOff(this, play);
            }
            break;
    }
    ObjSwitch_UpdateTwoTexScrollXY(this);
}

void ObjSwitch_CrystalTurnOffInit(ObjSwitch* this) {
    this->actionFunc = ObjSwitch_CrystalTurnOff;
    this->cooldownTimer = 100;
}

void ObjSwitch_CrystalTurnOff(ObjSwitch* this, PlayState* play) {
    if ((this->dyna.actor.params >> 4 & 7) != OBJSWITCH_SUBTYPE_CRYSTAL_1 || !this->cooldownOn ||
        func_8005B198() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        ObjSwitch_CrystalOffInit(this);
        ObjSwitch_UpdateTwoTexScrollXY(this);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void ObjSwitch_Update(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;

    if (this->releaseTimer > 0) {
        this->releaseTimer--;
    }
    if (this->cooldownTimer > 0) {
        this->cooldownTimer--;
    }

    this->actionFunc(this, play);

    switch ((this->dyna.actor.params & 7)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            this->unk_17F = this->dyna.unk_160;
            break;
        case OBJSWITCH_TYPE_EYE:
            this->unk_17F = this->tris.col.base.acFlags;
            this->tris.col.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->tris.col.base);
            break;
        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            if (!Player_InCsMode(play) && this->disableAcTimer > 0) {
                this->disableAcTimer--;
            }
            this->unk_17F = this->jntSph.col.base.acFlags;
            this->jntSph.col.base.acFlags &= ~AC_HIT;
            if (this->disableAcTimer <= 0) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->jntSph.col.base);
            }
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->jntSph.col.base);
            break;
    }
}

void ObjSwitch_DrawFloor(ObjSwitch* this, PlayState* play) {
    static Gfx* floorSwitchDLists[] = { gFloorSwitch1DL, gFloorSwitch3DL, gFloorSwitch2DL, gFloorSwitch2DL };

    Gfx_DrawDListOpa(play, floorSwitchDLists[(this->dyna.actor.params >> 4 & 7)]);
}

void ObjSwitch_DrawFloorRusty(ObjSwitch* this, PlayState* play) {
    Gfx_DrawDListOpa(play, gRustyFloorSwitchDL);
}

void ObjSwitch_DrawEye(ObjSwitch* this, PlayState* play) {
    static void* eyeTextures[][4] = {
        { gEyeSwitchGoldOpenTex, gEyeSwitchGoldOpeningTex, gEyeSwitchGoldClosingTex, gEyeSwitchGoldClosedTex },
        { gEyeSwitchSilverOpenTex, gEyeSwitchSilverHalfTex, gEyeSwitchSilverClosedTex, gEyeSwitchSilverClosedTex },
    };
    static Gfx* eyeDlists[] = { gEyeSwitch1DL, gEyeSwitch2DL };
    s32 pad;
    s32 subType = (this->dyna.actor.params >> 4 & 7);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[subType][this->eyeTexIndex]));
    gSPDisplayList(POLY_OPA_DISP++, eyeDlists[subType]);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjSwitch_DrawCrystal(ObjSwitch* this, PlayState* play) {
    static Gfx* xluDLists[] = { gCrystalSwitchCoreXluDL, gCrystalSwitchDiamondXluDL, NULL, NULL,
                                gCrystalSwitchCoreXluDL };
    static Gfx* opaDLists[] = { gCrystalSwitchCoreOpaDL, gCrystalSwitchDiamondOpaDL, NULL, NULL,
                                gCrystalSwitchCoreOpaDL };
    s32 pad1;
    s32 pad2;
    s32 subType;

    subType = (this->dyna.actor.params >> 4 & 7);
    func_8002ED80(&this->dyna.actor, play, 0);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D84(play->state.gfxCtx);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, xluDLists[subType]);

    CLOSE_DISPS(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (subType == OBJSWITCH_SUBTYPE_CRYSTAL_1) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(this->crystalSubtype1texture));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, this->crystalColor.r, this->crystalColor.g, this->crystalColor.b, 128);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, this->x1TexScroll, this->y1TexScroll, 0x20, 0x20, 1,
                                this->x2TexScroll, this->y2TexScroll, 0x20, 0x20));
    gSPDisplayList(POLY_OPA_DISP++, opaDLists[subType]);

    CLOSE_DISPS(play->state.gfxCtx);
}

static ObjSwitchActionFunc sDrawFuncs[] = {
    ObjSwitch_DrawFloor, ObjSwitch_DrawFloorRusty, ObjSwitch_DrawEye, ObjSwitch_DrawCrystal, ObjSwitch_DrawCrystal,
};

void ObjSwitch_Draw(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;

    sDrawFuncs[(this->dyna.actor.params & 7)](this, play);
}

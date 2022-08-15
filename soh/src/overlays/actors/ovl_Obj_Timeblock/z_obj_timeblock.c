/*
 * File: z_obj_timeblock.c
 * Overlay: ovl_Obj_Timeblock
 * Description: Song of Time Block
 */

#include "z_obj_timeblock.h"
#include "objects/object_timeblock/object_timeblock.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_4 | ACTOR_FLAG_25 | ACTOR_FLAG_27)

void ObjTimeblock_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Destroy(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Update(Actor* thisx, GlobalContext* globalCtx);
void ObjTimeblock_Draw(Actor* thisx, GlobalContext* globalCtx);

void ObjTimeblock_SetupNormal(ObjTimeblock* this);
void ObjTimeblock_SetupAltBehaviorVisible(ObjTimeblock* this);
void ObjTimeblock_SetupAltBehaviourNotVisible(ObjTimeblock* this);

s32 ObjTimeblock_WaitForOcarina(ObjTimeblock* this, GlobalContext* globalCtx);
s32 ObjTimeblock_WaitForSong(ObjTimeblock* this, GlobalContext* globalCtx);
void ObjTimeblock_DoNothing(ObjTimeblock* this, GlobalContext* globalCtx);
void ObjTimeblock_Normal(ObjTimeblock* this, GlobalContext* globalCtx);
void ObjTimeblock_AltBehaviorVisible(ObjTimeblock* this, GlobalContext* globalCtx);
void ObjTimeblock_AltBehaviourNotVisible(ObjTimeblock* this, GlobalContext* globalCtx);

const ActorInit Obj_Timeblock_InitVars = {
    ACTOR_OBJ_TIMEBLOCK,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_TIMEBLOCK,
    sizeof(ObjTimeblock),
    (ActorFunc)ObjTimeblock_Init,
    (ActorFunc)ObjTimeblock_Destroy,
    (ActorFunc)ObjTimeblock_Update,
    (ActorFunc)ObjTimeblock_Draw,
    NULL,
};

typedef struct {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 height;
    /* 0x08 */ s16 demoEffectParams;
} ObjTimeblockSizeOptions; // size = 0x0C

static ObjTimeblockSizeOptions sSizeOptions[] = {
    { 1.0, 60.0, 0x0018 },
    { 0.60, 40.0, 0x0019 },
};

static f32 sRanges[] = { 60.0, 100.0, 140.0, 180.0, 220.0, 260.0, 300.0, 300.0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 sPrimColors[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

u32 ObjTimeblock_CalculateIsVisible(ObjTimeblock* this) {
    if (!((this->dyna.actor.params >> 10) & 1)) {
        if (this->unk_177 == 0) {
            return this->unk_175;
        } else {
            u8 temp = ((this->dyna.actor.params >> 15) & 1) ? true : false;

            if (this->unk_177 == 1) {
                return this->unk_174 ^ temp;
            } else {
                u8 linkIsYoung = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? true : false;

                return this->unk_174 ^ temp ^ linkIsYoung;
            }
        }
    } else {
        return (((this->dyna.actor.params >> 15) & 1) ? true : false) ^ this->unk_174;
    }
}

void ObjTimeblock_SpawnDemoEffect(ObjTimeblock* this, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_EFFECT, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, 0, 0,
                sSizeOptions[(this->dyna.actor.params >> 8) & 1].demoEffectParams);
}

void ObjTimeblock_ToggleSwitchFlag(GlobalContext* globalCtx, s32 flag) {
    if (Flags_GetSwitch(globalCtx, flag)) {
        Flags_UnsetSwitch(globalCtx, flag);
    } else {
        Flags_SetSwitch(globalCtx, flag);
    }
}

void ObjTimeblock_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjTimeblock* this = (ObjTimeblock*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    CollisionHeader_GetVirtual(&gSongOfTimeBlockCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, sSizeOptions[(this->dyna.actor.params >> 8) & 1].scale);

    if ((this->dyna.actor.params >> 6) & 1) {
        this->unk_177 = 0;
    } else {
        this->unk_177 = ((this->dyna.actor.params & 0x3F) < 0x38) ? 2 : 1;
    }

    this->songObserverFunc = ObjTimeblock_WaitForOcarina;

    Actor_SetFocus(&this->dyna.actor, sSizeOptions[(this->dyna.actor.params >> 8) & 1].height);

    this->unk_174 = (Flags_GetSwitch(globalCtx, this->dyna.actor.params & 0x3F)) ? true : false;
    this->unk_175 = ((this->dyna.actor.params >> 15) & 1) ? true : false;
    this->isVisible = ObjTimeblock_CalculateIsVisible(this);

    if (!((this->dyna.actor.params >> 10) & 1)) {
        ObjTimeblock_SetupNormal(this);
    } else if (this->isVisible) {
        ObjTimeblock_SetupAltBehaviorVisible(this);
    } else {
        ObjTimeblock_SetupAltBehaviourNotVisible(this);
    }

    // "Block of time"
    osSyncPrintf("時のブロック (<arg> %04xH <type> save:%d color:%d range:%d move:%d)\n", (u16)this->dyna.actor.params,
                 this->unk_177, this->dyna.actor.home.rot.z & 7, (this->dyna.actor.params >> 11) & 7,
                 (this->dyna.actor.params >> 10) & 1);
}

void ObjTimeblock_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    ObjTimeblock* this = (ObjTimeblock*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

u8 ObjTimeblock_PlayerIsInRange(ObjTimeblock* this, GlobalContext* globalCtx) {
    if (this->isVisible && func_80043590(&this->dyna)) {
        return false;
    }

    if (this->dyna.actor.xzDistToPlayer <= sRanges[(this->dyna.actor.params >> 11) & 7]) {
        Vec3f distance;
        f32 blockSize;

        func_8002DBD0(&this->dyna.actor, &distance, &GET_PLAYER(globalCtx)->actor.world.pos);
        blockSize = this->dyna.actor.scale.x * 50.0f + 6.0f;
        // Return true if player's xz position is not inside the block
        if (blockSize < fabsf(distance.x) || blockSize < fabsf(distance.z)) {
            return true;
        }
    }

    return false;
}

s32 ObjTimeblock_WaitForOcarina(ObjTimeblock* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (ObjTimeblock_PlayerIsInRange(this, globalCtx)) {
        if (player->stateFlags2 & 0x1000000) {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
            this->songObserverFunc = ObjTimeblock_WaitForSong;
        } else {
            player->stateFlags2 |= 0x800000;
        }
    }
    return false;
}

s32 ObjTimeblock_WaitForSong(ObjTimeblock* this, GlobalContext* globalCtx) {
    if (globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->songObserverFunc = ObjTimeblock_WaitForOcarina;
    }
    if (globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_TIME ||
        (CVar_GetS32("gQuickOcarina", 0) != 0 && CHECK_QUEST_ITEM(QUEST_SONG_TIME) &&
         globalCtx->msgCtx.ocarinaAction == 1)) {
        if (this->unk_172 == 254) {
            this->songEndTimer = 110;
        } else {
            this->songEndTimer--;
            if (this->songEndTimer == 0) || (CVar_GetS32("gQuickOcarina", 0) != 0 && CHECK_QUEST_ITEM(QUEST_SONG_TIME) &&
                globalCtx->msgCtx.ocarinaAction == 1)) {
                return true;
            }
        }
    }
    return false;
}

void ObjTimeblock_SetupDoNothing(ObjTimeblock* this) {
    this->actionFunc = ObjTimeblock_DoNothing;
}

void ObjTimeblock_DoNothing(ObjTimeblock* this, GlobalContext* globalCtx) {
}

void ObjTimeblock_SetupNormal(ObjTimeblock* this) {
    this->actionFunc = ObjTimeblock_Normal;
}

void ObjTimeblock_Normal(ObjTimeblock* this, GlobalContext* globalCtx) {
    u32 newIsVisible;

    if (this->songObserverFunc(this, globalCtx) && this->demoEffectTimer <= 0) {
        ObjTimeblock_SpawnDemoEffect(this, globalCtx);
        this->demoEffectTimer = 160;

        // Possibly points the camera to this actor
        OnePointCutscene_Attention(globalCtx, &this->dyna.actor);
        // "◯◯◯◯ Time Block Attention Camera (frame counter  %d)\n"
        osSyncPrintf("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", globalCtx->state.frames);

        this->demoEffectFirstPartTimer = 12;

        if (this->unk_177 == 0) {
            this->dyna.actor.params ^= 0x8000;
        } else {
            ObjTimeblock_ToggleSwitchFlag(globalCtx, this->dyna.actor.params & 0x3F);
        }
    }

    this->unk_172 = globalCtx->msgCtx.lastPlayedSong;
    if (this->demoEffectFirstPartTimer > 0) {
        this->demoEffectFirstPartTimer--;
        if (this->demoEffectFirstPartTimer == 0) {
            if (this->unk_177 == 0) {
                this->unk_175 = ((this->dyna.actor.params >> 15) & 1) ? true : false;
            } else {
                this->unk_174 = (Flags_GetSwitch(globalCtx, this->dyna.actor.params & 0x3F)) ? true : false;
            }
        }
    }

    newIsVisible = ObjTimeblock_CalculateIsVisible(this);
    if (this->unk_177 == 1 && newIsVisible != this->isVisible) {
        ObjTimeblock_SetupDoNothing(this);
    }
    this->isVisible = newIsVisible;

    if (this->demoEffectTimer == 50) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void func_80BA06AC(ObjTimeblock* this, GlobalContext* globalCtx) {
    s32 switchFlag = this->dyna.actor.params & 0x3F;

    this->unk_172 = globalCtx->msgCtx.lastPlayedSong;

    if (this->demoEffectFirstPartTimer > 0 && --this->demoEffectFirstPartTimer == 0) {
        this->unk_174 = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;
    }

    this->isVisible = ObjTimeblock_CalculateIsVisible(this);
    this->unk_176 = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;
}

void ObjTimeblock_SetupAltBehaviorVisible(ObjTimeblock* this) {
    this->actionFunc = ObjTimeblock_AltBehaviorVisible;
}

void ObjTimeblock_AltBehaviorVisible(ObjTimeblock* this, GlobalContext* globalCtx) {
    if (this->songObserverFunc(this, globalCtx) && this->demoEffectTimer <= 0) {
        this->demoEffectFirstPartTimer = 12;
        ObjTimeblock_SpawnDemoEffect(this, globalCtx);
        this->demoEffectTimer = 160;
        OnePointCutscene_Attention(globalCtx, &this->dyna.actor);
        // "Time Block Attention Camera (frame counter)"
        osSyncPrintf("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", globalCtx->state.frames);
        ObjTimeblock_ToggleSwitchFlag(globalCtx, this->dyna.actor.params & 0x3F);
    }

    func_80BA06AC(this, globalCtx);

    if (this->demoEffectTimer == 50) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    }

    if (!this->isVisible && this->demoEffectTimer <= 0) {
        ObjTimeblock_SetupAltBehaviourNotVisible(this);
    }
}

void ObjTimeblock_SetupAltBehaviourNotVisible(ObjTimeblock* this) {
    this->actionFunc = ObjTimeblock_AltBehaviourNotVisible;
}

void ObjTimeblock_AltBehaviourNotVisible(ObjTimeblock* this, GlobalContext* globalCtx) {
    s32 switchFlag = this->dyna.actor.params & 0x3F;
    s8 switchFlagIsSet = (Flags_GetSwitch(globalCtx, switchFlag)) ? true : false;

    if (this->unk_176 ^ switchFlagIsSet && switchFlagIsSet ^ (((this->dyna.actor.params >> 15) & 1) ? true : false)) {
        if (this->demoEffectTimer <= 0) {
            ObjTimeblock_SpawnDemoEffect(this, globalCtx);
            this->demoEffectTimer = 160;
        }
        this->demoEffectFirstPartTimer = 12;
    }

    func_80BA06AC(this, globalCtx);

    if (this->isVisible && this->demoEffectTimer <= 0) {
        ObjTimeblock_SetupAltBehaviorVisible(this);
    }
}

void ObjTimeblock_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjTimeblock* this = (ObjTimeblock*)thisx;

    this->actionFunc(this, globalCtx);

    if (this->demoEffectTimer > 0) {
        this->demoEffectTimer--;
    }

    if (this->isVisible) {
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    } else {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    }
}

void ObjTimeblock_Draw(Actor* thisx, GlobalContext* globalCtx) {
    if (((ObjTimeblock*)thisx)->isVisible) {
        Color_RGB8* primColor = &sPrimColors[thisx->home.rot.z & 7];

        OPEN_DISPS(globalCtx->state.gfxCtx);

        func_80093D18(globalCtx->state.gfxCtx);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, primColor->r, primColor->g, primColor->b, 255);
        gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

        CLOSE_DISPS(globalCtx->state.gfxCtx);
    }
}

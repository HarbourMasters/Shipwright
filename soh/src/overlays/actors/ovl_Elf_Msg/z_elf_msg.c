/*
 * File: z_elf_msg.c
 * Overlay: ovl_Elf_Msg
 * Description: Readable Navi call spot
 */

#include "z_elf_msg.h"
#include "vt.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"

#define FLAGS ACTOR_FLAG_4

void ElfMsg_Init(Actor* thisx, PlayState* play);
void ElfMsg_Destroy(Actor* thisx, PlayState* play);
void ElfMsg_Update(Actor* thisx, PlayState* play);
void ElfMsg_Draw(Actor* thisx, PlayState* play);

void ElfMsg_CallNaviCuboid(ElfMsg* this, PlayState* play);
void ElfMsg_CallNaviCylinder(ElfMsg* this, PlayState* play);

const ActorInit Elf_Msg_InitVars = {
    ACTOR_ELF_MSG,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ElfMsg),
    (ActorFunc)ElfMsg_Init,
    (ActorFunc)ElfMsg_Destroy,
    (ActorFunc)ElfMsg_Update,
    (ActorFunc)ElfMsg_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_STOP),
};

void ElfMsg_SetupAction(ElfMsg* this, ElfMsgActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Checks a scene flag - if flag is set, the actor is killed and function returns 1. Otherwise returns 0.
 * Can also set a switch flag from params while killing.
 */
s32 ElfMsg_KillCheck(ElfMsg* this, PlayState* play) {
    if ((this->actor.world.rot.y > 0) && (this->actor.world.rot.y < 0x41) &&
        Flags_GetSwitch(play, this->actor.world.rot.y - 1)) {
        LOG_STRING("共倒れ"); // "Mutual destruction"
        if (((this->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(play, (this->actor.params >> 8) & 0x3F);
        }
        Actor_Kill(&this->actor);
        return 1;
    } else if ((this->actor.world.rot.y == -1) && Flags_GetClear(play, this->actor.room)) {
        LOG_STRING("共倒れ"); // "Mutual destruction"
        if (((this->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(play, (this->actor.params >> 8) & 0x3F);
        }
        Actor_Kill(&this->actor);
        return 1;
    } else if (((this->actor.params >> 8) & 0x3F) == 0x3F) {
        return 0;
    } else if (Flags_GetSwitch(play, (this->actor.params >> 8) & 0x3F)) {
        Actor_Kill(&this->actor);
        return 1;
    }
    return 0;
}

void ElfMsg_Init(Actor* thisx, PlayState* play) {
    ElfMsg* this = (ElfMsg*)thisx;

    // "Conditions for Elf Tag disappearing"
    osSyncPrintf(VT_FGCOL(CYAN) "\nエルフ タグ 消える条件 %d" VT_RST "\n", (thisx->params >> 8) & 0x3F);
    osSyncPrintf(VT_FGCOL(CYAN) "\nthisx->shape.angle.sy = %d\n" VT_RST, thisx->shape.rot.y);
    if (thisx->shape.rot.y >= 0x41) {
        // "Conditions for Elf Tag appearing"
        osSyncPrintf(VT_FGCOL(CYAN) "\nエルフ タグ 出現条件 %d" VT_RST "\n", thisx->shape.rot.y - 0x41);
    }

    if (!ElfMsg_KillCheck(this, play)) {
        Actor_ProcessInitChain(thisx, sInitChain);
        if (thisx->world.rot.x == 0) {
            thisx->scale.z = 0.4f;
            thisx->scale.x = 0.4f;
        } else {
            thisx->scale.x = thisx->scale.z = thisx->world.rot.x * 0.04f;
        }

        if (thisx->world.rot.z == 0) {
            thisx->scale.y = 0.4f;
        } else {
            thisx->scale.y = thisx->world.rot.z * 0.04f;
        }

        if (thisx->params & 0x4000) {
            ElfMsg_SetupAction(this, ElfMsg_CallNaviCuboid);
        } else {
            ElfMsg_SetupAction(this, ElfMsg_CallNaviCylinder);
        }

        thisx->shape.rot.x = thisx->shape.rot.y = thisx->shape.rot.z = 0;
    }
}

void ElfMsg_Destroy(Actor* thisx, PlayState* play) {
}

s32 ElfMsg_GetMessageId(ElfMsg* this) {
    // Negative message ID forces link to talk to Navi
    if (this->actor.params & 0x8000 || CVar_GetS32("gNoForcedNavi", 0) != 0) {
        return (this->actor.params & 0xFF) + 0x100;
    } else {
        return -((this->actor.params & 0xFF) + 0x100);
    }
}

void ElfMsg_CallNaviCuboid(ElfMsg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* navi = (EnElf*)player->naviActor;

    if ((fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < (100.0f * this->actor.scale.x)) &&
        (this->actor.world.pos.y <= player->actor.world.pos.y) &&
        ((player->actor.world.pos.y - this->actor.world.pos.y) < (100.0f * this->actor.scale.y)) &&
        (fabsf(player->actor.world.pos.z - this->actor.world.pos.z) < (100.0f * this->actor.scale.z))) {
        player->naviTextId = ElfMsg_GetMessageId(this);
        navi->elfMsg = this;
    }
}

s32 ElfMsg_WithinXZDistance(Vec3f* pos1, Vec3f* pos2, f32 distance) {
    return (SQ(pos2->x - pos1->x) + SQ(pos2->z - pos1->z)) < SQ(distance);
}

void ElfMsg_CallNaviCylinder(ElfMsg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* navi = (EnElf*)player->naviActor;

    // This fixes a crash when using a grotto exit when you never properly entered
    if (navi == NULL)
        return;

    if (ElfMsg_WithinXZDistance(&player->actor.world.pos, &this->actor.world.pos, this->actor.scale.x * 100.0f) &&
        (this->actor.world.pos.y <= player->actor.world.pos.y) &&
        ((player->actor.world.pos.y - this->actor.world.pos.y) < (100.0f * this->actor.scale.y))) {
        player->naviTextId = ElfMsg_GetMessageId(this);
        navi->elfMsg = this;
    }
}

void ElfMsg_Update(Actor* thisx, PlayState* play) {
    ElfMsg* this = (ElfMsg*)thisx;

    if (!ElfMsg_KillCheck(this, play)) {
        if (Actor_ProcessTalkRequest(&this->actor, play)) {
            if (((this->actor.params >> 8) & 0x3F) != 0x3F) {
                Flags_SetSwitch(play, (this->actor.params >> 8) & 0x3F);
            }
            Actor_Kill(&this->actor);
            return;
        }
        if ((this->actor.world.rot.y <= 0x41) || (this->actor.world.rot.y > 0x80) ||
            Flags_GetSwitch(play, this->actor.world.rot.y - 0x41)) {
            this->actionFunc(this, play);
        }
    }
}

#ifdef ZELDA_DEBUG
#include "overlays/ovl_Elf_Msg/ovl_Elf_Msg.h"
#endif

void ElfMsg_Draw(Actor* thisx, PlayState* play) 
{
#ifdef ZELDA_DEBUG
    OPEN_DISPS(play->state.gfxCtx);

    if (R_NAVI_MSG_REGION_ALPHA == 0) {
        return;
    }

    func_80093D18(play->state.gfxCtx);
    if (thisx->params & 0x8000) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 100, 100, R_NAVI_MSG_REGION_ALPHA);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, R_NAVI_MSG_REGION_ALPHA);
    }

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, D_809AD278);

    if (thisx->params & 0x4000) {
        gSPDisplayList(POLY_XLU_DISP++, sCubeDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, sCylinderDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
    #endif
}

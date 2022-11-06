/*
 * File: z_elf_msg2.c
 * Overlay: ovl_Elf_Msg2
 * Description: Targetable Navi check spot
 */

#include "z_elf_msg2.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void ElfMsg2_Init(Actor* thisx, PlayState* play);
void ElfMsg2_Destroy(Actor* thisx, PlayState* play);
void ElfMsg2_Update(Actor* thisx, PlayState* play);
void ElfMsg2_Draw(Actor* thisx, PlayState* play);

s32 ElfMsg2_GetMessageId(ElfMsg2* this);
void ElfMsg2_WaitUntilActivated(ElfMsg2* this, PlayState* play);
void ElfMsg2_WaitForTextRead(ElfMsg2* this, PlayState* play);

const ActorInit Elf_Msg2_InitVars = {
    ACTOR_ELF_MSG2,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ElfMsg2),
    (ActorFunc)ElfMsg2_Init,
    (ActorFunc)ElfMsg2_Destroy,
    (ActorFunc)ElfMsg2_Update,
    (ActorFunc)ElfMsg2_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_STOP),
};

void ElfMsg2_SetupAction(ElfMsg2* this, ElfMsg2ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Checks a scene flag - if flag is set, the actor is killed and function returns 1. Otherwise returns 0.
 * Can also set a switch flag from params while killing.
 */
s32 ElfMsg2_KillCheck(ElfMsg2* this, PlayState* play) {
    if ((this->actor.world.rot.y > 0) && (this->actor.world.rot.y < 0x41) &&
        Flags_GetSwitch(play, this->actor.world.rot.y - 1)) {
        LOG_STRING("共倒れ"); // "Mutual destruction"
        if (((this->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(play, ((this->actor.params >> 8) & 0x3F));
        }
        Actor_Kill(&this->actor);
        return 1;
    } else if ((this->actor.world.rot.y == -1) && Flags_GetClear(play, this->actor.room)) {
        LOG_STRING("共倒れ２"); // "Mutual destruction 2"
        if (((this->actor.params >> 8) & 0x3F) != 0x3F) {
            Flags_SetSwitch(play, ((this->actor.params >> 8) & 0x3F));
        }
        Actor_Kill(&this->actor);
        return 1;
    } else if (((this->actor.params >> 8) & 0x3F) == 0x3F) {
        return 0;
    } else if (Flags_GetSwitch(play, ((this->actor.params >> 8) & 0x3F))) {
        LOG_STRING("共倒れ"); // "Mutual destruction"
        Actor_Kill(&this->actor);
        return 1;
    }
    return 0;
}

void ElfMsg2_Init(Actor* thisx, PlayState* play) {
    ElfMsg2* this = (ElfMsg2*)thisx;

    osSyncPrintf(VT_FGCOL(CYAN) " Elf_Msg2_Actor_ct %04x\n\n" VT_RST, this->actor.params);
    if (!ElfMsg2_KillCheck(this, play)) {
        if ((this->actor.world.rot.x > 0) && (this->actor.world.rot.x < 8)) {
            this->actor.targetMode = this->actor.world.rot.x - 1;
        }
        Actor_ProcessInitChain(thisx, sInitChain);
        if (this->actor.world.rot.y >= 0x41) {
            ElfMsg2_SetupAction(this, ElfMsg2_WaitUntilActivated);
        } else {
            ElfMsg2_SetupAction(this, ElfMsg2_WaitForTextRead);
            this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_18; // Make actor targetable and Navi-checkable
            this->actor.textId = ElfMsg2_GetMessageId(this);
        }
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    }
}

void ElfMsg2_Destroy(Actor* thisx, PlayState* play) {
}

s32 ElfMsg2_GetMessageId(ElfMsg2* this) {
    return (this->actor.params & 0xFF) + 0x100;
}

/**
 * Runs while Navi text is up. Kills the actor upon closing the text box unless rot.z == 1, can also set a switch flag
 * from params.
 */
void ElfMsg2_WaitForTextClose(ElfMsg2* this, PlayState* play) {
    s32 switchFlag;

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        if (this->actor.world.rot.z != 1) {
            Actor_Kill(&this->actor);
            switchFlag = (this->actor.params >> 8) & 0x3F;
            if (switchFlag != 0x3F) {
                Flags_SetSwitch(play, switchFlag);
            }
        } else {
            ElfMsg2_SetupAction(this, ElfMsg2_WaitForTextRead);
        }
    }
}

/**
 * Runs while Navi text is not up.
 */
void ElfMsg2_WaitForTextRead(ElfMsg2* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        ElfMsg2_SetupAction(this, ElfMsg2_WaitForTextClose);
    }
}

/**
 * Idles until a switch flag is set, at which point the actor becomes interactable
 */
void ElfMsg2_WaitUntilActivated(ElfMsg2* this, PlayState* play) {
    // If (y >= 0x41) && (y <= 0x80), Idles until switch flag (actor.world.rot.y - 0x41) is set
    // If (y > 0x80), Idles forever (Bug?)
    if ((this->actor.world.rot.y >= 0x41) && (this->actor.world.rot.y <= 0x80) &&
        (Flags_GetSwitch(play, (this->actor.world.rot.y - 0x41)))) {
        ElfMsg2_SetupAction(this, ElfMsg2_WaitForTextRead);
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_18; // Make actor targetable and Navi-checkable
        this->actor.textId = ElfMsg2_GetMessageId(this);
    }
}

void ElfMsg2_Update(Actor* thisx, PlayState* play) {
    ElfMsg2* this = (ElfMsg2*)thisx;

    if (!ElfMsg2_KillCheck(this, play)) {
        this->actionFunc(this, play);
    }
}

#if ZELDA_DEBUG
#include "overlays/ovl_Elf_Msg2/ovl_Elf_Msg2.h"
#endif

void ElfMsg2_Draw(Actor* thisx, PlayState* play) {
#if ZELDA_DEBUG
    OPEN_DISPS(play->state.gfxCtx);

    if (R_NAVI_MSG_REGION_ALPHA == 0) {
        return;
    }

    func_80093D18(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 100, 255, R_NAVI_MSG_REGION_ALPHA);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, D_809ADC38);
    gSPDisplayList(POLY_XLU_DISP++, sCubeDL);

    CLOSE_DISPS(play->state.gfxCtx);
    #endif
}

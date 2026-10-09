/*
 * File: z_elf_msg.c
 * Overlay: ovl_Elf_Msg
 * Description: Tatl Hint (proximity-activated C-up hint?)
 */

#include "z_elf_msg.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ElfMsg_Init(Actor* thisx, PlayState* play);
void ElfMsg_Destroy(Actor* thisx, PlayState* play);
void ElfMsg_Update(Actor* thisx, PlayState* play);

void ElfMsg_SetupAction(ElfMsg* this, ElfMsgActionFunc actionFunc);
void func_8092E284(ElfMsg* this, PlayState* play);

ActorProfile Elf_Msg_Profile = {
    /**/ ACTOR_ELF_MSG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ElfMsg),
    /**/ ElfMsg_Init,
    /**/ ElfMsg_Destroy,
    /**/ ElfMsg_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

void ElfMsg_SetupAction(ElfMsg* this, ElfMsgActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s32 func_8092DF9C(ElfMsg* this, PlayState* play) {
    if ((this->actor.home.rot.y > 0) && (this->actor.home.rot.y <= 0x80) &&
        Flags_GetSwitch(play, this->actor.home.rot.y - 1)) {
        (void)"共倒れ"; // "Collapse together"
        if (ELFMSG_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
            Flags_SetSwitch(play, ELFMSG_GET_SWITCH_FLAG(&this->actor));
        }
        Actor_Kill(&this->actor);
        return true;
    }
    if (this->actor.home.rot.y == 0x81) {
        if (Flags_GetClear(play, this->actor.room)) {
            if (ELFMSG_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG_GET_SWITCH_FLAG(&this->actor));
            }
            Actor_Kill(&this->actor);
            return true;
        }
    }
    if (ELFMSG_GET_SWITCH_FLAG(&this->actor) == 0x7F) {
        return false;
    }
    if (Flags_GetSwitch(play, ELFMSG_GET_SWITCH_FLAG(&this->actor))) {
        (void)"共倒れ"; // "Collapse together"
        Actor_Kill(&this->actor);
        return true;
    }
    return false;
}

void ElfMsg_Init(Actor* thisx, PlayState* play) {
    ElfMsg* this = (ElfMsg*)thisx;

    if (!func_8092DF9C(this, play)) {
        Actor_ProcessInitChain(&this->actor, sInitChain);
        if (ABS_ALT(this->actor.home.rot.x) == 0) {
            this->actor.scale.x = this->actor.scale.z = 0.4f;
        } else {
            this->actor.scale.x = this->actor.scale.z = ABS_ALT(this->actor.home.rot.x) * 0.04f;
        }
        if (this->actor.home.rot.z == 0) {
            this->actor.scale.y = 0.4f;
        } else {
            this->actor.scale.y = this->actor.home.rot.z * 0.04f;
        }
        ElfMsg_SetupAction(this, func_8092E284);
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    }
}

void ElfMsg_Destroy(Actor* thisx, PlayState* play) {
}

s32 func_8092E1D0(ElfMsg* this) {
    if (ELFMSG_GET_8000(&this->actor) != 0) {
        return ELFMSG_GET_FF(&this->actor) + 0x200;
    } else {
        return -0x200 - ELFMSG_GET_FF(&this->actor);
    }
}

bool func_8092E1FC(ElfMsg* this) {
    return (this->actor.xzDistToPlayer < (100.0f * this->actor.scale.x)) && (this->actor.playerHeightRel >= 0.0f) &&
           (this->actor.playerHeightRel < (100.0f * this->actor.scale.y));
}

void func_8092E284(ElfMsg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* tatl = (EnElf*)player->tatlActor;

    if (GameInteractor_Should(VB_TATL_INTERRUPT_MSG, (player->tatlActor != NULL) && ((func_8092E1FC(this))), this)) {
        player->tatlTextId = func_8092E1D0(this);
        CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        tatl->elfMsg = &this->actor;
        if (this->actor.csId == CS_ID_NONE) {
            this->actor.csId = CS_ID_GLOBAL_TALK;
        }
        if ((player->tatlTextId < 0) && (this->actor.home.rot.x < 0)) {
            if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR) {
                CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
                CutsceneManager_Queue(this->actor.csId);
            } else if (CutsceneManager_IsNext(this->actor.csId)) {
                this->actor.home.rot.x = 0;
                CutsceneManager_Start(this->actor.csId, &this->actor);
                func_800E0348(play->cameraPtrs[CAM_ID_MAIN]);
            } else {
                CutsceneManager_Queue(this->actor.csId);
            }
        }
    }
}

void ElfMsg_Update(Actor* thisx, PlayState* play) {
    ElfMsg* this = (ElfMsg*)thisx;

    if (func_8092DF9C(this, play) == 0) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            if (ELFMSG_GET_SWITCH_FLAG(thisx) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG_GET_SWITCH_FLAG(thisx));
            }
            Actor_Kill(&this->actor);
            return;
        }

        if ((this->actor.home.rot.y >= 0) || (this->actor.home.rot.y < -0x80) ||
            Flags_GetSwitch(play, -1 - this->actor.home.rot.y)) {
            this->actionFunc(this, play);
        }
    }
}

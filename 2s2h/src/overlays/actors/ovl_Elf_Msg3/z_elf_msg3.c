/*
 * File: z_elf_msg3.c
 * Overlay: ovl_Elf_Msg3
 * Description: Tatl Message (Proximity-activated dialogue?)
 */

#include "z_elf_msg3.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ElfMsg3_Init(Actor* thisx, PlayState* play);
void ElfMsg3_Destroy(Actor* thisx, PlayState* play);
void ElfMsg3_Update(Actor* thisx, PlayState* play);

void func_80A2CF7C(ElfMsg3* this, PlayState* play);

ActorProfile Elf_Msg3_Profile = {
    /**/ ACTOR_ELF_MSG3,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ElfMsg3),
    /**/ ElfMsg3_Init,
    /**/ ElfMsg3_Destroy,
    /**/ ElfMsg3_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

void ElfMsg3_SetupAction(ElfMsg3* this, ElfMsg3ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s32 func_80A2CD1C(ElfMsg3* this, PlayState* play) {
    if ((this->actor.home.rot.y > 0) && (this->actor.home.rot.y <= 0x80) &&
        Flags_GetSwitch(play, this->actor.home.rot.y - 1)) {
        (void)"共倒れ"; // "Collapse together"
        if (ELFMSG3_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
            Flags_SetSwitch(play, ELFMSG3_GET_SWITCH_FLAG(&this->actor));
        }
        Actor_Kill(&this->actor);
        return true;
    }
    if (this->actor.home.rot.y == 0x81) {
        if (Flags_GetClear(play, this->actor.room)) {
            if (ELFMSG3_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG3_GET_SWITCH_FLAG(&this->actor));
            }
            Actor_Kill(&this->actor);
            return true;
        }
    }
    if (ELFMSG3_GET_SWITCH_FLAG(&this->actor) == 0x7F) {
        return false;
    }
    if (Flags_GetSwitch(play, ELFMSG3_GET_SWITCH_FLAG(&this->actor))) {
        (void)"共倒れ"; // "Collapse together"
        Actor_Kill(&this->actor);
        return true;
    }
    return false;
}

void ElfMsg3_Init(Actor* thisx, PlayState* play) {
    ElfMsg3* this = (ElfMsg3*)thisx;

    if (!func_80A2CD1C(this, play)) {
        Actor_ProcessInitChain(&this->actor, sInitChain);
        if (ABS_ALT(this->actor.home.rot.x) == 0) {
            this->actor.scale.z = 0.4f;
            this->actor.scale.x = 0.4f;
        } else {
            this->actor.scale.x = this->actor.scale.z = ABS_ALT(this->actor.home.rot.x) * 0.04f;
        }
        if (this->actor.home.rot.z == 0) {
            this->actor.scale.y = 0.4f;
        } else {
            this->actor.scale.y = this->actor.home.rot.z * 0.04f;
        }
        ElfMsg3_SetupAction(this, func_80A2CF7C);
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    }
}

void ElfMsg3_Destroy(Actor* thisx, PlayState* play) {
}

s32 func_80A2CF50(ElfMsg3* this) {
    if (ELFMSG3_GET_8000(&this->actor) != 0) {
        return ELFMSG3_GET_FF(&this->actor) + 0x200;
    } else {
        return -0x200 - ELFMSG3_GET_FF(&this->actor);
    }
}

void func_80A2CF7C(ElfMsg3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* tatl = (EnElf*)player->tatlActor;

    if (GameInteractor_Should(
            VB_TATL_INTERRUPT_MSG3,
            (((((player->tatlActor != NULL) &&
                (fabsf(player->actor.world.pos.x - this->actor.world.pos.x) < (100.0f * this->actor.scale.x))) &&
               (this->actor.world.pos.y <= player->actor.world.pos.y)) &&
              ((player->actor.world.pos.y - this->actor.world.pos.y) < (100.0f * this->actor.scale.y))) &&
             (fabsf(player->actor.world.pos.z - this->actor.world.pos.z) < (100.0f * this->actor.scale.z))),
            this)) {
        player->tatlTextId = func_80A2CF50(this);
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
                CutsceneManager_Start(this->actor.csId, &this->actor);
                func_800E0348(play->cameraPtrs[CAM_ID_MAIN]);
            } else {
                CutsceneManager_Queue(this->actor.csId);
            }
        }
    }
}

void ElfMsg3_Update(Actor* thisx, PlayState* play) {
    ElfMsg3* this = (ElfMsg3*)thisx;

    if (!func_80A2CD1C(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            if (ELFMSG3_GET_SWITCH_FLAG(thisx) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG3_GET_SWITCH_FLAG(thisx));
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

/*
 * File: z_elf_msg4.c
 * Overlay: ovl_Elf_Msg4
 * Description: Tatl Hint (another proximity-activated C-up hint?)
 */

#include "z_elf_msg4.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_Elf_Msg5/z_elf_msg5.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ElfMsg4_Init(Actor* thisx, PlayState* play);
void ElfMsg4_Destroy(Actor* thisx, PlayState* play);
void ElfMsg4_Update(Actor* thisx, PlayState* play);

void func_80AFD668(ElfMsg4* this, PlayState* play);
void func_80AFD770(ElfMsg4* this, PlayState* play);
s32 func_80AFD380(ElfMsg4* this, PlayState* play);
s32 ElfMsg4_GetTextId(ElfMsg4* this);

ActorProfile Elf_Msg4_Profile = {
    /**/ ACTOR_ELF_MSG4,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ElfMsg4),
    /**/ ElfMsg4_Init,
    /**/ ElfMsg4_Destroy,
    /**/ ElfMsg4_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

s32 func_80AFD380(ElfMsg4* this, PlayState* play) {
    if ((this->actor.home.rot.y > 0) && (this->actor.home.rot.y <= 0x80) &&
        Flags_GetSwitch(play, this->actor.home.rot.y - 1)) {
        (void)"共倒れ"; // "Collapse together"
        if (ELFMSG4_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
            Flags_SetSwitch(play, ELFMSG4_GET_SWITCH_FLAG(&this->actor));
        }
        Actor_Kill(&this->actor);
        return true;
    }
    if (this->actor.home.rot.y == 0x81) {
        if (Flags_GetClear(play, this->actor.room)) {
            if (ELFMSG4_GET_SWITCH_FLAG(&this->actor) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG4_GET_SWITCH_FLAG(&this->actor));
            }
            Actor_Kill(&this->actor);
            return true;
        }
    }
    if (ELFMSG4_GET_SWITCH_FLAG(&this->actor) == 0x7F) {
        return false;
    }
    if (Flags_GetSwitch(play, ELFMSG4_GET_SWITCH_FLAG(&this->actor))) {
        (void)"共倒れ"; // "Collapse together"
        Actor_Kill(&this->actor);
        return true;
    }
    return false;
}

void ElfMsg4_Init(Actor* thisx, PlayState* play) {
    ElfMsg4* this = (ElfMsg4*)thisx;

    if (!func_80AFD380(this, play)) {
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
        this->actor.shape.rot.z = 0;
        this->actionFunc = func_80AFD770;
        this->elfMsg5 = NULL;
        this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z;
    }
}

void ElfMsg4_Destroy(Actor* thisx, PlayState* play) {
}

s32 ElfMsg4_GetTextId(ElfMsg4* this) {
    if (ELFMSG4_GET_8000(&this->actor) != 0) {
        return ELFMSG4_GET_TEXT(&this->actor) + 0x200;
    } else {
        return -ELFMSG4_GET_TEXT(&this->actor) - 0x200;
    }
}

bool func_80AFD5E0(ElfMsg4* this) {
    return (this->actor.xzDistToPlayer < (100.0f * this->actor.scale.x)) && (this->actor.playerHeightRel >= 0.0f) &&
           (this->actor.playerHeightRel < (100.0f * this->actor.scale.y));
}

void func_80AFD668(ElfMsg4* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnElf* tatl = (EnElf*)player->tatlActor;

    if (GameInteractor_Should(VB_TATL_INTERRUPT_MSG4, (player->tatlActor != NULL) && func_80AFD5E0(this), this)) {
        player->tatlTextId = ElfMsg4_GetTextId(this);
        CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        tatl->elfMsg = this->elfMsg5;
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

void func_80AFD770(ElfMsg4* this, PlayState* play) {
    Actor* bgActor = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (bgActor != NULL) {
        if ((bgActor->id != ACTOR_ELF_MSG5) || (ELFMSG4_GET_TEXT(&this->actor) != ELFMSG5_GET_FF(bgActor)) ||
            (this->actor.csId != bgActor->csId)) {
            bgActor = bgActor->next;
        } else {
            this->elfMsg5 = bgActor;
            this->actionFunc = func_80AFD668;
            bgActor = bgActor->next;
        }
    }
}

void ElfMsg4_Update(Actor* thisx, PlayState* play) {
    Actor* bgActor;
    ElfMsg4* this = (ElfMsg4*)thisx;

    if (!func_80AFD380(this, play)) {
        bgActor = this->elfMsg5;
        if ((bgActor != NULL) && (bgActor->update == NULL)) {
            Actor_Kill(&this->actor);
            return;
        }

        if ((bgActor != NULL) && Actor_TalkOfferAccepted(bgActor, &play->state)) {
            if (ELFMSG4_GET_SWITCH_FLAG(thisx) != 0x7F) {
                Flags_SetSwitch(play, ELFMSG4_GET_SWITCH_FLAG(thisx));
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

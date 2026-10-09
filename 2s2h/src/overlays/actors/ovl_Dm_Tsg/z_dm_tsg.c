/*
 * File: z_dm_tsg.c
 * Overlay: ovl_Dm_Tsg
 * Description: Handles all the masks scrolling by as Link falls in the intro
 */

#include "z_dm_tsg.h"
#include "objects/object_open_obj/object_open_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmTsg_Init(Actor* thisx, PlayState* play);
void DmTsg_Destroy(Actor* thisx, PlayState* play);
void DmTsg_Update(Actor* thisx, PlayState* play);
void DmTsg_Draw(Actor* thisx, PlayState* play2);

ActorProfile Dm_Tsg_Profile = {
    /**/ ACTOR_DM_TSG,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_OPEN_OBJ,
    /**/ sizeof(DmTsg),
    /**/ DmTsg_Init,
    /**/ DmTsg_Destroy,
    /**/ DmTsg_Update,
    /**/ DmTsg_Draw,
};

void DmTsg_Init(Actor* thisx, PlayState* play) {
    DmTsg* this = (DmTsg*)thisx;
    s32 i;

    if (gSaveContext.save.entrance == ENTRANCE(OPENING_DUNGEON, 0)) {
        Actor_SetScale(&this->actor, 0.1f);
        for (i = 0; i < DMTSG_UNK_SIZE; i++) {
            this->unk_78C[i] = Rand_ZeroOne() * 0xFFFF;
            this->unk_856[i] = TRUNCF_BINANG(Rand_ZeroOne() * 200.0f) + 100;
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void DmTsg_Destroy(Actor* thisx, PlayState* play) {
}

void DmTsg_Update(Actor* thisx, PlayState* play) {
    DmTsg* this = (DmTsg*)thisx;
    s32 cueChannel;
    s16 i;

    for (i = 0; i < DMTSG_UNK_SIZE; i++) {
        this->unk_5F8[i] = (Math_SinS((i % 16U) * this->unk_854) * 60.0f) + 120.0f;
        this->unk_148[i].x = Math_CosS(this->unk_78C[i]) * this->unk_5F8[i];
        this->unk_148[i].y = this->unk_788 + (i * 16);
        this->unk_148[i].z = Math_SinS(this->unk_78C[i]) * this->unk_5F8[i];
        this->unk_78C[i] += this->unk_856[i];
    }

    this->canDraw = false;
    this->unk_854 += 40;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_514)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_514);
        if (play->csCtx.actorCues[cueChannel]->id != 1) {
            this->canDraw = true;
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    }
}

void DmTsg_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DmTsg* this = (DmTsg*)thisx;
    s32 i;
    u32 j;

    if (this->canDraw) {
        for (i = 0, j = 0; i < DMTSG_UNK_SIZE; i++) {
            Matrix_Translate(this->unk_148[i].x + this->actor.world.pos.x, this->unk_148[i].y + this->actor.world.pos.y,
                             this->unk_148[i].z + this->actor.world.pos.z, MTXMODE_NEW);
            Matrix_Scale(0.3f, 0.3f, 0.3f, MTXMODE_APPLY);
            AnimatedMat_DrawStep(play, Lib_SegmentedToVirtual(object_open_obj_Matanimheader_011458), j++);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            Gfx_DrawDListXlu(play, object_open_obj_DL_002D30);
            if (j >= 10) {
                j = 0;
            }
        }
    }
}

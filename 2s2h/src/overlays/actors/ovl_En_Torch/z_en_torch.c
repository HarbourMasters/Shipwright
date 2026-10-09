/*
 * File: z_en_torch.c
 * Overlay: ovl_En_Torch
 * Description: Grotto chest spawner
 */

#include "z_en_torch.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"

#define FLAGS 0x00000000

void EnTorch_Init(Actor* thisx, PlayState* play);

ActorProfile En_Torch_Profile = {
    /**/ ACTOR_EN_TORCH,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTorch),
    /**/ EnTorch_Init,
    /**/ NULL,
    /**/ NULL,
    /**/ NULL,
};

static u8 sChestContents[] = {
    GI_RUPEE_BLUE, GI_RUPEE_RED, GI_RUPEE_PURPLE, GI_RUPEE_SILVER, GI_BOMBCHUS_1, GI_BOMBCHUS_5, GI_BOMBS_1, GI_BOMBS_1,
};

void EnTorch_Init(Actor* thisx, PlayState* play) {
    EnTorch* this = (EnTorch*)thisx;
    s8 returnData = gSaveContext.respawn[RESPAWN_MODE_UNK_3].data;

    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOX, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0,
                ENBOX_PARAMS(ENBOX_TYPE_SMALL, sChestContents[(returnData >> 0x5) & 0x7], returnData));
    Actor_Kill(&this->actor);
}

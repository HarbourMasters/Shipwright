/*
 * File: z_bg_f40_swlift.c
 * Overlay: ovl_Bg_F40_Swlift
 * Description: Unused Stone Tower vertically-oscillating platform
 */

#include "z_bg_f40_swlift.h"
#include "objects/object_f40_obj/object_f40_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgF40Swlift_Init(Actor* thisx, PlayState* play);
void BgF40Swlift_Destroy(Actor* thisx, PlayState* play);
void BgF40Swlift_Update(Actor* thisx, PlayState* play2);
void BgF40Swlift_Draw(Actor* thisx, PlayState* play);

static s32 sSwitchFlags[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
static s32 sHeights[4];

ActorProfile Bg_F40_Swlift_Profile = {
    /**/ ACTOR_BG_F40_SWLIFT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_F40_OBJ,
    /**/ sizeof(BgF40Swlift),
    /**/ BgF40Swlift_Init,
    /**/ BgF40Swlift_Destroy,
    /**/ BgF40Swlift_Update,
    /**/ BgF40Swlift_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 550, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 5000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgF40Swlift_Init(Actor* thisx, PlayState* play) {
    BgF40Swlift* this = (BgF40Swlift*)thisx;
    s32 index;
    s32 pad;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    index = BG_F40_SWLIFT_GET_INDEX(thisx);
    // #region 2S2H [Port] - Change index >= 5 to index >= 4 to avoid OOB array access
    if ((index < 0) || (index >= 4)) { //! @bug An index greater than 3 will cause an out of bounds array access.
        Actor_Kill(&this->dyna.actor);
        return;
    }

    sHeights[index] = this->dyna.actor.world.pos.y;
    sSwitchFlags[index] = BG_F40_SWLIFT_GET_SWITCH_FLAG(thisx);
    if (index != 0) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, &gStoneTowerVerticallyOscillatingPlatformCol);
    this->dyna.actor.params = 0;
}

void BgF40Swlift_Destroy(Actor* thisx, PlayState* play) {
    BgF40Swlift* this = (BgF40Swlift*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void BgF40Swlift_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgF40Swlift* this = (BgF40Swlift*)thisx;
    s32 i;

    for (i = 1; i < ARRAY_COUNT(sSwitchFlags); i++) {
        if ((sSwitchFlags[i] == 0xFF) || !Flags_GetSwitch(play, sSwitchFlags[i])) {
            break;
        }
    }

    i--;
    if (i != this->dyna.actor.params) {
        f32 heightOffset;

        this->dyna.actor.params = -1;
        heightOffset = (sHeights[i] - this->dyna.actor.world.pos.y) * 0.1f;
        if (heightOffset > 0.0f) {
            heightOffset = CLAMP(heightOffset, 0.5f, 15.0f);
        } else {
            heightOffset = CLAMP(heightOffset, -15.0f, -0.5f);
        }

        if (Math_StepToF(&this->dyna.actor.speed, heightOffset, 1.0f) && (fabsf(heightOffset) <= 0.5f)) {
            if (Math_StepToF(&this->dyna.actor.world.pos.y, sHeights[i], fabsf(this->dyna.actor.speed))) {
                this->dyna.actor.params = i;
                this->timer = 48;
                this->dyna.actor.speed = 0.0f;
            }
        } else {
            this->dyna.actor.world.pos.y += this->dyna.actor.speed;
        }
    } else {
        if (this->timer == 0) {
            this->timer = 48;
        }
        this->timer--;
        this->dyna.actor.world.pos.y =
            sHeights[this->dyna.actor.params] + (Math_SinF(this->timer * (M_PIf / 24.0f)) * 5.0f);
    }
}

void BgF40Swlift_Draw(Actor* thisx, PlayState* play) {
    BgF40Swlift* this = (BgF40Swlift*)thisx;

    Gfx_DrawDListOpa(play, gStoneTowerVerticallyOscillatingPlatformDL);
}

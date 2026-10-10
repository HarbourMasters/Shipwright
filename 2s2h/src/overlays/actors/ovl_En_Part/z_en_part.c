/*
 * File: z_en_part.c
 * Overlay: ovl_En_Part
 * Description: Enemy body parts (spawned when dying)
 */

#include "z_en_part.h"
#include "overlays/effects/ovl_Effect_Ss_Dt_Bubble/z_eff_ss_dt_bubble.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPart_Init(Actor* thisx, PlayState* play);
void EnPart_Destroy(Actor* thisx, PlayState* play);
void EnPart_Update(Actor* thisx, PlayState* play);
void EnPart_Draw(Actor* thisx, PlayState* play);

void func_80865390(EnPart* this, PlayState* play);
void func_808654C4(EnPart* this, PlayState* play);

ActorProfile En_Part_Profile = {
    /**/ ACTOR_EN_PART,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnPart),
    /**/ EnPart_Init,
    /**/ EnPart_Destroy,
    /**/ EnPart_Update,
    /**/ EnPart_Draw,
};

void EnPart_Init(Actor* thisx, PlayState* play) {
}

void EnPart_Destroy(Actor* thisx, PlayState* play) {
}

void func_80865390(EnPart* this, PlayState* play) {
    this->actionFuncIndex = 1;
    this->actor.world.rot.y = Rand_ZeroOne() * 20000.0f;
    switch (this->actor.params) {
        case ENPART_TYPE_1:
        case ENPART_TYPE_4:
            this->unk146 += TRUNCF_BINANG(Rand_ZeroOne() * 17.0f) + 5;
            this->actor.velocity.y = Rand_ZeroOne() * 5.0f + 4.0f;
            this->actor.gravity = -0.6f - (Rand_ZeroOne() * 0.5f);
            this->unk14C = 0.15f;
            break;

        case ENPART_TYPE_15:
            this->actor.world.rot.y = this->actor.parent->shape.rot.y + 0x8000;
            this->unk146 = 100;
            this->actor.velocity.y = 7.0f;
            this->actor.speed = 2.0f;
            this->actor.gravity = -1.0f;
            break;

        default:
            break;
    }
}

void func_808654C4(EnPart* this, PlayState* play) {
    s16 effectScale;
    Vec3f effectPos;
    Vec3f effectVelocity = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f pos;

    if (this->actor.params == ENPART_TYPE_15) {
        this->unk146--;
        if (this->unk146 > 0) {
            this->actor.shape.rot.x += 0x3A98;
            this->actor.shape.rot.y = this->actor.shape.rot.y; // Set to itself
            this->actor.shape.rot.z = this->actor.shape.rot.z; // Set to itself
            if (BgCheck_SphVsFirstPoly(&play->colCtx, &this->actor.world.pos, 20.0f)) {
                this->unk146 = 0;
            }
        } else {
            Math_Vec3f_Copy(&pos, &this->actor.world.pos);
            pos.y = this->actor.floorHeight;
            func_800B3030(play, &pos, &gZeroVec3f, &gZeroVec3f, this->actor.scale.y * 1400.0f, 7, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 10, NA_SE_EN_EXTINCT);
            Actor_Kill(&this->actor);
        }
    } else if (this->unk146 <= 0) {
        switch (this->actor.params) {
            case ENPART_TYPE_1:
                func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, this->actor.scale.y * 4000.0f, 7,
                              1);
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 10, NA_SE_EN_EXTINCT);
                break;

            case ENPART_TYPE_4:
                for (i = 7; i >= 0; i--) {
                    effectPos.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
                    effectPos.y = Rand_CenteredFloat(50.0f) +
                                  (this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y));
                    effectPos.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
                    effectVelocity.y = Rand_ZeroOne() + 1.0f;
                    effectScale = Rand_S16Offset(80, 100);
                    EffectSsDtBubble_SpawnColorProfile(play, &effectPos, &effectVelocity, &gZeroVec3f, effectScale, 25,
                                                       DTBUBBLE_COLOR_PROFILE_RED, true);
                }
                break;

            default:
                break;
        }
        Actor_Kill(&this->actor);
    } else {
        this->unk146--;
        this->zRot += this->unk14C;
    }
}

EnPartActionFunc sActionFuncs[] = { func_80865390, func_808654C4 };

void EnPart_Update(Actor* thisx, PlayState* play) {
    EnPart* this = (EnPart*)thisx;

    Actor_MoveWithGravity(&this->actor);

    sActionFuncs[this->actionFuncIndex](this, play);
}

void EnPart_Draw(Actor* thisx, PlayState* play) {
    EnPart* this = (EnPart*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actor.params > ENPART_TYPE_0) {
        Matrix_RotateZF(this->zRot, MTXMODE_APPLY);
    }
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);
    if (this->actor.params == ENPART_TYPE_15) {
        gSPSegment(POLY_OPA_DISP++, 0x0C, gEmptyDL);
    }
    if (this->dList != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, this->dList);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

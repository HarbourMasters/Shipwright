/*
 * File: z_en_syateki_okuta.c
 * Overlay: ovl_En_Syateki_Okuta
 * Description: Shooting Gallery Octorok
 */

#include "z_en_syateki_okuta.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnSyatekiOkuta_Init(Actor* thisx, PlayState* play);
void EnSyatekiOkuta_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiOkuta_Update(Actor* thisx, PlayState* play);
void EnSyatekiOkuta_Draw(Actor* thisx, PlayState* play);

void EnSyatekiOkuta_SetupAttachToShootingGalleryMan(EnSyatekiOkuta* this);
void EnSyatekiOkuta_AttachToShootingGalleryMan(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_SetupDoNothing(EnSyatekiOkuta* this);
void EnSyatekiOkuta_DoNothing(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_Appear(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_SetupFloat(EnSyatekiOkuta* this);
void EnSyatekiOkuta_Float(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_SetupHide(EnSyatekiOkuta* this);
void EnSyatekiOkuta_Hide(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_Die(EnSyatekiOkuta* this, PlayState* play);
void EnSyatekiOkuta_UpdateHeadScale(EnSyatekiOkuta* this);

ActorProfile En_Syateki_Okuta_Profile = {
    /**/ ACTOR_EN_SYATEKI_OKUTA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_OKUTA,
    /**/ sizeof(EnSyatekiOkuta),
    /**/ EnSyatekiOkuta_Init,
    /**/ EnSyatekiOkuta_Destroy,
    /**/ EnSyatekiOkuta_Update,
    /**/ EnSyatekiOkuta_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, -30, { 0, 0, 0 } },
};

typedef enum ShootingGalleryOctorokAnimation {
    /* 0 */ SG_OCTO_ANIM_SHOOT, // unused
    /* 1 */ SG_OCTO_ANIM_DIE,
    /* 2 */ SG_OCTO_ANIM_HIDE,
    /* 3 */ SG_OCTO_ANIM_FLOAT,
    /* 4 */ SG_OCTO_ANIM_APPEAR,
    /* 5 */ SG_OCTO_ANIM_HIT, // unused
    /* 6 */ SG_OCTO_ANIM_MAX
} ShootingGalleryOctorokAnimation;

static AnimationInfo sAnimationInfo[SG_OCTO_ANIM_MAX] = {
    { &gOctorokShootAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },  // SG_OCTO_ANIM_SHOOT
    { &gOctorokDieAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },    // SG_OCTO_ANIM_DIE
    { &gOctorokHideAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },   // SG_OCTO_ANIM_HIDE
    { &gOctorokFloatAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -1.0f },  // SG_OCTO_ANIM_FLOAT
    { &gOctorokAppearAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f }, // SG_OCTO_ANIM_APPEAR
    { &gOctorokHitAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },    // SG_OCTO_ANIM_HIT
};

#include "assets/overlays/ovl_En_Syateki_Okuta/ovl_En_Syateki_Okuta.h"

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_OCTOROK, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6500, ICHAIN_STOP),
};

void EnSyatekiOkuta_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSyatekiOkuta* this = (EnSyatekiOkuta*)thisx;
    WaterBox* waterbox;
    f32 ySurface;
    s32 bgId;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &gOctorokSkel, &gOctorokAppearAnim, this->jointTable, this->morphTable,
                   OCTOROK_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &this->actor.world.pos);

    if (!WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &ySurface,
                                &waterbox) ||
        (ySurface <= this->actor.floorHeight)) {
        Actor_Kill(&this->actor);
    } else {
        this->actor.world.pos.y = this->actor.home.pos.y = ySurface;
    }

    this->deathTimer = 0;
    this->hitResultAlpha = 0;
    EnSyatekiOkuta_SetupAttachToShootingGalleryMan(this);
}

void EnSyatekiOkuta_Destroy(Actor* thisx, PlayState* play) {
    EnSyatekiOkuta* this = (EnSyatekiOkuta*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * Spawns the puff of smoke that appears when the Octorok disappears when it dies.
 */
void EnSyatekiOkuta_SpawnSmoke(Vec3f* pos, Vec3f* velocity, s16 scaleStep, PlayState* play) {
    static Color_RGBA8 sSmokePrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sSmokeEnvColor = { 150, 150, 150, 255 };

    func_800B0DE0(play, pos, velocity, &gZeroVec3f, &sSmokePrimColor, &sSmokeEnvColor, 400, scaleStep);
}

/**
 * Spawns the splash that appears when the Octorok appears from underwater, hides underwater, or dies.
 */
void EnSyatekiOkuta_SpawnSplash(EnSyatekiOkuta* this, PlayState* play) {
    EffectSsGSplash_Spawn(play, &this->actor.home.pos, NULL, NULL, 0, 800);
}

/*
 * Returns true if this Octorok is hidden (in other words, if it's in the center
 * column and has another Octorok in front of it), false otherwise.
 */
s32 EnSyatekiOkuta_IsHiddenByAnotherOctorok(EnSyatekiOkuta* this) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;
    s32 index = SG_OCTO_GET_INDEX(&this->actor);

    // Only the Octoroks in the center column can be obscured by another Octorok. The Octoroks in the
    // left and right columns are always visible, since the player looks at them from an angle.
    // Additionally, the Octorok in the front row is always visible, even if it's in the center column.
    if ((index == SG_OCTO_INDEX_FOR_POS(SG_OCTO_ROW_BACK, SG_OCTO_COL_CENTER)) ||
        (index == SG_OCTO_INDEX_FOR_POS(SG_OCTO_ROW_CENTER, SG_OCTO_COL_CENTER))) {
        // Checks to see if this Octorok is hidden by an Octorok immediately in front of it.
        if (SG_OCTO_GET_TYPE(syatekiMan->octorokFlags, SG_OCTO_INDEX_DIRECTLY_IN_FRONT(index)) != SG_OCTO_TYPE_NONE) {
            return true;
        }

        // If this Octorok is in the back row, it can also be hidden by an Octorok in the front row.
        if ((index == SG_OCTO_INDEX_FOR_POS(SG_OCTO_ROW_BACK, SG_OCTO_COL_CENTER)) &&
            (SG_OCTO_GET_TYPE(syatekiMan->octorokFlags, SG_OCTO_INDEX_FOR_POS(SG_OCTO_ROW_FRONT, SG_OCTO_COL_CENTER)) !=
             SG_OCTO_TYPE_NONE)) {
            return true;
        }
    }

    return false;
}

void EnSyatekiOkuta_SetupAttachToShootingGalleryMan(EnSyatekiOkuta* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gOctorokAppearAnim, 0.0f);
    this->actor.draw = NULL;
    this->actionFunc = EnSyatekiOkuta_AttachToShootingGalleryMan;
}

/**
 * Checks every NPC actor in the scene to find the Shooting Gallery Man. Once it finds him, this will
 * make him the parent to the Octorok. This is required because the Octoroks are normally spawned as
 * part of the Town Shooting Gallery scene, so they don't have anything that links them to the Shooting
 * Gallery Man, and the Octoroks need a pointer to him in order to access his Octorok flags. If this
 * actor is spawned in a scene *without* the Shooting Gallery Man, its action function will never change
 * from this function, and the Octorok will effectively do nothing.
 */
void EnSyatekiOkuta_AttachToShootingGalleryMan(EnSyatekiOkuta* this, PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_SYATEKI_MAN) {
            this->actor.parent = actorIt;
            EnSyatekiOkuta_SetupDoNothing(this);
            break;
        }

        actorIt = actorIt->next;
    }
}

/**
 * Stops the Octorok's animation, prevents it from drawing, and sets its action function to do nothing.
 * The intention here is to stay in this action function doing nothing until the Shooting Gallery Man
 * tells it to appear in EnSyatekiOkuta_CheckForSignal, at which point the action function will be changed.
 */
void EnSyatekiOkuta_SetupDoNothing(EnSyatekiOkuta* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gOctorokAppearAnim, 0.0f);
    this->actor.draw = NULL;
    Actor_SetScale(&this->actor, 0.01f);
    this->actionFunc = EnSyatekiOkuta_DoNothing;
}

void EnSyatekiOkuta_DoNothing(EnSyatekiOkuta* this, PlayState* play) {
}

void EnSyatekiOkuta_SetupAppear(EnSyatekiOkuta* this) {
    this->actor.draw = EnSyatekiOkuta_Draw;
    this->hitResultAlpha = 0;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_OCTO_ANIM_APPEAR);
    this->actionFunc = EnSyatekiOkuta_Appear;
}

/**
 * Jumps out of the water and starts floating.
 */
void EnSyatekiOkuta_Appear(EnSyatekiOkuta* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
        if (EnSyatekiOkuta_IsHiddenByAnotherOctorok(this)) {
            return;
        }

        EnSyatekiOkuta_SpawnSplash(this, play);
        Actor_PlaySfx(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnSyatekiOkuta_SetupFloat(this);
    }
}

void EnSyatekiOkuta_SetupFloat(EnSyatekiOkuta* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_OCTO_ANIM_FLOAT);
    this->actionFunc = EnSyatekiOkuta_Float;
}

/**
 * Floats in place until the Shooting Gallery Man tells it to hide.
 */
void EnSyatekiOkuta_Float(EnSyatekiOkuta* this, PlayState* play) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    // In practice, if the Octorok is floating, then the octorokState is either SG_OCTO_STATE_APPEARED or
    // SG_OCTO_STATE_HIDING. Only the latter state is greater than SG_OCTO_STATE_INITIAL, so that's what
    // this check is looking for.
    if (syatekiMan->octorokState >= SG_OCTO_STATE_INITIAL) {
        EnSyatekiOkuta_SetupHide(this);
    }
}

void EnSyatekiOkuta_SetupHide(EnSyatekiOkuta* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_OCTO_ANIM_HIDE);
    this->actionFunc = EnSyatekiOkuta_Hide;
}

/**
 * Retreats underwater, then makes the Octorok do nothing until the Shooting Gallery Man tells it to appear again.
 */
void EnSyatekiOkuta_Hide(EnSyatekiOkuta* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        EnSyatekiOkuta_SpawnSplash(this, play);
        Actor_PlaySfx(&this->actor, NA_SE_EN_DAIOCTA_LAND);
    } else if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnSyatekiOkuta_SetupDoNothing(this);
    }
}

void EnSyatekiOkuta_SetupDie(EnSyatekiOkuta* this) {
    this->deathTimer = 0;
    this->hitResultAlpha = 300;
    if (this->type == SG_OCTO_TYPE_RED) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_OCTAROCK_DEAD1);
    }

    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_OCTO_ANIM_DIE);
    this->actionFunc = EnSyatekiOkuta_Die;
}

/**
 * Plays the death animation and slowly shrinks the Octorok. Also spawns various bubble and dust
 * effects as it dies. Once the Octorok is finished with its death animation, this function will
 * make it do nothing until the Shooting Gallery Man tells it to appear again.
 */
void EnSyatekiOkuta_Die(EnSyatekiOkuta* this, PlayState* play) {
    static Vec3f sBubbleAccel = { 0.0f, -0.5f, 0.0f };
    static Color_RGBA8 sBubblePrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sBubbleEnvColor = { 150, 150, 150, 0 };
    Vec3f velocity;
    Vec3f pos;
    s32 pad;
    s32 i;

    if (this->hitResultAlpha > 0) {
        this->hitResultAlpha -= 15;
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->deathTimer == 0) {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y + 40.0f;
            pos.z = this->actor.world.pos.z;
            velocity.x = 0.0f;
            velocity.y = -0.5f;
            velocity.z = 0.0f;
            EnSyatekiOkuta_SpawnSmoke(&pos, &velocity, -20, play);
            Actor_PlaySfx(&this->actor, NA_SE_EN_OCTAROCK_DEAD2);
        }

        this->deathTimer++;
    }

    if (Animation_OnFrame(&this->skelAnime, 15.0f)) {
        EnSyatekiOkuta_SpawnSplash(this, play);
    }

    if (this->deathTimer < 3) {
        Actor_SetScale(&this->actor, ((this->deathTimer * 0.25f) + 1.0f) * 0.01f);
    } else if (this->deathTimer < 6) {
        Actor_SetScale(&this->actor, (1.5f - ((this->deathTimer - 2) * 0.2333f)) * 0.01f);
    } else if (this->deathTimer < 11) {
        Actor_SetScale(&this->actor, (((this->deathTimer - 5) * 0.04f) + 0.8f) * 0.01f);
    } else {
        if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.002f)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_COMMON_WATER_MID);
            for (i = 0; i < 10; i++) {
                velocity.x = (Rand_ZeroOne() - 0.5f) * 7.0f;
                velocity.y = Rand_ZeroOne() * 7.0f;
                velocity.z = (Rand_ZeroOne() - 0.5f) * 7.0f;
                EffectSsDtBubble_SpawnCustomColor(play, &this->actor.world.pos, &velocity, &sBubbleAccel,
                                                  &sBubblePrimColor, &sBubbleEnvColor, Rand_S16Offset(100, 50), 25,
                                                  false);
            }

            EnSyatekiOkuta_SetupDoNothing(this);
        }

        this->actor.scale.y = this->actor.scale.x;
        this->actor.scale.z = this->actor.scale.x;
    }
}

/**
 * Adjusts the collider's dimensions and position based on a few different factors, like the Octorok's
 * type, current scale and head scale, and current action.
 */
void EnSyatekiOkuta_UpdateCollision(EnSyatekiOkuta* this, PlayState* play) {
    this->collider.dim.height =
        (sCylinderInit.dim.height - this->collider.dim.yShift) * this->headScale.y * this->actor.scale.y * 100.0f;
    this->collider.dim.radius = sCylinderInit.dim.radius * this->actor.scale.x * 100.0f;

    if (this->actionFunc == EnSyatekiOkuta_Appear) {
        if ((this->type == SG_OCTO_TYPE_BLUE) && EnSyatekiOkuta_IsHiddenByAnotherOctorok(this)) {
            return;
        }

        if (this->skelAnime.curFrame < (this->skelAnime.endFrame - 5.0f)) {
            this->collider.dim.height *= 1.35f;
        }
    }

    if (this->type == SG_OCTO_TYPE_RED) {
        this->collider.dim.radius += 10;
        this->collider.dim.height += 15;
    }

    this->collider.dim.pos.x = this->actor.world.pos.x;
    // jointTable->y is the y-translation of the skeleton root, see `LIMB_ROOT_POS`
    this->collider.dim.pos.y = this->actor.world.pos.y + (this->skelAnime.jointTable->y * this->actor.scale.y);
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * Returns true if the Octorok has been hit, false otherwise.
 */
s32 EnSyatekiOkuta_CheckCollision(EnSyatekiOkuta* this, PlayState* play) {
    if ((this->actionFunc == EnSyatekiOkuta_Die) || (this->actionFunc == EnSyatekiOkuta_DoNothing)) {
        return false;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return true;
    }

    EnSyatekiOkuta_UpdateCollision(this, play);
    return false;
}

/**
 * Checks to see if both the archery game and this Octorok are in the appropriate state to consider
 * appearing. If the conditions are right, then the Shooting Gallery Man's Octorok flags determine
 * what type this Octorok should be for the current wave; if the type is *not* SG_OCTO_TYPE_NONE,
 * then this Octorok will set itself to the appropriate type and get ready to jump out of the water.
 */
void EnSyatekiOkuta_CheckForSignal(EnSyatekiOkuta* this, PlayState* play) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;
    s16 type;

    if ((this->actionFunc != EnSyatekiOkuta_Float) && (this->actionFunc != EnSyatekiOkuta_Appear) &&
        (syatekiMan->shootingGameState == SG_GAME_STATE_RUNNING) &&
        (syatekiMan->octorokState == SG_OCTO_STATE_APPEARING)) {
        type = SG_OCTO_GET_TYPE(syatekiMan->octorokFlags, SG_OCTO_GET_INDEX(&this->actor));
        if (type > SG_OCTO_TYPE_NONE) {
            Actor_SetScale(&this->actor, 0.01f);
            this->type = type;
            EnSyatekiOkuta_SetupAppear(this);
        }
    }
}

void EnSyatekiOkuta_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSyatekiOkuta* this = (EnSyatekiOkuta*)thisx;
    EnSyatekiMan* syatekiMan;

    this->actionFunc(this, play);

    if (this->actionFunc != EnSyatekiOkuta_DoNothing) {
        SkelAnime_Update(&this->skelAnime);
    }

    EnSyatekiOkuta_CheckForSignal(this, play);

    if (EnSyatekiOkuta_CheckCollision(this, play)) {
        syatekiMan = (EnSyatekiMan*)this->actor.parent;
        if (this->type == SG_OCTO_TYPE_RED) {
            Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
            play->interfaceCtx.minigamePoints++;
            syatekiMan->score++;
            syatekiMan->lastHitOctorokType = SG_OCTO_TYPE_RED;
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_SY_ERROR);
            syatekiMan->lastHitOctorokType = SG_OCTO_TYPE_BLUE;
        }

        EnSyatekiOkuta_SetupDie(this);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    EnSyatekiOkuta_UpdateHeadScale(this);
}

/**
 * Adjusts the scale of the Octorok's head based on their current action and their current animation frame.
 */
void EnSyatekiOkuta_UpdateHeadScale(EnSyatekiOkuta* this) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->actionFunc == EnSyatekiOkuta_Appear) {
        if (curFrame < 8.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = 1.0f;
        } else if (curFrame < 10.0f) {
            this->headScale.x = this->headScale.z = 1.0f;
            this->headScale.y = ((curFrame - 7.0f) * 0.4f) + 1.0f;
        } else if (curFrame < 14.0f) {
            this->headScale.x = this->headScale.z = ((curFrame - 9.0f) * 0.075f) + 1.0f;
            this->headScale.y = 1.8f - ((curFrame - 9.0f) * 0.25f);
        } else {
            this->headScale.x = this->headScale.z = 1.3f - ((curFrame - 13.0f) * 0.05f);
            this->headScale.y = ((curFrame - 13.0f) * 0.0333f) + 0.8f;
        }
    } else if (this->actionFunc == EnSyatekiOkuta_Float) {
        this->headScale.x = this->headScale.z = 1.0f;
        this->headScale.y = (Math_SinF((M_PIf / 16) * curFrame) * 0.2f) + 1.0f;
    } else if (this->actionFunc == EnSyatekiOkuta_Hide) {
        if (curFrame < 3.0f) {
            this->headScale.y = 1.0f;
        } else if (curFrame < 4.0f) {
            this->headScale.y = (curFrame - 2.0f) + 1.0f;
        } else {
            this->headScale.y = 2.0f - ((curFrame - 3.0f) * 0.333f);
        }
        this->headScale.x = this->headScale.z = 1.0f;
    } else if (this->actionFunc == EnSyatekiOkuta_Die) {
        curFrame += this->deathTimer;
        if (curFrame >= 35.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = 1.0f;
        } else if (curFrame < 4.0f) {
            this->headScale.x = this->headScale.z = 1.0f - (curFrame * 0.0666f);
            this->headScale.y = (curFrame * 0.1666f) + 1.0f;
        } else if (curFrame < 25.0f) {
            this->headScale.x = this->headScale.z = ((curFrame - 4.0f) * 0.01f) + 0.8f;
            this->headScale.y = 1.5f - ((curFrame - 4.0f) * 0.025f);
        } else if (curFrame < 27.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = ((curFrame - 24.0f) * 0.25f) + 1.0f;
        } else if (curFrame < 30.0f) {
            this->headScale.x = this->headScale.y = this->headScale.z = 1.5f - ((curFrame - 26.0f) * 0.233f);
        } else {
            this->headScale.x = this->headScale.y = this->headScale.z = ((curFrame - 29.0f) * 0.04f) + 0.8f;
        }
    } else {
        this->headScale.x = this->headScale.y = this->headScale.z = 1.0f;
    }
}

/**
 * Gets the scaling factor for animating the snout limb. If the limb is not being transformed, no scale value is
 * returned. Returns true if the snout scale should be updated, false otherwise. The snout scale is returned via the
 * `scale` parameter.
 */
s32 EnSyatekiOkuta_GetSnoutScale(EnSyatekiOkuta* this, f32 curFrame, Vec3f* scale) {
    if (this->actionFunc == EnSyatekiOkuta_Appear) {
        scale->y = 1.0f;
        scale->z = 1.0f;
        scale->x = (Math_SinF((M_PIf / 16) * curFrame) * 0.4f) + 1.0f;
    } else if (this->actionFunc == EnSyatekiOkuta_Die) {
        if ((curFrame >= 35.0f) || (curFrame < 25.0f)) {
            return false;
        }

        if (curFrame < 27.0f) {
            scale->z = 1.0f;
            scale->x = scale->y = ((curFrame - 24.0f) * 0.5f) + 1.0f;
        } else if (curFrame < 30.0f) {
            scale->z = (curFrame - 26.0f) * 0.333f + 1.0f;
            scale->x = scale->y = 2.0f - (curFrame - 26.0f) * 0.333f;
        } else {
            scale->z = 2.0f - ((curFrame - 29.0f) * 0.2f);
            scale->x = scale->y = 1.0f;
        }
    } else {
        return false;
    }

    return true;
}

s32 EnSyatekiOkuta_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    s32 pad;
    Vec3f scale;
    f32 curFrame;
    EnSyatekiOkuta* this = (EnSyatekiOkuta*)thisx;

    curFrame = this->skelAnime.curFrame;
    if (this->actionFunc == EnSyatekiOkuta_Die) {
        curFrame += this->deathTimer;
    }

    if (limbIndex == OCTOROK_LIMB_HEAD) {
        scale = this->headScale;
        Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    } else if ((limbIndex == OCTOROK_LIMB_SNOUT) && (EnSyatekiOkuta_GetSnoutScale(this, curFrame, &scale))) {
        Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
    }

    return false;
}

void EnSyatekiOkuta_Draw(Actor* thisx, PlayState* play) {
    EnSyatekiOkuta* this = (EnSyatekiOkuta*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->type == SG_OCTO_TYPE_RED) {
        gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0);
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, gShootingGalleryOctorokBlueMaterialDL);
    }

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnSyatekiOkuta_OverrideLimbDraw, NULL,
                      &this->actor);

    // Draw the circle or cross that appears when the player hits an Octorok
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    if (this->actionFunc == EnSyatekiOkuta_Die) {
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + 30.0f, this->actor.world.pos.z + 20.0f,
                         MTXMODE_NEW);

        if (this->hitResultAlpha > 255) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 64, 32, 255);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 210, 64, 32, this->hitResultAlpha);
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        if (this->type == SG_OCTO_TYPE_BLUE) {
            gSPDisplayList(POLY_XLU_DISP++, gShootingGalleryOctorokCrossDL);
        } else {
            gSPDisplayList(POLY_XLU_DISP++, gShootingGalleryOctorokCircleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#include "z_en_ex_ruppy.h"
#include "vt.h"
#include "../ovl_En_Diving_Game/z_en_diving_game.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

void EnExRuppy_Init(Actor* thisx, PlayState* play);
void EnExRuppy_Destroy(Actor* thisx, PlayState* play);
void EnExRuppy_Update(Actor* thisx, PlayState* play);
void EnExRuppy_Draw(Actor* thisx, PlayState* play);

void EnExRuppy_DropIntoWater(EnExRuppy* this, PlayState* play);
void EnExRuppy_WaitToBlowUp(EnExRuppy* this, PlayState* play);
void EnExRuppy_WaitAsCollectible(EnExRuppy* this, PlayState* play);
void EnExRuppy_GalleryTarget(EnExRuppy* this, PlayState* play);
void EnExRuppy_EnterWater(EnExRuppy* this, PlayState* play);
void EnExRuppy_Sink(EnExRuppy* this, PlayState* play);
void EnExRuppy_WaitInGame(EnExRuppy* this, PlayState* play);
void EnExRuppy_Kill(EnExRuppy* this, PlayState* play);

static s16 sEnExRuppyCollectibleTypes[] = {
    ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE, ITEM00_RUPEE_RED, ITEM00_RUPEE_ORANGE, ITEM00_RUPEE_PURPLE,
};

// Unused, as the function sets these directly
static s16 sRupeeValues[] = {
    1, 5, 20, 500, 50,
};

const ActorInit En_Ex_Ruppy_InitVars = {
    ACTOR_EN_EX_RUPPY,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnExRuppy),
    (ActorFunc)EnExRuppy_Init,
    (ActorFunc)EnExRuppy_Destroy,
    (ActorFunc)EnExRuppy_Update,
    (ActorFunc)EnExRuppy_Draw,
    NULL,
};

void EnExRuppy_Init(Actor* thisx, PlayState* play) {
    EnExRuppy* this = (EnExRuppy*)thisx;
    EnDivingGame* divingGame;
    f32 temp1;
    f32 temp2;
    s16 temp3;

    this->type = this->actor.params;
    // "Index"
    osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ インデックス ☆☆☆☆☆ %x\n" VT_RST, this->type);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);

    switch (this->type) {
        case 0:
            
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->unk_160 = 0.3f;
            } else {
                this->unk_160 = 0.01f;
            }
            Actor_SetScale(&this->actor, this->unk_160);
            this->actor.room = -1;
            this->actor.gravity = 0.0f;

            // If you haven't won the diving game before, you will always get 5 rupees
            if (!(gSaveContext.eventChkInf[3] & 0x100)) {
                this->rupeeValue = 5;
                this->colorIdx = 1;
            } else {
                temp1 = 200.99f;
                if (this->actor.parent != NULL) {
                    divingGame = (EnDivingGame*)this->actor.parent;
                    if (divingGame->actor.update != NULL) {
                        temp2 = divingGame->extraWinCount * 10.0f;
                        temp1 += temp2;
                    }
                }

                temp3 = Rand_ZeroFloat(temp1);
                if ((temp3 >= 0) && (temp3 < 40)) {
                    this->rupeeValue = 1;
                    this->colorIdx = 0;
                } else if ((temp3 >= 40) && (temp3 < 170)) {
                    this->rupeeValue = 5;
                    this->colorIdx = 1;
                } else if ((temp3 >= 170) && (temp3 < 190)) {
                    this->rupeeValue = 20;
                    this->colorIdx = 2;
                } else if ((temp3 >= 190) && (temp3 < 200)) {
                    this->rupeeValue = 50;
                    this->colorIdx = 4;
                } else {
                    this->unk_160 = 0.02f;
                    Actor_SetScale(&this->actor, this->unk_160);
                    this->rupeeValue = 500;
                    this->colorIdx = 3;
                    if (this->actor.parent != NULL) {
                        divingGame = (EnDivingGame*)this->actor.parent;
                        if (divingGame->actor.update != NULL) {
                            divingGame->extraWinCount = 0;
                        }
                    }
                }
            }
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->actor.shape.shadowScale = 0.3f;
                this->actor.shape.yOffset = 35.0f;
            } else {
                this->actor.shape.shadowScale = 7.0f;
                this->actor.shape.yOffset = 700.0f;
            }
            this->unk_15A = this->actor.world.rot.z;
            this->actor.world.rot.z = 0;
            this->timer = 30;
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actionFunc = EnExRuppy_DropIntoWater;
            break;

        case 1:
        case 2: // Giant pink ruppe that explodes when you touch it
            if (this->type == 1) {
                this->colorIdx = 4;
                if (CVar_GetS32("gNewDrops", 0) !=0) {
                    Actor_SetScale(&this->actor, 2.0f);
                } else {
                    Actor_SetScale(&this->actor, 0.1f);
                }
            } else {
                this->colorIdx = (s16)Rand_ZeroFloat(3.99f) + 1;
                if (CVar_GetS32("gNewDrops", 0) !=0) {
                    Actor_SetScale(thisx, 0.4f);
                } else {
                    Actor_SetScale(thisx, 0.02f);
                }
            }
            this->actor.gravity = -3.0f;
            // "Wow Coin"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ わーなーコイン ☆☆☆☆☆ \n" VT_RST);
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->actor.shape.shadowScale = 0.3f;
                this->actor.shape.yOffset = 35.0f;
            } else {
                this->actor.shape.shadowScale = 6.0f;
                this->actor.shape.yOffset = 700.0f;
            }
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actionFunc = EnExRuppy_WaitToBlowUp;
            break;

        case 3: // Spawned by the guard in Hyrule courtyard
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                Actor_SetScale(&this->actor, 0.4f);
            } else {
                Actor_SetScale(&this->actor, 0.02f);
            }
            this->colorIdx = 0;
            switch ((s16)Rand_ZeroFloat(30.99f)) {
                case 0:
                    this->colorIdx = 2;
                    break;
                case 10:
                case 20:
                case 30:
                    this->colorIdx = 1;
                    break;
            }
            this->actor.gravity = -3.0f;
            // "Normal rupee"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ ノーマルルピー ☆☆☆☆☆ \n" VT_RST);
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                this->actor.shape.shadowScale = 0.3f;
                this->actor.shape.yOffset = 35.0f;
            } else {
                this->actor.shape.yOffset = 700.0f;
                this->actor.shape.shadowScale = 6.0f;
            }
            this->actor.flags &= ~ACTOR_FLAG_0;
            this->actionFunc = EnExRuppy_WaitAsCollectible;
            break;

        case 4: // Progress markers in the shooting gallery
            this->actor.gravity = -3.0f;
            this->actor.flags &= ~ACTOR_FLAG_0;
            if (CVar_GetS32("gNewDrops", 0) !=0) {
                Actor_SetScale(&this->actor, 0.3f);
                this->actor.shape.shadowScale = 0.3f;
                this->actor.shape.yOffset = -1365.0f;
            } else {
                Actor_SetScale(&this->actor, 0.01f);
                this->actor.shape.shadowScale = 6.0f;
                this->actor.shape.yOffset = -700.0f;
            }

            this->actionFunc = EnExRuppy_GalleryTarget;
            break;
    }
}

void EnExRuppy_Destroy(Actor* thisx, PlayState* play) {
}

void EnExRuppy_SpawnSparkles(EnExRuppy* this, PlayState* play, s16 numSparkles, s32 movementType) {
    static Vec3f velocities[] = { { 0.0f, 0.1f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    static Vec3f accelerations[] = { { 0.0f, 0.01f, 0.0f }, { 0.0f, 0.0f, 0.0f } };
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 i;
    s16 scale;
    s16 life;

    if (numSparkles < 1) {
        numSparkles = 1;
    }

    primColor.r = 255;
    primColor.g = 255;
    primColor.b = 0;
    envColor.r = 255;
    envColor.g = 255;
    envColor.b = 255;
    velocity = velocities[movementType];
    accel = accelerations[movementType];
    scale = 3000;
    life = 16;

    for (i = 0; i < numSparkles; i++) {
        if (movementType == 1) {
            accel.x = Rand_CenteredFloat(20.0f);
            accel.z = Rand_CenteredFloat(20.0f);
            scale = 5000;
            life = 20;
        }
        pos.x = (Rand_ZeroOne() - 0.5f) * 10.0f + this->actor.world.pos.x;
        pos.y = (Rand_ZeroOne() - 0.5f) * 10.0f + (this->actor.world.pos.y + this->unk_160 * 600.0f);
        pos.z = (Rand_ZeroOne() - 0.5f) * 10.0f + this->actor.world.pos.z;
        EffectSsKiraKira_SpawnDispersed(play, &pos, &velocity, &accel, &primColor, &envColor, scale, life);
    }
}

void EnExRuppy_DropIntoWater(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;

    this->actor.shape.rot.y += 0x7A8;
    Math_ApproachF(&this->actor.gravity, -2.0f, 0.3f, 1.0f);
    EnExRuppy_SpawnSparkles(this, play, 2, 0);
    func_80078884(NA_SE_EV_RAINBOW_SHOWER - SFX_FLAG);
    divingGame = (EnDivingGame*)this->actor.parent;
    if ((divingGame != NULL) && (divingGame->actor.update != NULL) &&
        ((divingGame->unk_296 == 0) || (this->actor.bgCheckFlags & 0x20) || (this->timer == 0))) {
        this->invisible = true;
        this->actor.speedXZ = 0.0f;
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
        this->actor.gravity = 0.0f;
        func_80078914(&this->actor.projectedPos, NA_SE_EV_BOMB_DROP_WATER);
        this->actionFunc = EnExRuppy_EnterWater;
    }
}

void EnExRuppy_EnterWater(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame = (EnDivingGame*)this->actor.parent;
    f32 temp_f2;

    if ((divingGame != NULL) && (divingGame->actor.update != NULL) && (divingGame->unk_2A2 == 2)) {
        this->invisible = false;
        this->actor.world.pos.x = ((Rand_ZeroOne() - 0.5f) * 300.0f) + -260.0f;
        this->actor.world.pos.y = ((Rand_ZeroOne() - 0.5f) * 200.0f) + 370.0f;
        temp_f2 = this->unk_15A * -50.0f;
        if (!(gSaveContext.eventChkInf[3] & 0x100)) {
            temp_f2 += -500.0f;
            this->actor.world.pos.z = ((Rand_ZeroOne() - 0.5f) * 80.0f) + temp_f2;
        } else {
            temp_f2 += -300.0f;
            this->actor.world.pos.z = ((Rand_ZeroOne() - 0.5f) * 60.0f) + temp_f2;
        }
        this->actionFunc = EnExRuppy_Sink;
        this->actor.gravity = -1.0f;
    }
}

void EnExRuppy_Sink(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;
    Vec3f pos;
    s32 pad;

    if ((this->actor.bgCheckFlags & 0x20) && (this->actor.yDistToWater > 15.0f)) {
        pos = this->actor.world.pos;
        pos.y += this->actor.yDistToWater;
        this->actor.velocity.y = -1.0f;
        this->actor.gravity = -0.2f;
        EffectSsGSplash_Spawn(play, &pos, 0, 0, 0, 800);
        func_80078914(&this->actor.projectedPos, NA_SE_EV_BOMB_DROP_WATER);
        this->actionFunc = EnExRuppy_WaitInGame;
    }
    divingGame = (EnDivingGame*)this->actor.parent;
    if ((divingGame != NULL) && (divingGame->actor.update != NULL) && (divingGame->phase == ENDIVINGGAME_PHASE_ENDED)) {
        this->timer = 20;
        this->actionFunc = EnExRuppy_Kill;
    }
}

void EnExRuppy_WaitInGame(EnExRuppy* this, PlayState* play) {
    EnDivingGame* divingGame;
    Vec3f D_80A0B388 = { 0.0f, 0.1f, 0.0f };
    Vec3f D_80A0B394 = { 0.0f, 0.0f, 0.0f };
    f32 localConst = 30.0f;

    if (this->timer == 0) {
        this->timer = 10;
        EffectSsBubble_Spawn(play, &this->actor.world.pos, 0.0f, 5.0f, 5.0f, Rand_ZeroFloat(0.03f) + 0.07f);
    }
    if (this->actor.parent != NULL) {
        divingGame = (EnDivingGame*)this->actor.parent;
        if (divingGame->actor.update != NULL) {
            if (divingGame->phase == ENDIVINGGAME_PHASE_ENDED) {
                this->timer = 20;
                this->actionFunc = EnExRuppy_Kill;
            } else if (this->actor.xyzDistToPlayerSq < SQ(localConst)) {
                Rupees_ChangeBy(this->rupeeValue);
                func_80078884(NA_SE_SY_GET_RUPY);
                divingGame->grabbedRupeesCounter++;
                Actor_Kill(&this->actor);
            }
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void EnExRuppy_Kill(EnExRuppy* this, PlayState* play) {
    this->invisible += 1;
    this->invisible &= 1; // Net effect is this->invisible = !this->invisible;
    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

typedef struct {
    /* 0x000 */ Actor actor;
    /* 0x14C */ char unk_14C[0x11A];
    /* 0x226 */ s16 unk_226;
} EnExRuppyParentActor; // Unclear what actor was intended to spawn this.

void EnExRuppy_WaitToBlowUp(EnExRuppy* this, PlayState* play) {
    EnExRuppyParentActor* parent;
    Vec3f accel = { 0.0f, 0.1f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 distToBlowUp = 50.0f;
    s16 explosionScale;
    s16 explosionScaleStep;

    if (this->type == 2) {
        distToBlowUp = 30.0f;
    }
    if (this->actor.xyzDistToPlayerSq < SQ(distToBlowUp)) {
        parent = (EnExRuppyParentActor*)this->actor.parent;
        if (parent != NULL) {
            if (parent->actor.update != NULL) {
                parent->unk_226 = 1;
            }
        } else {
            // "That idiot! error"
            osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ そ、そんなばかな！エラー！！！！！ ☆☆☆☆☆ \n" VT_RST);
        }
        osSyncPrintf(VT_FGCOL(GREEN) "☆☆☆☆☆ バカめ！ ☆☆☆☆☆ \n" VT_RST); // "Stupid!"
        explosionScale = 100;
        explosionScaleStep = 30;
        if (this->type == 2) {
            explosionScale = 20;
            explosionScaleStep = 6;
        }
        EffectSsBomb2_SpawnLayered(play, &this->actor.world.pos, &velocity, &accel, explosionScale,
                                   explosionScaleStep);
        func_8002F71C(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f);
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
        Actor_Kill(&this->actor);
    }
}

void EnExRuppy_WaitAsCollectible(EnExRuppy* this, PlayState* play) {
    f32 localConst = 30.0f;

    if (this->actor.xyzDistToPlayerSq < SQ(localConst)) {
        func_80078884(NA_SE_SY_GET_RUPY);
        Item_DropCollectible(play, &this->actor.world.pos, (sEnExRuppyCollectibleTypes[this->colorIdx] | 0x8000));
        Actor_Kill(&this->actor);
    }
}

void EnExRuppy_GalleryTarget(EnExRuppy* this, PlayState* play) {
    if (CVar_GetS32("gNewDrops", 0) !=0) {
        if (this->galleryFlag) {
            Math_ApproachF(&this->actor.shape.yOffset, 35.0f, 0.5f, 200.0f);
        } else {
            Math_ApproachF(&this->actor.shape.yOffset, -1365.0f, 0.5f, 200.0f);
        }
    } else {
        if (this->galleryFlag) {
            Math_ApproachF(&this->actor.shape.yOffset, 700.0f, 0.5f, 200.0f);
        } else {
            Math_ApproachF(&this->actor.shape.yOffset, -700.0f, 0.5f, 200.0f);
        }
    }
}

void EnExRuppy_Update(Actor* thisx, PlayState* play) {
    EnExRuppy* this = (EnExRuppy*)thisx;

    this->actor.shape.rot.y += 1960;
    this->actionFunc(this, play);
    if (this->timer != 0) {
        this->timer--;
    }
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f, 0x1C);
}

void EnExRuppy_Draw(Actor* thisx, PlayState* play) {
    static void* rupeeTextures[] = {
        gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeePinkTex, gRupeeOrangeTex,
    };
    static void* rupeeTexturesNew[] = {
        GID_RUPEE_GREEN, GID_RUPEE_BLUE, GID_RUPEE_RED, GID_RUPEE_PURPLE, GID_RUPEE_GOLD,
    };
    s32 pad;
    EnExRuppy* this = (EnExRuppy*)thisx;

    if (!this->invisible) {
        OPEN_DISPS(play->state.gfxCtx);

        func_80093D18(play->state.gfxCtx);
        func_8002EBCC(thisx, play, 0);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (CVar_GetS32("gNewDrops", 0) !=0) {
            if (this->type == 4 && this->colorIdx >= 3) {
                //For some reason the red rupee target become purple.
                //when using new drops it will show as Gold and that wrong it need to be red.
                this->colorIdx = 2;
            }
            GetItem_Draw(play, rupeeTexturesNew[this->colorIdx]);
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(rupeeTextures[this->colorIdx]));
            gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

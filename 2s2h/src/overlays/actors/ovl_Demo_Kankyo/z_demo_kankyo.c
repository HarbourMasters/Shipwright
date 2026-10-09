/*
 * File: z_demo_kankyo.c
 * Overlay: ovl_Demo_Kankyo
 * Description: Background Lost Woods/Giant's Chamber/Moon Effects
 */

#include "z_demo_kankyo.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_bubble/object_bubble.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "BenPort.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoKankyo_Init(Actor* thisx, PlayState* play);
void DemoKankyo_Destroy(Actor* thisx, PlayState* play);
void DemoKankyo_Update(Actor* thisx, PlayState* play);
void DemoKankyo_Draw(Actor* thisx, PlayState* play);
void DemoKankyo_Reset(void);

void DemoKakyo_MoonSparklesActionFunc(DemoKankyo* this, PlayState* play);

static u8 sLostWoodsSparklesMutex = false; // make sure only one can exist at once
static s16 sLostWoodsSkyFishParticleNum = 0;

ActorProfile Demo_Kankyo_Profile = {
    /**/ ACTOR_DEMO_KANKYO,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(DemoKankyo),
    /**/ DemoKankyo_Init,
    /**/ DemoKankyo_Destroy,
    /**/ DemoKankyo_Update,
    /**/ DemoKankyo_Draw,
    /**/ DemoKankyo_Reset,
};

static s32 sObjectBubbleId = OBJECT_BUBBLE | 0x10000;

void DemoKankyo_SetupAction(DemoKankyo* this, DemoKankyoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void DemoKakyo_LostWoodsSparkleActionFunc(DemoKankyo* this, PlayState* play) {
    s32 pad;
    s32 i;
    f32 randSkyfishParticleNum;
    f32 repositionLimit; // Distance from posCenter when effects are relocated. Always set to 130.0f
    f32 eyeToAtNormX;
    f32 eyeToAtNormY;
    f32 eyeToAtNormZ;
    f32 eyeToAtMag;
    f32 posCenterX;
    f32 posCenterY;
    f32 posCenterZ;
    Vec3f eyeToAt;
    Player* player = GET_PLAYER(play);

    if (play->roomCtx.unk7A[1] != 0) {
        if (play->envCtx.precipitation[PRECIP_SNOW_MAX] != 0) {
            play->envCtx.precipitation[PRECIP_SNOW_MAX]--;
        } else {
            Actor_Kill(&this->actor);
        }
    } else if (play->envCtx.precipitation[PRECIP_SNOW_MAX] < DEMOKANKYO_EFFECT_COUNT) {
        play->envCtx.precipitation[PRECIP_SNOW_MAX] += 16;
    }

    // note: DemoKankyo can crash if placed in an area that snows (ObjectKankyo)
    // because they both use PRECIP_SNOW_MAX precipitation as an effect counter,
    // causing DemoKankyo to write beyond its efffect array boundry
    // this crash can occur if the two actors are in different scenes connected by an exit
    // e.g. if you add DemoKankyo to GoronShrine, you will crash entering/leaving through door
    for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_MAX]; i++) {
        repositionLimit = 130.0f;

        eyeToAt.x = play->view.at.x - play->view.eye.x;
        eyeToAt.y = play->view.at.y - play->view.eye.y;
        eyeToAt.z = play->view.at.z - play->view.eye.z;
        eyeToAtMag = sqrtf(SQXYZ(eyeToAt));
        eyeToAtNormX = eyeToAt.x / eyeToAtMag;
        eyeToAtNormY = eyeToAt.y / eyeToAtMag;
        eyeToAtNormZ = eyeToAt.z / eyeToAtMag;

        switch (this->effects[i].state) {
            case DEMO_KANKYO_STATE_INIT:
                this->effects[i].posBase.x = play->view.eye.x + (eyeToAtNormX * 80.0f);
                this->effects[i].posBase.y = play->view.eye.y + (eyeToAtNormY * 80.0f);
                this->effects[i].posBase.z = play->view.eye.z + (eyeToAtNormZ * 80.0f);

                this->effects[i].posOffset.x = (Rand_ZeroOne() - 0.5f) * 160.0f;
                this->effects[i].posOffset.y = 30.0f;
                this->effects[i].posOffset.z = (Rand_ZeroOne() - 0.5f) * 160.0f;

                this->effects[i].speedTarget = (Rand_ZeroOne() * 1.6f) + 0.5f;
                this->effects[i].alpha = 0;
                this->effects[i].alphaClock = Rand_ZeroOne() * 65535; // random 0 to max of u16
                this->effects[i].scale = 0.1f;

                // speedClock is angles in radians,
                // should have used Rand_ZeroOne() * 2 * M_PI
                // however, due to properties of sine waves, this is effectively still random
                this->effects[i].speedClock.x = Rand_ZeroOne() * 360.0f;
                this->effects[i].speedClock.y = Rand_ZeroOne() * 360.0f;
                this->effects[i].speedClock.z = Rand_ZeroOne() * 360.0f;
                this->effects[i].pad50 = 0;
                this->effects[i].state += DEMO_KANKYO_STATE_SINGLE;
                break;

            case DEMO_KANKYO_STATE_SINGLE:
            case DEMO_KANKYO_STATE_SKYFISH:
                this->effects[i].alphaClock++;
                posCenterX = play->view.eye.x + (eyeToAtNormX * 80.0f);
                posCenterY = play->view.eye.y + (eyeToAtNormY * 80.0f);
                posCenterZ = play->view.eye.z + (eyeToAtNormZ * 80.0f);
                this->effects[i].posOffsetPrev.x = this->effects[i].posOffset.x;
                this->effects[i].posOffsetPrev.y = this->effects[i].posOffset.y;
                this->effects[i].posOffsetPrev.z = this->effects[i].posOffset.z;

                if (this->effects[i].state == DEMO_KANKYO_STATE_SINGLE) {

                    // The first 32 effects will become skyfish particles
                    // This block is also init code and only runs once
                    if (i < 32) {
                        if (Rand_ZeroOne() < 0.5f) {
                            this->effects[i].LostWoodsSkyFishSpeedXZ = TRUNCF_BINANG(Rand_ZeroOne() * 200.0f) + 200;
                        } else {
                            this->effects[i].LostWoodsSkyFishSpeedXZ = -200 - TRUNCF_BINANG(Rand_ZeroOne() * 200.0f);
                        }
                        this->effects[i].LostWoodsSkyFishPosOffsetMax = TRUNCF_BINANG(Rand_ZeroOne() * 50.0f) + 15;
                        this->effects[i].LostWoodsSkyFishSpeedY = ((Rand_ZeroOne() * 10.0f) + 10.0f) * 0.01f;

                        // Only the 31st particle matters as sLostWoodsSkyFishParticleNum will be overwritten
                        // every particle until the last skyfish particle is initialized
                        randSkyfishParticleNum = Rand_ZeroOne();
                        if (randSkyfishParticleNum < 0.2f) {
                            sLostWoodsSkyFishParticleNum = 1;
                        } else if (randSkyfishParticleNum < 0.2f) {
                            // This case is never taken as the else-if conditional is identical to the previous one
                            sLostWoodsSkyFishParticleNum = 3;
                        } else if (randSkyfishParticleNum < 0.4f) {
                            sLostWoodsSkyFishParticleNum = 7;
                        } else {
                            sLostWoodsSkyFishParticleNum = 15;
                        }

                        if ((i & sLostWoodsSkyFishParticleNum) == 0) {
                            // Head particle
                            this->effects[i].posOffset.y = 0.0f;
                        }
                        this->effects[i].state = DEMO_KANKYO_STATE_SKYFISH;
                        this->effects[i].speedTarget = 0.0f;
                    }

                    Math_SmoothStepToF(&this->effects[i].scale, 0.1f, 0.1f, 0.001f, 0.00001f);
                    Math_SmoothStepToF(&this->effects[i].speed, this->effects[i].speedTarget, 0.5f, 0.2f, 0.02f);
                    this->effects[i].posOffset.x += sinf(this->effects[i].speedClock.x) * this->effects[i].speed;
                    this->effects[i].posOffset.y += sinf(this->effects[i].speedClock.y) * this->effects[i].speed;
                    this->effects[i].posOffset.z += sinf(this->effects[i].speedClock.z) * this->effects[i].speed;

                    switch ((i >> 1) & 3) {
                        case 0:
                            this->effects[i].speedClock.x += 0.008f;
                            this->effects[i].speedClock.y += 0.05f * Rand_ZeroOne();
                            this->effects[i].speedClock.z += 0.015f;
                            break;

                        case 1:
                            this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                            this->effects[i].speedClock.y += 0.05f * Rand_ZeroOne();
                            this->effects[i].speedClock.z += 0.005f * Rand_ZeroOne();
                            break;

                        case 2:
                            this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                            this->effects[i].speedClock.y += 0.4f * Rand_ZeroOne();
                            this->effects[i].speedClock.z += 0.004f * Rand_ZeroOne();
                            break;

                        case 3:
                            this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                            this->effects[i].speedClock.y += 0.08f * Rand_ZeroOne();
                            this->effects[i].speedClock.z += 0.05f * Rand_ZeroOne();
                            break;
                    }

                } else if (this->effects[i].state == DEMO_KANKYO_STATE_SKYFISH) {
                    if ((i & sLostWoodsSkyFishParticleNum) == 0) {
                        // Head particle
                        Math_SmoothStepToF(&this->effects[i].scale, 0.25f, 0.1f, 0.001f, 0.00001f);

                        Math_SmoothStepToF(&this->effects[i].posBase.x, player->actor.world.pos.x, 0.5f, 1.0f, 0.2f);
                        Math_SmoothStepToF(&this->effects[i].posBase.y, player->actor.world.pos.y + 50.0f, 0.5f, 1.0f,
                                           0.2f);
                        Math_SmoothStepToF(&this->effects[i].posBase.z, player->actor.world.pos.z, 0.5f, 1.0f, 0.2f);

                        Math_SmoothStepToF(&this->effects[i].posOffset.x,
                                           Math_SinS(this->effects[i].LostWoodsSkyFishSpeedXZClock - 0x8000) *
                                               this->effects[i].LostWoodsSkyFishPosOffsetMax,
                                           0.5f, 2.0f, 0.2f);
                        Math_SmoothStepToF(&this->effects[i].posOffset.z,
                                           Math_CosS(this->effects[i].LostWoodsSkyFishSpeedXZClock - 0x8000) *
                                               this->effects[i].LostWoodsSkyFishPosOffsetMax,
                                           0.5f, 2.0f, 0.2f);
                        this->effects[i].LostWoodsSkyFishSpeedXZClock += this->effects[i].LostWoodsSkyFishSpeedXZ;
                        this->effects[i].posOffset.y += sinf(this->effects[i].speedClock.y);
                        this->effects[i].speedClock.x += 0.2f * Rand_ZeroOne(); // unused calculation
                        this->effects[i].speedClock.y += this->effects[i].LostWoodsSkyFishSpeedY;
                        this->effects[i].speedClock.z += 0.1f * Rand_ZeroOne(); // unused calculation

                        this->effects[i].posOffset.x =
                            Math_SinS(this->effects[i].LostWoodsSkyFishSpeedXZClock - 0x8000) *
                            this->effects[i].LostWoodsSkyFishPosOffsetMax;
                        this->effects[i].posOffset.z =
                            Math_CosS(this->effects[i].LostWoodsSkyFishSpeedXZClock - 0x8000) *
                            this->effects[i].LostWoodsSkyFishPosOffsetMax;
                    } else {
                        // Tail Particles
                        Math_SmoothStepToF(&this->effects[i].scale, 0.1f, 0.1f, 0.001f, 0.00001f);

                        // Unused calculation, speed only used in posOffset calculations,
                        // but posOffset gets overwritten for tail particles immediately below
                        Math_SmoothStepToF(&this->effects[i].speed, 1.5f, 0.5f, 0.1f, 0.0002f);

                        // particles in the skyfish's tail are moved to the previous position of the particle directly
                        // in front
                        this->effects[i].posOffset.x = this->effects[i - 1].posOffsetPrev.x +
                                                       (this->effects[i - 1].posBase.x - this->effects[i].posBase.x);
                        this->effects[i].posOffset.y = this->effects[i - 1].posOffsetPrev.y +
                                                       (this->effects[i - 1].posBase.y - this->effects[i].posBase.y);
                        this->effects[i].posOffset.z = this->effects[i - 1].posOffsetPrev.z +
                                                       (this->effects[i - 1].posBase.z - this->effects[i].posBase.z);
                    }
                }

                if ((this->effects[i].state != DEMO_KANKYO_STATE_SKYFISH) &&
                    ((((this->effects[i].posBase.x + this->effects[i].posOffset.x) - posCenterX) > repositionLimit) ||
                     (((this->effects[i].posBase.x + this->effects[i].posOffset.x) - posCenterX) < -repositionLimit) ||
                     (((this->effects[i].posBase.y + this->effects[i].posOffset.y) - posCenterY) > repositionLimit) ||
                     (((this->effects[i].posBase.y + this->effects[i].posOffset.y) - posCenterY) < -repositionLimit) ||
                     (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - posCenterZ) > repositionLimit) ||
                     (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - posCenterZ) < -repositionLimit))) {
                    if (((this->effects[i].posOffset.x + this->effects[i].posBase.x) - posCenterX) > repositionLimit) {
                        this->effects[i].posOffset.x = 0.0f;
                        this->effects[i].posBase.x = posCenterX - repositionLimit;
                    }

                    if (((this->effects[i].posBase.x + this->effects[i].posOffset.x) - posCenterX) < -repositionLimit) {
                        this->effects[i].posOffset.x = 0.0f;
                        this->effects[i].posBase.x = posCenterX + repositionLimit;
                    }

                    if (((this->effects[i].posBase.y + this->effects[i].posOffset.y) - posCenterY) > 50.0f) {
                        this->effects[i].posOffset.y = 0.0f;
                        this->effects[i].posBase.y = posCenterY - 50.0f;
                    }

                    if (((this->effects[i].posBase.y + this->effects[i].posOffset.y) - posCenterY) < -50.0f) {
                        this->effects[i].posOffset.y = 0.0f;
                        this->effects[i].posBase.y = posCenterY + 50.0f;
                    }

                    if (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - posCenterZ) > repositionLimit) {
                        this->effects[i].posOffset.z = 0.0f;
                        this->effects[i].posBase.z = posCenterZ - repositionLimit;
                    }

                    if (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - posCenterZ) < -repositionLimit) {
                        this->effects[i].posOffset.z = 0.0f;
                        this->effects[i].posBase.z = posCenterZ + repositionLimit;
                    }

                    // 2S2H [Interpolation] Skip particle interpolation on next frame
                    this->effects[i].epoch++;
                }
                break;

            case DEMO_KANKYO_STATE_DISABLED:
                this->effects[i].state = DEMO_KANKYO_STATE_INIT;
                break;
        }
    }
}

void DemoKakyo_GiantObjectCheck(DemoKankyo* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->isSafeToDrawGiants = true;
        this->actor.objectSlot = this->objectSlot;
        DemoKankyo_SetupAction(this, DemoKakyo_MoonSparklesActionFunc);
    }
}

/*
 * used by Moon AND giants types
 */
void DemoKakyo_MoonSparklesActionFunc(DemoKankyo* this, PlayState* play) {
    s32 i;
    Vec3f eyeToAt;
    f32 eyeToAtNormX;
    f32 eyeToAtNormY;
    f32 eyeToAtNormZ;
    f32 eyeToAtMag;
    f32 halfScreenWidth;
    f32 randZeroOne;
    f32 pad0;
    Vec3f newEye;
    f32 halfScreenHeight;
    s32 pad1;
    Vec3f worldPos;

    if (play->envCtx.precipitation[PRECIP_SNOW_MAX] < DEMOKANKYO_EFFECT_COUNT) {
        play->envCtx.precipitation[PRECIP_SNOW_MAX] += 16;
    }

    eyeToAt.x = play->view.at.x - play->view.eye.x;
    eyeToAt.y = play->view.at.y - play->view.eye.y;
    eyeToAt.z = play->view.at.z - play->view.eye.z;
    eyeToAtMag = sqrtf(SQXYZ(eyeToAt));
    eyeToAtNormX = eyeToAt.x / eyeToAtMag;
    eyeToAtNormY = eyeToAt.y / eyeToAtMag;
    eyeToAtNormZ = eyeToAt.z / eyeToAtMag;

    halfScreenHeight = SCREEN_HEIGHT / 2;

    for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_MAX]; i++) {
        switch (this->effects[i].state) {
            case DEMO_KANKYO_STATE_INIT:
                this->effects[i].posBase.x = play->view.eye.x + (eyeToAtNormX * halfScreenHeight);
                this->effects[i].posBase.y = play->view.eye.y + (eyeToAtNormY * halfScreenHeight);
                this->effects[i].posBase.z = play->view.eye.z + (eyeToAtNormZ * halfScreenHeight);

                this->effects[i].posOffset.x = (Rand_ZeroOne() - 0.5f) * (2.0f * halfScreenHeight);
                this->effects[i].posOffset.y = (Rand_ZeroOne() - 0.5f) * (2.0f * halfScreenHeight);
                this->effects[i].posOffset.z = (Rand_ZeroOne() - 0.5f) * (2.0f * halfScreenHeight);

                this->effects[i].speedTarget = (Rand_ZeroOne() * 1.6f) + 0.5f;
                this->effects[i].alpha = 0;
                this->effects[i].alphaClock = (Rand_ZeroOne() * 65535);
                this->effects[i].scale = 0.2f;

                // speedClock is angles in radians,
                // should have used Rand_ZeroOne() * 2 * M_PI
                // however, due to properties of sine waves, this is effectively still random
                this->effects[i].speedClock.x = Rand_ZeroOne() * 360.0f;
                this->effects[i].speedClock.y = Rand_ZeroOne() * 360.0f;
                this->effects[i].speedClock.z = Rand_ZeroOne() * 360.0f;

                this->effects[i].pad50 = 0;
                this->effects[i].state += DEMO_KANKYO_STATE_SINGLE;
                break;

            case DEMO_KANKYO_STATE_SINGLE:
            case DEMO_KANKYO_STATE_SKYFISH:
                this->effects[i].alphaClock++;

                if (this->actor.params == DEMO_KANKYO_TYPE_MOON) { // this function gets reused for giants too
                    this->effects[i].posBase.y =
                        play->view.eye.y + (eyeToAtNormY * halfScreenHeight) + (SCREEN_HEIGHT / 3);
                }

                newEye.x = play->view.eye.x + (eyeToAtNormX * halfScreenHeight);
                newEye.y = play->view.eye.y + (eyeToAtNormY * halfScreenHeight);
                newEye.z = play->view.eye.z + (eyeToAtNormZ * halfScreenHeight);

                Math_SmoothStepToF(&this->effects[i].scale, 0.2f, 0.1f, 0.001f, 0.00001f);
                Math_SmoothStepToF(&this->effects[i].speed, this->effects[i].speedTarget, 0.5f, 0.2f, 0.02f);

                this->effects[i].posOffset.x += sinf(this->effects[i].speedClock.x) * this->effects[i].speed;
                this->effects[i].posOffset.y += sinf(this->effects[i].speedClock.y) * this->effects[i].speed;
                this->effects[i].posOffset.z += sinf(this->effects[i].speedClock.z) * this->effects[i].speed;

                switch ((i >> 1) & 3) {
                    case 0:
                        this->effects[i].speedClock.x += 0.008f;
                        this->effects[i].speedClock.y += 0.05f * Rand_ZeroOne();
                        this->effects[i].speedClock.z += 0.015f;
                        break;

                    case 1:
                        this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                        this->effects[i].speedClock.y += 0.05f * Rand_ZeroOne();
                        this->effects[i].speedClock.z += 0.005f * Rand_ZeroOne();
                        break;

                    case 2:
                        this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                        this->effects[i].speedClock.y += 0.4f * Rand_ZeroOne();
                        this->effects[i].speedClock.z += 0.004f * Rand_ZeroOne();
                        break;

                    case 3:
                        this->effects[i].speedClock.x += 0.01f * Rand_ZeroOne();
                        this->effects[i].speedClock.y += 0.08f * Rand_ZeroOne();
                        this->effects[i].speedClock.z += 0.05f * Rand_ZeroOne();
                        break;
                }

                if (((this->effects[i].posBase.x + this->effects[i].posOffset.x) - newEye.x) > halfScreenHeight) {
                    // 2S2H [Interpolation] Here and below, skip particle interp on next frame when position moves
                    if (ABS(this->effects[i].posBase.x - (newEye.x - halfScreenHeight)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.x = newEye.x - halfScreenHeight;
                }

                if (((this->effects[i].posBase.x + this->effects[i].posOffset.x) - newEye.x) < -halfScreenHeight) {
                    if (ABS(this->effects[i].posBase.x - (newEye.x + halfScreenHeight)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.x = newEye.x + halfScreenHeight;
                }

                worldPos.x = this->effects[i].posBase.x + this->effects[i].posOffset.x;
                worldPos.y = this->effects[i].posBase.y + this->effects[i].posOffset.y;
                worldPos.z = this->effects[i].posBase.z + this->effects[i].posOffset.z;

                randZeroOne = Math_Vec3f_DistXZ(&worldPos, &play->view.eye) / 200.0f;
                randZeroOne = CLAMP(randZeroOne, 0.0f, 1.0f);
                halfScreenWidth = 100.0f + randZeroOne + 60.0f; // range 160 to 161...? thats about half screen width

                // I think this code is shifting the effects 1 frame -> half screen at a time to keep it in-view
                if (halfScreenWidth < ((this->effects[i].posBase.y + this->effects[i].posOffset.y) - newEye.y)) {
                    if (ABS(this->effects[i].posBase.y - (newEye.y - halfScreenWidth)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.y = newEye.y - halfScreenWidth;
                }

                if (((this->effects[i].posBase.y + this->effects[i].posOffset.y) - newEye.y) < -halfScreenWidth) {
                    if (ABS(this->effects[i].posBase.y - (newEye.y + halfScreenWidth)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.y = newEye.y + halfScreenWidth;
                }

                if (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - newEye.z) > halfScreenHeight) {
                    if (ABS(this->effects[i].posBase.z - (newEye.z - halfScreenHeight)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.z = newEye.z - halfScreenHeight;
                }

                if (((this->effects[i].posBase.z + this->effects[i].posOffset.z) - newEye.z) < -halfScreenHeight) {
                    if (ABS(this->effects[i].posBase.z - (newEye.z + halfScreenHeight)) >= 120.0f) {
                        this->effects[i].epoch++;
                    }
                    this->effects[i].posBase.z = newEye.z + halfScreenHeight;
                }

                break;

            case DEMO_KANKYO_STATE_DISABLED:
                this->effects[i].state = DEMO_KANKYO_STATE_INIT;
                break;
        }
    }
}

void DemoKankyo_Init(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;
    s32 pad;
    s32 i;
    s32 objectSlot;

    // This must be a single line to match, possibly a macro?
    // clang-format off
    for (i = 0; i < ARRAY_COUNT(this->effects); i++) { this->effects[i].state = DEMO_KANKYO_STATE_INIT; }
    // clang-format on

    //! FAKE:
    if (1) {}

    switch (this->actor.params) {
        case DEMO_KANKYO_TYPE_LOSTWOODS:
            objectSlot = 0;
            this->actor.room = -1;
            if (sLostWoodsSparklesMutex == false) {
                DemoKankyo_SetupAction(this, DemoKakyo_LostWoodsSparkleActionFunc);
                sLostWoodsSparklesMutex = true;
            } else {
                Actor_Kill(&this->actor);
            }
            break;

        case DEMO_KANKYO_TYPE_GIANTS:
            this->isSafeToDrawGiants = false;
            objectSlot = Object_GetSlot(&play->objectCtx, sObjectBubbleId);
            DemoKankyo_SetupAction(this, DemoKakyo_GiantObjectCheck);
            break;

        case DEMO_KANKYO_TYPE_MOON:
            objectSlot = 0;
            this->isSafeToDrawGiants = true;
            DemoKankyo_SetupAction(this, DemoKakyo_MoonSparklesActionFunc);
            break;

        default:
            //! @bug: this causes a crash because the actionfunc is never set
            objectSlot = OBJECT_SLOT_NONE;
            break;
    }

    if (objectSlot > OBJECT_SLOT_NONE) {
        this->objectSlot = objectSlot;
    }
}

void DemoKankyo_Destroy(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    Actor_Kill(&this->actor);
}

void DemoKankyo_Update(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    this->actionFunc(this, play);
}

void DemoKakyo_DrawLostWoodsSparkle(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    f32 scaleAlpha;
    Vec3f worldPos;
    Vec3f screenPos;

    // if not underwater
    if (!(play->cameraPtrs[CAM_ID_MAIN]->stateFlags & CAM_STATE_UNDERWATER)) {
        OPEN_DISPS(play->state.gfxCtx);

        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);

        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gSun1Tex));
        gSPDisplayList(POLY_XLU_DISP++, gSunSparkleMaterialDL);

        for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_MAX]; i++) {
            worldPos.x = this->effects[i].posBase.x + this->effects[i].posOffset.x;
            worldPos.y = this->effects[i].posBase.y + this->effects[i].posOffset.y;
            worldPos.z = this->effects[i].posBase.z + this->effects[i].posOffset.z;

            Play_GetScreenPos(play, &worldPos, &screenPos);

            // #region 2S2H [Cosmetic] Increase particle render area for widescreen
            f32 xMin = 0.0f;
            f32 xMax = SCREEN_WIDTH;
            if (OTRGetAspectRatio() > 4.0f / 3.0f) {
                xMin = OTRGetDimensionFromLeftEdge(xMin);
                xMax = OTRGetDimensionFromRightEdge(xMax);
            }
            // #endregion

            // checking if particle is on screen
            if ((screenPos.x >= xMin) && (screenPos.x < xMax) && (screenPos.y >= 0.0f) &&
                (screenPos.y < SCREEN_HEIGHT)) {
                FrameInterpolation_RecordOpenChild(&this->effects[i], this->effects[i].epoch);
                Matrix_Translate(worldPos.x, worldPos.y, worldPos.z, MTXMODE_NEW);
                scaleAlpha = this->effects[i].alpha / 50.0f;
                if (scaleAlpha > 1.0f) {
                    scaleAlpha = 1.0f;
                }

                Matrix_Scale(this->effects[i].scale * scaleAlpha, this->effects[i].scale * scaleAlpha,
                             this->effects[i].scale * scaleAlpha, MTXMODE_APPLY);

                // adjust transparency of this particle
                if (i < 32) {
                    // Skyfish particles
                    if (this->effects[i].state != DEMO_KANKYO_STATE_SKYFISH) {
                        // still initializing
                        if (this->effects[i].alpha > 0) { // NOT DECR
                            this->effects[i].alpha--;
                        }
                    } else if (this->effects[i].alpha < 100) {
                        this->effects[i].alpha++;
                    }
                } else if (this->effects[i].state != DEMO_KANKYO_STATE_SKYFISH) {
                    if ((this->effects[i].alphaClock & 31) < 16) {
                        if (this->effects[i].alpha < 235) {
                            this->effects[i].alpha += 20;
                        }
                    } else if (this->effects[i].alpha > 20) {
                        this->effects[i].alpha -= 20;
                    }
                } else if ((this->effects[i].alphaClock & 15) < 8) {
                    if (this->effects[i].alpha < 255) {
                        this->effects[i].alpha += 100;
                    }
                } else if (this->effects[i].alpha > 10) {
                    this->effects[i].alpha -= 10;
                }

                gDPPipeSync(POLY_XLU_DISP++);

                switch (i & 1) {
                    case 0: // gold particles
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, this->effects[i].alpha);
                        gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 0, this->effects[i].alpha);
                        break;

                    case 1: // silver particles
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->effects[i].alpha);
                        gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, this->effects[i].alpha);
                        break;
                }

                Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
                Matrix_RotateZF(DEG_TO_RAD(play->state.frames * 20.0f), MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gSunSparkleModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

// draw, giants and moon
void DemoKankyo_DrawMoonAndGiant(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoKankyo* this = (DemoKankyo*)thisx;
    s16 i;
    f32 alphaScale;

    if (this->isSafeToDrawGiants != false) {
        Vec3f worldPos;
        Vec3f screenPos;
        s32 pad;
        GraphicsContext* gfxCtx = play->state.gfxCtx;

        OPEN_DISPS(gfxCtx);

        Gfx_SetupDL25_Xlu(gfxCtx);

        for (i = 0; i < play->envCtx.precipitation[PRECIP_SNOW_MAX]; i++) {
            worldPos.x = this->effects[i].posBase.x + this->effects[i].posOffset.x;
            worldPos.y = this->effects[i].posBase.y + this->effects[i].posOffset.y;
            worldPos.z = this->effects[i].posBase.z + this->effects[i].posOffset.z;

            Play_GetScreenPos(play, &worldPos, &screenPos);

            // #region 2S2H [Cosmetic] Increase effect render area for widescreen
            f32 xMin = 0.0f;
            f32 xMax = SCREEN_WIDTH;
            if (OTRGetAspectRatio() > 4.0f / 3.0f) {
                xMin = OTRGetDimensionFromLeftEdge(xMin);
                xMax = OTRGetDimensionFromRightEdge(xMax);
            }
            // #endregion

            // checking if effect is on screen
            if ((screenPos.x >= xMin) && (screenPos.x < xMax) && (screenPos.y >= 0.0f) &&
                (screenPos.y < SCREEN_HEIGHT)) {
                FrameInterpolation_RecordOpenChild(&this->effects[i], this->effects[i].epoch);
                Matrix_Translate(worldPos.x, worldPos.y, worldPos.z, MTXMODE_NEW);
                alphaScale = this->effects[i].alpha / 50.0f;
                if (alphaScale > 1.0f) {
                    alphaScale = 1.0f;
                }
                Matrix_Scale(this->effects[i].scale * alphaScale, this->effects[i].scale * alphaScale,
                             this->effects[i].scale * alphaScale, MTXMODE_APPLY);
                alphaScale = Math_Vec3f_DistXYZ(&worldPos, &play->view.eye) / 300.0f;
                alphaScale = CLAMP(1.0f - alphaScale, 0.0f, 1.0f);

                if (this->actor.params == DEMO_KANKYO_TYPE_GIANTS) {
                    this->effects[i].alpha = 255.0f * alphaScale;
                } else {
                    this->effects[i].alpha = 160.0f * alphaScale;
                }

                gDPPipeSync(POLY_XLU_DISP++);

                switch (i & 1) { // half/half slightly different shades of yellow/tan
                    case 0:
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 230, 230, 220, this->effects[i].alpha);
                        gDPSetEnvColor(POLY_XLU_DISP++, 230, 230, 30, this->effects[i].alpha);
                        break;

                    case 1:
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 200, 190, this->effects[i].alpha);
                        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 30, this->effects[i].alpha);
                        break;
                }

                gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

                Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

                Matrix_RotateZF(DEG_TO_RAD(play->state.frames * 20.0f), MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

                if (this->actor.params == DEMO_KANKYO_TYPE_GIANTS) {
                    gSPDisplayList(POLY_XLU_DISP++, gBubbleDL);
                } else {
                    gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);
                }
                FrameInterpolation_RecordCloseChild();
            }
        }

        CLOSE_DISPS(gfxCtx);
    }
}

void DemoKankyo_Draw(Actor* thisx, PlayState* play) {
    DemoKankyo* this = (DemoKankyo*)thisx;

    switch (this->actor.params) {
        case DEMO_KANKYO_TYPE_LOSTWOODS:
            DemoKakyo_DrawLostWoodsSparkle(&this->actor, play);
            break;

        case DEMO_KANKYO_TYPE_GIANTS:
        case DEMO_KANKYO_TYPE_MOON:
            DemoKankyo_DrawMoonAndGiant(&this->actor, play);
            break;
    }
}

void DemoKankyo_Reset(void) {
    sLostWoodsSparklesMutex = false;
    sLostWoodsSkyFishParticleNum = 0;
}

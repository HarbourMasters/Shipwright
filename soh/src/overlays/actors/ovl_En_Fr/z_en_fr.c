#include "z_en_fr.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "vt.h"
#include "objects/object_fr/object_fr.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4 | ACTOR_FLAG_25)

void EnFr_Init(Actor* thisx, PlayState* play);
void EnFr_Destroy(Actor* thisx, PlayState* play);
void EnFr_Update(Actor* thisx, PlayState* play);
void EnFr_UpdateIdle(Actor* thisx, PlayState* play);
void EnFr_UpdateActive(Actor* thisx, PlayState* play);
void EnFr_Draw(Actor* thisx, PlayState* play);
void EnFr_Reset(void);

// Animation Functions
void EnFr_SetupJumpingOutOfWater(EnFr* this, PlayState* play);
void EnFr_JumpingOutOfWater(EnFr* this, PlayState* play);
void EnFr_OrientOnLogSpot(EnFr* this, PlayState* play);
void EnFr_ChooseJumpFromLogSpot(EnFr* this, PlayState* play);
void EnFr_JumpingUp(EnFr* this, PlayState* play);
void EnFr_JumpingBackIntoWater(EnFr* this, PlayState* play);
void EnFr_DecrementBlinkTimerUpdate(EnFr* this);

// Activation
void EnFr_Idle(EnFr* this, PlayState* play);
void EnFr_Activate(EnFr* this, PlayState* play);
void EnFr_ActivateCheckFrogSong(EnFr* this, PlayState* play);

// Listening for Child Songs
void func_80A1BE98(EnFr* this, PlayState* play);
void EnFr_ListeningToOcarinaNotes(EnFr* this, PlayState* play);
void EnFr_ChildSong(EnFr* this, PlayState* play);
void EnFr_ChildSongFirstTime(EnFr* this, PlayState* play);

// Frog Song for HP Functions
void EnFr_TalkBeforeFrogSong(EnFr* this, PlayState* play);
void EnFr_SetupFrogSong(EnFr* this, PlayState* play);
void EnFr_ContinueFrogSong(EnFr* this, PlayState* play);
void EnFr_OcarinaMistake(EnFr* this, PlayState* play);

// Reward Functions
void EnFr_SetupReward(EnFr* this, PlayState* play, u8 unkCondition);
void EnFr_PrintTextBox(EnFr* this, PlayState* play);
void EnFr_TalkBeforeReward(EnFr* this, PlayState* play);
RandomizerCheck EnFr_RandomizerCheckFromSongIndex(u16 songIndex);
void EnFr_SetReward(EnFr* this, PlayState* play);

// Deactivate
void EnFr_Deactivate(EnFr* this, PlayState* play);
void EnFr_GiveReward(EnFr* this, PlayState* play);
void EnFr_SetIdle(EnFr* this, PlayState* play);

/*
Frogs params WIP docs

Represents 6 Actor Instances for frogs:
    - 1 Prop actor instance set to where Link plays Ocarina, manages 5 frogs
    - 5 NPC actor instances for the frogs themselves

0: Prop Actor Instance (located where link detects ocarina, interacts with Link)
1: Frog 0 (Yellow)
2: Frog 1 (Blue)
3: Frog 2 (Red)
4: Frog 3 (Purple)
5: Frog 4 (White)

Note that because of the Prop Actor, actor.params is 1 shifted from frogIndex
Therefore, frogIndex = actor.params - 1


sEnFrPointers.flags = 1
     - Activate frogs, frogs will jump out of the water

sEnFrPointers.flags = 1 to 11:
     - Counter: Frogs will sequentially jump out based on counter:
         - 1: Frog 1 (Blue)
         - 3: frog 3 (Purple)
         - 5: frog 0 (Yellow)
         - 7: frog 2 (Red)
         - 9: frog 4 (White)
     - Will proceed when counter reachers 11

sEnFrPointers.flags = 12
     - Deactivate frogs, frogs will jump back into the water
*/

EnFrPointers sEnFrPointers = {
    0x00,
    {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    },
};


// Flags for gSaveContext.eventChkInf[13]
static u16 sSongIndex[] = {
    0x0002, 0x0004, 0x0010, 0x0008, 0x0020, 0x0040, 0x0001, 0x0000,
};

// Frog to Index for Song Flag (sSongIndex) Mapping
static u8 sFrogToSongIndex[] = {
    FROG_SARIA, FROG_SUNS, FROG_SOT, FROG_ZL, FROG_EPONA,
};

// Song to Frog Index Mapping
static s32 sSongToFrog[] = {
    FROG_PURPLE, FROG_WHITE, FROG_YELLOW, FROG_BLUE, FROG_RED,
};

const ActorInit En_Fr_InitVars = {
    ACTOR_EN_FR,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FR,
    sizeof(EnFr),
    (ActorFunc)EnFr_Init,
    (ActorFunc)EnFr_Destroy,
    (ActorFunc)EnFr_Update,
    NULL,
    (ActorResetFunc)EnFr_Reset,
};

static Color_RGBA8 sEnFrColor[] = {
    { 200, 170, 0, 255 }, { 0, 170, 200, 255 }, { 210, 120, 100, 255 }, { 120, 130, 230, 255 }, { 190, 190, 190, 255 },
};

// Jumping back into water frog animation
//      sLogSpotToFromWater[frog].xzDist is magnitude of xz distance frog travels
//      sLogSpotToFromWater[frog].yaw is rot around y-axis of jumping back into water
//      sLogSpotToFromWater[frog].yDist is change in y distance frog travels
static LogSpotToFromWater sLogSpotToFromWater[] = {
    { 0.0f, 0.0f, 0.0f },              // Prop   (Where link pulls ocarina)
    { 80.0f, -0.375f * M_PI, -80.0f }, // FROG_YELLOW
    { 80.0f, -0.5f * M_PI, -80.0f },   // FROG_BLUE
    { 80.0f, -0.25f * M_PI, -80.0f },  // FROG_RED
    { 80.0f, 0.875f * M_PI, -80.0f },  // FROG_PURPLE
    { 80.0f, 0.5f * M_PI, -80.0f },    // FROG_WHITE
};

// Timer values for the frog choir song
static s16 sTimerFrogSong[] = {
    40, 20, 15, 12, 12,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

// Counter to Coordinate Frog jumping out of water one at a time
// Used as part of sEnFrPointers.flags
static u8 sTimerJumpingOutOfWater[] = {
    5, 1, 7, 3, 9,
};

// targetScale (default = 150.0) Actor scale target for Math_ApproachF
// Used as a frog grows from hearing a new child song
static f32 sGrowingScale[] = {
    180.0f,
    210.0f,
    240.0f,
    270.0f,
};

static u8 sSmallFrogNotes[] = {
    5,  // C-Down Ocarina
    2,  // A Button Ocarina
    9,  // C-Right Ocarina
    11, // C-Left Ocarina
    14, // C Up Ocarina
};

static s8 sLargeFrogNotes[] = {
    -7,  // C-Down Ocarina
    -10, // A Button Ocarina
    -3,  // C-Right Ocarina
    -1,  // C-Left Ocarina
    2,   // C Up Ocarina
};

static u8 sJumpOrder[] = {
    FROG_BLUE, FROG_YELLOW, FROG_RED, FROG_PURPLE, FROG_WHITE, FROG_BLUE, FROG_YELLOW, FROG_RED,
};

static u8 sOcarinaNotes[] = {
    OCARINA_NOTE_D4, OCARINA_NOTE_F4, OCARINA_NOTE_A4, OCARINA_NOTE_B4, OCARINA_NOTE_D5,
};

void EnFr_OrientUnderwater(EnFr* this) {
    Vec3f vec1;
    Vec3f vec2;

    vec1.x = vec1.y = 0.0f;
    vec1.z = this->xzDistToLogSpot = sLogSpotToFromWater[this->actor.params].xzDist;
    Matrix_RotateY(sLogSpotToFromWater[this->actor.params].yaw, MTXMODE_NEW);
    Matrix_MultVec3f(&vec1, &vec2);
    this->actor.world.pos.x = this->posLogSpot.x + vec2.x;
    this->actor.world.pos.z = this->posLogSpot.z + vec2.z;
    this->actor.world.pos.y = sLogSpotToFromWater[this->actor.params].yDist + this->posLogSpot.y;
    this->actor.world.rot.y = this->actor.shape.rot.y =
        (s16)(sLogSpotToFromWater[this->actor.params].yaw * ((f32)0x8000 / M_PI)) + 0x8000;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
}

void EnFr_Init(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (this->actor.params == 0) {
        this->actor.destroy = NULL;
        this->actor.draw = NULL;
        this->actor.update = EnFr_UpdateIdle;
        this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_4);
        this->actor.flags &= ~0;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        this->actor.textId = 0x40AC;
        this->actionFunc = EnFr_Idle;
    } else {
        if ((this->actor.params >= 6) || (this->actor.params < 0)) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "The argument is wrong!!"
            osSyncPrintf("%s[%d] : 引数が間違っている！！(%d)\n", __FILE__, __LINE__, this->actor.params);
            osSyncPrintf(VT_RST);
            ASSERT((this->actor.params >= 6) || (this->actor.params < 0));
        }

        this->objBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_GAMEPLAY_FIELD_KEEP);
        if (this->objBankIndex < 0) {
            Actor_Kill(&this->actor);
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There is no bank!!"
            osSyncPrintf("%s[%d] : バンクが無いよ！！\n", __FILE__, __LINE__);
            osSyncPrintf(VT_RST);
            ASSERT(this->objBankIndex < 0);
        }
    }
}

// Draw only the purple frog when ocarina is not pulled out on the log spot
void EnFr_DrawIdle(EnFr* this) {
    this->actor.draw = (this->actor.params - 1) != FROG_PURPLE ? NULL : EnFr_Draw;
}

void EnFr_DrawActive(EnFr* this) {
    this->actor.draw = EnFr_Draw;
}

void EnFr_Update(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;
    s32 pad;
    s32 frogIndex;
    s32 pad2;

    if (Object_IsLoaded(&play->objectCtx, this->objBankIndex)) {
        this->actor.flags &= ~ACTOR_FLAG_4;
        frogIndex = this->actor.params - 1;
        sEnFrPointers.frogs[frogIndex] = this;
        Actor_ProcessInitChain(&this->actor, sInitChain);
        // frog
        SkelAnime_InitFlex(play, &this->skelAnime, &object_fr_Skel_00B498, &object_fr_Anim_001534,
                           this->jointTable, this->morphTable, 24);
        // butterfly
        SkelAnime_Init(play, &this->skelAnimeButterfly, &gButterflySkel, &gButterflyAnim,
                       this->jointTableButterfly, this->morphTableButterfly, 8);
        // When playing the song for the HP, the frog with the next note and the butterfly turns on its lightsource
        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y,
                                  this->actor.home.pos.z, 255, 255, 255, -1);
        // Check to see if the song for a particular frog has been played.
        // If it has, the frog is larger. If not, the frog is smaller
        this->scale = gSaveContext.eventChkInf[13] & sSongIndex[sFrogToSongIndex[frogIndex]] ? 270.0f : 150.0f;
        // When the frogs are not active (link doesn't have his ocarina out),
        // Then shrink the frogs down by a factor of 10,000
        Actor_SetScale(&this->actor, this->scale * 0.0001f);
        this->actor.minVelocityY = -9999.0f;
        Actor_SetFocus(&this->actor, 10.0f);
        this->eyeTexIndex = 1;
        this->blinkTimer = (s16)(Rand_ZeroFloat(60.0f) + 20.0f);
        this->blinkFunc = EnFr_DecrementBlinkTimerUpdate;
        this->isBelowWaterSurfacePrevious = this->isBelowWaterSurfaceCurrent = false;
        this->isJumpingUp = false;
        this->posLogSpot = this->actor.world.pos;
        this->actionFunc = EnFr_SetupJumpingOutOfWater;
        this->isDeactivating = false;
        this->growingScaleIndex = 0;
        this->isActive = false;
        this->isJumpingToFrogSong = false;
        this->songIndex = FROG_NO_SONG;
        this->unusedButterflyActor = NULL;
        EnFr_OrientUnderwater(this);
        EnFr_DrawIdle(this);
        this->actor.update = EnFr_UpdateActive;
        this->isButterflyDrawn = false;
        this->xyAngleButterfly = 0x1000 * (s16)Rand_ZeroFloat(255.0f);
        this->posButterflyLight.x = this->posButterfly.x = this->posLogSpot.x;
        this->posButterflyLight.y = this->posButterfly.y = this->posLogSpot.y + 50.0f;
        this->posButterflyLight.z = this->posButterfly.z = this->posLogSpot.z;
        this->actor.flags &= ~ACTOR_FLAG_0;
    }
}

void EnFr_Destroy(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void EnFr_IsDivingIntoWater(EnFr* this, PlayState* play) {
    WaterBox* waterBox;
    f32 waterSurface;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                &waterSurface, &waterBox)) {
        this->isBelowWaterSurfacePrevious = this->isBelowWaterSurfaceCurrent;
        this->isBelowWaterSurfaceCurrent = this->actor.world.pos.y <= waterSurface ? true : false;
    }
}

void EnFr_DivingIntoWater(EnFr* this, PlayState* play) {
    Vec3f vec;

    // Jumping into or out of water
    if (this->isBelowWaterSurfaceCurrent != this->isBelowWaterSurfacePrevious) {
        vec.x = this->actor.world.pos.x;
        vec.y = this->actor.world.pos.y - 10.0f;
        vec.z = this->actor.world.pos.z;
        EffectSsGSplash_Spawn(play, &vec, NULL, NULL, 1, 1);

        if (this->isBelowWaterSurfaceCurrent == false) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_DIVE_INTO_WATER_L);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

s32 EnFr_IsBelowLogSpot(EnFr* this, f32* yDistToLogSpot) {
    yDistToLogSpot[0] = this->actor.world.pos.y - this->posLogSpot.y;
    if ((this->actor.velocity.y < 0.0f) && (yDistToLogSpot[0] <= 0.0f)) {
        this->actor.velocity.y = 0.0f;
        this->actor.world.pos.y = this->posLogSpot.y;
        return true;
    } else {
        return false;
    }
}

s32 EnFr_IsAboveAndWithin30DistXZ(Player* player, EnFr* this) {
    f32 xDistToPlayer = player->actor.world.pos.x - this->actor.world.pos.x;
    f32 zDistToPlayer = player->actor.world.pos.z - this->actor.world.pos.z;
    f32 yDistToPlayer = player->actor.world.pos.y - this->actor.world.pos.y;

    return ((SQ(xDistToPlayer) + SQ(zDistToPlayer)) <= SQ(30.0f)) && (yDistToPlayer >= 0.0f);
}

void EnFr_DecrementBlinkTimer(EnFr* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else {
        this->blinkFunc = EnFr_DecrementBlinkTimerUpdate;
    }
}

void EnFr_DecrementBlinkTimerUpdate(EnFr* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else if (this->eyeTexIndex) {
        this->eyeTexIndex = 0;
        this->blinkTimer = (s16)(Rand_ZeroFloat(60.0f) + 20.0f);
        this->blinkFunc = EnFr_DecrementBlinkTimer;
    } else {
        this->eyeTexIndex = 1;
        this->blinkTimer = 1;
    }
}

void EnFr_SetupJumpingOutOfWater(EnFr* this, PlayState* play) {
    if (sEnFrPointers.flags == sTimerJumpingOutOfWater[this->actor.params - 1]) {
        Animation_Change(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        EnFr_DrawActive(this);
        this->actionFunc = EnFr_JumpingOutOfWater;
    }
}

void EnFr_JumpingOutOfWater(EnFr* this, PlayState* play) {
    Vec3f vec1;
    Vec3f vec2;

    if (this->skelAnime.curFrame == 6.0f) {
        sEnFrPointers.flags++;
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.gravity = -10.0f;
        this->actor.speedXZ = 0.0f;
        this->actor.velocity.y = 47.0f;
    }

    vec1.x = vec1.y = 0.0f;
    vec1.z = this->xzDistToLogSpot;
    Matrix_RotateY(((this->actor.world.rot.y + 0x8000) / (f32)0x8000) * M_PI, MTXMODE_NEW);
    Matrix_MultVec3f(&vec1, &vec2);
    this->actor.world.pos.x = this->posLogSpot.x + vec2.x;
    this->actor.world.pos.z = this->posLogSpot.z + vec2.z;
    if (this->skelAnime.curFrame >= 3.0f) {
        Math_ApproachF(&this->xzDistToLogSpot, 0.0f, 1.0f, 10.0f);
    }

    if (EnFr_IsBelowLogSpot(this, &vec2.y)) {
        this->actor.gravity = 0.0f;
        this->actionFunc = EnFr_OrientOnLogSpot;
        this->unusedFloat = 0.0f;
    }

    if ((this->actor.velocity.y <= 0.0f) && (vec2.y < 40.0f)) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnFr_OrientOnLogSpot(EnFr* this, PlayState* play) {
    s16 rotYRemaining = Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 10000, 100);

    this->actor.world.rot.y = this->actor.shape.rot.y;

    if ((rotYRemaining == 0) && (this->skelAnime.curFrame == this->skelAnime.endFrame)) {
        sEnFrPointers.flags++;
        this->actionFunc = EnFr_ChooseJumpFromLogSpot;
        Animation_Change(&this->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
    }
}

void EnFr_ChooseJumpFromLogSpot(EnFr* this, PlayState* play) {
    if (sEnFrPointers.flags == 12) {
        this->actor.world.rot.y = ((f32)0x8000 / M_PI) * sLogSpotToFromWater[this->actor.params].yaw;
        Animation_Change(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        this->actionFunc = EnFr_JumpingBackIntoWater;
    } else if (this->isJumpingUp) {
        Animation_Change(&this->skelAnime, &object_fr_Anim_0007BC, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0007BC), ANIMMODE_ONCE, 0.0f);
        this->actionFunc = EnFr_JumpingUp;
    }
}

void EnFr_JumpingUp(EnFr* this, PlayState* play) {
    f32 yDistToLogSpot;

    if (this->skelAnime.curFrame == 6.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.gravity = -10.0f;
        this->actor.velocity.y = 25.0f;
        if (this->isJumpingToFrogSong) {
            this->isJumpingToFrogSong = false;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_EAT);
        }
    }

    if (EnFr_IsBelowLogSpot(this, &yDistToLogSpot)) {
        this->isJumpingUp = false;
        this->actor.gravity = 0.0f;
        Animation_Change(&this->skelAnime, &object_fr_Anim_0011C0, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_0011C0), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = EnFr_ChooseJumpFromLogSpot;
    } else if ((this->actor.velocity.y <= 0.0f) && (yDistToLogSpot < 40.0f)) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void EnFr_JumpingBackIntoWater(EnFr* this, PlayState* play) {
    f32 yUnderwater = sLogSpotToFromWater[this->actor.params].yDist + this->posLogSpot.y;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 10000, 100);
    if (this->skelAnime.curFrame == 6.0f) {
        this->skelAnime.playSpeed = 0.0f;
    } else if (this->skelAnime.curFrame == 3.0f) {
        this->actor.speedXZ = 6.0f;
        this->actor.gravity = -10.0f;
        this->actor.velocity.y = 25.0f;
    }

    // Final Spot Reached
    if ((this->actor.velocity.y < 0.0f) && (this->actor.world.pos.y < yUnderwater)) {
        Animation_Change(&this->skelAnime, &object_fr_Anim_001534, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_fr_Anim_001534), ANIMMODE_LOOP, 0.0f);
        this->actionFunc = EnFr_SetupJumpingOutOfWater;
        EnFr_DrawIdle(this);
        this->isDeactivating = true;
        EnFr_OrientUnderwater(this);
    }
}

void EnFr_SetScaleActive(EnFr* this, PlayState* play) {
    switch (this->isGrowing) {
        case false:
            Math_ApproachF(&this->scale, sGrowingScale[this->growingScaleIndex], 2.0f, 25.0f);
            if (this->scale >= sGrowingScale[this->growingScaleIndex]) {
                this->scale = sGrowingScale[this->growingScaleIndex];
                if (this->growingScaleIndex < 3) {
                    this->isGrowing = true;
                } else {
                    this->isJumpingUp = false;
                    this->isActive = false;
                }
            }
            break;
        case true:
            Math_ApproachF(&this->scale, 150.0f, 2.0f, 25.0f);
            if (this->scale <= 150.0f) {
                this->scale = 150.0f;
                this->growingScaleIndex++;
                if (this->growingScaleIndex >= 4) {
                    this->growingScaleIndex = 3;
                }
                this->isGrowing = false;
            }
            break;
    }
}

void EnFr_ButterflyPath(EnFr* this, PlayState* play) {
    s16 rotY = this->actor.shape.rot.y;
    f32 sin;
    Vec3f vec1;
    Vec3f vec2;

    this->xyAngleButterfly += 0x1000;
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = 0.0f;
    vec1.z = 25.0f;
    Matrix_MultVec3f(&vec1, &vec2);
    sin = Math_SinS(this->xyAngleButterfly * 2) * 5.0f;
    this->posButterfly.x = (Math_SinS(rotY) * sin) + vec2.x;
    this->posButterfly.y = (2.0f * Math_CosS(this->xyAngleButterfly)) + (this->posLogSpot.y + 50.0f);
    this->posButterfly.z = (Math_CosS(rotY) * sin) + vec2.z;
    Matrix_Translate(this->posButterfly.x, this->posButterfly.y, this->posButterfly.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = 0.0f;
    vec1.y = -15.0f;
    vec1.z = 20.0f;
    Matrix_MultVec3f(&vec1, &this->posButterflyLight);
}

void EnFr_UpdateActive(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    this->jumpCounter++;
    Actor_SetScale(&this->actor, this->scale * 0.0001f);

    if (this->isActive) {
        EnFr_SetScaleActive(this, play);
    } else {
        Actor_SetFocus(&this->actor, 10.0f);
        this->blinkFunc(this);
        this->actionFunc(this, play);
        EnFr_IsDivingIntoWater(this, play);
        EnFr_DivingIntoWater(this, play);
        SkelAnime_Update(&this->skelAnime);
        SkelAnime_Update(&this->skelAnimeButterfly);
        EnFr_ButterflyPath(this, play);
        Actor_MoveForward(&this->actor);
    }
}

s32 EnFr_SetupJumpingUp(EnFr* this, s32 frogIndex) {
    EnFr* frog = sEnFrPointers.frogs[frogIndex];
    u8 semitone;

    if (frog != NULL && frog->isJumpingUp == false) {
        semitone = frog->growingScaleIndex == 3 ? sLargeFrogNotes[frogIndex] : sSmallFrogNotes[frogIndex];
        if (this->songIndex == FROG_CHOIR_SONG) {
            frog->isJumpingToFrogSong = true;
        }
        frog->isJumpingUp = true;
        Audio_PlaySoundTransposed(&frog->actor.projectedPos, NA_SE_EV_FROG_JUMP, semitone);
        return true;
    } else {
        return false;
    }
}

void EnFr_Idle(EnFr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & 0x2000000) {
        if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_00;
        }

        OnePointCutscene_Init(play, 4110, ~0x62, &this->actor, MAIN_CAM);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        player->actor.world.pos.x = this->actor.world.pos.x; // x = 990.0f
        player->actor.world.pos.y = this->actor.world.pos.y; // y = 205.0f
        player->actor.world.pos.z = this->actor.world.pos.z; // z = -1220.0f
        player->currentYaw = player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y;
        this->reward = GI_NONE;
        this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        this->actionFunc = EnFr_Activate;
    } else if (EnFr_IsAboveAndWithin30DistXZ(player, this)) {
        player->unk_6A8 = &this->actor;
    }
}

void EnFr_Activate(EnFr* this, PlayState* play) {
    if (play->msgCtx.msgMode == MSGMODE_OCARINA_PLAYING) {
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        sEnFrPointers.flags = 1;
        this->actionFunc = EnFr_ActivateCheckFrogSong;
    } else if (play->msgCtx.msgMode == MSGMODE_PAUSED) { // Goes to Frogs 2 Song
        sEnFrPointers.flags = 1;
        this->actionFunc = EnFr_ActivateCheckFrogSong;
    }
}

void EnFr_ActivateCheckFrogSong(EnFr* this, PlayState* play) {
    if (sEnFrPointers.flags == 11) {
        // Check if all 6 child songs have been played for the frogs
        if ((gSaveContext.eventChkInf[13] & 0x2)        // ZL
            && (gSaveContext.eventChkInf[13] & 0x4)     // Epona
            && (gSaveContext.eventChkInf[13] & 0x10)    // Saria
            && (gSaveContext.eventChkInf[13] & 0x8)     // Suns
            && (gSaveContext.eventChkInf[13] & 0x20)    // SoT
            && (gSaveContext.eventChkInf[13] & 0x40)) { // SoS
            this->actionFunc = EnFr_TalkBeforeFrogSong;
            this->songIndex = FROG_CHOIR_SONG;
            Message_StartTextbox(play, 0x40AB, &this->actor);
        } else {
            this->songIndex = FROG_ZL;
            this->actionFunc = func_80A1BE98;
        }
    }
}

void func_80A1BE98(EnFr* this, PlayState* play) {
    EnFr* frog;
    s32 frogIndex;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        if (frog != NULL && frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
            continue;
        } else {
            return;
        }
    }

    func_8010BD58(play, OCARINA_ACTION_CHECK_NOWARP);
    this->actionFunc = EnFr_ListeningToOcarinaNotes;
}

void EnFr_ListeningToOcarinaNotes(EnFr* this, PlayState* play) {
    this->songIndex = FROG_NO_SONG;
    switch (play->msgCtx.ocarinaMode) { // Ocarina Song played
        case OCARINA_MODE_07:
            this->songIndex = FROG_ZL;
            break;
        case OCARINA_MODE_06:
            this->songIndex = FROG_EPONA;
            break;
        case OCARINA_MODE_05:
            this->songIndex = FROG_SARIA;
            break;
        case OCARINA_MODE_08:
            this->songIndex = FROG_SUNS;
            break;
        case OCARINA_MODE_09:
            this->songIndex = FROG_SOT;
            break;
        case OCARINA_MODE_0A:
            this->songIndex = FROG_STORMS;
            break;
        case OCARINA_MODE_04:
            EnFr_OcarinaMistake(this, play);
            break;
        case OCARINA_MODE_01:                           // Ocarina note played, but no song played
            switch (play->msgCtx.lastOcaNoteIdx) { // Jumping frogs in open ocarina based on ocarina note played
                case OCARINA_NOTE_D4:
                    EnFr_SetupJumpingUp(this, FROG_BLUE);
                    break;
                case OCARINA_NOTE_F4:
                    EnFr_SetupJumpingUp(this, FROG_YELLOW);
                    break;
                case OCARINA_NOTE_A4:
                    EnFr_SetupJumpingUp(this, FROG_RED);
                    break;
                case OCARINA_NOTE_B4:
                    EnFr_SetupJumpingUp(this, FROG_PURPLE);
                    break;
                case OCARINA_NOTE_D5:
                    EnFr_SetupJumpingUp(this, FROG_WHITE);
                    break;
            }
    }
    if (this->songIndex != FROG_NO_SONG) {
        this->jumpCounter = 0;
        this->actionFunc = EnFr_ChildSong;
    }
}

void EnFr_ChildSong(EnFr* this, PlayState* play) {
    EnFr* frog;
    u8 songIndex;

    if (this->jumpCounter < 48) {
        if (this->jumpCounter % 4 == 0) {
            EnFr_SetupJumpingUp(this, sJumpOrder[(this->jumpCounter >> 2) & 7]);
        }
    } else {
        songIndex = this->songIndex;
        if (songIndex == FROG_STORMS) {
            this->actor.textId = 0x40AA;
            EnFr_SetupReward(this, play, false);
        } else if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            frog = sEnFrPointers.frogs[sSongToFrog[songIndex]];
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            if (frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
                frog->isJumpingUp = true;
                frog->isActive = true;
                Audio_PlayActorSound2(&frog->actor, NA_SE_EV_FROG_GROW_UP);
                this->actionFunc = EnFr_ChildSongFirstTime;
            } else {
                this->jumpCounter = 48;
            }
        } else {
            this->actor.textId = 0x40A9;
            EnFr_SetupReward(this, play, true);
        }
    }
}

void EnFr_ChildSongFirstTime(EnFr* this, PlayState* play) {
    EnFr* frog = sEnFrPointers.frogs[sSongToFrog[this->songIndex]];

    if (frog->isActive == false) {
        this->actor.textId = 0x40A9;
        EnFr_SetupReward(this, play, true);
    }
}

void EnFr_TalkBeforeFrogSong(EnFr* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        this->frogSongTimer = 2;
        this->actionFunc = EnFr_SetupFrogSong;
    }
}

void EnFr_CheckOcarinaInputFrogSong(u8 ocarinaNote) {
    EnFr* frog;
    s32 frogIndexButterfly;
    s32 frogIndex;

    switch (ocarinaNote) {
        case 0:
            frogIndexButterfly = FROG_BLUE;
            break;
        case 1:
            frogIndexButterfly = FROG_YELLOW;
            break;
        case 2:
            frogIndexButterfly = FROG_RED;
            break;
        case 3:
            frogIndexButterfly = FROG_PURPLE;
            break;
        case 4:
            frogIndexButterfly = FROG_WHITE;
    }
    // Turn on or off butterfly above frog
    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        frog->isButterflyDrawn = frogIndex == frogIndexButterfly ? true : false;
    }
}

void EnFr_DeactivateButterfly() {
    s32 frogIndex;
    EnFr* frog;

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frog = sEnFrPointers.frogs[frogIndex];
        frog->isButterflyDrawn = false;
    }
}

u8 EnFr_GetNextNoteFrogSong(u8 ocarinaNoteIndex) {
    if (!(gSaveContext.eventChkInf[13] & 1)) {
        return gFrogsSongPtr[ocarinaNoteIndex];
    } else {
        return sOcarinaNotes[(s32)Rand_ZeroFloat(60.0f) % 5];
    }
}

void EnFr_SetupFrogSong(EnFr* this, PlayState* play) {
    if (this->frogSongTimer != 0) {
        this->frogSongTimer--;
    } else {
        this->frogSongTimer = 40;
        this->ocarinaNoteIndex = 0;
        func_8010BD58(play, OCARINA_ACTION_FROGS);
        this->ocarinaNote = EnFr_GetNextNoteFrogSong(this->ocarinaNoteIndex);
        EnFr_CheckOcarinaInputFrogSong(this->ocarinaNote);
        this->actionFunc = EnFr_ContinueFrogSong;
    }
}

s32 EnFr_IsFrogSongComplete(EnFr* this, PlayState* play) {
    u8 index;
    u8 ocarinaNote;
    MessageContext* msgCtx = &play->msgCtx;
    u8 ocarinaNoteIndex;

    if (this->ocarinaNote == (*msgCtx).lastOcaNoteIdx) { // required to match, possibly an array?
        this->ocarinaNoteIndex++;
        ocarinaNoteIndex = this->ocarinaNoteIndex;
        if (ocarinaNoteIndex >= 14) { // Frog Song is completed
            this->ocarinaNoteIndex = 13;
            return true;
        }
        // The first four notes have more frames to receive an input
        index = ocarinaNoteIndex < 4 ? (s32)ocarinaNoteIndex : 4;
        ocarinaNote = EnFr_GetNextNoteFrogSong(ocarinaNoteIndex);
        this->ocarinaNote = ocarinaNote;
        EnFr_CheckOcarinaInputFrogSong(ocarinaNote);
        this->frogSongTimer = sTimerFrogSong[index];
    }
    return false;
}

void EnFr_OcarinaMistake(EnFr* this, PlayState* play) {
    Message_CloseTextbox(play);
    this->reward = GI_NONE;
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    func_80078884(NA_SE_SY_OCARINA_ERROR);
    Audio_OcaSetInstrument(0);
    sEnFrPointers.flags = 12;
    EnFr_DeactivateButterfly();
    this->actionFunc = EnFr_Deactivate;
}

void EnFr_ContinueFrogSong(EnFr* this, PlayState* play) {
    s32 counter;
    EnFr* frog;
    s32 i;

    if (this->frogSongTimer == 0) {
        EnFr_OcarinaMistake(this, play);
    } else {
        this->frogSongTimer--;
        if (play->msgCtx.msgMode == MSGMODE_FROGS_PLAYING) {
            counter = 0;
            for (i = 0; i < ARRAY_COUNT(sEnFrPointers.frogs); i++) {
                frog = sEnFrPointers.frogs[i];
                if (frog != NULL && frog->actionFunc == EnFr_ChooseJumpFromLogSpot) {
                    continue;
                } else {
                    counter++;
                }
            }
            if (counter == 0 && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
                EnFr_OcarinaMistake(this, play);
                return;
            }
        }

        if (play->msgCtx.msgMode == MSGMODE_FROGS_WAITING) {
            play->msgCtx.msgMode = MSGMODE_FROGS_START;
            switch (play->msgCtx.lastOcaNoteIdx) {
                case OCARINA_NOTE_D4:
                    EnFr_SetupJumpingUp(this, FROG_BLUE);
                    break;
                case OCARINA_NOTE_F4:
                    EnFr_SetupJumpingUp(this, FROG_YELLOW);
                    break;
                case OCARINA_NOTE_A4:
                    EnFr_SetupJumpingUp(this, FROG_RED);
                    break;
                case OCARINA_NOTE_B4:
                    EnFr_SetupJumpingUp(this, FROG_PURPLE);
                    break;
                case OCARINA_NOTE_D5:
                    EnFr_SetupJumpingUp(this, FROG_WHITE);
            }
            if (EnFr_IsFrogSongComplete(this, play)) {
                this->actor.textId = 0x40AC;
                EnFr_SetupReward(this, play, false);
            }
        }
    }
}

void EnFr_SetupReward(EnFr* this, PlayState* play, u8 unkCondition) {
    EnFr_DeactivateButterfly();
    if (unkCondition) {
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
    } else {
        func_80078884(NA_SE_SY_CORRECT_CHIME);
    }

    Audio_OcaSetInstrument(0);
    play->msgCtx.msgMode = MSGMODE_PAUSED;
    this->actionFunc = EnFr_PrintTextBox;
}

void EnFr_PrintTextBox(EnFr* this, PlayState* play) {
    Message_StartTextbox(play, this->actor.textId, &this->actor);
    this->actionFunc = EnFr_TalkBeforeReward;
}

void EnFr_TalkBeforeReward(EnFr* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->frogSongTimer = 100;
        Message_CloseTextbox(play);
        this->actionFunc = EnFr_SetReward;
    }
}

RandomizerCheck EnFr_RandomizerCheckFromSongIndex(u16 songIndex) {
    switch (songIndex) {
        case FROG_ZL:
            return RC_ZR_FROGS_ZELDAS_LULLABY;
        case FROG_EPONA:
            return RC_ZR_FROGS_EPONAS_SONG;
        case FROG_SARIA:
            return RC_ZR_FROGS_SARIAS_SONG;
        case FROG_SUNS:
            return RC_ZR_FROGS_SUNS_SONG;
        case FROG_SOT:
            return RC_ZR_FROGS_SONG_OF_TIME;
        default:
            return RC_UNKNOWN_CHECK;
    }
}

void EnFr_SetReward(EnFr* this, PlayState* play) {
    u16 songIndex;

    sEnFrPointers.flags = 12;
    songIndex = this->songIndex;
    this->actionFunc = EnFr_Deactivate;
    this->reward = GI_NONE;
    this->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
    if ((songIndex >= FROG_ZL) && (songIndex <= FROG_SOT)) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            if (!gSaveContext.n64ddFlag) {
                this->reward = GI_RUPEE_PURPLE;
            } else {
                this->getItemEntry = Randomizer_GetItemFromKnownCheck(EnFr_RandomizerCheckFromSongIndex(songIndex), GI_RUPEE_PURPLE);
                this->reward = this->getItemEntry.getItemId;
            }
        } else {
            this->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_STORMS) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            if (!gSaveContext.n64ddFlag) {
                this->reward = GI_HEART_PIECE;
            } else {
                this->getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZR_FROGS_IN_THE_RAIN, GI_HEART_PIECE);
                this->reward = this->getItemEntry.getItemId;
            }
        } else {
            this->reward = GI_RUPEE_BLUE;
        }
    } else if (songIndex == FROG_CHOIR_SONG) {
        if (!(gSaveContext.eventChkInf[13] & sSongIndex[songIndex])) {
            gSaveContext.eventChkInf[13] |= sSongIndex[songIndex];
            if (!gSaveContext.n64ddFlag) {
                this->reward = GI_HEART_PIECE;
            } else {
                this->getItemEntry = Randomizer_GetItemFromKnownCheck(RC_ZR_FROGS_OCARINA_GAME, GI_HEART_PIECE);
                this->reward = this->getItemEntry.getItemId;
            }
        } else {
            this->reward = GI_RUPEE_PURPLE;
        }
    }
}

void EnFr_Deactivate(EnFr* this, PlayState* play) {
    EnFr* frogLoop1;
    EnFr* frogLoop2;
    s32 frogIndex;

    // Originally was going to have separate butterfly actor
    // Changed to include butterfly as part of frog actor
    // This unused code would have frozen the butterfly actor above frog
    if (this->unusedButterflyActor != NULL) {
        this->unusedButterflyActor->freezeTimer = 10;
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frogLoop1 = sEnFrPointers.frogs[frogIndex];
        if (frogLoop1 == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There are no frogs!?"
            osSyncPrintf("%s[%d]カエルがいない！？\n", __FILE__, __LINE__);
            osSyncPrintf(VT_RST);
            return;
        } else if (frogLoop1->isDeactivating != true) {
            return;
        }
    }

    for (frogIndex = 0; frogIndex < ARRAY_COUNT(sEnFrPointers.frogs); frogIndex++) {
        frogLoop2 = sEnFrPointers.frogs[frogIndex];
        if (frogLoop2 == NULL) {
            osSyncPrintf(VT_COL(RED, WHITE));
            // "There are no frogs!?"
            osSyncPrintf("%s[%d]カエルがいない！？\n", __FILE__, __LINE__);
            osSyncPrintf(VT_RST);
            return;
        }
        frogLoop2->isDeactivating = false;
    }

    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_FROG_CRY_0);
    if (this->reward == GI_NONE) {
        this->actionFunc = EnFr_Idle;
    } else {
        this->actionFunc = EnFr_GiveReward;
        if (!gSaveContext.n64ddFlag || this->getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, this->reward, 30.0f, 100.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, play, this->getItemEntry, 30.0f, 100.0f);
        }
    }
}

void EnFr_GiveReward(EnFr* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnFr_SetIdle;
    } else {
        if (!gSaveContext.n64ddFlag || this->getItemEntry.getItemId == GI_NONE) {
            func_8002F434(&this->actor, play, this->reward, 30.0f, 100.0f);
        } else {
            GiveItemEntryFromActor(&this->actor, play, this->getItemEntry, 30.0f, 100.0f);
        }
    }
}

void EnFr_SetIdle(EnFr* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play) || (gSaveContext.n64ddFlag && this->reward == RG_ICE_TRAP)) {
        this->actionFunc = EnFr_Idle;
    }
}

void EnFr_UpdateIdle(Actor* thisx, PlayState* play) {
    EnFr* this = (EnFr*)thisx;

    if (BREG(0)) {
        DebugDisplay_AddObject(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 255, 0, 0, 255, 4, play->state.gfxCtx);
    }
    this->jumpCounter++;
    this->actionFunc(this, play);
}

s32 EnFr_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if ((limbIndex == 7) || (limbIndex == 8)) {
        *dList = NULL;
    }
    return 0;
}

void EnFr_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFr* this = (EnFr*)thisx;

    if ((limbIndex == 7) || (limbIndex == 8)) {
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_Push();
        Matrix_ReplaceRotation(&play->billboardMtxF);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_Pop();
        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnFr_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        object_fr_Tex_0059A0,
        object_fr_Tex_005BA0,
    };
    s16 lightRadius;
    EnFr* this = (EnFr*)thisx;
    s16 frogIndex = this->actor.params - 1;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    // For the frogs 2 HP, the frog with the next note and the butterfly lights up
    lightRadius = this->isButterflyDrawn ? 95 : -1;
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->posButterflyLight.x, this->posButterflyLight.y,
                              this->posButterflyLight.z, 255, 255, 255, lightRadius);
    gDPSetEnvColor(POLY_OPA_DISP++, sEnFrColor[frogIndex].r, sEnFrColor[frogIndex].g, sEnFrColor[frogIndex].b, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnFr_OverrideLimbDraw, EnFr_PostLimbDraw, this);
    if (this->isButterflyDrawn) {
        Matrix_Translate(this->posButterfly.x, this->posButterfly.y, this->posButterfly.z, MTXMODE_NEW);
        Matrix_Scale(0.015f, 0.015f, 0.015f, MTXMODE_APPLY);
        Matrix_RotateZYX(this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, MTXMODE_APPLY);
        SkelAnime_DrawOpa(play, this->skelAnimeButterfly.skeleton, this->skelAnimeButterfly.jointTable, NULL, NULL,
                          NULL);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFr_Reset(void) {
    sEnFrPointers.flags = 0;
    sEnFrPointers.frogs[0] = NULL;
    sEnFrPointers.frogs[1] = NULL;
    sEnFrPointers.frogs[2] = NULL;
    sEnFrPointers.frogs[3] = NULL;
    sEnFrPointers.frogs[4] = NULL;
}
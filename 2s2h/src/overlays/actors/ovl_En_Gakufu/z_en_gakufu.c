/*
 * File: z_en_gakufu.c
 * Overlay: ovl_En_Gakufu
 * Description: Termina Field - 2D Song Buttons Appearing on Wall
 */

#include "z_en_gakufu.h"
#include "interface/parameter_static/parameter_static.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnGakufu_Init(Actor* thisx, PlayState* play);
void EnGakufu_Destroy(Actor* thisx, PlayState* play);
void EnGakufu_Update(Actor* thisx, PlayState* play);
void EnGakufu_Draw(Actor* thisx, PlayState* play);

void EnGakufu_ProcessNotes(EnGakufu* this);
s32 EnGakufu_IsPlayerInRange(EnGakufu* this, PlayState* play);
void EnGakufu_DisplayOnTimer(EnGakufu* this, PlayState* play);
void EnGakufu_WaitForTimer(EnGakufu* this, PlayState* play);
void EnGakufu_DoNothing(EnGakufu* this, PlayState* play);
void EnGakufu_GiveReward(EnGakufu* this, PlayState* play);
void EnGakufu_PlayRewardCutscene(EnGakufu* this, PlayState* play);
void EnGakufu_WaitForSong(EnGakufu* this, PlayState* play);

ActorProfile En_Gakufu_Profile = {
    /**/ ACTOR_EN_GAKUFU,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnGakufu),
    /**/ EnGakufu_Init,
    /**/ EnGakufu_Destroy,
    /**/ EnGakufu_Update,
    /**/ EnGakufu_Draw,
};

Vec3f sRewardDropsSpawnTerminaFieldPos = {
    -710.0f,
    -123.0f,
    -3528.0f,
};

/**
 * Determines which set of `sRewardDrops` to collect when successfully playing the notes on the wall.
 * Rewards come in batches of three, and the reward you get depends on the time of day.
 */
u8 sRewardDropsIndex[] = {
    3,  // CLOCK_TIME(0, 0)  to CLOCKTIME(1, 0)
    12, // CLOCK_TIME(1, 0)  to CLOCKTIME(2, 0)
    6,  // CLOCK_TIME(2, 0)  to CLOCKTIME(3, 0)
    12, // CLOCK_TIME(3, 0)  to CLOCKTIME(4, 0)
    9,  // CLOCK_TIME(4, 0)  to CLOCKTIME(5, 0)
    12, // CLOCK_TIME(5, 0)  to CLOCKTIME(6, 0)
    0,  // CLOCK_TIME(6, 0)  to CLOCKTIME(7, 0)
    12, // CLOCK_TIME(7, 0)  to CLOCKTIME(8, 0)
    3,  // CLOCK_TIME(8, 0)  to CLOCKTIME(9, 0)
    12, // CLOCK_TIME(9, 0)  to CLOCKTIME(10, 0)
    6,  // CLOCK_TIME(10, 0) to CLOCKTIME(11, 0)
    12, // CLOCK_TIME(11, 0) to CLOCKTIME(12, 0)
    3,  // CLOCK_TIME(12, 0) to CLOCKTIME(13, 0)
    12, // CLOCK_TIME(13, 0) to CLOCKTIME(14, 0)
    6,  // CLOCK_TIME(14, 0) to CLOCKTIME(15, 0)
    12, // CLOCK_TIME(15, 0) to CLOCKTIME(16, 0)
    3,  // CLOCK_TIME(16, 0) to CLOCKTIME(17, 0)
    12, // CLOCK_TIME(17, 0) to CLOCKTIME(18, 0)
    6,  // CLOCK_TIME(18, 0) to CLOCKTIME(19, 0)
    12, // CLOCK_TIME(19, 0) to CLOCKTIME(20, 0)
    9,  // CLOCK_TIME(20, 0) to CLOCKTIME(21, 0)
    12, // CLOCK_TIME(21, 0) to CLOCKTIME(22, 0)
    6,  // CLOCK_TIME(22, 0) to CLOCKTIME(23, 0)
    12, // CLOCK_TIME(23, 0) to CLOCKTIME(0, 0)
};

/**
 * The set of rewards when the song notes on the wall are played.
 * Should be thought of as a 2D array: sRewardDrops[5][3]
 * i.e. there are 5 sets of possible rewards. And each set gives out three rewards
 */
u8 sRewardDrops[] = {
    ITEM00_RUPEE_RED,   ITEM00_RUPEE_RED,   ITEM00_RUPEE_RED,   // Set 1 (index 0)
    ITEM00_RUPEE_RED,   ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, // Set 2 (index 3)
    ITEM00_RUPEE_BLUE,  ITEM00_RUPEE_BLUE,  ITEM00_RUPEE_BLUE,  // Set 3 (index 6)
    ITEM00_RUPEE_RED,   ITEM00_RUPEE_BLUE,  ITEM00_RUPEE_BLUE,  // Set 4 (index 9)
    ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, // Set 5 (index 12)
};

// y-offsets of ocarina buttons drawn on wall
f32 sOcarinaBtnWallYOffsets[] = {
    -4.0f, -2.0f, 0.0f, 1.0f, 3.0f,
};

TexturePtr sOcarinaBtnWallTextures[] = {
    gOcarinaATex, gOcarinaCDownTex, gOcarinaCRightTex, gOcarinaCLeftTex, gOcarinaCUpTex,
};

#include "overlays/ovl_En_Gakufu/ovl_En_Gakufu.h"

void EnGakufu_ProcessNotes(EnGakufu* this) {
    OcarinaStaff* playbackStaff;
    OcarinaSongButtons* ocarinaSongButtons;
    s32 songNumButtons;
    s32 i;
    s32 songIndex;

    AudioOcarina_TerminaWallGenerateNotes();
    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_DEFAULT);
    AudioOcarina_StartDefault((1 << this->songIndex) | 0x80000000);
    playbackStaff = AudioOcarina_GetPlaybackStaff();
    playbackStaff->pos = 0;
    playbackStaff->state = 0xFF;
    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);

    songIndex = this->songIndex;
    ocarinaSongButtons = &gOcarinaSongButtons[songIndex];

    //! FAKE:
    if (1) {}
    songNumButtons = gOcarinaSongButtons[this->songIndex].numButtons;

    for (i = 0; i < (u8)songNumButtons; i++) {
        this->buttonIndex[i] = ocarinaSongButtons->buttonIndex[i];
    }

    for (; i < ARRAY_COUNT(this->buttonIndex); i++) {
        this->buttonIndex[i] = OCARINA_BTN_INVALID;
    }
}

void EnGakufu_Init(Actor* thisx, PlayState* play) {
    EnGakufu* this = (EnGakufu*)thisx;

    this->songIndex = OCARINA_SONG_TERMINA_WALL;
    EnGakufu_ProcessNotes(this);
    Actor_SetScale(&this->actor, 1.0f);

    if (GAKUFU_GET_TYPE(&this->actor) == GAKUFU_MILK_BAR) {
        this->actor.draw = NULL;
        this->actionFunc = EnGakufu_WaitForTimer;
        return;
    }

    this->actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;

    if (EnGakufu_IsPlayerInRange(this, play)) {
        SET_EVENTINF(EVENTINF_31);
    } else {
        CLEAR_EVENTINF(EVENTINF_31);
    }

    this->actionFunc = EnGakufu_WaitForSong;
    CLEAR_EVENTINF(EVENTINF_32);
}

void EnGakufu_Destroy(Actor* thisx, PlayState* play) {
    EnGakufu* this = (EnGakufu*)thisx;

    if (GAKUFU_GET_TYPE(&this->actor) != GAKUFU_MILK_BAR) {
        CLEAR_EVENTINF(EVENTINF_31);
    }
}

/**
 * Special behaviour for the Milk Bar:
 * this->actor.home.rot.x acts as a timer. Draws the notes on the back of the stage
 * until the timer runs out
 */
void EnGakufu_DisplayOnTimer(EnGakufu* this, PlayState* play) {
    if (this->actor.home.rot.x > 0) {
        this->actor.draw = EnGakufu_Draw;
        this->actor.home.rot.x--;
    } else {
        this->actor.draw = NULL;
        this->actor.home.rot.x = 0;
        this->actionFunc = EnGakufu_WaitForTimer;
    }
}

/**
 * Special behaviour for the Milk Bar:
 * this->actor.home.rot.x acts as a timer. Waits for this timer to be set,
 * then rerolls the notes and start displaying the notes
 */
void EnGakufu_WaitForTimer(EnGakufu* this, PlayState* play) {
    if (this->actor.home.rot.x > 0) {
        EnGakufu_ProcessNotes(this);
        this->actionFunc = EnGakufu_DisplayOnTimer;
    }
}

void EnGakufu_DoNothing(EnGakufu* this, PlayState* play) {
}

s32 EnGakufu_IsPlayerInRange(EnGakufu* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 600.0f) {
        return true;
    } else {
        return false;
    }
}

/**
 * Reward the player with three item drops depending on the time of day
 */
void EnGakufu_GiveReward(EnGakufu* this, PlayState* play) {
    s32 hour;
    s32 i;

    Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);

    hour = TIME_TO_HOURS_F(CURRENT_TIME);
    for (i = 0; i < 3; i++) {
        Item_DropCollectible(play, &sRewardDropsSpawnTerminaFieldPos, sRewardDrops[i + sRewardDropsIndex[hour]]);
    }

    this->actionFunc = EnGakufu_DoNothing;
}

void EnGakufu_PlayRewardCutscene(EnGakufu* this, PlayState* play) {
    if (this->actor.csId == CS_ID_NONE) {
        EnGakufu_GiveReward(this, play);
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        EnGakufu_GiveReward(this, play);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

/**
 * Waits for the notes on the wall to be played and updates the corresponding flags
 * EVENTINF_31 is used to track if Player is within range of the wall
 * EVENTINF_32 is used to track if Player has played the notes of the wall
 */
void EnGakufu_WaitForSong(EnGakufu* this, PlayState* play) {
    if (CHECK_EVENTINF(EVENTINF_31)) {
        if (CHECK_EVENTINF(EVENTINF_32)) {
            CLEAR_EVENTINF(EVENTINF_31);
            CLEAR_EVENTINF(EVENTINF_32);

            this->actionFunc = EnGakufu_PlayRewardCutscene;
            EnGakufu_PlayRewardCutscene(this, play);
            this->actor.draw = NULL;
        } else if (!EnGakufu_IsPlayerInRange(this, play)) {
            CLEAR_EVENTINF(EVENTINF_31);
        }
    } else if (EnGakufu_IsPlayerInRange(this, play)) {
        SET_EVENTINF(EVENTINF_31);
    }
}

void EnGakufu_Update(Actor* thisx, PlayState* play) {
    EnGakufu* this = (EnGakufu*)thisx;

    this->actionFunc(this, play);
}

void EnGakufu_Draw(Actor* thisx, PlayState* play) {
    s32 i;
    s32 pad;
    EnGakufu* this = (EnGakufu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);
    gSPSegment(POLY_XLU_DISP++, 0x02, play->interfaceCtx.parameterSegment);

    // Invert the whole music staff, so the notes appear correctly in mirrored world
    if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
        Matrix_Scale(-1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    }

    for (i = 0; (i < ARRAY_COUNT(this->buttonIndex)) && (this->buttonIndex[i] != OCARINA_BTN_INVALID); i++) {
        Matrix_Push();
        Matrix_Translate(30 * i - 105, sOcarinaBtnWallYOffsets[this->buttonIndex[i]] * 7.5f, 1.0f, MTXMODE_APPLY);
        Matrix_Scale(0.6f, 0.6f, 0.6f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
        gDPLoadTextureBlock(POLY_XLU_DISP++, sOcarinaBtnWallTextures[this->buttonIndex[i]], G_IM_FMT_IA, G_IM_SIZ_8b,
                            16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 4, G_TX_NOLOD,
                            G_TX_NOLOD);

        if (this->buttonIndex[i] == OCARINA_BTN_A) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 80, 150, 255, 200);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 50, 200);
        }

        gSPDisplayList(POLY_XLU_DISP++, gGakufuButtonIndexDL);

        Matrix_Pop();
    }

    gSPSegment(POLY_XLU_DISP++, 0x02, play->sceneSegment);

    CLOSE_DISPS(play->state.gfxCtx);
}

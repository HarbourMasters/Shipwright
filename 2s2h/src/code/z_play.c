#include "z64.h"
#include "regs.h"
#include "functions.h"
#include "z64vismono.h"
#include "z64visfbuf.h"
#include <libultraship/bridge/consolevariablebridge.h>

// Variables are put before most headers as a hacky way to bypass bss reordering
s16 sTransitionFillTimer;
Input D_801F6C18;
TransitionTile sTransitionTile;
s32 gTransitionTileState;
VisMono sPlayVisMono;
Color_RGBA8_u32 gVisMonoColor;
VisFbuf sPlayVisFbuf;
VisFbuf* sPlayVisFbufInstance;
BombersNotebook sBombersNotebook;
u8 sBombersNotebookOpen;
u8 sMotionBlurStatus;

#include "variables.h"
#include "z64play.h"

#include "buffers.h"
#include "idle.h"
#include "regs.h"
#include "sys_cfb.h"

#include "z64bombers_notebook.h"
#include "z64debug_display.h"
#include "z64malloc.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "z64view.h"
#include "z64vismono.h"
#include "z64visfbuf.h"

#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "debug.h"
#include "BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/Enhancements/Graphics/Graphics.h"
#include "2s2h/DeveloperTools/CollisionViewer.h"
#include "2s2h/framebuffer_effects.h"
#include <string.h>

s32 gDbgCamEnabled = false;
u8 D_801D0D54 = false;

// #region 2S2H [General] Making gPlayState available
PlayState* gPlayState;
// #endregion

// Track when the notebook is closed so we can refresh our framebuffer captures
u8 sJustClosedBomberNotebook = false;

typedef enum {
    /* 0 */ MOTION_BLUR_OFF,
    /* 1 */ MOTION_BLUR_SETUP,
    /* 2 */ MOTION_BLUR_PROCESS
} MotionBlurStatus;

void Play_DrawMotionBlur(PlayState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    s32 alpha;
    Gfx* gfx;
    Gfx* gfxHead;

    // 2S2H [Port] Frame buffer effects for motion blur
    // Track render size when blur is active and that a copy was performed
    static u32 lastBlurWidth;
    static u32 lastBlurHeight;
    static u8 hasCopiedForFrame;

    if (R_MOTION_BLUR_PRIORITY_ENABLED) {
        alpha = R_MOTION_BLUR_PRIORITY_ALPHA;

        if (sMotionBlurStatus == MOTION_BLUR_OFF) {
            sMotionBlurStatus = MOTION_BLUR_SETUP;
        }
    } else if (R_MOTION_BLUR_ENABLED) {
        alpha = R_MOTION_BLUR_ALPHA;

        if (sMotionBlurStatus == MOTION_BLUR_OFF) {
            sMotionBlurStatus = MOTION_BLUR_SETUP;
        }
    } else {
        alpha = 0;
        sMotionBlurStatus = MOTION_BLUR_OFF;
    }

    MotionBlur_Override(&sMotionBlurStatus, &alpha);

    if (sMotionBlurStatus != MOTION_BLUR_OFF) {
        OPEN_DISPS(gfxCtx);

        gfxHead = POLY_OPA_DISP;
        gfx = Graph_GfxPlusOne(gfxHead);

        gSPDisplayList(OVERLAY_DISP++, gfx);

        this->pauseBgPreRender.fbuf = gfxCtx->curFrameBuffer;
        this->pauseBgPreRender.fbufSave = this->unk_18E64;

        // 2S2H [Port] When the render size changes, we need to skip the blur for one frame to
        // avoid rendering and copying a blank framebuffer
        if (sMotionBlurStatus == MOTION_BLUR_PROCESS &&
            (lastBlurWidth != OTRGetGameRenderWidth() || lastBlurHeight != OTRGetGameRenderHeight() ||
             sJustClosedBomberNotebook)) {
            sMotionBlurStatus = MOTION_BLUR_SETUP;
        }

        if (sMotionBlurStatus == MOTION_BLUR_PROCESS) {
            FB_DrawFromFramebuffer(&gfx, gBlurFrameBuffer, alpha);
        } else {
            sMotionBlurStatus = MOTION_BLUR_PROCESS;
            lastBlurWidth = OTRGetGameRenderWidth();
            lastBlurHeight = OTRGetGameRenderHeight();
        }

        hasCopiedForFrame = false;
        u8 interpolate = CVarGetInteger("gEnhancements.Graphics.MotionBlur.Interpolate", 0);

        // 2S2H [Port] Copy framebuffer, either once per game frame or all frames depending on interpolation setting
        FB_CopyToFramebuffer(&gfx, 0, gBlurFrameBuffer, !interpolate, &hasCopiedForFrame);

        gSPEndDisplayList(gfx++);

        Graph_BranchDlist(gfxHead, gfx);

        POLY_OPA_DISP = gfx;

        CLOSE_DISPS(gfxCtx);
    }
}

void Play_InitMotionBlur(void) {
    R_MOTION_BLUR_ENABLED = false;
    R_MOTION_BLUR_PRIORITY_ENABLED = false;
    sMotionBlurStatus = MOTION_BLUR_OFF;
}

void Play_DestroyMotionBlur(void) {
    R_MOTION_BLUR_ENABLED = false;
    R_MOTION_BLUR_PRIORITY_ENABLED = false;
    sMotionBlurStatus = MOTION_BLUR_OFF;
}

void Play_SetMotionBlurAlpha(u32 alpha) {
    R_MOTION_BLUR_ALPHA = alpha;
}

void Play_EnableMotionBlur(u32 alpha) {
    R_MOTION_BLUR_ALPHA = alpha;
    R_MOTION_BLUR_ENABLED = true;
}

void Play_DisableMotionBlur(void) {
    R_MOTION_BLUR_ENABLED = false;
}

// How much each color component contributes to the intensity image.
// These coefficients are close to what the YUV color space defines Y (luminance) as:
// https://en.wikipedia.org/wiki/YUV#Conversion_to/from_RGB
#define PLAY_INTENSITY_RED 2
#define PLAY_INTENSITY_GREEN 4
#define PLAY_INTENSITY_BLUE 1
#define PLAY_INTENSITY_NORM (0x1F * PLAY_INTENSITY_RED + 0x1F * PLAY_INTENSITY_GREEN + 0x1F * PLAY_INTENSITY_BLUE)
#define PLAY_INTENSITY_MIX(r, g, b, m) \
    ((((r)*PLAY_INTENSITY_RED + (g)*PLAY_INTENSITY_GREEN + (b)*PLAY_INTENSITY_BLUE) * (m)) / PLAY_INTENSITY_NORM)

/**
 * Converts an RGBA16 buffer to an Intensity Image
 *
 * @param[out] destI destination buffer
 * @param[in] srcRgba16 source buffer
 * @param[in] rgba16Width width of a full row for the RGBA16
 * @param[in] pixelLeft X coordinate of the top-left RGBA16 pixel to start with
 * @param[in] pixelTop Y coordinate of the top-left RGBA16 pixel to start with
 * @param[in] pixelRight X coordinate of the bottom-right RGBA16 pixel to end with
 * @param[in] pixelBottom Y coordinate of the bottom-right RGBA16 pixel to end with
 * @param[in] bitDepth bit depth for the intensity image
 */
void Play_ConvertRgba16ToIntensityImage(void* destI, u16* srcRgba16, s32 rgba16Width, s32 pixelLeft, s32 pixelTop,
                                        s32 pixelRight, s32 pixelBottom, s32 bitDepth) {
    s32 i;
    s32 j;
    u32 pixel;
    u32 r;
    u32 g;
    u32 b;

    switch (bitDepth) {
        case 4: {
            u8* destI4 = destI;
            u32 upper;
            u32 lower;

            for (i = pixelTop; i <= pixelBottom; i++) {
                for (j = pixelLeft; j <= pixelRight; j += 2) {
                    pixel = srcRgba16[i * rgba16Width + j];
                    r = RGBA16_GET_R(pixel);
                    g = RGBA16_GET_G(pixel);
                    b = RGBA16_GET_B(pixel);
                    upper = PLAY_INTENSITY_MIX(r, g, b, 15);

                    pixel = srcRgba16[i * rgba16Width + j + 1];
                    r = RGBA16_GET_R(pixel);
                    g = RGBA16_GET_G(pixel);
                    b = RGBA16_GET_B(pixel);
                    lower = PLAY_INTENSITY_MIX(r, g, b, 15);

                    *(destI4++) = (upper << 4) | lower;
                }
            }
            break;
        }

        case 5: {
            u8* destI5 = destI;

            for (i = pixelTop; i <= pixelBottom; i++) {
                for (j = pixelLeft; j <= pixelRight; j++) {
                    pixel = srcRgba16[i * rgba16Width + j];
                    r = RGBA16_GET_R(pixel);
                    g = RGBA16_GET_G(pixel);
                    b = RGBA16_GET_B(pixel);

                    pixel = 0;

                    *(destI5++) = PLAY_INTENSITY_MIX(r, g, b, 255) & 0xF8;
                }
            }
            break;
        }

        case 8: {
            u8* destI8 = destI;

            for (i = pixelTop; i <= pixelBottom; i++) {
                for (j = pixelLeft; j <= pixelRight; j++) {
                    pixel = srcRgba16[i * rgba16Width + j];

                    r = RGBA16_GET_R(pixel);
                    g = RGBA16_GET_G(pixel);
                    b = RGBA16_GET_B(pixel);

                    *(destI8++) = PLAY_INTENSITY_MIX(r, g, b, 255);
                }
            }
            break;
        }

        case 16: {
            u16* destI16 = destI;

            for (i = pixelTop; i <= pixelBottom; i++) {
                for (j = pixelLeft; j <= pixelRight; j++) {
                    *(destI16++) = srcRgba16[i * rgba16Width + j];
                }
            }
            break;
        }
    }
}

void Play_SetMotionBlurPriorityAlpha(u32 alpha) {
    R_MOTION_BLUR_PRIORITY_ALPHA = alpha;
}

void Play_EnableMotionBlurPriority(u32 alpha) {
    R_MOTION_BLUR_PRIORITY_ALPHA = alpha;
    R_MOTION_BLUR_PRIORITY_ENABLED = true;
}

void Play_DisableMotionBlurPriority(void) {
    R_MOTION_BLUR_PRIORITY_ENABLED = false;
}

// Will take the photograph, but doesn't compress and save it
void Play_TriggerPictoPhoto(void) {
    R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_SETUP;
}

void Play_TakePictoPhoto(PreRender* prerender) {
    PreRender_ApplyFilters(prerender);
    Play_ConvertRgba16ToIntensityImage(gHiBuffer.pictoPhotoI8, prerender->fbufSave, SCREEN_WIDTH, PICTO_PHOTO_TOPLEFT_X,
                                       PICTO_PHOTO_TOPLEFT_Y, (PICTO_PHOTO_TOPLEFT_X + PICTO_PHOTO_WIDTH) - 1,
                                       (PICTO_PHOTO_TOPLEFT_Y + PICTO_PHOTO_HEIGHT) - 1, 8);
    GameInteractor_Should(VB_PICTO_TAKE, true, prerender);
}

s32 Play_ChooseDynamicTransition(PlayState* this, s32 transitionType) {
    s32 nextTransitionType = transitionType;

    if (transitionType == TRANS_TYPE_FADE_DYNAMIC) {
        if (!gSaveContext.save.isNight) {
            nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            nextTransitionType = TRANS_TYPE_FADE_BLACK;
        }
    }
    if (nextTransitionType != transitionType) {
        this->transitionType = nextTransitionType;
    }
    return nextTransitionType;
}

void Play_SetupTransition(PlayState* this, s32 transitionType) {
    TransitionContext* transitionCtx = &this->transitionCtx;
    s32 fbdemoType;

    memset(transitionCtx, 0, sizeof(TransitionContext));

    fbdemoType = -1;
    if (transitionType & TRANS_TYPE_WIPE3) {
        fbdemoType = FBDEMO_WIPE3;
    } else if ((transitionType & 0x78) == TRANS_TYPE_WIPE4) { // Checks not only type, but also a max value of 39
        fbdemoType = FBDEMO_WIPE4;
    } else if (!(transitionType & (TRANS_TYPE_WIPE3 | TRANS_TYPE_WIPE4))) {
        switch (transitionType) {
            case TRANS_TYPE_TRIFORCE:
                fbdemoType = FBDEMO_TRIFORCE;
                break;

            case TRANS_TYPE_WIPE:
            case TRANS_TYPE_WIPE_FAST:
                fbdemoType = FBDEMO_WIPE1;
                break;

            case TRANS_TYPE_FADE_BLACK:
            case TRANS_TYPE_FADE_WHITE:
            case TRANS_TYPE_FADE_BLACK_FAST:
            case TRANS_TYPE_FADE_WHITE_FAST:
            case TRANS_TYPE_FADE_BLACK_SLOW:
            case TRANS_TYPE_FADE_WHITE_SLOW:
            case TRANS_TYPE_FADE_WHITE_CS_DELAYED:
            case TRANS_TYPE_FADE_WHITE_INSTANT:
            case TRANS_TYPE_FADE_GREEN:
            case TRANS_TYPE_FADE_BLUE:
                fbdemoType = FBDEMO_FADE;
                break;

            case TRANS_TYPE_FILL_WHITE_FAST:
            case TRANS_TYPE_FILL_WHITE:
                this->transitionMode = TRANS_MODE_FILL_WHITE_INIT;
                break;

            case TRANS_TYPE_INSTANT:
                this->transitionMode = TRANS_MODE_INSTANT;
                break;

            case TRANS_TYPE_FILL_BROWN:
                this->transitionMode = TRANS_MODE_FILL_BROWN_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_PERSIST:
                this->transitionMode = TRANS_MODE_SANDSTORM_INIT;
                break;

            case TRANS_TYPE_SANDSTORM_END:
                this->transitionMode = TRANS_MODE_SANDSTORM_END_INIT;
                break;

            case TRANS_TYPE_CS_BLACK_FILL:
                this->transitionMode = TRANS_MODE_CS_BLACK_FILL_INIT;
                break;

            case TRANS_TYPE_CIRCLE:
                fbdemoType = FBDEMO_CIRCLE;
                break;

            case TRANS_TYPE_WIPE5:
                fbdemoType = FBDEMO_WIPE5;
                break;

            default:
                fbdemoType = -1;
                _dbg_hungup("../z_play.c", 1420);
        }
    } else {
        fbdemoType = -1;
        _dbg_hungup("../z_play.c", 1423);
    }

    transitionCtx->transitionType = transitionType;
    transitionCtx->fbdemoType = fbdemoType;
    if (fbdemoType != -1) {
        Transition_Init(transitionCtx);
    }
}

void Play_ClearTransition(PlayState* this) {
    if (this->transitionCtx.fbdemoType != -1) {
        Transition_Destroy(&this->transitionCtx);
    }
    this->transitionCtx.transitionType = -1;
}

Gfx* Play_SetFog(PlayState* this, Gfx* gfx) {
    s32 fogFar = this->lightCtx.zFar * (5.0f / 64.0f);

    return Gfx_SetFogWithSync(gfx, this->lightCtx.fogColor[0], this->lightCtx.fogColor[1], this->lightCtx.fogColor[2],
                              0, this->lightCtx.fogNear, ((fogFar <= 1000) ? 1000 : fogFar));
}

void Play_Destroy(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    if (sBombersNotebookOpen) {
        MsgEvent_SendNullTask();
        SysCfb_SetLoResMode();
        gfxCtx->curFrameBuffer = SysCfb_GetFramebuffer(gfxCtx->framebufferIndex % 2);
        gfxCtx->zbuffer = SysCfb_GetZBuffer();
        gfxCtx->viMode = gActiveViMode;
        gfxCtx->viConfigFeatures = gViConfigFeatures;
        gfxCtx->xScale = gViConfigXScale;
        gfxCtx->yScale = gViConfigYScale;
        gfxCtx->updateViMode = true;
        sBombersNotebookOpen = false;

        sJustClosedBomberNotebook = false;
    }

    BombersNotebook_Destroy(&sBombersNotebook);
    this->state.gfxCtx->callback = NULL;
    this->state.gfxCtx->callbackArg = NULL;
    Play_DestroyMotionBlur();

    if (R_PAUSE_BG_PRERENDER_STATE != PAUSE_BG_PRERENDER_OFF) {
        PreRender_ApplyFiltersSlowlyDestroy(&this->pauseBgPreRender);
        R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
    }

    R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_OFF;
    PreRender_Destroy(&this->pauseBgPreRender);
    this->unk_18E58 = NULL;
    this->pictoPhotoI8 = NULL;
    this->unk_18E60 = NULL;
    this->unk_18E64 = NULL;
    this->unk_18E68 = NULL;
    Effect_DestroyAll(this);
    EffectSs_ClearAll(this);
    CollisionCheck_DestroyContext(this, &this->colChkCtx);

    if (gTransitionTileState == TRANS_TILE_READY) {
        TransitionTile_Destroy(&sTransitionTile);
        gTransitionTileState = TRANS_TILE_OFF;
    }

    if ((this->transitionMode == TRANS_MODE_INSTANCE_RUNNING) || D_801D0D54) {
        this->transitionCtx.destroy(&this->transitionCtx.instanceData);
        Play_ClearTransition(this);
        this->transitionMode = TRANS_MODE_OFF;
    }

    ShrinkWindow_Destroy();
    TransitionFade_Destroy(&this->unk_18E48);
    VisMono_Destroy(&sPlayVisMono);
    VisFbuf_Destroy(sPlayVisFbufInstance);
    sPlayVisFbufInstance = NULL;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_92_80)) {
        Actor_CleanupContext(&this->actorCtx, this);
    }
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_92_80);

    Interface_Destroy(this);
    KaleidoScopeCall_Destroy(this);
    KaleidoManager_Destroy();
    ZeldaArena_Cleanup();

    GameInteractor_ExecuteOnPlayDestroy();

    // #region 2S2H [General] Making gPlayState available
    gPlayState = NULL;
    // #endregion
}

#define PLAY_COMPRESS_BITS 5
#define PLAY_DECOMPRESS_BITS 8

void Play_CompressI8ToI5(void* srcI8, void* destI5, size_t size) {
    u32 i;
    u8* src = srcI8;
    s8* dest = destI5;
    s32 bitsLeft = PLAY_DECOMPRESS_BITS; // Bits left in the current dest pixel left to compress into
    u32 destPixel = 0;
    s32 shift;
    u32 srcPixel;

    for (i = 0; i < size; i++) {
        srcPixel = *src++;
        srcPixel = (srcPixel * 0x1F + 0x80) / 0xFF;
        shift = bitsLeft - PLAY_COMPRESS_BITS;
        if (shift > 0) {
            destPixel |= srcPixel << shift;
        } else {
            destPixel |= srcPixel >> -shift;
            *dest++ = destPixel;
            shift += PLAY_DECOMPRESS_BITS;
            destPixel = srcPixel << shift;
        }
        bitsLeft = shift;
    }

    if (bitsLeft < PLAY_DECOMPRESS_BITS) {
        *dest = destPixel;
    }
}

void Play_DecompressI5ToI8(void* srcI5, void* destI8, size_t size) {
    u32 i;
    u8* src = srcI5;
    s8* dest = destI8;
    s32 bitsLeft = PLAY_DECOMPRESS_BITS; // Bits left in the current src pixel left to decompress
    u32 destPixel;
    s32 shift;
    u32 srcPixel = *src++;

    for (i = 0; i < size; i++) {
        shift = bitsLeft - PLAY_COMPRESS_BITS;
        if (shift > 0) {
            destPixel = 0;
            destPixel |= srcPixel >> shift;
        } else {
            destPixel = 0;
            destPixel |= srcPixel << -shift;
            srcPixel = *src++;
            shift += PLAY_DECOMPRESS_BITS;
            destPixel |= srcPixel >> shift;
        }
        destPixel = (destPixel & 0x1F) * 0xFF / 0x1F;
        *dest++ = destPixel;
        bitsLeft = shift;
    }
}

f32 Play_GetWaterSurface(PlayState* this, Vec3f* pos, s32* lightIndex) {
    Player* player = GET_PLAYER(this);
    f32 waterSurfaceY = player->actor.world.pos.y;
    WaterBox* waterBox;
    s32 bgId;

    if (!WaterBox_GetSurfaceImpl(this, &this->colCtx, pos->x, pos->z, &waterSurfaceY, &waterBox, &bgId)) {
        return BGCHECK_Y_MIN;
    }

    if (waterSurfaceY < pos->y) {
        return BGCHECK_Y_MIN;
    }

    *lightIndex = WaterBox_GetLightSettingIndex(&this->colCtx, waterBox);
    return waterSurfaceY;
}

void Play_UpdateWaterCamera(PlayState* this, Camera* camera) {
    static s16 sQuakeIndex = -1;
    static s16 sIsCameraUnderwater = false;
    s32 pad;
    s32 lightIndex;
    Player* player = GET_PLAYER(this);

    sIsCameraUnderwater = camera->stateFlags & CAM_STATE_UNDERWATER;
    if (Play_GetWaterSurface(this, &camera->eye, &lightIndex) != BGCHECK_Y_MIN) {
        if (!sIsCameraUnderwater) {
            Camera_SetStateFlag(camera, CAM_STATE_UNDERWATER);
            sQuakeIndex = -1;
            Distortion_Request(DISTORTION_TYPE_UNDERWATER_ENTRY);
            Distortion_SetDuration(80);
        }

        Audio_SetExtraFilter(0x20);
        Environment_EnableUnderwaterLights(this, lightIndex);

        if ((sQuakeIndex == -1) || (Quake_GetTimeLeft(sQuakeIndex) == 10)) {
            s16 quakeIndex = Quake_Request(camera, QUAKE_TYPE_5);

            sQuakeIndex = quakeIndex;
            if (quakeIndex != 0) {
                Quake_SetSpeed(sQuakeIndex, 550);
                Quake_SetPerturbations(sQuakeIndex, 1, 1, 180, 0);
                Quake_SetDuration(sQuakeIndex, 1000);
            }
        }
        if (player->stateFlags3 & PLAYER_STATE3_8000) {
            Distortion_Request(DISTORTION_TYPE_ZORA_SWIMMING);
            Distortion_RemoveRequest(DISTORTION_TYPE_NON_ZORA_SWIMMING);
        } else {
            Distortion_Request(DISTORTION_TYPE_NON_ZORA_SWIMMING);
            Distortion_RemoveRequest(DISTORTION_TYPE_ZORA_SWIMMING);
        }
    } else {
        if (sIsCameraUnderwater) {
            Camera_UnsetStateFlag(camera, CAM_STATE_UNDERWATER);
        }
        Distortion_RemoveRequest(DISTORTION_TYPE_NON_ZORA_SWIMMING);
        Distortion_RemoveRequest(DISTORTION_TYPE_UNDERWATER_ENTRY);
        Distortion_RemoveRequest(DISTORTION_TYPE_ZORA_SWIMMING);
        if (sQuakeIndex != 0) {
            Quake_RemoveRequest(sQuakeIndex);
        }
        Environment_DisableUnderwaterLights(this);
        Audio_SetExtraFilter(0);
    }
}

void Play_UpdateTransition(PlayState* this) {
    s32 pad;

    if (this->transitionMode == TRANS_MODE_OFF) {
        return;
    }

    switch (this->transitionMode) {
        case TRANS_MODE_SETUP:
            if (this->transitionTrigger != TRANS_TRIGGER_END) {
                s16 sceneLayer = 0;

                Interface_SetHudVisibility(HUD_VISIBILITY_NONE);

                if (gSaveContext.nextCutsceneIndex >= 0xFFF0) {
                    sceneLayer = (gSaveContext.nextCutsceneIndex & 0xF) + 1;
                }

                if ((!(Entrance_GetTransitionFlags(this->nextEntrance + sceneLayer) & 0x8000) ||
                     ((this->nextEntrance == ENTRANCE(PATH_TO_MOUNTAIN_VILLAGE, 1)) &&
                      !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) ||
                     ((this->nextEntrance == ENTRANCE(ROAD_TO_SOUTHERN_SWAMP, 1)) &&
                      !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) ||
                     ((this->nextEntrance == ENTRANCE(TERMINA_FIELD, 2)) &&
                      !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) ||
                     ((this->nextEntrance == ENTRANCE(ROAD_TO_IKANA, 1)) &&
                      !CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE))) &&
                    (!Environment_IsFinalHours(this) || (Entrance_GetSceneId(this->nextEntrance + sceneLayer) < 0) ||
                     (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_FINAL_HOURS))) {
                    Audio_MuteAllSeqExceptSystemAndOcarina(20);
                    gSaveContext.seqId = NA_BGM_DISABLED;
                    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
                }

                if (Environment_IsForcedSequenceDisabled()) {
                    Audio_MuteAllSeqExceptSystemAndOcarina(20);
                    gSaveContext.seqId = NA_BGM_DISABLED;
                    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
                }

                if (Environment_IsFinalHours(this) && (Entrance_GetSceneId(this->nextEntrance + sceneLayer) >= 0) &&
                    (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_FINAL_HOURS)) {
                    Audio_MuteSfxAndAmbienceSeqExceptSysAndOca(20);
                }
            }

            if (!D_801D0D54 && GameInteractor_Should(VB_SETUP_TRANSITION, true)) {
                Play_SetupTransition(this, Play_ChooseDynamicTransition(this, this->transitionType));
            }

            if (this->transitionMode >= TRANS_MODE_FILL_WHITE_INIT) {
                // non-instance modes break out of this switch
                break;
            }
            // fallthrough
        case TRANS_MODE_INSTANCE_INIT: {
            s32 transWipeSpeed;
            s32 transFadeDuration;
            u32 color;

            this->transitionCtx.init(&this->transitionCtx.instanceData);

            if (this->transitionCtx.transitionType & (TRANS_TYPE_WIPE3 | TRANS_TYPE_WIPE4)) {
                this->transitionCtx.setType(&this->transitionCtx.instanceData,
                                            this->transitionCtx.transitionType | TRANS_TYPE_SET_PARAMS);
            }

            if ((this->transitionCtx.transitionType == TRANS_TYPE_WIPE_FAST) ||
                (this->transitionCtx.transitionType == TRANS_TYPE_FILL_WHITE_FAST)) {
                //! @bug TRANS_TYPE_FILL_WHITE_FAST will never reach this code.
                //! It is a non-instance type transition which doesn't run this case.
                transWipeSpeed = 28;
            } else {
                transWipeSpeed = 14;
            }
            gSaveContext.transWipeSpeed = transWipeSpeed;

            switch (this->transitionCtx.transitionType) {
                case TRANS_TYPE_FADE_BLACK_FAST:
                case TRANS_TYPE_FADE_WHITE_FAST:
                    transFadeDuration = 20;
                    break;

                case TRANS_TYPE_FADE_BLACK_SLOW:
                case TRANS_TYPE_FADE_WHITE_SLOW:
                    transFadeDuration = 150;
                    break;

                case TRANS_TYPE_FADE_WHITE_INSTANT:
                    transFadeDuration = 2;
                    break;

                default:
                    transFadeDuration = 60;
                    break;
            }
            gSaveContext.transFadeDuration = transFadeDuration;

            switch (this->transitionCtx.transitionType) {
                case TRANS_TYPE_FADE_WHITE:
                case TRANS_TYPE_FADE_WHITE_FAST:
                case TRANS_TYPE_FADE_WHITE_SLOW:
                case TRANS_TYPE_FADE_WHITE_CS_DELAYED:
                case TRANS_TYPE_FADE_WHITE_INSTANT:
                    color = RGBA8(160, 160, 160, 255);
                    break;

                case TRANS_TYPE_FADE_GREEN:
                    color = RGBA8(140, 140, 100, 255);
                    break;

                case TRANS_TYPE_FADE_BLUE:
                    color = RGBA8(70, 100, 110, 255);
                    break;

                default:
                    color = RGBA8(0, 0, 0, 255);
                    break;
            }

            this->transitionCtx.setColor(&this->transitionCtx.instanceData, color);
            if (this->transitionCtx.setEnvColor != NULL) {
                this->transitionCtx.setEnvColor(&this->transitionCtx.instanceData, color);
            }

            this->transitionCtx.setType(&this->transitionCtx.instanceData,
                                        (this->transitionTrigger == TRANS_TRIGGER_END) ? TRANS_INSTANCE_TYPE_FILL_OUT
                                                                                       : TRANS_INSTANCE_TYPE_FILL_IN);
            this->transitionCtx.start(&this->transitionCtx.instanceData);

            if (this->transitionCtx.transitionType == TRANS_TYPE_FADE_WHITE_CS_DELAYED) {
                this->transitionMode = TRANS_MODE_INSTANCE_WAIT;
            } else {
                this->transitionMode = TRANS_MODE_INSTANCE_RUNNING;
            }
            break;
        }

        case TRANS_MODE_INSTANCE_RUNNING:
            if (this->transitionCtx.isDone(&this->transitionCtx.instanceData)) {
                if (this->transitionTrigger != TRANS_TRIGGER_END) {
                    if (this->transitionCtx.transitionType == TRANS_TYPE_CIRCLE) {
                        D_801D0D54 = false;
                    }

                    if (gSaveContext.gameMode == GAMEMODE_OWL_SAVE) {
                        STOP_GAMESTATE(&this->state);
                        SET_NEXT_GAMESTATE(&this->state, TitleSetup_Init, sizeof(TitleSetupState));
                    } else if (gSaveContext.gameMode != GAMEMODE_FILE_SELECT) {
                        STOP_GAMESTATE(&this->state);
                        SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));
                        gSaveContext.save.entrance = this->nextEntrance;

                        if (gSaveContext.minigameStatus == MINIGAME_STATUS_ACTIVE) {
                            gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                        }
                    } else { // GAMEMODE_FILE_SELECT
                        STOP_GAMESTATE(&this->state);
                        SET_NEXT_GAMESTATE(&this->state, FileSelect_Init, sizeof(FileSelectState));
                    }
                } else {
                    if (this->transitionCtx.transitionType == TRANS_TYPE_CIRCLE) {
                        D_801D0D54 = true;
                    } else {
                        this->transitionCtx.destroy(&this->transitionCtx.instanceData);
                        Play_ClearTransition(this);
                    }
                    this->transitionMode = TRANS_MODE_OFF;
                    if (gTransitionTileState == TRANS_TILE_READY) {
                        TransitionTile_Destroy(&sTransitionTile);
                        gTransitionTileState = TRANS_TILE_OFF;
                        GameState_SetFramerateDivisor(&this->state, 3);
                    }
                }
                this->transitionTrigger = TRANS_TRIGGER_OFF;
            } else {
                this->transitionCtx.update(&this->transitionCtx.instanceData, this->state.framerateDivisor);
            }
            break;
    }

    // update non-instance transitions
    switch (this->transitionMode) {
        case TRANS_MODE_FILL_WHITE_INIT:
            sTransitionFillTimer = 0;
            this->envCtx.fillScreen = true;
            this->envCtx.screenFillColor[0] = 160;
            this->envCtx.screenFillColor[1] = 160;
            this->envCtx.screenFillColor[2] = 160;

            if (this->transitionTrigger != TRANS_TRIGGER_END) {
                this->envCtx.screenFillColor[3] = 0;
                this->transitionMode = TRANS_MODE_FILL_IN;
            } else {
                this->envCtx.screenFillColor[3] = 255;
                this->transitionMode = TRANS_MODE_FILL_OUT;
            }
            break;

        case TRANS_MODE_FILL_IN:
            this->envCtx.screenFillColor[3] = (sTransitionFillTimer / 20.0f) * 255.0f;

            if (sTransitionFillTimer >= 20) {
                STOP_GAMESTATE(&this->state);
                SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));
                gSaveContext.save.entrance = this->nextEntrance;
                this->transitionTrigger = TRANS_TRIGGER_OFF;
                this->transitionMode = TRANS_MODE_OFF;
            } else {
                sTransitionFillTimer++;
            }
            break;

        case TRANS_MODE_FILL_OUT:
            this->envCtx.screenFillColor[3] = (1.0f - (sTransitionFillTimer / 20.0f)) * 255.0f;

            if (sTransitionFillTimer >= 20) {
                gTransitionTileState = TRANS_TILE_OFF;
                GameState_SetFramerateDivisor(&this->state, 3);
                this->transitionTrigger = TRANS_TRIGGER_OFF;
                this->transitionMode = TRANS_MODE_OFF;
                this->envCtx.fillScreen = false;
            } else {
                sTransitionFillTimer++;
            }
            break;

        case TRANS_MODE_FILL_BROWN_INIT:
            sTransitionFillTimer = 0;
            this->envCtx.fillScreen = true;
            this->envCtx.screenFillColor[0] = 170;
            this->envCtx.screenFillColor[1] = 160;
            this->envCtx.screenFillColor[2] = 150;

            if (this->transitionTrigger != TRANS_TRIGGER_END) {
                this->envCtx.screenFillColor[3] = 0;
                this->transitionMode = TRANS_MODE_FILL_IN;
            } else {
                this->envCtx.screenFillColor[3] = 255;
                this->transitionMode = TRANS_MODE_FILL_OUT;
            }
            break;

        case TRANS_MODE_INSTANT:
            if (this->transitionTrigger != TRANS_TRIGGER_END) {
                STOP_GAMESTATE(&this->state);
                SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));
                gSaveContext.save.entrance = this->nextEntrance;
                this->transitionTrigger = TRANS_TRIGGER_OFF;
                this->transitionMode = TRANS_MODE_OFF;
            } else {
                gTransitionTileState = TRANS_TILE_OFF;
                GameState_SetFramerateDivisor(&this->state, 3);
                this->transitionTrigger = TRANS_TRIGGER_OFF;
                this->transitionMode = TRANS_MODE_OFF;
            }
            break;

        case TRANS_MODE_INSTANCE_WAIT:
            if (gSaveContext.cutsceneTransitionControl != 0) {
                this->transitionMode = TRANS_MODE_INSTANCE_RUNNING;
            }
            break;

        case TRANS_MODE_SANDSTORM_INIT:
            if (this->transitionTrigger != TRANS_TRIGGER_END) {
                this->envCtx.sandstormState = SANDSTORM_FILL;
                this->transitionMode = TRANS_MODE_SANDSTORM;
            } else {
                this->envCtx.sandstormState = SANDSTORM_UNFILL;
                this->envCtx.sandstormPrimA = 255;
                this->envCtx.sandstormEnvA = 255;
                this->transitionMode = TRANS_MODE_SANDSTORM;
            }
            break;

        case TRANS_MODE_SANDSTORM:
            Audio_PlaySfx_2(NA_SE_EV_SAND_STORM - SFX_FLAG);
            if (this->transitionTrigger == TRANS_TRIGGER_END) {
                if (this->envCtx.sandstormPrimA < 110) {
                    gTransitionTileState = TRANS_TILE_OFF;
                    GameState_SetFramerateDivisor(&this->state, 3);
                    this->transitionTrigger = TRANS_TRIGGER_OFF;
                    this->transitionMode = TRANS_MODE_OFF;
                }
            } else {
                if (this->envCtx.sandstormEnvA == 255) {
                    STOP_GAMESTATE(&this->state);
                    SET_NEXT_GAMESTATE(&this->state, Play_Init, sizeof(PlayState));
                    gSaveContext.save.entrance = this->nextEntrance;
                    this->transitionTrigger = TRANS_TRIGGER_OFF;
                    this->transitionMode = TRANS_MODE_OFF;
                }
            }
            break;

        case TRANS_MODE_SANDSTORM_END_INIT:
            if (this->transitionTrigger == TRANS_TRIGGER_END) {
                this->envCtx.sandstormState = SANDSTORM_DISSIPATE;
                this->envCtx.sandstormPrimA = 255;
                this->envCtx.sandstormEnvA = 255;

                // "It's here!!!!!!!!!"
                (void)"来た!!!!!!!!!!!!!!!!!!!!!";

                this->transitionMode = TRANS_MODE_SANDSTORM_END;
            } else {
                this->transitionMode = TRANS_MODE_SANDSTORM_INIT;
            }
            break;

        case TRANS_MODE_SANDSTORM_END:
            Audio_PlaySfx_2(NA_SE_EV_SAND_STORM - SFX_FLAG);

            if (this->transitionTrigger == TRANS_TRIGGER_END) {
                if (this->envCtx.sandstormPrimA <= 0) {
                    gTransitionTileState = TRANS_TILE_OFF;
                    GameState_SetFramerateDivisor(&this->state, 3);
                    this->transitionTrigger = TRANS_TRIGGER_OFF;
                    this->transitionMode = TRANS_MODE_OFF;
                }
            }
            break;

        case TRANS_MODE_CS_BLACK_FILL_INIT:
            sTransitionFillTimer = 0;
            this->envCtx.fillScreen = true;
            this->envCtx.screenFillColor[0] = 0;
            this->envCtx.screenFillColor[1] = 0;
            this->envCtx.screenFillColor[2] = 0;
            this->envCtx.screenFillColor[3] = 255;
            this->transitionMode = TRANS_MODE_CS_BLACK_FILL;
            break;

        case TRANS_MODE_CS_BLACK_FILL:
            if (gSaveContext.cutsceneTransitionControl != 0) {
                this->envCtx.screenFillColor[3] = gSaveContext.cutsceneTransitionControl;

                if (gSaveContext.cutsceneTransitionControl <= 100) {
                    gTransitionTileState = TRANS_TILE_OFF;
                    GameState_SetFramerateDivisor(&this->state, 3);
                    this->transitionTrigger = TRANS_TRIGGER_OFF;
                    this->transitionMode = TRANS_MODE_OFF;
                }
            }
            break;
    }
}

const char D_801DFA34[][4] = {
    "all", "a",  "a",  "b",  "b",  "c",  "c",  "d",   "d",   "e",  "e",  "f",  "fa", "fa", "fb", "fb",
    "fc",  "fc", "fd", "fd", "fe", "fe", "fg", "fg",  "fh",  "fh", "fi", "fi", "fj", "fj", "fk", "fk",
    "f",   "g",  "g",  "h",  "h",  "i",  "i",  "all", "all", "a",  "b",  "c",  "d",  "e",  "f",  "g",
    "h",   "i",  "f",  "fa", "fb", "fc", "fd", "fe",  "ff",  "fg", "fh", "fi", "fj", "fk",
};

void Play_UpdateMain(PlayState* this) {
    s32 pad;
    Input* input = this->state.input;
    s32 sp5C = false;
    u8 freezeFlashTimer;

    gSegments[4] = OS_K0_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSegments[5] = OS_K0_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSegments[2] = OS_K0_TO_PHYSICAL(this->sceneSegment);

    if (R_PICTO_PHOTO_STATE == PICTO_PHOTO_STATE_PROCESS) {
        R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_READY;
        MsgEvent_SendNullTask();
        Play_TakePictoPhoto(&this->pauseBgPreRender);
        R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_OFF;
    }

    Actor_SetMovementScale(this->state.framerateDivisor);

    if (FrameAdvance_Update(&this->frameAdvCtx, &input[1])) {
        if ((this->transitionMode == TRANS_MODE_OFF) && (this->transitionTrigger != TRANS_TRIGGER_OFF)) {
            this->transitionMode = TRANS_MODE_SETUP;
        }

        if (gTransitionTileState != TRANS_TILE_OFF) {
            switch (gTransitionTileState) {
                case TRANS_TILE_PROCESS:
                    if (TransitionTile_Init(&sTransitionTile, 10, 7) == NULL) {
                        gTransitionTileState = TRANS_TILE_OFF;
                    } else {
                        sTransitionTile.zBuffer = gZBufferPtr;
                        gTransitionTileState = TRANS_TILE_READY;
                        GameState_SetFramerateDivisor(&this->state, 1);
                    }
                    break;

                case TRANS_TILE_READY:
                    TransitionTile_Update(&sTransitionTile);
                    break;

                default:
                    break;
            }
        }

        Play_UpdateTransition(this);

        if (gTransitionTileState != TRANS_TILE_READY) {
            if (((gSaveContext.gameMode == GAMEMODE_NORMAL) &&
                 (((this->msgCtx.msgMode == MSGMODE_NONE) ||
                   (((this->msgCtx.currentTextId == 0xFF) && (this->msgCtx.msgMode == MSGMODE_TEXT_DONE)) &&
                    (this->msgCtx.textboxEndType == TEXTBOX_ENDTYPE_PAUSE_MENU))) ||
                  ((this->msgCtx.currentTextId >= 0x100) && (this->msgCtx.currentTextId <= 0x200)))) &&
                (this->gameOverCtx.state == GAMEOVER_INACTIVE)) {
                KaleidoSetup_Update(this);
            }

            sp5C = IS_PAUSED(&this->pauseCtx);

            AnimTaskQueue_Reset(&this->animTaskQueue);
            Object_UpdateEntries(&this->objectCtx);

            if (!sp5C && (IREG(72) == 0)) {
                this->gameplayFrames++;
                Rumble_SetUpdateEnabled(true);

                if ((this->actorCtx.freezeFlashTimer != 0) && ((this->actorCtx.freezeFlashTimer--) < 5)) {
                    freezeFlashTimer = this->actorCtx.freezeFlashTimer;
                    if (GameInteractor_Should(VB_FLASH_SCREEN_FOR_ENEMY_KILL,
                                              (freezeFlashTimer > 0) && ((freezeFlashTimer % 2) != 0))) {
                        this->envCtx.fillScreen = true;
                        this->envCtx.screenFillColor[0] = this->envCtx.screenFillColor[1] =
                            this->envCtx.screenFillColor[2] = 150;
                        this->envCtx.screenFillColor[3] = 80;
                    } else {
                        this->envCtx.fillScreen = false;
                    }
                } else {
                    Room_ProcessRoomRequest(this, &this->roomCtx);
                    CollisionCheck_AT(this, &this->colChkCtx);
                    CollisionCheck_OC(this, &this->colChkCtx);
                    CollisionCheck_Damage(this, &this->colChkCtx);
                    CollisionCheck_ClearContext(this, &this->colChkCtx);
                    if (!this->haltAllActors) {
                        Actor_UpdateAll(this, &this->actorCtx);
                    }
                    Cutscene_UpdateManual(this, &this->csCtx);
                    Cutscene_UpdateScripted(this, &this->csCtx);
                    Effect_UpdateAll(this);
                    EffectSs_UpdateAll(this);
                    EffFootmark_Update(this);
                }
            } else {
                Rumble_SetUpdateEnabled(false);
            }

            Room_Noop(this, &this->roomCtx.curRoom, &input[1], 0);
            Room_Noop(this, &this->roomCtx.prevRoom, &input[1], 1);
            Skybox_Update(&this->skyboxCtx);

            if (IS_PAUSED(&this->pauseCtx)) {
                KaleidoScopeCall_Update(this);
            } else if (this->gameOverCtx.state != GAMEOVER_INACTIVE) {
                GameOver_Update(this);
            }

            Message_Update(this);
            Interface_Update(this);
            AnimTaskQueue_Update(this, &this->animTaskQueue);
            SoundSource_UpdateAll(this);
            ShrinkWindow_Update(this->state.framerateDivisor);
            TransitionFade_Update(&this->unk_18E48, this->state.framerateDivisor);
        }
    }

    if (!sp5C || gDbgCamEnabled) {
        s32 i;

        this->nextCamera = this->activeCamId;
        for (i = 0; i < NUM_CAMS; i++) {
            if ((i != this->nextCamera) && (this->cameraPtrs[i] != NULL)) {
                Camera_Update(this->cameraPtrs[i]);
            }
        }
        Camera_Update(this->cameraPtrs[this->nextCamera]);
    }

    if (!sp5C) {
        Play_UpdateWaterCamera(this, this->cameraPtrs[this->nextCamera]);
        Distortion_Update();
    }

    Environment_Update(this, &this->envCtx, &this->lightCtx, &this->pauseCtx, &this->msgCtx, &this->gameOverCtx,
                       this->state.gfxCtx);

    if (this->sramCtx.status != 0) {
        if (GameInteractor_Should(VB_SAVE_USE_OWL_SAVE_TIMING, gSaveContext.save.isOwlSave)) {
            Sram_UpdateWriteToFlashOwlSave(&this->sramCtx);
        } else {
            Sram_UpdateWriteToFlashDefault(&this->sramCtx);
        }
    }
}

void Play_Update(PlayState* this) {
    if (!sBombersNotebookOpen) {
        if (this->pauseCtx.bombersNotebookOpen) {
            sBombersNotebookOpen = true;
            sBombersNotebook.loadState = BOMBERS_NOTEBOOK_LOAD_STATE_NONE;
        }

        sJustClosedBomberNotebook = false;
    } else if (CHECK_BTN_ALL(CONTROLLER1(&this->state)->press.button, BTN_L) ||
               CHECK_BTN_ALL(CONTROLLER1(&this->state)->press.button, BTN_B) ||
               CHECK_BTN_ALL(CONTROLLER1(&this->state)->press.button, BTN_START) || (gIrqMgrResetStatus != 0)) {
        sBombersNotebookOpen = false;
        this->pauseCtx.bombersNotebookOpen = false;
        sBombersNotebook.loadState = BOMBERS_NOTEBOOK_LOAD_STATE_NONE;
        this->msgCtx.msgLength = 0;
        this->msgCtx.msgMode = MSGMODE_NONE;
        this->msgCtx.currentTextId = 0;
        this->msgCtx.stateTimer = 0;
        Audio_PlaySfx(NA_SE_SY_CANCEL);

        sJustClosedBomberNotebook = true;
    }
    if (sBombersNotebookOpen) {
        BombersNotebook_Update(this, &sBombersNotebook, this->state.input);
        Message_Update(this);
    } else {
        Play_UpdateMain(this);
    }
}

void Play_PostWorldDraw(PlayState* this) {
    if (IS_PAUSED(&this->pauseCtx)) {
        KaleidoScopeCall_Draw(this);
    }

    if (gSaveContext.gameMode == GAMEMODE_NORMAL) {
        Interface_Draw(this);
    }

    if (!IS_PAUSED(&this->pauseCtx) || (this->msgCtx.currentTextId != 0xFF)) {
        Message_Draw(this);
    }

    if (this->gameOverCtx.state != GAMEOVER_INACTIVE) {
        GameOver_FadeLights(this);
    }

    // Shrink the whole screen display (at the end of First and Second Day by default)
    if (gSaveContext.screenScaleFlag) {
        Gfx* nextOpa;
        Gfx* opa;
        GraphicsContext* gfxCtx = this->state.gfxCtx;

        sPlayVisFbufInstance->scale = gSaveContext.screenScale / 1000.0f;

        OPEN_DISPS(gfxCtx);

        opa = POLY_OPA_DISP;
        nextOpa = Graph_GfxPlusOne(opa);
        gSPDisplayList(OVERLAY_DISP++, nextOpa);

        VisFbuf_Draw(sPlayVisFbufInstance, &nextOpa, this->unk_18E60);

        gSPEndDisplayList(nextOpa++);
        Graph_BranchDlist(opa, nextOpa);
        POLY_OPA_DISP = nextOpa;

        CLOSE_DISPS(gfxCtx);
    }
}

void Play_DrawMain(PlayState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    Lights* lights;
    Vec3f temp;
    u8 sp25B = false;
    f32 zFar;

    // #region 2S2H [Port] Frame buffer effects for pause menu
    // Track render size when paused and that a copy was performed
    static u32 lastPauseWidth;
    static u32 lastPauseHeight;
    static bool lastAltAssets;
    static bool hasCapturedPauseBuffer;
    u8 recapturePauseBuffer = false;

    // If the size has changed, alt assets toggled or dropped frames leading to the buffer not being copied,
    // set the prerender state back to setup to copy a new frame.
    // This requires not rendering kaleido during this copy to avoid kaleido itself being copied too.
    if ((R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_PROCESS ||
         R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) &&
        (lastPauseWidth != OTRGetGameRenderWidth() || lastPauseHeight != OTRGetGameRenderHeight() ||
         lastAltAssets != ResourceMgr_IsAltAssetsEnabled() || !hasCapturedPauseBuffer || sJustClosedBomberNotebook)) {
        R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_SETUP;
        recapturePauseBuffer = true;
    }
    // #endregion

    if (R_PAUSE_BG_PRERENDER_STATE >= PAUSE_BG_PRERENDER_UNK4) {
        PreRender_ApplyFiltersSlowlyDestroy(&this->pauseBgPreRender);
        R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
    }

    if ((R_PAUSE_BG_PRERENDER_STATE <= PAUSE_BG_PRERENDER_SETUP) && (gTransitionTileState <= TRANS_TILE_SETUP)) {
        if (this->skyboxCtx.shouldDraw || (this->roomCtx.curRoom.roomShape->base.type == ROOM_SHAPE_TYPE_IMAGE)) {
            func_8012CF0C(gfxCtx, false, true, 0, 0, 0);
        } else {
            func_8012CF0C(gfxCtx, true, true, this->lightCtx.fogColor[0], this->lightCtx.fogColor[1],
                          this->lightCtx.fogColor[2]);
        }
    } else {
        func_8012CF0C(gfxCtx, false, false, 0, 0, 0);
    }

    OPEN_DISPS(gfxCtx);

    gSegments[4] = OS_K0_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSegments[5] = OS_K0_TO_PHYSICAL(this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSegments[2] = OS_K0_TO_PHYSICAL(this->sceneSegment);

    gSPSegment(POLY_OPA_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);
    gSPSegment(OVERLAY_DISP++, 0x04, this->objectCtx.slots[this->objectCtx.mainKeepSlot].segment);

    gSPSegment(POLY_OPA_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);
    gSPSegment(OVERLAY_DISP++, 0x05, this->objectCtx.slots[this->objectCtx.subKeepSlot].segment);

    gSPSegment(POLY_OPA_DISP++, 0x02, this->sceneSegment);
    gSPSegment(POLY_XLU_DISP++, 0x02, this->sceneSegment);
    gSPSegment(OVERLAY_DISP++, 0x02, this->sceneSegment);

    if (1) {
        ShrinkWindow_Draw(gfxCtx);

        POLY_OPA_DISP = Play_SetFog(this, POLY_OPA_DISP);
        POLY_XLU_DISP = Play_SetFog(this, POLY_XLU_DISP);

        zFar = this->lightCtx.zFar;
        if (zFar > 12800.0f) {
            zFar = 12800.0f;
        }

        View_SetPerspective(&this->view, this->view.fovy, this->view.zNear, zFar);

        View_Apply(&this->view, 0xF);

        // Setup mirror mode matrix handling when we are not drawing kaleido
        if (R_PAUSE_BG_PRERENDER_STATE <= PAUSE_BG_PRERENDER_SETUP && CVarGetInteger("gModes.MirroredWorld.State", 0)) {
            gSPSetExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
            gSPSetExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
            gSPMatrix(POLY_OPA_DISP++, this->view.shipMirrorProjectionPtr,
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(POLY_XLU_DISP++, this->view.shipMirrorProjectionPtr,
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
            gSPMatrix(POLY_OPA_DISP++, this->view.viewingPtr, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
            gSPMatrix(POLY_XLU_DISP++, this->view.viewingPtr, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
        }

        // The billboard matrix temporarily stores the viewing matrix
        Matrix_MtxToMtxF(&this->view.viewing, &this->billboardMtxF);
        Matrix_MtxToMtxF(&this->view.projection, &this->viewProjectionMtxF);

        this->projectionMtxFDiagonal.x = this->viewProjectionMtxF.xx;
        this->projectionMtxFDiagonal.y = this->viewProjectionMtxF.yy;
        this->projectionMtxFDiagonal.z = -this->viewProjectionMtxF.zz;

        SkinMatrix_MtxFMtxFMult(&this->viewProjectionMtxF, &this->billboardMtxF, &this->viewProjectionMtxF);

        this->billboardMtxF.mf[3][2] = this->billboardMtxF.mf[3][1] = this->billboardMtxF.mf[3][0] =
            this->billboardMtxF.mf[2][3] = this->billboardMtxF.mf[1][3] = this->billboardMtxF.mf[0][3] = 0.0f;

        Matrix_Transpose(&this->billboardMtxF);

        this->billboardMtx = GRAPH_ALLOC(this->state.gfxCtx, 2 * sizeof(Mtx));

        Matrix_MtxFToMtx(&this->billboardMtxF, this->billboardMtx);
        Matrix_RotateYF(BINANG_TO_RAD((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(this)) + 0x8000)), MTXMODE_NEW);
        Matrix_ToMtx(this->billboardMtx + 1);

        gSPSegment(POLY_OPA_DISP++, 0x01, this->billboardMtx);
        gSPSegment(POLY_XLU_DISP++, 0x01, this->billboardMtx);
        gSPSegment(OVERLAY_DISP++, 0x01, this->billboardMtx);

        if (1) {
            Gfx* sp218;
            Gfx* sp214 = POLY_OPA_DISP;

            sp218 = Graph_GfxPlusOne(sp214);
            gSPDisplayList(OVERLAY_DISP++, sp218);

            if (((this->transitionMode == TRANS_MODE_INSTANCE_RUNNING) ||
                 (this->transitionMode == TRANS_TYPE_INSTANT)) ||
                D_801D0D54) {
                View spA8;

                View_Init(&spA8, gfxCtx);
                spA8.flags = 0xA;

                SET_FULLSCREEN_VIEWPORT(&spA8);

                View_ApplyTo(&spA8, &sp218);
                this->transitionCtx.draw(&this->transitionCtx.instanceData, &sp218);
            }

            TransitionFade_Draw(&this->unk_18E48, &sp218);
            if (gVisMonoColor.a != 0) {
                // 2S2H [Port] Implement VisMono by performing a framebuffer copy and redraw with an active
                // grayscale command to set the mono color
                FB_CopyToFramebuffer(&sp218, 0, gReusableFrameBuffer, false, NULL);
                gDPSetGrayscaleColor(sp218++, gVisMonoColor.r, gVisMonoColor.g, gVisMonoColor.b, gVisMonoColor.a);
                gSPGrayscale(sp218++, true);
                FB_DrawFromFramebuffer(&sp218, gReusableFrameBuffer, 255);
                gSPGrayscale(sp218++, false);
            }

            gSPEndDisplayList(sp218++);
            Graph_BranchDlist(sp214, sp218);
            POLY_OPA_DISP = sp218;
        }

        if (gTransitionTileState == TRANS_TILE_READY) {
            Gfx* sp90 = POLY_OPA_DISP;

            TransitionTile_Draw(&sTransitionTile, &sp90);
            POLY_OPA_DISP = sp90;
            sp25B = true;
            goto PostWorldDraw;
        }

        PreRender_SetValues(&this->pauseBgPreRender, gCfbWidth, gCfbHeight, gfxCtx->curFrameBuffer, gfxCtx->zbuffer);

        if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_PROCESS) {
            MsgEvent_SendNullTask();
            if (!gSaveContext.screenScaleFlag) {
                PreRender_ApplyFiltersSlowlyInit(&this->pauseBgPreRender);
            }
            R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_READY;
            SREG(33) |= 1;

            // 2S2H [Port] Jump to the pause render flow to avoid flashing a blank screen
            // since previous framebuffers aren't retained like hardware does
            goto PauseRenderDraw;
        } else {
        PauseRenderDraw:
            if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_READY) {
                Gfx* sp8C = POLY_OPA_DISP;

                FB_DrawFromFramebuffer(&sp8C, gPauseFrameBuffer, 255);

                gSPDisplayList(sp8C++, D_0E000000_TO_SEGMENTED(syncSegments));
                POLY_OPA_DISP = sp8C;
                sp25B = true;
                goto PostWorldDraw;
            }

            GameInteractor_ExecuteOnPlayDrawWorldStart();

            if (!this->soaringCsOrSoTCsPlaying) {
                if (1) {
                    if (((u32)this->skyboxId != SKYBOX_NONE) && !this->envCtx.skyboxDisabled) {
                        if ((this->skyboxId == SKYBOX_NORMAL_SKY) || (this->skyboxId == SKYBOX_3)) {
                            Environment_UpdateSkybox(this->skyboxId, &this->envCtx, &this->skyboxCtx);
                            Skybox_Draw(&this->skyboxCtx, gfxCtx, this->skyboxId, this->envCtx.skyboxBlend,
                                        this->view.eye.x, this->view.eye.y, this->view.eye.z);
                        } else if (!this->skyboxCtx.shouldDraw) {
                            Skybox_Draw(&this->skyboxCtx, gfxCtx, this->skyboxId, 0, this->view.eye.x, this->view.eye.y,
                                        this->view.eye.z);
                        }
                    }

                    Environment_Draw(this);
                }

                lights = LightContext_NewLights(&this->lightCtx, gfxCtx);

                if (this->roomCtx.curRoom.enablePosLights || (MREG(93) != 0)) {
                    lights->enablePosLights = true;
                }

                Lights_BindAll(lights, this->lightCtx.listHead, NULL, this);
                Lights_Draw(lights, gfxCtx);

                if (1) {
                    //! FAKE:
                    u32 roomDrawFlags = ((1) ? 1 : 0) | (((void)0, 1) ? 2 : 0);

                    if (CVarGetInteger("gEnhancements.Graphics.DisableSceneGeometryDistanceCheck", 0)) {
                        gSPSetExtraGeometryMode(POLY_OPA_DISP++, G_EX_ALWAYS_EXECUTE_BRANCH);
                        gSPSetExtraGeometryMode(POLY_XLU_DISP++, G_EX_ALWAYS_EXECUTE_BRANCH);
                    }

                    Scene_Draw(this);
                    if (this->roomCtx.unk78) {
                        Room_Draw(this, &this->roomCtx.curRoom, roomDrawFlags & 3);
                        Room_Draw(this, &this->roomCtx.prevRoom, roomDrawFlags & 3);
                    }

                    if (CVarGetInteger("gEnhancements.Graphics.DisableSceneGeometryDistanceCheck", 0)) {
                        gSPClearExtraGeometryMode(POLY_OPA_DISP++, G_EX_ALWAYS_EXECUTE_BRANCH);
                        gSPClearExtraGeometryMode(POLY_XLU_DISP++, G_EX_ALWAYS_EXECUTE_BRANCH);
                    }
                }

                if (this->skyboxCtx.shouldDraw) {
                    Vec3f quakeOffset;

                    if (1) {
                        quakeOffset = Camera_GetQuakeOffset(GET_ACTIVE_CAM(this));
                        Skybox_Draw(&this->skyboxCtx, gfxCtx, this->skyboxId, 0, this->view.eye.x + quakeOffset.x,
                                    this->view.eye.y + quakeOffset.y, this->view.eye.z + quakeOffset.z);
                    }
                }

                if (this->envCtx.precipitation[PRECIP_RAIN_CUR] != 0) {
                    Environment_DrawRain(this, &this->view, gfxCtx);
                }
            }

            if (1) {
                Environment_FillScreen(gfxCtx, 0, 0, 0, this->bgCoverAlpha, FILL_SCREEN_OPA);
            }

            if (1) {
                Actor_DrawAll(this, &this->actorCtx);
            }

            if (1) {
                if (!this->envCtx.sunDisabled) {
                    temp.x = this->view.eye.x + this->envCtx.sunPos.x;
                    temp.y = this->view.eye.y + this->envCtx.sunPos.y;
                    temp.z = this->view.eye.z + this->envCtx.sunPos.z;
                    Environment_DrawSunLensFlare(this, &this->envCtx, &this->view, gfxCtx, temp);
                }

                Environment_DrawCustomLensFlare(this);
            }

            if (1) {
                if (R_PLAY_FILL_SCREEN_ON) {
                    Environment_FillScreen(gfxCtx, R_PLAY_FILL_SCREEN_R, R_PLAY_FILL_SCREEN_G, R_PLAY_FILL_SCREEN_B,
                                           R_PLAY_FILL_SCREEN_ALPHA, FILL_SCREEN_OPA | FILL_SCREEN_XLU);
                }

                switch (this->envCtx.fillScreen) {
                    case 1:
                        Environment_FillScreen(gfxCtx, this->envCtx.screenFillColor[0], this->envCtx.screenFillColor[1],
                                               this->envCtx.screenFillColor[2], this->envCtx.screenFillColor[3],
                                               FILL_SCREEN_OPA | FILL_SCREEN_XLU);
                        break;

                    default:
                        break;
                }
            }

            if (1) {
                if (this->envCtx.sandstormState != SANDSTORM_OFF) {
                    Environment_DrawSandstorm(this, this->envCtx.sandstormState);
                }
            }

            if (this->worldCoverAlpha != 0) {
                Environment_FillScreen(gfxCtx, 0, 0, 0, this->worldCoverAlpha, FILL_SCREEN_OPA | FILL_SCREEN_XLU);
            }

            if (1) {
                DebugDisplay_DrawObjects(this);
            }

            Play_DrawMotionBlur(this);

            if ((R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_SETUP) ||
                (gTransitionTileState == TRANS_TILE_SETUP) || (R_PICTO_PHOTO_STATE == PICTO_PHOTO_STATE_SETUP)) {
                Gfx* sp74;
                Gfx* sp70 = POLY_OPA_DISP;

                sp74 = Graph_GfxPlusOne(sp70);
                gSPDisplayList(OVERLAY_DISP++, sp74);
                this->pauseBgPreRender.fbuf = gfxCtx->curFrameBuffer;

                if (R_PAUSE_BG_PRERENDER_STATE == PAUSE_BG_PRERENDER_SETUP) {
                    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_PROCESS;
                    this->pauseBgPreRender.fbufSave = gfxCtx->zbuffer;
                    this->pauseBgPreRender.cvgSave = this->unk_18E58;

                    // #region 2S2H [Port] Custom handling for pause prerender background capture
                    lastPauseWidth = OTRGetGameRenderWidth();
                    lastPauseHeight = OTRGetGameRenderHeight();
                    lastAltAssets = ResourceMgr_IsAltAssetsEnabled();
                    hasCapturedPauseBuffer = false;

                    FB_CopyToFramebuffer(&sp74, 0, gPauseFrameBuffer, false, &hasCapturedPauseBuffer);

                    // Set the state back to ready after the recapture is done
                    if (recapturePauseBuffer) {
                        R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_READY;
                    }
                    // #endregion
                } else if (R_PICTO_PHOTO_STATE == PICTO_PHOTO_STATE_SETUP) {
                    R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_PROCESS;
                    this->pauseBgPreRender.fbufSave = gfxCtx->zbuffer;
                    // BENTODO: Add this back when coverage is figured out - for the anti aliasing filter
                    // this->pauseBgPreRender.cvgSave = this->unk_18E58;
                    this->pauseBgPreRender.cvgSave = NULL;

                    // #region 2S2H [Port] Custom handling for picto box capture to CPU
                    FB_WriteFramebufferSliceToCPU(&sp74, this->pauseBgPreRender.fbufSave, false);
                    // #endregion
                } else {
                    gTransitionTileState = TRANS_TILE_PROCESS;
                    this->pauseBgPreRender.fbufSave = gfxCtx->zbuffer;
                    this->pauseBgPreRender.cvgSave = NULL;

                    // BENTODO: Figure out what the transition does with framebuffers
                }

                gSPEndDisplayList(sp74++);
                Graph_BranchDlist(sp70, sp74);
                POLY_OPA_DISP = sp74;
                this->unk_18B49 = 2;
                SREG(33) |= 1;

                // 2S2H [Port] Continue to render the post world for pausing to avoid flashing the HUD
                if (gTransitionTileState == TRANS_TILE_PROCESS || R_PICTO_PHOTO_STATE == PICTO_PHOTO_STATE_PROCESS) {
                    goto SkipPostWorldDraw;
                }
            }

            // Draw Enhancements that need to be placed in the world. This happens before the PostWorldDraw
            // so that they aren't drawn when the pause menu is up (e.g. collision viewer, actor name tags)
            GameInteractor_ExecuteOnPlayDrawWorldEnd();

        PostWorldDraw:
            if (1) {
                Play_PostWorldDraw(this);
            }
        }

        if (CVarGetInteger("gModes.MirroredWorld.State", 0)) {
            gSPClearExtraGeometryMode(POLY_OPA_DISP++, G_EX_INVERT_CULLING);
            gSPClearExtraGeometryMode(POLY_XLU_DISP++, G_EX_INVERT_CULLING);
        }
    }

SkipPostWorldDraw:

    if ((this->view.unk164 != 0) && !gDbgCamEnabled) {
        Camera_Update(GET_ACTIVE_CAM(this));
        View_UpdateViewingMatrix(&this->view);
        this->view.unk164 = 0;
        if ((this->skyboxId != SKYBOX_NONE) && !this->envCtx.skyboxDisabled) {
            Skybox_UpdateMatrix(&this->skyboxCtx, this->view.eye.x, this->view.eye.y, this->view.eye.z);
        }
    }

    if (!sp25B) {
        Environment_DrawSkyboxStars(this);
    }

    CLOSE_DISPS(gfxCtx);
}

void Play_Draw(PlayState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    {
        GraphicsContext* gfxCtx2 = this->state.gfxCtx;

        if (sBombersNotebookOpen) {
            if (gSysCfbHiResEnabled != 1) {
                MsgEvent_SendNullTask();
                SysCfb_SetHiResMode();
                gfxCtx2->curFrameBuffer = SysCfb_GetFramebuffer(gfxCtx2->framebufferIndex % 2);
                gfxCtx2->zbuffer = SysCfb_GetZBuffer();
                gfxCtx2->viMode = gActiveViMode;
                gfxCtx2->viConfigFeatures = gViConfigFeatures;
                gfxCtx2->xScale = gViConfigXScale;
                gfxCtx2->yScale = gViConfigYScale;
                gfxCtx2->updateViMode = true;
            }
        } else {
            if (gSysCfbHiResEnabled != 0) {
                MsgEvent_SendNullTask();
                SysCfb_SetLoResMode();
                gfxCtx2->curFrameBuffer = SysCfb_GetFramebuffer(gfxCtx2->framebufferIndex % 2);
                gfxCtx2->zbuffer = SysCfb_GetZBuffer();
                gfxCtx2->viMode = gActiveViMode;
                gfxCtx2->viConfigFeatures = gViConfigFeatures;
                gfxCtx2->xScale = gViConfigXScale;
                gfxCtx2->yScale = gViConfigYScale;
                gfxCtx2->updateViMode = true;
            }
        }
    }

    if (sBombersNotebookOpen && ((SREG(2) != 2) || (gZBufferPtr == NULL))) {
        BombersNotebook_Draw(&sBombersNotebook, gfxCtx);
        Message_Draw(this);
    } else {
        Play_DrawMain(this);
    }
}

void Play_Main(GameState* thisx) {
    static Input* prevInput = NULL;
    PlayState* this = (PlayState*)thisx;

    prevInput = CONTROLLER1(&this->state);
    DebugDisplay_Init();

    {
        GraphicsContext* gfxCtx = this->state.gfxCtx;

        if (1) {
            this->state.gfxCtx = NULL;
        }
        Play_Update(this);
        this->state.gfxCtx = gfxCtx;
    }

    {
        Input input = *CONTROLLER1(&this->state);

        if (1) {
            *CONTROLLER1(&this->state) = D_801F6C18;
        }
        FrameInterpolation_StartRecord();
        Play_Draw(this);
        FrameInterpolation_StopRecord();
        *CONTROLLER1(&this->state) = input;
    }

    CutsceneManager_Update();
    CutsceneManager_ClearWaiting();
}

bool Play_InCsMode(PlayState* this) {
    return (this->csCtx.state != CS_STATE_IDLE) || Player_InCsMode(this);
}

f32 Play_GetFloorSurfaceImpl(PlayState* this, MtxF* mtx, CollisionPoly** poly, s32* bgId, Vec3f* pos) {
    f32 floorHeight = BgCheck_EntityRaycastFloor3(&this->colCtx, poly, bgId, pos);

    if (floorHeight > BGCHECK_Y_MIN) {
        func_800C0094(*poly, pos->x, floorHeight, pos->z, mtx);
    } else {
        mtx->xy = 0.0f;
        mtx->zx = 0.0f;
        mtx->yx = 0.0f;
        mtx->xx = 0.0f;
        mtx->wz = 0.0f;
        mtx->xz = 0.0f;
        mtx->wy = 0.0f;
        mtx->wx = 0.0f;
        mtx->zz = 0.0f;
        mtx->yz = 0.0f;
        mtx->zy = 0.0f;
        mtx->yy = 1.0f;
        mtx->xw = pos->x;
        mtx->yw = pos->y;
        mtx->zw = pos->z;
        mtx->ww = 1.0f;
    }

    return floorHeight;
}

void Play_GetFloorSurface(PlayState* this, MtxF* mtx, Vec3f* pos) {
    CollisionPoly* poly;
    s32 bgId;

    Play_GetFloorSurfaceImpl(this, mtx, &poly, &bgId, pos);
}

void* Play_LoadFile(PlayState* this, RomFile* entry) {
    size_t size = entry->vromEnd - entry->vromStart;
    void* allocp = THA_AllocTailAlign16(&this->state.tha, size);

    DmaMgr_SendRequest0(allocp, entry->vromStart, size);

    return allocp;
}

void Play_InitEnvironment(PlayState* this, s16 skyboxId) {
    Skybox_Init(&this->state, &this->skyboxCtx, skyboxId);
    Environment_Init(this, &this->envCtx, 0);
}

void OTRPlay_InitScene(PlayState* play, s32 spawn);

void Play_InitScene(PlayState* this, s32 spawn) {
    OTRPlay_InitScene(this, spawn);
#if 0
    this->curSpawn = spawn;
    this->linkActorEntry = NULL;
    this->actorCsCamList = NULL;
    this->setupEntranceList = NULL;
    this->setupExitList = NULL;
    this->naviQuestHints = NULL;
    this->setupPathList = NULL;
    this->sceneMaterialAnims = NULL;
    this->roomCtx.unk74 = NULL;
    this->numSetupActors = 0;
    Object_InitContext(&this->state, &this->objectCtx);
    LightContext_Init(this, &this->lightCtx);
    Scene_ResetTransitionActorList(&this->state, &this->transitionActors);
    Room_Init(this, &this->roomCtx);
    gSaveContext.worldMapArea = 0;
    Scene_ExecuteCommands(this, this->sceneSegment);
    Play_InitEnvironment(this, this->skyboxId);
#endif
}

void OTRPlay_SpawnScene(PlayState* play, s32 sceneId, s32 spawn);

void Play_SpawnScene(PlayState* this, s32 sceneId, s32 spawn) {
    OTRPlay_SpawnScene(this, sceneId, spawn);
#if 0
    s32 pad;
    SceneTableEntry* scene = &gSceneTable[sceneId];

    scene->unk_D = 0;
    this->loadedScene = scene;
    this->sceneId = sceneId;
    this->sceneConfig = scene->drawConfig;
    this->sceneSegment = Play_LoadFile(this, &scene->segment);
    scene->unk_D = 0;
    gSegments[2] = VIRTUAL_TO_PHYSICAL(this->sceneSegment);
    Play_InitScene(this, spawn);
    Room_SetupFirstRoom(this, &this->roomCtx);
#endif
}

void Play_GetScreenPos(PlayState* this, Vec3f* worldPos, Vec3f* screenPos) {
    f32 invW;

    // screenPos temporarily stores the projectedPos
    Actor_GetProjectedPos(this, worldPos, screenPos, &invW);

    screenPos->x = (SCREEN_WIDTH / 2) + (screenPos->x * invW * (SCREEN_WIDTH / 2));
    screenPos->y = (SCREEN_HEIGHT / 2) - (screenPos->y * invW * (SCREEN_HEIGHT / 2));
}

s16 Play_CreateSubCamera(PlayState* this) {
    s16 subCamId;

    for (subCamId = CAM_ID_SUB_FIRST; subCamId < NUM_CAMS; subCamId++) {
        if (this->cameraPtrs[subCamId] == NULL) {
            break;
        }
    }

    // if no subCameras available
    if (subCamId == NUM_CAMS) {
        return CAM_ID_NONE;
    }

    this->cameraPtrs[subCamId] = &this->subCameras[subCamId - CAM_ID_SUB_FIRST];
    Camera_Init(this->cameraPtrs[subCamId], &this->view, &this->colCtx, this);
    this->cameraPtrs[subCamId]->camId = subCamId;

    return subCamId;
}

s16 Play_GetActiveCamId(PlayState* this) {
    return this->activeCamId;
}

s32 Play_ChangeCameraStatus(PlayState* this, s16 camId, s16 status) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    if (status == CAM_STATUS_ACTIVE) {
        this->activeCamId = camIdx;
    }

    return Camera_ChangeStatus(this->cameraPtrs[camIdx], status);
}

void Play_ClearCamera(PlayState* this, s16 camId) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    if (this->cameraPtrs[camIdx] != NULL) {
        Camera_ChangeStatus(this->cameraPtrs[camIdx], CAM_STATUS_INACTIVE);
        this->cameraPtrs[camIdx] = NULL;
    }
}

void Play_ClearAllSubCameras(PlayState* this) {
    s16 subCamId;

    for (subCamId = CAM_ID_SUB_FIRST; subCamId < NUM_CAMS; subCamId++) {
        if (this->cameraPtrs[subCamId] != NULL) {
            Play_ClearCamera(this, subCamId);
        }
    }

    this->activeCamId = CAM_ID_MAIN;
}

Camera* Play_GetCamera(PlayState* this, s16 camId) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    return this->cameraPtrs[camIdx];
}

/**
 * @return bit-packed success if each of the params were applied
 */
s32 Play_SetCameraAtEye(PlayState* this, s16 camId, Vec3f* at, Vec3f* eye) {
    if (!GameInteractor_Should(VB_SET_CAMERA_AT_EYE, true)) {
        return false;
    }
    s32 successfullySet = 0;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];

    successfullySet |= Camera_SetViewParam(camera, CAM_VIEW_AT, at);
    successfullySet <<= 1;
    successfullySet |= Camera_SetViewParam(camera, CAM_VIEW_EYE, eye);

    camera->dist = Math3D_Vec3f_DistXYZ(at, eye);

    if (camera->focalActor != NULL) {
        camera->focalActorAtOffset.x = at->x - camera->focalActor->world.pos.x;
        camera->focalActorAtOffset.y = at->y - camera->focalActor->world.pos.y;
        camera->focalActorAtOffset.z = at->z - camera->focalActor->world.pos.z;
    } else {
        camera->focalActorAtOffset.x = camera->focalActorAtOffset.y = camera->focalActorAtOffset.z = 0.0f;
    }

    camera->atLerpStepScale = 0.01f;

    return successfullySet;
}

/**
 * @return bit-packed success if each of the params were applied
 */
s32 Play_SetCameraAtEyeUp(PlayState* this, s16 camId, Vec3f* at, Vec3f* eye, Vec3f* up) {
    s32 successfullySet = 0;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];

    successfullySet |= Camera_SetViewParam(camera, CAM_VIEW_AT, at);
    successfullySet <<= 1;
    successfullySet |= Camera_SetViewParam(camera, CAM_VIEW_EYE, eye);
    successfullySet <<= 1;
    successfullySet |= Camera_SetViewParam(camera, CAM_VIEW_UP, up);

    camera->dist = Math3D_Vec3f_DistXYZ(at, eye);

    if (camera->focalActor != NULL) {
        camera->focalActorAtOffset.x = at->x - camera->focalActor->world.pos.x;
        camera->focalActorAtOffset.y = at->y - camera->focalActor->world.pos.y;
        camera->focalActorAtOffset.z = at->z - camera->focalActor->world.pos.z;
    } else {
        camera->focalActorAtOffset.x = camera->focalActorAtOffset.y = camera->focalActorAtOffset.z = 0.0f;
    }

    camera->atLerpStepScale = 0.01f;

    return successfullySet;
}

/**
 * @return true if the fov was successfully set
 */
s32 Play_SetCameraFov(PlayState* this, s16 camId, f32 fov) {
    if (!GameInteractor_Should(VB_SET_CAMERA_FOV, true)) {
        return false;
    }
    s32 successfullySet = Camera_SetViewParam(this->cameraPtrs[camId], CAM_VIEW_FOV, &fov) & 1;

    if (1) {}
    return successfullySet;
}

s32 Play_SetCameraRoll(PlayState* this, s16 camId, s16 roll) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    Camera* camera = this->cameraPtrs[camIdx];

    camera->roll = roll;

    return 1;
}

void Play_CopyCamera(PlayState* this, s16 destCamId, s16 srcCamId) {
    s16 srcCamId2 = (srcCamId == CAM_ID_NONE) ? this->activeCamId : srcCamId;
    s16 destCamId1 = (destCamId == CAM_ID_NONE) ? this->activeCamId : destCamId;

    Camera_Copy(this->cameraPtrs[destCamId1], this->cameraPtrs[srcCamId2]);
}

// Same as Play_ChangeCameraSetting but also calls Camera_InitFocalActorSettings
s32 func_80169A50(PlayState* this, s16 camId, Player* player, s16 setting) {
    Camera* camera;
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;

    camera = this->cameraPtrs[camIdx];
    Camera_InitFocalActorSettings(camera, &player->actor);
    return Camera_ChangeSetting(camera, setting);
}

s32 Play_ChangeCameraSetting(PlayState* this, s16 camId, s16 setting) {
    return Camera_ChangeSetting(Play_GetCamera(this, camId), setting);
}

// Related to bosses and fishing
void func_80169AFC(PlayState* this, s16 camId, s16 timer) {
    s16 camIdx = (camId == CAM_ID_NONE) ? this->activeCamId : camId;
    s16 i;

    Play_ClearCamera(this, camIdx);

    for (i = CAM_ID_SUB_FIRST; i < NUM_CAMS; i++) {
        if (this->cameraPtrs[i] != NULL) {
            Play_ClearCamera(this, i);
        }
    }

    if (timer <= 0) {
        Play_ChangeCameraStatus(this, CAM_ID_MAIN, CAM_STATUS_ACTIVE);
        this->cameraPtrs[CAM_ID_MAIN]->childCamId = this->cameraPtrs[CAM_ID_MAIN]->doorTimer2 = 0;
    }
}

s16 Play_GetCameraUID(PlayState* this, s16 camId) {
    Camera* camera = this->cameraPtrs[camId];

    if (camera != NULL) {
        return camera->uid;
    } else {
        return -1;
    }
}

// Unused in both MM and OoT, purpose is very unclear
s16 func_80169BF8(PlayState* this, s16 camId, s16 uid) {
    Camera* camera = this->cameraPtrs[camId];

    if (camera != NULL) {
        return 0;
    } else if (camera->uid != uid) {
        return 0;
    } else if (camera->status != CAM_STATUS_ACTIVE) {
        return 2;
    } else {
        return 1;
    }
}

u16 Play_GetActorCsCamSetting(PlayState* this, s32 csCamDataIndex) {
    ActorCsCamInfo* actorCsCamList = &this->actorCsCamList[csCamDataIndex];

    return actorCsCamList->setting;
}

Vec3s* Play_GetActorCsCamFuncData(PlayState* this, s32 csCamDataIndex) {
    ActorCsCamInfo* actorCsCamList = &this->actorCsCamList[csCamDataIndex];

    return Lib_SegmentedToVirtual(actorCsCamList->actorCsCamFuncData);
}

/**
 * Converts the number of a scene to its "original" equivalent, the default version of the area which the player first
 * enters.
 */
s16 Play_GetOriginalSceneId(s16 sceneId) {
    // Inverted Stone Tower Temple -> Stone Tower Temple
    if (sceneId == SCENE_INISIE_R) {
        return SCENE_INISIE_N;
    }

    // Purified Southern Swamp -> Poisoned Sothern Swamp
    if (sceneId == SCENE_20SICHITAI2) {
        return SCENE_20SICHITAI;
    }

    // Spring Mountain Village -> Winter Mountain Village
    if (sceneId == SCENE_10YUKIYAMANOMURA2) {
        return SCENE_10YUKIYAMANOMURA;
    }

    // Spring Goron Village -> Winter Goron Village
    if (sceneId == SCENE_11GORONNOSATO2) {
        return SCENE_11GORONNOSATO;
    }

    // Spring Path to Goron Village -> Winter Path to Goron Village
    if (sceneId == SCENE_17SETUGEN2) {
        return SCENE_17SETUGEN;
    }

    // Inverted Stone Tower -> Stone Tower
    if (sceneId == SCENE_F41) {
        return SCENE_F40;
    }

    return sceneId;
}

/**
 * Copies the flags set in ActorContext over to the current scene's CycleSceneFlags, usually using the original scene
 * number. Exception for Inverted Stone Tower Temple, which uses its own.
 */
void Play_SaveCycleSceneFlags(PlayState* this) {
    CycleSceneFlags* cycleSceneFlags;

    cycleSceneFlags = &gSaveContext.cycleSceneFlags[Play_GetOriginalSceneId(this->sceneId)];
    cycleSceneFlags->chest = this->actorCtx.sceneFlags.chest;
    cycleSceneFlags->switch0 = this->actorCtx.sceneFlags.switches[0];
    cycleSceneFlags->switch1 = this->actorCtx.sceneFlags.switches[1];

    if (this->sceneId == SCENE_INISIE_R) { // Inverted Stone Tower Temple
        cycleSceneFlags = &gSaveContext.cycleSceneFlags[this->sceneId];
    }

    cycleSceneFlags->collectible = this->actorCtx.sceneFlags.collectible[0];
    cycleSceneFlags->clearedRoom = this->actorCtx.sceneFlags.clearedRoom;
}

void Play_SetRespawnData(PlayState* this, s32 respawnMode, u16 entrance, s32 roomIndex, s32 playerParams, Vec3f* pos,
                         s16 yaw) {

    gSaveContext.respawn[respawnMode].entrance = Entrance_Create(entrance >> 9, 0, entrance & 0xF);
    gSaveContext.respawn[respawnMode].roomIndex = roomIndex;
    gSaveContext.respawn[respawnMode].pos = *pos;
    gSaveContext.respawn[respawnMode].yaw = yaw;
    gSaveContext.respawn[respawnMode].playerParams = playerParams;
    gSaveContext.respawn[respawnMode].tempSwitchFlags = this->actorCtx.sceneFlags.switches[2];
    gSaveContext.respawn[respawnMode].unk_18 = this->actorCtx.sceneFlags.collectible[1];
    gSaveContext.respawn[respawnMode].tempCollectFlags = this->actorCtx.sceneFlags.collectible[2];
}

void Play_SetupRespawnPoint(PlayState* this, s32 respawnMode, s32 playerParams) {
    Player* player = GET_PLAYER(this);

    if (this->sceneId != SCENE_KAKUSIANA) { // Grottos
        Play_SetRespawnData(this, respawnMode, ((void)0, gSaveContext.save.entrance), this->roomCtx.curRoom.num,
                            playerParams, &player->actor.world.pos, player->actor.shape.rot.y);
    }
}

// Override respawn data in Sakon's Hideout
void func_80169ECC(PlayState* this) {
    if (this->sceneId == SCENE_SECOM) {
        this->nextEntrance = ENTRANCE(IKANA_CANYON, 6);
        gSaveContext.respawnFlag = -7;
    }
}

// Gameplay_TriggerVoidOut ?
// Used by Player, Ikana_Rotaryroom, Bji01, Kakasi, LiftNuts, Test4, Warptag, WarpUzu, Roomtimer
void func_80169EFC(PlayState* this) {
    gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwitchFlags = this->actorCtx.sceneFlags.switches[2];
    gSaveContext.respawn[RESPAWN_MODE_DOWN].unk_18 = this->actorCtx.sceneFlags.collectible[1];
    gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = this->actorCtx.sceneFlags.collectible[2];
    this->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance;
    gSaveContext.respawnFlag = 1;
    func_80169ECC(this);
    this->transitionTrigger = TRANS_TRIGGER_START;
    this->transitionType = TRANS_TYPE_FADE_BLACK;
}

// Gameplay_LoadToLastEntrance ?
// Used by game_over and Test7
void func_80169F78(PlayState* this) {
    this->nextEntrance = gSaveContext.respawn[RESPAWN_MODE_TOP].entrance;
    gSaveContext.respawnFlag = -1;
    func_80169ECC(this);
    this->transitionTrigger = TRANS_TRIGGER_START;
    this->transitionType = TRANS_TYPE_FADE_BLACK;
}

// Gameplay_TriggerRespawn ?
// Used for void by Wallmaster, Deku Shrine doors. Also used by Player, Kaleido, DoorWarp1
void func_80169FDC(PlayState* this) {
    func_80169F78(this);
}

s32 Play_CamIsNotFixed(PlayState* this) {
    return this->roomCtx.curRoom.roomShape->base.type != ROOM_SHAPE_TYPE_IMAGE;
}

s32 FrameAdvance_IsEnabled(PlayState* this) {
    return this->frameAdvCtx.enabled != false;
}

// Unused, unchanged from OoT, which uses it only in one Camera function.
/**
 * @brief Tests if \p actor is a door and the sides are different rooms.
 *
 * @param[in] thisx GameState, promoted to play inside.
 * @param[in] actor Actor to test.
 * @param[out] yaw Facing angle of the actor, or reverse if in the back room.
 * @return true if \p actor is a door and the sides are in different rooms, false otherwise
 */
s32 func_8016A02C(PlayState* this, Actor* actor, s16* yaw) {
    TransitionActorEntry* transitionActor;
    s8 frontRoom;

    if (actor->category != ACTORCAT_DOOR) {
        return false;
    }

    transitionActor = &this->transitionActors.list[(u16)actor->params >> 10];
    frontRoom = transitionActor->sides[0].room;
    if (frontRoom == transitionActor->sides[1].room) {
        return false;
    }

    if (frontRoom == actor->room) {
        *yaw = actor->shape.rot.y;
    } else {
        *yaw = actor->shape.rot.y + 0x8000;
    }

    return true;
}

// Unused
/**
 * @brief Tests if \p pos is underwater.
 *
 * @param[in] this PlayState
 * @param[in] pos position to test
 * @return true if inside a waterbox and not above a void.
 */
s32 Play_IsUnderwater(PlayState* this, Vec3f* pos) {
    WaterBox* waterBox;
    CollisionPoly* poly;
    Vec3f waterSurfacePos;
    s32 bgId;

    waterSurfacePos = *pos;

    if ((WaterBox_GetSurface1(this, &this->colCtx, waterSurfacePos.x, waterSurfacePos.z, &waterSurfacePos.y,
                              &waterBox) == true) &&
        (pos->y < waterSurfacePos.y) &&
        (BgCheck_EntityRaycastFloor3(&this->colCtx, &poly, &bgId, &waterSurfacePos) != BGCHECK_Y_MIN)) {
        return true;
    } else {
        return false;
    }
}

s32 Play_IsDebugCamEnabled(void) {
    return gDbgCamEnabled;
}

// A mapping from playerCsIds to sGlobalCamDataSettings indices.
s16 sPlayerCsIdToCsCamId[] = {
    CS_CAM_ID_GLOBAL_ITEM_OCARINA,        // PLAYER_CS_ID_ITEM_OCARINA
    CS_CAM_ID_GLOBAL_ITEM_GET,            // PLAYER_CS_ID_ITEM_GET
    CS_CAM_ID_GLOBAL_ITEM_BOTTLE,         // PLAYER_CS_ID_ITEM_BOTTLE
    CS_CAM_ID_GLOBAL_ITEM_SHOW,           // PLAYER_CS_ID_ITEM_SHOW
    CS_CAM_ID_GLOBAL_WARP_PAD_MOON,       // PLAYER_CS_ID_WARP_PAD_MOON
    CS_CAM_ID_GLOBAL_MASK_TRANSFORMATION, // PLAYER_CS_ID_MASK_TRANSFORMATION
    CS_CAM_ID_GLOBAL_DEATH,               // PLAYER_CS_ID_DEATH
    CS_CAM_ID_GLOBAL_REVIVE,              // PLAYER_CS_ID_REVIVE
    CS_CAM_ID_GLOBAL_SONG_WARP,           // PLAYER_CS_ID_SONG_WARP
    CS_CAM_ID_GLOBAL_WARP_PAD_ENTRANCE,   // PLAYER_CS_ID_WARP_PAD_ENTRANCE
};

// Used by Player
/**
 * Extract the common cutscene ids used by Player from the scene and set the cutscene ids in this->playerCsIds.
 * If a playerCsId is not present in the scene, then that particular id is set to CS_ID_NONE.
 * Otherwise, if there is an ActorCutscene where csCamId matches the appropriate element of sPlayerCsIdToCsCamId,
 * set the corresponding playerActorCsId (and possibly change its priority for the zeroth one).
 */
void Play_AssignPlayerCsIdsFromScene(PlayState* this, s32 spawnCsId) {
    s32 i;
    s16* curPlayerCsId = this->playerCsIds;
    s16* csCamId = sPlayerCsIdToCsCamId;

    for (i = 0; i < ARRAY_COUNT(this->playerCsIds); i++, curPlayerCsId++, csCamId++) {
        ActorCutscene* csEntry;
        s32 curCsId;

        *curPlayerCsId = CS_ID_NONE;

        for (curCsId = spawnCsId; curCsId != CS_ID_NONE; curCsId = csEntry->additionalCsId) {
            csEntry = CutsceneManager_GetCutsceneEntry(curCsId);

            if (csEntry->csCamId == *csCamId) {
                if ((csEntry->csCamId == CS_CAM_ID_GLOBAL_ITEM_OCARINA) && (csEntry->priority == 700)) {
                    csEntry->priority = 550;
                }
                *curPlayerCsId = curCsId;
                break;
            }
        }
    }
}

// Set values to fill screen
void Play_FillScreen(PlayState* this, s16 fillScreenOn, u8 red, u8 green, u8 blue, u8 alpha) {
    R_PLAY_FILL_SCREEN_ON = fillScreenOn;
    R_PLAY_FILL_SCREEN_R = red;
    R_PLAY_FILL_SCREEN_G = green;
    R_PLAY_FILL_SCREEN_B = blue;
    R_PLAY_FILL_SCREEN_ALPHA = alpha;
}

void Play_Init(GameState* thisx) {
    PlayState* this = (PlayState*)thisx;
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    s32 pad;
    uintptr_t zAlloc;
    s32 zAllocSize;
    Player* player;
    s32 i;
    s32 scene;
    u8 sceneLayer;
    s32 pad2;

    if ((gSaveContext.respawnFlag == -4) || (gSaveContext.respawnFlag == -0x63)) {
        if (CHECK_EVENTINF(EVENTINF_TRIGGER_DAYTELOP)) {
            CLEAR_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
            STOP_GAMESTATE(&this->state);
            SET_NEXT_GAMESTATE(&this->state, DayTelop_Init, sizeof(DayTelopState));
            return;
        }

        gSaveContext.unk_3CA7 = 1;
        if (gSaveContext.respawnFlag == -0x63) {
            gSaveContext.respawnFlag = 2;
        }
    } else {
        gSaveContext.unk_3CA7 = 0;
    }

    if (gSaveContext.save.entrance == -1) {
        gSaveContext.save.entrance = 0;
        STOP_GAMESTATE(&this->state);
        SET_NEXT_GAMESTATE(&this->state, TitleSetup_Init, sizeof(TitleSetupState));
        return;
    }

    // #region 2S2H [General] Making gPlayState available
    // Setting after the early returns, so that Play_Destroy is registered to unset the ptr later
    gPlayState = this;
    // #endregion

    if ((gSaveContext.nextCutsceneIndex == 0xFFEF) || (gSaveContext.nextCutsceneIndex == 0xFFF0)) {
        scene = ((void)0, gSaveContext.save.entrance) >> 9;

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
            if (scene == ENTR_SCENE_MOUNTAIN_VILLAGE_WINTER) {
                scene = ENTR_SCENE_MOUNTAIN_VILLAGE_SPRING;
            } else if (scene == ENTR_SCENE_GORON_VILLAGE_WINTER) {
                scene = ENTR_SCENE_GORON_VILLAGE_SPRING;
            } else if (scene == ENTR_SCENE_PATH_TO_GORON_VILLAGE_WINTER) {
                scene = ENTR_SCENE_PATH_TO_GORON_VILLAGE_SPRING;
            } else if ((scene == ENTR_SCENE_SNOWHEAD) || (scene == ENTR_SCENE_PATH_TO_SNOWHEAD) ||
                       (scene == ENTR_SCENE_PATH_TO_MOUNTAIN_VILLAGE) || (scene == ENTR_SCENE_GORON_SHRINE) ||
                       (scene == ENTR_SCENE_GORON_RACETRACK)) {
                gSaveContext.nextCutsceneIndex = 0xFFF0;
            }
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            if (scene == ENTR_SCENE_SOUTHERN_SWAMP_POISONED) {
                scene = ENTR_SCENE_SOUTHERN_SWAMP_CLEARED;
            } else if (scene == ENTR_SCENE_WOODFALL) {
                gSaveContext.nextCutsceneIndex = 0xFFF1;
            }
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE) && (scene == ENTR_SCENE_IKANA_CANYON)) {
            gSaveContext.nextCutsceneIndex = 0xFFF2;
        }

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE) &&
            ((scene == ENTR_SCENE_GREAT_BAY_COAST) || (scene == ENTR_SCENE_ZORA_CAPE))) {
            gSaveContext.nextCutsceneIndex = 0xFFF0;
        }

        // "First cycle" Termina Field
        if (GameInteractor_Should(VB_TERMINA_FIELD_BE_EMPTY,
                                  INV_CONTENT(ITEM_OCARINA_OF_TIME) != ITEM_OCARINA_OF_TIME)) {
            if ((scene == ENTR_SCENE_TERMINA_FIELD) &&
                (((void)0, gSaveContext.save.entrance) != ENTRANCE(TERMINA_FIELD, 10))) {
                gSaveContext.nextCutsceneIndex = 0xFFF4;
            }
        }
        gSaveContext.save.entrance = Entrance_Create(scene, (((void)0, gSaveContext.save.entrance) >> 4) & 0x1F,
                                                     ((void)0, gSaveContext.save.entrance) & 0xF);
    }

    GameState_Realloc(&this->state, 0);
    KaleidoManager_Init(this);
    ShrinkWindow_Init();
    View_Init(&this->view, gfxCtx);
    Audio_SetExtraFilter(0);
    Quake_Init();
    Distortion_Init(this);

    for (i = 0; i < ARRAY_COUNT(this->cameraPtrs); i++) {
        this->cameraPtrs[i] = NULL;
    }

    Camera_Init(&this->mainCamera, &this->view, &this->colCtx, this);
    Camera_ChangeStatus(&this->mainCamera, CAM_STATUS_ACTIVE);

    for (i = 0; i < ARRAY_COUNT(this->subCameras); i++) {
        Camera_Init(&this->subCameras[i], &this->view, &this->colCtx, this);
        Camera_ChangeStatus(&this->subCameras[i], CAM_STATUS_INACTIVE);
    }

    this->cameraPtrs[CAM_ID_MAIN] = &this->mainCamera;
    this->cameraPtrs[CAM_ID_MAIN]->uid = CAM_ID_MAIN;
    this->activeCamId = CAM_ID_MAIN;

    Camera_OverwriteStateFlags(&this->mainCamera, CAM_STATE_0 | CAM_STATE_CHECK_WATER | CAM_STATE_2 | CAM_STATE_3 |
                                                      CAM_STATE_4 | CAM_STATE_DISABLE_MODE_CHANGE | CAM_STATE_6);
    Sram_Alloc(&this->state, &this->sramCtx);
    Regs_InitData(this);
    Message_Init(this);
    GameOver_Init(this);
    SoundSource_InitAll(this);
    EffFootmark_Init(this);
    Effect_Init(this);
    EffectSs_InitInfo(this, 100);
    CollisionCheck_InitContext(this, &this->colChkCtx);
    AnimTaskQueue_Reset(&this->animTaskQueue);
    Cutscene_InitContext(this, &this->csCtx);

    if (gSaveContext.nextCutsceneIndex != 0xFFEF) {
        gSaveContext.save.cutsceneIndex = gSaveContext.nextCutsceneIndex;
        gSaveContext.nextCutsceneIndex = 0xFFEF;
    }

    if (gSaveContext.save.cutsceneIndex == 0xFFFD) {
        gSaveContext.save.cutsceneIndex = 0;
    }

    if (gSaveContext.nextDayTime != NEXT_TIME_NONE) {
        gSaveContext.save.time = gSaveContext.nextDayTime;
        gSaveContext.skyboxTime = gSaveContext.nextDayTime;
    }

    if ((CURRENT_TIME >= CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 30))) {
        gSaveContext.save.isNight = true;
    } else {
        gSaveContext.save.isNight = false;
    }

    func_800EDDB0(this);

    if (((gSaveContext.gameMode != GAMEMODE_NORMAL) && (gSaveContext.gameMode != GAMEMODE_TITLE_SCREEN)) ||
        (gSaveContext.save.cutsceneIndex >= 0xFFF0)) {
        gSaveContext.nayrusLoveTimer = 0;
        Magic_Reset(this);
        gSaveContext.sceneLayer = (gSaveContext.save.cutsceneIndex & 0xF) + 1;

        // Set saved cutscene to 0 so it doesn't immediately play, but instead let the `CutsceneManager` handle it.
        gSaveContext.save.cutsceneIndex = 0;
    } else {
        gSaveContext.sceneLayer = 0;
    }

    sceneLayer = gSaveContext.sceneLayer;

    s32 sceneIdAbsolute =
        Entrance_GetSceneIdAbsolute(((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer));
    s32 spawnNum = Entrance_GetSpawnNum(((void)0, gSaveContext.save.entrance) + ((void)0, gSaveContext.sceneLayer));
    Play_SpawnScene(this, sceneIdAbsolute, spawnNum);

    KaleidoScopeCall_Init(this);
    Interface_Init(this);

    if (gSaveContext.nextDayTime != NEXT_TIME_NONE) {
        if (gSaveContext.nextDayTime == NEXT_TIME_DAY) {
            gSaveContext.save.day++;
            gSaveContext.save.eventDayCount++;
            gSaveContext.dogIsLost = true;
            gSaveContext.nextDayTime = NEXT_TIME_DAY_SET;
        } else {
            gSaveContext.nextDayTime = NEXT_TIME_NIGHT_SET;
        }
    }

    Play_InitMotionBlur();

    R_PAUSE_BG_PRERENDER_STATE = PAUSE_BG_PRERENDER_OFF;
    R_PICTO_PHOTO_STATE = PICTO_PHOTO_STATE_OFF;

    PreRender_Init(&this->pauseBgPreRender);
    PreRender_SetValuesSave(&this->pauseBgPreRender, gCfbWidth, gCfbHeight, NULL, NULL, NULL);
    PreRender_SetValues(&this->pauseBgPreRender, gCfbWidth, gCfbHeight, NULL, NULL);

    this->unk_18E64 = gWorkBuffer;
    this->pictoPhotoI8 = gHiBuffer.pictoPhotoI8;
    this->unk_18E68 = gHiBuffer.D_80784600;
    this->unk_18E58 = gHiBuffer.D_80784600;
    this->unk_18E60 = gHiBuffer.D_80784600;
    gTransitionTileState = TRANS_TILE_OFF;
    this->transitionMode = TRANS_MODE_OFF;
    D_801D0D54 = false;

    FrameAdvance_Init(&this->frameAdvCtx);
    Rand_Seed(osGetTime());
    Matrix_Init(&this->state);

    this->state.main = Play_Main;
    this->state.destroy = Play_Destroy;

    this->transitionTrigger = TRANS_TRIGGER_END;
    this->worldCoverAlpha = 0;
    this->bgCoverAlpha = 0;
    this->haltAllActors = false;
    this->soaringCsOrSoTCsPlaying = false;

    if (gSaveContext.gameMode != GAMEMODE_TITLE_SCREEN) {
        if (gSaveContext.nextTransitionType == TRANS_NEXT_TYPE_DEFAULT) {
            this->transitionType =
                (Entrance_GetTransitionFlags(((void)0, gSaveContext.save.entrance) + sceneLayer) >> 7) & 0x7F;
        } else {
            this->transitionType = gSaveContext.nextTransitionType;
            gSaveContext.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
        }
    } else {
        this->transitionType = TRANS_TYPE_FADE_BLACK;
    }

    TransitionFade_Init(&this->unk_18E48);
    TransitionFade_SetType(&this->unk_18E48, 3);
    TransitionFade_SetColor(&this->unk_18E48, RGBA8(160, 160, 160, 255));
    TransitionFade_Start(&this->unk_18E48);
    VisMono_Init(&sPlayVisMono);

    gVisMonoColor.a = 0;
    sPlayVisFbufInstance = &sPlayVisFbuf;
    VisFbuf_Init(sPlayVisFbufInstance);
    sPlayVisFbufInstance->lodProportion = 0.0f;
    sPlayVisFbufInstance->mode = VIS_FBUF_MODE_GENERAL;
    sPlayVisFbufInstance->primColor.r = 0;
    sPlayVisFbufInstance->primColor.g = 0;
    sPlayVisFbufInstance->primColor.b = 0;
    sPlayVisFbufInstance->primColor.a = 0;
    sPlayVisFbufInstance->envColor.r = 0;
    sPlayVisFbufInstance->envColor.g = 0;
    sPlayVisFbufInstance->envColor.b = 0;
    sPlayVisFbufInstance->envColor.a = 0;
    CutsceneFlags_UnsetAll(this);
    THA_GetRemaining(&this->state.tha);
    zAllocSize = THA_GetRemaining(&this->state.tha);
    zAlloc = (uintptr_t)THA_AllocTailAlign16(&this->state.tha, zAllocSize);

    //! @bug: Incorrect ALIGN16s
    ZeldaArena_Init((void*)((zAlloc + 8) & ~0xF), (zAllocSize - ((zAlloc + 8) & ~0xF)) + zAlloc);

    Actor_InitContext(this, &this->actorCtx, this->linkActorEntry);

    // Busyloop until the room loads
    while (!Room_ProcessRoomRequest(this, &this->roomCtx)) {}

    if ((CURRENT_DAY != 0) &&
        ((this->roomCtx.curRoom.type == ROOM_TYPE_DUNGEON) || (this->roomCtx.curRoom.type == ROOM_TYPE_BOSS))) {
        Actor_Spawn(&this->actorCtx, this, ACTOR_EN_TEST4, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0);
    }

    player = GET_PLAYER(this);

    Camera_InitFocalActorSettings(&this->mainCamera, &player->actor);
    gDbgCamEnabled = false;

    if (PLAYER_GET_BG_CAM_INDEX(&player->actor) != 0xFF) {
        Camera_ChangeActorCsCamIndex(&this->mainCamera, PLAYER_GET_BG_CAM_INDEX(&player->actor));
    }

    CutsceneManager_StoreCamera(&this->mainCamera);
    Interface_SetSceneRestrictions(this);
    Environment_PlaySceneSequence(this);
    gSaveContext.seqId = this->sceneSequences.seqId;
    gSaveContext.ambienceId = this->sceneSequences.ambienceId;
    AnimTaskQueue_Update(this, &this->animTaskQueue);
    // BENTODO: crash in Message_FindMessage
    Cutscene_HandleEntranceTriggers(this);
    gSaveContext.respawnFlag = 0;
    sBombersNotebookOpen = false;
    BombersNotebook_Init(&sBombersNotebook);

    sJustClosedBomberNotebook = false;

    GameInteractor_ExecuteOnSceneInit(sceneIdAbsolute, spawnNum);
}

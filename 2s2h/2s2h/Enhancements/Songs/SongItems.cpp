#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "archives/icon_item_static/icon_item_static_yar.h"
#include "interface/parameter_static/parameter_static.h"
#include "overlays/actors/ovl_En_Gs/z_en_gs.h"
extern s16 sEquipState;
extern s16 sEquipAnimTimer;
extern PlayerAnimationHeader* D_8085D17C[];
char* ResourceMgr_LoadTexOrDListByName(const char* filePath);
void Message_ResetOcarinaButtonAlphas(void);
void func_80836A5C(Player* player, PlayState* play);
void Player_Anim_PlayOnceAdjustedReverse(PlayState* play, Player* player, PlayerAnimationHeader* anim);
void Player_Action_63(Player* player, PlayState* play);
}

#define CVAR_NAME "gEnhancements.Songs.SongItems"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static u8 sPendingAutoPlaySong = 0xFF;
static bool sHideOcarinaStaff = false;
static bool sOcarinaErrorPlayed = false;
static u32 sSongIconsRGBA32[13][32 * 32];

static struct {
    s32 btn;
    bool isDpad;
    ItemId item;
    s32 slot;
} sPendingSongSwap = { -1, false, ITEM_NONE, SLOT_NONE };

static bool IsSongItem(ItemId item) {
    return (item >= ITEM_SONG_SONATA && item <= ITEM_SONG_SUN) || item == ITEM_SONG_LULLABY_INTRO;
}

static u8 SongItemToOcarinaId(ItemId item) {
    return item == ITEM_SONG_LULLABY_INTRO ? OCARINA_SONG_GORON_LULLABY_INTRO : (u8)(item - ITEM_SONG_SONATA);
}

static TexturePtr GetSongIconTexture(ItemId item) {
    if (item == ITEM_SONG_LULLABY_INTRO) {
        return (TexturePtr)sSongIconsRGBA32[12];
    }
    return (TexturePtr)sSongIconsRGBA32[item - ITEM_SONG_SONATA];
}

template <typename F> static void ForEachEquipSlot(F&& func) {
    for (int i = EQUIP_SLOT_C_LEFT; i <= EQUIP_SLOT_C_RIGHT; i++) {
        func(i, false);
    }
    if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
        for (int i = EQUIP_SLOT_D_RIGHT; i <= EQUIP_SLOT_D_UP; i++) {
            func(i, true);
        }
    }
}

static ItemId GetButtonItem(s32 btn, bool isDpad) {
    return (ItemId)(isDpad ? DPAD_GET_CUR_FORM_BTN_ITEM(btn) : GET_CUR_FORM_BTN_ITEM(btn));
}

static s32 GetButtonSlot(s32 btn, bool isDpad) {
    return isDpad ? DPAD_GET_CUR_FORM_BTN_SLOT(btn) : GET_CUR_FORM_BTN_SLOT(btn);
}

static void SetIconItemSegment(PlayState* play, s32 btn, bool isDpad, TexturePtr tex) {
    if (isDpad)
        play->interfaceCtx.iconItemSegment[DPAD_BUTTON(btn) + EQUIP_SLOT_MAX] = (char*)tex;
    else
        play->interfaceCtx.iconItemSegment[btn] = (char*)tex;
}

static void SetButtonItem(PlayState* play, s32 btn, bool isDpad, ItemId item, s32 itemSlot) {
    if (isDpad) {
        DPAD_SET_CUR_FORM_BTN_ITEM(btn, item);
        DPAD_SET_CUR_FORM_BTN_SLOT(btn, itemSlot);
        Interface_Dpad_LoadItemIconImpl(play, btn);
    } else {
        SET_CUR_FORM_BTN_ITEM(btn, item);
        SET_CUR_FORM_BTN_SLOT(btn, itemSlot);
        Interface_LoadItemIconImpl(play, btn);
    }
    if (IsSongItem(item)) {
        SetIconItemSegment(play, btn, isDpad, GetSongIconTexture(item));
    }
}

// 4x5 pixel font for song acronym text on icons.
// Each glyph is 4 pixels wide x 5 pixels tall. Each byte = one row, bit 3 = leftmost pixel.
// clang-format off
static const u8 sFont4x5[][5] = {
    { 0b0110, 0b1001, 0b1111, 0b1001, 0b1001 }, // A
    { 0b1110, 0b1001, 0b1110, 0b1001, 0b1110 }, // B
    { 0b1111, 0b1000, 0b1110, 0b1000, 0b1111 }, // E
    { 0b0111, 0b1000, 0b1011, 0b1001, 0b0110 }, // G
    { 0b1001, 0b1001, 0b1111, 0b1001, 0b1001 }, // H
    { 0b1110, 0b0100, 0b0100, 0b0100, 0b1110 }, // I
    { 0b1000, 0b1000, 0b1000, 0b1000, 0b1111 }, // L
    { 0b1001, 0b1111, 0b1111, 0b1001, 0b1001 }, // M
    { 0b1001, 0b1101, 0b1111, 0b1011, 0b1001 }, // N
    { 0b0110, 0b1001, 0b1001, 0b1001, 0b0110 }, // O
    { 0b1110, 0b1001, 0b1110, 0b1000, 0b1000 }, // P
    { 0b1110, 0b1001, 0b1110, 0b1010, 0b1001 }, // R
    { 0b0111, 0b1000, 0b0110, 0b0001, 0b1110 }, // S
    { 0b1110, 0b0100, 0b0100, 0b0100, 0b0100 }, // T
    { 0b1001, 0b1001, 0b1001, 0b1001, 0b0110 }, // U
};
// clang-format on
static const char sFontChars[] = "ABEGHILMNOPRSTU";

static const char* sSongAcronyms[13] = {
    "SOA", "GL", "BN", "EOE", "OTO", "SS", "SOT", "SOH", "ES", "SOAR", "STORM", "SUS", "GLI",
};

static int CharToFontIndex(char c) {
    for (int i = 0; sFontChars[i]; i++) {
        if (sFontChars[i] == c)
            return i;
    }
    return -1;
}

static void RenderTextOnIcon(u32* iconBuf, const char* text) {
    int len = (int)strlen(text);
    int textWidth = len * 5 - 1;
    int startX = (32 - textWidth) / 2;
    int startY = 26;

    // Pass 1: dark outline around each glyph pixel.
    // Writes unconditionally so the outline is visible against both transparent areas and bright
    // note pixels (important for white songs). The foreground pass overwrites glyph positions after.
    for (int ci = 0; ci < len; ci++) {
        int fi = CharToFontIndex(text[ci]);
        if (fi < 0)
            continue;
        for (int gy = 0; gy < 5; gy++) {
            u8 row = sFont4x5[fi][gy];
            for (int gx = 0; gx < 4; gx++) {
                if (!(row & (1 << (3 - gx))))
                    continue;
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (dx == 0 && dy == 0)
                            continue;
                        int px = startX + ci * 5 + gx + dx;
                        int py = startY + gy + dy;
                        if (px >= 0 && px < 32 && py >= 0 && py < 32) {
                            iconBuf[py * 32 + px] = 0xC0000000;
                        }
                    }
                }
            }
        }
    }

    // Pass 2: white foreground pixels
    for (int ci = 0; ci < len; ci++) {
        int fi = CharToFontIndex(text[ci]);
        if (fi < 0)
            continue;
        for (int gy = 0; gy < 5; gy++) {
            u8 row = sFont4x5[fi][gy];
            for (int gx = 0; gx < 4; gx++) {
                if (row & (1 << (3 - gx))) {
                    int px = startX + ci * 5 + gx;
                    int py = startY + gy;
                    if (px >= 0 && px < 32 && py >= 0 && py < 32) {
                        iconBuf[py * 32 + px] = 0xFFFFFFFF;
                    }
                }
            }
        }
    }
}

static void InitSongIcons() {
    static const Color_RGB8 sColors[] = {
        { 150, 255, 100 }, { 255, 80, 40 },   { 100, 150, 255 }, { 255, 160, 0 },   { 255, 100, 255 },
        { 255, 240, 100 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 }, { 255, 255, 255 },
        { 255, 255, 255 }, { 255, 255, 255 }, { 255, 80, 40 },
    };

    memset(sSongIconsRGBA32, 0, sizeof(sSongIconsRGBA32));

    u8* src = (u8*)ResourceMgr_LoadTexOrDListByName(dgItemIconSongNoteTex);
    if (src == nullptr) {
        return;
    }

    for (int s = 0; s < 13; s++) {
        Color_RGB8 c = sColors[s];
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 16; x++) {
                u8 px = src[y * 16 + x];
                u8 intensity = (px >> 4) * 0x11;
                u8 alpha = (px & 0xF) * 0x11;
                sSongIconsRGBA32[s][(7 + y) * 32 + (9 + x)] = (alpha << 24) | ((intensity * c.b / 255) << 16) |
                                                              ((intensity * c.g / 255) << 8) | (intensity * c.r / 255);
            }
        }
        RenderTextOnIcon(sSongIconsRGBA32[s], sSongAcronyms[s]);
    }
}

static void HandleSongEquip(PauseContext* pauseCtx) {
    PlayState* play = gPlayState;
    Input* input = &play->state.input[0];

    if (pauseCtx->state != PAUSE_STATE_MAIN || pauseCtx->pageIndex != PAUSE_QUEST ||
        (pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG && pauseCtx->mainState != PAUSE_MAIN_STATE_IDLE) ||
        pauseCtx->cursorSpecialPos != 0) {
        return;
    }

    s16 cursorPoint = pauseCtx->cursorPoint[PAUSE_QUEST];
    bool isHoveringSong = false;

    bool isDpadEquipsEnabled = CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0);

    if ((cursorPoint >= QUEST_SONG_SONATA && cursorPoint <= QUEST_SONG_SUN) ||
        cursorPoint == QUEST_SONG_LULLABY_INTRO) {
        if (CHECK_QUEST_ITEM(cursorPoint) ||
            (cursorPoint == QUEST_SONG_LULLABY && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO))) {
            isHoveringSong = true;
        }
    }

    auto updateButtonStatus = [&](u8 status) {
        if (gSaveContext.buttonStatus[EQUIP_SLOT_C_LEFT] == status)
            return;
        ForEachEquipSlot([&](s32 i, bool isDpadBtn) {
            if (isDpadBtn)
                gSaveContext.shipSaveContext.dpad.status[i] = status;
            else
                gSaveContext.buttonStatus[i] = status;
        });
        gSaveContext.hudVisibility = HUD_VISIBILITY_IDLE;
        Interface_SetHudVisibility(HUD_VISIBILITY_ALL);
    };

    updateButtonStatus((isHoveringSong && play->msgCtx.msgLength == 0) ? BTN_ENABLED : BTN_DISABLED);

    if (!isHoveringSong) {
        return;
    }

    // Convert the QuestItem cursor index to an ItemId.
    // ITEM_WALLET_GIANT is the base offset such that (ITEM_WALLET_GIANT + QUEST_SONG_*) == ITEM_SONG_*
    // due to the enum layout. Lullaby Intro is non-contiguous and must be handled separately.
    ItemId songItem;
    if (cursorPoint == QUEST_SONG_LULLABY && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) &&
        CHECK_QUEST_ITEM(QUEST_SONG_LULLABY_INTRO)) {
        songItem = ITEM_SONG_LULLABY_INTRO;
    } else {
        songItem = (ItemId)(ITEM_WALLET_GIANT + cursorPoint);
    }
    s32 targetBtn = -1;
    bool isDpad = false;

    static const u16 sEquipButtons[][4] = { { BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT, 0 },
                                            { BTN_DRIGHT, BTN_DLEFT, BTN_DDOWN, BTN_DUP } };

    ForEachEquipSlot([&](s32 i, bool isDpadBtn) {
        if (targetBtn < 0 &&
            CHECK_BTN_ALL(input->press.button, sEquipButtons[isDpadBtn][isDpadBtn ? i : i - EQUIP_SLOT_C_LEFT])) {
            targetBtn = i;
            isDpad = isDpadBtn;
        }
    });

    if (targetBtn < 0) {
        return;
    }

    ItemId targetItem = GetButtonItem(targetBtn, isDpad);
    s32 targetSlot = GetButtonSlot(targetBtn, isDpad);

    // Clear this song from any other button to prevent duplicates (and swap if needed)
    // The actual swap is deferred until the vanilla equip animation finishes (PAUSE_MAIN_STATE_IDLE)
    ForEachEquipSlot([&](s32 i, bool isDpadBtn) {
        if (i == targetBtn && isDpadBtn == isDpad)
            return;
        if (GetButtonItem(i, isDpadBtn) == songItem) {
            sPendingSongSwap = { i, isDpadBtn, targetItem, targetSlot };
            targetItem = ITEM_NONE;
            targetSlot = SLOT_NONE;
        }
    });

    // Set up the animation for KaleidoScope_UpdateItemEquip
    pauseCtx->equipTargetItem = songItem;
    // Use a unique fake slot so UpdateItemEquip doesn't accidentally swap empty buttons
    pauseCtx->equipTargetSlot = 100 + (cursorPoint - QUEST_SONG_SONATA);

    // EQUIP_SLOT_C -> PAUSE_EQUIP_C (-1), EQUIP_SLOT_D -> PAUSE_EQUIP_D (+3)
    pauseCtx->equipTargetCBtn = targetBtn + (isDpad ? 3 : -1);

    pauseCtx->mainState = PAUSE_MAIN_STATE_EQUIP_ITEM;
    pauseCtx->equipAnimX = pauseCtx->questVtx[cursorPoint * 4].v.ob[0] * 10;
    pauseCtx->equipAnimY = pauseCtx->questVtx[cursorPoint * 4].v.ob[1] * 10;
    pauseCtx->equipAnimAlpha = 255;
    sEquipState = 3; // EQUIP_STATE_MOVE_TO_C_BTN
    sEquipAnimTimer = 10;
    pauseCtx->equipAnimScale = 320;
    pauseCtx->equipAnimShrinkRate = 40;

    Audio_PlaySfx(NA_SE_SY_DECIDE);
}

// Mirrors Player_Action_63 OCARINA_MODE_END teardown
static void FinishOcarinaPlay(PlayState* play) {
    Player* player = GET_PLAYER(play);

    sPendingAutoPlaySong = 0xFF;
    sHideOcarinaStaff = false;
    Message_ResetOcarinaButtonAlphas();

    AudioOcarina_SetInstrument(OCARINA_INSTRUMENT_OFF);
    play->interfaceCtx.bButtonInterfaceDoActionActive = false;
    CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA]);
    player->actor.flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;
    if (player->ocarinaInteractionActor != NULL) {
        player->ocarinaInteractionActor->flags &= ~ACTOR_FLAG_OCARINA_INTERACTION;
    }
    player->stateFlags2 &= ~PLAYER_STATE2_USING_OCARINA;
    player->cv.haltActorsDuringCsAction = false;
    player->csAction = PLAYER_CSACTION_NONE;
    Message_CloseTextbox(play);
    play->msgCtx.ocarinaMode = OCARINA_MODE_NONE;

    if (player->actionFunc == Player_Action_63) {
        player->av2.actionVar2 = 1;
        func_80836A5C(player, play);
        Player_Anim_PlayOnceAdjustedReverse(play, player, D_8085D17C[player->transformation]);
    }
}

static bool NeedsOcarinaCleanup(EnGs* enGs, PlayState* play) {
    Player* player = GET_PLAYER(play);

    bool stoneInvolved = (enGs->unk_19A & 0x200) || (player->ocarinaInteractionActor == &enGs->actor);

    if (!stoneInvolved) {
        return false;
    }

    return (player->actor.flags & ACTOR_FLAG_OCARINA_INTERACTION) ||
           (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) || (play->msgCtx.ocarinaMode != OCARINA_MODE_NONE) ||
           (player->actionFunc == Player_Action_63);
}

static void RegisterSongItems() {
    InitSongIcons();

    COND_HOOK(OnKaleidoUpdate, CVAR, [](PauseContext* pauseCtx) {
        HandleSongEquip(pauseCtx);
        if (sPendingSongSwap.btn != -1 && pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) {
            SetButtonItem(gPlayState, sPendingSongSwap.btn, sPendingSongSwap.isDpad, sPendingSongSwap.item,
                          sPendingSongSwap.slot);
            sPendingSongSwap.btn = -1;
        }
    });

    COND_HOOK(BeforeKaleidoDrawPage, CVAR, [](PauseContext* pauseCtx, u16 pauseIndex) {
        if (pauseCtx->state != PAUSE_STATE_MAIN || pauseIndex != PAUSE_QUEST) {
            return;
        }

        PlayState* play = gPlayState;
        bool hasSongEquipped = false;

        ForEachEquipSlot([&](s32 i, bool isDpadBtn) {
            if (IsSongItem(GetButtonItem(i, isDpadBtn))) {
                hasSongEquipped = true;
            }
        });

        if (!hasSongEquipped) {
            return;
        }

        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL42_Opa(play->state.gfxCtx);

        // Use a custom combine mode to blend the dark translucent interior of the texture
        // to a yellowish-brown color matching the Quest Status background (180, 180, 120),
        // while keeping the pure white outline intact.
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

        gDPSetEnvColor(POLY_OPA_DISP++, 60, 70, 40, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        auto drawBorder = [&](ItemId item) {
            if (!IsSongItem(item))
                return;
            s32 questIndex = (item == ITEM_SONG_LULLABY_INTRO) ? QUEST_SONG_LULLABY : (s32)(item - ITEM_WALLET_GIANT);
            s32 vtxIndex = questIndex * 4;

            Vtx* nvtx = (Vtx*)GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Vtx));
            for (int v = 0; v < 4; v++) {
                nvtx[v] = pauseCtx->questVtx[vtxIndex + v];
            }

            // By default, width is 16 and height is 24. Expanding width by 4 on each side natively aligns to 24x24
            // border
            for (int v = 0; v < 4; v++) {
                nvtx[v].v.ob[0] += (v & 1) ? 4 : -4;
                nvtx[v].v.ob[1] += 1;
                nvtx[v].v.tc[0] = (v & 1) ? (32 << 5) : 0;
                nvtx[v].v.tc[1] = (v & 2) ? (32 << 5) : 0;
            }

            gSPVertex(POLY_OPA_DISP++, (uintptr_t)nvtx, 4, 0);
            POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, (TexturePtr)gEquippedItemOutlineTex, 32, 32, 0);
        };

        ForEachEquipSlot([&](s32 i, bool isDpadBtn) { drawBorder(GetButtonItem(i, isDpadBtn)); });

        CLOSE_DISPS(play->state.gfxCtx);
    });

    COND_VB_SHOULD(VB_KALEIDO_SWITCH_PAGE_WITH_DPAD, CVAR, {
        u16 button = va_arg(args, int);
        Input* input = &gPlayState->state.input[0];

        if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0) && CHECK_BTN_ALL(input->cur.button, button)) {
            PauseContext* pauseCtx = &gPlayState->pauseCtx;

            if (pauseCtx->pageIndex == PAUSE_QUEST && pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE_CURSOR_ON_SONG &&
                CHECK_QUEST_ITEM(pauseCtx->cursorPoint[PAUSE_QUEST])) {
                *should = false;
            }
        }
    });

    COND_VB_SHOULD(VB_KALEIDO_DRAW_EQUIP_ANIM_ICON, CVAR, {
        ItemId* equipAnimDrawItem = va_arg(args, ItemId*);
        if (IsSongItem(*equipAnimDrawItem)) {
            TexturePtr tex = GetSongIconTexture(*equipAnimDrawItem);
            OPEN_DISPS(gPlayState->state.gfxCtx);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, gPlayState->pauseCtx.equipAnimAlpha);
            gSPVertex(OVERLAY_DISP++, (uintptr_t)&gPlayState->pauseCtx.cursorVtx[16], 4, 0);
            gDPLoadTextureBlock(OVERLAY_DISP++, tex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
            CLOSE_DISPS(gPlayState->state.gfxCtx);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_DRAW_ITEM_EQUIPPED_OUTLINE, CVAR, {
        ItemId* equipAnimDrawItem = va_arg(args, ItemId*);
        if (IsSongItem(*equipAnimDrawItem)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_INTERFACE_LOAD_ITEM_ICON, CVAR, {
        u8 btn = va_arg(args, int);
        ItemId item = GetButtonItem(btn, false);
        if (IsSongItem(item)) {
            SetIconItemSegment(gPlayState, btn, false, GetSongIconTexture(item));
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_INTERFACE_LOAD_DPAD_ITEM_ICON, CVAR, {
        u8 btn = va_arg(args, int);
        ItemId item = GetButtonItem(btn, true);
        if (IsSongItem(item)) {
            SetIconItemSegment(gPlayState, btn, true, GetSongIconTexture(item));
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        if (IsSongItem(*itemId)) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_GET_ITEM_ON_BUTTON, CVAR, {
        EquipSlot slot = (EquipSlot)va_arg(args, int);
        ItemId* item = va_arg(args, ItemId*);

        if (IsSongItem(*item)) {
            if (INV_CONTENT(ITEM_OCARINA_OF_TIME) != ITEM_OCARINA_OF_TIME ||
                (IS_RANDO && !Rando::Logic::canPlaySong(SongItemToOcarinaId(*item)))) {
                if (!sOcarinaErrorPlayed) {
                    Audio_PlaySfx(NA_SE_SY_OCARINA_ERROR);
                    sOcarinaErrorPlayed = true;
                }
                *item = ITEM_NONE;
                return;
            }
            sPendingAutoPlaySong = SongItemToOcarinaId(*item);
            sHideOcarinaStaff = true;
            *item = ITEM_OCARINA_OF_TIME;
        }
    });

    COND_VB_SHOULD(VB_OVERRIDE_OCARINA_STAFF_STATE, CVAR, {
        if (sPendingAutoPlaySong != 0xFF) {
            OcarinaStaff* staff = va_arg(args, OcarinaStaff*);
            staff->state = sPendingAutoPlaySong;
            sPendingAutoPlaySong = 0xFF;
            Message_ResetOcarinaButtonAlphas();
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_DRAW_OCARINA_STAFF, CVAR, {
        if (sHideOcarinaStaff) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_EN_GS_BEFORE_GOSSIP_GROTTO_SEQUENCE, CVAR, {
        EnGs* enGs = va_arg(args, EnGs*);
        PlayState* play = va_arg(args, PlayState*);

        if (NeedsOcarinaCleanup(enGs, play)) {
            FinishOcarinaPlay(play);
        }
    });

    COND_VB_SHOULD(VB_EN_GS_FINISH_OCARINA_ON_RESET, CVAR, {
        EnGs* enGs = va_arg(args, EnGs*);
        PlayState* play = va_arg(args, PlayState*);

        if (NeedsOcarinaCleanup(enGs, play)) {
            FinishOcarinaPlay(play);
        }
    });

    COND_HOOK(OnSceneInit, CVAR, [](s8 sceneId, s8 spawnNum) {
        sPendingAutoPlaySong = 0xFF;
        sHideOcarinaStaff = false;
        sOcarinaErrorPlayed = false;
        sPendingSongSwap.btn = -1;
    });

    COND_HOOK(OnGameStateUpdate, CVAR, []() {
        if (gPlayState == nullptr) {
            return;
        }

        // Reset error sound flag when no item buttons are held
        Input* input = &gPlayState->state.input[0];
        if (!CHECK_BTN_ANY(input->cur.button,
                           BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT | BTN_DRIGHT | BTN_DLEFT | BTN_DDOWN | BTN_DUP)) {
            sOcarinaErrorPlayed = false;
        }

        // Maintain colored song icons on HUD buttons across scene transitions / form changes
        ForEachEquipSlot([&](s32 i, bool isDpadBtn) {
            ItemId item = GetButtonItem(i, isDpadBtn);
            if (!IsSongItem(item)) {
                return;
            }

            // Auto-upgrade Lullaby Intro to full Lullaby when the player learns it
            if (item == ITEM_SONG_LULLABY_INTRO && CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                item = ITEM_SONG_LULLABY;
                SetButtonItem(gPlayState, i, isDpadBtn, item, GetButtonSlot(i, isDpadBtn));
            }

            SetIconItemSegment(gPlayState, i, isDpadBtn, GetSongIconTexture(item));
        });

        // Hide ocarina staff during auto-play
        if (!sHideOcarinaStaff || sPendingAutoPlaySong != 0xFF) {
            return;
        }

        u8 mode = gPlayState->msgCtx.msgMode;
        if (mode < MSGMODE_OCARINA_STARTING || mode > MSGMODE_40) {
            sHideOcarinaStaff = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSongItems, { CVAR_NAME });

#include "kaleido.h"

#include "soh/frame_interpolation.h"
#include "soh/ShipUtils.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include <textures/message_static/message_static.h>
#include <textures/parameter_static/parameter_static.h>
extern PlayState* gPlayState;
}
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh_assets.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "consolevariablebridge.h"
#include "soh/Enhancements/cosmetics/cosmeticsTypes.h"
#include "soh/Enhancements/randomizer/dungeon.h"

#include <sstream>

extern "C" {
void KaleidoScope_MoveCursorToSpecialPos(PlayState* play, u16 specialPos);
}

namespace Rando {

void KaleidoEntryIcon::LoadIconTex(std::vector<Gfx>* mEntryDl) {
    if (mIconFormat == G_IM_FMT_IA) {
        if (mIconSize == G_IM_SIZ_8b) {
            Gfx iconTexture[] = { gsDPLoadTextureBlock(
                mIconResourceName, G_IM_FMT_IA, G_IM_SIZ_8b, mIconWidth, mIconHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
            mEntryDl->insert(mEntryDl->end(), std::begin(iconTexture), std::end(iconTexture));
        }
    } else if (mIconFormat == G_IM_FMT_RGBA) {
        if (mIconSize == G_IM_SIZ_32b) {
            Gfx iconTexture[] = { gsDPLoadTextureBlock(
                mIconResourceName, G_IM_FMT_RGBA, G_IM_SIZ_32b, mIconWidth, mIconHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
            mEntryDl->insert(mEntryDl->end(), std::begin(iconTexture), std::end(iconTexture));
        }
    }
}

KaleidoEntry::KaleidoEntry(int16_t x, int16_t y, std::string text) : mX(x), mY(y), mText(std::move(text)) {
    mHeight = 0;
    mWidth = 0;
    vtx = nullptr;
}

void KaleidoEntry::SetYOffset(int yOffset) {
    mY = yOffset;
}

void KaleidoEntryIcon::Draw(PlayState* play, std::vector<Gfx>* mEntryDl) {
    if (vtx == nullptr) {
        return;
    }
    size_t numChar = mText.length();
    if (numChar == 0) {
        return;
    }

    Color_RGBA8 textColor = { 255, 255, 255, 255 };
    if (mAchieved) {
        textColor = { 0x98, 0xFF, 0x44, 255 };
    }

    Matrix_Translate(mX, mY, 0.0f, MTXMODE_APPLY);

    mEntryDl->push_back(gsSPMatrix(Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                                   G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

    // icon
    if (mGrayscale) {
        mEntryDl->push_back(gsDPSetGrayscaleColor(109, 109, 109, 255));
        mEntryDl->push_back(gsSPGrayscale(true));
    }
    mEntryDl->push_back(gsDPSetPrimColor(0, 0, mIconColor.r, mIconColor.g, mIconColor.b, mIconColor.a));
    mEntryDl->push_back(gsSPVertex(vtx, 4, 0));
    LoadIconTex(mEntryDl);
    mEntryDl->push_back(gsSP1Quadrangle(0, 2, 3, 1, 0));
    mEntryDl->push_back(gsSPGrayscale(false));

    // text
    mEntryDl->push_back(gsDPSetPrimColor(0, 0, textColor.r, textColor.g, textColor.b, textColor.a));
    for (size_t i = 0, vtxGroup = 0; i < numChar; i++) {
        // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
        // handle loading groups of 16 chars at a time until there are no more left to load.
        // By this point 4 vertices have already been loaded for the preceding icon.
        if (i % 16 == 0) {
            size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
            mEntryDl->push_back(gsSPVertex(&vtx[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
            vtxGroup++;
        }

        auto texture = reinterpret_cast<uintptr_t>(Ship_GetCharFontTexture(mText[i]));
        auto vertexStart = static_cast<int16_t>(4 * (i % 16));

        Gfx charTexture[] = { gsDPLoadTextureBlock_4b(texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0,
                                                      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                                                      G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
        mEntryDl->insert(mEntryDl->end(), std::begin(charTexture), std::end(charTexture));
        mEntryDl->push_back(gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
    }
    mEntryDl->push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
}

Kaleido::Kaleido() {
    const auto ctx = Rando::Context::GetInstance();
    int yOffset = 2;
    mEntries.push_back(std::make_shared<KaleidoEntryIconFlag>(
        gRupeeCounterIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, Color_RGBA8{ 0xC8, 0xFF, 0x64, 255 },
        FlagType::FLAG_RANDOMIZER_INF, static_cast<int>(RAND_INF_GREG_FOUND), 0, yOffset, "Greg"));
    yOffset += 18;
    if (ctx->GetOption(RSK_TRIFORCE_HUNT)) {
        mEntries.push_back(std::make_shared<KaleidoEntryIconCountRequired>(
            gTriforcePieceTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255, 255, 255, 255 }, 0, yOffset,
            reinterpret_cast<int*>(&gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected),
            ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).Get() + 1,
            ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_TOTAL).Get() + 1));
        yOffset += 18;
    }
    if (ctx->GetOption(RSK_SHUFFLE_OCARINA_BUTTONS)) {
        mEntries.push_back(std::make_shared<KaleidoEntryOcarinaButtons>(0, yOffset));
        yOffset += 18;
    }
    if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).IsNot(RO_BOSS_SOULS_OFF)) {
        static const char* bossSoulNames[] = {
            "Gohma's Soul",    "King Dodongo's Soul", "Barinade's Soul",    "Phantom Ganon's Soul",
            "Volvagia's Soul", "Morpha's Soul",       "Bongo Bongo's Soul", "Twinrova's Soul",
        };
        for (int i = RAND_INF_GOHMA_SOUL; i < RAND_INF_GANON_SOUL; i++) {
            mEntries.push_back(std::make_shared<KaleidoEntryIconFlag>(
                gBossSoulTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255, 255, 255, 255 },
                FlagType::FLAG_RANDOMIZER_INF, i, 0, yOffset, bossSoulNames[i - RAND_INF_GOHMA_SOUL]));
            yOffset += 18;
        }
    }
    if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON)) {
        mEntries.push_back(std::make_shared<KaleidoEntryIconFlag>(
            gBossSoulTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255, 255, 255, 255 },
            FlagType::FLAG_RANDOMIZER_INF, RAND_INF_GANON_SOUL, 0, yOffset, "Ganon's Soul"));
        yOffset += 18;
    }
    if (ctx->GetOption(RSK_SHUFFLE_SILVER_RUPEES).Get() > RO_DUNGEON_ITEM_LOC_VANILLA) {
        for (int i = RG_SILVER_RUPEE_FIRST; i <= RG_SILVER_RUPEE_LAST; i++) {
            uint8_t dungeonId = ctx->GetSilverRupeeCounter(static_cast<RandomizerGet>(i)).DungeonID();
            RandomizerCheckQuest dungeonQuest = ctx->GetDungeon(dungeonId)->IsMQ() ? RCQUEST_MQ : RCQUEST_VANILLA;
            RandomizerCheckQuest rupeeQuest = ctx->GetSilverRupeeCounter(static_cast<RandomizerGet>(i)).Quest();
            if (dungeonQuest == rupeeQuest) {
                mEntries.push_back(
                    std::make_shared<KaleidoEntrySilverRupeeCounter>(static_cast<RandomizerGet>(i), 0, yOffset));
                yOffset += 18;
            }
        }
    }
}

extern "C" {
void FrameInterpolation_RecordCloseChild(void);
void FrameInterpolation_RecordOpenChild(const void* a, int b);
}

void Kaleido::Draw(PlayState* play) {
    if (play == nullptr || mEntries.empty()) {
        return;
    }
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    mEntryDl.clear();
    OPEN_DISPS(play->state.gfxCtx);
    mEntryDl.push_back(gsDPPipeSync());
    Gfx_SetupDL_42Opa(play->state.gfxCtx);
    mEntryDl.push_back(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));

    // Move the matrix origin to the top-left corner of the kaleido page
    Matrix_Translate(-108.f, 58.f, 0.0f, MTXMODE_APPLY);
    // Invert the matrix to render vertices with positive going down
    Matrix_Scale(1.0f, -1.0f, 1.0f, MTXMODE_APPLY);
    // The scrolling logic is in here because the built in kaleido input throttling happens
    // in its Draw functions, which get called after their update functions. I hate it but fixing
    // it would be a much larger Kaleido change.
    bool shouldScroll = false;
    bool dpad = CVarGetInteger(CVAR_SETTING("DPadOnPause"), 0);
    if (((pauseCtx->unk_1E4 == 0) || (pauseCtx->unk_1E4 == 5) || (pauseCtx->unk_1E4 == 8)) &&
        (pauseCtx->pageIndex == PAUSE_QUEST)) {
        if (!((pauseCtx->state != 6) || ((pauseCtx->stickRelX == 0) && (pauseCtx->stickRelY == 0)))) {
            if (pauseCtx->cursorSpecialPos == 0) {
                if ((pauseCtx->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                    if (mTopIndex > 0) {
                        mTopIndex--;
                        shouldScroll = true;
                    }
                } else if ((pauseCtx->stickRelY < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DDOWN))) {
                    if (mTopIndex + mNumVisible < mEntries.size()) {
                        mTopIndex++;
                        shouldScroll = true;
                    }
                }
                if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    pauseCtx->unk_1E4 = 0;
                } else if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    pauseCtx->unk_1E4 = 0;
                }
            } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                if ((pauseCtx->stickRelX > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DRIGHT))) {
                    pauseCtx->cursorSpecialPos = 0;
                    Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_RIGHT) {
                if ((pauseCtx->stickRelX < -30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DLEFT))) {
                    pauseCtx->cursorSpecialPos = 0;
                    Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                }
            }
        } else if (pauseCtx->cursorSpecialPos != 0 && pauseCtx->state == 7) {
            pauseCtx->cursorSpecialPos = 0;
        }
    }
    int yOffset = 2;
    for (int i = mTopIndex; i < (mTopIndex + mNumVisible) && i < mEntries.size(); i++) {
        auto& entry = mEntries[i];
        if (shouldScroll) {
            entry->SetYOffset(yOffset);
            yOffset += 18;
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
        Matrix_Push();
        entry->Draw(play, &mEntryDl);
        Matrix_Pop();
    }

    mEntryDl.push_back(gsSPEndDisplayList());
    gSPDisplayList(POLY_OPA_DISP++, mEntryDl.data());
    CLOSE_DISPS(play->state.gfxCtx);
}

void Kaleido::Update(PlayState* play) {
    for (int i = mTopIndex; i < (mTopIndex + mNumVisible) && i < mEntries.size(); i++) {
        const auto& entry = mEntries[i];
        entry->Update(play);
    }
}

extern "C" void RandoKaleido_DrawMiscCollectibles(PlayState* play) {
    OTRGlobals::Instance->gRandoContext->GetKaleido()->Draw(play);
}

extern "C" void RandoKaleido_UpdateMiscCollectibles(int16_t inDungeonScene) {
    PauseContext* pauseCtx = &gPlayState->pauseCtx;
    if (pauseCtx->randoQuestMode && pauseCtx->pageIndex == PAUSE_QUEST) {
        OTRGlobals::Instance->gRandoContext->GetKaleido()->Update(gPlayState);
    }
}

KaleidoEntryIconFlag::KaleidoEntryIconFlag(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth,
                                           int iconHeight, Color_RGBA8 iconColor, FlagType flagType, int flag,
                                           int16_t x, int16_t y, std::string name)
    : mFlagType(flagType), mFlag(flag), KaleidoEntryIcon(iconResourceName, iconFormat, iconSize, iconWidth, iconHeight,
                                                         iconColor, x, y, std::move(name)) {
    BuildVertices();
}

void KaleidoEntryIconFlag::Update(PlayState* play) {
    mAchieved = GameInteractor::RawAction::CheckFlag(mFlagType, static_cast<int16_t>(mFlag));
    mGrayscale = !mAchieved;
}

KaleidoEntryIconCountRequired::KaleidoEntryIconCountRequired(const char* iconResourceName, int iconFormat, int iconSize,
                                                             int iconWidth, int iconHeight, Color_RGBA8 iconColor,
                                                             int16_t x, int16_t y, int* watch, int required, int total)
    : mWatch(watch), mRequired(required), mTotal(total),
      KaleidoEntryIcon(iconResourceName, iconFormat, iconSize, iconWidth, iconHeight, iconColor, x, y) {
    if (mWatch != nullptr) {
        mCount = *mWatch;
    }
    mAchieved = mCount >= mRequired;
    mGrayscale = mCount == 0;
    BuildText();
    BuildVertices();
}

void KaleidoEntryIconCountRequired::BuildText() {
    std::ostringstream totals;
    if (mCount < mRequired) {
        totals << mCount;
    }
    if (mRequired != 0 && mCount < mRequired) {
        totals << '/' << mRequired;
    }
    if (mTotal >= mRequired && mCount >= mRequired) {
        totals << '/' << mTotal;
    }
    mText = totals.str();
}

KaleidoEntrySilverRupeeCounter::KaleidoEntrySilverRupeeCounter(RandomizerGet rgid, int16_t x, int16_t y)
    : mRgid(rgid), KaleidoEntryIconCountRequired(gRupeeCounterIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16,
                                                 Color_RGBA8{ 255, 255, 255, 255 }, x, y) {
    mCount = OTRGlobals::Instance->gRandoContext->GetSilverRupeeCounter(mRgid).GetCollected();
    mRequired = OTRGlobals::Instance->gRandoContext->GetSilverRupeeCounter(mRgid).GetTotal();
    mAchieved = mCount >= mRequired;
    mGrayscale = mCount == 0;
    BuildText();
    BuildVertices();
}

void KaleidoEntrySilverRupeeCounter::BuildText() {
    KaleidoEntryIconCountRequired::BuildText();
    CustomMessage name = CustomMessage(Rando::StaticData::RetrieveItem(mRgid).GetName());
    // Abbreviate Dungeon Names
    name.Replace("Spirit Temple", "SpT");
    name.Replace("Shadow Temple", "ShT");
    name.Replace("Bottom of the Well", "BotW");
    name.Replace("Gerudo Training Grounds", "GTG");
    name.Replace("Dodongo's Cavern", "DC");
    name.Replace("Ice Cavern", "IC");
    name.Replace("Ganon's Castle", "GC");
    // Remove MQ to make spoilers less prevalent
    name.Replace("MQ", "");
    // Remove "Silver Rupee" from the name
    name.Replace("Silver Rupee", "");
    if (!mText.empty()) {
        mText += " ";
    }
    mText += name.GetForCurrentLanguage();
}

void KaleidoEntrySilverRupeeCounter::Update(PlayState* play) {
    int newCount = OTRGlobals::Instance->gRandoContext->GetSilverRupeeCounter(mRgid).GetCollected();
    if (mCount != newCount) {
        mCount = newCount;
        BuildText();
        RebuildVertices();
        mAchieved = mCount >= mRequired;
        mGrayscale = mCount == 0;
    }
}

void KaleidoEntryIcon::BuildVertices() {
    int offsetY = 0;
    int offsetX = 0;
    // 4 vertices per character, plus one for the preceding icon.
    Vtx* vertices = (Vtx*)calloc(sizeof(Vtx[4]), mText.length() + 1);
    // Vertex for the preceding icon.
    Ship_CreateQuadVertexGroup(vertices, offsetX, offsetY, mIconWidth, mIconHeight, 0);
    offsetX += 18;
    for (size_t i = 0; i < mText.length(); i++) {
        int charWidth = static_cast<int>(Ship_GetCharFontWidth(mText[i]));
        if ((offsetX + charWidth) > 220) {
            offsetX -= Ship_GetCharFontWidth(mText[i - 1]);
            offsetX -= Ship_GetCharFontWidth(mText[i - 2]);
            mText = mText.substr(0, i - 2) + "...";
            int periodWidth = Ship_GetCharFontWidth('.');
            Ship_CreateQuadVertexGroup(&(vertices)[(i - 1) * 4], offsetX, offsetY, periodWidth, 16, 0);
            offsetX += periodWidth;
            Ship_CreateQuadVertexGroup(&(vertices)[(i)*4], offsetX, offsetY, periodWidth, 16, 0);
            offsetX += periodWidth;
            Ship_CreateQuadVertexGroup(&(vertices)[(i + 1) * 4], offsetX, offsetY, periodWidth, 16, 0);
            offsetX += periodWidth;
            break;
        }
        Ship_CreateQuadVertexGroup(&(vertices)[(i + 1) * 4], offsetX, offsetY, charWidth, 16, 0);
        offsetX += charWidth;
    }
    offsetY += FONT_CHAR_TEX_HEIGHT;
    mWidth = static_cast<int16_t>(offsetX);
    mHeight = static_cast<int16_t>(offsetY);

    vertices[1].v.ob[0] = 16;
    vertices[2].v.ob[1] = 16;
    vertices[3].v.ob[0] = 16;
    vertices[3].v.ob[1] = 16;
    vtx = vertices;
}

KaleidoEntryIcon::KaleidoEntryIcon(const char* iconResourceName, int iconFormat, int iconSize, int iconWidth,
                                   int iconHeight, Color_RGBA8 iconColor, int16_t x, int16_t y, std::string text)
    : mIconResourceName(iconResourceName), mIconFormat(iconFormat), mIconSize(iconSize), mIconWidth(iconWidth),
      mIconHeight(iconHeight), mIconColor(iconColor), KaleidoEntry(x, y, std::move(text)) {
}

void KaleidoEntryIcon::RebuildVertices() {
    free(vtx);
    vtx = nullptr;
    BuildVertices();
}

void KaleidoEntryIconCountRequired::Update(PlayState* play) {
    if (mCount != *mWatch) {
        mCount = *mWatch;
        BuildText();
        RebuildVertices();
        mAchieved = mCount >= mRequired;
        mGrayscale = mCount == 0;
    }
}

KaleidoEntryOcarinaButtons::KaleidoEntryOcarinaButtons(int16_t x, int16_t y)
    : KaleidoEntryIcon(gItemIconOcarinaOfTimeTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32,
                       Color_RGBA8{ 255, 255, 255, 255 }, x, y, "\x9F\xA5\xA6\xA7\xA8") {
    CalculateColors();
    BuildVertices();
}

void KaleidoEntryOcarinaButtons::CalculateColors() {
    Color_RGB8 aButtonColor = { 80, 150, 255 };
    if (CVarGetInteger(CVAR_COSMETIC("HUD.AButton.Changed"), 0)) {
        aButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.AButton.Value"), aButtonColor);
    } else if (CVarGetInteger(CVAR_COSMETIC("DefaultColorScheme"), COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
        aButtonColor = { 80, 255, 150 };
    }
    mButtonColors[0] = { aButtonColor.r, aButtonColor.g, aButtonColor.b, 255 };
    Color_RGB8 cButtonsColor = { 255, 255, 50 };
    Color_RGB8 cUpButtonColor = cButtonsColor;
    Color_RGB8 cDownButtonColor = cButtonsColor;
    Color_RGB8 cLeftButtonColor = cButtonsColor;
    Color_RGB8 cRightButtonColor = cButtonsColor;
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CButtons.Changed"), 0)) {
        cUpButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), cButtonsColor);
        cDownButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), cButtonsColor);
        cLeftButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), cButtonsColor);
        cRightButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CButtons.Value"), cButtonsColor);
    }
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CUpButton.Changed"), 0)) {
        cUpButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CUpButton.Value"), cUpButtonColor);
    }
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CDownButton.Changed"), 0)) {
        cDownButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CDownButton.Value"), cDownButtonColor);
    }
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CLeftButton.Changed"), 0)) {
        cLeftButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CLeftButton.Value"), cLeftButtonColor);
    }
    if (CVarGetInteger(CVAR_COSMETIC("HUD.CRightButton.Changed"), 0)) {
        cRightButtonColor = CVarGetColor24(CVAR_COSMETIC("HUD.CRightButton.Value"), cRightButtonColor);
    }
    mButtonColors[1] = { cUpButtonColor.r, cUpButtonColor.g, cUpButtonColor.b, 255 };
    mButtonColors[2] = { cDownButtonColor.r, cDownButtonColor.g, cDownButtonColor.b, 255 };
    mButtonColors[3] = { cLeftButtonColor.r, cLeftButtonColor.g, cLeftButtonColor.b, 255 };
    mButtonColors[4] = { cRightButtonColor.r, cRightButtonColor.g, cRightButtonColor.b, 255 };
}

void KaleidoEntryOcarinaButtons::Update(PlayState* play) {
    mButtonCollected[0] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_A) > 0;
    mButtonCollected[1] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_UP) > 0;
    mButtonCollected[2] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_DOWN) > 0;
    mButtonCollected[3] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_LEFT) > 0;
    mButtonCollected[4] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_RIGHT) > 0;
    CalculateColors();
    mAchieved = false;
    mGrayscale = true;
    for (int i = 0; i < mButtonCollected.size(); i++) {
        if (!mButtonCollected[i]) {
            mButtonColors[i] = Color_RGBA8{ 109, 109, 109, 255 };
        } else {
            mAchieved = true;
            mGrayscale = false;
        }
    }
}

void KaleidoEntryOcarinaButtons::Draw(PlayState* play, std::vector<Gfx>* mEntryDl) {
    if (vtx == nullptr) {
        return;
    }
    size_t numChar = mText.length();
    if (numChar == 0) {
        return;
    }

    Matrix_Translate(mX, mY, 0.0f, MTXMODE_APPLY);
    //        Matrix_Scale(0.75f, 0.75f, 0.75f, MTXMODE_APPLY);

    mEntryDl->push_back(gsSPMatrix(Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                                   G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

    // icon
    if (mGrayscale) {
        mEntryDl->push_back(gsDPSetGrayscaleColor(109, 109, 109, 255));
        mEntryDl->push_back(gsSPGrayscale(true));
    }
    mEntryDl->push_back(gsDPSetPrimColor(0, 0, mIconColor.r, mIconColor.g, mIconColor.b, mIconColor.a));
    mEntryDl->push_back(gsSPVertex(vtx, 4, 0));
    LoadIconTex(mEntryDl);
    mEntryDl->push_back(gsSP1Quadrangle(0, 2, 3, 1, 0));
    mEntryDl->push_back(gsSPGrayscale(false));

    // text
    for (size_t i = 0, vtxGroup = 0; i < numChar; i++) {
        mEntryDl->push_back(
            gsDPSetPrimColor(0, 0, mButtonColors[i].r, mButtonColors[i].g, mButtonColors[i].b, mButtonColors[i].a));

        // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
        // handle loading groups of 16 chars at a time until there are no more left to load.
        // By this point 4 vertices have already been loaded for the preceding icon.
        if (i % 16 == 0) {
            size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
            mEntryDl->push_back(gsSPVertex(&vtx[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
            vtxGroup++;
        }

        auto texture = reinterpret_cast<uintptr_t>(Ship_GetCharFontTexture(mText[i]));
        auto vertexStart = static_cast<int16_t>(4 * (i % 16));

        Gfx charTexture[] = { gsDPLoadTextureBlock_4b(texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0,
                                                      G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                                                      G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
        mEntryDl->insert(mEntryDl->end(), std::begin(charTexture), std::end(charTexture));
        mEntryDl->push_back(gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
    }
    mEntryDl->push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
}
} // namespace Rando

void RandoKaleido_RegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnKaleidoscopeUpdate>(
        RandoKaleido_UpdateMiscCollectibles);
}

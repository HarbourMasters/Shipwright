#include "kaleido.h"

#include "soh/frame_interpolation.h"

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

#include <sstream>

namespace Rando {

    void KaleidoEntryIcon::LoadIconTex(std::vector<Gfx>* mEntryDl) {
        if (mIconFormat == G_IM_FMT_IA) {
            if (mIconSize == G_IM_SIZ_8b) {
                Gfx iconTexture[] = { gsDPLoadTextureBlock(mIconResourceName, G_IM_FMT_IA, G_IM_SIZ_8b, mIconWidth, mIconHeight, 0,
                                              G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                              G_TX_NOLOD, G_TX_NOLOD) };
                mEntryDl->insert(mEntryDl->end(), std::begin(iconTexture), std::end(iconTexture));
            }
        } else if (mIconFormat == G_IM_FMT_RGBA) {
            if (mIconSize == G_IM_SIZ_32b) {
                Gfx iconTexture[] = { gsDPLoadTextureBlock(mIconResourceName, G_IM_FMT_RGBA, G_IM_SIZ_32b, mIconWidth, mIconHeight, 0,
                                                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                                           G_TX_NOLOD, G_TX_NOLOD) };
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
//        Matrix_Scale(0.75f, 0.75f, 0.75f, MTXMODE_APPLY);

        mEntryDl->push_back(gsSPMatrix(Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

        // icon
        if (!mAchieved) {
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
            uint16_t texIndex = mText[i] - 32;

            // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
            // handle loading groups of 16 chars at a time until there are no more left to load.
            // By this point 4 vertices have already been loaded for the preceding icon.
            if (i % 16 == 0) {
                size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
                mEntryDl->push_back(gsSPVertex(&vtx[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
                vtxGroup++;
            }

            if (texIndex != 0) {
                auto texture = reinterpret_cast<uintptr_t>(Font_FetchCharTexture(texIndex));
                auto vertexStart = static_cast<int16_t>(4 * (i % 16));

                Gfx charTexture[] = {gsDPLoadTextureBlock_4b(texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH,
                                                             FONT_CHAR_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                                             G_TX_NOMIRROR | G_TX_CLAMP,
                                                             G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD)};
                mEntryDl->insert(mEntryDl->end(), std::begin(charTexture), std::end(charTexture));
                mEntryDl->push_back(
                        gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
            }
        }
        mEntryDl->push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
    }

    Kaleido::Kaleido() {
        const auto ctx = Rando::Context::GetInstance();
        int yOffset = 2;
        mEntries.push_back(std::make_shared<KaleidoEntryIconFlag>(gRupeeCounterIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16,
                              Color_RGBA8{ 0xC8, 0xFF, 0x64, 255 }, FlagType::FLAG_RANDOMIZER_INF,
                              static_cast<int>(RAND_INF_GREG_FOUND), 0, yOffset, "Greg"));
        yOffset += 18;
        if (ctx->GetOption(RSK_TRIFORCE_HUNT)) {
            mEntries.push_back(
                    std::make_shared<KaleidoEntryIconCountRequired>(
                            gTriforcePieceTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255,255,255,255 }, 0,
                            yOffset, reinterpret_cast<int*>(&gSaveContext.triforcePiecesCollected),
                            ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).GetSelectedOptionIndex() + 1,
                            ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_TOTAL).GetSelectedOptionIndex() + 1));
            yOffset += 18;
        }
        if (ctx->GetOption(RSK_SHUFFLE_OCARINA_BUTTONS)) {
            mEntries.push_back(std::make_shared<KaleidoEntryOcarinaButtons>(0, yOffset));
            yOffset += 18;
        }
        if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).IsNot(RO_BOSS_SOULS_OFF)) {
            static const char* bossSoulNames[] = {
                    "Ghoma's Soul",
                    "King Dodongo's Soul",
                    "Barinade's Soul",
                    "Phantom Ganon's Soul",
                    "Volvagia's Soul",
                    "Morpha's Soul",
                    "Bongo Bongo's Soul",
                    "Twinrova's Soul",
            };
            for (int i = RAND_INF_GOHMA_SOUL; i < RAND_INF_GANON_SOUL; i++) {
                mEntries.push_back(
                    std::make_shared<KaleidoEntryIconFlag>(
                        gBossSoulTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255, 255, 255, 255 },
                        FlagType::FLAG_RANDOMIZER_INF, i, 0, yOffset, bossSoulNames[i - RAND_INF_GOHMA_SOUL]
                    )
                );
                yOffset += 18;
            }
        }
        if (ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON)) {
            mEntries.push_back(
                    std::make_shared<KaleidoEntryIconFlag>(
                            gBossSoulTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, Color_RGBA8{ 255, 255, 255, 255 },
                            FlagType::FLAG_RANDOMIZER_INF, RAND_INF_GANON_SOUL, 0, yOffset, "Ganon's Soul"
                    )
            );
            yOffset += 18;
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
        if ((pauseCtx->stickRelY > 30) || CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
            if (mTopIndex > 0) {
                mTopIndex--;
                shouldScroll = true;
            }
        } else if ((pauseCtx->stickRelY < -30) || CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
            if (mTopIndex + mNumVisible < mEntries.size()) {
                mTopIndex++;
                shouldScroll = true;
            }
        }
        int yOffset = 2;
        for (int i = mTopIndex; i < (mTopIndex + mNumVisible) && i < mEntries.size(); i++) {
            auto& entry = mEntries[i];
            if (shouldScroll) {
                entry->SetYOffset(yOffset);
                yOffset += 18;
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            Matrix_Push();
            entry->Draw(play, &mEntryDl);
            Matrix_Pop();
        }

        mEntryDl.push_back(gsSPEndDisplayList());
        gSPDisplayList(POLY_KAL_DISP++, mEntryDl.data());
        CLOSE_DISPS(play->state.gfxCtx);
    }

    void Kaleido::Update(PlayState *play) {
        for(int i = mTopIndex; i < (mTopIndex + mNumVisible) && i < mEntries.size(); i++) {
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

    KaleidoEntryIconFlag::KaleidoEntryIconFlag(const char *iconResourceName, int iconFormat, int iconSize, int iconWidth,
                                               int iconHeight, Color_RGBA8 iconColor, FlagType flagType, int flag,
                                               int16_t x, int16_t y, std::string name) :
        mFlagType(flagType), mFlag(flag),
        KaleidoEntryIcon(iconResourceName, iconFormat, iconSize, iconWidth, iconHeight, iconColor, x, y, std::move(name)) {
        BuildVertices();
    }

    void KaleidoEntryIconFlag::Update(PlayState* play) {
        mAchieved = GameInteractor::RawAction::CheckFlag(mFlagType, static_cast<int16_t>(mFlag));
    }

    KaleidoEntryIconCountRequired::KaleidoEntryIconCountRequired(const char *iconResourceName, int iconFormat,
        int iconSize, int iconWidth, int iconHeight, Color_RGBA8 iconColor, int16_t x, int16_t y, int* watch,
        int required, int total) : mWatch(watch), mRequired(required), mTotal(total),
        KaleidoEntryIcon(iconResourceName, iconFormat, iconSize, iconWidth, iconHeight, iconColor, x, y) {
        mCount = *mWatch;
        BuildText();
        BuildVertices();
    }

    void KaleidoEntryIconCountRequired::BuildText() {
        std::ostringstream totals;
        totals << mCount;
        if (mRequired != 0 && mCount < mRequired) {
            totals << '/' << mRequired;
        }
        if (mTotal >= mRequired && mCount >= mRequired) {
            totals << '/' << mTotal;
        }
        mText = totals.str();
    }

    void KaleidoEntryIcon::BuildVertices() {
        int offsetY = 0;
        int offsetX = 0;
        // 4 vertices per character, plus one for the preceding icon.
        Vtx* vertices = (Vtx*)calloc(sizeof(Vtx[4]), mText.length() + 1);
        // Vertex for the preceding icon.
        Interface_CreateQuadVertexGroup(vertices, offsetX, offsetY, mIconWidth, mIconHeight, 0);
        offsetX += 18;
        for (size_t i = 0; i < mText.length(); i++) {
            auto charIndex = static_cast<uint16_t>(mText[i] - 32);
            int charWidth = 0;
            if (charIndex >= 0) {
                charWidth = static_cast<int>(Message_GetCharacterWidth(charIndex) * (100.0f / R_TEXT_CHAR_SCALE));
            }
            Interface_CreateQuadVertexGroup(&(vertices)[(i + 1) * 4], offsetX, offsetY, charWidth, 16, 0);
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

    KaleidoEntryIcon::KaleidoEntryIcon(const char *iconResourceName, int iconFormat, int iconSize, int iconWidth,
                                       int iconHeight, Color_RGBA8 iconColor, int16_t x, int16_t y, std::string text)
                                       : mIconResourceName(iconResourceName), mIconFormat(iconFormat), mIconSize(iconSize),
                                       mIconWidth(iconWidth), mIconHeight(iconHeight), mIconColor(iconColor),
                                       KaleidoEntry(x, y, std::move(text)) {}

    void KaleidoEntryIcon::RebuildVertices() {
        free(vtx);
        vtx = nullptr;
        BuildVertices();
    }

    void KaleidoEntryIconCountRequired::Update(PlayState *play) {
        if (mCount != *mWatch) {
            mCount = *mWatch;
            BuildText();
            RebuildVertices();
            mAchieved = mCount >= mRequired;
        }

    }

    KaleidoEntryOcarinaButtons::KaleidoEntryOcarinaButtons(int16_t x, int16_t y) :
            KaleidoEntryIcon(gItemIconOcarinaOfTimeTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32,
                             Color_RGBA8{ 255, 255, 255, 255 }, x, y, "\x9F\xA5\xA6\xA7\xA8") {
        CalculateColors();
        BuildVertices();
    }

    void KaleidoEntryOcarinaButtons::CalculateColors() {
        // The commented out code in this function is for handling custom cosmetics for the
        // A and C buttons. For contrast/accessibility reasons, I've chosen not to respect the
        // custom cosmetics for now. I've left the code commented out in case that decision gets
        // reversed later.
        Color_RGB8 aButtonColor = { 80, 150, 255 };
//        if (CVarGetInteger("gCosmetics.Hud_AButton.Changed", 0)) {
//            aButtonColor = CVarGetColor24("gCosmetics.Hud_AButton.Value", aButtonColor);
        /*} else*/ if (CVarGetInteger("gCosmetics.DefaultColorScheme", COLORSCHEME_N64) == COLORSCHEME_GAMECUBE) {
            aButtonColor = { 80, 255, 150};
        }
        mButtonColors[0] = { aButtonColor.r, aButtonColor.g, aButtonColor.b, 255 };
        Color_RGB8 cButtonsColor = { 255, 255, 50 };
        Color_RGB8 cUpButtonColor = cButtonsColor;
        Color_RGB8 cDownButtonColor = cButtonsColor;
        Color_RGB8 cLeftButtonColor = cButtonsColor;
        Color_RGB8 cRightButtonColor = cButtonsColor;
//        if (CVarGetInteger("gCosmetics.Hud_CButtons.Changed", 0)) {
//            cUpButtonColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
//            cDownButtonColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
//            cLeftButtonColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
//            cRightButtonColor = CVarGetColor24("gCosmetics.Hud_CButtons.Value", cButtonsColor);
//        }
//        if (CVarGetInteger("gCosmetics.Hud_CUpButton.Changed", 0)) {
//            cUpButtonColor = CVarGetColor24("gCosmetics.Hud_CUpButton.Value", cUpButtonColor);
//        }
//        if (CVarGetInteger("gCosmetics.Hud_CDownButton.Changed", 0)) {
//            cDownButtonColor = CVarGetColor24("gCosmetics.Hud_CDownButton.Value", cDownButtonColor);
//        }
//        if (CVarGetInteger("gCosmetics.Hud_CLeftButton.Changed", 0)) {
//            cLeftButtonColor = CVarGetColor24("gCosmetics.Hud_CLeftButton.Value", cLeftButtonColor);
//        }
//        if (CVarGetInteger("gCosmetics.Hud_CRightButton.Changed", 0)) {
//            cRightButtonColor = CVarGetColor24("gCosmetics.Hud_CRightButton.Value", cRightButtonColor);
//        }
        mButtonColors[1] = { cUpButtonColor.r, cUpButtonColor.g, cUpButtonColor.b, 255 };
        mButtonColors[2] = { cDownButtonColor.r, cDownButtonColor.g, cDownButtonColor.b, 255 };
        mButtonColors[3] = { cLeftButtonColor.r, cLeftButtonColor.g, cLeftButtonColor.b, 255 };
        mButtonColors[4] = { cRightButtonColor.r, cRightButtonColor.g, cRightButtonColor.b, 255 };
    }

    void KaleidoEntryOcarinaButtons::Update(PlayState *play) {
        mButtonCollected[0] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_A) > 0;
        mButtonCollected[1] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_UP) > 0;
        mButtonCollected[2] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_DOWN) > 0;
        mButtonCollected[3] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_LEFT) > 0;
        mButtonCollected[4] = GameInteractor::RawAction::CheckFlag(FLAG_RANDOMIZER_INF, RAND_INF_HAS_OCARINA_C_RIGHT) > 0;
        CalculateColors();
        mAchieved = false;
        for (int i = 0; i < mButtonCollected.size(); i++) {
            if (!mButtonCollected[i]) {
                mButtonColors[i] = Color_RGBA8{ 109, 109, 109, 255 };
            } else {
                mAchieved = true;
            }
        }
    }

    void KaleidoEntryOcarinaButtons::Draw(PlayState *play, std::vector<Gfx>* mEntryDl) {
        if (vtx == nullptr) {
            return;
        }
        size_t numChar = mText.length();
        if (numChar == 0) {
            return;
        }

        Matrix_Translate(mX, mY, 0.0f, MTXMODE_APPLY);
//        Matrix_Scale(0.75f, 0.75f, 0.75f, MTXMODE_APPLY);

        mEntryDl->push_back(gsSPMatrix(Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

        // icon
        if (!mAchieved) {
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
            mEntryDl->push_back(gsDPSetPrimColor(0, 0, mButtonColors[i].r, mButtonColors[i].g, mButtonColors[i].b, mButtonColors[i].a));
            uint16_t texIndex = mText[i] - 32;

            // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
            // handle loading groups of 16 chars at a time until there are no more left to load.
            // By this point 4 vertices have already been loaded for the preceding icon.
            if (i % 16 == 0) {
                size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
                mEntryDl->push_back(gsSPVertex(&vtx[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
                vtxGroup++;
            }

            if (texIndex != 0) {
                auto texture = reinterpret_cast<uintptr_t>(Font_FetchCharTexture(texIndex));
                auto vertexStart = static_cast<int16_t>(4 * (i % 16));

                Gfx charTexture[] = {gsDPLoadTextureBlock_4b(texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH,
                                                             FONT_CHAR_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                                                             G_TX_NOMIRROR | G_TX_CLAMP,
                                                             G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD)};
                mEntryDl->insert(mEntryDl->end(), std::begin(charTexture), std::end(charTexture));
                mEntryDl->push_back(
                        gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
            }
        }
        mEntryDl->push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
    }
} // Rando

void RandoKaleido_RegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnKaleidoscopeUpdate>(RandoKaleido_UpdateMiscCollectibles);
}
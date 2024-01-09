#include "kaleido.h"

#include "soh/frame_interpolation.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include <textures/message_static/message_static.h>
#include <textures/parameter_static/parameter_static.h>
}
#include "soh/OTRGlobals.h"

namespace Rando {
    KaleidoEntry::KaleidoEntry(const char* iconResourceName, std::string name, std::string value, int16_t x, int16_t y)
        : mIconResourceName(iconResourceName), mName(std::move(name)), mValue(std::move(value)), mX(x), mY(y) {
        int16_t offsetX = 0;
        std::string text = mName + mValue;
        // 4 vertices per character, plus one for the preceding icon.
        Vtx* vertices = (Vtx*)calloc(sizeof(Vtx[4]), text.length() + 1);
        // Vertex for the preceding icon.
        Interface_CreateQuadVertexGroup(vertices, offsetX, 0, 16, 16, 0);
        offsetX += 16;
        for (size_t i = 0; i < text.length(); i++) {
            int16_t charIndex = text[i] - 32;
            int16_t charWidth = 0;
            if (charIndex >= 0) {
                charWidth = (int16_t)(Message_GetCharacterWidth(charIndex) * (100.0f / R_TEXT_CHAR_SCALE));
            }
            Interface_CreateQuadVertexGroup(&(vertices)[(i + 1) * 4], offsetX, 0, charWidth, 16, 0);
            offsetX += charWidth;
        }

        vtx = vertices;
//        mtx = new Mtx();
    }

    void KaleidoEntry::Draw(PlayState* play, std::vector<Gfx>* mEntryDl) {
        if (vtx == nullptr/* || mtx == nullptr*/) {
            return;
        }
        float scale = 75.0f / 100.0f;
        std::string text = mName + mValue;
        size_t numChar = text.length();
        if (numChar == 0) {
            return;
        }

        Color_RGBA8 textboxColor = {0, 0, 0, 255};
        Color_RGBA8 textColor = {255, 255, 255, 255};

        Matrix_Translate(mX, mY, 0.0f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
//        Matrix_RotateZ(-(play->pauseCtx.unk_1FC) / 100.0f, MTXMODE_APPLY);
        Matrix_RotateY(-1.57f, MTXMODE_APPLY);
        Matrix_RotateX(-3.14f, MTXMODE_APPLY);
//        Matrix_ToMtx(mtx, (char*)__FILE__, __LINE__);

        mEntryDl->push_back(gsSPMatrix(Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__), G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

        // icon
        mEntryDl->push_back(gsDPSetPrimColor(0, 0, 0xC8, 0xFF, 0x64, 255));
        mEntryDl->push_back(gsSPVertex(vtx, 4, 0));
        Gfx iconTexture[] = { gsDPLoadTextureBlock(gRupeeCounterIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                              G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                              G_TX_NOLOD, G_TX_NOLOD) };
        mEntryDl->insert(mEntryDl->end(), std::begin(iconTexture), std::end(iconTexture));
        mEntryDl->push_back(gsSP1Quadrangle(0, 2, 3, 1, 0));

        // text
        mEntryDl->push_back(gsDPSetPrimColor(0, 0, textColor.r, textColor.g, textColor.b, textColor.a));
        for (size_t i = 0, vtxGroup = 0; i < numChar; i++) {
            uint16_t texIndex = text[i] - 32;

            // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
            // handle loading groups of 16 chars at a time until there are no more left to load.
            // By this point 4 vertices have already been loaded for the preceding icon.
            if (i % 16 == 0) {
                size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
                mEntryDl->push_back(gsSPVertex(&vtx[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
                vtxGroup++;
            }

            if (texIndex != 0) {
                uintptr_t texture = reinterpret_cast<uintptr_t>(Font_FetchCharTexture(texIndex));
                int16_t vertexStart = 4 * (i % 16);

                Gfx charTexture[] = { gsDPLoadTextureBlock_4b(texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH,
                                      FONT_CHAR_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                                      G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
                mEntryDl->insert(mEntryDl->end(), std::begin(charTexture), std::end(charTexture));
                mEntryDl->push_back(gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
            }
        }
        mEntryDl->push_back(gsSPPopMatrix(G_MTX_MODELVIEW));
    }

    Kaleido::Kaleido() {
        mEntries.emplace_back(gRupeeCounterIconTex, "Greg Found: ", "No", 0, 0);
    }

    extern "C" {
    void FrameInterpolation_RecordCloseChild(void);
    void FrameInterpolation_RecordOpenChild(const void* a, int b);
    }

    void Kaleido::Draw(PlayState* play) {
        if (play == nullptr || mEntries.empty()) {
            return;
        }
        mEntryDl.clear();
        OPEN_DISPS(play->state.gfxCtx);
        mEntryDl.push_back(gsDPPipeSync());
        Gfx_SetupDL_42Opa(play->state.gfxCtx);
        mEntryDl.push_back(gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM));
        for (auto& entry : mEntries) {
            entry.Draw(play, &mEntryDl);
        }

        mEntryDl.push_back(gsSPEndDisplayList());
        gSPDisplayList(POLY_KAL_DISP++, mEntryDl.data());
        CLOSE_DISPS(play->state.gfxCtx);
    }

    extern "C" void RandoKaleido_DrawMiscCollectibles(PlayState* play) {
        OTRGlobals::Instance->gRandoContext->GetKaleido()->Draw(play);
    }
} // Rando
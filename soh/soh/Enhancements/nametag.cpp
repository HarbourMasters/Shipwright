#include "nametag.h"
#include <libultraship/bridge.h>
#include <vector>
#include <algorithm>
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/custom-message/CustomMessageInterfaceAddon.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "textures/message_static/message_static.h"
extern PlayState* gPlayState;
}

typedef struct {
    Actor* actor;
    std::string text; // Original text
    std::string processedText; // Text filtered for supported font textures
    const char* tag; // Tag identifier
    Color_RGBA8 textColor; // Text color override. Global color is used if alpha is 0
    int16_t height; // Textbox height
    int16_t width; // Textbox width
    int16_t yOffset; // Addition Y offset
    Mtx* mtx; // Allocated Mtx for rendering
    Vtx* vtx; // Allocated Vtx for rendering
} NameTag;

static std::vector<NameTag> nameTags;
static std::vector<Gfx> nameTagDl;

void FreeNameTag(NameTag* nameTag) {
    if (nameTag->vtx != nullptr) {
        free(nameTag->vtx);
        nameTag->vtx = nullptr;
    }
    if (nameTag->mtx != nullptr) {
        free(nameTag->mtx);
        nameTag->mtx = nullptr;
    }
}

void DrawNameTag(PlayState* play, const NameTag* nameTag) {
    if (nameTag->actor == nullptr || nameTag->actor->draw == nullptr || !nameTag->actor->isDrawn ||
        nameTag->vtx == nullptr || nameTag->mtx == nullptr) {
        return;
    }

    // Name tag is too far away to meaningfully read, don't bother rendering it
    if (nameTag->actor->xyzDistToPlayerSq > 200000.0f) {
        return;
    }

    // Fade out name tags that are far away
    float alpha = 1.0f;
    if (nameTag->actor->xyzDistToPlayerSq > 160000.0f) {
        alpha = (200000.0f - nameTag->actor->xyzDistToPlayerSq) / 40000.0f;
    }

    float scale = 75.0f / 150.f;

    size_t numChar = nameTag->processedText.length();
    // No text to render
    if (numChar == 0) {
        return;
    }

    Color_RGBA8 textboxColor = { 0, 0, 0, 80};
    Color_RGBA8 textColor = { 255, 255, 255, 255 };

    if (CVarGetInteger("gCosmetics.Hud_NameTagActorBackground.Changed", 0)) {
        textboxColor = CVarGetColor("gCosmetics.Hud_NameTagActorBackground.Value", textboxColor);
    }
    if (CVarGetInteger("gCosmetics.Hud_NameTagActorText.Changed", 0)) {
        textColor = CVarGetColor("gCosmetics.Hud_NameTagActorText.Value", textColor);
    }

    FrameInterpolation_RecordOpenChild(nameTag->actor, 10);

    // Prefer the highest between world position and focus position if targetable
    float posY = nameTag->actor->world.pos.y;
    if (nameTag->actor->flags & ACTOR_FLAG_TARGETABLE) {
        posY = std::max(posY, nameTag->actor->focus.pos.y);
    }

    posY += nameTag->yOffset + 16;

    // Set position, billboard effect, scale (with mirror mode), then center nametag
    Matrix_Translate(nameTag->actor->world.pos.x, posY, nameTag->actor->world.pos.z, MTXMODE_NEW);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Scale(scale * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1), -scale, 1.0f, MTXMODE_APPLY);
    Matrix_Translate(-(float)nameTag->width / 2, -nameTag->height, 0, MTXMODE_APPLY);
    Matrix_ToMtx(nameTag->mtx, (char*)__FILE__, __LINE__);

    nameTagDl.push_back(gsSPMatrix(nameTag->mtx, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW));

    // textbox
    nameTagDl.push_back(gsSPVertex(nameTag->vtx, 4, 0));
    nameTagDl.push_back(gsDPSetPrimColor(0, 0, textboxColor.r, textboxColor.g, textboxColor.b, textboxColor.a * alpha));

    // Multi-instruction macro, need to insert all to the dl buffer
    Gfx textboxTexture[] = { gsDPLoadTextureBlock_4b(gDefaultMessageBackgroundTex, G_IM_FMT_I, 128, 64, 0, G_TX_MIRROR,
                                                     G_TX_NOMIRROR, 7, 0, G_TX_NOLOD, G_TX_NOLOD) };
    nameTagDl.insert(nameTagDl.end(), std::begin(textboxTexture), std::end(textboxTexture));

    nameTagDl.push_back(gsSP1Quadrangle(0, 2, 3, 1, 0));

    // text
    if (nameTag->textColor.a == 0) {
        nameTagDl.push_back(gsDPSetPrimColor(0, 0, textColor.r, textColor.g, textColor.b, textColor.a * alpha));
    } else {
        nameTagDl.push_back(gsDPSetPrimColor(0, 0, nameTag->textColor.r, nameTag->textColor.g, nameTag->textColor.b,
                                             nameTag->textColor.a * alpha));
    }

    for (size_t i = 0, vtxGroup = 0; i < numChar; i++) {
        uint16_t texIndex = nameTag->processedText[i] - 32;

        // A maximum of 64 Vtx can be loaded at once by gSPVertex, or basically 16 characters
        // handle loading groups of 16 chars at a time until there are no more left to load
        if (i % 16 == 0) {
            size_t numVtxToLoad = std::min<size_t>(numChar - i, 16) * 4;
            nameTagDl.push_back(gsSPVertex(&(nameTag->vtx)[4 + (vtxGroup * 16 * 4)], numVtxToLoad, 0));
            vtxGroup++;
        }

        if (texIndex != 0 && nameTag->processedText[i] != '\n') {
            uintptr_t texture = (uintptr_t)Font_FetchCharTexture(texIndex);
            int16_t vertexStart = 4 * (i % 16);

            // Multi-instruction macro, need to insert all to the dl buffer
            Gfx charTexture[] = { gsDPLoadTextureBlock_4b(
                texture, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD) };
            nameTagDl.insert(nameTagDl.end(), std::begin(charTexture), std::end(charTexture));

            nameTagDl.push_back(gsSP1Quadrangle(vertexStart, vertexStart + 2, vertexStart + 3, vertexStart + 1, 0));
        }
    }

    nameTagDl.push_back(gsSPPopMatrix(G_MTX_MODELVIEW));

    FrameInterpolation_RecordCloseChild();
}

// Draw all the name tags by leveraging a system heap buffer for majority of the graphics commands
void DrawNameTags() {
    if (gPlayState == nullptr || nameTags.size() == 0) {
        return;
    }

    nameTagDl.clear();

    OPEN_DISPS(gPlayState->state.gfxCtx);

    // Setup before rendering name tags
    Gfx_SetupDL_38Xlu(gPlayState->state.gfxCtx);
    nameTagDl.push_back(gsDPSetAlphaDither(G_AD_DISABLE));
    nameTagDl.push_back(gsSPClearGeometryMode(G_SHADE));

    nameTagDl.push_back(
        gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0));

    // Add all the name tags
    for (const auto& nameTag : nameTags) {
        DrawNameTag(gPlayState, &nameTag);
    }

    // End the display list buffer
    nameTagDl.push_back(gsSPEndDisplayList());

    gSPDisplayList(POLY_XLU_DISP++, nameTagDl.data());

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void UpdateNameTags() {
    // Leveraging ZBuffer above allows the name tags to be obscured by OPA surfaces based on depth.
    // However, XLU surfaces do not calculate depth with regards to other XLU surfaces.
    // With multiple name tags, a tag can only obscure other tags based on draw order.
    // Here we sort the tags so that actors further away from the camera are ordered first.
    std::sort(nameTags.begin(), nameTags.end(), [](NameTag a, NameTag b) {
        if (a.actor == nullptr || b.actor == nullptr) {
            return true;
        }

        f32 aDistToCamera = Actor_WorldDistXZToPoint(a.actor, &gPlayState->mainCamera.eye);
        f32 bDistToCamera = Actor_WorldDistXZToPoint(b.actor, &gPlayState->mainCamera.eye);

        return aDistToCamera > bDistToCamera;
    });
}

extern "C" void NameTag_RegisterForActorWithOptions(Actor* actor, const char* text, NameTagOptions options) {
    std::string processedText = std::string(Interface_ReplaceSpecialCharacters((char*)text));

    // Strip out unsupported characters
    processedText.erase(std::remove_if(processedText.begin(), processedText.end(), [](const char& c) {
        // 172 is max supported texture for the in-game font system,
        // and filter anything less than a space but not the newline or nul characters
        return c > 172 || (c < ' ' && c != '\n' && c != '\0');
    }), processedText.end());

    int16_t numChar = processedText.length();
    int16_t numLines = 1;
    int16_t offsetX = 0;
    int16_t maxOffsetX = 0;

    // 4 vertex per character plus one extra group of 4 for the textbox
    Vtx* vertices = (Vtx*)calloc(sizeof(Vtx[4]), numChar + 1);

    // Set all the char vtx first to get the total size for the textbox
    for (size_t i = 0; i < numChar; i++) {
        if (processedText[i] == '\n') {
            offsetX = 0;
            numLines++;
        }

        int16_t charIndex = processedText[i] - 32;
        int16_t charWidth = 0;
        // Don't add width for newline chars
        if (charIndex >= 0) {
            charWidth = (int16_t)(Message_GetCharacterWidth(charIndex) * (100.0f / R_TEXT_CHAR_SCALE));
        }

        Interface_CreateQuadVertexGroup(&(vertices)[(i + 1) * 4], offsetX, (numLines - 1) * 16, charWidth, 16, 0);
        offsetX += charWidth;

        if (offsetX > maxOffsetX) {
            maxOffsetX = offsetX;
        }
    }

    // Vtx for textbox, add +/- 4 in all directions
    int16_t height = (numLines * 16) + 8;
    int16_t width = maxOffsetX + 8;
    Interface_CreateQuadVertexGroup(vertices, -4, -4, width, height, 0);

    // Update the texture coordinates to consume the full textbox texture size (including mirror region)
    vertices[1].v.tc[0] = 256 << 5;
    vertices[2].v.tc[1] = 64 << 5;
    vertices[3].v.tc[0] = 256 << 5;
    vertices[3].v.tc[1] = 64 << 5;

    NameTag nameTag;
    nameTag.actor = actor;
    nameTag.text = std::string(text);
    nameTag.processedText = processedText;
    nameTag.tag = options.tag;
    nameTag.textColor = options.textColor;
    nameTag.height = height;
    nameTag.width = width;
    nameTag.yOffset = options.yOffset;
    nameTag.mtx = new Mtx();
    nameTag.vtx = vertices;

    nameTags.push_back(nameTag);
}

extern "C" void NameTag_RegisterForActor(Actor* actor, const char* text) {
    NameTag_RegisterForActorWithOptions(actor, text, {});
}

extern "C" void NameTag_RemoveAllForActor(Actor* actor) {
    for (auto it = nameTags.begin(); it != nameTags.end();) {
        if (it->actor == actor) {
            FreeNameTag(&(*it));
            it = nameTags.erase(it);
        } else {
            it++;
        }
    }
}

extern "C" void NameTag_RemoveAllByTag(const char* tag) {
    for (auto it = nameTags.begin(); it != nameTags.end();) {
        if (it->tag != nullptr && strcmp(it->tag, tag) == 0) {
            FreeNameTag(&(*it));
            it = nameTags.erase(it);
        } else {
            it++;
        }
    }
}

void RemoveAllNameTags() {
    for (auto& nameTag : nameTags) {
        FreeNameTag(&nameTag);
    }

    nameTags.clear();
}

static bool sRegisteredHooks = false;

void NameTag_RegisterHooks() {
    if (sRegisteredHooks) {
        return;
    }

    sRegisteredHooks = true;

    // Reorder tags every frame to mimic depth rendering
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() { UpdateNameTags(); });

    // Render name tags at the end of player draw to avoid overflowing the display buffers
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDrawEnd>([]() { DrawNameTags(); });

    // Remove all name tags on play state destroy as all actors are removed anyways
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>([]() { RemoveAllNameTags(); });
}

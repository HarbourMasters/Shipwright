#include <libultraship/bridge/resourcebridge.h>
#include <fast/resource/type/Texture.h>
#include <spdlog/spdlog.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#include <cstring>
#include <map>
#include <string>
#include <unordered_set>

extern "C" {
#include "z64.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
extern PlayState* gPlayState;
}

/*
A pause page is normally drawn as 15 separate sections.
This allows a mod to instead supply the whole page as one texture
*/

static constexpr int PAGE_WIDTH = 240;
static constexpr int PAGE_HEIGHT = 160;
static constexpr int SECTION_WIDTH = 80;
static constexpr int SECTION_HEIGHT = 32;
static constexpr int SECTION_ROWS = 5;
static constexpr int SECTION_VTX_COUNT = 15 * 4;
// Half a texel to offset the 0.5 texel offset of the game
static constexpr s16 HALF_TEXEL = 16;
// Index of section "10" (column 1, row 0) in a page's texture array.
// Use section "10" because section "00" is shared between the save and game-over pages.
static constexpr int PAGE_NAME_SECTION = 5;

static const char* sLanguageSuffixes[] = { "ENG", "GER", "FRA", "JPN" };

static std::map<std::pair<void**, u8>, const char*> sCombinedPages;

static std::string GetPageName(void** textures) {
    // gPause<PageName><Column><Row><optional<Language>>Tex
    std::string name = static_cast<const char*>(textures[PAGE_NAME_SECTION]);
    name = name.substr(name.rfind('/') + 1);
    if (name.ends_with("Tex")) {
        name.resize(name.size() - strlen("Tex"));
    }
    for (const char* language : sLanguageSuffixes) {
        if (name.ends_with(language)) {
            name.resize(name.size() - strlen(language));
        }
    }
    if (name.ends_with("10")) {
        name.resize(name.size() - strlen("10"));
    }
    return name;
}

// Native IA8 must be unscaled
static bool IsPageTexture(const std::shared_ptr<Fast::Texture>& texture, const char* path) {
    bool validTexture;
    if (texture->Flags & TEX_FLAG_LOAD_AS_RAW) {
        validTexture = texture->Width * 4 == PAGE_WIDTH * texture->HByteScale &&
                       texture->Height == PAGE_HEIGHT * texture->VPixelScale;
    } else {
        validTexture = texture->Type == Fast::TextureType::GrayscaleAlpha8bpp && texture->Width == PAGE_WIDTH &&
                       texture->Height == PAGE_HEIGHT && texture->HByteScale == 1.0f && texture->VPixelScale == 1.0f;
    }
    if (!validTexture) {
        SPDLOG_WARN("{} is not a 240x160 IA8 texture or a raw RGBA replacement of one; drawing the page's sections",
                    path + strlen("__OTR__"));
    }
    return validTexture;
}

static const char* FindCombinedPage(void** textures) {
    auto key = std::make_pair(textures, gSaveContext.language);
    auto cached = sCombinedPages.find(key);
    if (cached != sCombinedPages.end()) {
        return cached->second;
    }

    std::string page = GetPageName(textures);
    std::string language = sLanguageSuffixes[gSaveContext.language];
    // Display lists keep the path's pointer.
    static std::unordered_set<std::string> sPaths;
    const char* found = nullptr;
    for (const std::string& name : { page + language + "Tex", page + "Tex" }) {
        const char* path = sPaths.insert("__OTR__textures/icon_item_static/" + name).first->c_str();
        auto texture = std::dynamic_pointer_cast<Fast::Texture>(ResourceLoad(path));
        if (texture != nullptr) {
            found = IsPageTexture(texture, path) ? path : nullptr;
            break;
        }
    }
    return sCombinedPages[key] = found;
}

static void LoadPageTexture(Gfx** gfx, const char* path) {
    gDPSetTextureImage((*gfx)++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, path);
    gDPSetTile((*gfx)++, G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
               G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD);
    gDPLoadSync((*gfx)++);
    gDPLoadBlockWide(*gfx, G_TX_LOADTILE, 0, 0,
                     ((PAGE_WIDTH * PAGE_HEIGHT + G_IM_SIZ_8b_INCR) >> G_IM_SIZ_8b_SHIFT) - 1,
                     CALC_DXT(PAGE_WIDTH, G_IM_SIZ_8b_BYTES));
    *gfx += 2;
    gDPPipeSync((*gfx)++);
    gDPSetTile((*gfx)++, G_IM_FMT_IA, G_IM_SIZ_8b, ((PAGE_WIDTH * G_IM_SIZ_8b_LINE_BYTES) + 7) >> 3, 0, G_TX_RENDERTILE,
               0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK,
               G_TX_NOLOD);
    gDPSetTileSize((*gfx)++, G_TX_RENDERTILE, 0, 0, (PAGE_WIDTH - 1) << G_TEXTURE_IMAGE_FRAC,
                   (PAGE_HEIGHT - 1) << G_TEXTURE_IMAGE_FRAC);
}

static Vtx* CreatePageVertices(Vtx* vertices) {
    Vtx* page = (Vtx*)Graph_Alloc(gPlayState->state.gfxCtx, SECTION_VTX_COUNT * sizeof(Vtx));
    memcpy(page, vertices, SECTION_VTX_COUNT * sizeof(Vtx));
    for (int quad = 0; quad < SECTION_VTX_COUNT / 4; quad++) {
        s16 left = (quad / SECTION_ROWS) * SECTION_WIDTH * 32 - HALF_TEXEL;
        s16 top = (quad % SECTION_ROWS) * SECTION_HEIGHT * 32 - HALF_TEXEL;
        Vtx* vtx = &page[quad * 4];
        vtx[0].v.tc[0] = vtx[2].v.tc[0] = left;
        vtx[1].v.tc[0] = vtx[3].v.tc[0] = left + SECTION_WIDTH * 32;
        vtx[0].v.tc[1] = vtx[1].v.tc[1] = top;
        vtx[2].v.tc[1] = vtx[3].v.tc[1] = top + SECTION_HEIGHT * 32;
    }
    return page;
}

static void DrawSectionQuads(Gfx** gfx, Vtx* vertices) {
    for (int first = 0; first < SECTION_VTX_COUNT; first += 32) {
        int count = MIN(32, SECTION_VTX_COUNT - first);
        gSPVertex((*gfx)++, (uintptr_t)(vertices + first), count, 0);
        for (int j = 0; j < count; j += 4) {
            gSP1Quadrangle((*gfx)++, j, j + 2, j + 3, j + 1, 0);
        }
    }
}

static void RegisterCombinedPausePages() {
    COND_VB_SHOULD(VB_DRAW_PAUSE_PAGE_SECTIONS, true, {
        Gfx** gfx = va_arg(args, Gfx**);
        Vtx* vertices = va_arg(args, Vtx*);
        void** textures = va_arg(args, void**);

        const char* path = FindCombinedPage(textures);
        if (path == nullptr) {
            return;
        }

        *should = false;
        gDPPipeSync((*gfx)++);
        LoadPageTexture(gfx, path);
        DrawSectionQuads(gfx, CreatePageVertices(vertices));
    });

    COND_HOOK(OnAssetAltChange, true, []() { sCombinedPages.clear(); });
}

static RegisterShipInitFunc initFunc(RegisterCombinedPausePages);

#include <libultraship/bridge/resourcebridge.h>
#include <fast/resource/ResourceType.h>
#include <fast/resource/type/Texture.h>
#include <spdlog/spdlog.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"

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

// A pause page is 15 sections, each clamped at its own edge, so filtering leaves a seam between them. A mod can
// instead supply the whole page as one texture: textures/icon_item_static/gPause<Page><LANG>Tex, or gPause<Page>Tex
// for every language. It is 240x160 IA8, or a raw RGBA replacement scaled relative to that.

static constexpr int PAGE_WIDTH = 240;
static constexpr int PAGE_HEIGHT = 160;
static constexpr int SECTION_WIDTH = 80;
static constexpr int SECTION_HEIGHT = 32;
static constexpr int SECTION_ROWS = 5;
static constexpr int SECTION_VTX_COUNT = 15 * 4;
// Half a texel in s10.5: cancels the half texel the bilinear filter adds, so the page lines up with the image.
static constexpr s16 HALF_TEXEL = 16;

static const char* sLanguageSuffixes[] = { "ENG", "GER", "FRA", "JPN" };

// The combined texture found for each section array and language, or nullptr for none.
static std::map<std::pair<void**, u8>, const char*> sCombinedPages;

// The page's name, from its column 1, row 0 section: the one section no two pages share
// (gPauseGameOver10Tex is drawn with the save prompt's other sections).
static std::string GetPageName(void** textures) {
    std::string name = static_cast<const char*>(textures[SECTION_ROWS]);
    name = name.substr(name.rfind('/') + 1);
    for (const char* suffix : { "Tex", "ENG", "GER", "FRA", "JPN", "10" }) {
        if (name.ends_with(suffix)) {
            name.resize(name.size() - strlen(suffix));
        }
    }
    return name;
}

// Display lists keep a texture path's pointer, and the interpreter memoizes resolved paths by pointer.
static const char* GetStableTexturePath(const std::string& path) {
    static std::unordered_set<std::string> sPaths;
    return sPaths.insert("__OTR__textures/icon_item_static/" + path).first->c_str();
}

static bool HasFile(const char* path) {
    return ResourceMgr_FileExists(path) || (ResourceMgr_IsAltAssetsEnabled() && ResourceMgr_FileAltExists(path));
}

// Native IA8 must be unscaled because its decoder uses the render tile's 240-byte stride.
// Raw RGBA replacements use scale metadata relative to 240x160 IA8 (one byte per texel).
static bool IsPageTexture(const char* path) {
    auto resource = ResourceLoad(path);
    if (resource == nullptr || resource->GetInitData()->Type != static_cast<uint32_t>(Fast::ResourceType::Texture)) {
        return false;
    }

    auto texture = std::static_pointer_cast<Fast::Texture>(resource);
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
    std::string language = sLanguageSuffixes[gSaveContext.language % ARRAY_COUNT(sLanguageSuffixes)];
    const char* found = nullptr;
    for (const std::string& name : { page + language + "Tex", page + "Tex" }) {
        const char* path = GetStableTexturePath(name);
        if (HasFile(path)) {
            found = IsPageTexture(path) ? path : nullptr;
            break;
        }
    }
    return sCombinedPages[key] = found;
}

// gDPLoadTextureBlock with the wide load block: 240x160 texels overflow LoadBlock's count.
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

// A copy of the section quads, each pointed at its part of the page. The save prompt's vertices can be drawn twice in
// a frame with different textures, so the originals stay untouched.
static Vtx* CreatePageVertices(Vtx* vertices) {
    Vtx* page = (Vtx*)Graph_Alloc(gPlayState->state.gfxCtx, SECTION_VTX_COUNT * sizeof(Vtx));
    memcpy(page, vertices, SECTION_VTX_COUNT * sizeof(Vtx));
    for (int quad = 0; quad < SECTION_VTX_COUNT / 4; quad++) {
        // func_80823A0C lays the sections out column by column: top left, top right, bottom left, bottom right
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

// In the batches KaleidoScope_DrawPageSections uses: the vertex buffer holds 32.
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

#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <png.h>

using json = nlohmann::json;

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Items.ColorPictograph"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static s16 fileNumber = 1;
static u16 pictoPhotoRGBABuffer[PICTO_PHOTO_SIZE];

void SavePictoPng() {
    const int width = PICTO_PHOTO_WIDTH;
    const int height = PICTO_PHOTO_HEIGHT;

    std::string path =
        Ship::Context::GetPathRelativeToAppDirectory("saves/picto" + std::to_string(fileNumber) + ".png");

    FILE* fp = fopen(path.c_str(), "wb");
    if (!fp)
        throw std::runtime_error("Failed to open PNG file for write");

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
        throw std::runtime_error("png_create_write_struct failed");

    png_infop info = png_create_info_struct(png);
    if (!info)
        throw std::runtime_error("png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png)))
        throw std::runtime_error("libpng write error");

    png_init_io(png, fp);

    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png, info);

    // RGBA8 buffer
    std::vector<uint8_t> buffer(width * height * 4);

    for (int i = 0; i < width * height; ++i) {
        uint16_t px = pictoPhotoRGBABuffer[i];
        px = (px >> 8) | (px << 8);
        buffer[i * 4 + 0] = ((px >> 11) & 0x1F) << 3; // R
        buffer[i * 4 + 1] = ((px >> 6) & 0x1F) << 3;  // G
        buffer[i * 4 + 2] = ((px >> 1) & 0x1F) << 3;  // B
        buffer[i * 4 + 3] = (px & 1) ? 255 : 0;       // A
    }

    std::vector<png_bytep> rows(height);
    for (int y = 0; y < height; ++y)
        rows[y] = &buffer[y * width * 4];

    png_write_image(png, rows.data());
    png_write_end(png, nullptr);

    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

void LoadPictoPNG() {
    std::string path =
        Ship::Context::GetPathRelativeToAppDirectory("saves/picto" + std::to_string(fileNumber) + ".png");

    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        std::memset(pictoPhotoRGBABuffer, 0, sizeof(pictoPhotoRGBABuffer));
        return;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (setjmp(png_jmpbuf(png)))
        throw std::runtime_error("libpng read error");

    png_init_io(png, fp);
    png_read_info(png, info);

    png_uint_32 width, height;
    int depth, color;
    png_get_IHDR(png, info, &width, &height, &depth, &color, nullptr, nullptr, nullptr);

    if (width != PICTO_PHOTO_WIDTH || height != PICTO_PHOTO_HEIGHT)
        throw std::runtime_error("PNG dimensions mismatch");

    png_set_expand(png);
    png_set_strip_16(png);
    png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
    png_read_update_info(png, info);

    std::vector<uint8_t> buffer(width * height * 4);
    std::vector<png_bytep> rows(height);

    for (size_t y = 0; y < height; ++y)
        rows[y] = &buffer[y * width * 4];

    png_read_image(png, rows.data());

    for (size_t i = 0; i < width * height; ++i) {
        uint8_t r = buffer[i * 4 + 0] >> 3;
        uint8_t g = buffer[i * 4 + 1] >> 3;
        uint8_t b = buffer[i * 4 + 2] >> 3;
        uint8_t a = buffer[i * 4 + 3] ? 1 : 0;
        uint16_t px = (r << 11) | (g << 6) | (b << 1) | a;
        pictoPhotoRGBABuffer[i] = (px >> 8) | (px << 8);
    }

    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);
}

void ConvertImage(u16* destI, u16* srcRgba16, s32 rgba16Width, s32 pixelLeft, s32 pixelTop, s32 pixelRight,
                  s32 pixelBottom) {
    for (int i = pixelTop; i <= pixelBottom; i++) {
        for (int j = pixelLeft; j <= pixelRight; j++) {
            u16 px = srcRgba16[i * rgba16Width + j];
            px = (px >> 8) | (px << 8);
            destI[(i - pixelTop) * PICTO_PHOTO_WIDTH + (j - pixelLeft)] = px;
        }
    }
}

void DrawPicto(s16 sp2CC) {

    OPEN_DISPS(gPlayState->state.gfxCtx);

    // Get rid of Sepia tone from prior gDPSetPrimColor call
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    gSPInvalidateTexCache(OVERLAY_DISP++, (uintptr_t)(pictoPhotoRGBABuffer) + (0xA00 * sp2CC));
    gDPLoadTextureBlock(OVERLAY_DISP++, (u16*)(pictoPhotoRGBABuffer) + (0x500 * sp2CC), G_IM_FMT_RGBA, G_IM_SIZ_16b,
                        PICTO_PHOTO_WIDTH, 8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void RegisterColorPictograph() {
    COND_VB_SHOULD(VB_PICTO_TAKE, true, {
        PreRender* prerender = va_arg(args, PreRender*);
        ConvertImage(pictoPhotoRGBABuffer, prerender->fbufSave, SCREEN_WIDTH, PICTO_PHOTO_TOPLEFT_X,
                     PICTO_PHOTO_TOPLEFT_Y, (PICTO_PHOTO_TOPLEFT_X + PICTO_PHOTO_WIDTH) - 1,
                     (PICTO_PHOTO_TOPLEFT_Y + PICTO_PHOTO_HEIGHT) - 1);

        // Probably don't need to do this everytime, just on Save (specifically owl save)
        SavePictoPng();
    });

    COND_VB_SHOULD(VB_PICTO_DISPLAY, CVAR, {
        s16* sp2CC = va_arg(args, s16*);

        // might need something better to check for existance
        if (pictoPhotoRGBABuffer[0] != 0) {
            DrawPicto(*sp2CC);
            *should = false;
        }
    });

    COND_HOOK(OnSaveLoad, true, [](s16 fileNum) { fileNumber = fileNum + 1; });

    COND_VB_SHOULD(VB_PICTO_ACTIVATE, true, {
        if (*should) {
            LoadPictoPNG();
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterColorPictograph, { CVAR_NAME });

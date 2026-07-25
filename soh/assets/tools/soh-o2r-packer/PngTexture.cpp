// Encoding a PNG into an N64 texture is the one part of building soh.o2r that nothing else
// provides. Torch decodes N64 texture data out of a rom and libultraship decodes it again for
// rendering, but neither goes the other way. Everything else the packer needs -- the resource
// header, the texture type enum, size arithmetic, directory walking, zipping, portVersion --
// comes from torch, so this file is the whole of what had to be written.
//
// The quantisation follows ZAPD's ZTexture exactly, since these bytes have to match the archives
// ZAPD produced. n64graphics is not a substitute: it scales (x * 15 / 255) where ZAPD shifts
// (x >> 4), which differ for most inputs.

#include "PngTexture.h"

#include <cstdio>
#include <fstream>
#include <vector>

#include "binarytools/BinaryWriter.h"
#include "factories/BaseFactory.h"
#include "factories/ResourceType.h"
#include "n64graphics/stb_image.h"
#include "utils/TextureUtils.h"

namespace fs = std::filesystem;

namespace {

TextureType TypeFromString(const std::string& format) {
    if (format == "rgba32") return TextureType::RGBA32bpp;
    if (format == "rgb5a1") return TextureType::RGBA16bpp;
    if (format == "ci4")    return TextureType::Palette4bpp;
    if (format == "ci8")    return TextureType::Palette8bpp;
    if (format == "i4")     return TextureType::Grayscale4bpp;
    if (format == "i8")     return TextureType::Grayscale8bpp;
    if (format == "ia4")    return TextureType::GrayscaleAlpha4bpp;
    if (format == "ia8")    return TextureType::GrayscaleAlpha8bpp;
    if (format == "ia16")   return TextureType::GrayscaleAlpha16bpp;
    return TextureType::Error;
}

std::vector<uint8_t> Encode(const uint8_t* rgba, int w, int h, TextureType type) {
    auto px = [&](int y, int x, int c) -> uint8_t { return rgba[(((size_t)y * w) + x) * 4 + c]; };
    std::vector<uint8_t> out(TextureUtils::CalculateTextureSize(type, w, h));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const size_t i = (size_t)y * w + x;

            switch (type) {
                case TextureType::RGBA32bpp:
                    out[i * 4 + 0] = px(y, x, 0);
                    out[i * 4 + 1] = px(y, x, 1);
                    out[i * 4 + 2] = px(y, x, 2);
                    out[i * 4 + 3] = px(y, x, 3);
                    break;

                case TextureType::RGBA16bpp: {
                    const uint16_t data = ((px(y, x, 0) >> 3) << 11) | ((px(y, x, 1) >> 3) << 6) |
                                          ((px(y, x, 2) >> 3) << 1) | (px(y, x, 3) != 0);
                    out[i * 2 + 0] = (data & 0xFF00) >> 8;
                    out[i * 2 + 1] = (data & 0x00FF);
                    break;
                }

                case TextureType::Grayscale4bpp:
                    if (x % 2 == 0) {
                        out[i / 2] = (uint8_t)(((px(y, x, 0) / 16) << 4) + (px(y, x + 1, 0) / 16));
                    }
                    break;

                case TextureType::Grayscale8bpp:
                    out[i] = px(y, x, 0);
                    break;

                case TextureType::GrayscaleAlpha4bpp:
                    if (x % 2 == 0) {
                        const uint8_t hi = ((px(y, x, 0) >> 5) << 1) | (px(y, x, 3) != 0);
                        const uint8_t lo = ((px(y, x + 1, 0) >> 5) << 1) | (px(y, x + 1, 3) != 0);
                        out[i / 2] = (uint8_t)((hi << 4) | lo);
                    }
                    break;

                case TextureType::GrayscaleAlpha8bpp:
                    out[i] = (uint8_t)((((px(y, x, 0) >> 4) & 0xF) << 4) | ((px(y, x, 3) >> 4) & 0xF));
                    break;

                case TextureType::GrayscaleAlpha16bpp:
                    out[i * 2 + 0] = px(y, x, 0);
                    out[i * 2 + 1] = px(y, x, 3);
                    break;

                // Palettes need a TLUT this packer has no way to build.
                default:
                    return {};
            }
        }
    }

    return out;
}

} // namespace

namespace PngTexture {

bool IsFormat(const std::string& format) {
    return TypeFromString(format) != TextureType::Error;
}

bool Convert(const fs::path& png, const fs::path& dest, const std::string& format) {
    const TextureType type = TypeFromString(format);

    int w = 0, h = 0, channels = 0;
    uint8_t* pixels = stbi_load(png.string().c_str(), &w, &h, &channels, 4);
    if (pixels == nullptr) {
        fprintf(stderr, "%s: %s\n", png.string().c_str(), stbi_failure_reason());
        return false;
    }

    const std::vector<uint8_t> encoded = Encode(pixels, w, h, type);
    stbi_image_free(pixels);

    if (encoded.empty()) {
        fprintf(stderr, "%s: cannot encode %s\n", png.string().c_str(), format.c_str());
        return false;
    }

    LUS::BinaryWriter writer;
    BaseExporter::WriteHeader(writer, Torch::ResourceType::Texture, 0);
    writer.Write((uint32_t)type);
    writer.Write((uint32_t)w);
    writer.Write((uint32_t)h);
    writer.Write((uint32_t)encoded.size());
    writer.Write((char*)encoded.data(), encoded.size());

    const std::vector<char> payload = writer.ToVector();
    writer.Close();

    fs::create_directories(dest.parent_path());
    std::ofstream out(dest, std::ios::binary);
    out.write(payload.data(), payload.size());

    return out.good();
}

} // namespace PngTexture

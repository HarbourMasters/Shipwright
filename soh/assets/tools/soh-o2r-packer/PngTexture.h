#ifndef PNGTEXTURE_H
#define PNGTEXTURE_H

#include <filesystem>
#include <string>

namespace PngTexture {

// Whether format names an N64 texture format, as in <name>.<format>.png
bool IsFormat(const std::string& format);

// Decodes png and writes it to dest as a libultraship texture resource.
bool Convert(const std::filesystem::path& png, const std::filesystem::path& dest, const std::string& format);

} // namespace PngTexture

#endif

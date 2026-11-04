// Packs soh/assets/custom into soh.o2r -- the port's own assets, as opposed to anything
// extracted from a rom.
//
// usage: soh-o2r-packer <custom assets dir> <out.o2r> <M.m.p>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <string>

#include "Companion.h"
#include "PngTexture.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <custom assets dir> <out.o2r> <M.m.p>\n", argv[0]);
        return 1;
    }

    const fs::path assetsDir = argv[1];
    const fs::path outPath = argv[2];
    const std::string version = argv[3];

    if (!fs::is_directory(assetsDir)) {
        fprintf(stderr, "not a directory: %s\n", assetsDir.string().c_str());
        return 1;
    }

    // Companion::Pack archives a directory as-is, so stage the assets in the shape the archive
    // should have and let torch do the rest.
    const fs::path stage = outPath.string() + ".stage";
    std::error_code ec;
    fs::remove_all(stage, ec);
    fs::create_directories(stage);

    for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const fs::path& path = entry.path();
        const std::string rel = fs::relative(path, assetsDir).generic_string();
        const std::string filename = path.filename().string();

        // <name>.<format>.png becomes a texture resource archived as <name>
        if (std::count(filename.begin(), filename.end(), '.') >= 2 && path.extension() == ".png") {
            const std::string stem = path.stem().string();
            const std::string format = stem.substr(stem.find_last_of('.') + 1);

            if (PngTexture::IsFormat(format)) {
                const std::string arc = rel.substr(0, rel.size() - (format.size() + 5));
                if (!PngTexture::Convert(path, stage / arc, format)) {
                    return 1;
                }
                continue;
            }
        }

        // Only json is carried over from accessibility
        if (rel.find("accessibility") != std::string::npos && path.extension() != ".json") {
            continue;
        }

        fs::create_directories((stage / rel).parent_path());
        fs::copy_file(path, stage / rel, fs::copy_options::overwrite_existing);
    }

    fs::create_directories(outPath.parent_path(), ec);
    fs::remove(outPath, ec);
    Companion::Pack(stage.string(), outPath.string(), ArchiveType::O2R, version);
    fs::remove_all(stage, ec);

    return 0;
}

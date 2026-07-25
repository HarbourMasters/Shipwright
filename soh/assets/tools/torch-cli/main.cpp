// Build-time ROM extraction, for the ExtractAssets target and anything else that needs an
// archive without launching the game.
//
// soh links torch as a static library (USE_STANDALONE=OFF), which compiles out torch's own
// CLI, so this supplies the entry point. It calls the same SohTorch::Extract the game does.

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "TorchExtract.h"

namespace fs = std::filesystem;

static void Usage(const char* argv0) {
    fprintf(stderr, "usage: %s --src <asset yml dir> --dest <output dir> --version <M.m.p> <rom|dir> [rom|dir...]\n",
            argv0);
}

static bool IsRom(const fs::path& path) {
    const std::string ext = path.extension().string();
    return ext == ".z64" || ext == ".n64" || ext == ".v64";
}

// A directory argument extracts every rom directly inside it, which is how the target is
// normally driven: drop a vanilla and a master quest rom in, get oot.o2r and oot-mq.o2r.
static std::vector<std::string> CollectRoms(const std::vector<std::string>& args) {
    std::vector<std::string> roms;

    for (const auto& arg : args) {
        std::error_code ec;
        if (fs::is_directory(arg, ec)) {
            std::vector<std::string> found;
            for (fs::directory_iterator it(arg, ec), end; it != end; it.increment(ec)) {
                if (ec) {
                    break;
                }
                if (it->is_regular_file(ec) && IsRom(it->path())) {
                    found.push_back(it->path().string());
                }
            }
            std::sort(found.begin(), found.end());
            roms.insert(roms.end(), found.begin(), found.end());
        } else {
            roms.push_back(arg);
        }
    }

    return roms;
}

int main(int argc, char** argv) {
    std::string src, dest, version;
    std::vector<std::string> romArgs;

    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        auto next = [&](const char* what) -> std::string {
            if (i + 1 >= argc) {
                fprintf(stderr, "missing argument after %s\n", what);
                exit(1);
            }
            return argv[++i];
        };

        if (arg == "--src") {
            src = next("--src");
        } else if (arg == "--dest") {
            dest = next("--dest");
        } else if (arg == "--version") {
            version = next("--version");
        } else if (!arg.empty() && arg[0] == '-') {
            fprintf(stderr, "unknown option: %s\n", arg.c_str());
            Usage(argv[0]);
            return 1;
        } else {
            romArgs.push_back(arg);
        }
    }

    if (src.empty() || dest.empty() || version.empty() || romArgs.empty()) {
        Usage(argv[0]);
        return 1;
    }

    const std::vector<std::string> roms = CollectRoms(romArgs);
    if (roms.empty()) {
        fprintf(stderr, "no roms found in: ");
        for (const auto& arg : romArgs) {
            fprintf(stderr, "%s ", arg.c_str());
        }
        fprintf(stderr, "\n");
        return 1;
    }

    for (const auto& rom : roms) {
        // A fresh extraction per ROM; torch names the archive from config.yml.
        const std::string archive = SohTorch::Extract(rom, src, dest, version, nullptr);
        if (archive.empty()) {
            fprintf(stderr, "failed to extract %s\n", rom.c_str());
            return 1;
        }
        printf("%s -> %s/%s\n", rom.c_str(), dest.c_str(), archive.c_str());
    }

    return 0;
}

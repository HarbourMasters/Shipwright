#include "OotrsArchive.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include "ship/Context.h"
#include "ship/resource/ResourceManager.h"
#include "ship/resource/archive/ArchiveManager.h"
#include "ship/utils/binarytools/BinaryWriter.h"
#include "spdlog/spdlog.h"

#include "soh/resource/type/SohResourceType.h"

namespace SOH {

namespace {

size_t gOotrsSongCount = 0;
std::vector<std::string> gOotrsSkipped;

bool HasExtension(const std::string& name, const std::string& extension) {
    if (name.length() < extension.length()) {
        return false;
    }

    return std::equal(extension.rbegin(), extension.rend(), name.rbegin(),
                      [](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

std::string EntryStem(const std::string& name) {
    size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

std::string Trim(const std::string& value) {
    size_t start = 0;
    size_t end = value.length();

    while (start < end && (unsigned char)value[start] <= ' ') {
        start++;
    }
    while (end > start && (unsigned char)value[end - 1] <= ' ') {
        end--;
    }

    return value.substr(start, end - start);
}

std::vector<std::string> SplitLines(const std::vector<char>& buffer) {
    std::string text(buffer.begin(), buffer.end());

    if (text.rfind("\xEF\xBB\xBF", 0) == 0) {
        text = text.substr(3);
    }

    std::vector<std::string> lines;
    size_t start = 0;
    while (start <= text.length()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) {
            lines.push_back(Trim(text.substr(start)));
            break;
        }
        lines.push_back(Trim(text.substr(start, end - start)));
        start = end + 1;
    }

    return lines;
}

std::vector<std::string> SplitList(const std::string& value, char separator) {
    std::vector<std::string> parts;
    size_t start = 0;

    while (start <= value.length()) {
        size_t end = value.find(separator, start);
        std::string part = Trim(end == std::string::npos ? value.substr(start) : value.substr(start, end - start));
        if (!part.empty()) {
            parts.push_back(part);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }

    return parts;
}

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](char c) { return (char)std::tolower(c); });
    return value;
}

} // namespace

bool ParseOotrsMeta(const std::vector<char>& buffer, OotrsMeta& out) {
    std::vector<std::string> lines = SplitLines(buffer);

    if (lines.size() < 2 || lines[0].empty()) {
        return false;
    }

    out.name = lines[0];

    const std::string& bank = lines[1];
    if (bank.empty() || bank == "-") {
        out.hasCustomBank = true;
    } else {
        out.fontIndex = (uint8_t)std::strtoul(bank.c_str(), nullptr, 16);
    }

    if (lines.size() > 2) {
        out.isFanfare = ToLower(lines[2]) == "fanfare";
    }

    if (lines.size() > 3) {
        out.groups = SplitList(lines[3], ',');
    }

    for (size_t i = 4; i < lines.size(); i++) {
        if (!lines[i].empty()) {
            out.zsounds.push_back(lines[i]);
        }
    }

    return true;
}

OotrsArchive::OotrsArchive(const std::string& archivePath)
    : Archive(archivePath), mZip(nullptr), mHasCustomBank(false) {
}

OotrsArchive::~OotrsArchive() {
    SPDLOG_TRACE("destruct OotrsArchive: {}", GetPath());
    Close();
}

bool OotrsArchive::Open() {
    mSongs.clear();
    mHasCustomBank = false;

    mZip = std::make_shared<Ship::O2rArchive>(GetPath());
    if (!mZip->Open()) {
        mZip = nullptr;
        return false;
    }

    std::unordered_map<std::string, std::string> seqEntries;
    std::unordered_map<std::string, std::string> metaEntries;

    for (const auto& [hash, name] : *mZip->ListFiles()) {
        if (HasExtension(name, ".zbank") || HasExtension(name, ".bankmeta")) {
            mHasCustomBank = true;
        } else if (HasExtension(name, ".seq")) {
            seqEntries[EntryStem(name)] = name;
        } else if (HasExtension(name, ".meta")) {
            metaEntries[EntryStem(name)] = name;
        }
    }

    std::string packName = std::filesystem::path(GetPath()).stem().generic_string();

    if (mHasCustomBank) {
        SPDLOG_WARN("Custom music pack \"{}\" ships a custom soundbank, which is not supported yet. Skipping.",
                    packName);
        return true;
    }

    for (const auto& [stem, seqEntry] : seqEntries) {
        std::string metaEntry;
        auto match = metaEntries.find(stem);
        if (match != metaEntries.end()) {
            metaEntry = match->second;
        } else if (seqEntries.size() == 1 && metaEntries.size() == 1) {
            metaEntry = metaEntries.begin()->second;
        } else {
            SPDLOG_WARN("Custom music pack \"{}\" has no meta file for sequence \"{}\". Skipping.", packName, seqEntry);
            continue;
        }

        auto metaFile = mZip->LoadFile(metaEntry);
        if (metaFile == nullptr || !metaFile->IsLoaded) {
            SPDLOG_WARN("Custom music pack \"{}\" could not read meta file \"{}\". Skipping.", packName, metaEntry);
            continue;
        }

        OotrsMeta meta;
        if (!ParseOotrsMeta(*metaFile->Buffer, meta)) {
            SPDLOG_WARN("Custom music pack \"{}\" has a malformed meta file \"{}\". Skipping.", packName, metaEntry);
            continue;
        }

        if (meta.hasCustomBank) {
            mHasCustomBank = true;
            SPDLOG_INFO("Custom music pack \"{}\" ships a custom soundbank, which is not supported yet. Skipping.",
                        packName);
            mSongs.clear();
            return true;
        }

        std::string name = meta.name;
        std::replace(name.begin(), name.end(), '_', ' ');

        std::string virtualPath = "custom/music/" + packName + "/" + name + (meta.isFanfare ? "_fanfare" : "_bgm");

        mSongs[virtualPath] = { seqEntry, meta.fontIndex };
        IndexFile(virtualPath);
    }

    return true;
}

bool OotrsArchive::Close() {
    if (mZip == nullptr) {
        return true;
    }

    bool closed = mZip->Close();
    mZip = nullptr;
    return closed;
}

bool OotrsArchive::WriteFile(const std::string& filename, const std::vector<uint8_t>& data) {
    SPDLOG_ERROR("Cannot write \"{}\": ootrs archives are read only.", filename);
    return false;
}

std::shared_ptr<Ship::File> OotrsArchive::LoadFile(uint64_t hash) {
    const std::string* filePath =
        Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->HashToString(hash);
    if (filePath == nullptr) {
        return nullptr;
    }

    return LoadFile(*filePath);
}

std::shared_ptr<Ship::File> OotrsArchive::LoadFile(const std::string& filePath) {
    auto song = mSongs.find(filePath);
    if (song == mSongs.end() || mZip == nullptr) {
        return nullptr;
    }

    auto seqFile = mZip->LoadFile(song->second.zipSeqEntry);
    if (seqFile == nullptr || !seqFile->IsLoaded) {
        SPDLOG_ERROR("Failed to read sequence \"{}\" from {}", song->second.zipSeqEntry, GetPath());
        return nullptr;
    }

    // Temp workaround to loading info directly. Easier to get into the system than handling of .meta files
    auto fileToLoad = std::make_shared<Ship::File>();
    fileToLoad->Buffer = BuildSequenceResource(*seqFile->Buffer, song->second.fontIndex);
    fileToLoad->IsLoaded = true;

    return fileToLoad;
}

std::shared_ptr<std::vector<char>> OotrsArchive::BuildSequenceResource(const std::vector<char>& seqData,
                                                                       uint8_t fontIndex) {
    Ship::BinaryWriter writer;
    writer.SetEndianness(Ship::Endianness::Native);

    writer.Write((uint8_t)Ship::Endianness::Native);
    writer.Write((uint8_t)1);
    writer.Write((uint8_t)0);
    writer.Write((uint8_t)0);
    writer.Write((uint32_t)ResourceType::SOH_AudioSequence);
    writer.Write((uint32_t)2);
    writer.Write((uint64_t)0xDEADBEEFDEADBEEF);
    while (writer.GetBaseAddress() < OTR_HEADER_SIZE) {
        writer.Write((uint8_t)0);
    }

    writer.Write((uint32_t)seqData.size());
    writer.Write(const_cast<char*>(seqData.data()), seqData.size());

    writer.Write((uint8_t)0);
    writer.Write((uint8_t)2);
    writer.Write((uint8_t)2);
    writer.Write((uint32_t)1);
    writer.Write(fontIndex);

    return std::make_shared<std::vector<char>>(writer.ToVector());
}

bool OotrsArchive::HasCustomBank() const {
    return mHasCustomBank;
}

size_t OotrsArchive::GetSongCount() const {
    return mSongs.size();
}

void MountOotrsArchives(const std::vector<std::filesystem::path>& paths) {
    auto archiveManager = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();

    for (const auto& path : paths) {
        auto archive = std::make_shared<OotrsArchive>(path.generic_string());
        archive->Load();

        if (!archive->IsLoaded()) {
            SPDLOG_ERROR("Failed to load custom music pack \"{}\"", path.generic_string());
            continue;
        }

        if (archive->HasCustomBank()) {
            gOotrsSkipped.push_back(path.filename().generic_string());
            continue;
        }

        if (archive->GetSongCount() == 0) {
            SPDLOG_WARN("Custom music pack \"{}\" contains no usable sequences.", path.generic_string());
            continue;
        }

        if (archiveManager->AddArchive(archive) == nullptr) {
            continue;
        }

        gOotrsSongCount += archive->GetSongCount();
    }
}

size_t GetOotrsSongCount() {
    return gOotrsSongCount;
}

const std::vector<std::string>& GetOotrsSkippedForCustomBank() {
    return gOotrsSkipped;
}

} // namespace SOH

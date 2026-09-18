#pragma once

#include <filesystem>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "ship/resource/File.h"
#include "ship/resource/archive/Archive.h"
#include "ship/resource/archive/O2rArchive.h"

namespace SOH {

struct OotrsMeta {
    std::string name;
    uint8_t fontIndex = 0;
    bool hasCustomBank = false;
    bool isFanfare = false;
    std::vector<std::string> groups;
    std::vector<std::string> zsounds;
};

struct OotrsSong {
    std::string zipSeqEntry;
    uint8_t fontIndex;
};

class OotrsArchive final : virtual public Ship::Archive {
  public:
    OotrsArchive(const std::string& archivePath);
    ~OotrsArchive();

    bool Open() override;
    bool Close() override;
    bool WriteFile(const std::string& filename, const std::vector<uint8_t>& data) override;

    std::shared_ptr<Ship::File> LoadFile(const std::string& filePath) override;
    std::shared_ptr<Ship::File> LoadFile(uint64_t hash) override;

    bool HasCustomBank() const;
    size_t GetSongCount() const;

  private:
    std::shared_ptr<std::vector<char>> BuildSequenceResource(const std::vector<char>& seqData, uint8_t fontIndex);

    std::shared_ptr<Ship::O2rArchive> mZip;
    std::unordered_map<std::string, OotrsSong> mSongs;
    bool mHasCustomBank;
};

bool ParseOotrsMeta(const std::vector<char>& buffer, OotrsMeta& out);

void MountOotrsArchives(const std::vector<std::filesystem::path>& paths);

size_t GetOotrsSongCount();

const std::vector<std::string>& GetOotrsSkippedForCustomBank();

} // namespace SOH

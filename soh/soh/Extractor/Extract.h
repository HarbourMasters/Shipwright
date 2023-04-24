#ifndef EXTRACT_H
#define EXTRACT_H

#include <stdint.h>
#include <string>
#include <memory>
#include <vector>

// Values come from windows.h
#ifndef IDYES
#define IDYES 6
#endif
#ifndef IDNO
#define IDNO 7
#endif

static constexpr size_t MB_BASE = 1024 * 1024;
static constexpr size_t MB32 = 32 * MB_BASE;
static constexpr size_t MB54 = 54 * MB_BASE;
static constexpr size_t MB64 = 64 * MB_BASE;

class Extractor {
    std::unique_ptr<unsigned char[]> mRomData = std::make_unique<unsigned char[]>(MB64);
    std::string mCurrentRomPath;
    size_t mCurRomSize = 0;

    bool GetRomPathFromBox();

    uint32_t GetRomVerCrc() const;
    size_t GetCurRomSize() const;
    bool ValidateAndFixRom();
    bool ValidateRomSize() const;

    bool ValidateRom(bool skipCrcBox = false);
    const char* GetZapdVerStr() const;
    bool IsMasterQuest() const;
    
    void SetRomInfo(const std::string& path);

    void GetRoms(std::vector<std::string>& roms);
    void ShowSizeErrorBox() const;
    void ShowCrcErrorBox() const;
    int ShowRomPickBox(uint32_t verCrc) const;

  public:
    //TODO create some kind of abstraction for message boxes.
    static int ShowYesNoBox(const char* title, const char* text);
    static void ShowErrorBox(const char* title, const char* text);

    bool Run();
    bool CallZapd();
    const char* GetZapdStr();
};
#endif

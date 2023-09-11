#ifdef _WIN32
#include <Windows.h>
#include <winuser.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#endif
#include "Extract.h"
#include "portable-file-dialogs.h"
#include <Utils/BitConverter.h>

#ifdef unix
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef _MSC_VER
#define BSWAP32 _byteswap_ulong
#define BSWAP16 _byteswap_ushort
#elif __has_include(<byteswap.h>)
#include <byteswap.h>
#define BSWAP32 bswap_32
#define BSWAP16 bswap_16
#else
#define BSWAP16(value) ((((value)&0xff) << 8) | ((value) >> 8))

#define BSWAP32(value) \
    (((uint32_t)BSWAP16((uint16_t)((value)&0xffff)) << 16) | (uint32_t)BSWAP16((uint16_t)((value) >> 16)))
#endif

#if defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#elif __llvm__
#define UNREACHABLE __builtin_assume(0)
#else
#define UNREACHABLE __builtin_unreachable();
#endif

#include <stdlib.h>

#include <SDL2/SDL_messagebox.h>

#include <array>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <random>
#include <string>

extern "C" uint32_t CRC32C(unsigned char* data, size_t dataSize);

static constexpr uint32_t OOT_PAL_GC = 0x09465AC3;
static constexpr uint32_t OOT_PAL_GC_DBG1 = 0x871E1C92; // 03-21-2002 build
static constexpr uint32_t OOT_PAL_GC_DBG2 = 0x87121EFE; // 03-13-2002 build
static constexpr uint32_t OOT_PAL_GC_MQ_DBG = 0x917D18F6;
static constexpr uint32_t OOT_PAL_10 = 0xB044B569;
static constexpr uint32_t OOT_PAL_11 = 0xB2055FBD;

static const std::unordered_map<uint32_t, const char*> verMap = {
    { OOT_PAL_GC, "PAL Gamecube" },
    { OOT_PAL_GC_DBG1, "PAL Debug 1" },
    { OOT_PAL_GC_DBG2, "PAL Debug 2" },
    { OOT_PAL_GC_MQ_DBG, "PAL MQ Debug" },
    { OOT_PAL_11, "PAL N64 1.1" },
};

// TODO only check the first 54MB of the rom.
static constexpr std::array<const uint32_t, 8> goodCrcs = {
    0xfa8c0555, // MQ DBG 64MB (Original overdump)
    0x8652ac4c, // MQ DBG 64MB
    0x5B8A1EB7, // MQ DBG 64MB (Empty overdump)
    0x1f731ffe, // MQ DBG 54MB
    0x044b3982, // NMQ DBG 54MB
    0xEB15D7B9, // NMQ DBG 64MB
    0xDA8E61BF, // GC PAL
    0xE033FBBA, // N64 PAL 1.1
};

enum class ButtonId : int {
    YES,
    NO,
    FIND,
};


void Extractor::ShowErrorBox(const char* title, const char* text) {
#ifdef _WIN32
    MessageBoxA(nullptr, text, title, MB_OK | MB_ICONERROR);
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, nullptr);
#endif
}

void Extractor::ShowSizeErrorBox() const {
    std::unique_ptr<char[]> boxBuffer = std::make_unique<char[]>(mCurrentRomPath.size() + 100);
    snprintf(boxBuffer.get(), mCurrentRomPath.size() + 100,
             "The rom file %s was not a valid size. Was %zu MB, expecting 32, 54, or 64MB.", mCurrentRomPath.c_str(),
             mCurRomSize / MB_BASE);
    ShowErrorBox("Invalid Rom Size", boxBuffer.get());
}

void Extractor::ShowCrcErrorBox() const {
    ShowErrorBox("Rom CRC invalid", "Rom CRC did not match the list of known good roms. Please find another.");
}

int Extractor::ShowRomPickBox(uint32_t verCrc) const {
    std::unique_ptr<char[]> boxBuffer = std::make_unique<char[]>(mCurrentRomPath.size() + 100);
    SDL_MessageBoxData boxData = { 0 };
    SDL_MessageBoxButtonData buttons[3] = { { 0 } };
    int ret;

    buttons[0].buttonid = 0;
    buttons[0].text = "Yes";
    buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    buttons[1].buttonid = 1;
    buttons[1].text = "No";
    buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    buttons[2].buttonid = 2;
    buttons[2].text = "Find ROM";
    boxData.numbuttons = 3;
    boxData.flags = SDL_MESSAGEBOX_INFORMATION;
    boxData.message = boxBuffer.get();
    boxData.title = "Rom Detected";
    boxData.window = nullptr;

    boxData.buttons = buttons;
    snprintf(boxBuffer.get(), mCurrentRomPath.size() + 100,
             "Rom detected: %s, Header CRC32: %8X. It appears to be: %s. Use this rom?", mCurrentRomPath.c_str(),
             verCrc, verMap.at(verCrc));

    SDL_ShowMessageBox(&boxData, &ret);
    return ret;
}

int Extractor::ShowYesNoBox(const char* title, const char* box) {
    int ret;
#ifdef _WIN32
    ret = MessageBoxA(nullptr, box, title, MB_YESNO | MB_ICONQUESTION);
#else
    SDL_MessageBoxData boxData = { 0 };
    SDL_MessageBoxButtonData buttons[2] = { { 0 } };

    buttons[0].buttonid = IDYES;
    buttons[0].text = "Yes";
    buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    buttons[1].buttonid = IDNO;
    buttons[1].text = "No";
    buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    boxData.numbuttons = 2;
    boxData.flags = SDL_MESSAGEBOX_INFORMATION;
    boxData.message = box;
    boxData.title = title;
    boxData.buttons = buttons;
    SDL_ShowMessageBox(&boxData, &ret);
#endif
    return ret;
}

void Extractor::SetRomInfo(const std::string& path) {
    mCurrentRomPath = path;
    mCurRomSize = GetCurRomSize();
}

void Extractor::FilterRoms(std::vector<std::string>& roms, RomSearchMode searchMode) {
    std::ifstream inFile;
    std::vector<std::string>::iterator it = roms.begin();

    while (it != roms.end()) {
        std::string rom = *it;
        SetRomInfo(rom);

        // Skip. We will handle rom size errors later on after filtering
        if (!ValidateRomSize()) {
            it++;
            continue;
        }

        inFile.open(rom, std::ios::in | std::ios::binary);
        inFile.read((char*)mRomData.get(), mCurRomSize);
        inFile.clear();
        inFile.close();

        BitConverter::RomToBigEndian(mRomData.get(), mCurRomSize);

        // Rom doesn't claim to be valid
        // Game type doesn't match search mode
        if (!verMap.contains(GetRomVerCrc()) ||
            (searchMode == RomSearchMode::Vanilla && IsMasterQuest()) ||
            (searchMode == RomSearchMode::MQ && !IsMasterQuest())) {
            it = roms.erase(it);
            continue;
        }

        it++;
    }
}

void Extractor::GetRoms(std::vector<std::string>& roms) {
#ifdef _WIN32
    WIN32_FIND_DATAA ffd;
    HANDLE h = FindFirstFileA(".\\*", &ffd);

    do {
        if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char* ext = PathFindExtensionA(ffd.cFileName);

            // Check for any standard N64 rom file extensions.
            if ((strcmp(ext, ".z64") == 0) || (strcmp(ext, ".n64") == 0) || (strcmp(ext, ".v64") == 0))
                roms.push_back(ffd.cFileName);
        }
    } while (FindNextFileA(h, &ffd) != 0);
    // if (h != nullptr) {
    //    CloseHandle(h);
    //}
#elif unix
    // Open the directory of the app.
    DIR* d = opendir(".");
    struct dirent* dir;

    if (d != NULL) {
        // Go through each file in the directory
        while ((dir = readdir(d)) != NULL) {
            struct stat path;

            // Check if current entry is not folder
            stat(dir->d_name, &path);
            if (S_ISREG(path.st_mode)) {

                // Get the position of the extension character.
                char* ext = strrchr(dir->d_name, '.');
                if (ext != NULL && (strcmp(ext, ".z64") == 0 || strcmp(ext, ".n64") == 0 ||
                    strcmp(ext, ".v64") == 0)) {
                    roms.push_back(dir->d_name);
                }
            }
        }
    }
    closedir(d);
#else
    for (const auto& file : std::filesystem::directory_iterator("./")) {
        if (file.is_directory())
            continue;
        if ((file.path().extension() == ".n64") || (file.path().extension() == ".z64") ||
            (file.path().extension() == ".v64")) {
            roms.push_back((file.path()));
        }
    }
#endif
}

bool Extractor::GetRomPathFromBox() {
#ifdef _WIN32
    OPENFILENAMEA box = { 0 };
    char nameBuffer[512];
    nameBuffer[0] = 0;

    box.lStructSize = sizeof(box);
    box.lpstrFile = nameBuffer;
    box.nMaxFile = sizeof(nameBuffer) / sizeof(nameBuffer[0]);
    box.lpstrTitle = "Open Rom";
    box.Flags = OFN_NOCHANGEDIR | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    box.lpstrFilter = "N64 Roms\0*.z64;*.v64;*.n64\0\0";
    if (!GetOpenFileNameA(&box)) {
        DWORD err = CommDlgExtendedError();
        // GetOpenFileName will return 0 but no error is set if the user just closes the box.
        if (err != 0) {
            const char* errStr = nullptr;
            switch (err) {
                case FNERR_BUFFERTOOSMALL:
                    errStr = "Path buffer too small. Move file closer to root of your drive";
                    break;
                case FNERR_INVALIDFILENAME:
                    errStr = "File name for rom provided is invalid.";
                    break;
                case FNERR_SUBCLASSFAILURE:
                    errStr = "Failed to open a filebox because there is not enough RAM to do so.";
                    break;
            }
            MessageBoxA(nullptr, "Box Error", errStr, MB_OK | MB_ICONERROR);
            return false;
        }
    }
    // The box was closed without something being selected.
    if (nameBuffer[0] == 0) {
        return false;
    }
    mCurrentRomPath = nameBuffer;
    #else
    auto selection = pfd::open_file("Select a file", ".", { "N64 Roms", "*.z64 *.n64 *.v64" }).result();

    if (selection.empty()) {
        return false;
    }

    mCurrentRomPath = selection[0];
    #endif
    mCurRomSize = GetCurRomSize();
    return true;
}

uint32_t Extractor::GetRomVerCrc() const {
    return BSWAP32(((uint32_t*)mRomData.get())[4]);
}

size_t Extractor::GetCurRomSize() const {
    return std::filesystem::file_size(mCurrentRomPath);
}

bool Extractor::ValidateAndFixRom() {
    // The MQ debug rom sometimes has the header patched to look like a US rom. Change it back
    if (GetRomVerCrc() == OOT_PAL_GC_MQ_DBG) {
        mRomData[0x3E] = 'P';
    }

    const uint32_t actualCrc = CRC32C(mRomData.get(), mCurRomSize);

    for (const uint32_t crc : goodCrcs) {
        if (actualCrc == crc) {
            return true;
        }
    }
    return false;
}

bool Extractor::ValidateRomSize() const {
    if (mCurRomSize != MB32 && mCurRomSize != MB54 && mCurRomSize != MB64) {
        return false;
    }
    return true;
}

bool Extractor::ValidateRom(bool skipCrcTextBox) {
    if (!ValidateRomSize()) {
        ShowSizeErrorBox();
        return false;
    }
    if (!ValidateAndFixRom()) {
        if (!skipCrcTextBox) {
            ShowCrcErrorBox();
        }
        return false;
    }
    return true;
}

bool Extractor::ManuallySearchForRom() {
    std::ifstream inFile;

    if (!GetRomPathFromBox()) {
        ShowErrorBox("No rom selected", "No Rom selected. Exiting");
        return false;
    }

    inFile.open(mCurrentRomPath, std::ios::in | std::ios::binary);

    if (!inFile.is_open()) {
        return false; // TODO Handle error
    }

    inFile.read((char*)mRomData.get(), mCurRomSize);
    inFile.close();
    BitConverter::RomToBigEndian(mRomData.get(), mCurRomSize);

    if (!ValidateRom()) {
        return false;
    }

    return true;
}

bool Extractor::ManuallySearchForRomMatchingType(RomSearchMode searchMode) {
    if (!ManuallySearchForRom()) {
        return false;
    }

    char msgBuf[150];
    snprintf(msgBuf, 150, "The selected rom does not match the expected game type\nExpected type: %s.\n\nDo you want to search again?",
        searchMode == RomSearchMode::MQ ? "Master Quest" : "Vanilla");

    while ((searchMode == RomSearchMode::Vanilla && IsMasterQuest()) ||
           (searchMode == RomSearchMode::MQ && !IsMasterQuest())) {
        int ret = ShowYesNoBox("Wrong Game Type", msgBuf);
        switch (ret) {
            case IDYES:
                if (!ManuallySearchForRom()) {
                    return false;
                }
                continue;
            case IDNO:
                return false;
            default:
                UNREACHABLE;
                break;
        }
    }

    return true;
}

bool Extractor::Run(RomSearchMode searchMode) {
    std::vector<std::string> roms;
    std::ifstream inFile;

    GetRoms(roms);
    FilterRoms(roms, searchMode);

    if (roms.empty()) {
        int ret = ShowYesNoBox("No roms found", "No roms found. Look for one?");

        switch (ret) {
            case IDYES:
                if (!ManuallySearchForRomMatchingType(searchMode)) {
                    return false;
                }
                break;
            case IDNO:
                ShowErrorBox("No rom selected", "No rom selected. Exiting");
                return false;
            default:
                UNREACHABLE;
                break;
        }
    }

    for (const auto& rom : roms) {
        SetRomInfo(rom);

        if (!ValidateRomSize()) {
            ShowSizeErrorBox();
            continue;
        }

        inFile.open(rom, std::ios::in | std::ios::binary);
        inFile.read((char*)mRomData.get(), mCurRomSize);
        inFile.clear();
        inFile.close();
        BitConverter::RomToBigEndian(mRomData.get(), mCurRomSize);

        int option = ShowRomPickBox(GetRomVerCrc());

        if (option == (int)ButtonId::YES) {
            if (!ValidateRom(true)) {
                if (rom == roms.back()) {
                    ShowCrcErrorBox();
                } else {
                    ShowErrorBox("Rom CRC invalid",
                                 "Rom CRC did not match the list of known good roms. Trying the next one...");
                }
                continue;
            }
            break;
        } else if (option == (int)ButtonId::FIND) {
            if (!ManuallySearchForRomMatchingType(searchMode)) {
                return false;
            }
            break;
        } else if (option == (int)ButtonId::NO) {
            if (rom == roms.back()) {
                ShowErrorBox("No rom provided", "No rom provided. Exiting");
                return false;
            }
            continue;
        }
        break;
    }
    return true;
}

bool Extractor::IsMasterQuest() const {
    switch (GetRomVerCrc()) {
        case OOT_PAL_GC_MQ_DBG:
            return true;
        case OOT_PAL_10:
        case OOT_PAL_11:
        case OOT_PAL_GC:
        case OOT_PAL_GC_DBG1:
            return false;
        default:
            UNREACHABLE;
    }
}

const char* Extractor::GetZapdVerStr() const {
    switch (GetRomVerCrc()) {
        case OOT_PAL_GC:
            return "GC_NMQ_PAL_F";
        case OOT_PAL_GC_DBG1:
            return "GC_NMQ_D";
        case OOT_PAL_GC_MQ_DBG:
            return "GC_MQ_D";
        case OOT_PAL_11:
            return "N64_PAL_11";
        default:
            // We should never be in a state where this path happens.
            UNREACHABLE;
            break;
    }
}

std::string Extractor::Mkdtemp() {
    std::string temp_dir = std::filesystem::temp_directory_path().string();
    
    // create 6 random alphanumeric characters
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 1);

    char randchr[7];
    for (int i = 0; i < 6; i++) {
        randchr[i] = charset[dist(gen)];
    }
    randchr[6] = '\0';

    std::string tmppath = temp_dir + "/extractor-" + randchr;
    std::filesystem::create_directory(tmppath);
    return tmppath;
}

extern "C" int zapd_main(int argc, char** argv);

bool Extractor::CallZapd(std::string installPath, std::string exportdir) {
    constexpr int argc = 16;
    char xmlPath[1024];
    char confPath[1024];
    std::array<const char*, argc> argv;
    const char* version = GetZapdVerStr();
    const char* otrFile = IsMasterQuest() ? "oot-mq.otr" : "oot.otr";

    std::string romPath = std::filesystem::absolute(mCurrentRomPath).string();
    installPath = std::filesystem::absolute(installPath).string();
    exportdir = std::filesystem::absolute(exportdir).string();
    // Work this out in the temporary folder
    std::string tempdir = Mkdtemp();
    std::string curdir = std::filesystem::current_path().string();
#ifdef _WIN32
    std::filesystem::copy(installPath + "/assets", tempdir + "/assets",
        std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing);
#else
    std::filesystem::create_symlink(installPath + "/assets", tempdir + "/assets");
#endif

    std::filesystem::current_path(tempdir);

    snprintf(xmlPath, 1024, "assets/extractor/xmls/%s", version);
    snprintf(confPath, 1024, "assets/extractor/Config_%s.xml", version);

    argv[0] = "ZAPD";
    argv[1] = "ed";
    argv[2] = "-i";
    argv[3] = xmlPath;
    argv[4] = "-b";
    argv[5] = romPath.c_str();
    argv[6] = "-fl";
    argv[7] = "assets/extractor/filelists";
    argv[8] = "-gsf";
    argv[9] = "1";
    argv[10] = "-rconf";
    argv[11] = confPath;
    argv[12] = "-se";
    argv[13] = "OTR";
    argv[14] = "--otrfile";
    argv[15] = otrFile;

#ifdef _WIN32
    // Grab a handle to the command window.
    HWND cmdWindow = GetConsoleWindow();

    // Normally the command window is hidden. We want the window to be shown here so the user can see the progess of the extraction.
    ShowWindow(cmdWindow, SW_SHOW);
    SetWindowPos(cmdWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
#endif

    zapd_main(argc, (char**)argv.data());

#ifdef _WIN32
    // Hide the command window again.
    ShowWindow(cmdWindow, SW_HIDE);
#endif

    std::filesystem::copy(otrFile, exportdir + "/" + otrFile, std::filesystem::copy_options::overwrite_existing);

    // Go back to where this game was executed from
    std::filesystem::current_path(curdir);
    std::filesystem::remove_all(tempdir);

    return 0;
}


#include "BenPort.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <chrono>

#include <ship/resource/ResourceManager.h>
#include <fast/Fast3dWindow.h>
#include <ship/resource/File.h>
#include <ship/window/Window.h>

#include "z64animation.h"
#include "z64bgcheck.h"
#include <libultraship/libultra/gbi.h>
#include <ship/window/gui/Fonts.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif
#include <ship/audio/AudioPlayer.h>
#include "variables.h"
#include "z64.h"
#include "macros.h"
#include <ship/utils/StringHelper.h>
#include <nlohmann/json.hpp>
#include "build.h"
#include <stb_image.h>

#include <fast/interpreter.h>
#include <fast/backends/gfx_rendering_api.h>
#include <fast/Fast3dWindow.h>

#ifdef __APPLE__
#include <SDL_scancode.h>
#else
#include <SDL2/SDL_scancode.h>
#endif
#include "Extractor/Extract.h"
// OTRTODO
// #include <functions.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#ifdef ENABLE_CROWD_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
CrowdControl* CrowdControl::Instance;
#endif

#include <libultraship/libultraship.h>
#include <libultraship/controller/controldeck/ControlDeck.h>
#include <fast/resource/ResourceType.h>
#include <BenGui/BenGui.hpp>
#include <BenGui/BenMenu.h>

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/Enhancements/GfxPatcher/AuthenticGfxPatches.h"
#include "2s2h/Enhancements/GfxPatcher/PlayerCustomFlipbooks.h"
#include "2s2h/Enhancements/ModMenu/ModMenu.h"
#include "2s2h/DeveloperTools/DebugConsole.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/Rando/Spoiler/Spoiler.h"
#include "2s2h/SaveManager/SaveManager.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/PresetManager/PresetManager.h"
#include "2s2h/config/ConfigUpdaters.h"

// Resource Types/Factories
#include <ship/resource/type/Blob.h>
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/type/Matrix.h>
#include <fast/resource/type/Texture.h>
#include <fast/resource/type/Vertex.h>
#include "2s2h/resource/type/2shResourceType.h"
#include "2s2h/resource/type/Animation.h"
#include "2s2h/resource/type/Array.h"
#include "2s2h/resource/type/AudioSample.h"
#include "2s2h/resource/type/AudioSequence.h"
#include "2s2h/resource/type/AudioSoundFont.h"
#include "2s2h/resource/type/CollisionHeader.h"
#include "2s2h/resource/type/Cutscene.h"
#include "2s2h/resource/type/Path.h"
#include "2s2h/resource/type/PlayerAnimation.h"
#include "2s2h/resource/type/Scene.h"
#include "2s2h/resource/type/Skeleton.h"
#include "2s2h/resource/type/SkeletonLimb.h"
#include <ship/resource/factory/BlobFactory.h>
#include <fast/resource/factory/DisplayListFactory.h>
#include <fast/resource/factory/MatrixFactory.h>
#include <fast/resource/factory/TextureFactory.h>
#include <fast/resource/factory/VertexFactory.h>
#include "2s2h/resource/importer/AnimationFactory.h"
#include "2s2h/resource/importer/ArrayFactory.h"
#include "2s2h/resource/importer/AudioSampleFactory.h"
#include "2s2h/resource/importer/AudioSequenceFactory.h"
#include "2s2h/resource/importer/AudioSoundFontFactory.h"
#include "2s2h/resource/importer/CollisionHeaderFactory.h"
#include "2s2h/resource/importer/CutsceneFactory.h"
#include "2s2h/resource/importer/PathFactory.h"
#include "2s2h/resource/importer/PlayerAnimationFactory.h"
#include "2s2h/resource/importer/SceneFactory.h"
#include "2s2h/resource/importer/SkeletonFactory.h"
#include "2s2h/resource/importer/SkeletonLimbFactory.h"
#include "2s2h/resource/importer/TextMMFactory.h"
#include "2s2h/resource/importer/BackgroundFactory.h"
#include "2s2h/resource/importer/TextureAnimationFactory.h"
#include "2s2h/resource/importer/KeyFrameFactory.h"
#include <ship/window/gui/resource/Font.h>
#include <ship/window/FileDropMgr.h>
#include <ship/window/gui/resource/FontFactory.h>
#include "2s2h/Enhancements/Audio/AudioCollection.h"
#include "BenGui/BenInputEditorWindow.h"

OTRGlobals* OTRGlobals::Instance;
GameInteractor* GameInteractor::Instance;
AudioCollection* AudioCollection::Instance;

extern "C" char** cameraStrings;
bool prevAltAssets = false;
std::vector<std::shared_ptr<std::string>> cameraStdStrings;

Color_RGB8 kokiriColor = { 0x1E, 0x69, 0x1B };
Color_RGB8 goronColor = { 0x64, 0x14, 0x00 };
Color_RGB8 zoraColor = { 0x00, 0xEC, 0x64 };

int32_t previousImGuiScaleIndex;
float previousImGuiScale;

typedef struct {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
} ArchiveVersion;

std::shared_ptr<Fast::Fast3dWindow> benFast3dWindow;
static ArchiveVersion DetectArchiveVersion(std::string path, bool isO2rType);
static bool VerifyArchiveVersion(ArchiveVersion version);
std::string portArchivePath = "";
static bool shipArchiveVersionMatch = false;

OTRGlobals::OTRGlobals() {
    context = Ship::Context::CreateUninitializedInstance("2 Ship 2 Harkinian", appShortName, "2ship2harkinian.json");

    portArchivePath = Ship::Context::LocateFileAcrossAppDirs("2ship.o2r");
    ArchiveVersion portArchiveVersion = DetectArchiveVersion("2ship.o2r", true);
    shipArchiveVersionMatch = portArchiveVersion.major == gBuildVersionMajor &&
                              portArchiveVersion.minor == gBuildVersionMinor &&
                              portArchiveVersion.patch == gBuildVersionPatch;

    context->InitConfiguration();
    context->InitConsoleVariables();

    auto controlDeck = std::make_shared<LUS::ControlDeck>(std::vector<CONTROLLERBUTTONS_T>({
        BTN_CUSTOM_MODIFIER1,
        BTN_CUSTOM_MODIFIER2,
        BTN_CUSTOM_OCARINA_NOTE_D4,
        BTN_CUSTOM_OCARINA_NOTE_F4,
        BTN_CUSTOM_OCARINA_NOTE_A4,
        BTN_CUSTOM_OCARINA_NOTE_B4,
        BTN_CUSTOM_OCARINA_NOTE_D5,
        BTN_CUSTOM_OCARINA_DISABLE_SONGS,
        BTN_CUSTOM_OCARINA_PITCH_UP,
        BTN_CUSTOM_OCARINA_PITCH_DOWN,
    }));
    context->InitControlDeck(controlDeck);
    context->InitResourceManager({ portArchivePath }, {}, 3, true);
    context->InitConsole();

    auto benInputEditorWindow = std::make_shared<BenInputEditorWindow>("gWindows.BenInputEditor", "2S2H Input Editor");
    benFast3dWindow =
        std::make_shared<Fast::Fast3dWindow>(std::vector<std::shared_ptr<Ship::GuiWindow>>({ benInputEditorWindow }));
    context->InitWindow(benFast3dWindow);

    BenGui::SetupMenu();

    if (shipArchiveVersionMatch) {

        auto overlay = context->GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay();
        overlay->LoadFont("Press Start 2P", 12.0f, "fonts/PressStart2P-Regular.ttf");
        overlay->LoadFont("Fipps", 32.0f, "fonts/Fipps-Regular.otf");
        overlay->SetCurrentFont(CVarGetString(CVAR_GAME_OVERLAY_FONT, "Press Start 2P"));

        fontMono = CreateFontWithSize(16.0f, "fonts/Inconsolata-Regular.ttf");
        fontMonoLarger = CreateFontWithSize(20.0f, "fonts/Inconsolata-Regular.ttf");
        fontMonoLargest = CreateFontWithSize(24.0f, "fonts/Inconsolata-Regular.ttf");
        fontStandard = CreateFontWithSize(16.0f, "fonts/Montserrat-Regular.ttf");
        fontStandardLarger = CreateFontWithSize(20.0f, "fonts/Montserrat-Regular.ttf");
        fontStandardLargest = CreateFontWithSize(24.0f, "fonts/Montserrat-Regular.ttf");
        ImGui::GetIO().FontDefault = fontStandardLarger;
    }

    previousImGuiScaleIndex = -1;
    previousImGuiScale = defaultImGuiScale;
    ScaleImGui();
}

typedef enum ExtractSteps {
    ES_PORT_ARCHIVE,
    ES_WINDOWS,
    ES_EXTRACT_ARGS,
    ES_EXTRACT,
    ES_VERIFY,
} ExtractSteps;

typedef enum PromptSteps {
    PS_FILE_CHECK,
    PS_LOCAL,
    PS_FIRST,
    PS_DUPE,
    PS_WAIT,
    PS_NONE,
} PromptSteps;

typedef enum WindowsSteps {
    WS_TEMP,
    WS_PERMS,
    WS_ONEDRIVE,
    WS_DONE,
} WindowsSteps;

bool IsSubpath(const std::filesystem::path& path, const std::filesystem::path& base) {
    auto rel = std::filesystem::relative(path, base);
    return !rel.empty() && rel.native()[0] != '.';
}

bool PathTestCleanup(FILE* tfile) {
    try {
        if (std::filesystem::exists("./text.txt"))
            std::filesystem::remove("./text.txt");
        if (std::filesystem::exists("./test/"))
            std::filesystem::remove("./test/");
    } catch (std::filesystem::filesystem_error const& ex) { return false; }
    return true;
}

static bool RemoveArchiveAcrossAppDirs(const std::string& fileName) {
    for (const std::string& path : { Ship::Context::GetPathRelativeToAppDirectory(fileName, appShortName),
                                     Ship::Context::GetPathRelativeToAppBundle(fileName), "./" + fileName }) {
        std::error_code err;
        if (std::filesystem::remove(path, err)) {
            SPDLOG_INFO("Removed outdated archive {}", path);
        } else if (err) {
            SPDLOG_ERROR("Failed to remove outdated archive {}: {}", path, err.message());
        }
    }
    return !std::filesystem::exists(Ship::Context::LocateFileAcrossAppDirs(fileName, appShortName));
}

void CheckAndCreateModFolder() {
    try {
        std::string modsPath = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
        if (!std::filesystem::exists(modsPath)) {
            // Create mods folder relative to app dir
            modsPath = Ship::Context::GetPathRelativeToAppDirectory("mods", appShortName);
            std::string filePath = modsPath + "/custom_mod_files_go_here.txt";
            if (std::filesystem::create_directories(modsPath)) {
                std::ofstream(filePath).close();
            }
        }
    } catch (std::filesystem::filesystem_error const& ex) {
        // Couldn't make the folder, continue silently
        return;
    }
}

namespace BenGui {
extern std::shared_ptr<BenGui::BenMenu> mBenMenu;
}

void OTRGlobals::RunExtract(int argc, char* argv[]) {
    bool extractDone = false;
    ExtractSteps extractStep = ES_PORT_ARCHIVE;
    WindowsSteps windowsStep = WS_TEMP;
    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(OTRGlobals::Instance->context->GetWindow());
    auto gui = wnd->GetGui();

    bool shouldRegen = VerifyArchiveVersion(DetectArchiveVersion("mm.o2r", true));

    std::filesystem::path ownPath;
    std::vector<std::string> args;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
    }
    Extractor extract;
    PromptSteps promptStep = PS_FILE_CHECK;
    bool romsFromSearch = false;
    std::atomic<size_t> extractCount = 0, totalExtract = 0;

    std::string installPath = std::filesystem::absolute(Ship::Context::GetAppBundlePath()).string();
    std::string dataPath = std::filesystem::absolute(Ship::Context::GetAppDirectoryPath(appShortName)).string();
    std::string file;

#if defined(__SWITCH__)
    BenGui::RegisterPopup("Outdated ROM Archives",
                          "\x1b[2;2HYou've launched 2Ship with an old ROM O2R file."
                          "\x1b[4;2HPlease regenerate a new ROM O2R and relaunch."
                          "\x1b[6;2HPress the Home button to exit...",
                          "OK", "", [&]() { exit(1); });
#elif defined(__WIIU__)
    BenGui::RegisterPopup("Outdated ROM Archives",
                          "You've launched 2Ship with an old a ROM O2R file.\n\n"
                          "Please generate a ROM O2R and relaunch.\n\n"
                          "Press and hold the Power button to shutdown...",
                          "OK", "", [&]() { exit(1); });
    OSFatal();
#endif

    if (!std::filesystem::exists(installPath + "/assets")) {
        BenGui::RegisterPopup("Extractor assets not found",
                              "No O2R files found. Missing 'assets/' folder needed to generate OTR file.\nPlease "
                              "re-extract them from the download or.\n\nExiting...",
                              "OK", "", [&]() { exit(1); });
    } else if (shouldRegen) {
        if (RemoveArchiveAcrossAppDirs("mm.o2r")) {
            BenGui::RegisterPopup("Outdated ROM Archive",
                                  "Your mm.o2r was created with incompatible versions of 2Ship.\nYou will "
                                  "now be redirected to re-extract them.");
        } else {
            BenGui::RegisterPopup("Outdated ROM Archive",
                                  "Your mm.o2r was created with incompatible versions of 2Ship, but it\n"
                                  "could not be removed automatically. Please delete it and relaunch.\n\nExiting...",
                                  "OK", "", [&]() { exit(1); });
        }
    }

    std::shared_ptr<BS::thread_pool> threadPool = std::make_shared<BS::thread_pool>(1);
    std::optional<std::future<void>> extractionTask;

#if not defined(__SWITCH__) && not defined(__WIIU__)
    CheckAndCreateModFolder();
#endif

    while (!extractDone) {
        if (BenGui::PopupsQueued() > 0 || extractionTask.has_value()) {
            goto render;
        }
        switch (extractStep) {
            case ES_PORT_ARCHIVE: {
                if (shipArchiveVersionMatch) {
#ifdef _WIN32
                    extractStep = ES_WINDOWS;
#elif (defined(__WIIU__) || defined(__SWITCH__))
                    extractStep = ES_VERIFY;
#else
                    extractStep = args.empty() ? ES_EXTRACT : ES_EXTRACT_ARGS;
#endif
                } else {
                    std::string msg;

#if defined(__SWITCH__)
                    msg = "\x1b[4;2HPlease re-extract it from the download.\n"
                          "\x1b[6;2HPress the Home button to exit...";
#elif defined(__WIIU__)
                    msg = "Please extract the 2ship.o2r from the 2 Ship 2 Harkinian download\nto your folder.\n\nPress "
                          "and hold the power\n"
                          "button to shutdown...";
#else
                    msg = "Please extract the 2ship.o2r from the 2 Ship 2 Harkinian download to your "
                          "folder.\n\nExiting...";
#endif
                    std::string title =
                        !std::filesystem::exists(portArchivePath) ? "Missing 2ship.o2r" : "2ship.o2r is outdated";
                    BenGui::RegisterPopup(title, msg, "OK", "", [&]() { exit(1); });
                }
                continue;
            }
            case ES_WINDOWS: {
                switch (windowsStep) {
                    case WS_TEMP: {
#ifdef _WIN32
                        char* tempVar = getenv("TEMP");
                        std::filesystem::path tempPath;
                        try {
                            tempPath = std::filesystem::canonical(tempVar);
                        } catch (std::filesystem::filesystem_error const& ex) {
                            std::string userPath = getenv("USERPROFILE");
                            userPath.append("\\AppData\\Local\\Temp");
                            tempPath = std::filesystem::canonical(userPath);
                        }
                        wchar_t buffer[MAX_PATH];
                        GetModuleFileName(NULL, buffer, _countof(buffer));
                        ownPath = std::filesystem::canonical(buffer).parent_path();
                        if (IsSubpath(ownPath, tempPath)) {
                            BenGui::RegisterPopup("2S2H Path Error",
                                                  "2S2H is running in a temp folder.\nExtract the .zip and run again.",
                                                  "OK", "", [&]() { exit(0); });
                        } else {
                            windowsStep = WS_PERMS;
                        }
#endif
                        continue;
                    }
                    case WS_PERMS: {
                        FILE* tfile = fopen("./text.txt", "w");
                        std::filesystem::path tfolder = std::filesystem::path("./test/");
                        bool error = false;
                        try {
                            create_directories(tfolder);
                        } catch (std::filesystem::filesystem_error const& ex) { error = true; }
                        if (tfile == NULL || error) {
                            BenGui::RegisterPopup("2S2H Permissions Error",
                                                  "2S2H does not have proper file permissions.\nPlease move it to a "
                                                  "folder that does and run again.",
                                                  "OK", "", [&]() {
                                                      fclose(tfile);
                                                      PathTestCleanup(tfile);
                                                      exit(0);
                                                  });
                        } else {
                            fclose(tfile);
                            if (!PathTestCleanup(tfile)) {
                                BenGui::RegisterPopup(
                                    "2S2H Permissions Error",
                                    "2S2H does not have proper file permissions.\nPlease move it to a "
                                    "folder that does and run again.",
                                    "OK", "", [&]() { exit(0); });
                            }
                            windowsStep = WS_ONEDRIVE;
                        }
                        continue;
                    }
                    case WS_ONEDRIVE: {
                        if (ownPath.string().find("OneDrive") != std::string::npos) {
                            BenGui::RegisterPopup("2S2H Path Error",
                                                  "2S2H appears to be in a OneDrive folder, which will cause issues.\n"
                                                  "Please move it to a folder outside of OneDrive, like the root of a\n"
                                                  "drive (e.g. \"C:\\Games\\2S2H\").",
                                                  "OK", "", [&]() { exit(0); });
                        } else {
                            windowsStep = WS_DONE;
                            extractStep = args.empty() ? ES_EXTRACT : ES_EXTRACT_ARGS;
                        }
                        continue;
                    }
                    default:
                        continue;
                }
                break;
            }
            case ES_EXTRACT_ARGS: {
#if !defined(__SWITCH__) && !defined(__WIIU__)
                if (args.empty()) {
                    if (!std::filesystem::exists(Ship::Context::LocateFileAcrossAppDirs("mm.o2r", appShortName))) {
                        extractStep = ES_EXTRACT;
                        promptStep = PS_FILE_CHECK;
                    } else {
                        extractStep = ES_VERIFY;
                    }
                    continue;
                }
                if (romsFromSearch &&
                    std::filesystem::exists(Ship::Context::LocateFileAcrossAppDirs("mm.o2r", appShortName))) {
                    SPDLOG_INFO("mm.o2r generated, skipping {} other ROM(s) found in the app folder", args.size());
                    args.clear();
                    continue;
                }
                file = args.at(0);
                args.erase(args.begin());
                extract = Extractor();
                if (extract.RunFileStandalone(file)) {
                    extractionTask = threadPool->submit_task([&]() -> void {
                        extract.CallZapd(installPath, Ship::Context::GetAppDirectoryPath(appShortName), &extractCount,
                                         &totalExtract);
                        extractCount = totalExtract = 0;
                    });
                } else {
                    bool open = true;
                    std::string msg = "File\n" + std::string(file) + "\nis not a ROM or does not match supported ROMs.";
                    BenGui::RegisterPopup("2S2H ROM Error", msg.c_str());
                }
#else
                extractStep = ES_VERIFY;
#endif
                break;
            }
            case ES_EXTRACT: {
                switch (promptStep) {
                    case PS_FILE_CHECK: {
                        if (!std::filesystem::exists(Ship::Context::LocateFileAcrossAppDirs("mm.o2r", appShortName))) {
                            BenGui::RegisterPopup(
                                "No O2R Files", "No O2R files found. Generate one now?", "Yes", "No",
                                [&]() { promptStep = PS_LOCAL; }, [&]() { exit(0); });
                        } else {
                            extractStep = ES_VERIFY;
                        }
                        continue;
                    }
                    case PS_LOCAL: {
                        extract = Extractor();
                        extract.SetSearchPath(installPath);
                        extract.GetRoms(args);
                        if (installPath != dataPath) {
                            extract.SetSearchPath(dataPath);
                            extract.GetRoms(args);
                        }
                        romsFromSearch = !args.empty();
                        if (!args.empty()) {
                            promptStep = PS_WAIT;
                            BenGui::RegisterPopup(
                                "ROMs found", "ROMs found in application directory. Would you like to process them?",
                                "Yes", "No", [&]() { extractStep = ES_EXTRACT_ARGS; },
                                [&]() { promptStep = PS_FIRST; });
                        } else {
                            promptStep = PS_FIRST;
                        }
                        continue;
                    }
                    case PS_FIRST: {
                        if (!extract.ManuallySearchForRomMatchingType(RomSearchMode::Both)) {
                            promptStep = PS_FILE_CHECK;
                            continue;
                        }
                        extractionTask = threadPool->submit_task([&]() -> void {
                            extract.CallZapd(installPath, Ship::Context::GetAppDirectoryPath(appShortName),
                                             &extractCount, &totalExtract);
                            extractStep = ES_VERIFY;
                            extractCount = 0;
                            totalExtract = 0;
                        });
                        continue;
                    }
                    default:
                        break;
                }
                break;
            }
            case ES_VERIFY: {
                if (!std::filesystem::exists(Ship::Context::LocateFileAcrossAppDirs("mm.o2r", appShortName))) {
                    BenGui::RegisterPopup("No ROM Archives",
                                          "No ROM O2R files detected. Please generate a ROM O2R and relaunch.", "OK",
                                          "", [&]() { exit(0); });
                }
                extractDone = true;
                continue;
            }
            default:
                break;
        }

    render:
        if (!WindowIsRunning()) {
            exit(0);
        }
        // Process window events for resize, mouse, keyboard events
        wnd->HandleEvents();
        UIWidgets::Colors themeColor =
            static_cast<UIWidgets::Colors>(CVarGetInteger("gSettings.Menu.Theme", UIWidgets::Colors::LightBlue));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UIWidgets::ColorValues.at(themeColor));
        ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGray));

        // Skip dropped frames
        if (!wnd->IsFrameReady()) {
            continue;
        }
        gui->StartDraw();
        benFast3dWindow->StartFrame();
        benFast3dWindow->RunGuiOnly();
        if (extractionTask.has_value()) {
            auto status = extractionTask->wait_for(std::chrono::milliseconds(0));
            if (status == std::future_status::ready) {
                try {
                    extractionTask->get();
                } catch (const std::exception& e) {
                    BenGui::RegisterPopup("Extraction Crashed", e.what(), "Close", "", []() { exit(1); });
                }
                extractionTask.reset();
            } else {
                if (!ImGui::IsPopupOpen("ROM Extraction")) {
                    ImGui::OpenPopup("ROM Extraction");
                }
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 8.0f));
                auto color = UIWidgets::ColorValues.at(THEME_COLOR);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(color.x, color.y, color.z, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(color.x, color.y, color.z, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
                if (ImGui::BeginPopupModal("ROM Extraction", NULL,
                                           ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
                                               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                               ImGuiWindowFlags_NoSavedSettings)) {
                    float progress = (totalExtract > 0.0f ? (float)extractCount / (float)totalExtract : 0) * 100.0f;
                    auto filename = std::filesystem::path(file).filename().string();
                    ImGui::Text("Extracting %s...%s", filename.c_str(),
                                roundf(progress) == 100.0f ? " Done. Finishing up." : "");
                    std::string overlay = extractCount > 0 ? fmt::format("{:.0f}%", progress) : "Starting Up";
                    ImGui::ProgressBar(progress / 100.0f, ImVec2(600.0f, 50.0f), overlay.c_str());
                    ImGui::EndPopup();
                }
                ImGui::PopStyleColor(3);
                ImGui::PopStyleVar(2);
            }
        }
        gui->EndDraw();
        benFast3dWindow->EndFrame();
        ImGui::PopStyleColor(2);
    }

#ifdef __SWITCH__
    Ship::Switch::Init(Ship::PreInitPhase);
#elif defined(__WIIU__)
    Ship::WiiU::Init(appShortName);
#endif
}

void OTRGlobals::Initialize() {
    std::string mmPath = Ship::Context::LocateFileAcrossAppDirs("mm.o2r", appShortName);
    if (std::filesystem::exists(mmPath)) {
        context->GetResourceManager()->GetArchiveManager()->AddArchive(mmPath);
    }

    std::unordered_set<uint32_t> validHashes = { MM_NTSC_US_10, MM_NTSC_US_GC };

#if (_DEBUG)
    auto defaultLogLevel = spdlog::level::debug;
#else
    auto defaultLogLevel = spdlog::level::info;
#endif
    context->InitConfiguration();
    context->InitConsoleVariables();
    auto logLevel = static_cast<spdlog::level::level_enum>(CVarGetInteger("gDeveloperTools.LogLevel", defaultLogLevel));
    context->InitLogging(logLevel, logLevel);
    Ship::Context::GetRawInstance()->GetLogger()->set_pattern("[%H:%M:%S.%e] [%s:%#] [%^%l%$] %v");

    std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow())->GetGfxDebugger();
    context->InitFileDropMgr();

    // tell LUS to reserve 3 2S2H specific threads (Game, Audio, Save)
    prevAltAssets = CVarGetInteger("gEnhancements.Mods.AlternateAssets", 0);
    context->GetResourceManager()->SetAltAssetsEnabled(prevAltAssets);

    context->InitCrashHandler();

    context->GetWindow()->SetAutoCaptureMouse(CVarGetInteger("gSettings.EnableMouse", 0) &&
                                              CVarGetInteger("gSettings.AutoCaptureMouse", 1));
    context->GetWindow()->SetForceCursorVisibility(CVarGetInteger("gSettings.CursorVisibility", 0));

    context->InitAudio({ .SampleRate = 32000, .SampleLength = 1024, .DesiredBuffered = 1680 });

    SPDLOG_INFO("Starting 2 Ship 2 Harkinian version {} (Branch: {} | Commit: {})", (char*)gBuildVersion,
                (char*)gGitBranch, (char*)gGitCommitHash);

    auto loader = context->GetResourceManager()->GetResourceLoader();
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryBinaryTextureV0>(), RESOURCE_FORMAT_BINARY,
                                    "Texture", static_cast<uint32_t>(Fast::ResourceType::Texture), 0);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryBinaryTextureV1>(), RESOURCE_FORMAT_BINARY,
                                    "Texture", static_cast<uint32_t>(Fast::ResourceType::Texture), 1);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryBinaryVertexV0>(), RESOURCE_FORMAT_BINARY,
                                    "Vertex", static_cast<uint32_t>(Fast::ResourceType::Vertex), 0);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryXMLVertexV0>(), RESOURCE_FORMAT_XML, "Vertex",
                                    static_cast<uint32_t>(Fast::ResourceType::Vertex), 0);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryBinaryDisplayListV0>(),
                                    RESOURCE_FORMAT_BINARY, "DisplayList",
                                    static_cast<uint32_t>(Fast::ResourceType::DisplayList), 0);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryXMLDisplayListV0>(), RESOURCE_FORMAT_XML,
                                    "DisplayList", static_cast<uint32_t>(Fast::ResourceType::DisplayList), 0);
    loader->RegisterResourceFactory(std::make_shared<Fast::ResourceFactoryBinaryMatrixV0>(), RESOURCE_FORMAT_BINARY,
                                    "Matrix", static_cast<uint32_t>(Fast::ResourceType::Matrix), 0);
    loader->RegisterResourceFactory(std::make_shared<Ship::ResourceFactoryBinaryBlobV0>(), RESOURCE_FORMAT_BINARY,
                                    "Blob", static_cast<uint32_t>(Ship::ResourceType::Blob), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryArrayV0>(), RESOURCE_FORMAT_BINARY,
                                    "Array", static_cast<uint32_t>(SOH::ResourceType::SOH_Array), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryAnimationV0>(), RESOURCE_FORMAT_BINARY,
                                    "Animation", static_cast<uint32_t>(SOH::ResourceType::SOH_Animation), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryPlayerAnimationV0>(),
                                    RESOURCE_FORMAT_BINARY, "PlayerAnimation",
                                    static_cast<uint32_t>(SOH::ResourceType::SOH_PlayerAnimation), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinarySceneV0>(), RESOURCE_FORMAT_BINARY,
                                    "Room", static_cast<uint32_t>(SOH::ResourceType::SOH_Room), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryCollisionHeaderV0>(),
                                    RESOURCE_FORMAT_BINARY, "CollisionHeader",
                                    static_cast<uint32_t>(SOH::ResourceType::SOH_CollisionHeader), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinarySkeletonV0>(), RESOURCE_FORMAT_BINARY,
                                    "Skeleton", static_cast<uint32_t>(SOH::ResourceType::SOH_Skeleton), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryXMLSkeletonV0>(), RESOURCE_FORMAT_XML,
                                    "Skeleton", static_cast<uint32_t>(SOH::ResourceType::SOH_Skeleton), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinarySkeletonLimbV0>(),
                                    RESOURCE_FORMAT_BINARY, "SkeletonLimb",
                                    static_cast<uint32_t>(SOH::ResourceType::SOH_SkeletonLimb), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryXMLSkeletonLimbV0>(), RESOURCE_FORMAT_XML,
                                    "SkeletonLimb", static_cast<uint32_t>(SOH::ResourceType::SOH_SkeletonLimb), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryPathMMV0>(), RESOURCE_FORMAT_BINARY,
                                    "Path", static_cast<uint32_t>(SOH::ResourceType::SOH_Path), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryCutsceneV0>(), RESOURCE_FORMAT_BINARY,
                                    "Cutscene", static_cast<uint32_t>(SOH::ResourceType::SOH_Cutscene), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryTextMMV0>(), RESOURCE_FORMAT_BINARY,
                                    "TextMM", static_cast<uint32_t>(SOH::ResourceType::TSH_TextMM), 0);

    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryAudioSampleV2>(), RESOURCE_FORMAT_BINARY,
                                    "AudioSample", static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSample), 2);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryXMLAudioSampleV0>(), RESOURCE_FORMAT_XML,
                                    "Sample", static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSample), 0);

    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryAudioSoundFontV2>(),
                                    RESOURCE_FORMAT_BINARY, "AudioSoundFont",
                                    static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSoundFont), 2);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryXMLSoundFontV0>(), RESOURCE_FORMAT_XML,
                                    "SoundFont", static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSoundFont), 0);

    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryAudioSequenceV2>(),
                                    RESOURCE_FORMAT_BINARY, "AudioSequence",
                                    static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSequence), 2);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryXMLAudioSequenceV0>(), RESOURCE_FORMAT_XML,
                                    "Sequence", static_cast<uint32_t>(SOH::ResourceType::SOH_AudioSequence), 0);

    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryBackgroundV0>(), RESOURCE_FORMAT_BINARY,
                                    "Background", static_cast<uint32_t>(SOH::ResourceType::SOH_Background), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryTextureAnimationV0>(),
                                    RESOURCE_FORMAT_BINARY, "TextureAnimation",
                                    static_cast<uint32_t>(SOH::ResourceType::TSH_TexAnim), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryKeyFrameAnim>(), RESOURCE_FORMAT_BINARY,
                                    "KeyFrameAnim", static_cast<uint32_t>(SOH::ResourceType::TSH_CKeyFrameAnim), 0);
    loader->RegisterResourceFactory(std::make_shared<SOH::ResourceFactoryBinaryKeyFrameSkel>(), RESOURCE_FORMAT_BINARY,
                                    "KeyFrameSkel", static_cast<uint32_t>(SOH::ResourceType::TSH_CKeyFrameSkel), 0);

    // gSaveStateMgr = std::make_shared<SaveStateMgr>();
    // gRandomizer = std::make_shared<Randomizer>();

    auto versions = context->GetResourceManager()->GetArchiveManager()->GetGameVersions();
    for (uint32_t version : versions) {
        if (!validHashes.contains(version)) {
#if defined(__SWITCH__)
            SPDLOG_ERROR("Invalid O2R File!");
#elif defined(__WIIU__)
            Ship::WiiU::ThrowInvalidOTR();
#else
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Invalid O2R File",
                                     "Attempted to load an invalid O2R file. Try regenerating.", nullptr);
            SPDLOG_ERROR("Invalid O2R File!");
#endif
            exit(1);
        }
    }
}

OTRGlobals::~OTRGlobals() {
}

extern "C" uint32_t Ship_GetInterpolationFPS() {
    return OTRGlobals::Instance->GetInterpolationFPS();
}

struct ExtensionEntry {
    std::string path;
    std::string ext;
};

void OTRGlobals::ScaleImGui() {
    int32_t imGuiScaleIndex = CVarGetInteger("gSettings.ImGuiScale", defaultImGuiScale);
    if (imGuiScaleIndex == previousImGuiScaleIndex) {
        return;
    }

    float scale = imguiScaleOptionToValue[imGuiScaleIndex];
    float newScale = scale / previousImGuiScale;
    ImGui::GetStyle().ScaleAllSizes(newScale);
    ImGui::GetIO().FontGlobalScale = scale;
    previousImGuiScale = scale;
    previousImGuiScaleIndex = imGuiScaleIndex;
}

ImFont* OTRGlobals::CreateDefaultFontWithSize(float size) {
    auto mImGuiIo = &ImGui::GetIO();
    ImFontConfig fontCfg = ImFontConfig();
    fontCfg.OversampleH = fontCfg.OversampleV = 1;
    fontCfg.PixelSnapH = true;
    fontCfg.SizePixels = size;
    ImFont* font = mImGuiIo->Fonts->AddFontDefault(&fontCfg);
    // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
    float iconFontSize = size * 2.0f / 3.0f;
    static const ImWchar sIconsRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.GlyphMinAdvanceX = iconFontSize;
    mImGuiIo->Fonts->AddFontFromMemoryCompressedBase85TTF(fontawesome_compressed_data_base85, iconFontSize,
                                                          &iconsConfig, sIconsRanges);
    return font;
}

uint32_t OTRGlobals::GetInterpolationFPS() {
    if (CVarGetInteger("gMatchRefreshRate", 0)) {
        return Ship::Context::GetRawInstance()->GetWindow()->GetCurrentRefreshRate();
    } else if (CVarGetInteger(CVAR_VSYNC_ENABLED, 1) ||
               !Ship::Context::GetRawInstance()->GetWindow()->CanDisableVerticalSync()) {
        return std::min<uint32_t>(Ship::Context::GetRawInstance()->GetWindow()->GetCurrentRefreshRate(),
                                  CVarGetInteger("gInterpolationFPS", 20));
    }
    return CVarGetInteger("gInterpolationFPS", 20);
}

extern "C" void OTRMessage_Init();
extern "C" void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
extern "C" int AudioPlayer_Buffered(void);
extern "C" int AudioPlayer_GetDesiredBuffered(void);
extern "C" void ResourceMgr_LoadDirectory(const char* resName);
std::unordered_map<std::string, ExtensionEntry> ExtensionCache;

static struct {
    std::thread thread;
    std::condition_variable cv_to_thread, cv_from_thread;
    std::mutex mutex;
    bool running;
    bool processing;
} audio;

void OTRAudio_Thread() {
    while (audio.running) {
        {
            std::unique_lock<std::mutex> Lock(audio.mutex);
            while (!audio.processing && audio.running) {
                audio.cv_to_thread.wait(Lock);
            }

            if (!audio.running) {
                break;
            }
        }
        std::unique_lock<std::mutex> Lock(audio.mutex);
// AudioMgr_ThreadEntry(&gAudioMgr);
//  528 and 544 relate to 60 fps at 32 kHz 32000/60 = 533.333..
//  in an ideal world, one third of the calls should use num_samples=544 and two thirds num_samples=528
#define SAMPLES_HIGH 560
#define SAMPLES_LOW 528

#define AUDIO_FRAMES_PER_UPDATE (R_UPDATE_RATE > 0 ? R_UPDATE_RATE : 1)
#define NUM_AUDIO_CHANNELS 2

        int samples_left = AudioPlayer_Buffered();
        u32 num_audio_samples = samples_left < AudioPlayer_GetDesiredBuffered() ? SAMPLES_HIGH : SAMPLES_LOW;

        // 3 is the maximum authentic frame divisor.
        s16 audio_buffer[SAMPLES_HIGH * NUM_AUDIO_CHANNELS * 3];
        for (int i = 0; i < AUDIO_FRAMES_PER_UPDATE; i++) {
            AudioMgr_CreateNextAudioBuffer(audio_buffer + i * (num_audio_samples * NUM_AUDIO_CHANNELS),
                                           num_audio_samples);
        }

        AudioPlayer_Play((u8*)audio_buffer,
                         num_audio_samples * (sizeof(int16_t) * NUM_AUDIO_CHANNELS * AUDIO_FRAMES_PER_UPDATE));

        audio.processing = false;
        audio.cv_from_thread.notify_one();
    }
}

// C->C++ Bridge
extern "C" void OTRAudio_Init() {
    // Precache all our samples, sequences, etc...
    ResourceMgr_LoadDirectory("audio");

    if (!audio.running) {
        audio.running = true;
        audio.thread = std::thread(OTRAudio_Thread);
    }
}

extern "C" char** gSequenceMap;
extern "C" size_t gSequenceMapSize;

extern "C" char** gFontMap;
extern "C" size_t gFontMapSize;

extern "C" void OTRAudio_Exit() {
    // Tell the audio thread to stop
    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        audio.running = false;
    }
    audio.cv_to_thread.notify_all();

    // Wait until the audio thread quit
    audio.thread.join();
    for (size_t i = 0; i < gSequenceMapSize; i++) {
        free(gSequenceMap[i]);
    }
    free(gSequenceMap);

    for (size_t i = 0; i < gFontMapSize; i++) {
        free(gFontMap[i]);
    }
    free(gFontMap);
    free(gAudioCtx.seqLoadStatus);
    free(gAudioCtx.fontLoadStatus);
}

extern "C" void OTRExtScanner() {
    auto lst = *Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->ListFiles("*").get();

    for (auto& rPath : lst) {
        std::vector<std::string> raw = StringHelper::Split(rPath, ".");
        std::string ext = raw[raw.size() - 1];
        std::string nPath = rPath.substr(0, rPath.size() - (ext.size() + 1));
        replace(nPath.begin(), nPath.end(), '\\', '/');

        ExtensionCache[nPath] = { rPath, ext };
    }
}

// Read the port version from an archive file
ArchiveVersion ReadPortVersionFromArchive(std::string archivePath, bool isO2rType) {
    ArchiveVersion version = {};

    // Use a temporary archive instance to load the archive appropriately and read the version file
    std::shared_ptr<Ship::Archive> archive;
    if (isO2rType) {
        archive = make_shared<Ship::O2rArchive>(archivePath);
    } else {
#ifdef INCLUDE_MPQ_SUPPORT
        archive = make_shared<Ship::OtrArchive>(archivePath);
#else
        SPDLOG_ERROR("An OTR File, {}, was found but support for them is not included. File will be ignored.",
                     archivePath.c_str());
#endif
    }
    if (archive->Open()) {
        auto t = archive->LoadFile("portVersion");
        if (t != nullptr && t->IsLoaded) {
            auto stream = std::make_shared<Ship::MemoryStream>(t->Buffer->data(), t->Buffer->size());
            auto reader = std::make_shared<Ship::BinaryReader>(stream);
            Ship::Endianness endianness = (Ship::Endianness)reader->ReadUByte();
            reader->SetEndianness(endianness);
            version.major = reader->ReadUInt16();
            version.minor = reader->ReadUInt16();
            version.patch = reader->ReadUInt16();
        }
    }

    return version;
}

// Check that a 2ship.o2r exists and matches the version of 2ship running
// Otherwise show a message and exit
// For Windows/Mac/Linux if the version doesn't match, offer to regenerate it
ArchiveVersion DetectArchiveVersion(std::string fileName, bool isO2rType) {
    bool isArchiveOld = false;
    std::string archivePath = Ship::Context::LocateFileAcrossAppDirs(fileName, appShortName);

    // Doesn't exist so nothing to do here
    if (!std::filesystem::exists(archivePath)) {
        return { INT16_MAX, INT16_MAX, INT16_MAX };
    }

    return ReadPortVersionFromArchive(archivePath, isO2rType);
}

extern "C" void Messagebox_ShowErrorBox(char* title, char* body) {
    Extractor::ShowErrorBox(title, body);
}

bool VerifyArchiveVersion(ArchiveVersion version) {
    return version.major != INT16_MAX && version.major != gBuildVersionMajor;
}

extern "C" void InitOTR(int argc, char* argv[]) {
    OTRGlobals::Instance = new OTRGlobals();
    OTRGlobals::Instance->RunExtract(argc, argv);

    OTRGlobals::Instance->Initialize();

    std::shared_ptr<Ship::Config> conf = OTRGlobals::Instance->context->GetConfig();
    conf->RegisterVersionUpdater(std::make_shared<Ben::ConfigVersion1Updater>());
    conf->RunVersionUpdates();
    Ship::Context::GetRawInstance()->GetConsoleVariables()->Save();

    GameInteractor::Instance = new GameInteractor();
    AudioCollection::Instance = new AudioCollection();
    LoadGuiTextures();
    ModMenu_LoadArchives();
    BenGui::SetupGuiElements();
    ShipInit::InitAll();
    Rando::Init();
    GfxPatcher_ApplyNecessaryAuthenticPatches();
    DebugConsole_Init();
    GameInteractor::Instance->RegisterOwnHooks();
    CustomItem::RegisterHooks();
    CustomMessage::RegisterHooks();
    Rando::StaticData::PopulateCheckNames();

    OTRMessage_Init();
    OTRAudio_Init();
    OTRExtScanner();
    PlayerCustomFlipbooks_Patch();

    // Just came up with arbitrary numbers that seemed to work, this is
    // usually set once(?) in currently stubbed out areas of code.
    gIrqMgrRetraceTime = Ship_Random(700000, 850000);

    time_t now = time(NULL);
    tm* tm_now = localtime(&now);
    if (tm_now->tm_mon == 11 && tm_now->tm_mday >= 24 && tm_now->tm_mday <= 25) {
        CVarRegisterInteger("gLetItSnow", 1);
    } else {
        CVarClear("gLetItSnow");
    }

    srand(now);
#ifdef ENABLE_CROWD_CONTROL
    CrowdControl::Instance = new CrowdControl();
    CrowdControl::Instance->Init();
    if (CVarGetInteger("gCrowdControl", 0)) {
        CrowdControl::Instance->Enable();
    } else {
        CrowdControl::Instance->Disable();
    }
#endif

    Ship::Context::GetRawInstance()->GetFileDropMgr()->RegisterDropHandler(BinarySaveConverter_HandleFileDropped);
    Ship::Context::GetRawInstance()->GetFileDropMgr()->RegisterDropHandler(SaveManager_HandleFileDropped);
}

extern "C" void SaveManager_ThreadPoolWait() {
    // SaveManager::Instance->ThreadPoolWait();
}

extern "C" void DeinitOTR() {
    SaveManager_ThreadPoolWait();
    OTRAudio_Exit();
#ifdef ENABLE_CROWD_CONTROL
    CrowdControl::Instance->Disable();
    CrowdControl::Instance->Shutdown();
#endif

    // Destroying gui here because we have shared ptrs to LUS objects which output to SPDLOG which is destroyed before
    // these shared ptrs.
    BenGui::Destroy();
    benFast3dWindow = nullptr;

    OTRGlobals::Instance->context = nullptr;
    Ship::Context::DestroyInstance();
    delete AudioCollection::Instance;
}

#ifdef _WIN32
extern "C" uint64_t GetFrequency() {
    LARGE_INTEGER nFreq;

    QueryPerformanceFrequency(&nFreq);

    return nFreq.QuadPart;
}

extern "C" uint64_t GetPerfCounter() {
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);

    return ticks.QuadPart;
}
#else
extern "C" uint64_t GetFrequency() {
    return 1000; // sec -> ms
}

extern "C" uint64_t GetPerfCounter() {
    struct timespec monotime;
    clock_gettime(CLOCK_MONOTONIC, &monotime);

    uint64_t remainingMs = (monotime.tv_nsec / 1000000);

    // in milliseconds
    return monotime.tv_sec * 1000 + remainingMs;
}
#endif

extern "C" uint64_t GetUnixTimestamp() {
    auto time = std::chrono::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
    long now = millis.count();
    return now;
}

extern "C" void Graph_StartFrame() {
#ifndef __WIIU__
    using Ship::KbScancode;
    int32_t dwScancode = OTRGlobals::Instance->context->GetWindow()->GetLastScancode();
    OTRGlobals::Instance->context->GetWindow()->SetLastScancode(-1);

    switch (dwScancode) {
#if 0
        case KbScancode::LUS_KB_F5: {
            if (CVarGetInteger("gSaveStatesEnabled", 0) == 0) {
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                    6.0f, true, "Save states not enabled. Check Cheats Menu.");
                return;
            }
            const unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
            const SaveStateReturn stateReturn =
                OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::SAVE });

            switch (stateReturn) {
                case SaveStateReturn::SUCCESS:
                    SPDLOG_INFO("[SOH] Saved state to slot {}", slot);
                    break;
                case SaveStateReturn::FAIL_WRONG_GAMESTATE:
                    SPDLOG_ERROR("[SOH] Can not save a state outside of \"GamePlay\"");
                    break;
                    [[unlikely]] default : break;
            }
            break;
        }
        case KbScancode::LUS_KB_F6: {
            if (CVarGetInteger("gSaveStatesEnabled", 0) == 0) {
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                    6.0f, true, "Save states not enabled. Check Cheats Menu.");
                return;
            }
            unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
            slot++;
            if (slot > 5) {
                slot = 0;
            }
            OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot);
            SPDLOG_INFO("Set SaveState slot to {}.", slot);
            break;
        }
        case KbScancode::LUS_KB_F7: {
            if (CVarGetInteger("gSaveStatesEnabled", 0) == 0) {
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGameOverlay()->TextDrawNotification(
                    6.0f, true, "Save states not enabled. Check Cheats Menu.");
                return;
            }
            const unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
            const SaveStateReturn stateReturn =
                OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::LOAD });

            switch (stateReturn) {
                case SaveStateReturn::SUCCESS:
                    SPDLOG_INFO("[SOH] Loaded state from slot {}", slot);
                    break;
                case SaveStateReturn::FAIL_INVALID_SLOT:
                    SPDLOG_ERROR("[SOH] Invalid State Slot Number {}", slot);
                    break;
                case SaveStateReturn::FAIL_STATE_EMPTY:
                    SPDLOG_ERROR("[SOH] State Slot {} is empty", slot);
                    break;
                case SaveStateReturn::FAIL_WRONG_GAMESTATE:
                    SPDLOG_ERROR("[SOH] Can not load a state outside of \"GamePlay\"");
                    break;
                    [[unlikely]] default : break;
            }

            break;
        }
#endif
#if defined(_WIN32) || defined(__APPLE__)
        case KbScancode::LUS_KB_F9: {
            // Toggle TTS
            CVarSetInteger("gA11yTTS", !CVarGetInteger("gA11yTTS", 0));
            break;
        }
#endif
        case KbScancode::LUS_KB_TAB: {
            // Toggle HD Assets
            if (CVarGetInteger("gEnhancements.Mods.AlternateAssetsHotkey", 1)) {
                CVarSetInteger("gEnhancements.Mods.AlternateAssets",
                               !CVarGetInteger("gEnhancements.Mods.AlternateAssets", 0));
            }
            break;
        }
    }
#endif
}

// Interpolated frames of a tick are evenly spaced numerators time+step, time+2*step, ... over denom.
void RunCommands(Gfx* Commands, int time, int step, int denom, int count) {
    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(OTRGlobals::Instance->context->GetWindow());

    if (wnd == nullptr) {
        return;
    }

    // Process window events for resize, mouse, keyboard events
    wnd->HandleEvents();

    auto intp = wnd->GetInterpreterWeak().lock().get();
    intp->mInterpolationIndex = 0;

    UIWidgets::Colors themeColor =
        static_cast<UIWidgets::Colors>(CVarGetInteger("gSettings.Menu.Theme", UIWidgets::Colors::LightBlue));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, UIWidgets::ColorValues.at(themeColor));
    for (int i = 0; i < count; i++) {
        time += step;
        std::unordered_map<Mtx*, MtxF> mtx_replacements =
            (time == denom) ? std::unordered_map<Mtx*, MtxF>() : FrameInterpolation_Interpolate((float)time / denom);
        intp->mInterpolationT = (float)time / denom;
        wnd->DrawAndRunGraphicsCommands(Commands, mtx_replacements);
        intp->mInterpolationIndex++;
    }
    ImGui::PopStyleColor();
}

// C->C++ Bridge
extern "C" void Graph_ProcessGfxCommands(Gfx* commands) {
    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        audio.processing = true;
    }

    audio.cv_to_thread.notify_one();
    int target_fps = OTRGlobals::Instance->GetInterpolationFPS();
    static int last_fps;
    static int last_update_rate;
    static int time;
    int fps = target_fps;
    int original_fps = 60 / R_UPDATE_RATE;
    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());

    if (target_fps == 20 || original_fps > target_fps) {
        fps = original_fps;
    }

    if (last_fps != fps || last_update_rate != R_UPDATE_RATE) {
        time = 0;
    }

    // time_base = fps * original_fps (one second)
    int next_original_frame = fps;

    int start_time = time;
    int count = 0;
    while (time + original_fps <= next_original_frame) {
        time += original_fps;
        count++;
    }

    time -= fps;

    if (wnd != nullptr) {
        wnd->SetTargetFps(fps);
    }

    int step = original_fps;
    // When the gfx debugger is active, only run with the final mtx
    if (GfxDebuggerIsDebugging()) {
        start_time = next_original_frame;
        step = 0;
        count = 1;
    }

    RunCommands(commands, start_time, step, next_original_frame, count);

    last_fps = fps;
    last_update_rate = R_UPDATE_RATE;

    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        while (audio.processing) {
            audio.cv_from_thread.wait(Lock);
        }
    }

    bool curAltAssets = CVarGetInteger("gEnhancements.Mods.AlternateAssets", 0);
    if (prevAltAssets != curAltAssets) {
        prevAltAssets = curAltAssets;
        Ship::Context::GetRawInstance()->GetResourceManager()->SetAltAssetsEnabled(curAltAssets);
        gfx_texture_cache_clear();
        PlayerCustomFlipbooks_Patch();
        SOH::SkeletonPatcher::UpdateSkeletons();
        // GameInteractor::Instance->ExecuteHooks<GameInteractor::OnAssetAltChange>();
    }

    // OTRTODO: FIGURE OUT END FRAME POINT
    /* if (OTRGlobals::Instance->context->GetWindow()->lastScancode != -1)
         OTRGlobals::Instance->context->GetWindow()->lastScancode = -1;*/
}

float divisor_num = 0.0f;

// Batch a coordinate to have its depth read later by OTRGetPixelDepth
extern "C" void OTRGetPixelDepthPrepare(float x, float y) {
    // Invert the Y value to match the origin values used in the renderer
    float adjustedY = SCREEN_HEIGHT - y;

    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    if (wnd == nullptr) {
        return;
    }

    wnd->GetPixelDepthPrepare(x, adjustedY);
}

extern "C" uint16_t OTRGetPixelDepth(float x, float y) {
    // Invert the Y value to match the origin values used in the renderer
    float adjustedY = SCREEN_HEIGHT - y;

    auto wnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    if (wnd == nullptr) {
        return 0;
    }

    return wnd->GetPixelDepth(x, adjustedY);
}

extern "C" bool ResourceMgr_IsAltAssetsEnabled() {
    return Ship::Context::GetRawInstance()->GetResourceManager()->IsAltAssetsEnabled();
}

extern "C" uint32_t ResourceMgr_GetNumGameVersions() {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions().size();
}

extern "C" uint32_t ResourceMgr_GetGameVersion(int index) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];
}

extern "C" uint32_t ResourceMgr_GetGamePlatform(int index) {
    uint32_t version =
        Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];

    switch (version) {
        case MM_NTSC_US_10:
            return GAME_PLATFORM_N64;
        case MM_NTSC_US_GC:
            return GAME_PLATFORM_GC;
    }
}

extern "C" uint32_t ResourceMgr_GetGameRegion(int index) {
    uint32_t version =
        Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->GetGameVersions()[index];

    switch (version) {
        case MM_NTSC_US_10:
        case MM_NTSC_US_GC:
            return GAME_REGION_NTSC;
    }
}

extern "C" void ResourceMgr_LoadDirectory(const char* resName) {
    Ship::Context::GetRawInstance()->GetResourceManager()->LoadResources(resName);
}
extern "C" void ResourceMgr_DirtyDirectory(const char* resName) {
    Ship::Context::GetRawInstance()->GetResourceManager()->DirtyResources(resName);
}

extern "C" void ResourceMgr_UnloadResource(const char* resName) {
    std::string path = resName;
    if (path.starts_with("__OTR__")) {
        path = path.substr(7);
    }
    Ship::Context::GetRawInstance()->GetResourceManager()->UnloadResource(path);
}

static void ResourceMgr_PreloadAltWhenItExists(const char* resName) {
    std::string path = resName;
    if (path.starts_with("__OTR__")) {
        path = path.substr(7);
    }

    if (ResourceMgr_IsAltAssetsEnabled() && ExtensionCache.contains(Ship::IResource::gAltAssetPrefix + path)) {
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(Ship::IResource::gAltAssetPrefix + path,
                                                                            true);
    }
}

// OTRTODO: There is probably a more elegant way to go about this...
// Kenix: This is definitely leaking memory when it's called.
extern "C" char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize) {
    auto lst = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(searchMask);
    char** result = (char**)malloc(lst->size() * sizeof(char*));

    for (size_t i = 0; i < lst->size(); i++) {
        char* str = (char*)malloc(lst.get()[0][i].size() + 1);
        memcpy(str, lst.get()[0][i].data(), lst.get()[0][i].size());
        str[lst.get()[0][i].size()] = '\0';
        result[i] = str;
    }
    *resultSize = lst->size();

    return result;
}

extern "C" uint8_t ResourceMgr_FileExists(const char* filePath) {
    std::string path = filePath;
    if (path.substr(0, 7) == "__OTR__") {
        path = path.substr(7);
    }

    return ExtensionCache.contains(path);
}

extern "C" void ResourceMgr_LoadFile(const char* resName) {
    Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(resName);
}

std::shared_ptr<Ship::IResource> GetResourceByName(const char* path) {
    return Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path);
}

extern "C" char* ResourceMgr_LoadFileFromDisk(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    fseek(file, 0, SEEK_END);
    int fSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(fSize);
    fread(data, 1, fSize, file);

    fclose(file);

    return data;
}

extern "C" uint8_t ResourceMgr_ResourceIsBackground(char* texPath) {
    auto res = GetResourceByName(texPath);
    return res->GetInitData()->Type == static_cast<uint32_t>(SOH::ResourceType::SOH_Background);
}

extern "C" char* ResourceMgr_LoadJPEG(char* data, size_t dataSize) {
    static char* finalBuffer = 0;

    if (finalBuffer == 0)
        finalBuffer = (char*)malloc(dataSize);

    int w;
    int h;
    int comp;

    unsigned char* pixels =
        stbi_load_from_memory((const unsigned char*)data, 320 * 240 * 2, &w, &h, &comp, STBI_rgb_alpha);
    // unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, 480 * 240 * 2, &w, &h, &comp,
    // STBI_rgb_alpha);
    int idx = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint16_t* bufferTest = (uint16_t*)finalBuffer;
            int pixelIdx = ((y * w) + x) * 4;

            uint8_t r = pixels[pixelIdx + 0] / 8;
            uint8_t g = pixels[pixelIdx + 1] / 8;
            uint8_t b = pixels[pixelIdx + 2] / 8;

            uint8_t alphaBit = pixels[pixelIdx + 3] != 0;

            uint16_t data = (r << 11) + (g << 6) + (b << 1) + alphaBit;

            finalBuffer[idx++] = (data & 0xFF00) >> 8;
            finalBuffer[idx++] = (data & 0x00FF);
        }
    }

    return (char*)finalBuffer;
}

extern "C" uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);

extern "C" uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);

extern "C" char* ResourceMgr_LoadTexOrDListByName(const char* filePath) {
    auto res = GetResourceByName(filePath);

    if (res->GetInitData()->Type == static_cast<uint32_t>(Fast::ResourceType::DisplayList))
        return (char*)&((std::static_pointer_cast<Fast::DisplayList>(res))->Instructions[0]);
    else if (res->GetInitData()->Type == static_cast<uint32_t>(SOH::ResourceType::SOH_Array))
        return (char*)(std::static_pointer_cast<SOH::Array>(res))->Vertices.data();
    else {
        return (char*)ResourceGetDataByName(filePath);
    }
}

extern "C" char* ResourceMgr_LoadIfDListByName(const char* filePath) {
    auto res = GetResourceByName(filePath);

    if (res->GetInitData()->Type == static_cast<uint32_t>(Fast::ResourceType::DisplayList))
        return (char*)&((std::static_pointer_cast<Fast::DisplayList>(res))->Instructions[0]);

    return nullptr;
}

// extern "C" Sprite* GetSeedTexture(uint8_t index) {
//     return OTRGlobals::Instance->gRandomizer->GetSeedTexture(index);
// }

extern "C" char* ResourceMgr_LoadPlayerAnimByName(const char* animPath) {
    auto anim = std::static_pointer_cast<SOH::PlayerAnimation>(GetResourceByName(animPath));

    return (char*)&anim->limbRotData[0];
}

extern "C" void ResourceMgr_PushCurrentDirectory(char* path) {
    Fast::gfx_push_current_dir(path);
}

extern "C" Gfx* ResourceMgr_LoadGfxByName(const char* path) {
    ResourceMgr_PreloadAltWhenItExists(path);

    auto res = std::static_pointer_cast<Fast::DisplayList>(GetResourceByName(path));
    return (Gfx*)&res->Instructions[0];
}

typedef struct {
    int index;
    Gfx instruction;
} GfxPatch;

std::unordered_map<std::string, std::unordered_map<std::string, GfxPatch>> originalGfx;

// Attention! This is primarily for cosmetics & bug fixes. For things like mods and model replacement you should be
// using OTRs instead (When that is available). Index can be found using the commented out section below.
extern "C" void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));

    // Leaving this here for people attempting to find the correct Dlist index to patch
    /*if (strcmp("__OTR__objects/object_gi_longsword/gGiBiggoronSwordDL", path) == 0) {
        for (int i = 0; i < res->instructions.size(); i++) {
            Gfx* gfx = (Gfx*)&res->instructions[i];
            // Log all commands
            // SPDLOG_INFO("index:{} command:{}", i, gfx->words.w0 >> 24);
            // Log only SetPrimColors
            if (gfx->words.w0 >> 24 == 250) {
                SPDLOG_INFO("index:{} r:{} g:{} b:{} a:{}", i, _SHIFTR(gfx->words.w1, 24, 8), _SHIFTR(gfx->words.w1, 16,
    8), _SHIFTR(gfx->words.w1, 8, 8), _SHIFTR(gfx->words.w1, 0, 8));
            }
        }
    }*/

    // Index refers to individual gfx words, which are half the size on 32-bit
    // if (sizeof(uintptr_t) < 8) {
    // index /= 2;
    // }

    // Do not patch custom assets as they most likely do not have the same instructions as authentic assets
    if (res->GetInitData()->IsCustom) {
        return;
    }

    Gfx* gfx = (Gfx*)&res->Instructions[index];

    if (!originalGfx.contains(path) || !originalGfx[path].contains(patchName)) {
        originalGfx[path][patchName] = { index, *gfx };
    }

    *gfx = instruction;
}

extern "C" void ResourceMgr_PatchGfxCopyCommandByName(const char* path, const char* patchName, int destinationIndex,
                                                      int sourceIndex) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));

    // Do not patch custom assets as they most likely do not have the same instructions as authentic assets
    if (res->GetInitData()->IsCustom) {
        return;
    }

    Gfx* destinationGfx = (Gfx*)&res->Instructions[destinationIndex];
    Gfx sourceGfx = *(Gfx*)&res->Instructions[sourceIndex];

    if (!originalGfx.contains(path) || !originalGfx[path].contains(patchName)) {
        originalGfx[path][patchName] = { destinationIndex, *destinationGfx };
    }

    *destinationGfx = sourceGfx;
}

extern "C" void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName) {
    if (originalGfx.contains(path) && originalGfx[path].contains(patchName)) {
        auto res = std::static_pointer_cast<Fast::DisplayList>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));

        if (res->GetInitData()->IsCustom) {
            return;
        }

        Gfx* gfx = (Gfx*)&res->Instructions[originalGfx[path][patchName].index];
        *gfx = originalGfx[path][patchName].instruction;

        originalGfx[path].erase(patchName);
    }
}

extern "C" size_t ResourceMgr_GetPatchCountForDL(const char* path) {
    if (originalGfx.contains(path)) {
        return originalGfx[path].size();
    }
    return 0;
}

extern "C" void ResourceMgr_ResetAllPatchesForDL(const char* path) {
    if (!originalGfx.contains(path)) {
        return;
    }

    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));

    // Iterate through all patches and restore original instructions
    auto& patches = originalGfx[path];
    for (auto it = patches.begin(); it != patches.end();) {
        Gfx* gfx = (Gfx*)&res->Instructions[it->second.index];
        *gfx = it->second.instruction;
        // erase() returns the next iterator, allowing safe iteration during removal
        it = patches.erase(it);
    }

    // Clean up empty map entry
    if (patches.empty()) {
        originalGfx.erase(path);
    }
}

extern "C" char* ResourceMgr_LoadVtxArrayByName(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    return (char*)res->Vertices.data();
}

extern "C" size_t ResourceMgr_GetVtxArraySizeByName(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    return res->Vertices.size();
}

extern "C" char* ResourceMgr_LoadArrayByName(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    return (char*)res->Scalars.data();
}

extern "C" size_t ResourceMgr_GetArraySizeByName(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    return res->Scalars.size();
}

// Loads U8 data from an Array resource into an externally managed buffer, or mallocs a new buffer
// if the passed in a nullptr. This malloced buffer must be freed by the caller.
extern "C" u8* ResourceMgr_LoadArrayByNameAsU8(const char* path, u8* buffer) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    if (buffer == nullptr) {
        buffer = (u8*)malloc(sizeof(u8) * res->Scalars.size());
    }

    for (size_t i = 0; i < res->Scalars.size(); i++) {
        buffer[i] = res->Scalars[i].u8;
    }

    return buffer;
}

// Loads Vec3s data from an Array resource.
// mallocs a new buffer that must be freed by the caller.
extern "C" char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path) {
    auto res = std::static_pointer_cast<SOH::Array>(GetResourceByName(path));

    // if (res->CachedGameAsset != nullptr)
    //     return (char*)res->CachedGameAsset;
    // else
    // {
    Vec3s* data = (Vec3s*)malloc(sizeof(Vec3s) * res->Scalars.size());

    for (size_t i = 0; i < res->Scalars.size(); i += 3) {
        data[(i / 3)].x = res->Scalars[i + 0].s16;
        data[(i / 3)].y = res->Scalars[i + 1].s16;
        data[(i / 3)].z = res->Scalars[i + 2].s16;
    }

    // res->CachedGameAsset = data;

    return (char*)data;
    // }
}

extern "C" AnimatedMaterial* ResourceMgr_LoadAnimatedMatByName(const char* path) {
    return (AnimatedMaterial*)ResourceGetDataByName(path);
}

extern "C" CollisionHeader* ResourceMgr_LoadColByName(const char* path) {
    return (CollisionHeader*)ResourceGetDataByName(path);
}

extern "C" Vtx* ResourceMgr_LoadVtxByName(char* path) {
    return (Vtx*)ResourceGetDataByName(path);
}

extern "C" Mtx* ResourceMgr_LoadMtxByName(char* path) {
    return (Mtx*)ResourceGetDataByName(path);
}

extern "C" SequenceData ResourceMgr_LoadSeqByName(const char* path) {
    SequenceData* sequence = (SequenceData*)ResourceGetDataByName(path);
    return *sequence;
}
extern "C" SequenceData* ResourceMgr_LoadSeqPtrByName(const char* path) {
    SequenceData* sequence = (SequenceData*)ResourceGetDataByName(path);
    return sequence;
}
extern "C" KeyFrameSkeleton* ResourceMgr_LoadKeyFrameSkelByName(const char* path) {
    return (KeyFrameSkeleton*)ResourceGetDataByName(path);
}

extern "C" KeyFrameAnimation* ResourceMgr_LoadKeyFrameAnimByName(const char* path) {
    return (KeyFrameAnimation*)ResourceGetDataByName(path);
}
// std::map<std::string, SoundFontSample*> cachedCustomSFs;
#if 0
extern "C" SoundFontSample* ReadCustomSample(const char* path) {
    return nullptr;
    /*
        if (!ExtensionCache.contains(path))
            return nullptr;

        ExtensionEntry entry = ExtensionCache[path];

        auto sampleRaw = Ship::Context::GetRawInstance()->GetResourceManager()->LoadFile(entry.path);
        uint32_t* strem = (uint32_t*)sampleRaw->Buffer.get();
        uint8_t* strem2 = (uint8_t*)strem;

        SoundFontSample* sampleC = new SoundFontSample;

        if (entry.ext == "wav") {
            drwav_uint32 channels;
            drwav_uint32 sampleRate;
            drwav_uint64 totalPcm;
            drmp3_int16* pcmData =
                drwav_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->BufferSize, &channels, &sampleRate,
       &totalPcm, NULL); sampleC->size = totalPcm; sampleC->sampleAddr = (uint8_t*)pcmData; sampleC->codec = CODEC_S16;

            sampleC->loop = new AdpcmLoop;
            sampleC->loop->start = 0;
            sampleC->loop->end = sampleC->size - 1;
            sampleC->loop->count = 0;
            sampleC->sampleRateMagicValue = 'RIFF';
            sampleC->sampleRate = sampleRate;

            cachedCustomSFs[path] = sampleC;
            return sampleC;
        } else if (entry.ext == "mp3") {
            drmp3_config mp3Info;
            drmp3_uint64 totalPcm;
            drmp3_int16* pcmData =
                drmp3_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->BufferSize, &mp3Info, &totalPcm, NULL);

            sampleC->size = totalPcm * mp3Info.channels * sizeof(short);
            sampleC->sampleAddr = (uint8_t*)pcmData;
            sampleC->codec = CODEC_S16;

            sampleC->loop = new AdpcmLoop;
            sampleC->loop->start = 0;
            sampleC->loop->end = sampleC->size;
            sampleC->loop->count = 0;
            sampleC->sampleRateMagicValue = 'RIFF';
            sampleC->sampleRate = mp3Info.sampleRate;

            cachedCustomSFs[path] = sampleC;
            return sampleC;
        }

        return nullptr;
    */
}

extern "C" SoundFontSample* ResourceMgr_LoadAudioSample(const char* path) {
    return (SoundFontSample*)ResourceGetDataByName(path);
}
#endif

extern "C" SoundFont* ResourceMgr_LoadAudioSoundFontByName(const char* path) {
    return (SoundFont*)ResourceGetDataByName(path);
}

extern "C" SoundFont* ResourceMgr_LoadAudioSoundFontByCRC(uint64_t crc) {
    return (SoundFont*)ResourceGetDataByCrc(crc);
}

extern "C" int ResourceMgr_OTRSigCheck(char* imgData) {
    uintptr_t i = (uintptr_t)(imgData);

    // if (i == 0xD9000000 || i == 0xE7000000 || (i & 1) == 1)
    if ((i & 1) == 1)
        return 0;

    // if ((i & 0xFF000000) != 0xAB000000 && (i & 0xFF000000) != 0xCD000000 && i != 0) {
    if (i != 0) {
        if (imgData[0] == '_' && imgData[1] == '_' && imgData[2] == 'O' && imgData[3] == 'T' && imgData[4] == 'R' &&
            imgData[5] == '_' && imgData[6] == '_')
            return 1;
    }

    return 0;
}

// Load animation with explicit alt asset path checking.
// When Alt Assets is OFF: use original path directly (O2R or vanilla)
// When Alt Assets is ON: try alt/ prefix first, fall back to regular path if not found or invalid
extern "C" AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path) {
    bool isAlt = ResourceMgr_IsAltAssetsEnabled();

    if (isAlt) {
        std::string pathStr = std::string(path);
        static const std::string sOtr = "__OTR__";

        if (pathStr.starts_with(sOtr)) {
            pathStr = pathStr.substr(sOtr.length());
        }

        // Try alt/ first
        pathStr = Ship::IResource::gAltAssetPrefix + pathStr;
        AnimationHeaderCommon* animHeader = (AnimationHeaderCommon*)ResourceGetDataByName(pathStr.c_str());

        // If alt loaded successfully, verify it has valid data
        if (animHeader != NULL) {
            // Check for valid frame count (> 0)
            if (animHeader->frameCount > 0) {
                // For Normal animations: check frameData (comes after frameCount in AnimationHeader)
                // For Link animations: check segment (comes after frameCount in LinkAnimationHeader)
                // We check both to be safe - if either is valid, the animation is usable
                AnimationHeader* normalAnim = (AnimationHeader*)animHeader;
                PlayerAnimationHeader* playerAnim = (PlayerAnimationHeader*)animHeader;

                // Valid if Normal animation has frameData OR Link animation has segment
                if (normalAnim->frameData != NULL || playerAnim->segmentVoid != NULL) {
                    return animHeader;
                }
            }
            // Alt loaded but is invalid (broken), fall through to original path
        }

        // Fall back to original path
        return (AnimationHeaderCommon*)ResourceGetDataByName(path);
    }

    // Alt OFF: use original path directly
    return (AnimationHeaderCommon*)ResourceGetDataByName(path);
}

extern "C" SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime) {
    std::string pathStr = std::string(path);
    static const std::string sOtr = "__OTR__";

    if (pathStr.starts_with(sOtr)) {
        pathStr = pathStr.substr(sOtr.length());
    }

    bool isAlt = ResourceMgr_IsAltAssetsEnabled();

    if (isAlt) {
        pathStr = Ship::IResource::gAltAssetPrefix + pathStr;
    }

    SkeletonHeader* skelHeader = (SkeletonHeader*)ResourceGetDataByName(pathStr.c_str());

    // If there isn't an alternate model, load the regular one
    if (isAlt && skelHeader == NULL) {
        skelHeader = (SkeletonHeader*)ResourceGetDataByName(path);
    }

    // This function is only called when a skeleton is initialized.
    // Therefore we can take this opportunity to take note of the Skeleton that is created...
    if (skelAnime != nullptr) {
        auto stringPath = std::string(path);
        SOH::SkeletonPatcher::RegisterSkeleton(stringPath, skelAnime);
    }

    return skelHeader;
}

extern "C" void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime) {
    if (skelAnime != nullptr)
        SOH::SkeletonPatcher::UnregisterSkeleton(skelAnime);
}

extern "C" void ResourceMgr_ClearSkeletons() {
    SOH::SkeletonPatcher::ClearSkeletons();
}

extern "C" s32* ResourceMgr_LoadCSByName(const char* path) {
    return (s32*)ResourceGetDataByName(path);
}

ImFont* OTRGlobals::CreateFontWithSize(float size, std::string fontPath) {
    auto mImGuiIo = &ImGui::GetIO();
    ImFont* font;
    if (fontPath == "") {
        ImFontConfig fontCfg = ImFontConfig();
        fontCfg.OversampleH = fontCfg.OversampleV = 1;
        fontCfg.PixelSnapH = true;
        fontCfg.SizePixels = size;
        font = mImGuiIo->Fonts->AddFontDefault(&fontCfg);
    } else {
        auto initData = std::make_shared<Ship::ResourceInitData>();
        initData->Format = RESOURCE_FORMAT_BINARY;
        initData->Type = static_cast<uint32_t>(RESOURCE_TYPE_FONT);
        initData->ResourceVersion = 0;
        initData->Path = fontPath;
        std::shared_ptr<Ship::Font> fontData = std::static_pointer_cast<Ship::Font>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(fontPath, false, initData));
        ImFontConfig fontConf;
        fontConf.FontDataOwnedByAtlas = false;
        font = mImGuiIo->Fonts->AddFontFromMemoryTTF(fontData->Data, fontData->DataSize, size, &fontConf, nullptr);
    }
    // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
    float iconFontSize = size * 2.0f / 3.0f;
    static const ImWchar sIconsRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.GlyphMinAdvanceX = iconFontSize;
    mImGuiIo->Fonts->AddFontFromMemoryCompressedBase85TTF(fontawesome_compressed_data_base85, iconFontSize,
                                                          &iconsConfig, sIconsRanges);
    return font;
}

std::filesystem::path GetSaveFile(std::shared_ptr<Ship::Config> Conf) {
    const std::string fileName =
        Conf->GetString("Game.SaveName", Ship::Context::GetPathRelativeToAppDirectory("oot_save.sav"));
    std::filesystem::path saveFile = std::filesystem::absolute(fileName);

    if (!exists(saveFile.parent_path())) {
        create_directories(saveFile.parent_path());
    }

    return saveFile;
}

std::filesystem::path GetSaveFile() {
    const std::shared_ptr<Ship::Config> pConf = OTRGlobals::Instance->context->GetConfig();

    return GetSaveFile(pConf);
}

void OTRGlobals::CheckSaveFile(size_t sramSize) const {
    const std::shared_ptr<Ship::Config> pConf = Instance->context->GetConfig();

    std::filesystem::path savePath = GetSaveFile(pConf);
    std::fstream saveFile(savePath, std::fstream::in | std::fstream::out | std::fstream::binary);
    if (saveFile.fail()) {
        saveFile.open(savePath, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::app);
        for (int i = 0; i < sramSize; ++i) {
            saveFile.write("\0", 1);
        }
    }
    saveFile.close();
}

// extern "C" void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size) {
//     SaveManager::ReadSaveFile(GetSaveFile(), addr, dramAddr, size);
// }

// extern "C" void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size) {
//     SaveManager::WriteSaveFile(GetSaveFile(), addr, dramAddr, size);
// }

std::wstring StringToU16(const std::string& s) {
    std::vector<unsigned long> result;
    size_t i = 0;
    while (i < s.size()) {
        unsigned long uni;
        size_t nbytes;
        bool error = false;
        unsigned char c = s[i++];
        if (c < 0x80) { // ascii
            uni = c;
            nbytes = 0;
        } else if (c <= 0xBF) { // assuming kata/hiragana delimiter
            nbytes = 0;
            uni = '\1';
        } else if (c <= 0xDF) {
            uni = c & 0x1F;
            nbytes = 1;
        } else if (c <= 0xEF) {
            uni = c & 0x0F;
            nbytes = 2;
        } else if (c <= 0xF7) {
            uni = c & 0x07;
            nbytes = 3;
        }
        for (size_t j = 0; j < nbytes; ++j) {
            unsigned char c = s[i++];
            uni <<= 6;
            uni += c & 0x3F;
        }
        if (uni != '\1')
            result.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < result.size(); ++i) {
        unsigned long uni = result[i];
        if (uni <= 0xFFFF) {
            utf16 += (wchar_t)uni;
        } else {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

int CopyStringToCharBuffer(const std::string& inputStr, char* buffer, const int maxBufferSize) {
    if (!inputStr.empty()) {
        // Prevent potential horrible overflow due to implicit conversion of maxBufferSize to an unsigned. Prevents
        // negatives.
        memset(buffer, 0, std::max<int>(0, maxBufferSize));
        // Gaurentee that this value will be greater than 0, regardless of passed variables.
        const int copiedCharLen = std::min<int>(std::max<int>(0, maxBufferSize - 1), inputStr.length());
        memcpy(buffer, inputStr.c_str(), copiedCharLen);
        return copiedCharLen;
    }

    return 0;
}

extern "C" void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char)) {
    const std::vector<uint32_t> hira1 = {
        u'を', u'ぁ', u'ぃ', u'ぅ', u'ぇ', u'ぉ', u'ゃ', u'ゅ', u'ょ', u'っ', u'-',  u'あ', u'い',
        u'う', u'え', u'お', u'か', u'き', u'く', u'け', u'こ', u'さ', u'し', u'す', u'せ', u'そ',
    };

    const std::vector<uint32_t> hira2 = {
        u'た', u'ち', u'つ', u'て', u'と', u'な', u'に', u'ぬ', u'ね', u'の', u'は', u'ひ', u'ふ', u'へ', u'ほ', u'ま',
        u'み', u'む', u'め', u'も', u'や', u'ゆ', u'よ', u'ら', u'り', u'る', u'れ', u'ろ', u'わ', u'ん', u'゛', u'゜',
    };

    std::wstring wstr = StringToU16(str);

    for (const auto& c : wstr) {
        if (c < 0x80) {
            printImpl(printer, c);
        } else if (c >= u'｡' && c <= u'ﾟ') { // katakana
            printImpl(printer, c - 0xFEC0);
        } else {
            auto it = std::find(hira1.begin(), hira1.end(), c);
            if (it != hira1.end()) { // hiragana block 1
                printImpl(printer, 0x88 + std::distance(hira1.begin(), it));
            }

            auto it2 = std::find(hira2.begin(), hira2.end(), c);
            if (it2 != hira2.end()) { // hiragana block 2
                printImpl(printer, 0xe0 + std::distance(hira2.begin(), it2));
            }
        }
    }
}

// Gets the width of the main ImGui window
extern "C" uint32_t OTRGetCurrentWidth() {
    return OTRGlobals::Instance->context->GetWindow()->GetWidth();
}

// Gets the height of the main ImGui window
extern "C" uint32_t OTRGetCurrentHeight() {
    return OTRGlobals::Instance->context->GetWindow()->GetHeight();
}

Color_RGB8 GetColorForControllerLED() {
#if 0
    auto brightness = CVarGetFloat("gLedBrightness", 1.0f) / 1.0f;
    Color_RGB8 color = { 0, 0, 0 };
    if (brightness > 0.0f) {
        LEDColorSource source =
            static_cast<LEDColorSource>(CVarGetInteger("gLedColorSource", LED_SOURCE_TUNIC_ORIGINAL));
        bool criticalOverride = CVarGetInteger("gLedCriticalOverride", 1);
        if (gPlayState && (source == LED_SOURCE_TUNIC_ORIGINAL || source == LED_SOURCE_TUNIC_COSMETICS)) {
            switch (CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1) {
                case PLAYER_TUNIC_KOKIRI:
                    color = source == LED_SOURCE_TUNIC_COSMETICS
                                ? CVarGetColor24("gCosmetics.Link_KokiriTunic.Value", kokiriColor)
                                : kokiriColor;
                    break;
                case PLAYER_TUNIC_GORON:
                    color = source == LED_SOURCE_TUNIC_COSMETICS
                                ? CVarGetColor24("gCosmetics.Link_GoronTunic.Value", goronColor)
                                : goronColor;
                    break;
                case PLAYER_TUNIC_ZORA:
                    color = source == LED_SOURCE_TUNIC_COSMETICS
                                ? CVarGetColor24("gCosmetics.Link_ZoraTunic.Value", zoraColor)
                                : zoraColor;
                    break;
            }
        }
        if (source == LED_SOURCE_CUSTOM) {
            color = CVarGetColor24("gLedPort1Color", { 255, 255, 255 });
        }
        if (criticalOverride || source == LED_SOURCE_HEALTH) {
            if (HealthMeter_IsCritical()) {
                color = { 0xFF, 0, 0 };
            } else if (source == LED_SOURCE_HEALTH) {
                if (gSaveContext.health / gSaveContext.healthCapacity <= 0.4f) {
                    color = { 0xFF, 0xFF, 0 };
                } else {
                    color = { 0, 0xFF, 0 };
                }
            }
        }
        color.r = color.r * brightness;
        color.g = color.g * brightness;
        color.b = color.b * brightness;
    }
#endif
    return { 0, 0, 0 };
}

extern "C" void OTRControllerCallback(uint8_t rumble) {
    // We call this every tick, SDL accounts for this use and prevents driver spam
    // https://github.com/libsdl-org/SDL/blob/f17058b562c8a1090c0c996b42982721ace90903/src/joystick/SDL_joystick.c#L1114-L1144
    Ship::Context::GetRawInstance()->GetControlDeck()->GetControllerByPort(0)->GetLED()->SetLEDColor(
        GetColorForControllerLED());

    static std::shared_ptr<BenInputEditorWindow> controllerConfigWindow = nullptr;
    if (controllerConfigWindow == nullptr) {
        controllerConfigWindow = std::dynamic_pointer_cast<BenInputEditorWindow>(
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("2S2H Input Editor"));
        // note: the current implementation may not be desired in LUS, as "true" rumble support
        //    using osMotor calls is planned: https://github.com/Kenix3/libultraship/issues/9
    }
    if (controllerConfigWindow->TestingRumble()) {
        return;
    }

    // TODO: other ports?
    if (rumble) {
        Ship::Context::GetRawInstance()->GetControlDeck()->GetControllerByPort(0)->GetRumble()->StartRumble();
    } else {
        Ship::Context::GetRawInstance()->GetControlDeck()->GetControllerByPort(0)->GetRumble()->StopRumble();
    }
}

extern "C" float OTRGetAspectRatio() {
    return Ship::Context::GetRawInstance()->GetWindow()->GetAspectRatio();
}

extern "C" float OTRGetDimensionFromLeftEdge(float v) {
    auto fastWnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    auto intP = fastWnd->GetInterpreterWeak().lock();

    if (!intP) {
        assert(false && "Lost reference to Fast::Interpreter");
        return v;
    }

    auto gfx_native_dimensions = intP->mNativeDimensions;

    return (gfx_native_dimensions.width / 2 - gfx_native_dimensions.height / 2 * OTRGetAspectRatio() + (v));
}

extern "C" float OTRGetDimensionFromRightEdge(float v) {
    auto fastWnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    auto intP = fastWnd->GetInterpreterWeak().lock();

    if (!intP) {
        assert(false && "Lost reference to Fast::Interpreter");
        return v;
    }

    auto gfx_native_dimensions = intP->mNativeDimensions;

    return (gfx_native_dimensions.width / 2 + gfx_native_dimensions.height / 2 * OTRGetAspectRatio() -
            (gfx_native_dimensions.width - v));
}

// Gets the width of the current render target area
extern "C" uint32_t OTRGetGameRenderWidth() {
    auto fastWnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    auto intP = fastWnd->GetInterpreterWeak().lock();

    if (!intP) {
        assert(false && "Lost reference to Fast::Interpreter");
        return 320;
    }

    uint32_t height, width;
    intP->GetCurDimensions(&width, &height);

    return width;
}

// Gets the height of the current render target area
extern "C" uint32_t OTRGetGameRenderHeight() {
    auto fastWnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    auto intP = fastWnd->GetInterpreterWeak().lock();

    if (!intP) {
        assert(false && "Lost reference to Fast::Interpreter");
        return 240;
    }

    uint32_t height, width;
    intP->GetCurDimensions(&width, &height);

    return height;
}

f32 floorf(f32 x);
f32 ceilf(f32 x);

extern "C" int16_t OTRGetRectDimensionFromLeftEdge(float v) {
    return ((int)floorf(OTRGetDimensionFromLeftEdge(v)));
}

extern "C" int16_t OTRGetRectDimensionFromRightEdge(float v) {
    return ((int)ceilf(OTRGetDimensionFromRightEdge(v)));
}

// Takes a HUD coordinate(320x240) and converts it to the game window pixel coordinates (any size, any aspect ratio)
// Though the HUD uses a 320x240 coordinates system, the size of the HUD box is scaled up to match the window height
// If the game window is 4:3, this will return the same value.

/*
Example, if the game window is 16:9 at twice the resolution of the HUD:
Calling with X (0,0) will return 8
Calling with Y (1,1) will return 10

. . . x _ _ _ _ _ _ _ . . .
. . . _ y _ _ _ _ _ _ . . .
. . . _ _ _ HUD _ _ _ . . .
. . . _ _ _ _ _ _ _ _ . . .
. . . _ _ _ _ _ _ _ _ . . .
*/
extern "C" int32_t OTRConvertHUDXToScreenX(int32_t v) {
    auto fastWnd = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow());
    auto intP = fastWnd->GetInterpreterWeak().lock();

    if (!intP) {
        assert(false && "Lost reference to Fast::Interpreter");
        return v;
    }

    uint32_t gameHeight, gameWidth;
    float gameAspectRatio = fastWnd->GetAspectRatio();
    intP->GetCurDimensions(&gameWidth, &gameHeight);
    float hudAspectRatio = 4.0f / 3.0f;
    int32_t hudHeight = gameHeight;
    int32_t hudWidth = hudHeight * hudAspectRatio;

    float hudScreenRatio = (hudWidth / 320.0f);
    float hudCoord = v * hudScreenRatio;
    float gameOffset = (int32_t(gameWidth) - hudWidth) / 2;
    float gameCoord = hudCoord + gameOffset;
    float gameScreenRatio = (320.0f / gameWidth);
    float screenScaledCoord = gameCoord * gameScreenRatio;
    int32_t screenScaledCoordInt = screenScaledCoord;

    return screenScaledCoordInt;
}

extern "C" void Gfx_RegisterBlendedTexture(const char* name, u8* mask, u8* replacement) {
    if (auto intP = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow())
                        ->GetInterpreterWeak()
                        .lock()) {
        intP->RegisterBlendedTexture(name, mask, replacement);
    } else {
        assert(false && "Lost reference to Fast::Interpreter");
    }
}

extern "C" void Gfx_UnregisterBlendedTexture(const char* name) {
    if (auto intP = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow())
                        ->GetInterpreterWeak()
                        .lock()) {
        intP->UnregisterBlendedTexture(name);
    } else {
        assert(false && "Lost reference to Fast::Interpreter");
    }
}

extern "C" void Gfx_TextureCacheDelete(const uint8_t* texAddr) {
    char* imgName = (char*)texAddr;

    if (texAddr == nullptr) {
        return;
    }

    if (ResourceMgr_OTRSigCheck(imgName)) {
        texAddr = (const uint8_t*)ResourceGetDataByName(imgName);
    }

    if (auto intP = std::dynamic_pointer_cast<Fast::Fast3dWindow>(Ship::Context::GetRawInstance()->GetWindow())
                        ->GetInterpreterWeak()
                        .lock()) {
        intP->TextureCacheDelete(texAddr);
    } else {
        assert(false && "Lost reference to Fast::Interpreter");
    }
}

extern "C" int AudioPlayer_Buffered(void) {
    return AudioPlayerBuffered();
}

extern "C" int AudioPlayer_GetDesiredBuffered(void) {
    return AudioPlayerGetDesiredBuffered();
}

extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len) {
    AudioPlayerPlayFrame(buf, len);
}

extern "C" int Controller_ShouldRumble(size_t slot) {
    // don't rumble if we don't have rumble mappings
    if (Ship::Context::GetRawInstance()
            ->GetControlDeck()
            ->GetControllerByPort(static_cast<uint8_t>(slot))
            ->GetRumble()
            ->GetAllRumbleMappings()
            .empty()) {
        return 0;
    }

    // don't rumble if we don't have connected gamepads
    if (Ship::Context::GetRawInstance()
            ->GetControlDeck()
            ->GetConnectedPhysicalDeviceManager()
            ->GetConnectedSDLGamepadsForPort(slot)
            .empty()) {
        return 0;
    }

    // rumble
    return 1;
}

// Helper to redirect the user to the boot screen in place of known console crash scenarios, and emits a notification
extern "C" bool Ship_HandleConsoleCrashAsReset() {
    // If fix crashes is on, return false and let fallback handling process in source
    if (CVarGetInteger("gEnhancements.Fixes.ConsoleCrashes", 1)) {
        return false;
    }

    std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
        ->Dispatch("reset");

    Notification::Emit({
        .itemIcon = "__OTR__icon_item_24_static_yar/gQuestIconGoldSkulltulaTex",
        .message = "Crash prevented!",
        .remainingTime = 10.0f,
    });

    return true;
}

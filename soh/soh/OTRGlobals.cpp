#include "OTRGlobals.h"
#include "OTRAudio.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <ResourceManager.h>
#include <File.h>
#include <DisplayList.h>
#include <Window.h>
#include <GameVersions.h>

#include "z64animation.h"
#include "z64bgcheck.h"
#include "Enhancements/gameconsole.h"
#include <libultraship/libultra/gbi.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif
#include <Array.h>
#include <stb/stb_image.h>
#define DRMP3_IMPLEMENTATION
#include <dr_libs/mp3.h>
#define DRWAV_IMPLEMENTATION
#include <dr_libs/wav.h>
#include <AudioPlayer.h>
#include "Enhancements/speechsynthesizer/SpeechSynthesizer.h"
#include "Enhancements/controls/GameControlEditor.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/audio/AudioCollection.h"
#include "Enhancements/audio/AudioEditor.h"
#include "Enhancements/debugconsole.h"
#include "Enhancements/debugger/debugger.h"
#include "Enhancements/randomizer/randomizer.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/3drando/random.hpp"
#include "Enhancements/gameplaystats.h"
#include "Enhancements/n64_weird_frame_data.inc"
#include "frame_interpolation.h"
#include "variables.h"
#include "z64.h"
#include "macros.h"
#include <Utils/StringHelper.h>
#include <Hooks.h>
#include "Enhancements/custom-message/CustomMessageManager.h"

#if not defined (__SWITCH__) && not defined(__WIIU__)
#include "Extractor/Extract.h"
#endif

#include <Fast3D/gfx_pc.h>
#include <Fast3D/gfx_rendering_api.h>

#ifdef __APPLE__
#include <SDL_scancode.h>
#else
#include <SDL2/SDL_scancode.h>
#endif

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#elif defined(__WIIU__)
#include <port/wiiu/WiiUImpl.h>
#endif


#include "Enhancements/custom-message/CustomMessageTypes.h"
#include <functions.h>
#include "Enhancements/item-tables/ItemTableManager.h"
#include "GameMenuBar.hpp"

#ifdef ENABLE_CROWD_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
CrowdControl* CrowdControl::Instance;
#endif

#include "Enhancements/mods.h"
#include "Enhancements/game-interactor/GameInteractor.h"
#include <libultraship/libultraship.h>

// Resource Types/Factories
#include "soh/resource/type/Animation.h"
#include "soh/resource/type/AudioSample.h"
#include "soh/resource/type/AudioSequence.h"
#include "soh/resource/type/AudioSoundFont.h"
#include "soh/resource/type/CollisionHeader.h"
#include "soh/resource/type/Cutscene.h"
#include "soh/resource/type/Path.h"
#include "soh/resource/type/PlayerAnimation.h"
#include "soh/resource/type/Scene.h"
#include "soh/resource/type/Skeleton.h"
#include "soh/resource/type/SkeletonLimb.h"
#include "soh/resource/type/Text.h"
#include "soh/resource/importer/AnimationFactory.h"
#include "soh/resource/importer/AudioSampleFactory.h"
#include "soh/resource/importer/AudioSequenceFactory.h"
#include "soh/resource/importer/AudioSoundFontFactory.h"
#include "soh/resource/importer/CollisionHeaderFactory.h"
#include "soh/resource/importer/CutsceneFactory.h"
#include "soh/resource/importer/PathFactory.h"
#include "soh/resource/importer/PlayerAnimationFactory.h"
#include "soh/resource/importer/SceneFactory.h"
#include "soh/resource/importer/SkeletonFactory.h"
#include "soh/resource/importer/SkeletonLimbFactory.h"
#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/importer/BackgroundFactory.h"

OTRGlobals* OTRGlobals::Instance;
SaveManager* SaveManager::Instance;
CustomMessageManager* CustomMessageManager::Instance;
ItemTableManager* ItemTableManager::Instance;
GameInteractor* GameInteractor::Instance;
AudioCollection* AudioCollection::Instance;
SpeechSynthesizer* SpeechSynthesizer::Instance;

extern "C" char** cameraStrings;
std::vector<std::shared_ptr<std::string>> cameraStdStrings;

// OTRTODO: A lot of these left in Japanese are used by the mempak manager. LUS does not currently support mempaks. Ignore unused ones.
const char* constCameraStrings[] = {
    "INSUFFICIENT",
    "KEYFRAMES",
    "YOU CAN ADD MORE",
    "FINISHED",
    "PLAYING",
    "DEMO CAMERA TOOL",
    "CANNOT PLAY",
    "KEYFRAME   ",
    "PNT   /      ",
    ">            >",
    "<            <",
    "<          >",
    GFXP_KATAKANA "*ﾌﾟﾚｲﾔ-*",
    "E MODE FIX",
    "E MODE ABS",
    GFXP_HIRAGANA "ｶﾞﾒﾝ" GFXP_KATAKANA "   ﾃﾞﾓ", // OTRTODO: Unused, get a translation! Number 15
    GFXP_HIRAGANA "ｶﾞﾒﾝ   ﾌﾂｳ", // OTRTODO: Unused, get a translation! Number 16
    "P TIME  MAX",
    GFXP_KATAKANA "ﾘﾝｸ" GFXP_HIRAGANA "    ｷｵｸ", // OTRTODO: Unused, get a translation! Number 18
    GFXP_KATAKANA "ﾘﾝｸ" GFXP_HIRAGANA "     ﾑｼ", // OTRTODO: Unused, get a translation! Number 19
    "*VIEWPT*",
    "*CAMPOS*",
    "DEBUG CAMERA",
    "CENTER/LOCK",
    "CENTER/FREE",
    "DEMO CONTROL",
    GFXP_KATAKANA "ﾒﾓﾘ" GFXP_HIRAGANA "ｶﾞﾀﾘﾏｾﾝ",
    "p",
    "e",
    "s",
    "l",
    "c",
    GFXP_KATAKANA "ﾒﾓﾘﾊﾟｯｸ",
    GFXP_KATAKANA "ｾｰﾌﾞ",
    GFXP_KATAKANA "ﾛｰﾄﾞ",
    GFXP_KATAKANA "ｸﾘｱ-",
    GFXP_HIRAGANA "ｦﾇｶﾅｲﾃﾞﾈ",
    "FREE      BYTE",
    "NEED      BYTE",
    GFXP_KATAKANA "*ﾒﾓﾘ-ﾊﾟｯｸ*",
    GFXP_HIRAGANA "ｦﾐﾂｹﾗﾚﾏｾﾝ",
    GFXP_KATAKANA "ﾌｧｲﾙ " GFXP_HIRAGANA "ｦ",
    GFXP_HIRAGANA "ｼﾃﾓｲｲﾃﾞｽｶ?",
    GFXP_HIRAGANA "ｹﾞﾝｻﾞｲﾍﾝｼｭｳﾁｭｳﾉ", // OTRTODO: Unused, get a translation! Number 43
    GFXP_KATAKANA "ﾌｧｲﾙ" GFXP_HIRAGANA "ﾊﾊｷｻﾚﾏｽ", // OTRTODO: Unused, get a translation! Number 44
    GFXP_HIRAGANA "ﾊｲ",
    GFXP_HIRAGANA "ｲｲｴ",
    GFXP_HIRAGANA "ｼﾃｲﾏｽ",
    GFXP_HIRAGANA "ｳﾜｶﾞｷ", // OTRTODO: Unused, get a translation! Number 48
    GFXP_HIRAGANA "ｼﾏｼﾀ",
    "USE       BYTE",
    GFXP_HIRAGANA "ﾆｼｯﾊﾟｲ",
    "E MODE REL",
    "FRAME       ",
    "KEY   /       ",
    "(CENTER)",
    "(ORIG)",
    "(PLAYER)",
    "(ALIGN)",
    "(SET)",
    "(OBJECT)",
    GFXP_KATAKANA "ﾎﾟｲﾝﾄNo.     ", // OTRTODO: Unused, need translation. Number 62
    "FOV              ",
    "N FRAME          ",
    "Z ROT            ",
    GFXP_KATAKANA  "ﾓ-ﾄﾞ        ", // OTRTODO: Unused, need translation. Number 65
    "  R FOCUS   ",
    "PMAX              ",
    "DEPTH             ",
    "XROT              ",
    "YROT              ",
    GFXP_KATAKANA "ﾌﾚ-ﾑ         ",
    GFXP_KATAKANA "ﾄ-ﾀﾙ         ",
    GFXP_KATAKANA "ｷ-     /   ",
};

OTRGlobals::OTRGlobals() {
    std::vector<std::string> OTRFiles;
    std::string mqPath = Ship::Window::GetPathRelativeToAppDirectory("oot-mq.otr");
    if (std::filesystem::exists(mqPath)) { 
        OTRFiles.push_back(mqPath);
    } 
    std::string ootPath = Ship::Window::GetPathRelativeToAppDirectory("oot.otr");
    if (std::filesystem::exists(ootPath)) {
        OTRFiles.push_back(ootPath);
    }
    std::string sohOtrPath = Ship::Window::GetPathRelativeToAppBundle("soh.otr");
    if (std::filesystem::exists(sohOtrPath)) {
        OTRFiles.push_back(sohOtrPath);
    }
    std::string patchesPath = Ship::Window::GetPathRelativeToAppDirectory("mods");
    if (patchesPath.length() > 0 && std::filesystem::exists(patchesPath)) {
        if (std::filesystem::is_directory(patchesPath)) {
            for (const auto& p : std::filesystem::recursive_directory_iterator(patchesPath)) {
                if (StringHelper::IEquals(p.path().extension().string(), ".otr")) {
                    OTRFiles.push_back(p.path().generic_string());
                }
            }
        }
    }
    std::unordered_set<uint32_t> ValidHashes = { 
        OOT_PAL_MQ,
        OOT_NTSC_JP_MQ,
        OOT_NTSC_US_MQ,
        OOT_PAL_GC_MQ_DBG,
        OOT_NTSC_US_10,
        OOT_NTSC_US_11,
        OOT_NTSC_US_12,
        OOT_PAL_10,
        OOT_PAL_11,
        OOT_NTSC_JP_GC_CE,
        OOT_NTSC_JP_GC,
        OOT_NTSC_US_GC,
        OOT_PAL_GC,
        OOT_PAL_GC_DBG1,
        OOT_PAL_GC_DBG2
    };
    context = Ship::Window::CreateInstance("Ship of Harkinian", "soh", OTRFiles);

    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Animation, "Animation", std::make_shared<Ship::AnimationFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_PlayerAnimation, "PlayerAnimation", std::make_shared<Ship::PlayerAnimationFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Room, "Room", std::make_shared<Ship::SceneFactory>()); // Is room scene? maybe?
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_CollisionHeader, "CollisionHeader", std::make_shared<Ship::CollisionHeaderFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Skeleton, "Skeleton", std::make_shared<Ship::SkeletonFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_SkeletonLimb, "SkeletonLimb", std::make_shared<Ship::SkeletonLimbFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Path, "Path", std::make_shared<Ship::PathFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Cutscene, "Cutscene", std::make_shared<Ship::CutsceneFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Text, "Text", std::make_shared<Ship::TextFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_AudioSample, "AudioSample", std::make_shared<Ship::AudioSampleFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_AudioSoundFont, "AudioSoundFont", std::make_shared<Ship::AudioSoundFontFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_AudioSequence, "AudioSequence", std::make_shared<Ship::AudioSequenceFactory>());
    context->GetResourceManager()->GetResourceLoader()->RegisterResourceFactory(Ship::ResourceType::SOH_Background, "Background", std::make_shared<Ship::BackgroundFactory>());

    gSaveStateMgr = std::make_shared<SaveStateMgr>();
    gRandomizer = std::make_shared<Randomizer>();

    hasMasterQuest = hasOriginal = false;

    // Move the camera strings from read only memory onto the heap (writable memory)
    // This is in OTRGlobals right now because this is a place that will only ever be run once at the beginning of startup.
    // We should probably find some code in db_camera that does initialization and only run once, and then dealloc on deinitialization.
    cameraStrings = (char**)malloc(sizeof(constCameraStrings));
    for (int32_t i = 0; i < sizeof(constCameraStrings) / sizeof(char*); i++) {
        // OTRTODO: never deallocated...
        auto dup = strdup(constCameraStrings[i]);
        cameraStrings[i] = dup;
    }

    auto versions = context->GetResourceManager()->GetArchive()->GetGameVersions();

    for (uint32_t version : versions) {
        if (!ValidHashes.contains(version)) {
#if defined(__SWITCH__)
            SPDLOG_ERROR("Invalid OTR File!");
#elif defined(__WIIU__)
            Ship::WiiU::ThrowInvalidOTR();
#else
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Invalid OTR File",
                                     "Attempted to load an invalid OTR file. Try regenerating.", nullptr);
            SPDLOG_ERROR("Invalid OTR File!");
#endif
            exit(1);
        }
        switch (version) {
            case OOT_PAL_MQ:
            case OOT_NTSC_JP_MQ:
            case OOT_NTSC_US_MQ:
            case OOT_PAL_GC_MQ_DBG:
                hasMasterQuest = true;
                break;
            case OOT_NTSC_US_10:
            case OOT_NTSC_US_11:
            case OOT_NTSC_US_12:
            case OOT_PAL_10:
            case OOT_PAL_11:
            case OOT_NTSC_JP_GC_CE:
            case OOT_NTSC_JP_GC:
            case OOT_NTSC_US_GC:
            case OOT_PAL_GC:
            case OOT_PAL_GC_DBG1:
            case OOT_PAL_GC_DBG2:
                hasOriginal = true;
                break;
            default:
                break;
        }
    }
}

OTRGlobals::~OTRGlobals() {
}

bool OTRGlobals::HasMasterQuest() {
    return hasMasterQuest;
}

bool OTRGlobals::HasOriginal() {
    return hasOriginal;
}

uint32_t OTRGlobals::GetInterpolationFPS() {
    if (Ship::WindowBackend() == Ship::Backend::DX11) {
        return CVarGetInteger("gInterpolationFPS", 20);
    }

    if (CVarGetInteger("gMatchRefreshRate", 0)) {
        return Ship::Window::GetInstance()->GetCurrentRefreshRate();
    }

    return std::min<uint32_t>(Ship::Window::GetInstance()->GetCurrentRefreshRate(), CVarGetInteger("gInterpolationFPS", 20));
}

struct ExtensionEntry {
    std::string path;
    std::string ext;
};

extern uintptr_t clearMtx;
extern "C" Mtx gMtxClear;
extern "C" MtxF gMtxFClear;
extern "C" void OTRMessage_Init();
extern "C" void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
extern "C" int AudioPlayer_Buffered(void);
extern "C" int AudioPlayer_GetDesiredBuffered(void);
extern "C" void ResourceMgr_LoadDirectory(const char* resName);
extern "C" SequenceData ResourceMgr_LoadSeqByName(const char* path);
std::unordered_map<std::string, ExtensionEntry> ExtensionCache;

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
        //AudioMgr_ThreadEntry(&gAudioMgr);
        // 528 and 544 relate to 60 fps at 32 kHz 32000/60 = 533.333..
        // in an ideal world, one third of the calls should use num_samples=544 and two thirds num_samples=528
        //#define SAMPLES_HIGH 560
        //#define SAMPLES_LOW 528
        // PAL values
        //#define SAMPLES_HIGH 656
        //#define SAMPLES_LOW 624

        // 44KHZ values
        #define SAMPLES_HIGH 752
        #define SAMPLES_LOW 720

        #define AUDIO_FRAMES_PER_UPDATE (R_UPDATE_RATE > 0 ? R_UPDATE_RATE : 1 )
        #define NUM_AUDIO_CHANNELS 2

        int samples_left = AudioPlayer_Buffered();
        u32 num_audio_samples = samples_left < AudioPlayer_GetDesiredBuffered() ? SAMPLES_HIGH : SAMPLES_LOW;

        // 3 is the maximum authentic frame divisor.
        s16 audio_buffer[SAMPLES_HIGH * NUM_AUDIO_CHANNELS * 3];
        for (int i = 0; i < AUDIO_FRAMES_PER_UPDATE; i++) {
            AudioMgr_CreateNextAudioBuffer(audio_buffer + i * (num_audio_samples * NUM_AUDIO_CHANNELS), num_audio_samples);
        }

        AudioPlayer_Play((u8*)audio_buffer, num_audio_samples * (sizeof(int16_t) * NUM_AUDIO_CHANNELS * AUDIO_FRAMES_PER_UPDATE));

        audio.processing = false;
        audio.cv_from_thread.notify_one();
    }
}

// C->C++ Bridge
extern "C" void OTRAudio_Init()
{
    // Precache all our samples, sequences, etc...
    ResourceMgr_LoadDirectory("audio");

    if (!audio.running) {
        audio.running = true;
        audio.thread = std::thread(OTRAudio_Thread);
    }
}

extern "C" void OTRAudio_Exit() {
    // Tell the audio thread to stop
    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        audio.running = false;
    }
    audio.cv_to_thread.notify_all();

    // Wait until the audio thread quit
    audio.thread.join();
}

extern "C" void VanillaItemTable_Init() {
    static GetItemEntry getItemTable[] = {
        GET_ITEM(ITEM_BOMBS_5,          OBJECT_GI_BOMB_1,        GID_BOMB,             0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBS_5),
        GET_ITEM(ITEM_NUTS_5,           OBJECT_GI_NUTS,          GID_NUTS,             0x34, 0x0C, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_NUTS_5),
        GET_ITEM(ITEM_BOMBCHU,          OBJECT_GI_BOMB_2,        GID_BOMBCHU,          0x33, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBCHUS_10),
        GET_ITEM(ITEM_BOW,              OBJECT_GI_BOW,           GID_BOW,              0x31, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOW),
        GET_ITEM(ITEM_SLINGSHOT,        OBJECT_GI_PACHINKO,      GID_SLINGSHOT,        0x30, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SLINGSHOT),
        GET_ITEM(ITEM_BOOMERANG,        OBJECT_GI_BOOMERANG,     GID_BOOMERANG,        0x35, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOOMERANG),
        GET_ITEM(ITEM_STICK,            OBJECT_GI_STICK,         GID_STICK,            0x37, 0x0D, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_STICKS_1),
        GET_ITEM(ITEM_HOOKSHOT,         OBJECT_GI_HOOKSHOT,      GID_HOOKSHOT,         0x36, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_HOOKSHOT),
        GET_ITEM(ITEM_LONGSHOT,         OBJECT_GI_HOOKSHOT,      GID_LONGSHOT,         0x4F, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_LONGSHOT),
        GET_ITEM(ITEM_LENS,             OBJECT_GI_GLASSES,       GID_LENS,             0x39, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_LENS),
        GET_ITEM(ITEM_LETTER_ZELDA,     OBJECT_GI_LETTER,        GID_LETTER_ZELDA,     0x69, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_LETTER_ZELDA),
        GET_ITEM(ITEM_OCARINA_TIME,     OBJECT_GI_OCARINA,       GID_OCARINA_TIME,     0x3A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_OCARINA_OOT),
        GET_ITEM(ITEM_HAMMER,           OBJECT_GI_HAMMER,        GID_HAMMER,           0x38, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_HAMMER),
        GET_ITEM(ITEM_COJIRO,           OBJECT_GI_NIWATORI,      GID_COJIRO,           0x02, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_COJIRO),
        GET_ITEM(ITEM_BOTTLE,           OBJECT_GI_BOTTLE,        GID_BOTTLE,           0x42, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOTTLE),
        GET_ITEM(ITEM_POTION_RED,       OBJECT_GI_LIQUID,        GID_POTION_RED,       0x43, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_POTION_RED),
        GET_ITEM(ITEM_POTION_GREEN,     OBJECT_GI_LIQUID,        GID_POTION_GREEN,     0x44, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_POTION_GREEN),
        GET_ITEM(ITEM_POTION_BLUE,      OBJECT_GI_LIQUID,        GID_POTION_BLUE,      0x45, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_POTION_BLUE),
        GET_ITEM(ITEM_FAIRY,            OBJECT_GI_BOTTLE,        GID_BOTTLE,           0x46, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_FAIRY),
        GET_ITEM(ITEM_MILK_BOTTLE,      OBJECT_GI_MILK,          GID_MILK,             0x98, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MILK_BOTTLE),
        GET_ITEM(ITEM_LETTER_RUTO,      OBJECT_GI_BOTTLE_LETTER, GID_LETTER_RUTO,      0x99, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_LETTER_RUTO),
        GET_ITEM(ITEM_BEAN,             OBJECT_GI_BEAN,          GID_BEAN,             0x48, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BEAN),
        GET_ITEM(ITEM_MASK_SKULL,       OBJECT_GI_SKJ_MASK,      GID_MASK_SKULL,       0x10, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_SKULL),
        GET_ITEM(ITEM_MASK_SPOOKY,      OBJECT_GI_REDEAD_MASK,   GID_MASK_SPOOKY,      0x11, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_SPOOKY),
        GET_ITEM(ITEM_CHICKEN,          OBJECT_GI_NIWATORI,      GID_CHICKEN,          0x48, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_CHICKEN),
        GET_ITEM(ITEM_MASK_KEATON,      OBJECT_GI_KI_TAN_MASK,   GID_MASK_KEATON,      0x12, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_KEATON),
        GET_ITEM(ITEM_MASK_BUNNY,       OBJECT_GI_RABIT_MASK,    GID_MASK_BUNNY,       0x13, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_BUNNY),
        GET_ITEM(ITEM_MASK_TRUTH,       OBJECT_GI_TRUTH_MASK,    GID_MASK_TRUTH,       0x17, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_TRUTH),
        GET_ITEM(ITEM_POCKET_EGG,       OBJECT_GI_EGG,           GID_EGG,              0x01, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_POCKET_EGG),
        GET_ITEM(ITEM_POCKET_CUCCO,     OBJECT_GI_NIWATORI,      GID_CHICKEN,          0x48, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_POCKET_CUCCO),
        GET_ITEM(ITEM_ODD_MUSHROOM,     OBJECT_GI_MUSHROOM,      GID_ODD_MUSHROOM,     0x03, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_ODD_MUSHROOM),
        GET_ITEM(ITEM_ODD_POTION,       OBJECT_GI_POWDER,        GID_ODD_POTION,       0x04, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_ODD_POTION),
        GET_ITEM(ITEM_SAW,              OBJECT_GI_SAW,           GID_SAW,              0x05, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SAW),
        GET_ITEM(ITEM_SWORD_BROKEN,     OBJECT_GI_BROKENSWORD,   GID_SWORD_BROKEN,     0x08, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SWORD_BROKEN),
        GET_ITEM(ITEM_PRESCRIPTION,     OBJECT_GI_PRESCRIPTION,  GID_PRESCRIPTION,     0x09, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_PRESCRIPTION),
        GET_ITEM(ITEM_FROG,             OBJECT_GI_FROG,          GID_FROG,             0x0D, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_FROG),
        GET_ITEM(ITEM_EYEDROPS,         OBJECT_GI_EYE_LOTION,    GID_EYEDROPS,         0x0E, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_EYEDROPS),
        GET_ITEM(ITEM_CLAIM_CHECK,      OBJECT_GI_TICKETSTONE,   GID_CLAIM_CHECK,      0x0A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_CLAIM_CHECK),
        GET_ITEM(ITEM_SWORD_KOKIRI,     OBJECT_GI_SWORD_1,       GID_SWORD_KOKIRI,     0xA4, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SWORD_KOKIRI),
        GET_ITEM(ITEM_SWORD_BGS,        OBJECT_GI_LONGSWORD,     GID_SWORD_BGS,        0x4B, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SWORD_KNIFE),
        GET_ITEM(ITEM_SHIELD_DEKU,      OBJECT_GI_SHIELD_1,      GID_SHIELD_DEKU,      0x4C, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_SHIELD_DEKU),
        GET_ITEM(ITEM_SHIELD_HYLIAN,    OBJECT_GI_SHIELD_2,      GID_SHIELD_HYLIAN,    0x4D, 0xA0, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_SHIELD_HYLIAN),
        GET_ITEM(ITEM_SHIELD_MIRROR,    OBJECT_GI_SHIELD_3,      GID_SHIELD_MIRROR,    0x4E, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SHIELD_MIRROR),
        GET_ITEM(ITEM_TUNIC_GORON,      OBJECT_GI_CLOTHES,       GID_TUNIC_GORON,      0x50, 0xA0, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_TUNIC_GORON),
        GET_ITEM(ITEM_TUNIC_ZORA,       OBJECT_GI_CLOTHES,       GID_TUNIC_ZORA,       0x51, 0xA0, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_TUNIC_ZORA),
        GET_ITEM(ITEM_BOOTS_IRON,       OBJECT_GI_BOOTS_2,       GID_BOOTS_IRON,       0x53, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOOTS_IRON),
        GET_ITEM(ITEM_BOOTS_HOVER,      OBJECT_GI_HOVERBOOTS,    GID_BOOTS_HOVER,      0x54, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOOTS_HOVER),
        GET_ITEM(ITEM_QUIVER_40,        OBJECT_GI_ARROWCASE,     GID_QUIVER_40,        0x56, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_QUIVER_40),
        GET_ITEM(ITEM_QUIVER_50,        OBJECT_GI_ARROWCASE,     GID_QUIVER_50,        0x57, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_QUIVER_50),
        GET_ITEM(ITEM_BOMB_BAG_20,      OBJECT_GI_BOMBPOUCH,     GID_BOMB_BAG_20,      0x58, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BOMB_BAG_20),
        GET_ITEM(ITEM_BOMB_BAG_30,      OBJECT_GI_BOMBPOUCH,     GID_BOMB_BAG_30,      0x59, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_BOMB_BAG_30),
        GET_ITEM(ITEM_BOMB_BAG_40,      OBJECT_GI_BOMBPOUCH,     GID_BOMB_BAG_40,      0x5A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_BOMB_BAG_40),
        GET_ITEM(ITEM_GAUNTLETS_SILVER, OBJECT_GI_GLOVES,        GID_GAUNTLETS_SILVER, 0x5B, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_GAUNTLETS_SILVER),
        GET_ITEM(ITEM_GAUNTLETS_GOLD,   OBJECT_GI_GLOVES,        GID_GAUNTLETS_GOLD,   0x5C, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_GAUNTLETS_GOLD),
        GET_ITEM(ITEM_SCALE_SILVER,     OBJECT_GI_SCALE,         GID_SCALE_SILVER,     0xCD, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SCALE_SILVER),
        GET_ITEM(ITEM_SCALE_GOLDEN,     OBJECT_GI_SCALE,         GID_SCALE_GOLDEN,     0xCE, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SCALE_GOLD),
        GET_ITEM(ITEM_STONE_OF_AGONY,   OBJECT_GI_MAP,           GID_STONE_OF_AGONY,   0x68, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_STONE_OF_AGONY),
        GET_ITEM(ITEM_GERUDO_CARD,      OBJECT_GI_GERUDO,        GID_GERUDO_CARD,      0x7B, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_GERUDO_CARD),
        GET_ITEM(ITEM_OCARINA_FAIRY,    OBJECT_GI_OCARINA_0,     GID_OCARINA_FAIRY,    0x4A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_OCARINA_FAIRY),
        GET_ITEM(ITEM_SEEDS,            OBJECT_GI_SEED,          GID_SEEDS,            0xDC, 0x50, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_SEEDS_5),
        GET_ITEM(ITEM_HEART_CONTAINER,  OBJECT_GI_HEARTS,        GID_HEART_CONTAINER,  0xC6, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_HEART_CONTAINER),
        GET_ITEM(ITEM_HEART_PIECE_2,    OBJECT_GI_HEARTS,        GID_HEART_PIECE,      0xC2, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_HEART_PIECE),
        GET_ITEM(ITEM_KEY_BOSS,         OBJECT_GI_BOSSKEY,       GID_KEY_BOSS,         0xC7, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_BOSS_KEY,        MOD_NONE, GI_KEY_BOSS),
        GET_ITEM(ITEM_COMPASS,          OBJECT_GI_COMPASS,       GID_COMPASS,          0x67, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_COMPASS),
        GET_ITEM(ITEM_DUNGEON_MAP,      OBJECT_GI_MAP,           GID_DUNGEON_MAP,      0x66, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_MAP),
        GET_ITEM(ITEM_KEY_SMALL,        OBJECT_GI_KEY,           GID_KEY_SMALL,        0x60, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY,       MOD_NONE, GI_KEY_SMALL),
        GET_ITEM(ITEM_MAGIC_SMALL,      OBJECT_GI_MAGICPOT,      GID_MAGIC_SMALL,      0x52, 0x6F, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_MAGIC_SMALL),
        GET_ITEM(ITEM_MAGIC_LARGE,      OBJECT_GI_MAGICPOT,      GID_MAGIC_LARGE,      0x52, 0x6E, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_MAGIC_LARGE),
        GET_ITEM(ITEM_WALLET_ADULT,     OBJECT_GI_PURSE,         GID_WALLET_ADULT,     0x5E, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_WALLET_ADULT),
        GET_ITEM(ITEM_WALLET_GIANT,     OBJECT_GI_PURSE,         GID_WALLET_GIANT,     0x5F, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_WALLET_GIANT),
        GET_ITEM(ITEM_WEIRD_EGG,        OBJECT_GI_EGG,           GID_EGG,              0x9A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_WEIRD_EGG),
        GET_ITEM(ITEM_HEART,            OBJECT_GI_HEART,         GID_HEART,            0x55, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_HEART),
        GET_ITEM(ITEM_ARROWS_SMALL,     OBJECT_GI_ARROW,         GID_ARROWS_SMALL,     0xE6, 0x48, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_ARROWS_SMALL),
        GET_ITEM(ITEM_ARROWS_MEDIUM,    OBJECT_GI_ARROW,         GID_ARROWS_MEDIUM,    0xE6, 0x49, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_ARROWS_MEDIUM),
        GET_ITEM(ITEM_ARROWS_LARGE,     OBJECT_GI_ARROW,         GID_ARROWS_LARGE,     0xE6, 0x4A, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_ARROWS_LARGE),
        GET_ITEM(ITEM_RUPEE_GREEN,      OBJECT_GI_RUPY,          GID_RUPEE_GREEN,      0x6F, 0x00, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_GREEN),
        GET_ITEM(ITEM_RUPEE_BLUE,       OBJECT_GI_RUPY,          GID_RUPEE_BLUE,       0xCC, 0x01, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_BLUE),
        GET_ITEM(ITEM_RUPEE_RED,        OBJECT_GI_RUPY,          GID_RUPEE_RED,        0xF0, 0x02, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_RED),
        GET_ITEM(ITEM_HEART_CONTAINER,  OBJECT_GI_HEARTS,        GID_HEART_CONTAINER,  0xC6, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_HEART_CONTAINER_2),
        GET_ITEM(ITEM_MILK,             OBJECT_GI_MILK,          GID_MILK,             0x98, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_MILK),
        GET_ITEM(ITEM_MASK_GORON,       OBJECT_GI_GOLONMASK,     GID_MASK_GORON,       0x14, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_GORON),
        GET_ITEM(ITEM_MASK_ZORA,        OBJECT_GI_ZORAMASK,      GID_MASK_ZORA,        0x15, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_ZORA),
        GET_ITEM(ITEM_MASK_GERUDO,      OBJECT_GI_GERUDOMASK,    GID_MASK_GERUDO,      0x16, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_MASK_GERUDO),
        GET_ITEM(ITEM_BRACELET,         OBJECT_GI_BRACELET,      GID_BRACELET,         0x79, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_BRACELET),
        GET_ITEM(ITEM_RUPEE_PURPLE,     OBJECT_GI_RUPY,          GID_RUPEE_PURPLE,     0xF1, 0x14, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_PURPLE),
        GET_ITEM(ITEM_RUPEE_GOLD,       OBJECT_GI_RUPY,          GID_RUPEE_GOLD,       0xF2, 0x13, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_RUPEE_GOLD),
        GET_ITEM(ITEM_SWORD_BGS,        OBJECT_GI_LONGSWORD,     GID_SWORD_BGS,        0x0C, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_SWORD_BGS),
        GET_ITEM(ITEM_ARROW_FIRE,       OBJECT_GI_M_ARROW,       GID_ARROW_FIRE,       0x70, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_ARROW_FIRE),
        GET_ITEM(ITEM_ARROW_ICE,        OBJECT_GI_M_ARROW,       GID_ARROW_ICE,        0x71, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_ARROW_ICE),
        GET_ITEM(ITEM_ARROW_LIGHT,      OBJECT_GI_M_ARROW,       GID_ARROW_LIGHT,      0x72, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_ARROW_LIGHT),
        GET_ITEM(ITEM_SKULL_TOKEN,      OBJECT_GI_SUTARU,        GID_SKULL_TOKEN,      0xB4, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SKULLTULA_TOKEN, MOD_NONE, GI_SKULL_TOKEN),
        GET_ITEM(ITEM_DINS_FIRE,        OBJECT_GI_GODDESS,       GID_DINS_FIRE,        0xAD, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_DINS_FIRE),
        GET_ITEM(ITEM_FARORES_WIND,     OBJECT_GI_GODDESS,       GID_FARORES_WIND,     0xAE, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_FARORES_WIND),
        GET_ITEM(ITEM_NAYRUS_LOVE,      OBJECT_GI_GODDESS,       GID_NAYRUS_LOVE,      0xAF, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_MAJOR,           MOD_NONE, GI_NAYRUS_LOVE),
        GET_ITEM(ITEM_BULLET_BAG_30,    OBJECT_GI_DEKUPOUCH,     GID_BULLET_BAG,       0x07, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_BULLET_BAG_30),
        GET_ITEM(ITEM_BULLET_BAG_40,    OBJECT_GI_DEKUPOUCH,     GID_BULLET_BAG,       0x07, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_BULLET_BAG_40),
        GET_ITEM(ITEM_STICKS_5,         OBJECT_GI_STICK,         GID_STICK,            0x37, 0x0D, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_STICKS_5),
        GET_ITEM(ITEM_STICKS_10,        OBJECT_GI_STICK,         GID_STICK,            0x37, 0x0D, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_STICKS_10),
        GET_ITEM(ITEM_NUTS_5,           OBJECT_GI_NUTS,          GID_NUTS,             0x34, 0x0C, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_NUTS_5_2),
        GET_ITEM(ITEM_NUTS_10,          OBJECT_GI_NUTS,          GID_NUTS,             0x34, 0x0C, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_NUTS_10),
        GET_ITEM(ITEM_BOMB,             OBJECT_GI_BOMB_1,        GID_BOMB,             0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBS_1),
        GET_ITEM(ITEM_BOMBS_10,         OBJECT_GI_BOMB_1,        GID_BOMB,             0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBS_10),
        GET_ITEM(ITEM_BOMBS_20,         OBJECT_GI_BOMB_1,        GID_BOMB,             0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBS_20),
        GET_ITEM(ITEM_BOMBS_30,         OBJECT_GI_BOMB_1,        GID_BOMB,             0x32, 0x59, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBS_30),
        GET_ITEM(ITEM_SEEDS_30,         OBJECT_GI_SEED,          GID_SEEDS,            0xDC, 0x50, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_SEEDS_30),
        GET_ITEM(ITEM_BOMBCHUS_5,       OBJECT_GI_BOMB_2,        GID_BOMBCHU,          0x33, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBCHUS_5),
        GET_ITEM(ITEM_BOMBCHUS_20,      OBJECT_GI_BOMB_2,        GID_BOMBCHU,          0x33, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BOMBCHUS_20),
        GET_ITEM(ITEM_FISH,             OBJECT_GI_FISH,          GID_FISH,             0x47, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_FISH),
        GET_ITEM(ITEM_BUG,              OBJECT_GI_INSECT,        GID_BUG,              0x7A, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BUGS),
        GET_ITEM(ITEM_BLUE_FIRE,        OBJECT_GI_FIRE,          GID_BLUE_FIRE,        0x5D, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BLUE_FIRE),
        GET_ITEM(ITEM_POE,              OBJECT_GI_GHOST,         GID_POE,              0x97, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_POE),
        GET_ITEM(ITEM_BIG_POE,          OBJECT_GI_GHOST,         GID_BIG_POE,          0xF9, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_JUNK,            MOD_NONE, GI_BIG_POE),
        GET_ITEM(ITEM_KEY_SMALL,        OBJECT_GI_KEY,           GID_KEY_SMALL,        0xF3, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_SMALL_KEY,       MOD_NONE, GI_DOOR_KEY),
        GET_ITEM(ITEM_RUPEE_GREEN,      OBJECT_GI_RUPY,          GID_RUPEE_GREEN,      0xF4, 0x00, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_GREEN_LOSE),
        GET_ITEM(ITEM_RUPEE_BLUE,       OBJECT_GI_RUPY,          GID_RUPEE_BLUE,       0xF5, 0x01, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_BLUE_LOSE),
        GET_ITEM(ITEM_RUPEE_RED,        OBJECT_GI_RUPY,          GID_RUPEE_RED,        0xF6, 0x02, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_RED_LOSE),
        GET_ITEM(ITEM_RUPEE_PURPLE,     OBJECT_GI_RUPY,          GID_RUPEE_PURPLE,     0xF7, 0x14, CHEST_ANIM_SHORT, ITEM_CATEGORY_JUNK,            MOD_NONE, GI_RUPEE_PURPLE_LOSE),
        GET_ITEM(ITEM_HEART_PIECE_2,    OBJECT_GI_HEARTS,        GID_HEART_PIECE,      0xFA, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_HEART_PIECE_WIN),
        GET_ITEM(ITEM_STICK_UPGRADE_20, OBJECT_GI_STICK,         GID_STICK,            0x90, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_STICK_UPGRADE_20),
        GET_ITEM(ITEM_STICK_UPGRADE_30, OBJECT_GI_STICK,         GID_STICK,            0x91, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_STICK_UPGRADE_30),
        GET_ITEM(ITEM_NUT_UPGRADE_30,   OBJECT_GI_NUTS,          GID_NUTS,             0xA7, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_NUT_UPGRADE_30),
        GET_ITEM(ITEM_NUT_UPGRADE_40,   OBJECT_GI_NUTS,          GID_NUTS,             0xA8, 0x80, CHEST_ANIM_SHORT, ITEM_CATEGORY_LESSER,          MOD_NONE, GI_NUT_UPGRADE_40),
        GET_ITEM(ITEM_BULLET_BAG_50,    OBJECT_GI_DEKUPOUCH,     GID_BULLET_BAG_50,    0x6C, 0x80, CHEST_ANIM_LONG,  ITEM_CATEGORY_LESSER,          MOD_NONE, GI_BULLET_BAG_50),
        GET_ITEM_NONE,
        GET_ITEM_NONE,
        GET_ITEM_NONE // GI_MAX - if you need to add to this table insert it before this entry.
    };
    ItemTableManager::Instance->AddItemTable(MOD_NONE);
    for (uint8_t i = 0; i < ARRAY_COUNT(getItemTable); i++) {
        // The vanilla item table array started with ITEM_BOMBS_5,
        // but the GetItemID enum started with GI_NONE. Then everywhere
        // that table was accessed used `GetItemID - 1`. This allows the
        // "first" item of the new map to start at 1, syncing it up with
        // the GetItemID values and removing the need for the `- 1`
        ItemTableManager::Instance->AddItemEntry(MOD_NONE, i+1, getItemTable[i]);
    }
}

std::unordered_map<uint32_t, uint32_t> ItemIDtoGetItemID{
    { ITEM_ARROWS_LARGE, GI_ARROWS_LARGE },
    { ITEM_ARROWS_MEDIUM, GI_ARROWS_MEDIUM },
    { ITEM_ARROWS_SMALL, GI_ARROWS_SMALL },
    { ITEM_ARROW_FIRE, GI_ARROW_FIRE },
    { ITEM_ARROW_ICE, GI_ARROW_ICE },
    { ITEM_ARROW_LIGHT, GI_ARROW_LIGHT },
    { ITEM_BEAN, GI_BEAN },
    { ITEM_BIG_POE, GI_BIG_POE },
    { ITEM_BLUE_FIRE, GI_BLUE_FIRE },
    { ITEM_BOMB, GI_BOMBS_1 },
    { ITEM_BOMBCHU, GI_BOMBCHUS_10 },
    { ITEM_BOMBCHUS_20, GI_BOMBCHUS_20 },
    { ITEM_BOMBCHUS_5, GI_BOMBCHUS_5 },
    { ITEM_BOMBS_10, GI_BOMBS_10 },
    { ITEM_BOMBS_20, GI_BOMBS_20 },
    { ITEM_BOMBS_30, GI_BOMBS_30 },
    { ITEM_BOMBS_5, GI_BOMBS_5 },
    { ITEM_BOMB_BAG_20, GI_BOMB_BAG_20 },
    { ITEM_BOMB_BAG_30, GI_BOMB_BAG_30 },
    { ITEM_BOMB_BAG_40, GI_BOMB_BAG_40 },
    { ITEM_BOOMERANG, GI_BOOMERANG },
    { ITEM_BOOTS_HOVER, GI_BOOTS_HOVER },
    { ITEM_BOOTS_IRON, GI_BOOTS_IRON },
    { ITEM_BOTTLE, GI_BOTTLE },
    { ITEM_BOW, GI_BOW },
    { ITEM_BRACELET, GI_BRACELET },
    { ITEM_BUG, GI_BUGS },
    { ITEM_BULLET_BAG_30, GI_BULLET_BAG_30 },
    { ITEM_BULLET_BAG_40, GI_BULLET_BAG_40 },
    { ITEM_BULLET_BAG_50, GI_BULLET_BAG_50 }, { ITEM_CHICKEN, GI_CHICKEN },
    { ITEM_CLAIM_CHECK, GI_CLAIM_CHECK },
    { ITEM_COJIRO, GI_COJIRO },
    { ITEM_COMPASS, GI_COMPASS },
    { ITEM_DINS_FIRE, GI_DINS_FIRE },
    { ITEM_DUNGEON_MAP, GI_MAP },
    { ITEM_EYEDROPS, GI_EYEDROPS },
    { ITEM_FAIRY, GI_FAIRY },
    { ITEM_FARORES_WIND, GI_FARORES_WIND },
    { ITEM_FISH, GI_FISH },
    { ITEM_FROG, GI_FROG },
    { ITEM_GAUNTLETS_GOLD, GI_GAUNTLETS_GOLD },
    { ITEM_GAUNTLETS_SILVER, GI_GAUNTLETS_SILVER },
    { ITEM_GERUDO_CARD, GI_GERUDO_CARD },
    { ITEM_HAMMER, GI_HAMMER },
    { ITEM_HEART, GI_HEART },
    { ITEM_HEART_CONTAINER, GI_HEART_CONTAINER },
    { ITEM_HEART_CONTAINER, GI_HEART_CONTAINER_2 },
    { ITEM_HEART_PIECE_2, GI_HEART_PIECE },
    { ITEM_HEART_PIECE_2, GI_HEART_PIECE_WIN },
    { ITEM_HOOKSHOT, GI_HOOKSHOT },
    { ITEM_KEY_BOSS, GI_KEY_BOSS },
    { ITEM_KEY_SMALL, GI_DOOR_KEY },
    { ITEM_KEY_SMALL, GI_KEY_SMALL },
    { ITEM_LENS, GI_LENS },
    { ITEM_LETTER_RUTO, GI_LETTER_RUTO },
    { ITEM_LETTER_ZELDA, GI_LETTER_ZELDA },
    { ITEM_LONGSHOT, GI_LONGSHOT },
    { ITEM_MAGIC_LARGE, GI_MAGIC_LARGE },
    { ITEM_MAGIC_SMALL, GI_MAGIC_SMALL },
    { ITEM_MASK_BUNNY, GI_MASK_BUNNY },
    { ITEM_MASK_GERUDO, GI_MASK_GERUDO },
    { ITEM_MASK_GORON, GI_MASK_GORON },
    { ITEM_MASK_KEATON, GI_MASK_KEATON },
    { ITEM_MASK_SKULL, GI_MASK_SKULL },
    { ITEM_MASK_SPOOKY, GI_MASK_SPOOKY },
    { ITEM_MASK_TRUTH, GI_MASK_TRUTH },
    { ITEM_MASK_ZORA, GI_MASK_ZORA },
    { ITEM_MILK, GI_MILK },
    { ITEM_MILK_BOTTLE, GI_MILK_BOTTLE },
    { ITEM_NAYRUS_LOVE, GI_NAYRUS_LOVE },
    { ITEM_NUT, GI_NUTS_5 },
    { ITEM_NUTS_10, GI_NUTS_10 },
    { ITEM_NUTS_5, GI_NUTS_5 },
    { ITEM_NUTS_5, GI_NUTS_5_2 },
    { ITEM_NUT_UPGRADE_30, GI_NUT_UPGRADE_30 },
    { ITEM_NUT_UPGRADE_40, GI_NUT_UPGRADE_40 },
    { ITEM_OCARINA_FAIRY, GI_OCARINA_FAIRY },
    { ITEM_OCARINA_TIME, GI_OCARINA_OOT },
    { ITEM_ODD_MUSHROOM, GI_ODD_MUSHROOM },
    { ITEM_ODD_POTION, GI_ODD_POTION },
    { ITEM_POCKET_CUCCO, GI_POCKET_CUCCO },
    { ITEM_POCKET_EGG, GI_POCKET_EGG },
    { ITEM_POE, GI_POE },
    { ITEM_POTION_BLUE, GI_POTION_BLUE },
    { ITEM_POTION_GREEN, GI_POTION_GREEN },
    { ITEM_POTION_RED, GI_POTION_RED },
    { ITEM_PRESCRIPTION, GI_PRESCRIPTION },
    { ITEM_QUIVER_40, GI_QUIVER_40 },
    { ITEM_QUIVER_50, GI_QUIVER_50 },
    { ITEM_RUPEE_BLUE, GI_RUPEE_BLUE },
    { ITEM_RUPEE_BLUE, GI_RUPEE_BLUE_LOSE },
    { ITEM_RUPEE_GOLD, GI_RUPEE_GOLD },
    { ITEM_RUPEE_GREEN, GI_RUPEE_GREEN },
    { ITEM_RUPEE_GREEN, GI_RUPEE_GREEN_LOSE },
    { ITEM_RUPEE_PURPLE, GI_RUPEE_PURPLE },
    { ITEM_RUPEE_PURPLE, GI_RUPEE_PURPLE_LOSE },
    { ITEM_RUPEE_RED, GI_RUPEE_RED },
    { ITEM_RUPEE_RED, GI_RUPEE_RED_LOSE },
    { ITEM_SAW, GI_SAW },
    { ITEM_SCALE_GOLDEN, GI_SCALE_GOLD },
    { ITEM_SCALE_SILVER, GI_SCALE_SILVER },
    { ITEM_SEEDS, GI_SEEDS_5 },
    { ITEM_SEEDS_30, GI_SEEDS_30 },
    { ITEM_SHIELD_DEKU, GI_SHIELD_DEKU },
    { ITEM_SHIELD_HYLIAN, GI_SHIELD_HYLIAN },
    { ITEM_SHIELD_MIRROR, GI_SHIELD_MIRROR },
    { ITEM_SKULL_TOKEN, GI_SKULL_TOKEN },
    { ITEM_SLINGSHOT, GI_SLINGSHOT },
    { ITEM_STICK, GI_STICKS_1 },
    { ITEM_STICKS_10, GI_STICKS_10 },
    { ITEM_STICKS_5, GI_STICKS_5 },
    { ITEM_STICK_UPGRADE_20, GI_STICK_UPGRADE_20 },
    { ITEM_STICK_UPGRADE_30, GI_STICK_UPGRADE_30 },
    { ITEM_STONE_OF_AGONY, GI_STONE_OF_AGONY },
    { ITEM_SWORD_BGS, GI_SWORD_BGS },
    { ITEM_SWORD_BGS, GI_SWORD_KNIFE },
    { ITEM_SWORD_BROKEN, GI_SWORD_BROKEN },
    { ITEM_SWORD_KOKIRI, GI_SWORD_KOKIRI },
    { ITEM_TUNIC_GORON, GI_TUNIC_GORON },
    { ITEM_TUNIC_ZORA, GI_TUNIC_ZORA },
    { ITEM_WALLET_ADULT, GI_WALLET_ADULT },
    { ITEM_WALLET_GIANT, GI_WALLET_GIANT },
    { ITEM_WEIRD_EGG, GI_WEIRD_EGG }
};

extern "C" uint32_t GetGIID(uint32_t itemID) {
    if (ItemIDtoGetItemID.contains(itemID))
        return ItemIDtoGetItemID.at(itemID);
    return -1;
}

extern "C" void OTRExtScanner() {
    auto lst = *OTRGlobals::Instance->context->GetResourceManager()->GetArchive()->ListFiles("*").get();

    for (auto& rPath : lst) {
        std::vector<std::string> raw = StringHelper::Split(rPath, ".");
        std::string ext = raw[raw.size() - 1];
        std::string nPath = rPath.substr(0, rPath.size() - (ext.size() + 1));
        replace(nPath.begin(), nPath.end(), '\\', '/');

        ExtensionCache[nPath] = { rPath, ext };
    }
}

extern "C" void InitOTR() {
#if not defined (__SWITCH__) && not defined(__WIIU__)
    if (!std::filesystem::exists(Ship::Window::GetPathRelativeToAppDirectory("oot-mq.otr")) &&
        !std::filesystem::exists(Ship::Window::GetPathRelativeToAppDirectory("oot.otr"))){
        if (Extractor::ShowYesNoBox("No OTR Files", "No OTR files found. Generate one now?") == IDYES) {
            Extractor extract;
            if (!extract.Run()) {
                Extractor::ShowErrorBox("Error", "An error occured, no OTR file was generated. Exiting...");
                exit(1);
            }
            extract.CallZapd();
        } else {
            exit(1);
        }
        if (Extractor::ShowYesNoBox("Extraction Complete", "ROM Extracted. Extract another?") == IDYES) {
            Extractor extract;
            if (!extract.Run()) {
                Extractor::ShowErrorBox("Error", "An error occured, an OTR file may have been generated by a different step. Continuing...");
            } else {
                extract.CallZapd();
            }
        }
    }
#endif

#ifdef __SWITCH__
    Ship::Switch::Init(Ship::PreInitPhase);
#elif defined(__WIIU__)
    Ship::WiiU::Init();
#endif
    Ship::AddSetupHooksDelegate(GameMenuBar::SetupHooks);
    Ship::RegisterMenuDrawMethod(GameMenuBar::Draw);

    OTRGlobals::Instance = new OTRGlobals();
    SaveManager::Instance = new SaveManager();
    CustomMessageManager::Instance = new CustomMessageManager();
    ItemTableManager::Instance = new ItemTableManager();
    GameInteractor::Instance = new GameInteractor();
    AudioCollection::Instance = new AudioCollection();
#ifdef __APPLE__
    SpeechSynthesizer::Instance = new DarwinSpeechSynthesizer();
    SpeechSynthesizer::Instance->Init();
#elif defined(_WIN32)
    SpeechSynthesizer::Instance = new SAPISpeechSynthesizer();
    SpeechSynthesizer::Instance->Init();
#endif
    
    clearMtx = (uintptr_t)&gMtxClear;
    OTRMessage_Init();
    OTRAudio_Init();
    InitCosmeticsEditor();
    GameControlEditor::Init();
    InitAudioEditor();
    DebugConsole_Init();
    Debug_Init();
    Rando_Init();
    InitItemTracker();
    InitEntranceTracker();
    InitStatTracker();
    CheckTracker::InitCheckTracker();
    OTRExtScanner();
    VanillaItemTable_Init();

    InitMods();

    time_t now = time(NULL);
    tm *tm_now = localtime(&now);
    if (tm_now->tm_mon == 11 && tm_now->tm_mday >= 24 && tm_now->tm_mday <= 25) {
        CVarRegisterInteger("gLetItSnow", 1);
    } else {
        CVarClear("gLetItSnow");
    }
#ifdef ENABLE_CROWD_CONTROL
    CrowdControl::Instance = new CrowdControl();
    CrowdControl::Instance->Init();
    if (CVarGetInteger("gCrowdControl", 0)) {
        CrowdControl::Instance->Enable();
    } else {
        CrowdControl::Instance->Disable();
    }
#endif
}

extern "C" void DeinitOTR() {
    OTRAudio_Exit();
#if defined(_WIN32) || defined(__APPLE__)
    SpeechSynthesizerUninitialize();
#endif
#ifdef ENABLE_CROWD_CONTROL
    CrowdControl::Instance->Disable();
    CrowdControl::Instance->Shutdown();
#endif
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

// C->C++ Bridge
extern "C" void Graph_ProcessFrame(void (*run_one_game_iter)(void)) {
    OTRGlobals::Instance->context->MainLoop(run_one_game_iter);
}

extern bool ShouldClearTextureCacheAtEndOfFrame;

extern "C" void Graph_StartFrame() {
#ifndef __WIIU__
    using Ship::KbScancode;
    int32_t dwScancode = OTRGlobals::Instance->context->GetLastScancode();
    OTRGlobals::Instance->context->SetLastScancode(-1);

    switch (dwScancode) {
        case KbScancode::LUS_KB_F5: {
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
                [[unlikely]] default:
                    break;
            }
            break;
        }
        case KbScancode::LUS_KB_F6: {
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
                [[unlikely]] default:
                    break;
            }

            break;
        }
#if defined(_WIN32) || defined(__APPLE__)
        case KbScancode::LUS_KB_F9: {
            // Toggle TTS
            CVarSetInteger("gA11yTTS", !CVarGetInteger("gA11yTTS", 0));
            break;
        }
#endif
        case KbScancode::LUS_KB_TAB: {
            // Toggle HD Assets
            CVarSetInteger("gAltAssets", !CVarGetInteger("gAltAssets", 0));
            ShouldClearTextureCacheAtEndOfFrame = true;
            break;
        }
    }
#endif
    OTRGlobals::Instance->context->StartFrame();
}

void RunCommands(Gfx* Commands, const std::vector<std::unordered_map<Mtx*, MtxF>>& mtx_replacements) {
    for (const auto& m : mtx_replacements) {
        gfx_run(Commands, m);
        gfx_end_frame();
    }
}

// C->C++ Bridge
extern "C" void Graph_ProcessGfxCommands(Gfx* commands) {
    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        audio.processing = true;
    }

    audio.cv_to_thread.notify_one();
    std::vector<std::unordered_map<Mtx*, MtxF>> mtx_replacements;
    int target_fps = OTRGlobals::Instance->GetInterpolationFPS();
    static int last_fps;
    static int last_update_rate;
    static int time;
    int fps = target_fps;
    int original_fps = 60 / R_UPDATE_RATE;

    if (target_fps == 20 || original_fps > target_fps) {
        fps = original_fps;
    }

    if (last_fps != fps || last_update_rate != R_UPDATE_RATE) {
        time = 0;
    }

    // time_base = fps * original_fps (one second)
    int next_original_frame = fps;

    while (time + original_fps <= next_original_frame) {
        time += original_fps;
        if (time != next_original_frame) {
            mtx_replacements.push_back(FrameInterpolation_Interpolate((float)time / next_original_frame));
        } else {
            mtx_replacements.emplace_back();
        }
    }

    time -= fps;

    OTRGlobals::Instance->context->SetTargetFps(fps);

    int threshold = CVarGetInteger("gExtraLatencyThreshold", 80);
    OTRGlobals::Instance->context->SetMaximumFrameLatency(threshold > 0 && target_fps >= threshold ? 2 : 1);

    RunCommands(commands, mtx_replacements);

    last_fps = fps;
    last_update_rate = R_UPDATE_RATE;

    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        while (audio.processing) {
            audio.cv_from_thread.wait(Lock);
        }
    }

    if (ShouldClearTextureCacheAtEndOfFrame) {
        gfx_texture_cache_clear();
        Ship::SkeletonPatcher::UpdateSkeletons();
        ShouldClearTextureCacheAtEndOfFrame = false;
    }

    // OTRTODO: FIGURE OUT END FRAME POINT
   /* if (OTRGlobals::Instance->context->lastScancode != -1)
        OTRGlobals::Instance->context->lastScancode = -1;*/

}

float divisor_num = 0.0f;

extern "C" void OTRGetPixelDepthPrepare(float x, float y) {
    OTRGlobals::Instance->context->GetPixelDepthPrepare(x, y);
}

extern "C" uint16_t OTRGetPixelDepth(float x, float y) {
    return OTRGlobals::Instance->context->GetPixelDepth(x, y);
}

extern "C" uint32_t ResourceMgr_GetNumGameVersions() {
    return OTRGlobals::Instance->context->GetResourceManager()->GetArchive()->GetGameVersions().size();
}

extern "C" uint32_t ResourceMgr_GetGameVersion(int index) {
    return OTRGlobals::Instance->context->GetResourceManager()->GetArchive()->GetGameVersions()[index];
}

uint32_t IsSceneMasterQuest(s16 sceneNum) {
    uint32_t value = 0;
    if (OTRGlobals::Instance->HasMasterQuest()) {
        if (!OTRGlobals::Instance->HasOriginal()) {
            value = 1;
        } else if (gSaveContext.isMasterQuest) {
            value = 1;
        } else {
            value = 0;
            if (gSaveContext.n64ddFlag) {
                if (!OTRGlobals::Instance->gRandomizer->masterQuestDungeons.empty()) {
                    if (gPlayState != NULL && OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(sceneNum)) {
                        value = 1;
                    }
                }
            }
        }
    }
    return value;
}

uint32_t IsGameMasterQuest() {
    return gPlayState != NULL ? IsSceneMasterQuest(gPlayState->sceneNum) : 0;
}

extern "C" uint32_t ResourceMgr_GameHasMasterQuest() {
    return OTRGlobals::Instance->HasMasterQuest();
}

extern "C" uint32_t ResourceMgr_GameHasOriginal() {
    return OTRGlobals::Instance->HasOriginal();
}

extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum) {
    return IsSceneMasterQuest(sceneNum);
}

extern "C" uint32_t ResourceMgr_IsGameMasterQuest() {
    return IsGameMasterQuest();
}

extern "C" void ResourceMgr_LoadDirectory(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->LoadDirectory(resName);
}
extern "C" void ResourceMgr_DirtyDirectory(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->DirtyDirectory(resName);
}

// OTRTODO: There is probably a more elegant way to go about this...
// Kenix: This is definitely leaking memory when it's called.
extern "C" char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize) {
    auto lst = OTRGlobals::Instance->context->GetResourceManager()->GetArchive()->ListFiles(searchMask);
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
    if(path.substr(0, 7) == "__OTR__"){
        path = path.substr(7);
    }

    return ExtensionCache.contains(path);
}

extern "C" void ResourceMgr_LoadFile(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->LoadResource(resName);
}

std::shared_ptr<Ship::Resource> GetResourceByNameHandlingMQ(const char* path) {
    std::string Path = path;
    if (ResourceMgr_IsGameMasterQuest()) {
        size_t pos = 0;
        if ((pos = Path.find("/nonmq/", 0)) != std::string::npos) {
            Path.replace(pos, 7, "/mq/");
        }
    }
    return OTRGlobals::Instance->context->GetResourceManager()->LoadResource(Path.c_str());
}

extern "C" char* GetResourceDataByNameHandlingMQ(const char* path) {
    auto res = GetResourceByNameHandlingMQ(path);
    
    if (res == nullptr) {
        return nullptr;
    }
    
    return (char*)res->GetPointer();
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
    auto res = GetResourceByNameHandlingMQ(texPath);
    return res->InitData->Type == Ship::ResourceType::SOH_Background;
}

extern "C" char* ResourceMgr_LoadJPEG(char* data, size_t dataSize)
{
    static char* finalBuffer = 0;

    if (finalBuffer == 0)
        finalBuffer = (char*)malloc(dataSize);

    int w;
    int h;
    int comp;

    unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, 320 * 240 * 2, &w, &h, &comp, STBI_rgb_alpha);
    //unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, 480 * 240 * 2, &w, &h, &comp, STBI_rgb_alpha);
    int idx = 0;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
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
    auto res = GetResourceByNameHandlingMQ(filePath);

    if (res->InitData->Type == Ship::ResourceType::DisplayList)
        return (char*)&((std::static_pointer_cast<Ship::DisplayList>(res))->Instructions[0]);
    else if (res->InitData->Type == Ship::ResourceType::Array)
        return (char*)(std::static_pointer_cast<Ship::Array>(res))->Vertices.data();
    else {
        return (char*)GetResourceDataByNameHandlingMQ(filePath);
    }
}

extern "C" char* ResourceMgr_LoadIfDListByName(const char* filePath) {
    auto res = GetResourceByNameHandlingMQ(filePath);

    if (res->InitData->Type == Ship::ResourceType::DisplayList)
        return (char*)&((std::static_pointer_cast<Ship::DisplayList>(res))->Instructions[0]);
    
    return nullptr;
}

extern "C" Sprite* GetSeedTexture(uint8_t index) {
    return OTRGlobals::Instance->gRandomizer->GetSeedTexture(index);
}

extern "C" char* ResourceMgr_LoadPlayerAnimByName(const char* animPath) {
    auto anim = std::static_pointer_cast<Ship::PlayerAnimation>(GetResourceByNameHandlingMQ(animPath));

    return (char*)&anim->limbRotData[0];
}

extern "C" void ResourceMgr_PushCurrentDirectory(char* path)
{
    gfx_push_current_dir(path);
}

extern "C" Gfx* ResourceMgr_LoadGfxByName(const char* path)
{
    auto res = std::static_pointer_cast<Ship::DisplayList>(GetResourceByNameHandlingMQ(path));
    return (Gfx*)&res->Instructions[0];
}

typedef struct {
    int index;
    Gfx instruction;
} GfxPatch;

std::unordered_map<std::string, std::unordered_map<std::string, GfxPatch>> originalGfx;

// Attention! This is primarily for cosmetics & bug fixes. For things like mods and model replacement you should be using OTRs
// instead (When that is available). Index can be found using the commented out section below.
extern "C" void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction) {
    auto res = std::static_pointer_cast<Ship::DisplayList>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    // Leaving this here for people attempting to find the correct Dlist index to patch
    /*if (strcmp("__OTR__objects/object_gi_longsword/gGiBiggoronSwordDL", path) == 0) {
        for (int i = 0; i < res->instructions.size(); i++) {
            Gfx* gfx = (Gfx*)&res->instructions[i];
            // Log all commands
            // SPDLOG_INFO("index:{} command:{}", i, gfx->words.w0 >> 24);
            // Log only SetPrimColors
            if (gfx->words.w0 >> 24 == 250) {
                SPDLOG_INFO("index:{} r:{} g:{} b:{} a:{}", i, _SHIFTR(gfx->words.w1, 24, 8), _SHIFTR(gfx->words.w1, 16, 8), _SHIFTR(gfx->words.w1, 8, 8), _SHIFTR(gfx->words.w1, 0, 8));
            }
        }
    }*/

    // Index refers to individual gfx words, which are half the size on 32-bit
    // if (sizeof(uintptr_t) < 8) {
    // index /= 2;
    // }

    Gfx* gfx = (Gfx*)&res->Instructions[index];

    if (!originalGfx.contains(path) || !originalGfx[path].contains(patchName)) {
        originalGfx[path][patchName] = {
            index,
            *gfx
        };
    }

    *gfx = instruction;
}

extern "C" void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName) {
    if (originalGfx.contains(path) && originalGfx[path].contains(patchName)) {
        auto res = std::static_pointer_cast<Ship::DisplayList>(
            OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

        Gfx* gfx = (Gfx*)&res->Instructions[originalGfx[path][patchName].index];
        *gfx = originalGfx[path][patchName].instruction;

        originalGfx[path].erase(patchName);
    }
}

extern "C" char* ResourceMgr_LoadArrayByName(const char* path)
{
    auto res = std::static_pointer_cast<Ship::Array>(GetResourceByNameHandlingMQ(path));

    return (char*)res->Scalars.data();
}

extern "C" char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path) {
    auto res = std::static_pointer_cast<Ship::Array>(GetResourceByNameHandlingMQ(path));

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

extern "C" CollisionHeader* ResourceMgr_LoadColByName(const char* path) {
    return (CollisionHeader*)GetResourceDataByName(path, false);
}

extern "C" Vtx* ResourceMgr_LoadVtxByName(char* path) {
    return (Vtx*)GetResourceDataByName(path, false);
}

extern "C" SequenceData ResourceMgr_LoadSeqByName(const char* path) {
    SequenceData* sequence = (SequenceData*)GetResourceDataByName(path, false);
    return *sequence;
}

std::map<std::string, SoundFontSample*> cachedCustomSFs;

extern "C" SoundFontSample* ReadCustomSample(const char* path) {
    return nullptr;
/*
    if (!ExtensionCache.contains(path))
        return nullptr;

    ExtensionEntry entry = ExtensionCache[path];

    auto sampleRaw = OTRGlobals::Instance->context->GetResourceManager()->LoadFile(entry.path);
    uint32_t* strem = (uint32_t*)sampleRaw->Buffer.get();
    uint8_t* strem2 = (uint8_t*)strem;

    SoundFontSample* sampleC = new SoundFontSample;

    if (entry.ext == "wav") {
        drwav_uint32 channels;
        drwav_uint32 sampleRate;
        drwav_uint64 totalPcm;
        drmp3_int16* pcmData =
            drwav_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->BufferSize, &channels, &sampleRate, &totalPcm, NULL);
        sampleC->size = totalPcm;
        sampleC->sampleAddr = (uint8_t*)pcmData;
        sampleC->codec = CODEC_S16;

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
    return (SoundFontSample*)GetResourceDataByName(path, false);
}

extern "C" SoundFont* ResourceMgr_LoadAudioSoundFont(const char* path) {
    return (SoundFont*)GetResourceDataByName(path, false);
}

extern "C" int ResourceMgr_OTRSigCheck(char* imgData)
{
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

extern "C" AnimationHeaderCommon* ResourceMgr_LoadAnimByName(const char* path) {
    return (AnimationHeaderCommon*)GetResourceDataByName(path, false);
}

extern "C" SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path, SkelAnime* skelAnime) {
    std::string pathStr = std::string(path);
    static const std::string sOtr = "__OTR__";

    if (pathStr.starts_with(sOtr)) {
        pathStr = pathStr.substr(sOtr.length());
    }

    bool isAlt = CVarGetInteger("gAltAssets", 0);

    if (isAlt) {
        pathStr = Ship::Resource::gAltAssetPrefix + pathStr;
    }

    SkeletonHeader* skelHeader = (SkeletonHeader*)GetResourceDataByName(pathStr.c_str(), false);

    // If there isn't an alternate model, load the regular one
    if (isAlt && skelHeader == NULL) {
        skelHeader = (SkeletonHeader*)GetResourceDataByName(path, false);
    }

    // This function is only called when a skeleton is initialized.
    // Therefore we can take this oppurtunity to take note of the Skeleton that is created...
    if (skelAnime != nullptr) {
        auto stringPath = std::string(path);
        Ship::SkeletonPatcher::RegisterSkeleton(stringPath, skelAnime);
    }

    return skelHeader;
}

extern "C" void ResourceMgr_UnregisterSkeleton(SkelAnime* skelAnime) {
    if (skelAnime != nullptr)
        Ship::SkeletonPatcher::UnregisterSkeleton(skelAnime);
}

extern "C" void ResourceMgr_ClearSkeletons(SkelAnime* skelAnime) {
    if (skelAnime != nullptr)
        Ship::SkeletonPatcher::ClearSkeletons();
}

extern "C" s32* ResourceMgr_LoadCSByName(const char* path) {
    return (s32*)GetResourceDataByNameHandlingMQ(path);
}

std::filesystem::path GetSaveFile(std::shared_ptr<Mercury> Conf) {
    const std::string fileName = Conf->getString("Game.SaveName", Ship::Window::GetPathRelativeToAppDirectory("oot_save.sav"));
    std::filesystem::path saveFile = std::filesystem::absolute(fileName);

    if (!exists(saveFile.parent_path())) {
        create_directories(saveFile.parent_path());
    }

    return saveFile;
}

std::filesystem::path GetSaveFile() {
    const std::shared_ptr<Mercury> pConf = OTRGlobals::Instance->context->GetConfig();

    return GetSaveFile(pConf);
}

void OTRGlobals::CheckSaveFile(size_t sramSize) const {
    const std::shared_ptr<Mercury> pConf = Instance->context->GetConfig();

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

extern "C" void Ctx_ReadSaveFile(uintptr_t addr, void* dramAddr, size_t size) {
    SaveManager::ReadSaveFile(GetSaveFile(), addr, dramAddr, size);
}

extern "C" void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size) {
    SaveManager::WriteSaveFile(GetSaveFile(), addr, dramAddr, size);
}

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
        // Prevent potential horrible overflow due to implicit conversion of maxBufferSize to an unsigned. Prevents negatives.
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
        unsigned char convt = ' ';
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

extern "C" uint32_t OTRGetCurrentWidth() {
    return OTRGlobals::Instance->context->GetCurrentWidth();
}

extern "C" uint32_t OTRGetCurrentHeight() {
    return OTRGlobals::Instance->context->GetCurrentHeight();
}

extern "C" void OTRControllerCallback(uint8_t rumble, uint8_t ledColor) {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    for (int i = 0; i < controlDeck->GetNumConnectedPorts(); ++i) {
        auto physicalDevice = controlDeck->GetDeviceFromPortIndex(i);
        switch (ledColor) {
            case 0:
                physicalDevice->SetLed(i, 255, 0, 0);
                break;
            case 1:
                physicalDevice->SetLed(i, 0x1E, 0x69, 0x1B);
                break;
            case 2:
                physicalDevice->SetLed(i, 0x64, 0x14, 0x00);
                break;
            case 3:
                physicalDevice->SetLed(i, 0x00, 0x3C, 0x64);
                break;
        }

        physicalDevice->SetRumble(i, rumble);
    }
}

extern "C" float OTRGetAspectRatio() {
    return gfx_current_dimensions.aspect_ratio;
}

extern "C" float OTRGetDimensionFromLeftEdge(float v) {
    return (SCREEN_WIDTH / 2 - SCREEN_HEIGHT / 2 * OTRGetAspectRatio() + (v));
}

extern "C" float OTRGetDimensionFromRightEdge(float v) {
    return (SCREEN_WIDTH / 2 + SCREEN_HEIGHT / 2 * OTRGetAspectRatio() - (SCREEN_WIDTH - v));
}

f32 floorf(f32 x);
f32 ceilf(f32 x);

extern "C" int16_t OTRGetRectDimensionFromLeftEdge(float v) {
    return ((int)floorf(OTRGetDimensionFromLeftEdge(v)));
}

extern "C" int16_t OTRGetRectDimensionFromRightEdge(float v) {
    return ((int)ceilf(OTRGetDimensionFromRightEdge(v)));
}

extern "C" bool AudioPlayer_Init(void) {
    return AudioPlayerInit();
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
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();
    
    if (slot < controlDeck->GetNumConnectedPorts()) {
        auto physicalDevice = controlDeck->GetDeviceFromPortIndex(slot);
        
        if (physicalDevice->GetProfile(slot)->UseRumble && physicalDevice->CanRumble()) {
            return 1;
        }
    }

    return 0;
}

extern "C" void Controller_BlockGameInput() {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    controlDeck->BlockGameInput();
}

extern "C" void Controller_UnblockGameInput() {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    controlDeck->UnblockGameInput();
}

extern "C" void Hooks_ExecuteAudioInit() {
    Ship::ExecuteHooks<Ship::AudioInit>();
}

extern "C" void* getN64WeirdFrame(s32 i) {
    char* weirdFrameBytes = reinterpret_cast<char*>(n64WeirdFrames);
    return &weirdFrameBytes[i + sizeof(n64WeirdFrames)];
}

extern "C" int GetEquipNowMessage(char* buffer, char* src, const int maxBufferSize) {
    std::string postfix;

    if (gSaveContext.language == LANGUAGE_FRA) {
        postfix = "\x04\x1A\x08" "D\x96sirez-vous l'\x96quiper maintenant?" "\x09&&"
                  "\x1B%g" "Oui" "&"
                           "Non" "%w\x02";
    } else if (gSaveContext.language == LANGUAGE_GER) {
        postfix = "\x04\x1A\x08" "M""\x9A""chtest Du es jetzt ausr\x9Esten?" "\x09&&"
                  "\x1B%g" "Ja!" "&"
                           "Nein!" "%w\x02";
    } else {
        postfix = "\x04\x1A\x08" "Would you like to equip it now?" "\x09&&"
                  "\x1B%g" "Yes" "&"
                           "No" "%w\x02";
    }
    CustomMessageManager::Instance->FormatCustomMessage(postfix);
    std::string str;
    std::string FixedBaseStr(src);
    int RemoveControlChar = FixedBaseStr.find_first_of("\x02");

    if (RemoveControlChar != std::string::npos) {
        FixedBaseStr = FixedBaseStr.substr(0, RemoveControlChar);
    }
    str = FixedBaseStr + postfix;

    if (!str.empty()) {
        memset(buffer, 0, maxBufferSize);
        const int copiedCharLen = std::min<int>(maxBufferSize - 1, str.length());
        memcpy(buffer, str.c_str(), copiedCharLen);
        return copiedCharLen;
    }
    return 0;
}

extern "C" void Randomizer_LoadSettings(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadRandomizerSettings(spoilerFileName);
}

extern "C" void Randomizer_LoadHintLocations(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadHintLocations(spoilerFileName);
}

extern "C" void Randomizer_LoadMerchantMessages(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadMerchantMessages(spoilerFileName);
}

extern "C" void Randomizer_LoadRequiredTrials(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadRequiredTrials(spoilerFileName);
}

extern "C" void Randomizer_LoadMasterQuestDungeons(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadMasterQuestDungeons(spoilerFileName);
}

extern "C" void Randomizer_LoadItemLocations(const char* spoilerFileName, bool silent) {
    OTRGlobals::Instance->gRandomizer->LoadItemLocations(spoilerFileName, silent);
}

extern "C" bool Randomizer_IsTrialRequired(RandomizerInf trial) {
    return OTRGlobals::Instance->gRandomizer->IsTrialRequired(trial);
}

extern "C" void Randomizer_LoadEntranceOverrides(const char* spoilerFileName, bool silent) {
    OTRGlobals::Instance->gRandomizer->LoadEntranceOverrides(spoilerFileName, silent);
}

extern "C" u32 SpoilerFileExists(const char* spoilerFileName) {
    return OTRGlobals::Instance->gRandomizer->SpoilerFileExists(spoilerFileName);
}

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey) {
    return OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(randoSettingKey);
}

extern "C" RandomizerCheck Randomizer_GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actorId, sceneNum, actorParams);
}

extern "C" ScrubIdentity Randomizer_IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData) {
    return OTRGlobals::Instance->gRandomizer->IdentifyScrub(sceneNum, actorParams, respawnData);
}

extern "C" ShopItemIdentity Randomizer_IdentifyShopItem(s32 sceneNum, u8 slotIndex) {
    return OTRGlobals::Instance->gRandomizer->IdentifyShopItem(sceneNum, slotIndex);
}

extern "C" CowIdentity Randomizer_IdentifyCow(s32 sceneNum, s32 posX, s32 posZ) {
    return OTRGlobals::Instance->gRandomizer->IdentifyCow(sceneNum, posX, posZ);
}

extern "C" GetItemEntry ItemTable_Retrieve(int16_t getItemID) {
    GetItemEntry giEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, getItemID);
    return giEntry;
}

extern "C" GetItemEntry ItemTable_RetrieveEntry(s16 tableID, s16 getItemID) {
    return ItemTableManager::Instance->RetrieveItemEntry(tableID, getItemID);
}

extern "C" GetItemEntry Randomizer_GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId) {
    return OTRGlobals::Instance->gRandomizer->GetItemFromActor(actorId, sceneNum, actorParams, ogId);
}

extern "C" GetItemEntry Randomizer_GetItemFromActorWithoutObtainabilityCheck(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId) {
    return OTRGlobals::Instance->gRandomizer->GetItemFromActor(actorId, sceneNum, actorParams, ogId, false);
}

extern "C" GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(randomizerCheck, ogId);
}

extern "C" GetItemEntry Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(randomizerCheck, ogId, false);
}

extern "C" ItemObtainability Randomizer_GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck) {
    return OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerCheck(randomizerCheck);
}

extern "C" CustomMessageEntry Randomizer_GetCustomGetItemMessage(Player* player) {
    s16 giid;
    if (player->getItemEntry.objectId != OBJECT_INVALID) {
        giid = player->getItemEntry.getItemId;
    } else {
        giid = player->getItemId;
    }
    const CustomMessageEntry getItemText = CustomMessageManager::Instance->RetrieveMessage(Randomizer::getItemMessageTableID, giid);
    return getItemText;
}

extern "C" int CustomMessage_RetrieveIfExists(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    uint16_t textId = msgCtx->textId;
    Font* font = &msgCtx->font;
    char* buffer = font->msgBuf;
    const int maxBufferSize = sizeof(font->msgBuf);
    CustomMessageEntry messageEntry;
    s16 actorParams = 0;
    if (gSaveContext.n64ddFlag) {
        if (textId == TEXT_RANDOMIZER_CUSTOM_ITEM) {
            Player* player = GET_PLAYER(play);
            if (player->getItemEntry.getItemId == RG_ICE_TRAP) {
                u16 iceTrapTextId = Random(0, NUM_ICE_TRAP_MESSAGES);
                messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::IceTrapRandoMessageTableID, iceTrapTextId);
                if (CVarGetInteger("gLetItSnow", 0)) {
                    messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::IceTrapRandoMessageTableID, NUM_ICE_TRAP_MESSAGES + 1);
                }
            } else if (player->getItemEntry.getItemId >= RG_DEKU_TREE_MAP && player->getItemEntry.getItemId <= RG_ICE_CAVERN_MAP) {
                messageEntry = OTRGlobals::Instance->gRandomizer->GetMapGetItemMessageWithHint(player->getItemEntry);
            } else {
                messageEntry = Randomizer_GetCustomGetItemMessage(player);
            }
        } else if (textId == TEXT_RANDOMIZER_GOSSIP_STONE_HINTS && Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) != RO_GOSSIP_STONES_NONE &&
            (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == RO_GOSSIP_STONES_NEED_NOTHING ||
             (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == RO_GOSSIP_STONES_NEED_TRUTH &&
              Player_GetMask(play) == PLAYER_MASK_TRUTH) ||
             (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == RO_GOSSIP_STONES_NEED_STONE && CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)))) {

            Actor* stone = GET_PLAYER(play)->targetActor; 
            actorParams = stone->params;

            // if we're in a generic grotto
            if (play->sceneNum == 62 && actorParams == 14360) {
                // look for the chest in the actorlist to determine
                // which grotto we're in
                int numOfActorLists =
                    sizeof(play->actorCtx.actorLists) / sizeof(play->actorCtx.actorLists[0]);
                for (int i = 0; i < numOfActorLists; i++) {
                    if (play->actorCtx.actorLists[i].length) {
                        if (play->actorCtx.actorLists[i].head->id == 10) {
                            // set the params for the hint check to be negative chest params
                            actorParams = 0 - play->actorCtx.actorLists[i].head->params;
                        }
                    }
                }
            }

            RandomizerCheck hintCheck =
                Randomizer_GetCheckFromActor(stone->id, play->sceneNum, actorParams);

            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, hintCheck);
        } else if ((textId == TEXT_ALTAR_CHILD || textId == TEXT_ALTAR_ADULT)) {
            // rando hints at altar
            messageEntry = (LINK_IS_ADULT)
               ? CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT)
               : CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD);
        } else if (textId == TEXT_GANONDORF) {
            if (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT || !Randomizer_GetSettingValue(RSK_GANONDORF_LIGHT_ARROWS_HINT)) {
                messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT);
            } else {
                messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_GANONDORF);
            }
        // textId: TEXT_SCRUB_RANDOM + (randomizerInf - RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT)
        } else if (textId >= TEXT_SCRUB_RANDOM && textId <= TEXT_SCRUB_RANDOM + NUM_SCRUBS) {
            RandomizerInf randoInf = (RandomizerInf)((textId - TEXT_SCRUB_RANDOM) + RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT);
            messageEntry = OTRGlobals::Instance->gRandomizer->GetMerchantMessage(randoInf, TEXT_SCRUB_RANDOM, Randomizer_GetSettingValue(RSK_SCRUB_TEXT_HINT) == RO_GENERIC_OFF);
        // Shop items each have two message entries, second one offset by NUM_SHOP_ITEMS
        // textId: TEXT_SHOP_ITEM_RANDOM + (randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1)
        // textId: TEXT_SHOP_ITEM_RANDOM + ((randomizerInf - RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1) + NUM_SHOP_ITEMS)
        } else if (textId >= TEXT_SHOP_ITEM_RANDOM && textId <= TEXT_SHOP_ITEM_RANDOM + (NUM_SHOP_ITEMS * 2)) {
            if (textId < TEXT_SHOP_ITEM_RANDOM + NUM_SHOP_ITEMS) {
                RandomizerInf randoInf = (RandomizerInf)((textId - TEXT_SHOP_ITEM_RANDOM) + RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1);
                messageEntry = OTRGlobals::Instance->gRandomizer->GetMerchantMessage(randoInf, TEXT_SHOP_ITEM_RANDOM);
            } else {
                RandomizerInf randoInf = (RandomizerInf)((textId - (TEXT_SHOP_ITEM_RANDOM + NUM_SHOP_ITEMS)) + RAND_INF_SHOP_ITEMS_KF_SHOP_ITEM_1);
                messageEntry = OTRGlobals::Instance->gRandomizer->GetMerchantMessage(randoInf, TEXT_SHOP_ITEM_RANDOM_CONFIRM);
            }
        } else if (CVarGetInteger("gRandomizeRupeeNames", 0) &&
                   (textId == TEXT_BLUE_RUPEE || textId == TEXT_RED_RUPEE || textId == TEXT_PURPLE_RUPEE ||
                   textId == TEXT_HUGE_RUPEE)) {
            messageEntry = Randomizer::GetRupeeMessage(textId);
            // In rando, replace Navi's general overworld hints with rando-related gameplay tips
        } else if (CVarGetInteger("gRandoRelevantNavi", 1) && textId >= 0x0140 && textId <= 0x015F) {
            u16 naviTextId = Random(0, NUM_NAVI_MESSAGES);
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::NaviRandoMessageTableID, naviTextId);
        } else if (Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS) && textId == TEXT_BEAN_SALESMAN) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, TEXT_BEAN_SALESMAN);
        } else if (Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF && (textId == TEXT_MEDIGORON ||
          (textId == TEXT_GRANNYS_SHOP && !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
                    (Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE) || INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK)) ||
          (textId == TEXT_CARPET_SALESMAN_1 && !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)) ||
          (textId == TEXT_CARPET_SALESMAN_2 && !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN)))) {
            RandomizerInf randoInf;
            if (textId == TEXT_MEDIGORON) {
                randoInf = RAND_INF_MERCHANTS_MEDIGORON;
            } else if (textId == TEXT_GRANNYS_SHOP) {
                randoInf = RAND_INF_MERCHANTS_GRANNYS_SHOP;
            } else {
                randoInf = RAND_INF_MERCHANTS_CARPET_SALESMAN;
            }
            messageEntry = OTRGlobals::Instance->gRandomizer->GetMerchantMessage(randoInf, textId, Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_ON_HINT);
        } else if (Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC) &&
                   (textId == TEXT_BUY_BOMBCHU_10_DESC || textId == TEXT_BUY_BOMBCHU_10_PROMPT)) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, textId);
        } else if (textId == TEXT_CURSED_SKULLTULA_PEOPLE) {
            actorParams = GET_PLAYER(play)->targetActor->params;
            RandomizerSettingKey rsk = (RandomizerSettingKey)(RSK_KAK_10_SKULLS_HINT + (actorParams - 1));
            if (Randomizer_GetSettingValue(rsk)) {
                messageEntry = OTRGlobals::Instance->gRandomizer->GetCursedSkullMessage(actorParams);
            }
        } else if (Randomizer_GetSettingValue(RSK_DAMPES_DIARY_HINT) && textId == TEXT_DAMPES_DIARY) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::randoMiscHintsTableID, TEXT_DAMPES_DIARY);
        } else if (play->sceneNum == SCENE_TAKARAYA &&
                   Randomizer_GetSettingValue(RSK_GREG_HINT) &&
                   (textId == 0x704C || textId == 0x6E || textId == 0x84)) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::randoMiscHintsTableID, TEXT_CHEST_GAME_PROCEED);
        } else if (Randomizer_GetSettingValue(RSK_SHUFFLE_WARP_SONGS) &&
                   (textId >= TEXT_WARP_MINUET_OF_FOREST && textId <= TEXT_WARP_PRELUDE_OF_LIGHT)) {
            messageEntry = OTRGlobals::Instance->gRandomizer->GetWarpSongMessage(textId, Randomizer_GetSettingValue(RSK_WARP_SONG_HINTS) == RO_GENERIC_ON);
        } else if (textId == TEXT_LAKE_HYLIA_WATER_SWITCH_NAVI || textId == TEXT_LAKE_HYLIA_WATER_SWITCH_SIGN) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, textId);
        } else if (textId == 0x3052 || (textId >= 0x3069 && textId <= 0x3070)) { //Fire Temple gorons
            u16 choice = Random(0, NUM_GORON_MESSAGES);
            messageEntry = OTRGlobals::Instance->gRandomizer->GetGoronMessage(choice);
        }
    }
    if (textId == TEXT_GS_NO_FREEZE || textId == TEXT_GS_FREEZE) {
        if (CVarGetInteger("gInjectItemCounts", 0) != 0) {
            // The freeze text cannot be manually dismissed and must be auto-dismissed.
            // This is fine and even wanted when skull tokens are not shuffled, but when
            // when they are shuffled we don't want to be able to manually dismiss the box.
            // Otherwise if we get a token from a chest or an NPC we get stuck in the ItemGet
            // animation until the text box auto-dismisses.
            // RANDOTODO: Implement a way to determine if an item came from a skulltula and
            // inject the auto-dismiss control code if it did.
            if (CVarGetInteger("gSkulltulaFreeze", 0) != 0 &&
                !(gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_TOKENS) != RO_TOKENSANITY_OFF)) {
                textId = TEXT_GS_NO_FREEZE;
            } else {
                textId = TEXT_GS_FREEZE;
            }
            // In vanilla, GS token count is incremented prior to the text box displaying
            // In rando we need to bump the token count by one to show the correct count
            s16 gsCount = gSaveContext.inventory.gsTokens + (gSaveContext.n64ddFlag ? 1 : 0);
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, textId);
            CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{gsCount}}", std::to_string(gsCount));
        }
    }
    if (textId == TEXT_HEART_CONTAINER && CVarGetInteger("gInjectItemCounts", 0)) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, TEXT_HEART_CONTAINER);
        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{heartContainerCount}}", std::to_string(gSaveContext.sohStats.heartContainers + 1));
    }
    if (textId == TEXT_HEART_PIECE && CVarGetInteger("gInjectItemCounts", 0)) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, TEXT_HEART_PIECE);
        CustomMessageManager::ReplaceStringInMessage(messageEntry, "{{heartPieceCount}}", std::to_string(gSaveContext.sohStats.heartPieces + 1));
    }
    if (textId == TEXT_MARKET_GUARD_NIGHT && CVarGetInteger("gMarketSneak", 0) && play->sceneNum == SCENE_ENTRA_N) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, TEXT_MARKET_GUARD_NIGHT);
    }
    if (textId == TEXT_RANDO_SAVE_VERSION_WARNING) {
        messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, TEXT_RANDO_SAVE_VERSION_WARNING);
    }
    if (messageEntry.textBoxType != -1) {
        font->charTexBuf[0] = (messageEntry.textBoxType << 4) | messageEntry.textBoxPos;
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                return msgCtx->msgLength = font->msgLength =
                           CopyStringToCharBuffer(messageEntry.french, buffer, maxBufferSize);
            case LANGUAGE_GER:
                return msgCtx->msgLength = font->msgLength =
                           CopyStringToCharBuffer(messageEntry.german, buffer, maxBufferSize);
            case LANGUAGE_ENG:
            default:
                return msgCtx->msgLength = font->msgLength =
                           CopyStringToCharBuffer(messageEntry.english, buffer, maxBufferSize);
        }
    }
    return false;
}

extern "C" void Overlay_DisplayText(float duration, const char* text) {
    Ship::GetGameOverlay()->TextDrawNotification(duration, true, text);
}

extern "C" void Overlay_DisplayText_Seconds(int seconds, const char* text) {
    float duration = seconds * OTRGlobals::Instance->GetInterpolationFPS() * 0.05;
    Ship::GetGameOverlay()->TextDrawNotification(duration, true, text);
}

extern "C" void Entrance_ClearEntranceTrackingData(void) {
    ClearEntranceTrackingData();
}

extern "C" void Entrance_InitEntranceTrackingData(void) {
    InitEntranceTrackingData();
}

extern "C" void EntranceTracker_SetCurrentGrottoID(s16 entranceIndex) {
    SetCurrentGrottoIDForTracker(entranceIndex);
}

extern "C" void EntranceTracker_SetLastEntranceOverride(s16 entranceIndex) {
    SetLastEntranceOverrideForTracker(entranceIndex);
}

extern "C" void Gfx_RegisterBlendedTexture(const char* name, u8* mask, u8* replacement) {
    gfx_register_blended_texture(name, mask, replacement);
}

extern "C" void SaveManager_ThreadPoolWait() {
    SaveManager::Instance->ThreadPoolWait();
}
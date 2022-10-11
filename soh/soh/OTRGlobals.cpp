#include "OTRGlobals.h"
#include "OTRAudio.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include <libultraship/ResourceMgr.h>
#include <libultraship/DisplayList.h>
#include <libultraship/PlayerAnimation.h>
#include <libultraship/Skeleton.h>
#include <libultraship/Window.h>
#include <libultraship/GameVersions.h>

#include "z64animation.h"
#include "z64bgcheck.h"
#include "Enhancements/gameconsole.h"
#include <ultra64/gbi.h>
#include <libultraship/Animation.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif
#include <libultraship/CollisionHeader.h>
#include <libultraship/Array.h>
#include <libultraship/Cutscene.h>
#include <stb/stb_image.h>
#define DRMP3_IMPLEMENTATION
#include <dr_libs/mp3.h>
#define DRWAV_IMPLEMENTATION
#include <dr_libs/wav.h>
#include <libultraship/AudioPlayer.h>
#include "Enhancements/controls/GameControlEditor.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugconsole.h"
#include "Enhancements/debugger/debugger.h"
#include "Enhancements/randomizer/randomizer.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/n64_weird_frame_data.inc"
#include "frame_interpolation.h"
#include "variables.h"
#include "macros.h"
#include <Utils/StringHelper.h>
#include <libultraship/Hooks.h>
#include "Enhancements/custom-message/CustomMessageManager.h"

#include <Fast3D/gfx_pc.h>
#include <Fast3D/gfx_rendering_api.h>

#ifdef __APPLE__
#include <SDL_scancode.h>
#else
#include <SDL2/SDL_scancode.h>
#endif

#ifdef __SWITCH__
#include <libultraship/SwitchImpl.h>
#elif defined(__WIIU__)
#include <libultraship/WiiUImpl.h>
#endif

#include <libultraship/Audio.h>
#include "Enhancements/custom-message/CustomMessageTypes.h"
#include <functions.h>
#include "Enhancements/item-tables/ItemTableManager.h"
#include "GameMenuBar.hpp"

#ifdef ENABLE_CROWD_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
CrowdControl* CrowdControl::Instance;
#endif

OTRGlobals* OTRGlobals::Instance;
SaveManager* SaveManager::Instance;
CustomMessageManager* CustomMessageManager::Instance;
ItemTableManager* ItemTableManager::Instance;

OTRGlobals::OTRGlobals() {
    context = Ship::Window::CreateInstance("Ship of Harkinian");
    gSaveStateMgr = std::make_shared<SaveStateMgr>();
    gRandomizer = std::make_shared<Randomizer>();
}

OTRGlobals::~OTRGlobals() {
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
extern "C" void ResourceMgr_CacheDirectory(const char* resName);
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
    ResourceMgr_CacheDirectory("audio");

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
        GET_ITEM(ITEM_BOMBS_5, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBS_5),
        GET_ITEM(ITEM_NUTS_5, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT, MOD_NONE, GI_NUTS_5),
        GET_ITEM(ITEM_BOMBCHU, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBCHUS_10),
        GET_ITEM(ITEM_BOW, OBJECT_GI_BOW, GID_BOW, 0x31, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOW),
        GET_ITEM(ITEM_SLINGSHOT, OBJECT_GI_PACHINKO, GID_SLINGSHOT, 0x30, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SLINGSHOT),
        GET_ITEM(ITEM_BOOMERANG, OBJECT_GI_BOOMERANG, GID_BOOMERANG, 0x35, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOOMERANG),
        GET_ITEM(ITEM_STICK, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT, MOD_NONE, GI_STICKS_1),
        GET_ITEM(ITEM_HOOKSHOT, OBJECT_GI_HOOKSHOT, GID_HOOKSHOT, 0x36, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HOOKSHOT),
        GET_ITEM(ITEM_LONGSHOT, OBJECT_GI_HOOKSHOT, GID_LONGSHOT, 0x4F, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_LONGSHOT),
        GET_ITEM(ITEM_LENS, OBJECT_GI_GLASSES, GID_LENS, 0x39, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_LENS),
        GET_ITEM(ITEM_LETTER_ZELDA, OBJECT_GI_LETTER, GID_LETTER_ZELDA, 0x69, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_LETTER_ZELDA),
        GET_ITEM(ITEM_OCARINA_TIME, OBJECT_GI_OCARINA, GID_OCARINA_TIME, 0x3A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_OCARINA_OOT),
        GET_ITEM(ITEM_HAMMER, OBJECT_GI_HAMMER, GID_HAMMER, 0x38, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HAMMER),
        GET_ITEM(ITEM_COJIRO, OBJECT_GI_NIWATORI, GID_COJIRO, 0x02, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_COJIRO),
        GET_ITEM(ITEM_BOTTLE, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x42, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOTTLE),
        GET_ITEM(ITEM_POTION_RED, OBJECT_GI_LIQUID, GID_POTION_RED, 0x43, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POTION_RED),
        GET_ITEM(ITEM_POTION_GREEN, OBJECT_GI_LIQUID, GID_POTION_GREEN, 0x44, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POTION_GREEN),
        GET_ITEM(ITEM_POTION_BLUE, OBJECT_GI_LIQUID, GID_POTION_BLUE, 0x45, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POTION_BLUE),
        GET_ITEM(ITEM_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, 0x46, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_FAIRY),
        GET_ITEM(ITEM_MILK_BOTTLE, OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MILK_BOTTLE),
        GET_ITEM(ITEM_LETTER_RUTO, OBJECT_GI_BOTTLE_LETTER, GID_LETTER_RUTO, 0x99, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_LETTER_RUTO),
        GET_ITEM(ITEM_BEAN, OBJECT_GI_BEAN, GID_BEAN, 0x48, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_BEAN),
        GET_ITEM(ITEM_MASK_SKULL, OBJECT_GI_SKJ_MASK, GID_MASK_SKULL, 0x10, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_SKULL),
        GET_ITEM(ITEM_MASK_SPOOKY, OBJECT_GI_REDEAD_MASK, GID_MASK_SPOOKY, 0x11, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_SPOOKY),
        GET_ITEM(ITEM_CHICKEN, OBJECT_GI_NIWATORI, GID_CHICKEN, 0x48, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_CHICKEN),
        GET_ITEM(ITEM_MASK_KEATON, OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, 0x12, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_KEATON),
        GET_ITEM(ITEM_MASK_BUNNY, OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY, 0x13, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_BUNNY),
        GET_ITEM(ITEM_MASK_TRUTH, OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, 0x17, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_TRUTH),
        GET_ITEM(ITEM_POCKET_EGG, OBJECT_GI_EGG, GID_EGG, 0x01, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POCKET_EGG),
        GET_ITEM(ITEM_POCKET_CUCCO, OBJECT_GI_NIWATORI, GID_CHICKEN, 0x48, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POCKET_CUCCO),
        GET_ITEM(ITEM_ODD_MUSHROOM, OBJECT_GI_MUSHROOM, GID_ODD_MUSHROOM, 0x03, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_ODD_MUSHROOM),
        GET_ITEM(ITEM_ODD_POTION, OBJECT_GI_POWDER, GID_ODD_POTION, 0x04, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_ODD_POTION),
        GET_ITEM(ITEM_SAW, OBJECT_GI_SAW, GID_SAW, 0x05, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SAW),
        GET_ITEM(ITEM_SWORD_BROKEN, OBJECT_GI_BROKENSWORD, GID_SWORD_BROKEN, 0x08, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SWORD_BROKEN),
        GET_ITEM(ITEM_PRESCRIPTION, OBJECT_GI_PRESCRIPTION, GID_PRESCRIPTION, 0x09, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_PRESCRIPTION),
        GET_ITEM(ITEM_FROG, OBJECT_GI_FROG, GID_FROG, 0x0D, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_FROG),
        GET_ITEM(ITEM_EYEDROPS, OBJECT_GI_EYE_LOTION, GID_EYEDROPS, 0x0E, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_EYEDROPS),
        GET_ITEM(ITEM_CLAIM_CHECK, OBJECT_GI_TICKETSTONE, GID_CLAIM_CHECK, 0x0A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_CLAIM_CHECK),
        GET_ITEM(ITEM_SWORD_KOKIRI, OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, 0xA4, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SWORD_KOKIRI),
        GET_ITEM(ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD, GID_SWORD_BGS, 0x4B, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SWORD_KNIFE),
        GET_ITEM(ITEM_SHIELD_DEKU, OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU, 0x4C, 0xA0, CHEST_ANIM_SHORT, MOD_NONE, GI_SHIELD_DEKU),
        GET_ITEM(ITEM_SHIELD_HYLIAN, OBJECT_GI_SHIELD_2, GID_SHIELD_HYLIAN, 0x4D, 0xA0, CHEST_ANIM_SHORT, MOD_NONE, GI_SHIELD_HYLIAN),
        GET_ITEM(ITEM_SHIELD_MIRROR, OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, 0x4E, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SHIELD_MIRROR),
        GET_ITEM(ITEM_TUNIC_GORON, OBJECT_GI_CLOTHES, GID_TUNIC_GORON, 0x50, 0xA0, CHEST_ANIM_LONG, MOD_NONE, GI_TUNIC_GORON),
        GET_ITEM(ITEM_TUNIC_ZORA, OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, 0x51, 0xA0, CHEST_ANIM_LONG, MOD_NONE, GI_TUNIC_ZORA),
        GET_ITEM(ITEM_BOOTS_IRON, OBJECT_GI_BOOTS_2, GID_BOOTS_IRON, 0x53, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOOTS_IRON),
        GET_ITEM(ITEM_BOOTS_HOVER, OBJECT_GI_HOVERBOOTS, GID_BOOTS_HOVER, 0x54, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOOTS_HOVER),
        GET_ITEM(ITEM_QUIVER_40, OBJECT_GI_ARROWCASE, GID_QUIVER_40, 0x56, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_QUIVER_40),
        GET_ITEM(ITEM_QUIVER_50, OBJECT_GI_ARROWCASE, GID_QUIVER_50, 0x57, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_QUIVER_50),
        GET_ITEM(ITEM_BOMB_BAG_20, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, 0x58, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOMB_BAG_20),
        GET_ITEM(ITEM_BOMB_BAG_30, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, 0x59, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOMB_BAG_30),
        GET_ITEM(ITEM_BOMB_BAG_40, OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_40, 0x5A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BOMB_BAG_40),
        GET_ITEM(ITEM_GAUNTLETS_SILVER, OBJECT_GI_GLOVES, GID_GAUNTLETS_SILVER, 0x5B, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_GAUNTLETS_SILVER),
        GET_ITEM(ITEM_GAUNTLETS_GOLD, OBJECT_GI_GLOVES, GID_GAUNTLETS_GOLD, 0x5C, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_GAUNTLETS_GOLD),
        GET_ITEM(ITEM_SCALE_SILVER, OBJECT_GI_SCALE, GID_SCALE_SILVER, 0xCD, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SCALE_SILVER),
        GET_ITEM(ITEM_SCALE_GOLDEN, OBJECT_GI_SCALE, GID_SCALE_GOLDEN, 0xCE, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SCALE_GOLD),
        GET_ITEM(ITEM_STONE_OF_AGONY, OBJECT_GI_MAP, GID_STONE_OF_AGONY, 0x68, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_STONE_OF_AGONY),
        GET_ITEM(ITEM_GERUDO_CARD, OBJECT_GI_GERUDO, GID_GERUDO_CARD, 0x7B, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_GERUDO_CARD),
        GET_ITEM(ITEM_OCARINA_FAIRY, OBJECT_GI_OCARINA_0, GID_OCARINA_FAIRY, 0x4A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_OCARINA_FAIRY),
        GET_ITEM(ITEM_SEEDS, OBJECT_GI_SEED, GID_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT, MOD_NONE, GI_SEEDS_5),
        GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HEART_CONTAINER),
        GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xC2, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HEART_PIECE),
        GET_ITEM(ITEM_KEY_BOSS, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, 0xC7, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_KEY_BOSS),
        GET_ITEM(ITEM_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, 0x67, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_COMPASS),
        GET_ITEM(ITEM_DUNGEON_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, 0x66, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MAP),
        GET_ITEM(ITEM_KEY_SMALL, OBJECT_GI_KEY, GID_KEY_SMALL, 0x60, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_KEY_SMALL),
        GET_ITEM(ITEM_MAGIC_SMALL, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL, 0x52, 0x6F, CHEST_ANIM_SHORT, MOD_NONE, GI_MAGIC_SMALL),
        GET_ITEM(ITEM_MAGIC_LARGE, OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE, 0x52, 0x6E, CHEST_ANIM_SHORT, MOD_NONE, GI_MAGIC_LARGE),
        GET_ITEM(ITEM_WALLET_ADULT, OBJECT_GI_PURSE, GID_WALLET_ADULT, 0x5E, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_WALLET_ADULT),
        GET_ITEM(ITEM_WALLET_GIANT, OBJECT_GI_PURSE, GID_WALLET_GIANT, 0x5F, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_WALLET_GIANT),
        GET_ITEM(ITEM_WEIRD_EGG, OBJECT_GI_EGG, GID_EGG, 0x9A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_WEIRD_EGG),
        GET_ITEM(ITEM_HEART, OBJECT_GI_HEART, GID_HEART, 0x55, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HEART),
        GET_ITEM(ITEM_ARROWS_SMALL, OBJECT_GI_ARROW, GID_ARROWS_SMALL, 0xE6, 0x48, CHEST_ANIM_SHORT, MOD_NONE, GI_ARROWS_SMALL),
        GET_ITEM(ITEM_ARROWS_MEDIUM, OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, 0xE6, 0x49, CHEST_ANIM_SHORT, MOD_NONE, GI_ARROWS_MEDIUM),
        GET_ITEM(ITEM_ARROWS_LARGE, OBJECT_GI_ARROW, GID_ARROWS_LARGE, 0xE6, 0x4A, CHEST_ANIM_SHORT, MOD_NONE, GI_ARROWS_LARGE),
        GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0x6F, 0x00, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_GREEN),
        GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xCC, 0x01, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_BLUE),
        GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF0, 0x02, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_RED),
        GET_ITEM(ITEM_HEART_CONTAINER, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xC6, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HEART_CONTAINER_2),
        GET_ITEM(ITEM_MILK, OBJECT_GI_MILK, GID_MILK, 0x98, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MILK),
        GET_ITEM(ITEM_MASK_GORON, OBJECT_GI_GOLONMASK, GID_MASK_GORON, 0x14, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_GORON),
        GET_ITEM(ITEM_MASK_ZORA, OBJECT_GI_ZORAMASK, GID_MASK_ZORA, 0x15, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_ZORA),
        GET_ITEM(ITEM_MASK_GERUDO, OBJECT_GI_GERUDOMASK, GID_MASK_GERUDO, 0x16, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_MASK_GERUDO),
        GET_ITEM(ITEM_BRACELET, OBJECT_GI_BRACELET, GID_BRACELET, 0x79, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BRACELET),
        GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF1, 0x14, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_PURPLE),
        GET_ITEM(ITEM_RUPEE_GOLD, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0xF2, 0x13, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_GOLD),
        GET_ITEM(ITEM_SWORD_BGS, OBJECT_GI_LONGSWORD, GID_SWORD_BGS, 0x0C, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_SWORD_BGS),
        GET_ITEM(ITEM_ARROW_FIRE, OBJECT_GI_M_ARROW, GID_ARROW_FIRE, 0x70, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_ARROW_FIRE),
        GET_ITEM(ITEM_ARROW_ICE, OBJECT_GI_M_ARROW, GID_ARROW_ICE, 0x71, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_ARROW_ICE),
        GET_ITEM(ITEM_ARROW_LIGHT, OBJECT_GI_M_ARROW, GID_ARROW_LIGHT, 0x72, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_ARROW_LIGHT),
        GET_ITEM(ITEM_SKULL_TOKEN, OBJECT_GI_SUTARU, GID_SKULL_TOKEN, 0xB4, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_SKULL_TOKEN),
        GET_ITEM(ITEM_DINS_FIRE, OBJECT_GI_GODDESS, GID_DINS_FIRE, 0xAD, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_DINS_FIRE),
        GET_ITEM(ITEM_FARORES_WIND, OBJECT_GI_GODDESS, GID_FARORES_WIND, 0xAE, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_FARORES_WIND),
        GET_ITEM(ITEM_NAYRUS_LOVE, OBJECT_GI_GODDESS, GID_NAYRUS_LOVE, 0xAF, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_NAYRUS_LOVE),
        GET_ITEM(ITEM_BULLET_BAG_30, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BULLET_BAG_30),
        GET_ITEM(ITEM_BULLET_BAG_40, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG, 0x07, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BULLET_BAG_40),
        GET_ITEM(ITEM_STICKS_5, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT, MOD_NONE, GI_STICKS_5),
        GET_ITEM(ITEM_STICKS_10, OBJECT_GI_STICK, GID_STICK, 0x37, 0x0D, CHEST_ANIM_SHORT, MOD_NONE, GI_STICKS_10),
        GET_ITEM(ITEM_NUTS_5, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT, MOD_NONE, GI_NUTS_5_2),
        GET_ITEM(ITEM_NUTS_10, OBJECT_GI_NUTS, GID_NUTS, 0x34, 0x0C, CHEST_ANIM_SHORT, MOD_NONE, GI_NUTS_10),
        GET_ITEM(ITEM_BOMB, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBS_1),
        GET_ITEM(ITEM_BOMBS_10, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBS_10),
        GET_ITEM(ITEM_BOMBS_20, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBS_20),
        GET_ITEM(ITEM_BOMBS_30, OBJECT_GI_BOMB_1, GID_BOMB, 0x32, 0x59, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBS_30),
        GET_ITEM(ITEM_SEEDS_30, OBJECT_GI_SEED, GID_SEEDS, 0xDC, 0x50, CHEST_ANIM_SHORT, MOD_NONE, GI_SEEDS_30),
        GET_ITEM(ITEM_BOMBCHUS_5, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBCHUS_5),
        GET_ITEM(ITEM_BOMBCHUS_20, OBJECT_GI_BOMB_2, GID_BOMBCHU, 0x33, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_BOMBCHUS_20),
        GET_ITEM(ITEM_FISH, OBJECT_GI_FISH, GID_FISH, 0x47, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_FISH),
        GET_ITEM(ITEM_BUG, OBJECT_GI_INSECT, GID_BUG, 0x7A, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BUGS),
        GET_ITEM(ITEM_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, 0x5D, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BLUE_FIRE),
        GET_ITEM(ITEM_POE, OBJECT_GI_GHOST, GID_POE, 0x97, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_POE),
        GET_ITEM(ITEM_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, 0xF9, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BIG_POE),
        GET_ITEM(ITEM_KEY_SMALL, OBJECT_GI_KEY, GID_KEY_SMALL, 0xF3, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_DOOR_KEY),
        GET_ITEM(ITEM_RUPEE_GREEN, OBJECT_GI_RUPY, GID_RUPEE_GREEN, 0xF4, 0x00, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_GREEN_LOSE),
        GET_ITEM(ITEM_RUPEE_BLUE, OBJECT_GI_RUPY, GID_RUPEE_BLUE, 0xF5, 0x01, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_BLUE_LOSE),
        GET_ITEM(ITEM_RUPEE_RED, OBJECT_GI_RUPY, GID_RUPEE_RED, 0xF6, 0x02, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_RED_LOSE),
        GET_ITEM(ITEM_RUPEE_PURPLE, OBJECT_GI_RUPY, GID_RUPEE_PURPLE, 0xF7, 0x14, CHEST_ANIM_SHORT, MOD_NONE, GI_RUPEE_PURPLE_LOSE),
        GET_ITEM(ITEM_HEART_PIECE_2, OBJECT_GI_HEARTS, GID_HEART_PIECE, 0xFA, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_HEART_PIECE_WIN),
        GET_ITEM(ITEM_STICK_UPGRADE_20, OBJECT_GI_STICK, GID_STICK, 0x90, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_STICK_UPGRADE_20),
        GET_ITEM(ITEM_STICK_UPGRADE_30, OBJECT_GI_STICK, GID_STICK, 0x91, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_STICK_UPGRADE_30),
        GET_ITEM(ITEM_NUT_UPGRADE_30, OBJECT_GI_NUTS, GID_NUTS, 0xA7, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_NUT_UPGRADE_30),
        GET_ITEM(ITEM_NUT_UPGRADE_40, OBJECT_GI_NUTS, GID_NUTS, 0xA8, 0x80, CHEST_ANIM_SHORT, MOD_NONE, GI_NUT_UPGRADE_40),
        GET_ITEM(ITEM_BULLET_BAG_50, OBJECT_GI_DEKUPOUCH, GID_BULLET_BAG_50, 0x6C, 0x80, CHEST_ANIM_LONG, MOD_NONE, GI_BULLET_BAG_50),
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

extern "C" void OTRExtScanner() {
    auto lst = *OTRGlobals::Instance->context->GetResourceManager()->ListFiles("*.*").get();

    for (auto& rPath : lst) {
        std::vector<std::string> raw = StringHelper::Split(rPath, ".");
        std::string ext = raw[raw.size() - 1];
        std::string nPath = rPath.substr(0, rPath.size() - (ext.size() + 1));
        replace(nPath.begin(), nPath.end(), '\\', '/');

        ExtensionCache[nPath] = { rPath, ext };
    }
}

extern "C" void InitOTR() {
#ifdef __SWITCH__
    Ship::Switch::Init(Ship::PreInitPhase);
#elif defined(__WIIU__)
    Ship::WiiU::Init();
#endif
    SohImGui::AddSetupHooksDelegate(GameMenuBar::SetupHooks);
    SohImGui::RegisterMenuDrawMethod(GameMenuBar::Draw);

    OTRGlobals::Instance = new OTRGlobals();
    SaveManager::Instance = new SaveManager();
    CustomMessageManager::Instance = new CustomMessageManager();
    ItemTableManager::Instance = new ItemTableManager();
    auto t = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("version");

    if (!t->bHasLoadError)
    {
        uint32_t gameVersion = LE32SWAP(*((uint32_t*)t->buffer.get()));
        OTRGlobals::Instance->context->GetResourceManager()->SetGameVersion(gameVersion);
    }

    clearMtx = (uintptr_t)&gMtxClear;
    OTRMessage_Init();
    OTRAudio_Init();
    InitCosmeticsEditor();
    GameControlEditor::Init();
    DebugConsole_Init();
    Debug_Init();
    Rando_Init();
    InitItemTracker();
    OTRExtScanner();
    VanillaItemTable_Init();

#ifdef ENABLE_CROWD_CONTROL
    CrowdControl::Instance = new CrowdControl();
    CrowdControl::Instance->InitCrowdControl();
#endif
}

extern "C" void DeinitOTR() {
    OTRAudio_Exit();
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

extern "C" void Graph_StartFrame() {
#ifndef __WIIU__
    // Why -1?
    int32_t dwScancode = OTRGlobals::Instance->context->GetLastScancode();
    OTRGlobals::Instance->context->SetLastScancode(-1);

    switch (dwScancode - 1) {
        case SDL_SCANCODE_F5: {
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
        case SDL_SCANCODE_F6: {
            unsigned int slot = OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
            slot++;
            if (slot > 5) {
                slot = 0;
            }
            OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot);
            SPDLOG_INFO("Set SaveState slot to {}.", slot);
            break;
        }
        case SDL_SCANCODE_F7: {
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
    int target_fps = CVar_GetS32("gInterpolationFPS", 20);
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

    int threshold = CVar_GetS32("gExtraLatencyThreshold", 80);
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

extern "C" uint32_t ResourceMgr_GetGameVersion()
{
    return OTRGlobals::Instance->context->GetResourceManager()->GetGameVersion();
}

extern "C" uint32_t ResourceMgr_IsGameMasterQuest() {
    uint32_t version = OTRGlobals::Instance->context->GetResourceManager()->GetGameVersion();

    switch (version) {
        case OOT_PAL_MQ:
        case OOT_NTSC_JP_MQ:
        case OOT_NTSC_US_MQ:
        case OOT_PAL_GC_MQ_DBG:
            return 1;
        case OOT_NTSC_10:
        case OOT_NTSC_11:
        case OOT_NTSC_12:
        case OOT_PAL_10:
        case OOT_PAL_11:
        case OOT_NTSC_JP_GC_CE:
        case OOT_NTSC_JP_GC:
        case OOT_NTSC_US_GC:
        case OOT_PAL_GC:
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
            return 0;
        default:
            SPDLOG_WARN("Unknown rom detected. Defaulting to Non-mq {:x}", version);
            return 0;

    }
}

extern "C" void ResourceMgr_CacheDirectory(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->CacheDirectory(resName);
}
extern "C" void ResourceMgr_DirtyDirectory(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->DirtyDirectory(resName);
}

extern "C" void ResourceMgr_InvalidateCache() {
    OTRGlobals::Instance->context->GetResourceManager()->InvalidateResourceCache();
}

// OTRTODO: There is probably a more elegant way to go about this...
extern "C" char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize) {
    auto lst = OTRGlobals::Instance->context->GetResourceManager()->ListFiles(searchMask);
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

extern "C" void ResourceMgr_LoadFile(const char* resName) {
    OTRGlobals::Instance->context->GetResourceManager()->LoadResource(resName);
}

extern "C" char* ResourceMgr_LoadFileRaw(const char* resName) {
    return OTRGlobals::Instance->context->GetResourceManager()->LoadFile(resName)->buffer.get();
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

extern "C" char* ResourceMgr_LoadJPEG(char* data, int dataSize)
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

extern "C" char* ResourceMgr_LoadTexByName(const char* texPath);

extern "C" uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);

extern "C" uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);

extern "C" uint32_t ResourceMgr_LoadTexSizeByName(const char* texPath);

extern "C" char* ResourceMgr_LoadTexOrDListByName(const char* filePath) {
    auto res = OTRGlobals::Instance->context->GetResourceManager()->LoadResource(filePath);

    if (res->resType == Ship::ResourceType::DisplayList)
        return (char*)&((std::static_pointer_cast<Ship::DisplayList>(res))->instructions[0]);
    else if (res->resType == Ship::ResourceType::Array)
        return (char*)(std::static_pointer_cast<Ship::Array>(res))->vertices.data();
    else
        return ResourceMgr_LoadTexByName(filePath);
}

extern "C" Sprite* GetSeedTexture(uint8_t index) {
    return Randomizer::GetSeedTexture(index);
}

extern "C" char* ResourceMgr_LoadPlayerAnimByName(const char* animPath) {
    auto anim = std::static_pointer_cast<Ship::PlayerAnimation>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(animPath));

    return (char*)&anim->limbRotData[0];
}

extern "C" Gfx* ResourceMgr_LoadGfxByName(const char* path)
{
    auto res = std::static_pointer_cast<Ship::DisplayList>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));
    return (Gfx*)&res->instructions[0];
}

extern "C" Gfx* ResourceMgr_PatchGfxByName(const char* path, int size) {
    auto res = std::static_pointer_cast<Ship::DisplayList>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));
    res->instructions.resize(res->instructions.size() + size);
    return (Gfx*)&res->instructions[0];
}

extern "C" char* ResourceMgr_LoadArrayByName(const char* path)
{
    auto res = std::static_pointer_cast<Ship::Array>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    return (char*)res->scalars.data();
}

extern "C" char* ResourceMgr_LoadArrayByNameAsVec3s(const char* path) {
    auto res =
        std::static_pointer_cast<Ship::Array>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (res->cachedGameAsset != nullptr)
        return (char*)res->cachedGameAsset;
    else
    {
        Vec3s* data = (Vec3s*)malloc(sizeof(Vec3s) * res->scalars.size());

        for (size_t i = 0; i < res->scalars.size(); i += 3) {
            data[(i / 3)].x = res->scalars[i + 0].s16;
            data[(i / 3)].y = res->scalars[i + 1].s16;
            data[(i / 3)].z = res->scalars[i + 2].s16;
        }

        res->cachedGameAsset = data;

        return (char*)data;
    }
}

extern "C" CollisionHeader* ResourceMgr_LoadColByName(const char* path)
{
    auto colRes = std::static_pointer_cast<Ship::CollisionHeader>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (colRes->cachedGameAsset != nullptr)
        return (CollisionHeader*)colRes->cachedGameAsset;

    CollisionHeader* colHeader = (CollisionHeader*)malloc(sizeof(CollisionHeader));

    colHeader->minBounds.x = colRes->absMinX;
    colHeader->minBounds.y = colRes->absMinY;
    colHeader->minBounds.z = colRes->absMinZ;

    colHeader->maxBounds.x = colRes->absMaxX;
    colHeader->maxBounds.y = colRes->absMaxY;
    colHeader->maxBounds.z = colRes->absMaxZ;

    colHeader->vtxList = (Vec3s*)malloc(sizeof(Vec3s) * colRes->vertices.size());
    colHeader->numVertices = colRes->vertices.size();

    for (size_t i = 0; i < colRes->vertices.size(); i++)
    {
        colHeader->vtxList[i].x = colRes->vertices[i].x;
        colHeader->vtxList[i].y = colRes->vertices[i].y;
        colHeader->vtxList[i].z = colRes->vertices[i].z;
    }

    colHeader->polyList = (CollisionPoly*)malloc(sizeof(CollisionPoly) * colRes->polygons.size());
    colHeader->numPolygons = colRes->polygons.size();

    for (size_t i = 0; i < colRes->polygons.size(); i++)
    {
        colHeader->polyList[i].type = colRes->polygons[i].type;
        colHeader->polyList[i].flags_vIA = colRes->polygons[i].vtxA;
        colHeader->polyList[i].flags_vIB = colRes->polygons[i].vtxB;
        colHeader->polyList[i].vIC = colRes->polygons[i].vtxC;
        colHeader->polyList[i].normal.x = colRes->polygons[i].a;
        colHeader->polyList[i].normal.y = colRes->polygons[i].b;
        colHeader->polyList[i].normal.z = colRes->polygons[i].c;
        colHeader->polyList[i].dist = colRes->polygons[i].d;
    }

    colHeader->surfaceTypeList = (SurfaceType*)malloc(colRes->polygonTypes.size() * sizeof(SurfaceType));

    for (size_t i = 0; i < colRes->polygonTypes.size(); i++)
    {
        colHeader->surfaceTypeList[i].data[0] = colRes->polygonTypes[i] >> 32;
        colHeader->surfaceTypeList[i].data[1] = colRes->polygonTypes[i] & 0xFFFFFFFF;
    }

    colHeader->cameraDataList = (CamData*)malloc(sizeof(CamData) * colRes->camData->entries.size());

    for (size_t i = 0; i < colRes->camData->entries.size(); i++)
    {
        colHeader->cameraDataList[i].cameraSType = colRes->camData->entries[i]->cameraSType;
        colHeader->cameraDataList[i].numCameras = colRes->camData->entries[i]->numData;

        int idx = colRes->camData->entries[i]->cameraPosDataIdx;

        colHeader->cameraDataList[i].camPosData = (Vec3s*)malloc(sizeof(Vec3s));

        if (colRes->camData->cameraPositionData.size() > 0)
        {
            colHeader->cameraDataList[i].camPosData->x = colRes->camData->cameraPositionData[idx]->x;
            colHeader->cameraDataList[i].camPosData->y = colRes->camData->cameraPositionData[idx]->y;
            colHeader->cameraDataList[i].camPosData->z = colRes->camData->cameraPositionData[idx]->z;
        }
        else
        {
            colHeader->cameraDataList[i].camPosData->x = 0;
            colHeader->cameraDataList[i].camPosData->y = 0;
            colHeader->cameraDataList[i].camPosData->z = 0;
        }
    }

    colHeader->numWaterBoxes = colRes->waterBoxes.size();
    colHeader->waterBoxes = (WaterBox*)malloc(sizeof(WaterBox) * colHeader->numWaterBoxes);

    for (int i = 0; i < colHeader->numWaterBoxes; i++)
    {
        colHeader->waterBoxes[i].xLength = colRes->waterBoxes[i].xLength;
        colHeader->waterBoxes[i].ySurface = colRes->waterBoxes[i].ySurface;
        colHeader->waterBoxes[i].xMin = colRes->waterBoxes[i].xMin;
        colHeader->waterBoxes[i].zMin = colRes->waterBoxes[i].zMin;
        colHeader->waterBoxes[i].xLength = colRes->waterBoxes[i].xLength;
        colHeader->waterBoxes[i].zLength = colRes->waterBoxes[i].zLength;
        colHeader->waterBoxes[i].properties = colRes->waterBoxes[i].properties;
    }

    colRes->cachedGameAsset = colHeader;

    return (CollisionHeader*)colHeader;
}

extern "C" Vtx* ResourceMgr_LoadVtxByName(const char* path)
{
	auto res = std::static_pointer_cast<Ship::Array>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));
	return (Vtx*)res->vertices.data();
}

extern "C" SequenceData ResourceMgr_LoadSeqByName(const char* path)
{
    auto file = std::static_pointer_cast<Ship::AudioSequence>(OTRGlobals::Instance->context->GetResourceManager()
                    ->LoadResource(path));

    SequenceData seqData;
    seqData.seqNumber = file->seqNumber;
    seqData.medium = file->medium;
    seqData.cachePolicy = file->cachePolicy;
    seqData.numFonts = file->fonts.size();

    for (int i = 0; i < seqData.numFonts; i++)
        seqData.fonts[i] = file->fonts[i];

    seqData.seqData = file->seqData.data();
    seqData.seqDataSize = file->seqData.size();

    return seqData;
}

std::map<std::string, SoundFontSample*> cachedCustomSFs;

extern "C" SoundFontSample* ReadCustomSample(const char* path) {

    if (!ExtensionCache.contains(path))
        return nullptr;

    ExtensionEntry entry = ExtensionCache[path];

    auto sampleRaw = OTRGlobals::Instance->context->GetResourceManager()->LoadFile(entry.path);
    uint32_t* strem = (uint32_t*)sampleRaw->buffer.get();
    uint8_t* strem2 = (uint8_t*)strem;

    SoundFontSample* sampleC = new SoundFontSample;

    if (entry.ext == "wav") {
        drwav_uint32 channels;
        drwav_uint32 sampleRate;
        drwav_uint64 totalPcm;
        drmp3_int16* pcmData =
            drwav_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->dwBufferSize, &channels, &sampleRate, &totalPcm, NULL);
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
            drmp3_open_memory_and_read_pcm_frames_s16(strem2, sampleRaw->dwBufferSize, &mp3Info, &totalPcm, NULL);

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
}

extern "C" SoundFontSample* ResourceMgr_LoadAudioSample(const char* path)
{
    if (std::string(path) == "")
        return nullptr;

    if (cachedCustomSFs.find(path) != cachedCustomSFs.end())
        return cachedCustomSFs[path];

    SoundFontSample* cSample = ReadCustomSample(path);

    if (cSample != nullptr)
        return cSample;

    auto sample = std::static_pointer_cast<Ship::AudioSample>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (sample == nullptr)
        return NULL;

    if (sample->cachedGameAsset != nullptr)
    {
        SoundFontSample* sampleC = (SoundFontSample*)sample->cachedGameAsset;
        return (SoundFontSample*)sample->cachedGameAsset;
    }
    else
    {
        SoundFontSample* sampleC = new SoundFontSample;

        sampleC->sampleAddr = sample->data.data();

        sampleC->size = sample->data.size();
        sampleC->codec = sample->codec;
        sampleC->medium = sample->medium;
        sampleC->unk_bit26 = sample->unk_bit26;
        sampleC->unk_bit25 = sample->unk_bit25;

        sampleC->book = new AdpcmBook[sample->book.books.size() * sizeof(int16_t)];
        sampleC->book->npredictors = sample->book.npredictors;
        sampleC->book->order = sample->book.order;

        for (size_t i = 0; i < sample->book.books.size(); i++)
            sampleC->book->book[i] = sample->book.books[i];

        sampleC->loop = new AdpcmLoop;
        sampleC->loop->start = sample->loop.start;
        sampleC->loop->end = sample->loop.end;
        sampleC->loop->count = sample->loop.count;

        for (int i = 0; i < 16; i++)
            sampleC->loop->state[i] = 0;

        for (size_t i = 0; i < sample->loop.states.size(); i++)
            sampleC->loop->state[i] = sample->loop.states[i];

        sample->cachedGameAsset = sampleC;
        return sampleC;
    }
}

extern "C" SoundFont* ResourceMgr_LoadAudioSoundFont(const char* path) {
    auto soundFont =
        std::static_pointer_cast<Ship::AudioSoundFont>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (soundFont == nullptr)
        return NULL;

    if (soundFont->cachedGameAsset != nullptr)
    {
        return (SoundFont*)soundFont->cachedGameAsset;
    }
    else
    {
        SoundFont* soundFontC = (SoundFont*)malloc(sizeof(SoundFont));

        soundFontC->fntIndex = soundFont->id;
        soundFontC->numDrums = soundFont->drums.size();
        soundFontC->numInstruments = soundFont->instruments.size();
        soundFontC->numSfx = soundFont->soundEffects.size();
        soundFontC->sampleBankId1 = soundFont->data1 >> 8;
        soundFontC->sampleBankId2 = soundFont->data1 & 0xFF;

        soundFontC->drums = (Drum**)malloc(sizeof(Drum*) * soundFont->drums.size());

        for (size_t i = 0; i < soundFont->drums.size(); i++)
        {
            Drum* drum = (Drum*)malloc(sizeof(Drum));

            drum->releaseRate = soundFont->drums[i].releaseRate;
            drum->pan = soundFont->drums[i].pan;
            drum->loaded = 0;

            if (soundFont->drums[i].env.size() == 0)
                drum->envelope = NULL;
            else
            {
                drum->envelope = (AdsrEnvelope*)malloc(sizeof(AdsrEnvelope) * soundFont->drums[i].env.size());

                for (size_t k = 0; k < soundFont->drums[i].env.size(); k++)
                {
                    drum->envelope[k].delay = BE16SWAP(soundFont->drums[i].env[k]->delay);
                    drum->envelope[k].arg = BE16SWAP(soundFont->drums[i].env[k]->arg);
                }
            }

            drum->sound.sample = ResourceMgr_LoadAudioSample(soundFont->drums[i].sampleFileName.c_str());
            drum->sound.tuning = soundFont->drums[i].tuning;

            soundFontC->drums[i] = drum;
        }

        soundFontC->instruments = (Instrument**)malloc(sizeof(Instrument*) * soundFont->instruments.size());

        for (size_t i = 0; i < soundFont->instruments.size(); i++) {

            if (soundFont->instruments[i].isValidEntry)
            {
                Instrument* inst = (Instrument*)malloc(sizeof(Instrument));

                inst->loaded = 0;
                inst->releaseRate = soundFont->instruments[i].releaseRate;
                inst->normalRangeLo = soundFont->instruments[i].normalRangeLo;
                inst->normalRangeHi = soundFont->instruments[i].normalRangeHi;

                if (soundFont->instruments[i].env.size() == 0)
                    inst->envelope = NULL;
                else
                {
                    inst->envelope = (AdsrEnvelope*)malloc(sizeof(AdsrEnvelope) * soundFont->instruments[i].env.size());

                    for (int k = 0; k < soundFont->instruments[i].env.size(); k++)
                    {
                        inst->envelope[k].delay = BE16SWAP(soundFont->instruments[i].env[k]->delay);
                        inst->envelope[k].arg = BE16SWAP(soundFont->instruments[i].env[k]->arg);
                    }
                }
                if (soundFont->instruments[i].lowNotesSound != nullptr)
                {
                    inst->lowNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].lowNotesSound->sampleFileName.c_str());
                    inst->lowNotesSound.tuning = soundFont->instruments[i].lowNotesSound->tuning;
                } else {
                    inst->lowNotesSound.sample = NULL;
                    inst->lowNotesSound.tuning = 0;
                }

                if (soundFont->instruments[i].normalNotesSound != nullptr) {
                    inst->normalNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].normalNotesSound->sampleFileName.c_str());
                    inst->normalNotesSound.tuning = soundFont->instruments[i].normalNotesSound->tuning;

                } else {
                    inst->normalNotesSound.sample = NULL;
                    inst->normalNotesSound.tuning = 0;
                }

                if (soundFont->instruments[i].highNotesSound != nullptr) {
                    inst->highNotesSound.sample =
                        ResourceMgr_LoadAudioSample(soundFont->instruments[i].highNotesSound->sampleFileName.c_str());
                    inst->highNotesSound.tuning = soundFont->instruments[i].highNotesSound->tuning;
                } else {
                    inst->highNotesSound.sample = NULL;
                    inst->highNotesSound.tuning = 0;
                }

                soundFontC->instruments[i] = inst;
            }
            else
            {
                soundFontC->instruments[i] = nullptr;
            }
        }

        soundFontC->soundEffects = (SoundFontSound*)malloc(sizeof(SoundFontSound) * soundFont->soundEffects.size());

        for (size_t i = 0; i < soundFont->soundEffects.size(); i++)
        {
            soundFontC->soundEffects[i].sample = ResourceMgr_LoadAudioSample(soundFont->soundEffects[i]->sampleFileName.c_str());
            soundFontC->soundEffects[i].tuning = soundFont->soundEffects[i]->tuning;
        }

        soundFont->cachedGameAsset = soundFontC;
        return soundFontC;
    }
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
    auto res = std::static_pointer_cast<Ship::Animation>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (res->cachedGameAsset != nullptr)
        return (AnimationHeaderCommon*)res->cachedGameAsset;

    AnimationHeaderCommon* anim = nullptr;

    if (res->type == Ship::AnimationType::Normal) {
        AnimationHeader* animNormal = (AnimationHeader*)malloc(sizeof(AnimationHeader));

        animNormal->common.frameCount = res->frameCount;
        animNormal->frameData = (int16_t*)malloc(res->rotationValues.size() * sizeof(int16_t));

        for (size_t i = 0; i < res->rotationValues.size(); i++)
            animNormal->frameData[i] = res->rotationValues[i];

        animNormal->jointIndices = (JointIndex*)malloc(res->rotationIndices.size() * sizeof(Vec3s));

        for (size_t i = 0; i < res->rotationIndices.size(); i++) {
            animNormal->jointIndices[i].x = res->rotationIndices[i].x;
            animNormal->jointIndices[i].y = res->rotationIndices[i].y;
            animNormal->jointIndices[i].z = res->rotationIndices[i].z;
        }

        animNormal->staticIndexMax = res->limit;

        anim = (AnimationHeaderCommon*)animNormal;
    }
    else if (res->type == Ship::AnimationType::Curve)
    {
        TransformUpdateIndex* animCurve = (TransformUpdateIndex*)malloc(sizeof(TransformUpdateIndex));

        animCurve->copyValues = (s16*)malloc(res->copyValuesArr.size() * sizeof(s16));

        for (size_t i = 0; i < res->copyValuesArr.size(); i++)
            animCurve->copyValues[i] = res->copyValuesArr[i];

        animCurve->transformData = (TransformData*)malloc(res->transformDataArr.size() * sizeof(TransformData));

        for (size_t i = 0; i < res->transformDataArr.size(); i++)
        {
            animCurve->transformData[i].unk_00 = res->transformDataArr[i].unk_00;
            animCurve->transformData[i].unk_02 = res->transformDataArr[i].unk_02;
            animCurve->transformData[i].unk_04 = res->transformDataArr[i].unk_04;
            animCurve->transformData[i].unk_06 = res->transformDataArr[i].unk_06;
            animCurve->transformData[i].unk_08 = res->transformDataArr[i].unk_08;
        }

        animCurve->refIndex = (u8*)malloc(res->refIndexArr.size());
        for (size_t i = 0; i < res->refIndexArr.size(); i++)
            animCurve->refIndex[i] = res->refIndexArr[i];

        anim = (AnimationHeaderCommon*)animCurve;
    }
    else {
        LinkAnimationHeader* animLink = (LinkAnimationHeader*)malloc(sizeof(LinkAnimationHeader));
        animLink->common.frameCount = res->frameCount;
        animLink->segment = (void*)res->segPtr;

        anim = (AnimationHeaderCommon*)animLink;
    }

    res->cachedGameAsset = anim;

    return anim;
}

extern "C" SkeletonHeader* ResourceMgr_LoadSkeletonByName(const char* path) {
    auto res = std::static_pointer_cast<Ship::Skeleton>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));

    if (res->cachedGameAsset != nullptr)
        return (SkeletonHeader*)res->cachedGameAsset;

    SkeletonHeader* baseHeader = nullptr;

    if (res->type == Ship::SkeletonType::Normal)
    {
        baseHeader = (SkeletonHeader*)malloc(sizeof(SkeletonHeader));
    }
    else if (res->type == Ship::SkeletonType::Curve)
    {
        SkelCurveLimbList* curve = (SkelCurveLimbList*)malloc(sizeof(SkelCurveLimbList));
        curve->limbCount = res->limbCount;
        curve->limbs = (SkelCurveLimb**)malloc(res->limbCount * sizeof(SkelCurveLimb*));
        baseHeader = (SkeletonHeader*)curve;
    }
    else {
        FlexSkeletonHeader* flex = (FlexSkeletonHeader*)malloc(sizeof(FlexSkeletonHeader));
        flex->dListCount = res->dListCount;

        baseHeader = (SkeletonHeader*)flex;
    }

    if (res->type != Ship::SkeletonType::Curve)
    {
        baseHeader->limbCount = res->limbCount;
        baseHeader->segment = (void**)malloc(sizeof(StandardLimb*) * res->limbTable.size());
    }

    for (size_t i = 0; i < res->limbTable.size(); i++) {
        std::string limbStr = res->limbTable[i];
        auto limb = std::static_pointer_cast<Ship::SkeletonLimb>(
            OTRGlobals::Instance->context->GetResourceManager()->LoadResource(limbStr.c_str()));

        if (limb->limbType == Ship::LimbType::LOD) {
            LodLimb* limbC = (LodLimb*)malloc(sizeof(LodLimb));
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;

            if (limb->dListPtr != "") {
                auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dLists[0] = dList;
            } else {
                limbC->dLists[0] = nullptr;
            }

            if (limb->dList2Ptr != "") {
                auto dList = ResourceMgr_LoadGfxByName(limb->dList2Ptr.c_str());
                limbC->dLists[1] = dList;
            } else {
                limbC->dLists[1] = nullptr;
            }

            baseHeader->segment[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Standard)
        {
            const auto limbC = new StandardLimb;
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;
            limbC->dList = nullptr;

            if (!limb->dListPtr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dList = dList;
            }

            baseHeader->segment[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Curve)
        {
            const auto limbC = new SkelCurveLimb;

            limbC->firstChildIdx = limb->childIndex;
            limbC->nextLimbIdx = limb->siblingIndex;
            limbC->dList[0] = nullptr;
            limbC->dList[1] = nullptr;

            if (!limb->dListPtr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dListPtr.c_str());
                limbC->dList[0] = dList;
            }

            if (!limb->dList2Ptr.empty()) {
                const auto dList = ResourceMgr_LoadGfxByName(limb->dList2Ptr.c_str());
                limbC->dList[1] = dList;
            }

            const auto curve = reinterpret_cast<SkelCurveLimbList*>(baseHeader);
            curve->limbs[i] = limbC;
        }
        else if (limb->limbType == Ship::LimbType::Skin)
        {
            const auto limbC = new SkinLimb;
            limbC->jointPos.x = limb->transX;
            limbC->jointPos.y = limb->transY;
            limbC->jointPos.z = limb->transZ;
            limbC->child = limb->childIndex;
            limbC->sibling = limb->siblingIndex;

            if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_DList)
                limbC->segmentType = static_cast<int32_t>(limb->skinSegmentType);
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_4)
                limbC->segmentType = 4;
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_5)
                limbC->segmentType = 5;
            else
                limbC->segmentType = 0;

            if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_DList)
                limbC->segment = ResourceMgr_LoadGfxByName(limb->skinDList.c_str());
            else if (limb->skinSegmentType == Ship::ZLimbSkinType::SkinType_4) {
                const auto animData = new SkinAnimatedLimbData;
                const int skinDataSize = limb->skinData.size();

                animData->totalVtxCount = limb->skinVtxCnt;
                animData->limbModifCount = skinDataSize;
                animData->limbModifications = new SkinLimbModif[animData->limbModifCount];
                animData->dlist = ResourceMgr_LoadGfxByName(limb->skinDList2.c_str());

                for (size_t i = 0; i < skinDataSize; i++)
                {
                    animData->limbModifications[i].vtxCount = limb->skinData[i].unk_8_arr.size();
                    animData->limbModifications[i].transformCount = limb->skinData[i].unk_C_arr.size();
                    animData->limbModifications[i].unk_4 = limb->skinData[i].unk_4;

                    animData->limbModifications[i].skinVertices = new SkinVertex[limb->skinData[i].unk_8_arr.size()];

                    for (int k = 0; k < limb->skinData[i].unk_8_arr.size(); k++)
                    {
                        animData->limbModifications[i].skinVertices[k].index = limb->skinData[i].unk_8_arr[k].unk_0;
                        animData->limbModifications[i].skinVertices[k].s = limb->skinData[i].unk_8_arr[k].unk_2;
                        animData->limbModifications[i].skinVertices[k].t = limb->skinData[i].unk_8_arr[k].unk_4;
                        animData->limbModifications[i].skinVertices[k].normX = limb->skinData[i].unk_8_arr[k].unk_6;
                        animData->limbModifications[i].skinVertices[k].normY = limb->skinData[i].unk_8_arr[k].unk_7;
                        animData->limbModifications[i].skinVertices[k].normZ = limb->skinData[i].unk_8_arr[k].unk_8;
                        animData->limbModifications[i].skinVertices[k].alpha = limb->skinData[i].unk_8_arr[k].unk_9;
                    }

                    animData->limbModifications[i].limbTransformations =
                        new SkinTransformation[limb->skinData[i].unk_C_arr.size()];

                    for (int k = 0; k < limb->skinData[i].unk_C_arr.size(); k++)
                    {
                        animData->limbModifications[i].limbTransformations[k].limbIndex = limb->skinData[i].unk_C_arr[k].unk_0;
                        animData->limbModifications[i].limbTransformations[k].x = limb->skinData[i].unk_C_arr[k].x;
                        animData->limbModifications[i].limbTransformations[k].y = limb->skinData[i].unk_C_arr[k].y;
                        animData->limbModifications[i].limbTransformations[k].z = limb->skinData[i].unk_C_arr[k].z;
                        animData->limbModifications[i].limbTransformations[k].scale = limb->skinData[i].unk_C_arr[k].unk_8;
                    }
                }

                limbC->segment = animData;

                //limbC->segment = nullptr;
            }

            /*if (limb->dListPtr != "") {
                auto dList = ResourceMgr_LoadGfxByName((char*)limb->dListPtr.c_str());
                limbC->unk_8 = dList;
            }
            else {
                limbC->unk_8 = nullptr;
            }*/

            baseHeader->segment[i] = limbC;
        }
        else
        {
            // OTRTODO: Print error here...
        }
    }

    res->cachedGameAsset = baseHeader;

    return baseHeader;
}

extern "C" s32* ResourceMgr_LoadCSByName(const char* path)
{
    auto res = std::static_pointer_cast<Ship::Cutscene>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));
    return (s32*)res->commands.data();
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
    OTRGlobals::Instance->context->ReadSaveFile(GetSaveFile(), addr, dramAddr, size);
}

extern "C" void Ctx_WriteSaveFile(uintptr_t addr, void* dramAddr, size_t size) {
    OTRGlobals::Instance->context->WriteSaveFile(GetSaveFile(), addr, dramAddr, size);
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

extern "C" void OTRControllerCallback(ControllerCallback* controller) {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    for (int i = 0; i < controlDeck->GetNumVirtualDevices(); ++i) {
        auto physicalDevice = controlDeck->GetPhysicalDeviceFromVirtualSlot(i);
        physicalDevice->WriteToSource(i, controller);
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
    if (OTRGlobals::Instance->context->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetAudioPlayer()->Init();
    }

    return false;
}

extern "C" int AudioPlayer_Buffered(void) {
    if (OTRGlobals::Instance->context->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetAudioPlayer()->Buffered();
    }
}

extern "C" int AudioPlayer_GetDesiredBuffered(void) {
    if (OTRGlobals::Instance->context->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetAudioPlayer()->GetDesiredBuffered();
    }
}

extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len) {
    if (OTRGlobals::Instance->context->GetAudioPlayer() != nullptr) {
        OTRGlobals::Instance->context->GetAudioPlayer()->Play(buf, len);
    }
}

extern "C" int Controller_ShouldRumble(size_t i) {
    auto controlDeck = Ship::Window::GetInstance()->GetControlDeck();

    for (int i = 0; i < controlDeck->GetNumVirtualDevices(); ++i) {
        auto physicalDevice = controlDeck->GetPhysicalDeviceFromVirtualSlot(i);
        if (physicalDevice->CanRumble()) {
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

extern "C" void Randomizer_LoadItemLocations(const char* spoilerFileName, bool silent) {
    OTRGlobals::Instance->gRandomizer->LoadItemLocations(spoilerFileName, silent);
}

extern "C" bool Randomizer_IsTrialRequired(RandomizerInf trial) {
    return OTRGlobals::Instance->gRandomizer->IsTrialRequired(trial);
}

extern "C" bool SpoilerFileExists(const char* spoilerFileName) {
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

extern "C" CustomMessageEntry Randomizer_GetNaviMessage() {
    u16 naviTextId = rand() % NUM_NAVI_MESSAGES;
    return CustomMessageManager::Instance->RetrieveMessage(Randomizer::NaviRandoMessageTableID, naviTextId);
}

extern "C" CustomMessageEntry Randomizer_GetIceTrapMessage() {
    u16 iceTrapTextId = rand() % NUM_ICE_TRAP_MESSAGES;
    return CustomMessageManager::Instance->RetrieveMessage(Randomizer::IceTrapRandoMessageTableID, iceTrapTextId);
}

extern "C" CustomMessageEntry Randomizer_GetAltarMessage() {
    return (LINK_IS_ADULT)
               ? CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT)
               : CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD);
}

extern "C" CustomMessageEntry Randomizer_GetGanonText() {
    return CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT);
}

extern "C" CustomMessageEntry Randomizer_GetGanonHintText() {
    return CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, TEXT_GANONDORF);
}

extern "C" CustomMessageEntry Randomizer_GetHintFromCheck(RandomizerCheck check) {
    // we don't want to make a copy of the std::string returned from GetHintFromCheck 
    // so we're just going to let RVO take care of it
    const CustomMessageEntry hintText = CustomMessageManager::Instance->RetrieveMessage(Randomizer::hintMessageTableID, check);
    return hintText;
}

extern "C" GetItemEntry ItemTable_Retrieve(int16_t getItemID) {
    GetItemEntry giEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, getItemID);
    return giEntry;
}

extern "C" GetItemEntry ItemTable_RetrieveEntry(s16 tableID, s16 getItemID) {
    return ItemTableManager::Instance->RetrieveItemEntry(tableID, getItemID);
}

extern "C" GetItemEntry Randomizer_GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogId) {
    s16 getItemModIndex;
    RandomizerCheck randomizerCheck = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actorId, sceneNum, actorParams);
    // if we got unknown check here, we don't need to do anything else, just return the ogId.
    if (randomizerCheck == RC_UNKNOWN_CHECK) {
        return ItemTable_RetrieveEntry(MOD_NONE, ogId);
    }
    if (OTRGlobals::Instance->gRandomizer->CheckContainsVanillaItem(randomizerCheck)) {
        getItemModIndex = MOD_NONE;
    } else {
        getItemModIndex = MOD_RANDOMIZER;
    }
    s16 itemID = OTRGlobals::Instance->gRandomizer->GetItemIdFromActor(actorId, sceneNum, actorParams, ogId);

    if (OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerCheck(randomizerCheck) != CAN_OBTAIN) {
        return ItemTable_RetrieveEntry(MOD_NONE, GI_RUPEE_BLUE);
    }

    return ItemTable_RetrieveEntry(getItemModIndex, itemID);
}

extern "C" GetItemEntry Randomizer_GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    s16 getItemModIndex;

    // if we got unknown check here, we don't need to do anything else, just return the ogId.
    if (randomizerCheck == RC_UNKNOWN_CHECK) {
        return ItemTable_RetrieveEntry(MOD_NONE, ogId);
    }
    if (OTRGlobals::Instance->gRandomizer->CheckContainsVanillaItem(randomizerCheck)) {
        getItemModIndex = MOD_NONE;
    } else {
        getItemModIndex = MOD_RANDOMIZER;
    }
    s16 itemID = OTRGlobals::Instance->gRandomizer->GetItemIdFromKnownCheck(randomizerCheck, ogId);

    if (OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerCheck(randomizerCheck) != CAN_OBTAIN) {
        return ItemTable_RetrieveEntry(MOD_NONE, GI_RUPEE_BLUE);
    }

    return ItemTable_RetrieveEntry(getItemModIndex, itemID);
}

extern "C" GetItemEntry Randomizer_GetItemFromKnownCheckWithoutObtainabilityCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    s16 getItemModIndex;
    if (OTRGlobals::Instance->gRandomizer->CheckContainsVanillaItem(randomizerCheck)) {
        getItemModIndex = MOD_NONE;
    } else {
        getItemModIndex = MOD_RANDOMIZER;
    }
    s16 itemID = OTRGlobals::Instance->gRandomizer->GetItemIdFromKnownCheck(randomizerCheck, ogId);

    return ItemTable_RetrieveEntry(getItemModIndex, itemID);
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

extern "C" int CustomMessage_RetrieveIfExists(GlobalContext* globalCtx) {
    MessageContext* msgCtx = &globalCtx->msgCtx;
    uint16_t textId = msgCtx->textId;
    Font* font = &msgCtx->font;
    char* buffer = font->msgBuf;
    const int maxBufferSize = sizeof(font->msgBuf);
    CustomMessageEntry messageEntry;
    if (gSaveContext.n64ddFlag) {
        if (textId == TEXT_RANDOMIZER_CUSTOM_ITEM) {
            Player* player = GET_PLAYER(globalCtx);
            if (player->getItemEntry.getItemId == RG_ICE_TRAP) {
                messageEntry = Randomizer_GetIceTrapMessage();
            } else {
                messageEntry = Randomizer_GetCustomGetItemMessage(player);
            }
        } else if (textId == TEXT_RANDOMIZER_GOSSIP_STONE_HINTS && Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) != 0 &&
            (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == 1 ||
             (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == 2 &&
              Player_GetMask(globalCtx) == PLAYER_MASK_TRUTH) ||
             (Randomizer_GetSettingValue(RSK_GOSSIP_STONE_HINTS) == 3 && CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)))) {

            s16 actorParams = msgCtx->talkActor->params;

            // if we're in a generic grotto
            if (globalCtx->sceneNum == 62 && actorParams == 14360) {
                // look for the chest in the actorlist to determine
                // which grotto we're in
                int numOfActorLists =
                    sizeof(globalCtx->actorCtx.actorLists) / sizeof(globalCtx->actorCtx.actorLists[0]);
                for (int i = 0; i < numOfActorLists; i++) {
                    if (globalCtx->actorCtx.actorLists[i].length) {
                        if (globalCtx->actorCtx.actorLists[i].head->id == 10) {
                            // set the params for the hint check to be negative chest params
                            actorParams = 0 - globalCtx->actorCtx.actorLists[i].head->params;
                        }
                    }
                }
            }

            RandomizerCheck hintCheck =
                Randomizer_GetCheckFromActor(msgCtx->talkActor->id, globalCtx->sceneNum, actorParams);

            messageEntry = Randomizer_GetHintFromCheck(hintCheck);
        } else if (textId == TEXT_ALTAR_CHILD || textId == TEXT_ALTAR_ADULT) {
            // rando hints at altar
            messageEntry = Randomizer_GetAltarMessage();
        } else if (textId == TEXT_GANONDORF) {
            if (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) {
                messageEntry = Randomizer_GetGanonText();
            } else {
                messageEntry = Randomizer_GetGanonHintText();
            }
        // Business Scrub textID is TEXT_SCRUB_RANDOM + their price, anywhere from 0-95
        } else if (textId >= TEXT_SCRUB_RANDOM && textId <= TEXT_SCRUB_RANDOM + 95) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, textId);
        // Shop items each have two message entries
        } else if (textId >= TEXT_SHOP_ITEM_RANDOM && textId <= TEXT_SHOP_ITEM_RANDOM + (NUM_SHOP_ITEMS * 2)) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, textId);
        } else if (CVar_GetS32("gRandomizeRupeeNames", 0) &&
                   (textId == TEXT_BLUE_RUPEE || textId == TEXT_RED_RUPEE || textId == TEXT_PURPLE_RUPEE ||
                   textId == TEXT_HUGE_RUPEE)) {
            messageEntry = Randomizer::GetRupeeMessage(textId);
            // In rando, replace Navi's general overworld hints with rando-related gameplay tips
        } else if (CVar_GetS32("gRandoRelevantNavi", 1) && textId >= 0x0140 && textId <= 0x015F) {
            messageEntry = Randomizer_GetNaviMessage();
        } else if (Randomizer_GetSettingValue(RSK_SHUFFLE_MAGIC_BEANS) && textId == TEXT_BEAN_SALESMAN) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(Randomizer::merchantMessageTableID, TEXT_BEAN_SALESMAN);
        } else if (Randomizer_GetSettingValue(RSK_BOMBCHUS_IN_LOGIC) &&
                   (textId == TEXT_BUY_BOMBCHU_10_DESC || textId == TEXT_BUY_BOMBCHU_10_PROMPT)) {
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, textId);
        }
    }
    if (textId == TEXT_GS_NO_FREEZE || textId == TEXT_GS_FREEZE) {
        if (CVar_GetS32("gInjectSkulltulaCount", 0) != 0) {
            // The freeze text cannot be manually dismissed and must be auto-dismissed.
            // This is fine and even wanted when skull tokens are not shuffled, but when
            // when they are shuffled we don't want to be able to manually dismiss the box.
            // Otherwise if we get a token from a chest or an NPC we get stuck in the ItemGet
            // animation until the text box auto-dismisses.
            // RANDOTODO: Implement a way to determine if an item came from a skulltula and
            // inject the auto-dismiss control code if it did.
            if (CVar_GetS32("gSkulltulaFreeze", 0) != 0 &&
                !(gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_TOKENS) > 0)) {
                textId = TEXT_GS_NO_FREEZE;
            } else {
                textId = TEXT_GS_FREEZE;
            }
            messageEntry = CustomMessageManager::Instance->RetrieveMessage(customMessageTableID, textId);
        }
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
    SohImGui::GetGameOverlay()->TextDrawNotification(duration, true, text);
}

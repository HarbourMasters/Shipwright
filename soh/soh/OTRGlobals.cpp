#include "OTRGlobals.h"
#include "OTRAudio.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include "ResourceMgr.h"
#include "DisplayList.h"
#include "PlayerAnimation.h"
#include "Skeleton.h"
#include "Window.h"
#include "z64animation.h"
#include "z64bgcheck.h"
#include "Enhancements/gameconsole.h"
#include <ultra64/gbi.h>
#include <Animation.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif
#include <CollisionHeader.h>
#include <Array.h>
#include <Cutscene.h>
#include "Lib/stb/stb_image.h"
#define DRMP3_IMPLEMENTATION
#include "Lib/dr_libs/mp3.h"
#define DRWAV_IMPLEMENTATION
#include "Lib/dr_libs/wav.h"
#include "AudioPlayer.h"
#include "Enhancements/controls/GameControlEditor.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugconsole.h"
#include "Enhancements/debugger/debugger.h"
#include <soh/Enhancements/randomizer/randomizer_item_tracker.h>
#include "Enhancements/n64_weird_frame_data.inc"
#include "soh/frame_interpolation.h"
#include "variables.h"
#include "macros.h"
#include <Utils/StringHelper.h>
#include "Hooks.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>

#include "Lib/Fast3D/gfx_pc.h"
#include "Lib/Fast3D/gfx_rendering_api.h"

#ifdef __APPLE__
#include <SDL_scancode.h>
#else
#include <SDL2/SDL_scancode.h>
#endif

#ifdef __SWITCH__
#include "SwitchImpl.h"
#elif defined(__WIIU__)
#include "WiiUImpl.h"
#endif

#include <Audio.h>
#include <soh/Enhancements/custom-message/CustomMessageTypes.h>
#include <functions.h>

OTRGlobals* OTRGlobals::Instance;
SaveManager* SaveManager::Instance;
CustomMessageManager* CustomMessageManager::Instance;

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
    OTRGlobals::Instance = new OTRGlobals();
    SaveManager::Instance = new SaveManager();
    CustomMessageManager::Instance = new CustomMessageManager();
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

extern "C" void Hooks_ExecuteAudioInit() {
    Ship::ExecuteHooks<Ship::AudioInit>();
}

extern "C" void* getN64WeirdFrame(s32 i) {
    char* weirdFrameBytes = reinterpret_cast<char*>(n64WeirdFrames);
    return &weirdFrameBytes[i + sizeof(n64WeirdFrames)];
}

extern "C" s16 Randomizer_GetItemModelFromId(s16 itemId) {
    return OTRGlobals::Instance->gRandomizer->GetItemModelFromId(itemId);
}

extern "C" s32 Randomizer_GetItemIDFromGetItemID(s32 getItemId) {
    return OTRGlobals::Instance->gRandomizer->GetItemIDFromGetItemID(getItemId);
}

extern "C" void Randomizer_LoadSettings(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadRandomizerSettings(spoilerFileName);
}

extern "C" void Randomizer_LoadHintLocations(const char* spoilerFileName) {
    OTRGlobals::Instance->gRandomizer->LoadHintLocations(spoilerFileName);
}

extern "C" void Randomizer_LoadItemLocations(const char* spoilerFileName, bool silent) {
    OTRGlobals::Instance->gRandomizer->LoadItemLocations(spoilerFileName, silent);
}

extern "C" bool SpoilerFileExists(const char* spoilerFileName) {
    return OTRGlobals::Instance->gRandomizer->SpoilerFileExists(spoilerFileName);
}

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey) {
    return OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(randoSettingKey);
}

extern "C" RandomizerCheck Randomizer_GetCheckFromActor(s16 sceneNum, s16 actorId, s16 actorParams) {
    return OTRGlobals::Instance->gRandomizer->GetCheckFromActor(sceneNum, actorId, actorParams);
}

extern "C" CustomMessageEntry Randomizer_GetScrubMessage(u16 scrubTextId) {
    int price = 0;
    switch (scrubTextId) {
        case TEXT_SCRUB_POH:
            price = 10;
            break;
        case TEXT_SCRUB_STICK_UPGRADE:
        case TEXT_SCRUB_NUT_UPGRADE:
            price = 40;
            break;
    }
    return CustomMessageManager::Instance->RetrieveMessage(Randomizer::scrubMessageTableID, price);
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

extern "C" s32 Randomizer_GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum) {
    return OTRGlobals::Instance->gRandomizer->GetRandomizedItemId(ogId, actorId, actorParams, sceneNum);
}

extern "C" s32 Randomizer_GetItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return OTRGlobals::Instance->gRandomizer->GetRandomizedItemIdFromKnownCheck(randomizerCheck, ogId);
}

extern "C" bool Randomizer_ObtainedFreestandingIceTrap(RandomizerCheck randomizerCheck, GetItemID ogId, Actor* actor) {
    return gSaveContext.n64ddFlag && (actor->parent != NULL) &&
         Randomizer_GetItemIdFromKnownCheck(randomizerCheck, ogId) == GI_ICE_TRAP;
}

extern "C" bool Randomizer_ItemIsIceTrap(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return gSaveContext.n64ddFlag && Randomizer_GetItemIdFromKnownCheck(randomizerCheck, ogId) == GI_ICE_TRAP;
}

extern "C" CustomMessageEntry Randomizer_GetCustomGetItemMessage(GetItemID giid, char* buffer, const int maxBufferSize) {
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
            messageEntry =
                Randomizer_GetCustomGetItemMessage((GetItemID)GET_PLAYER(globalCtx)->getItemId, buffer, maxBufferSize);
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
                Randomizer_GetCheckFromActor(globalCtx->sceneNum, msgCtx->talkActor->id, actorParams);

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
        } else if (textId == TEXT_SCRUB_POH || textId == TEXT_SCRUB_STICK_UPGRADE || textId == TEXT_SCRUB_NUT_UPGRADE) {
            messageEntry = Randomizer_GetScrubMessage(textId);
        }
    }
    if (textId == TEXT_GS_NO_FREEZE || textId == TEXT_GS_FREEZE) {
        if (CVar_GetS32("gInjectSkulltulaCount", 0) != 0) {
            if (CVar_GetS32("gSkulltulaFreeze", 0) != 0) {
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

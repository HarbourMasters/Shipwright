#include "OTRGlobals.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include "GlobalCtx2.h"
#include "GameSettings.h"
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
#include <Vertex.h>
#include <CollisionHeader.h>
#include <Array.h>
#include <Cutscene.h>
#include <Texture.h>
#include "Lib/stb/stb_image.h"
#include "AudioPlayer.h"
#include "Enhancements/debugconsole.h"
#include "Enhancements/debugger/debugger.h"
#include "Utils/BitConverter.h"
#include "variables.h"
#include "macros.h"
#include <Utils/StringHelper.h>

OTRGlobals* OTRGlobals::Instance;

static struct {
    std::condition_variable cv_to_thread, cv_from_thread;
    std::mutex mutex;
    bool initialized;
    bool processing;
} audio;

OTRGlobals::OTRGlobals() {
    context = Ship::GlobalCtx2::CreateInstance("Ship of Harkinian");
    context->GetWindow()->Init();
}

OTRGlobals::~OTRGlobals() {
}

extern uintptr_t clearMtx;
extern "C" Mtx gMtxClear;
extern "C" MtxF gMtxFClear;
extern "C" void OTRMessage_Init();
extern "C" void AudioMgr_CreateNextAudioBuffer(s16* samples, u32 num_samples);
extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len);
extern "C" int AudioPlayer_Buffered(void);
extern "C" int AudioPlayer_GetDesiredBuffered(void);

// C->C++ Bridge
extern "C" void InitOTR() {
    OTRGlobals::Instance = new OTRGlobals();
    auto t = OTRGlobals::Instance->context->GetResourceManager()->LoadFile("version");

    if (!t->bHasLoadError)
    {
        //uint32_t gameVersion = BitConverter::ToUInt32BE((uint8_t*)t->buffer.get(), 0);
        uint32_t gameVersion = *((uint32_t*)t->buffer.get());
        OTRGlobals::Instance->context->GetResourceManager()->SetGameVersion(gameVersion);
    }

    clearMtx = (uintptr_t)&gMtxClear;
    OTRMessage_Init();
    DebugConsole_Init();
    Debug_Init();
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
    OTRGlobals::Instance->context->GetWindow()->MainLoop(run_one_game_iter);
}

extern "C" void Graph_StartFrame() {
    OTRGlobals::Instance->context->GetWindow()->StartFrame();
}

// C->C++ Bridge
extern "C" void Graph_ProcessGfxCommands(Gfx* commands) {
    OTRGlobals::Instance->context->GetWindow()->SetFrameDivisor(R_UPDATE_RATE);

    if (!audio.initialized) {
        audio.initialized = true;
        std::thread([]() {
            for (;;) {
                {
                    std::unique_lock<std::mutex> Lock(audio.mutex);
                    while (!audio.processing) {
                        audio.cv_to_thread.wait(Lock);
                    }
                }
                //AudioMgr_ThreadEntry(&gAudioMgr);
                // 528 and 544 relate to 60 fps at 32 kHz 32000/60 = 533.333..
                // in an ideal world, one third of the calls should use num_samples=544 and two thirds num_samples=528
                #define SAMPLES_HIGH 560
                #define SAMPLES_LOW 528
                // PAL values
                //#define SAMPLES_HIGH 656
                //#define SAMPLES_LOW 624
                #define AUDIO_FRAMES_PER_UPDATE (R_UPDATE_RATE > 0 ? R_UPDATE_RATE : 1 )
                #define NUM_AUDIO_CHANNELS 2
                int samples_left = AudioPlayer_Buffered();
                u32 num_audio_samples = samples_left < AudioPlayer_GetDesiredBuffered() ? SAMPLES_HIGH : SAMPLES_LOW;
                // printf("Audio samples: %d %u\n", samples_left, num_audio_samples);

                // 3 is the maximum authentic frame divisor.
                s16 audio_buffer[SAMPLES_HIGH * NUM_AUDIO_CHANNELS * 3];
                for (int i = 0; i < AUDIO_FRAMES_PER_UPDATE; i++) {
                    AudioMgr_CreateNextAudioBuffer(audio_buffer + i * (num_audio_samples * NUM_AUDIO_CHANNELS), num_audio_samples);
                }
                //for (uint32_t i = 0; i < 2 * num_audio_samples; i++) {
                //    audio_buffer[i] = Rand_Next() & 0xFF;
                //}
                // printf("Audio samples before submitting: %d\n", audio_api->buffered());
                AudioPlayer_Play((u8*)audio_buffer, num_audio_samples * (sizeof(int16_t) * NUM_AUDIO_CHANNELS * AUDIO_FRAMES_PER_UPDATE));

                {
                    std::unique_lock<std::mutex> Lock(audio.mutex);
                    audio.processing = false;
                }
                audio.cv_from_thread.notify_one();
            }
        }).detach();
    }

    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        audio.processing = true;
    }
    audio.cv_to_thread.notify_one();

    OTRGlobals::Instance->context->GetWindow()->RunCommands(commands);

    {
        std::unique_lock<std::mutex> Lock(audio.mutex);
        while (audio.processing) {
            audio.cv_from_thread.wait(Lock);
        }
    }

    // OTRTODO: FIGURE OUT END FRAME POINT
   /* if (OTRGlobals::Instance->context->GetWindow()->lastScancode != -1)
        OTRGlobals::Instance->context->GetWindow()->lastScancode = -1;*/

}

float divisor_num = 0.0f;

extern "C" void OTRGetPixelDepthPrepare(float x, float y) {
    OTRGlobals::Instance->context->GetWindow()->GetPixelDepthPrepare(x, y);
}

extern "C" uint16_t OTRGetPixelDepth(float x, float y) {
    return OTRGlobals::Instance->context->GetWindow()->GetPixelDepth(x, y);
}

extern "C" int32_t OTRGetLastScancode()
{
    return OTRGlobals::Instance->context->GetWindow()->lastScancode;
}

extern "C" void OTRResetScancode()
{
    OTRGlobals::Instance->context->GetWindow()->lastScancode = -1;
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

extern "C" char* ResourceMgr_LoadTexOrDListByName(const char* filePath) {
    auto res = OTRGlobals::Instance->context->GetResourceManager()->LoadResource(filePath);

    if (res->resType == Ship::ResourceType::DisplayList)
        return (char*)&((std::static_pointer_cast<Ship::DisplayList>(res))->instructions[0]);
    else if (res->resType == Ship::ResourceType::Array)
        return (char*)(std::static_pointer_cast<Ship::Array>(res))->vertices.data();
    else
        return ResourceMgr_LoadTexByName(filePath);
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

        for (int i = 0; i < res->scalars.size(); i += 3) {
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

    for (int i = 0; i < colRes->vertices.size(); i++)
    {
        colHeader->vtxList[i].x = colRes->vertices[i].x;
        colHeader->vtxList[i].y = colRes->vertices[i].y;
        colHeader->vtxList[i].z = colRes->vertices[i].z;
    }

    colHeader->polyList = (CollisionPoly*)malloc(sizeof(CollisionPoly) * colRes->polygons.size());
    colHeader->numPolygons = colRes->polygons.size();

    for (int i = 0; i < colRes->polygons.size(); i++)
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

    for (int i = 0; i < colRes->polygonTypes.size(); i++)
    {
        colHeader->surfaceTypeList[i].data[0] = colRes->polygonTypes[i] >> 32;
        colHeader->surfaceTypeList[i].data[1] = colRes->polygonTypes[i] & 0xFFFFFFFF;
    }

    colHeader->cameraDataList = (CamData*)malloc(sizeof(CamData) * colRes->camData->entries.size());

    for (int i = 0; i < colRes->camData->entries.size(); i++)
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

extern "C" Vtx * ResourceMgr_LoadVtxByName(const char* path)
{
	auto res = std::static_pointer_cast<Ship::Array>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(path));
	return (Vtx*)res->vertices.data();
}

extern "C" int ResourceMgr_OTRSigCheck(char* imgData)
{
	uintptr_t i = (uintptr_t)(imgData);

    if (i == 0xD9000000 || i == 0xE7000000 || (i & 1) == 1)
        return 0;

    if ((i & 0xFF000000) != 0xAB000000 && (i & 0xFF000000) != 0xCD000000 && i != 0) {
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

        for (int i = 0; i < res->rotationValues.size(); i++)
            animNormal->frameData[i] = res->rotationValues[i];

        animNormal->jointIndices = (JointIndex*)malloc(res->rotationIndices.size() * sizeof(Vec3s));

        for (int i = 0; i < res->rotationIndices.size(); i++) {
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

        for (int i = 0; i < res->copyValuesArr.size(); i++)
            animCurve->copyValues[i] = res->copyValuesArr[i];

        animCurve->transformData = (TransformData*)malloc(res->transformDataArr.size() * sizeof(TransformData));

        for (int i = 0; i < res->transformDataArr.size(); i++)
        {
            animCurve->transformData[i].unk_00 = res->transformDataArr[i].unk_00;
            animCurve->transformData[i].unk_02 = res->transformDataArr[i].unk_02;
            animCurve->transformData[i].unk_04 = res->transformDataArr[i].unk_04;
            animCurve->transformData[i].unk_06 = res->transformDataArr[i].unk_06;
            animCurve->transformData[i].unk_08 = res->transformDataArr[i].unk_08;
        }

        animCurve->refIndex = (u8*)malloc(res->refIndexArr.size());
        for (int i = 0; i < res->refIndexArr.size(); i++)
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

    for (int i = 0; i < res->limbTable.size(); i++) {
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

                for (int i = 0; i < skinDataSize; i++)
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
    return OTRGlobals::Instance->context->GetWindow()->GetCurrentWidth();
}

extern "C" uint32_t OTRGetCurrentHeight() {
    return OTRGlobals::Instance->context->GetWindow()->GetCurrentHeight();
}

extern "C" void OTRControllerCallback(ControllerCallback* controller) {
    auto controllers = OTRGlobals::Instance->context->GetWindow()->Controllers;
    for (int i = 0; i < controllers.size(); i++) {
        for (int j = 0; j < controllers[i].size(); j++) {
            OTRGlobals::Instance->context->GetWindow()->Controllers[i][j]->WriteToSource(controller);
        }
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

extern "C" void bswapSoundFontSound(SoundFontSound* swappable) {
    swappable->sample = (SoundFontSample*)BOMSWAP32((u32)swappable->sample);
    swappable->tuningAsU32 = BOMSWAP32((u32)swappable->tuningAsU32);
}

extern "C" void bswapDrum(Drum* swappable) {
    bswapSoundFontSound(&swappable->sound);
    swappable->envelope = (AdsrEnvelope*)BOMSWAP32((u32)swappable->envelope);
}

extern "C" void bswapInstrument(Instrument* swappable) {
    swappable->envelope = (AdsrEnvelope*)BOMSWAP32((u32)swappable->envelope);
    bswapSoundFontSound(&swappable->lowNotesSound);
    bswapSoundFontSound(&swappable->normalNotesSound);
    bswapSoundFontSound(&swappable->highNotesSound);
}

extern "C" void bswapSoundFontSample(SoundFontSample* swappable) {
    u32 origBitfield = BOMSWAP32(swappable->asU32);

    swappable->codec = (origBitfield >> 28) & 0x0F;
    swappable->medium = (origBitfield >> 24) & 0x03;
    swappable->unk_bit26 = (origBitfield >> 22) & 0x01;
    swappable->unk_bit25 = (origBitfield >> 21) & 0x01;
    swappable->size = (origBitfield) & 0x00FFFFFF;

    swappable->sampleAddr = (u8*)BOMSWAP32((u32)swappable->sampleAddr);
    swappable->loop = (AdpcmLoop*)BOMSWAP32((u32)swappable->loop);
    swappable->book = (AdpcmBook*)BOMSWAP32((u32)swappable->book);
}

extern "C" void bswapAdpcmLoop(AdpcmLoop* swappable) {
    swappable->start = (u32)BOMSWAP32((u32)swappable->start);
    swappable->end = (u32)BOMSWAP32((u32)swappable->end);
    swappable->count = (u32)BOMSWAP32((u32)swappable->count);

    if (swappable->count != 0) {
        for (int i = 0; i < 16; i++) {
            swappable->state[i] = (s16)BOMSWAP16(swappable->state[i]);
        }
    }
}

extern "C" void bswapAdpcmBook(AdpcmBook* swappable) {
    swappable->order = (u32)BOMSWAP32((u32)swappable->order);
    swappable->npredictors = (u32)BOMSWAP32((u32)swappable->npredictors);

    for (int i = 0; i < swappable->npredictors * swappable->order * sizeof(s16) * 4; i++)
        swappable->book[i] = (s16)BOMSWAP16(swappable->book[i]);
}

extern "C" bool AudioPlayer_Init(void) {
    if (OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer()->Init();
    }

    return false;
}

extern "C" int AudioPlayer_Buffered(void) {
    if (OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer()->Buffered();
    }
}

extern "C" int AudioPlayer_GetDesiredBuffered(void) {
    if (OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer() != nullptr) {
        return OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer()->GetDesiredBuffered();
    }
}

extern "C" void AudioPlayer_Play(const uint8_t* buf, uint32_t len) {
    if (OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer() != nullptr) {
        OTRGlobals::Instance->context->GetWindow()->GetAudioPlayer()->Play(buf, len);
    }
}

extern "C" int Controller_ShouldRumble(size_t i) {
    for (const auto& controller : Ship::Window::Controllers.at(i))
    {
        float rumble_strength = CVar_GetFloat(StringHelper::Sprintf("gCont%i_RumbleStrength", i).c_str(), 1.0f);

        if (controller->CanRumble() && rumble_strength > 0.001f) {
            return 1;
        }
    }

    return 0;
}

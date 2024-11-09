#include "z64.h"

//OTRTODO - this is awful

extern "C" {
void InitOTR();
void Graph_ProcessFrame(void (*run_one_game_iter)(void));
void Graph_StartFrame();
void Graph_ProcessGfxCommands(Gfx* commands);
void OTRLogString(const char* src);
void OTRGfxPrint(const char* str, void* printer, void (*printImpl)(void*, char));
void OTRSetFrameDivisor(int divisor);
void OTRGetPixelDepthPrepare(float x, float y);
uint16_t OTRGetPixelDepth(float x, float y);
int32_t OTRGetLastScancode();
void ResourceMgr_LoadDirectory(const char* resName);
uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);
uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);
size_t ResourceGetTexSizeByName(const char* name);
char* ResourceMgr_LoadTexOrDListByName(char* filePath);
char* ResourceMgr_LoadIfDListByName(char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(char* animPath);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
Vtx* ResourceMgr_LoadVtxByName(char* path);
CollisionHeader* ResourceMgr_LoadColByName(char* path);
uint64_t GetPerfCounter();
int ResourceMgr_OTRSigCheck(char* imgData);
void ResourceMgr_PushCurrentDirectory(char* path);

}


extern "C" void gSPSegment(void* value, int segNum, uintptr_t target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    // OTRTODO: Disabled for now to fix an issue with HD Textures.
    // With HD textures, we need to pass the path to F3D, not the raw texture data.
    // Otherwise the needed metadata is not available for proper rendering...
    // This should *not* cause any crashes, but some testing may be needed...
    // UPDATE: To maintain compatability it will still do the old behavior if the resource is a display list.
    // That should not affect HD textures.
    if (res) {
        uintptr_t desiredTarget = (uintptr_t)ResourceMgr_LoadIfDListByName(imgData);

        if (desiredTarget != NULL)
            target = desiredTarget;
    }

    __gSPSegment(value, segNum, target);
}

extern "C" void gSPSegmentLoadRes(void* value, int segNum, uintptr_t target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    if (res) {
        target = (uintptr_t)ResourceMgr_LoadTexOrDListByName(imgData);
    }

    __gSPSegment(value, segNum, target);
}

extern "C" void gSPDisplayList(Gfx* pkt, Gfx* dl) {
    char* imgData = (char*)dl;

    if (ResourceMgr_OTRSigCheck(imgData) == 1) {
        
        //ResourceMgr_PushCurrentDirectory(imgData);
        //gsSPPushCD(pkt++, imgData);
        dl = ResourceMgr_LoadGfxByName(imgData);
    }

    __gSPDisplayList(pkt, dl);
}

extern "C" void gSPDisplayListOffset(Gfx* pkt, Gfx* dl, int offset) {
    char* imgData = (char*)dl;

    if (ResourceMgr_OTRSigCheck(imgData) == 1)
        dl = ResourceMgr_LoadGfxByName(imgData);

    __gSPDisplayList(pkt, dl + offset);
}

extern "C" void gSPVertex(Gfx* pkt, uintptr_t v, int n, int v0) {

    if (ResourceMgr_OTRSigCheck((char*)v) == 1)
        v = (uintptr_t)ResourceMgr_LoadVtxByName((char*)v);

    __gSPVertex(pkt, v, n, v0);
}

extern "C" void gSPInvalidateTexCache(Gfx* pkt, uintptr_t texAddr)
{
    char* imgData = (char*)texAddr;
    
    if (texAddr != 0 && ResourceMgr_OTRSigCheck(imgData)) {
        // Temporary solution to the mq/nonmq issue, this will be
        // handled better with LUS 1.0
        texAddr = (uintptr_t)ResourceMgr_LoadTexOrDListByName(imgData); 
    }

    __gSPInvalidateTexCache(pkt, texAddr);
 }

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
void ResourceMgr_CacheDirectory(const char* resName);
void ResourceMgr_LoadFile(const char* resName);
char* ResourceMgr_LoadFileFromDisk(const char* filePath);
char* ResourceMgr_LoadTexByName(char* texPath);
uint16_t ResourceMgr_LoadTexWidthByName(char* texPath);
uint16_t ResourceMgr_LoadTexHeightByName(char* texPath);
uint32_t ResourceMgr_LoadTexSizeByName(char* texPath);
char* ResourceMgr_LoadTexOrDListByName(char* filePath);
char* ResourceMgr_LoadPlayerAnimByName(char* animPath);
char* ResourceMgr_GetNameByCRC(uint64_t crc, char* alloc);
Gfx* ResourceMgr_LoadGfxByCRC(uint64_t crc);
Gfx* ResourceMgr_LoadGfxByName(char* path);
Vtx* ResourceMgr_LoadVtxByCRC(uint64_t crc);
Vtx* ResourceMgr_LoadVtxByName(char* path);
CollisionHeader* ResourceMgr_LoadColByName(char* path);
uint64_t GetPerfCounter();
int ResourceMgr_OTRSigCheck(char* imgData);

}


extern "C" void gSPSegment(void* value, int segNum, uintptr_t target) {
    char* imgData = (char*)target;

    int res = ResourceMgr_OTRSigCheck(imgData);

    if (res)
        target = (uintptr_t)ResourceMgr_LoadTexOrDListByName(imgData);

    __gSPSegment(value, segNum, target);
}

extern "C" void gDPSetTextureImage(Gfx* pkt, u32 format, u32 size, u32 width, uintptr_t i) {
    __gDPSetTextureImage(pkt, format, size, width, i);
}

extern "C" void gDPSetTextureImageFB(Gfx* pkt, u32 format, u32 size, u32 width, int fb)
{
    __gDPSetTextureImageFB(pkt, format, size, width, fb);
}

extern "C" void gSPDisplayList(Gfx* pkt, Gfx* dl) {
    char* imgData = (char*)dl;

    if (ResourceMgr_OTRSigCheck(imgData) == 1)
        dl = ResourceMgr_LoadGfxByName(imgData);

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
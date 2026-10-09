#ifndef Z64SCENE_H
#define Z64SCENE_H

#include "ultra64.h"
#include "z64dma.h"
#include "z64cutscene.h"
#include "unk.h"

struct GameState;
struct PlayState;

#define ROOM_MAX 32 // maximum number of rooms in a scene
#define ROOM_TRANSITION_MAX 48 // maximum number of transition actors in a scene

#define SPAWN_ROT_FLAGS(rotation, flags) (((rotation) << 7) | (flags))

typedef struct {
    /* 0x0 */ uintptr_t vromStart;
    /* 0x4 */ uintptr_t vromEnd;
    char* fileName;
} RomFile; // size = 0x8

#define ROOM_DRAW_OPA (1 << 0)
#define ROOM_DRAW_XLU (1 << 1)

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ u32 data2;
} SCmdBase; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdSpawnList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdActorList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdCsCameraList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdColHeader; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdRoomList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x2 */ UNK_TYPE1 pad2[2];
    /* 0x4 */ s8  west;
    /* 0x5 */ s8  vertical;
    /* 0x6 */ s8  south;
    /* 0x7 */ u8  clothIntensity;
} SCmdWindSettings; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdEntranceList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  naviQuestHintFileId;
    /* 0x4 */ u32 subKeepId;
} SCmdSpecialFiles; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  gpFlag1;
    /* 0x4 */ u32 gpFlag2;
} SCmdRoomBehavior; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdMesh; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdObjectList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdLightList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdPathList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdTransitionActorList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdLightSettingList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x2 */ UNK_TYPE1 pad2[2];
    /* 0x4 */ u8  hour;
    /* 0x5 */ u8  min;
    /* 0x6 */ u8  timeSpeed;
} SCmdTimeSettings; // size = 0x7

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x2 */ UNK_TYPE1 pad2[2];
    /* 0x4 */ u8  skyboxId;
    /* 0x5 */ u8  skyboxConfig;
    /* 0x6 */ u8  envLightMode;
} SCmdSkyboxSettings; // size = 0x7

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x2 */ UNK_TYPE1 pad2[2];
    /* 0x4 */ u8  unk4;
    /* 0x5 */ u8  unk5;
} SCmdSkyboxDisables; // size = 0x6

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdExitList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ u32 data2;
} SCmdEndMarker; // size = 0x8

// 2S2H [Custom Audio]. Was originally u8 seqId. Made 16 bit to allow for more than 255 sequences.
typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  specId;
    /* 0x2 */ UNK_TYPE1 unk_02[4];
    /* 0x6 */ u8  ambienceId;
    /* 0x7 */ u16  seqId;
} SCmdSoundSettings; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x2 */ UNK_TYPE1 unk_02[5];
    /* 0x7 */ u8  echo;
} SCmdEchoSettings; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdCutsceneData; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  scriptListCount;
    /* 0x4 */ void* segment;
} SCmdCsScriptList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdAltHeaders; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ UNK_TYPE1 pad[7];
} SCmdRegionVisited; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdTextureAnimations; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdCutsceneList; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  data1;
    /* 0x4 */ void* segment;
} SCmdMapData; // size = 0x8

typedef struct {
    /* 0x0 */ u8  code;
    /* 0x1 */ u8  num;
    /* 0x4 */ void* segment;
} SCmdMapDataChests; // size = 0x8

typedef enum {
    /* 0 */ ROOM_SHAPE_TYPE_NORMAL,
    /* 1 */ ROOM_SHAPE_TYPE_IMAGE,
    /* 2 */ ROOM_SHAPE_TYPE_CULLABLE,
    /* 3 */ ROOM_SHAPE_TYPE_NONE,
    /* 4 */ ROOM_SHAPE_TYPE_MAX
} RoomShapeType;

typedef struct {
    /* 0x0 */ u8 type;
} RoomShapeBase; // size = 0x1

typedef struct {
    /* 0x0 */ Gfx* opa;
    /* 0x4 */ Gfx* xlu;
} RoomShapeDListsEntry; // size = 0x8

typedef struct {
    /* 0x0 */ RoomShapeBase base;
    /* 0x1 */ u8 numEntries;
    /* 0x4 */ RoomShapeDListsEntry* entries;
    /* 0x8 */ RoomShapeDListsEntry* entriesEnd;
} RoomShapeNormal; // size = 0xC

typedef enum {
    /* 1 */ ROOM_SHAPE_IMAGE_AMOUNT_SINGLE = 1,
    /* 2 */ ROOM_SHAPE_IMAGE_AMOUNT_MULTI
} RoomShapeImageAmountType;

typedef struct {
    /* 0x0 */ RoomShapeBase base;
    /* 0x1 */ u8 amountType; // uses `RoomShapeImageAmountType`
    /* 0x4 */ RoomShapeDListsEntry* entry;
} RoomShapeImageBase; // size = 0x8

typedef struct {
    /* 0x00 */ RoomShapeImageBase base;
    /* 0x08 */ void* source;
    /* 0x0C */ u32 unk_0C;
    /* 0x10 */ void* tlut;
    /* 0x14 */ u16 width;
    /* 0x16 */ u16 height;
    /* 0x18 */ u8 fmt;
    /* 0x19 */ u8 siz;
    /* 0x1A */ u16 tlutMode;
    /* 0x1C */ u16 tlutCount;
} RoomShapeImageSingle; // size = 0x20

typedef struct {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ u8 bgCamIndex; // the bg cam index this entry is for
    /* 0x04 */ void* source;
    /* 0x08 */ u32 unk_0C;
    /* 0x0C */ void* tlut;
    /* 0x10 */ u16 width;
    /* 0x12 */ u16 height;
    /* 0x14 */ u8 fmt;
    /* 0x15 */ u8 siz;
    /* 0x16 */ u16 tlutMode;
    /* 0x18 */ u16 tlutCount;
} RoomShapeImageMultiBgEntry; // size = 0x1C

typedef struct {
    /* 0x0 */ RoomShapeImageBase base;
    /* 0x8 */ u8 numBackgrounds;
    /* 0xC */ RoomShapeImageMultiBgEntry* backgrounds;
} RoomShapeImageMulti; // size = 0x10

typedef struct {
    /* 0x0 */ Vec3s boundsSphereCenter;
    /* 0x6 */ s16 boundsSphereRadius;
    /* 0x8 */ Gfx* opa;
    /* 0xC */ Gfx* xlu;
} RoomShapeCullableEntry; // size = 0x10

typedef struct {
    /* 0x0 */ RoomShapeBase base;
    /* 0x1 */ u8 numEntries;
    /* 0x4 */ RoomShapeCullableEntry* entries;
    /* 0x8 */ RoomShapeCullableEntry* entriesEnd;
} RoomShapeCullable; // size = 0xC

typedef union {
    RoomShapeBase base;
    RoomShapeNormal normal;
    union {
        RoomShapeImageBase base;
        RoomShapeImageSingle single;
        RoomShapeImageMulti multi;
    } image;
    RoomShapeCullable cullable;
} RoomShape; // "Ground Shape"

typedef enum RoomType {
    /* 0 */ ROOM_TYPE_NORMAL,
    /* 1 */ ROOM_TYPE_DUNGEON,
    /* 2 */ ROOM_TYPE_INDOORS, // Reduces player run speed and blocks player from attacking or jumping.
    /* 3 */ ROOM_TYPE_3,
    /* 4 */ ROOM_TYPE_4, // Prevents switching to CAM_SET_HORSE when mounting a horse.
    /* 5 */ ROOM_TYPE_BOSS // Disables Environment_AdjustLights
} RoomType;

typedef enum RoomEnvironmentType {
    /* 0 */ ROOM_ENV_DEFAULT,
    /* 1 */ ROOM_ENV_COLD,
    /* 2 */ ROOM_ENV_WARM, // Unused.
    /* 3 */ ROOM_ENV_HOT, // Unused.
    /* 4 */ ROOM_ENV_UNK_STRETCH_1,
    /* 5 */ ROOM_ENV_UNK_STRETCH_2, // Unused.
    /* 6 */ ROOM_ENV_UNK_STRETCH_3
} RoomEnvironmentType;

typedef struct {
    /* 0x00 */ s8 num; // -1 is invalid room
    /* 0x01 */ u8 unk1;
    /* 0x02 */ u8 environmentType;
    /* 0x03 */ u8 type;
    /* 0x04 */ s8 echo;
    /* 0x05 */ u8 lensMode;
    /* 0x06 */ u8 enablePosLights;
    /* 0x08 */ RoomShape* roomShape;
    /* 0x0C */ void* segment;
    /* 0x10 */ UNK_TYPE1 pad10[0x4];
} Room; // size = 0x14

typedef struct RoomContext {
    /* 0x00 */ Room curRoom;
    /* 0x14 */ Room prevRoom;
    /* 0x28 */ void* bufPtrs[2]; // Start and end pointers for the room buffer. Can be split into two pages, where page 0 is allocated from the start pointer and page 1 is allocated from the end pointer.
    /* 0x30 */ u8 activeBufPage; // 0 - First page in memory, 1 - Second page
    /* 0x31 */ s8 status; // 0 - Free for new room request, 1 - DmaRequest for a new room is in progress
    /* 0x34 */ void* roomRequestAddr; // Pointer to where the requested room segment will be stored
    /* 0x38 */ DmaRequest dmaRequest;
    /* 0x58 */ OSMesgQueue loadQueue;
    /* 0x70 */ OSMesg loadMsg[1];
    /* 0x74 */ void* unk74;
    /* 0x78 */ s8 unk78;
    /* 0x79 */ s8 unk79;
    /* 0x7A */ UNK_TYPE2 unk7A[3];
} RoomContext; // size = 0x80

typedef struct RoomList {
    /* 0x0 */ u8 count;
    /* 0x4 */ RomFile* romFiles; // Array of rom addresses for each room in a scene
} RoomList; // size = 0x8

typedef void(*RoomDrawHandler)(struct PlayState* play, Room* room, u32 flags);

typedef struct TransitionActorEntry {
    struct {
        s8 room;    // Room to switch to
        s8 bgCamIndex; // How the camera reacts during the transition. -2 for spiral staircase. -1 for generic door. 0+ will index scene CamData
    } /* 0x0 */ sides[2]; // 0 = front, 1 = back
    /* 0x4 */ s16 id;
    /* 0x6 */ Vec3s pos;
    /* 0xC */ s16 rotY;
    /* 0xE */ u16 params;
} TransitionActorEntry; // size = 0x10

typedef struct {
    /* 0x0 */ u8 count;
    /* 0x4 */ TransitionActorEntry* list;
} TransitionActorList; // size = 0x8

typedef struct ActorEntry {
    /* 0x0 */ s16 id;
    /* 0x2 */ Vec3s pos;
    /* 0x8 */ Vec3s rot;
    /* 0xE */ s16 params;
} ActorEntry; // size = 0x10

typedef struct {
    /* 0x0 */ u8 spawn;
    /* 0x1 */ u8 room;
} EntranceEntry; // size = 0x2

typedef struct {
    /* 0x0 */ s8 sceneId;
    /* 0x1 */ s8 spawnNum;
    /* 0x2 */ u16 flags;
} EntranceTableEntry; // size = 0x4

typedef struct {
    /* 0x0 */ u32 tableCount : 8; // unused
    /* 0x4 */ EntranceTableEntry** table;
    /* 0x8 */ char* name; // unused
} SceneEntranceTableEntry; // size = 0xC

typedef struct {
    /* 0x00 */ s16 id; // Negative ids mean that the object is unloaded
    /* 0x02 */ UNK_TYPE1 pad2[0x2];
    /* 0x04 */ void* segment;
    /* 0x08 */ DmaRequest dmaReq;
    /* 0x28 */ OSMesgQueue loadQueue;
    /* 0x40 */ OSMesg loadMsg;
} ObjectEntry; // size = 0x44

typedef struct {
    /* 0x0 */ RomFile segment;
    /* 0x8 */ u16 titleTextId;
    /* 0xA */ u8 unk_A;
    /* 0xB */ u8 drawConfig;
    /* 0xC */ u8 unk_C;
    /* 0xD */ u8 unk_D;
} SceneTableEntry; // size = 0x10

typedef struct {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
    /* 0x4 */ u8 lodFrac;
} F3DPrimColor; // size = 0x5

typedef struct {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
} F3DEnvColor; // size = 0x4

typedef struct {
    /* 0x0 */ u16 keyFrameLength;
    /* 0x2 */ u16 keyFrameCount;
    /* 0x4 */ F3DPrimColor* primColors;
    /* 0x8 */ F3DEnvColor* envColors;
    /* 0xC */ u16* keyFrames;
} AnimatedMatColorParams; // size = 0x10

typedef struct {
    /* 0x0 */ s8 xStep;
    /* 0x1 */ s8 yStep;
    /* 0x2 */ u8 width;
    /* 0x3 */ u8 height;
} AnimatedMatTexScrollParams; // size = 0x4

typedef struct {
    /* 0x0 */ u16 keyFrameLength;
    /* 0x4 */ TexturePtr* textureList;
    /* 0x8 */ u8* textureIndexList;
} AnimatedMatTexCycleParams; // size = 0xC

typedef struct {
    /* 0x0 */ s8 segment;
    /* 0x2 */ s16 type;
    /* 0x4 */ void* params;
} AnimatedMaterial; // size = 0x8

// TODO: ZAPD
typedef RoomShapeCullableEntry PolygonDlist2;
typedef RoomShapeCullable PolygonType2;

#define OBJECT_SLOT_NONE -1

typedef struct {
    /* 0x000 */ void* spaceStart;
    /* 0x004 */ void* spaceEnd;
    /* 0x008 */ u8 numEntries; // total amount of used entries
    /* 0x009 */ u8 numPersistentEntries; // amount of entries that won't be reused when loading a new object list (when loading a new room)
    /* 0x00A */ u8 mainKeepSlot; // "gameplay_keep" slot
    /* 0x00B */ u8 subKeepSlot; // "gameplay_field_keep" or "gameplay_dangeon_keep" slot
    /* 0x00C */ ObjectEntry slots[35];
} ObjectContext; // size = 0x958

#define PATH_INDEX_NONE -1
#define ADDITIONAL_PATH_INDEX_NONE (u8)-1

typedef struct {
    /* 0x0 */ u8 count; // Number of points in the path
    /* 0x1 */ u8 additionalPathIndex;
    /* 0x2 */ s16 customValue; // Path specific to help distinguish different paths
    /* 0x4 */ Vec3s* points; // Segment Address to the array of points
} Path; // size = 0x8

typedef struct {
    /* 0x0 */ u16 mapId;
    /* 0x2 */ s16 centerX;
    /* 0x4 */ s16 floorY;
    /* 0x6 */ s16 centerZ;
    /* 0x8 */ u16 flags;
} MapDataRoom; // size = 0xA

#define MAP_DATA_NO_MAP 0xFFFF
#define MAP_DATA_ROOM_FLIP_X 1
#define MAP_DATA_ROOM_FLIP_Y 2
#define MAP_DATA_ROOM_GET_EXTRA_STOREYS(mapDataRoom) ((((mapDataRoom)->flags) >> 2) & 7)

typedef struct {
    /* 0x0 */ MapDataRoom* rooms;
    /* 0x4 */ s16 scale;
} MapDataScene; // size  = 0x8

typedef struct {
    /* 0x0 */ s16 room;
    /* 0x2 */ s16 chestFlagId;
    /* 0x4 */ s16 x;
    /* 0x6 */ s16 y;
    /* 0x8 */ s16 z;
} MapDataChest; // size = 0xA

// TODO: ZAPD updates
typedef SCmdMapData SCmdMinimapSettings;
typedef SCmdMapDataChests SCmdMinimapChests;
typedef MapDataRoom MinimapEntry;
typedef MapDataScene MinimapList;
typedef MapDataChest MinimapChest;
typedef SCmdTransitionActorList SCmdTransiActorList;

// TODO: consider merging with bgCamInfo?
typedef struct {
    /* 0x0 */ s16 setting; // camera setting described by CameraSettingType enum
    /* 0x2 */ s16 count;
    /* 0x4 */ Vec3s* actorCsCamFuncData; // s16 data grouped in threes
} ActorCsCamInfo; // size = 0x8

typedef ActorCsCamInfo CsCameraEntry; // TODO: Remove once ZAPD updates its structs

typedef union {
    /* Command: N/A  */ SCmdBase                base;
    /* Command: 0x00 */ SCmdSpawnList           spawnList;
    /* Command: 0x01 */ SCmdActorList           actorList;
    /* Command: 0x02 */ SCmdCsCameraList        actorCsCamList;
    /* Command: 0x03 */ SCmdColHeader           colHeader;
    /* Command: 0x04 */ SCmdRoomList            roomList;
    /* Command: 0x05 */ SCmdWindSettings        windSettings;
    /* Command: 0x06 */ SCmdEntranceList        entranceList;
    /* Command: 0x07 */ SCmdSpecialFiles        specialFiles;
    /* Command: 0x08 */ SCmdRoomBehavior        roomBehavior;
    /* Command: 0x09 */ // Unused
    /* Command: 0x0A */ SCmdMesh                mesh;
    /* Command: 0x0B */ SCmdObjectList          objectList;
    /* Command: 0x0C */ SCmdLightList           lightList;
    /* Command: 0x0D */ SCmdPathList            pathList;
    /* Command: 0x0E */ SCmdTransitionActorList transitionActorList;
    /* Command: 0x0F */ SCmdLightSettingList    lightSettingList;
    /* Command: 0x10 */ SCmdTimeSettings        timeSettings;
    /* Command: 0x11 */ SCmdSkyboxSettings      skyboxSettings;
    /* Command: 0x12 */ SCmdSkyboxDisables      skyboxDisables;
    /* Command: 0x13 */ SCmdExitList            exitList;
    /* Command: 0x14 */ SCmdEndMarker           endMarker;
    /* Command: 0x15 */ SCmdSoundSettings       soundSettings;
    /* Command: 0x16 */ SCmdEchoSettings        echoSettings;
    /* Command: 0x17 */ SCmdCsScriptList        scriptList;
    /* Command: 0x18 */ SCmdAltHeaders          altHeaders;
    /* Command: 0x19 */ SCmdRegionVisited       regionVisited;
    /* Command: 0x1A */ SCmdTextureAnimations   textureAnimations;
    /* Command: 0x1B */ SCmdCutsceneList        cutsceneList;
    /* Command: 0x1C */ SCmdMapData             mapData;
    /* Command: 0x1D */ // Unused
    /* Command: 0x1E */ SCmdMapDataChests       mapDataChests;
} SceneCmd; // size = 0x8

// Sets cursor point options on the world map
typedef enum RegionId {
    /*  -1 */ REGION_NONE = -1,
    /* 0x0 */ REGION_GREAT_BAY,
    /* 0x1 */ REGION_ZORA_HALL,
    /* 0x2 */ REGION_ROMANI_RANCH,
    /* 0x3 */ REGION_DEKU_PALACE,
    /* 0x4 */ REGION_WOODFALL,
    /* 0x5 */ REGION_CLOCK_TOWN,
    /* 0x6 */ REGION_SNOWHEAD,
    /* 0x7 */ REGION_IKANA_GRAVEYARD,
    /* 0x8 */ REGION_IKANA_CANYON,
    /* 0x9 */ REGION_GORON_VILLAGE,
    /* 0xA */ REGION_STONE_TOWER,
    /* 0xB */ REGION_MAX
} RegionId;

// Sets warp points for owl statues
typedef enum OwlWarpId {
    /*  0x0 */ OWL_WARP_GREAT_BAY_COAST,
    /*  0x1 */ OWL_WARP_ZORA_CAPE,
    /*  0x2 */ OWL_WARP_SNOWHEAD,
    /*  0x3 */ OWL_WARP_MOUNTAIN_VILLAGE,
    /*  0x4 */ OWL_WARP_CLOCK_TOWN,
    /*  0x5 */ OWL_WARP_MILK_ROAD,
    /*  0x6 */ OWL_WARP_WOODFALL,
    /*  0x7 */ OWL_WARP_SOUTHERN_SWAMP,
    /*  0x8 */ OWL_WARP_IKANA_CANYON,
    /*  0x9 */ OWL_WARP_STONE_TOWER,
    /*  0xA */ OWL_WARP_ENTRANCE, // Special index for warping to the entrance of a scene
    /*  0xB */ OWL_WARP_MAX,
    /* 0xFF */ OWL_WARP_NONE = 0xFF
} OwlWarpId;

// Sets cloud visibility on the world map
typedef enum TingleMapId {
    /* 0 */ TINGLE_MAP_CLOCK_TOWN,
    /* 1 */ TINGLE_MAP_WOODFALL,
    /* 2 */ TINGLE_MAP_SNOWHEAD,
    /* 3 */ TINGLE_MAP_ROMANI_RANCH,
    /* 4 */ TINGLE_MAP_GREAT_BAY,
    /* 5 */ TINGLE_MAP_STONE_TOWER,
    /* 6 */ TINGLE_MAP_MAX
} TingleMapId;

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, _entranceSceneId, _betterMapSelectIndex, _humanName) enumValue,
#define DEFINE_SCENE_UNSET(enumValue) enumValue,

typedef enum SceneId {
    #include "tables/scene_table.h"
    /* 0x71 */ SCENE_MAX
} SceneId;

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

typedef enum {
    /* 0x00 */ ENTR_SCENE_MAYORS_RESIDENCE,
    /* 0x01 */ ENTR_SCENE_MAJORAS_LAIR,
    /* 0x02 */ ENTR_SCENE_MAGIC_HAGS_POTION_SHOP,
    /* 0x03 */ ENTR_SCENE_RANCH_HOUSE,
    /* 0x04 */ ENTR_SCENE_HONEY_AND_DARLINGS_SHOP,
    /* 0x05 */ ENTR_SCENE_BENEATH_THE_GRAVERYARD,
    /* 0x06 */ ENTR_SCENE_SOUTHERN_SWAMP_CLEARED,
    /* 0x07 */ ENTR_SCENE_CURIOSITY_SHOP,
    /* 0x08 */ ENTR_SCENE_UNSET_08,
    /* 0x09 */ ENTR_SCENE_UNSET_09,
    /* 0x0A */ ENTR_SCENE_GROTTOS,
    /* 0x0B */ ENTR_SCENE_UNSET_0B,
    /* 0x0C */ ENTR_SCENE_UNSET_0C,
    /* 0x0D */ ENTR_SCENE_UNSET_0D,
    /* 0x0E */ ENTR_SCENE_CUTSCENE,
    /* 0x0F */ ENTR_SCENE_UNSET_0F,
    /* 0x10 */ ENTR_SCENE_IKANA_CANYON,
    /* 0x11 */ ENTR_SCENE_PIRATES_FORTRESS,
    /* 0x12 */ ENTR_SCENE_MILK_BAR,
    /* 0x13 */ ENTR_SCENE_STONE_TOWER_TEMPLE,
    /* 0x14 */ ENTR_SCENE_TREASURE_CHEST_SHOP,
    /* 0x15 */ ENTR_SCENE_STONE_TOWER_TEMPLE_INVERTED,
    /* 0x16 */ ENTR_SCENE_CLOCK_TOWER_ROOFTOP,
    /* 0x17 */ ENTR_SCENE_OPENING_DUNGEON,
    /* 0x18 */ ENTR_SCENE_WOODFALL_TEMPLE,
    /* 0x19 */ ENTR_SCENE_PATH_TO_MOUNTAIN_VILLAGE,
    /* 0x1A */ ENTR_SCENE_IKANA_CASTLE,
    /* 0x1B */ ENTR_SCENE_DEKU_SCRUB_PLAYGROUND,
    /* 0x1C */ ENTR_SCENE_ODOLWAS_LAIR,
    /* 0x1D */ ENTR_SCENE_TOWN_SHOOTING_GALLERY,
    /* 0x1E */ ENTR_SCENE_SNOWHEAD_TEMPLE,
    /* 0x1F */ ENTR_SCENE_MILK_ROAD,
    /* 0x20 */ ENTR_SCENE_PIRATES_FORTRESS_INTERIOR,
    /* 0x21 */ ENTR_SCENE_SWAMP_SHOOTING_GALLERY,
    /* 0x22 */ ENTR_SCENE_PINNACLE_ROCK,
    /* 0x23 */ ENTR_SCENE_FAIRY_FOUNTAIN,
    /* 0x24 */ ENTR_SCENE_SWAMP_SPIDER_HOUSE,
    /* 0x25 */ ENTR_SCENE_OCEANSIDE_SPIDER_HOUSE,
    /* 0x26 */ ENTR_SCENE_ASTRAL_OBSERVATORY,
    /* 0x27 */ ENTR_SCENE_MOON_DEKU_TRIAL,
    /* 0x28 */ ENTR_SCENE_DEKU_PALACE,
    /* 0x29 */ ENTR_SCENE_MOUNTAIN_SMITHY,
    /* 0x2A */ ENTR_SCENE_TERMINA_FIELD,
    /* 0x2B */ ENTR_SCENE_POST_OFFICE,
    /* 0x2C */ ENTR_SCENE_MARINE_RESEARCH_LAB,
    /* 0x2D */ ENTR_SCENE_DAMPES_HOUSE,
    /* 0x2E */ ENTR_SCENE_UNSET_2E,
    /* 0x2F */ ENTR_SCENE_GORON_SHRINE,
    /* 0x30 */ ENTR_SCENE_ZORA_HALL,
    /* 0x31 */ ENTR_SCENE_TRADING_POST,
    /* 0x32 */ ENTR_SCENE_ROMANI_RANCH,
    /* 0x33 */ ENTR_SCENE_TWINMOLDS_LAIR,
    /* 0x34 */ ENTR_SCENE_GREAT_BAY_COAST,
    /* 0x35 */ ENTR_SCENE_ZORA_CAPE,
    /* 0x36 */ ENTR_SCENE_LOTTERY_SHOP,
    /* 0x37 */ ENTR_SCENE_UNSET_37,
    /* 0x38 */ ENTR_SCENE_PIRATES_FORTRESS_EXTERIOR,
    /* 0x39 */ ENTR_SCENE_FISHERMANS_HUT,
    /* 0x3A */ ENTR_SCENE_GORON_SHOP,
    /* 0x3B */ ENTR_SCENE_DEKU_KINGS_CHAMBER,
    /* 0x3C */ ENTR_SCENE_MOON_GORON_TRIAL,
    /* 0x3D */ ENTR_SCENE_ROAD_TO_SOUTHERN_SWAMP,
    /* 0x3E */ ENTR_SCENE_DOGGY_RACETRACK,
    /* 0x3F */ ENTR_SCENE_CUCCO_SHACK,
    /* 0x40 */ ENTR_SCENE_IKANA_GRAVEYARD,
    /* 0x41 */ ENTR_SCENE_GOHTS_LAIR,
    /* 0x42 */ ENTR_SCENE_SOUTHERN_SWAMP_POISONED,
    /* 0x43 */ ENTR_SCENE_WOODFALL,
    /* 0x44 */ ENTR_SCENE_MOON_ZORA_TRIAL,
    /* 0x45 */ ENTR_SCENE_GORON_VILLAGE_SPRING,
    /* 0x46 */ ENTR_SCENE_GREAT_BAY_TEMPLE,
    /* 0x47 */ ENTR_SCENE_WATERFALL_RAPIDS,
    /* 0x48 */ ENTR_SCENE_BENEATH_THE_WELL,
    /* 0x49 */ ENTR_SCENE_ZORA_HALL_ROOMS,
    /* 0x4A */ ENTR_SCENE_GORON_VILLAGE_WINTER,
    /* 0x4B */ ENTR_SCENE_GORON_GRAVERYARD,
    /* 0x4C */ ENTR_SCENE_SAKONS_HIDEOUT,
    /* 0x4D */ ENTR_SCENE_MOUNTAIN_VILLAGE_WINTER,
    /* 0x4E */ ENTR_SCENE_GHOST_HUT,
    /* 0x4F */ ENTR_SCENE_DEKU_SHRINE,
    /* 0x50 */ ENTR_SCENE_ROAD_TO_IKANA,
    /* 0x51 */ ENTR_SCENE_SWORDMANS_SCHOOL,
    /* 0x52 */ ENTR_SCENE_MUSIC_BOX_HOUSE,
    /* 0x53 */ ENTR_SCENE_IGOS_DU_IKANAS_LAIR,
    /* 0x54 */ ENTR_SCENE_TOURIST_INFORMATION,
    /* 0x55 */ ENTR_SCENE_STONE_TOWER,
    /* 0x56 */ ENTR_SCENE_STONE_TOWER_INVERTED,
    /* 0x57 */ ENTR_SCENE_MOUNTAIN_VILLAGE_SPRING,
    /* 0x58 */ ENTR_SCENE_PATH_TO_SNOWHEAD,
    /* 0x59 */ ENTR_SCENE_SNOWHEAD,
    /* 0x5A */ ENTR_SCENE_PATH_TO_GORON_VILLAGE_WINTER,
    /* 0x5B */ ENTR_SCENE_PATH_TO_GORON_VILLAGE_SPRING,
    /* 0x5C */ ENTR_SCENE_GYORGS_LAIR,
    /* 0x5D */ ENTR_SCENE_SECRET_SHRINE,
    /* 0x5E */ ENTR_SCENE_STOCK_POT_INN,
    /* 0x5F */ ENTR_SCENE_GREAT_BAY_CUTSCENE,
    /* 0x60 */ ENTR_SCENE_CLOCK_TOWER_INTERIOR,
    /* 0x61 */ ENTR_SCENE_WOODS_OF_MYSTERY,
    /* 0x62 */ ENTR_SCENE_LOST_WOODS,
    /* 0x63 */ ENTR_SCENE_MOON_LINK_TRIAL,
    /* 0x64 */ ENTR_SCENE_THE_MOON,
    /* 0x65 */ ENTR_SCENE_BOMB_SHOP,
    /* 0x66 */ ENTR_SCENE_GIANTS_CHAMBER,
    /* 0x67 */ ENTR_SCENE_GORMAN_TRACK,
    /* 0x68 */ ENTR_SCENE_GORON_RACETRACK,
    /* 0x69 */ ENTR_SCENE_EAST_CLOCK_TOWN,
    /* 0x6A */ ENTR_SCENE_WEST_CLOCK_TOWN,
    /* 0x6B */ ENTR_SCENE_NORTH_CLOCK_TOWN,
    /* 0x6C */ ENTR_SCENE_SOUTH_CLOCK_TOWN,
    /* 0x6D */ ENTR_SCENE_LAUNDRY_POOL,
    /* 0x6E */ ENTR_SCENE_MAX
} EntranceSceneId;

/*
* 0xFE00:  Index into sSceneEntranceTable (Scene)
* 0x01F0:  Index into the scenes specific entrance table (Spawn)
* 0x000F:  Index into the specific entrance table (Layer), stored separately in sceneLayer
*/
#define ENTRANCE(scene, spawn) ((((ENTR_SCENE_##scene) & 0x7F) << 9) | (((spawn) & 0x1F) << 4))

#define ENTR_LOAD_OPENING -1

/*
* Entrances used in cutscene destination. Includes scene layer that's immediately applied to `nextCutsceneIndex` and removed.
* See `CutsceneScriptEntry.nextEntrance`
* 0xFE00:  Index into sSceneEntranceTable (Scene)
* 0x01F0:  Index into the scenes specific entrance table (Spawn)
* 0x000F:  Index into the specific entrance table (Layer)
*/
#define CS_ENTRANCE(scene, spawn, layer) ((((ENTR_SCENE_##scene) & 0x7F) << 9) | (((spawn) & 0x1F) << 4) | ((layer) & 0xF))

// SceneTableEntry draw configs
typedef enum {
    /* 0 */ SCENE_DRAW_CFG_DEFAULT,
    /* 1 */ SCENE_DRAW_CFG_MAT_ANIM,
    /* 2 */ SCENE_DRAW_CFG_NOTHING,
    /* 3 */ SCENE_DRAW_CFG_UNUSED_3,
    /* 4 */ SCENE_DRAW_CFG_UNUSED_4,
    /* 5 */ SCENE_DRAW_CFG_UNUSED_5,
    /* 6 */ SCENE_DRAW_CFG_GREAT_BAY_TEMPLE,
    /* 7 */ SCENE_DRAW_CFG_MAT_ANIM_MANUAL_STEP
} SceneDrawConfigIds;

// TODO: make ZAPD use this enum for `SCENE_CMD_SPECIAL_FILES`
// Leftover from OoT
typedef enum {
    /* 0 */ NAVI_QUEST_HINTS_NONE,
    /* 1 */ NAVI_QUEST_HINTS_OVERWORLD,
    /* 2 */ NAVI_QUEST_HINTS_DUNGEON
} NaviQuestHintFileId;

// SceneTableEntry commands
typedef enum {
    /* 0x00 */ SCENE_CMD_ID_SPAWN_LIST,
    /* 0x01 */ SCENE_CMD_ID_ACTOR_LIST,
    /* 0x02 */ SCENE_CMD_ID_ACTOR_CUTSCENE_CAM_LIST,
    /* 0x03 */ SCENE_CMD_ID_COL_HEADER,
    /* 0x04 */ SCENE_CMD_ID_ROOM_LIST,
    /* 0x05 */ SCENE_CMD_ID_WIND_SETTINGS,
    /* 0x06 */ SCENE_CMD_ID_ENTRANCE_LIST,
    /* 0x07 */ SCENE_CMD_ID_SPECIAL_FILES,
    /* 0x08 */ SCENE_CMD_ID_ROOM_BEHAVIOR,
    /* 0x09 */ SCENE_CMD_ID_UNK_09,
    /* 0x0A */ SCENE_CMD_ID_ROOM_SHAPE,
    /* 0x0B */ SCENE_CMD_ID_OBJECT_LIST,
    /* 0x0C */ SCENE_CMD_ID_LIGHT_LIST,
    /* 0x0D */ SCENE_CMD_ID_PATH_LIST,
    /* 0x0E */ SCENE_CMD_ID_TRANSI_ACTOR_LIST,
    /* 0x0F */ SCENE_CMD_ID_ENV_LIGHT_SETTINGS,
    /* 0x10 */ SCENE_CMD_ID_TIME_SETTINGS,
    /* 0x11 */ SCENE_CMD_ID_SKYBOX_SETTINGS,
    /* 0x12 */ SCENE_CMD_ID_SKYBOX_DISABLES,
    /* 0x13 */ SCENE_CMD_ID_EXIT_LIST,
    /* 0x14 */ SCENE_CMD_ID_END,
    /* 0x15 */ SCENE_CMD_ID_SOUND_SETTINGS,
    /* 0x16 */ SCENE_CMD_ID_ECHO_SETTINGS,
    /* 0x17 */ SCENE_CMD_ID_CUTSCENE_SCRIPT_LIST,
    /* 0x18 */ SCENE_CMD_ID_ALTERNATE_HEADER_LIST,
    /* 0x19 */ SCENE_CMD_ID_SET_REGION_VISITED,
    /* 0x1A */ SCENE_CMD_ID_ANIMATED_MATERIAL_LIST,
    /* 0x1B */ SCENE_CMD_ID_ACTOR_CUTSCENE_LIST,
    /* 0x1C */ SCENE_CMD_ID_MAP_DATA,
    /* 0x1D */ SCENE_CMD_ID_UNUSED_1D,
    /* 0x1E */ SCENE_CMD_ID_MAP_DATA_CHESTS,
    /* 0x1F */ SCENE_CMD_MAX
} SceneCommandTypeId;

#define SCENE_CMD_SPAWN_LIST(numSpawns, spawnList) \
    { SCENE_CMD_ID_SPAWN_LIST, numSpawns, CMD_PTR(spawnList) }

#define SCENE_CMD_ACTOR_LIST(numActors, actorList) \
    { SCENE_CMD_ID_ACTOR_LIST, numActors, CMD_PTR(actorList) }

#define SCENE_CMD_ACTOR_CUTSCENE_CAM_LIST(numCams, camList) \
    { SCENE_CMD_ID_ACTOR_CUTSCENE_CAM_LIST, numCams, CMD_PTR(camList) }

#define SCENE_CMD_COL_HEADER(colHeader) \
    { SCENE_CMD_ID_COL_HEADER, 0, CMD_PTR(colHeader) }

#define SCENE_CMD_ROOM_LIST(numRooms, roomList) \
    { SCENE_CMD_ID_ROOM_LIST, numRooms, CMD_PTR(roomList) }

#define SCENE_CMD_WIND_SETTINGS(xDir, yDir, zDir, strength) \
    { SCENE_CMD_ID_WIND_SETTINGS, 0, CMD_BBBB(xDir, yDir, zDir, strength) }

#define SCENE_CMD_ENTRANCE_LIST(entranceList) \
    { SCENE_CMD_ID_ENTRANCE_LIST, 0, CMD_PTR(entranceList) }

#define SCENE_CMD_SPECIAL_FILES(naviQuestHintFileId, keepObjectId) \
    { SCENE_CMD_ID_SPECIAL_FILES, naviQuestHintFileId, CMD_W(keepObjectId) }

#define SCENE_CMD_ROOM_BEHAVIOR(type, environment, lensMode, msgCtxunk12044, enablePosLights, stormState)        \
    { SCENE_CMD_ID_ROOM_BEHAVIOR, type,                                                                          \
      environment | _SHIFTL(lensMode, 8, 1) | _SHIFTL(msgCtxunk12044, 10, 1) | _SHIFTL(enablePosLights, 11, 1) | \
          _SHIFTL(stormState, 12, 1) }

#define SCENE_CMD_UNK_09() \
    { SCENE_CMD_ID_UNK_09, 0, CMD_W(0) }

#define SCENE_CMD_ROOM_SHAPE(roomShape) \
    { SCENE_CMD_ID_ROOM_SHAPE, 0, CMD_PTR(roomShape) }

#define SCENE_CMD_OBJECT_LIST(numObjects, objectList) \
    { SCENE_CMD_ID_OBJECT_LIST, numObjects, CMD_PTR(objectList) }

#define SCENE_CMD_LIGHT_LIST(numLights, lightList) \
    { SCENE_CMD_ID_LIGHT_LIST, numLights, CMD_PTR(lightList) }

#define SCENE_CMD_PATH_LIST(pathList) \
    { SCENE_CMD_ID_PATH_LIST, 0, CMD_PTR(pathList) }

#define SCENE_CMD_TRANSITION_ACTOR_LIST(numTransitionActors, actorList) \
    { SCENE_CMD_ID_TRANSI_ACTOR_LIST, numTransitionActors, CMD_PTR(actorList) }

#define SCENE_CMD_ENV_LIGHT_SETTINGS(numLightSettings, lightSettingsList) \
    { SCENE_CMD_ID_ENV_LIGHT_SETTINGS, numLightSettings, CMD_PTR(lightSettingsList) }

#define SCENE_CMD_TIME_SETTINGS(hour, min, speed) \
    { SCENE_CMD_ID_TIME_SETTINGS, 0, CMD_BBBB(hour, min, speed, 0) }

#define SCENE_CMD_SKYBOX_SETTINGS(externalTextureFileId, skyboxId, weather, lightMode) \
    { SCENE_CMD_ID_SKYBOX_SETTINGS, externalTextureFileId, CMD_BBBB(skyboxId, weather, lightMode, 0) }

#define SCENE_CMD_SKYBOX_DISABLES(disableSky, disableSunMoon) \
    { SCENE_CMD_ID_SKYBOX_DISABLES, 0, CMD_BBBB(disableSky, disableSunMoon, 0, 0) }

#define SCENE_CMD_EXIT_LIST(exitList) \
    { SCENE_CMD_ID_EXIT_LIST, 0, CMD_PTR(exitList) }

#define SCENE_CMD_END() \
    { SCENE_CMD_ID_END, 0, CMD_W(0) }

#define SCENE_CMD_SOUND_SETTINGS(specId, ambienceId, seqId) \
    { SCENE_CMD_ID_SOUND_SETTINGS, specId, CMD_BBBB(0, 0, ambienceId, seqId) }

#define SCENE_CMD_ECHO_SETTINGS(echo) \
    { SCENE_CMD_ID_ECHO_SETTINGS, 0, CMD_BBBB(0, 0, 0, echo) }

#define SCENE_CMD_CUTSCENE_SCRIPT_LIST(numEntries, scriptList) \
    { SCENE_CMD_ID_CUTSCENE_SCRIPT_LIST, numEntries, CMD_PTR(scriptList) }

#define SCENE_CMD_ALTERNATE_HEADER_LIST(alternateHeaderList) \
    { SCENE_CMD_ID_ALTERNATE_HEADER_LIST, 0, CMD_PTR(alternateHeaderList) }

#define SCENE_CMD_SET_REGION_VISITED() \
    { SCENE_CMD_ID_SET_REGION_VISITED, 0, CMD_W(0) }

#define SCENE_CMD_ANIMATED_MATERIAL_LIST(matAnimList) \
    { SCENE_CMD_ID_ANIMATED_MATERIAL_LIST, 0, CMD_PTR(matAnimList) }

#define SCENE_CMD_ACTOR_CUTSCENE_LIST(numEntries, actorCutsceneList) \
    { SCENE_CMD_ID_ACTOR_CUTSCENE_LIST, numEntries, CMD_PTR(actorCutsceneList) }

#define SCENE_CMD_MAP_DATA(mapData) \
    { SCENE_CMD_ID_MAP_DATA, 0, CMD_PTR(mapData) }

#define SCENE_CMD_MAP_DATA_CHESTS(chestCount, chestInfo) \
    { SCENE_CMD_ID_MAP_DATA_CHESTS, chestCount, CMD_PTR(chestInfo) }

#define SCENE_CMD_MINIMAP_INFO SCENE_CMD_MAP_DATA
#define SCENE_CMD_MINIMAP_COMPASS_ICON_INFO SCENE_CMD_MAP_DATA_CHESTS

s32 Object_SpawnPersistent(ObjectContext* objectCtx, s16 id);
void Object_InitContext(struct GameState* gameState, ObjectContext* objectCtx);
void Object_UpdateEntries(ObjectContext* objectCtx);
s32 Object_GetSlot(ObjectContext* objectCtx, s16 objectId);
s32 Object_IsLoaded(ObjectContext* objectCtx, s32 slot);
void Object_LoadAll(ObjectContext* objectCtx);
void* func_8012F73C(ObjectContext* objectCtx, s32 slot, s16 id);
void Scene_CommandSpawnList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandActorList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandActorCutsceneCamList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandCollisionHeader(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandRoomList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandEntranceList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandSpecialFiles(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandRoomBehavior(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandMesh(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandObjectList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandLightList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandPathList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandTransitionActorList(struct PlayState* play, SceneCmd* cmd);
void Scene_ResetTransitionActorList(GameState* gameState, TransitionActorList* transitionActors);
void Scene_CommandEnvLightSettings(struct PlayState* play, SceneCmd* cmd);
void Scene_LoadAreaTextures(struct PlayState* play, s32 fileIndex);
void Scene_CommandSkyboxSettings(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandSkyboxDisables(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandTimeSettings(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandWindSettings(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandExitList(struct PlayState* play, SceneCmd* cmd);
void Scene_Command09(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandSoundSettings(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandEchoSetting(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandAltHeaderList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandCutsceneScriptList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandCutsceneList(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandMapData(struct PlayState* play, SceneCmd* cmd);
void Scene_Command1D(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandMapDataChests(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandSetRegionVisitedFlag(struct PlayState* play, SceneCmd* cmd);
void Scene_CommandAnimatedMaterials(struct PlayState* play, SceneCmd* cmd);
void Scene_SetExitFade(struct PlayState* play);
s32 Scene_ExecuteCommands(struct PlayState* play, SceneCmd* sceneCmd);
u16 Entrance_Create(s32 scene, s32 spawn, s32 layer);
u16 Entrance_CreateFromSpawn(s32 spawn);
void Scene_Draw(struct PlayState* play);
void Scene_DrawConfigDefault(struct PlayState* play);
Gfx* AnimatedMat_TexScroll(struct PlayState* play, AnimatedMatTexScrollParams* params);
void AnimatedMat_DrawTexScroll(struct PlayState* play, s32 segment, void* params);
Gfx* AnimatedMat_TwoLayerTexScroll(struct PlayState* play, AnimatedMatTexScrollParams* params);
void AnimatedMat_DrawTwoTexScroll(struct PlayState* play, s32 segment, void* params);
void AnimatedMat_SetColor(struct PlayState* play, s32 segment, F3DPrimColor* primColorResult, F3DEnvColor* envColor);
void AnimatedMat_DrawColor(struct PlayState* play, s32 segment, void* params);
s32 AnimatedMat_Lerp(s32 min, s32 max, f32 norm);
void AnimatedMat_DrawColorLerp(struct PlayState* play, s32 segment, void* params);
f32 Scene_LagrangeInterp(s32 n, f32 x[], f32 fx[], f32 xp);
u8 Scene_LagrangeInterpColor(s32 n, f32 x[], f32 fx[], f32 xp);
void AnimatedMat_DrawColorNonLinearInterp(struct PlayState* play, s32 segment, void* params);
void AnimatedMat_DrawTexCycle(struct PlayState* play, s32 segment, void* params);
void AnimatedMat_DrawMain(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step, u32 flags);
void AnimatedMat_Draw(struct PlayState* play, AnimatedMaterial* matAnim);
void AnimatedMat_DrawOpa(struct PlayState* play, AnimatedMaterial* matAnim);
void AnimatedMat_DrawXlu(struct PlayState* play, AnimatedMaterial* matAnim);
void AnimatedMat_DrawAlpha(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio);
void AnimatedMat_DrawAlphaOpa(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio);
void AnimatedMat_DrawAlphaXlu(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio);
void AnimatedMat_DrawStep(struct PlayState* play, AnimatedMaterial* matAnim, u32 step);
void AnimatedMat_DrawStepOpa(struct PlayState* play, AnimatedMaterial* matAnim, u32 step);
void AnimatedMat_DrawStepXlu(struct PlayState* play, AnimatedMaterial* matAnim, u32 step);
void AnimatedMat_DrawAlphaStep(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step);
void AnimatedMat_DrawAlphaStepOpa(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step);
void AnimatedMat_DrawAlphaStepXlu(struct PlayState* play, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step);
void Scene_DrawConfigMatAnim(struct PlayState* play);
void Scene_DrawConfig3(struct PlayState* play);
void Scene_DrawConfig4(struct PlayState* play);
void Scene_DrawConfigDoNothing(struct PlayState* play);
void Scene_SetRenderModeXlu(struct PlayState* play, s32 index, u32 flags);
void Scene_SetCullFlag(struct PlayState* play, s32 index, u32 flags);
void Scene_DrawConfig5(struct PlayState* play);
void Scene_DrawConfigMatAnimManualStep(struct PlayState* play);
void Scene_DrawConfigGreatBayTemple(struct PlayState* play);
EntranceTableEntry* Entrance_GetTableEntry(u16 entrance);
s32 Entrance_GetSceneId(u16 entrance);
s32 Entrance_GetSceneIdAbsolute(u16 entrance);
s32 Entrance_GetSpawnNum(u16 entrance);
s32 Entrance_GetTransitionFlags(u16 entrance);

#endif

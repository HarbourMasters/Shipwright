#ifndef Z64BGCHECK_H
#define Z64BGCHECK_H

#ifdef __cplusplus
#ifndef this
#define this thisx
#endif
extern "C" {
#endif
#include "z64actor.h"

struct PlayState;
//struct Actor;
//struct DynaPolyActor;

#define SS_NULL 0xFFFF

#define COLPOLY_NORMAL_FRAC (1.0f / SHT_MAX)
#define COLPOLY_SNORMAL(x) ((s16)((x) * SHT_MAX))
#define COLPOLY_GET_NORMAL(n) ((n)*COLPOLY_NORMAL_FRAC)
#define COLPOLY_VIA_FLAG_TEST(vIA, flags) ((vIA) & (((flags)&7) << 13))
#define COLPOLY_VTX_INDEX(vI) ((vI)&0x1FFF)
#define COLPOLY_VTX(vtxId, flags) ((((flags) & 7) << 13) | ((vtxId) & 0x1FFF))

#define DYNAPOLY_INVALIDATE_LOOKUP (1 << 0)

#define BGACTOR_NEG_ONE -1
#define BG_ACTOR_MAX 50
#define DYNA_WATERBOX_MAX 30
#define BGCHECK_SCENE BG_ACTOR_MAX
#define BGCHECK_Y_MAX 32000.0f
#define BGCHECK_Y_MIN -32000.0f
#define BGCHECK_XYZ_ABSMAX 32760.0f
#define BGCHECK_SUBDIV_OVERLAP 50
#define BGCHECK_SUBDIV_MIN 150.0f

// Macros for `WaterBox.properties`
#define WATERBOX_LIGHT_INDEX_NONE 0x1F // warns and defaults to 0
#define WATERBOX_ROOM(p) ((((s32)p) >> 13) & 0x3F)
#define WATERBOX_ROOM_ALL 0x3F // value for "room index" indicating "all rooms"
#define WATERBOX_FLAG_19 (1 << 19)
#define WATERBOX_PROPERTIES(bgCamIndex, lightIndex, room, setFlag19) \
    ((((bgCamIndex) & 0xFF) <<  0) | \
     (((lightIndex) & 0x1F) <<  8) | \
     (((room)       & 0x3F) << 13) | \
     (((setFlag19)  &    1) << 19))

// bccFlags (bgcheck check flags)
#define BGCHECK_CHECK_WALL (1 << 0)
#define BGCHECK_CHECK_FLOOR (1 << 1)
#define BGCHECK_CHECK_CEILING (1 << 2)
#define BGCHECK_CHECK_ONE_FACE (1 << 3)
#define BGCHECK_CHECK_DYNA (1 << 4)
#define BGCHECK_CHECK_ALL \
    (BGCHECK_CHECK_WALL | BGCHECK_CHECK_FLOOR | BGCHECK_CHECK_CEILING | BGCHECK_CHECK_ONE_FACE | BGCHECK_CHECK_DYNA)

// bciFlags (bgcheck ignore flags)
#define BGCHECK_IGNORE_NONE 0
#define BGCHECK_IGNORE_CEILING (1 << 0)
#define BGCHECK_IGNORE_WALL (1 << 1)
#define BGCHECK_IGNORE_FLOOR (1 << 2)

// xpFlags (poly exclusion flags)
#define COLPOLY_IGNORE_NONE 0
#define COLPOLY_IGNORE_CAMERA (1 << 0)
#define COLPOLY_IGNORE_ENTITY (1 << 1)
#define COLPOLY_IGNORE_PROJECTILES (1 << 2)

// CollisionContext flags
#define BGCHECK_FLAG_REVERSE_CONVEYOR_FLOW 1

// Macros for `SurfaceType`
#define SURFACETYPE0(bgCamIndex, exitIndex, floorType, unk18, wallType, floorProperty, isSoft, isHorseBlocked) \
    ((((bgCamIndex)     & 0xFF) <<  0) | \
     (((exitIndex)      & 0x1F) <<  8) | \
     (((floorType)      & 0x1F) << 13) | \
     (((unk18)          & 0x07) << 18) | \
     (((wallType)       & 0x1F) << 21) | \
     (((floorProperty)  & 0x0F) << 26) | \
     (((isSoft)         &    1) << 30) | \
     (((isHorseBlocked) &    1) << 31))

#define SURFACETYPE1(material, floorEffect, lightSetting, echo, canHookshot, conveyorSpeed, conveyorDirection, unk27) \
    ((((material)          & 0x0F) <<  0) | \
     (((floorEffect)       & 0x03) <<  4) | \
     (((lightSetting)      & 0x1F) <<  6) | \
     (((echo)              & 0x3F) << 11) | \
     (((canHookshot)       &    1) << 17) | \
     (((conveyorSpeed)     & 0x07) << 18) | \
     (((conveyorDirection) & 0x3F) << 21) | \
     (((unk27)             &    1) << 27))

typedef struct {
    /* 0x0 */ Vec3s pos;
} BgVertex; // size = 0x6

typedef struct {
    /* 0x00 */ Vec3f scale;
    /* 0x0C */ Vec3s rot;
    /* 0x14 */ Vec3f pos;
} ScaleRotPos; // size = 0x20

typedef struct CollisionPoly {
    /* 0x0 */ u16 type;
    union {
        /* 0x2 */ u16 vtxData[3];
        struct {
            /* 0x2 */ u16 flags_vIA; // 0xE000 is poly exclusion flags (xpFlags), 0x1FFF is vtxId
            /* 0x4 */ u16 flags_vIB; // 0xE000 is flags, 0x1FFF is vtxId
                                      // 0x2000 = poly IsConveyor surface
            /* 0x6 */ u16 vIC;
        };
    };
    /* 0x8 */ Vec3s normal; // Unit normal vector
                             // Value ranges from -0x7FFF to 0x7FFF, representing -1.0 to 1.0; 0x8000 is invalid

    /* 0xE */ s16 dist; // Plane distance from origin along the normal
} CollisionPoly; // size = 0x10

typedef struct {
    /* 0x0 */ u16 setting; // camera setting described by CameraSettingType enum
    /* 0x2 */ s16 count; // only used when `bgCamFuncData` is a list of points used for crawlspaces (unused oot legacy)
    /* 0x4 */ Vec3s* bgCamFuncData; // s16 data grouped in threes (ex. Vec3s), is usually of type `BgCamFuncData`
} BgCamInfo; // size = 0x8

typedef BgCamInfo CamData; // TODO: ZAPD compatibility

// The structure used for all instances of s16 data from `BgCamInfo`.
typedef struct {
    /* 0x00 */ Vec3s pos;
    /* 0x06 */ Vec3s rot;
    /* 0x0C */ s16 fov;
    /* 0x0E */ union {
        s16 roomImageOverrideBgCamIndex;
        s16 unk_0E;
    };
    /* 0x10 */ s16 unk_10;
} BgCamFuncData; // size = 0x12

typedef struct {
    /* 0x0 */ Vec3s minPos;
    /* 0x6 */ s16 xLength;
    /* 0x8 */ s16 zLength;
    /* 0xC */ u32 properties;
    // 0x0008_0000 = ?
    // 0x0007_E000 = room index, 0x3F = all rooms
    // 0x0000_1F00 = lighting setting index
    // 0x0000_00FF = bgCam index
} WaterBox; // size = 0x10

typedef enum FloorType {
    /*  0 */ FLOOR_TYPE_0,
    /*  1 */ FLOOR_TYPE_1,
    /*  2 */ FLOOR_TYPE_2,
    /*  3 */ FLOOR_TYPE_3,
    /*  4 */ FLOOR_TYPE_4,
    /*  5 */ FLOOR_TYPE_5,
    /*  6 */ FLOOR_TYPE_6,
    /*  7 */ FLOOR_TYPE_7,
    /*  8 */ FLOOR_TYPE_8,
    /*  9 */ FLOOR_TYPE_9,
    /* 10 */ FLOOR_TYPE_10,
    /* 11 */ FLOOR_TYPE_11,
    /* 12 */ FLOOR_TYPE_12,
    /* 13 */ FLOOR_TYPE_13,
    /* 14 */ FLOOR_TYPE_14,
    /* 15 */ FLOOR_TYPE_15
} FloorType;

typedef enum WallType {
    /*  0 */ WALL_TYPE_0,
    /*  1 */ WALL_TYPE_1,
    /*  2 */ WALL_TYPE_2,
    /*  3 */ WALL_TYPE_3,
    /*  4 */ WALL_TYPE_4,
    /*  5 */ WALL_TYPE_5,
    /*  6 */ WALL_TYPE_6,
    /*  7 */ WALL_TYPE_7,
    /*  8 */ WALL_TYPE_8,
    /*  9 */ WALL_TYPE_9,
    /* 10 */ WALL_TYPE_10,
    /* 11 */ WALL_TYPE_11,
    /* 12 */ WALL_TYPE_12,
    /* 32 */ WALL_TYPE_MAX = 32
} WallType;

#define WALL_FLAG_0 (1 << 0)
#define WALL_FLAG_1 (1 << 1)
#define WALL_FLAG_2 (1 << 2)
#define WALL_FLAG_3 (1 << 3)
#define WALL_FLAG_4 (1 << 4)
#define WALL_FLAG_5 (1 << 5)
#define WALL_FLAG_6 (1 << 6)

// SurfaceType can leave footsteps imprints in the material
#define MATERIAL_PROPERTY_SOFT_IMPRINT (1 << 0)

typedef enum FloorProperty {
    /*  0 */ FLOOR_PROPERTY_0,
    /*  1 */ FLOOR_PROPERTY_1,
    /*  2 */ FLOOR_PROPERTY_2,
    /*  5 */ FLOOR_PROPERTY_5 = 5,
    /*  6 */ FLOOR_PROPERTY_6,
    /*  7 */ FLOOR_PROPERTY_7,
    /*  8 */ FLOOR_PROPERTY_8,
    /*  9 */ FLOOR_PROPERTY_9,
    /* 11 */ FLOOR_PROPERTY_11 = 11,
    /* 12 */ FLOOR_PROPERTY_12,
    /* 13 */ FLOOR_PROPERTY_13
} FloorProperty;

typedef enum SurfaceSfxOffset {
    /*  0 */ SURFACE_SFX_OFFSET_DIRT,
    /*  1 */ SURFACE_SFX_OFFSET_SAND,
    /*  2 */ SURFACE_SFX_OFFSET_STONE,
    /*  3 */ SURFACE_SFX_OFFSET_DIRT_SHALLOW,
    /*  4 */ SURFACE_SFX_OFFSET_WATER_SHALLOW,
    /*  5 */ SURFACE_SFX_OFFSET_WATER_DEEP,
    /*  6 */ SURFACE_SFX_OFFSET_TALL_GRASS,
    /*  7 */ SURFACE_SFX_OFFSET_LAVA, // MAGMA?
    /*  8 */ SURFACE_SFX_OFFSET_GRASS,
    /*  9 */ SURFACE_SFX_OFFSET_CARPET,
    /* 10 */ SURFACE_SFX_OFFSET_WOOD,
    /* 11 */ SURFACE_SFX_OFFSET_BRIDGE, // WOOD_PLANK?
    /* 12 */ SURFACE_SFX_OFFSET_VINE, // METAL?
    /* 13 */ SURFACE_SFX_OFFSET_DIRT_DEEP,
    /* 14 */ SURFACE_SFX_OFFSET_SNOW,
    /* 15 */ SURFACE_SFX_OFFSET_ICE
} SurfaceSfxOffset;

typedef enum SurfaceMaterial {
    /*  0 */ SURFACE_MATERIAL_DIRT,
    /*  1 */ SURFACE_MATERIAL_SAND,
    /*  2 */ SURFACE_MATERIAL_STONE,
    /*  3 */ SURFACE_MATERIAL_DIRT_SHALLOW,
    /*  4 */ SURFACE_MATERIAL_WATER_SHALLOW,
    /*  5 */ SURFACE_MATERIAL_WATER_DEEP,
    /*  6 */ SURFACE_MATERIAL_TALL_GRASS,
    /*  7 */ SURFACE_MATERIAL_LAVA, // MAGMA?
    /*  8 */ SURFACE_MATERIAL_GRASS,
    /*  9 */ SURFACE_MATERIAL_BRIDGE, // WOOD_PLANK?
    /* 10 */ SURFACE_MATERIAL_WOOD,
    /* 11 */ SURFACE_MATERIAL_DIRT_SOFT,
    /* 12 */ SURFACE_MATERIAL_ICE,
    /* 13 */ SURFACE_MATERIAL_CARPET,
    /* 14 */ SURFACE_MATERIAL_SNOW,
    /* 15 */ SURFACE_MATERIAL_MAX
} SurfaceMaterial;

typedef enum FloorEffect {
    /* 0 */ FLOOR_EFFECT_0,
    /* 1 */ FLOOR_EFFECT_1,
    /* 2 */ FLOOR_EFFECT_2,
    /* 3 */ FLOOR_EFFECT_3
} FloorEffect;

typedef enum ConveyorSpeed {
    /* 0 */ CONVEYOR_SPEED_DISABLED,
    /* 1 */ CONVEYOR_SPEED_SLOW,
    /* 2 */ CONVEYOR_SPEED_MEDIUM,
    /* 3 */ CONVEYOR_SPEED_FAST,
    /* 4 */ CONVEYOR_SPEED_MAX
} ConveyorSpeed;

#define CONVEYOR_DIRECTION_TO_BINANG(conveyorDirection) ((conveyorDirection) * (0x10000 / 64))

typedef struct {
    /* 0x0 */ u32 data[2];

    // Word 0
    // 0x8000_0000 = is horse blocked
    // 0x4000_0000 = is soft
    // 0x3C00_0000 = floor property
    // 0x03E0_0000 = wall type
    // 0x001C_0000 = ?
    // 0x0003_E000 = floor type
    // 0x0000_1F00 = scene exit index
    // 0x0000_00FF = bgCam index
    // Word 1
    // 0x0800_0000 = wall damage
    // 0x07E0_0000 = conveyor direction
    // 0x001C_0000 = conveyor speed
    // 0x0002_0000 = is hookable surface
    // 0x0001_F800 = echo/reverb
    // 0x0000_07C0 = lighting setting index
    // 0x0000_0030 = floor effect
    // 0x0000_000F = material

} SurfaceType; // size = 0x8

typedef struct CollisionHeader {
    /* 0x00 */ Vec3s minBounds; // minimum coordinates of poly bounding box
    /* 0x06 */ Vec3s maxBounds; // maximum coordinates of poly bounding box
    /* 0x0C */ u16 numVertices;
    /* 0x10 */ Vec3s* vtxList;
    /* 0x14 */ u16 numPolygons;
    /* 0x18 */ CollisionPoly* polyList;
    /* 0x1C */ SurfaceType* surfaceTypeList;
    /* 0x20 */ BgCamInfo* bgCamList;
    /* 0x24 */ u16 numWaterBoxes;
    /* 0x28 */ WaterBox* waterBoxes;
} CollisionHeader; // size = 0x2C

typedef struct {
    /* 0x0 */ s16 polyId;
    /* 0x2 */ u16 next; // index of the next SSNode in the list, or SS_NULL if last element 
} SSNode; // size = 0x4

// represents a linked list of type SSNode
typedef struct {
    /* 0x0 */ u16 head; // index of the first SSNode in the list, or SS_NULL if the list is empty
} SSList; // size = 0x2

typedef struct {
    /* 0x0 */ u16 max;
    /* 0x2 */ u16 count;
    /* 0x4 */ SSNode* tbl;
    /* 0x8 */ u8* polyCheckTbl;
} SSNodeList; // size = 0xC

typedef struct {
    /* 0x0 */ SSNode* tbl;
    /* 0x4 */ u32 count;
    /* 0x8 */ s32 maxNodes;
} DynaSSNodeList; // size = 0xC

typedef struct {
    /* 0x0 */ s32 unk0;
    /* 0x4 */ WaterBox* boxes;
} DynaWaterBoxList; // size = 0x8

typedef struct {
    /* 0x0 */ SSList floor;
    /* 0x2 */ SSList wall;
    /* 0x4 */ SSList ceiling;
} StaticLookup; // size = 0x6

typedef struct {
    /* 0x0 */ s16 polyStartIndex;
    /* 0x2 */ SSList ceiling;
    /* 0x4 */ SSList wall;
    /* 0x6 */ SSList floor;
} DynaLookup; // size = 0x8

typedef struct {
    /* 0x00 */ Actor* actor;
    /* 0x04 */ CollisionHeader* colHeader;
    /* 0x08 */ DynaLookup dynaLookup;
    /* 0x10 */ u16 vtxStartIndex;
    /* 0x12 */ u16 waterboxesStartIndex;
    /* 0x14 */ ScaleRotPos prevTransform;
    /* 0x34 */ ScaleRotPos curTransform;
    /* 0x54 */ Sphere16 boundingSphere;
    /* 0x5C */ f32 minY;
    /* 0x60 */ f32 maxY;
} BgActor; // size = 0x64

#define BGACTOR_IN_USE (1 << 0) // The bgActor entry is in use
#define BGACTOR_1 (1 << 1)
#define BGACTOR_COLLISION_DISABLED (1 << 2) // The collision of the bgActor is disabled
#define BGACTOR_CEILING_COLLISION_DISABLED (1 << 3) // The ceiling collision of the bgActor is ignored
#define BGACTOR_FLOOR_COLLISION_DISABLED (1 << 5) // The floor collision of the bgActor is ignored

typedef struct {
    /* 0x0000 */ u8 bitFlag;
    /* 0x0004 */ BgActor bgActors[BG_ACTOR_MAX];
    /* 0x138C */ u16 bgActorFlags[BG_ACTOR_MAX];
    /* 0x13F0 */ CollisionPoly* polyList;
    /* 0x13F4 */ Vec3s* vtxList;
    /* 0x13F8 */ DynaWaterBoxList waterBoxList;
    /* 0x1400 */ DynaSSNodeList polyNodes;
    /* 0x140C */ s32 polyNodesMax;
    /* 0x1410 */ s32 polyListMax;
    /* 0x1414 */ s32 vtxListMax;
} DynaCollisionContext; // size = 0x1418

typedef struct CollisionContext {
    /* 0x0000 */ CollisionHeader* colHeader; // scene's static collision
    /* 0x0004 */ Vec3f minBounds;            // minimum coordinates of collision bounding box
    /* 0x0010 */ Vec3f maxBounds;            // maximum coordinates of collision bounding box
    /* 0x001C */ Vec3i subdivAmount;         // x, y, z subdivisions of the scene's static collision
    /* 0x0028 */ Vec3f subdivLength;         // x, y, z subdivision worldspace lengths
    /* 0x0034 */ Vec3f subdivLengthInv;      // inverse of subdivision length
    /* 0x0040 */ StaticLookup* lookupTbl;    // 3d array of length subdivAmount
    /* 0x0044 */ SSNodeList polyNodes;
    /* 0x0050 */ DynaCollisionContext dyna;
    /* 0x1468 */ u32 memSize; // Size of all allocated memory plus CollisionContext
    /* 0x146C */ u32 flags;   // bit 0 reverses conveyor direction (i.e. water flow in Great Bay Temple)
} CollisionContext; // size = 0x1470

typedef struct {
    /* 0x00 */ struct PlayState* play;
    /* 0x04 */ CollisionContext* colCtx;
    /* 0x08 */ u16 xpFlags;
    /* 0x0C */ CollisionPoly** resultPoly;
    /* 0x10 */ f32 yIntersect;
    /* 0x14 */ Vec3f* pos;
    /* 0x18 */ s32* bgId;
    /* 0x1C */ s32 unk1C;
    /* 0x20 */ struct Actor* actor;
    /* 0x24 */ u32 unk_24;
    /* 0x28 */ f32 checkDist;
    /* 0x2C */ DynaCollisionContext* dyna;
    /* 0x30 */ SSList* ssList;
} DynaRaycast; // size = 0x34

typedef struct
{
    /* 0x00 */ StaticLookup* lookup;
    /* 0x04 */ SSList* ssList;
    /* 0x08 */ CollisionContext* colCtx;
    /* 0x0C */ u16 xpFlags1;
    /* 0x0E */ u16 xpFlags2;
    /* 0x10 */ Vec3f* posA;
    /* 0x14 */ Vec3f* posB;
    /* 0x18 */ Vec3f* outPos;
    /* 0x1C */ CollisionPoly** outPoly;
    /* 0x20 */ f32 outDistSq;
    /* 0x24 */ f32 checkDist;
    /* 0x28 */ s32 bccFlags;
    /* 0x2C */ Actor* actor;
} StaticLineTest; // size = 0x30

typedef struct {
    /* 0x00 */ CollisionContext* colCtx;
    /* 0x04 */ u16 xpFlags;
    /* 0x08 */ DynaCollisionContext* dyna;
    /* 0x0C */ SSList* ssList;
    /* 0x10 */ Vec3f* posA;
    /* 0x14 */ Vec3f* posB;
    /* 0x18 */ Vec3f* posResult;
    /* 0x1C */ CollisionPoly** resultPoly;
    /* 0x20 */ s32 checkOneFace; // bccFlags & 0x8
    /* 0x24 */ f32* distSq;      // distance from posA to poly squared
    /* 0x28 */ f32 checkDist;    // distance from poly
    /* 0x2C */ Actor* actor;
    /* 0x30 */ s32 bgId;
} DynaLineTest; // size = 0x34

typedef struct {
    /* 0x00 */ CollisionPoly* poly;
    /* 0x04 */ Vec3s* vtxList;
    /* 0x08 */ Vec3f* posA;
    /* 0x0C */ Vec3f* posB;
    /* 0x10 */ Vec3f* planeIntersect;
    /* 0x14 */ s32 checkOneFace;
    /* 0x18 */ f32 checkDist;
} BgLineVsPolyTest; // size = 0x1C

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x4 */ u32 memSize;
} BgSpecialSceneMaxMemory; // size = 0x8

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ s16 maxNodes;
    /* 0x4 */ s16 maxPolygons;
    /* 0x6 */ s16 maxVertices;
} BgSpecialSceneMaxObjects; // size = 0x8

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ s16 xSubdivisions;
    /* 0x4 */ s16 ySubdivisions;
    /* 0x6 */ s16 zSubdivisions;
    /* 0x8 */ s32 unk8;
} BgSpecialSceneMeshSubdivision; // size = 0xC

typedef struct {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ Vec3s subdivAmount;
    /* 0x8 */ s32 nodeListMax; // if -1, dynamically compute max nodes
} BgCheckSceneSubdivisionEntry; // size = 0xC

void SSNode_SetValue(SSNode* node, s16* polyIndex, u16 next);
void SSList_SetNull(SSList* ssList);
void SSNodeList_SetSSListHead(SSNodeList* list, SSList* ssList, s16* polyIndex);
void DynaSSNodeList_SetSSListHead(DynaSSNodeList* list, SSList* ssList, s16* polyIndex);
void DynaSSNodeList_Init(struct PlayState* play, DynaSSNodeList* list);
void DynaSSNodeList_Alloc(struct PlayState* play, DynaSSNodeList* list, u32 numNodes);
void DynaSSNodeList_ResetCount(DynaSSNodeList* list);
u16 DynaSSNodeList_GetNextNodeIdx(DynaSSNodeList* list);
void BgCheck_Vec3sToVec3f(Vec3s* src, Vec3f* dest);
void BgCheck_Vec3fToVec3s(Vec3s* dest, Vec3f* src);
f32 func_800BFD84(CollisionPoly *poly, f32 arg1, f32 arg2);
s32 func_800BFDEC(CollisionPoly* polyA, CollisionPoly* polyB, u32* outVtxId0, u32* outVtxId1);
s16 CollisionPoly_GetMinY(CollisionPoly* poly, Vec3s* vertices);
void CollisionPoly_GetNormalF(CollisionPoly* poly, f32* nx, f32* ny, f32* nz);
void func_800C0094(CollisionPoly* poly, f32 tx, f32 ty, f32 tz, MtxF* dest);
f32 CollisionPoly_GetPointDistanceFromPlane(CollisionPoly* poly, Vec3f* point);
void CollisionPoly_GetVerticesByBgId(CollisionPoly* poly, s32 bgId, CollisionContext* colCtx, Vec3f* dest);
s32 CollisionPoly_SphVsPoly(CollisionPoly* poly, Vec3s* vtxList, Vec3f* center, f32 radius);
void StaticLookup_AddPolyToSSList(CollisionContext* colCtx, SSList* ssList, CollisionPoly* polyList, Vec3s* vtxList, s16 polyId);
void StaticLookup_AddPoly(StaticLookup* lookup, CollisionContext* colCtx, CollisionPoly* polyList, Vec3s* vtxList, s16 index);
void BgCheck_GetSubdivisionMinBounds(CollisionContext* colCtx, Vec3f* pos, s32* sx, s32* sy, s32* sz);
void BgCheck_GetSubdivisionMaxBounds(CollisionContext* colCtx, Vec3f* pos, s32* sx, s32* sy, s32* sz);
void BgCheck_GetPolySubdivisionBounds(CollisionContext* colCtx, Vec3s* vtxList, CollisionPoly* polyList, s32* subdivMinX, s32* subdivMinY, s32* subdivMinZ, s32* subdivMaxX, s32* subdivMaxY, s32* subdivMaxZ, s16 polyId);
s32 BgCheck_PolyIntersectsSubdivision(Vec3f* min, Vec3f* max, CollisionPoly* polyList, Vec3s* vtxList, s16 polyId);
u32 BgCheck_InitStaticLookup(CollisionContext* colCtx, struct PlayState* play, StaticLookup* lookupTbl);
s32 BgCheck_IsSmallMemScene(struct PlayState* play);
s32 BgCheck_TryGetCustomMemsize(s32 sceneId, u32* memSize);
void BgCheck_SetSubdivisionDimension(f32 min, s32 subdivAmount, f32* max, f32* subdivLength, f32* subdivLengthInv);
s32 BgCheck_GetSpecialSceneMaxObjects(struct PlayState* play, s32* maxNodes, s32* maxPolygons, s32* maxVertices);
void BgCheck_Allocate(CollisionContext* colCtx, struct PlayState* play, CollisionHeader* colHeader);
void BgCheck_SetContextFlags(CollisionContext* colCtx, u32 flags);
void BgCheck_UnsetContextFlags(CollisionContext* colCtx, u32 flags);
CollisionHeader* BgCheck_GetCollisionHeader(CollisionContext* colCtx, s32 bgId);
f32 BgCheck_RaycastFloorImpl(struct PlayState* play, CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, s32* outBgId, Vec3f* pos, Actor* actor, u32 arg7, f32 checkDist, s32 arg9);
f32 BgCheck_CameraRaycastFloor1(CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor1(CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor2(struct PlayState* play, CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor2_1(struct PlayState* play, CollisionContext* colCtx, CollisionPoly** outPoly, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor3(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor5(CollisionContext* colCtx, CollisionPoly** outPoly, s32* outBgId, Actor* actor, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor5_2(struct PlayState* play, CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor5_3(struct PlayState* play, CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor6(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos, f32 checkDist);
f32 BgCheck_EntityRaycastFloor7(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos);
f32 BgCheck_AnyRaycastFloor1(CollisionContext* colCtx, CollisionPoly* outPoly, Vec3f* pos);
f32 BgCheck_AnyRaycastFloor2(CollisionContext* colCtx, CollisionPoly* outPoly, s32* bgId, Vec3f* pos);
f32 BgCheck_CameraRaycastFloor2(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor8(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Actor* actor, Vec3f* pos);
f32 BgCheck_EntityRaycastFloor9(CollisionContext* colCtx, CollisionPoly** outPoly, s32* bgId, Vec3f* pos);
s32 BgCheck_CheckWallImpl(CollisionContext* colCtx, u16 xpFlags, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight, u8 argA);
s32 BgCheck_EntitySphVsWall1(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius, CollisionPoly** outPoly, f32 checkHeight);
s32 BgCheck_EntitySphVsWall2(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius, CollisionPoly** outPoly, s32* outBgId, f32 checkHeight);
s32 BgCheck_EntitySphVsWall3(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight);
s32 BgCheck_EntitySphVsWall4(CollisionContext* colCtx, Vec3f* posResult, Vec3f* posNext, Vec3f* posPrev, f32 radius, CollisionPoly** outPoly, s32* outBgId, Actor* actor, f32 checkHeight);
s32 BgCheck_CheckCeilingImpl(CollisionContext* colCtx, u16 xpFlags, f32* outY, Vec3f* pos, f32 checkHeight, CollisionPoly** outPoly, s32* outBgId, Actor* actor);
s32 BgCheck_AnyCheckCeiling(CollisionContext* colCtx, f32* outY, Vec3f* pos, f32 checkHeight);
s32 BgCheck_EntityCheckCeiling(CollisionContext* colCtx, f32* outY, Vec3f* pos, f32 checkHeight, CollisionPoly** outPoly, s32* outBgId, Actor* actor);
s32 BgCheck_CameraLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId);
s32 BgCheck_CameraLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId);
s32 BgCheck_EntityLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId);
s32 BgCheck_EntityLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId, Actor* actor);
s32 BgCheck_EntityLineTest3(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId, Actor* actor, f32 checkDist);
s32 BgCheck_ProjectileLineTest(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId);
s32 BgCheck_AnyLineTest1(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkOneFace);
s32 BgCheck_AnyLineTest2(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace);
s32 BgCheck_AnyLineTest3(CollisionContext* colCtx, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32 checkWall, s32 checkFloor, s32 checkCeil, s32 checkOneFace, s32* bgId);
s32 BgCheck_SphVsFirstPolyImpl(CollisionContext* colCtx, u16 xpFlags, CollisionPoly** outPoly, s32* outBgId, Vec3f* center, f32 radius, Actor* actor, u16 bciFlags);
s32 BgCheck_SphVsFirstPoly(CollisionContext* colCtx, Vec3f* center, f32 radius);
s32 BgCheck_SphVsFirstWall(CollisionContext* colCtx, Vec3f* center, f32 radius);
void SSNodeList_Init(SSNodeList* this);
void SSNodeList_Alloc(struct PlayState* play, SSNodeList* thisx, s32 tblMax, s32 numPolys);
SSNode* SSNodeList_GetNextNode(SSNodeList* this);
u16 SSNodeList_GetNextNodeIdx(SSNodeList* this);
void ScaleRotPos_Init(ScaleRotPos* srp);
void ScaleRotPos_SetValue(ScaleRotPos* srp, Vec3f* scale, Vec3s* rot, Vec3f* pos);
s32 ScaleRotPos_IsEqual(ScaleRotPos* a, ScaleRotPos* b);
void DynaLookup_ResetLists(DynaLookup* dynaLookup);
void DynaLookup_Reset(DynaLookup* dynaLookup);
void DynaLookup_ResetVtxStartIndex(u16* vtxStartIndex);
void DynaLookup_ResetWaterBoxStartIndex(u16* waterBoxStartIndex);
void BgActor_Init(struct PlayState* play, BgActor* bgActor);
void BgActor_SetActor(BgActor* bgActor, Actor* actor, CollisionHeader* colHeader);
s32 BgActor_IsTransformUnchanged(BgActor* bgActor);
void DynaPoly_NullPolyList(CollisionPoly** polyList);
void DynaPoly_AllocPolyList(struct PlayState* play, CollisionPoly** polyList, s32 numPolys);
void DynaPoly_NullVtxList(Vec3s** vtxList);
void DynaPoly_AllocVtxList(struct PlayState* play, Vec3s** vtxList, s32 numVtx);
void DynaPoly_InitWaterBoxList(DynaWaterBoxList* waterBoxList);
void DynaPoly_AllocWaterBoxList(struct PlayState* play, DynaWaterBoxList* waterBoxList, s32 numWaterBoxes);
void DynaPoly_SetBgActorPrevTransform(struct PlayState* play, BgActor* bgActor);
s32 DynaPoly_IsBgIdBgActor(s32 bgId);
void DynaPoly_Init(struct PlayState* play, DynaCollisionContext* dyna);
void DynaPoly_Alloc(struct PlayState* play, DynaCollisionContext* dyna);
s32 DynaPoly_SetBgActor(struct PlayState* play, DynaCollisionContext* dyna, Actor* actor, CollisionHeader* colHeader);
DynaPolyActor* DynaPoly_GetActor(CollisionContext* colCtx, s32 bgId);
void DynaPoly_DisableCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_EnableCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_DisableCeilingCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_EnableCeilingCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_DisableFloorCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_EnableFloorCollision(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void DynaPoly_InvalidateLookup(struct PlayState* play, DynaCollisionContext* dyna);
void DynaPoly_DeleteBgActor(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId);
void BgCheck_CalcWaterboxDimensions(Vec3f* minPos, Vec3f* maxXPos, Vec3f* maxZPos, Vec3s* minPosOut, s16* xLength, s16* zLength);
void DynaPoly_AddBgActorToLookup(struct PlayState* play, DynaCollisionContext* dyna, s32 bgId, s32* vtxStartIndex, s32* polyStartIndex, s32* waterBoxStartIndex);
void DynaPoly_UnsetAllInteractFlags(struct PlayState* play, DynaCollisionContext* dyna, Actor* actor);
void DynaPoly_UpdateContext(struct PlayState* play, DynaCollisionContext* dyna);
void func_800C756C(DynaCollisionContext* dyna, s32* numPolygons, s32* numVertices, s32* numWaterBoxes);
void DynaPoly_UpdateBgActorTransforms(struct PlayState* play, DynaCollisionContext* dyna);
void CollisionHeader_SegmentedToVirtual(CollisionHeader* colHeader);
void CollisionHeader_GetVirtual(CollisionHeader* colHeader, CollisionHeader** dest);
void BgCheck_InitCollisionHeaders(CollisionContext* colCtx, struct PlayState* play);

u32 SurfaceType_GetBgCamIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u16 BgCheck_GetBgCamSettingImpl(CollisionContext* colCtx, u32 bgCamIndex, s32 bgId);
u16 BgCheck_GetBgCamSetting(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u16 BgCheck_GetBgCamCountImpl(CollisionContext* colCtx, u32 bgCamIndex, s32 bgId);
u16 BgCheck_GetBgCamCount(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
Vec3s* BgCheck_GetBgCamFuncDataImpl(CollisionContext* colCtx, s32 bgCamIndex, s32 bgId);
Vec3s* BgCheck_GetBgCamFuncData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);

u32 SurfaceType_GetSceneExitIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
FloorType SurfaceType_GetFloorType(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 func_800C99FC(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_GetWallFlags(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_CheckWallFlag0(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_CheckWallFlag1(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_CheckWallFlag2(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
FloorProperty SurfaceType_GetFloorProperty2(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
FloorProperty SurfaceType_GetFloorProperty(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_IsSoft(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_IsHorseBlocked(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
SurfaceMaterial SurfaceType_GetMaterial(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u16 SurfaceType_GetSfxOffset(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_HasMaterialProperty(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 propertyType);
FloorEffect SurfaceType_GetFloorEffect(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_GetLightSettingIndex(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_GetEcho(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_IsHookshotSurface(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_IsIgnoredByEntities(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_IsIgnoredByProjectiles(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 SurfaceType_IsFloorConveyor(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 func_800C9DDC(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
ConveyorSpeed SurfaceType_GetConveyorSpeed(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_GetConveyorDirection(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
u32 SurfaceType_IsWallDamage(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
s32 WaterBox_GetSurfaceImpl(struct PlayState* play, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface, WaterBox** outWaterBox, s32* bgId);
s32 WaterBox_GetSurface1(struct PlayState* play, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface, WaterBox** outWaterBox);
s32 WaterBox_GetSurface1_2(struct PlayState* play, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface, WaterBox** outWaterBox);
s32 WaterBox_GetSurface2(struct PlayState* play, CollisionContext* colCtx, Vec3f* pos, f32 surfaceCheckDist, WaterBox** outWaterBox, s32* bgId);
f32 func_800CA568(CollisionContext* colCtx, s32 waterBoxId, s32 bgId);
u16 WaterBox_GetBgCamSetting(CollisionContext* colCtx, WaterBox* waterBox, s32 bgId);
void WaterBox_GetSceneBgCamSetting(CollisionContext* colCtx, WaterBox* waterBox);
u32 WaterBox_GetLightSettingIndex(CollisionContext* colCtx, WaterBox* waterBox);
s32 func_800CA6F0(struct PlayState* play, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface, WaterBox** outWaterBox, s32* bgId);
s32 func_800CA9D0(struct PlayState* play, CollisionContext* colCtx, f32 x, f32 z, f32* ySurface, WaterBox** outWaterBox);
s32 func_800CAA14(CollisionPoly* polyA, CollisionPoly* polyB, Vec3f* pointA, Vec3f* pointB, Vec3f* closestPoint);

#ifdef __cplusplus
}
#undef this
#endif


#endif

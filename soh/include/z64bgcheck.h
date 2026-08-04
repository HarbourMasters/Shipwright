#ifndef Z_BGCHECK_H
#define Z_BGCHECK_H

struct PlayState;
struct Actor;
struct DynaPolyActor;

#define COLPOLY_NORMAL_FRAC (1.0f / SHT_MAX)
#define COLPOLY_SNORMAL(x) ((s16)((x) * SHT_MAX))
#define COLPOLY_GET_NORMAL(n) ((n)*COLPOLY_NORMAL_FRAC)
#define COLPOLY_VIA_FLAG_TEST(vIA, flags) ((vIA) & (((flags)&7) << 13))
#define COLPOLY_VTX_INDEX(vI) ((vI)&0x1FFF)

#define DYNAPOLY_INVALIDATE_LOOKUP (1 << 0)

#define BGACTOR_NEG_ONE -1
#define BG_ACTOR_MAX 50
#define BGCHECK_SCENE BG_ACTOR_MAX
#define BGCHECK_Y_MIN -32000.0f
#define BGCHECK_XYZ_ABSMAX 32760.0f
#define BGCHECK_SUBDIV_OVERLAP 50
#define BGCHECK_SUBDIV_MIN 150.0f

#define FUNC_80041EA4_RESPAWN 5
#define FUNC_80041EA4_MOUNT_WALL 6
#define FUNC_80041EA4_STOP 8
#define FUNC_80041EA4_VOID_OUT 12

#define WATERBOX_ROOM(p) ((p >> 13) & 0x3F)

typedef struct {
    Vec3f scale;
    Vec3s rot;
    Vec3f pos;
} ScaleRotPos;

typedef struct {
    /* 0x00 */ u16 type;
    union {
        u16 vtxData[3];
        struct {
            /* 0x02 */ u16 flags_vIA; // 0xE000 is poly exclusion flags (xpFlags), 0x1FFF is vtxId
            /* 0x04 */ u16 flags_vIB; // 0xE000 is flags, 0x1FFF is vtxId
                                      // 0x2000 = poly IsConveyor surface
            /* 0x06 */ u16 vIC;
        };
    };
    /* 0x08 */ Vec3s normal; // Unit normal vector
                             // Value ranges from -0x7FFF to 0x7FFF, representing -1.0 to 1.0; 0x8000 is invalid

    /* 0x0E */ s16 dist; // Plane distance from origin along the normal
} CollisionPoly; // size = 0x10

typedef struct {
    /* 0x00 */ u16 cameraSType;
    /* 0x02 */ s16 numCameras;
    /* 0x04 */ Vec3s* camPosData;
} CamData;

typedef struct {
    /* 0x00 */ s16 xMin;
    /* 0x02 */ s16 ySurface;
    /* 0x04 */ s16 zMin;
    /* 0x06 */ s16 xLength;
    /* 0x08 */ s16 zLength;
    /* 0x0C */ u32 properties;

    // 0x0008_0000 = ?
    // 0x0007_E000 = Room Index, 0x3F = all rooms
    // 0x0000_1F00 = Lighting Settings Index
    // 0x0000_00FF = CamData index
} WaterBox; // size = 0x10

typedef struct {
    u32 data[2];

    // Type 1
    // 0x0800_0000 = wall damage
} SurfaceType;

typedef struct {
    /* 0x00 */ Vec3s minBounds; // minimum coordinates of poly bounding box
    /* 0x06 */ Vec3s maxBounds; // maximum coordinates of poly bounding box
    /* 0x0C */ u16 numVertices;
    /* 0x10 */ Vec3s* vtxList;
    /* 0x14 */ u16 numPolygons;
    /* 0x18 */ CollisionPoly* polyList;
    /* 0x1C */ SurfaceType* surfaceTypeList;
    /* 0x20 */ CamData* cameraDataList;
    /* 0x24 */ u16 numWaterBoxes;
    /* 0x28 */ WaterBox* waterBoxes;
    size_t cameraDataListLen; // OTRTODO: Added to allow for bounds checking the cameraDataList.
} CollisionHeader; // original name: BGDataInfo

typedef struct {
    s16 polyId;
    u16 next; // next SSNode index
} SSNode;

typedef struct {
    u16 head; // first SSNode index
} SSList;

typedef struct {
    /* 0x00 */ u16 max;          // original name: short_slist_node_size
    /* 0x02 */ u16 count;        // original name: short_slist_node_last_index
    /* 0x04 */ SSNode* tbl;      // original name: short_slist_node_tbl
    /* 0x08 */ u8* polyCheckTbl; // points to an array of bytes, one per static poly. Zero initialized when starting a
                                 // bg check, and set to 1 if that poly has already been tested.
} SSNodeList;

typedef struct {
    SSNode* tbl;
    s32 count;
    s32 max;
} DynaSSNodeList;

typedef struct {
    SSList floor;
    SSList wall;
    SSList ceiling;
} StaticLookup;

typedef struct {
    u16 polyStartIndex;
    SSList ceiling;
    SSList wall;
    SSList floor;
} DynaLookup;

typedef struct {
    /* 0x00 */ struct Actor* actor;
    /* 0x04 */ CollisionHeader* colHeader;
    /* 0x08 */ DynaLookup dynaLookup;
    /* 0x10 */ u16 vtxStartIndex;
    /* 0x14 */ ScaleRotPos prevTransform;
    /* 0x34 */ ScaleRotPos curTransform;
    /* 0x54 */ Sphere16 boundingSphere;
    /* 0x5C */ f32 minY;
    /* 0x60 */ f32 maxY;
} BgActor; // size = 0x64

typedef struct {
    /* 0x0000 */ u8 bitFlag;
    /* 0x0004 */ BgActor bgActors[BG_ACTOR_MAX];
    /* 0x138C */ u16 bgActorFlags[BG_ACTOR_MAX]; // & 0x0008 = no dyna ceiling
    /* 0x13F0 */ CollisionPoly* polyList;
    /* 0x13F4 */ Vec3s* vtxList;
    /* 0x13F8 */ DynaSSNodeList polyNodes;
    /* 0x1404 */ s32 polyNodesMax;
    /* 0x1408 */ s32 polyListMax;
    /* 0x140C */ s32 vtxListMax;
} DynaCollisionContext; // size = 0x1410

typedef struct CollisionContext {
    /* 0x00 */ CollisionHeader* colHeader; // scene's static collision
    /* 0x04 */ Vec3f minBounds;            // minimum coordinates of collision bounding box
    /* 0x10 */ Vec3f maxBounds;            // maximum coordinates of collision bounding box
    /* 0x1C */ Vec3i subdivAmount;         // x, y, z subdivisions of the scene's static collision
    /* 0x28 */ Vec3f subdivLength;         // x, y, z subdivision worldspace lengths
    /* 0x34 */ Vec3f subdivLengthInv;      // inverse of subdivision length
    /* 0x40 */ StaticLookup* lookupTbl;    // 3d array of length subdivAmount
    /* 0x44 */ SSNodeList polyNodes;
    /* 0x50 */ DynaCollisionContext dyna;
    /* 0x1460 */ u32 memSize; // Size of all allocated memory plus CollisionContext
} CollisionContext; // size = 0x1464

typedef struct {
    /* 0x00 */ struct PlayState* play;
    /* 0x04 */ struct CollisionContext* colCtx;
    /* 0x08 */ u16 xpFlags;
    /* 0x0C */ CollisionPoly** resultPoly;
    /* 0x10 */ f32 yIntersect;
    /* 0x14 */ Vec3f* pos;
    /* 0x18 */ s32* bgId;
    /* 0x1C */ struct Actor* actor;
    /* 0x20 */ u32 unk_20;
    /* 0x24 */ f32 chkDist;
    /* 0x28 */ DynaCollisionContext* dyna;
    /* 0x2C */ SSList* ssList;
} DynaRaycast;

typedef struct {
    /* 0x00 */ struct CollisionContext* colCtx;
    /* 0x04 */ u16 xpFlags;
    /* 0x08 */ DynaCollisionContext* dyna;
    /* 0x0C */ SSList* ssList;
    /* 0x10 */ Vec3f* posA;
    /* 0x14 */ Vec3f* posB;
    /* 0x18 */ Vec3f* posResult;
    /* 0x1C */ CollisionPoly** resultPoly;
    /* 0x20 */ s32 chkOneFace; // bccFlags & 0x8
    /* 0x24 */ f32* distSq;    // distance from posA to poly squared
    /* 0x28 */ f32 chkDist;    // distance from poly
} DynaLineTest;

#endif

/**
 * File: z_en_horse.c
 * Overlay: ovl_En_Horse
 * Description: Rideable horses
 */

#include "z_en_horse.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
#include "objects/object_horse/object_horse.h"
#include "objects/object_hni/object_hni.h"
#include "scenes/overworld/spot09/spot09_scene.h"

#define FLAGS ACTOR_FLAG_4

typedef void (*EnHorseCsFunc)(EnHorse*, PlayState*, CsCmdActorAction*);
typedef void (*EnHorseActionFunc)(EnHorse*, PlayState*);

void EnHorse_Init(Actor* thisx, PlayState* play);
void EnHorse_Destroy(Actor* thisx, PlayState* play);
void EnHorse_Update(Actor* thisx, PlayState* play);
void EnHorse_Draw(Actor* thisx, PlayState* play);

void EnHorse_InitCutscene(EnHorse* this, PlayState* play);
void EnHorse_InitHorsebackArchery(EnHorse* this);
void EnHorse_InitFleePlayer(EnHorse* this);
void EnHorse_ResetIdleAnimation(EnHorse* this);
void EnHorse_StartIdleRidable(EnHorse* this);
void EnHorse_InitInactive(EnHorse* this);
void EnHorse_InitIngoHorse(EnHorse* this);

void EnHorse_Frozen(EnHorse* this, PlayState* play);
void EnHorse_Inactive(EnHorse* this, PlayState* play);
void EnHorse_Idle(EnHorse* this, PlayState* play);
void EnHorse_FollowPlayer(EnHorse* this, PlayState* play);
void EnHorse_UpdateIngoRace(EnHorse* this, PlayState* play);
void EnHorse_MountedIdle(EnHorse* this, PlayState* play);
void EnHorse_MountedIdleWhinneying(EnHorse* this, PlayState* play);
void EnHorse_MountedTurn(EnHorse* this, PlayState* play);
void EnHorse_MountedWalk(EnHorse* this, PlayState* play);
void EnHorse_MountedTrot(EnHorse* this, PlayState* play);
void EnHorse_MountedGallop(EnHorse* this, PlayState* play);
void EnHorse_MountedRearing(EnHorse* this, PlayState* play);
void EnHorse_Stopping(EnHorse* this, PlayState* play);
void EnHorse_Reverse(EnHorse* this, PlayState* play);
void EnHorse_LowJump(EnHorse* this, PlayState* play);
void EnHorse_HighJump(EnHorse* this, PlayState* play);
void EnHorse_BridgeJump(EnHorse* this, PlayState* play);
void EnHorse_CutsceneUpdate(EnHorse* this, PlayState* play);
void EnHorse_UpdateHorsebackArchery(EnHorse* this, PlayState* play);
void EnHorse_FleePlayer(EnHorse* this, PlayState* play);

static AnimationHeader* sEponaAnimHeaders[] = {
    &gEponaIdleAnim,     &gEponaWhinnyAnim,    &gEponaRefuseAnim,  &gEponaRearingAnim,     &gEponaWalkingAnim,
    &gEponaTrottingAnim, &gEponaGallopingAnim, &gEponaJumpingAnim, &gEponaJumpingHighAnim,
};

static AnimationHeader* sHniAnimHeaders[] = {
    &gHorseIngoIdleAnim,      &gHorseIngoWhinnyAnim,  &gHorseIngoRefuseAnim,
    &gHorseIngoRearingAnim,   &gHorseIngoWalkingAnim, &gHorseIngoTrottingAnim,
    &gHorseIngoGallopingAnim, &gHorseIngoJumpingAnim, &gHorseIngoJumpingHighAnim,
};

static AnimationHeader** sAnimationHeaders[] = { sEponaAnimHeaders, sHniAnimHeaders };

static f32 sPlaybackSpeeds[] = { 2.0f / 3.0f, 2.0f / 3.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f / 3.0f, 2.0f / 3.0f };

static SkeletonHeader* sSkeletonHeaders[] = { &gEponaSkel, &gHorseIngoSkel };

const ActorInit En_Horse_InitVars = {
    ACTOR_EN_HORSE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE,
    sizeof(EnHorse),
    (ActorFunc)EnHorse_Init,
    (ActorFunc)EnHorse_Destroy,
    (ActorFunc)EnHorse_Update,
    (ActorFunc)EnHorse_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COLTYPE_NONE,
        AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000400, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphItemsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphItemsInit,
};

static CollisionCheckInfoInit D_80A65F38 = { 10, 35, 100, MASS_HEAVY };

typedef struct {
    s16 scene;
    Vec3s pos;
    s16 angle;
} EnHorseSpawnpoint;

static EnHorseSpawnpoint sHorseSpawns[] = {
    // Hyrule Field
    { SCENE_SPOT00, 16, 0, 1341, 0 },
    { SCENE_SPOT00, -1297, 0, 1459, 0 },
    { SCENE_SPOT00, -5416, -300, 1296, 0 },
    { SCENE_SPOT00, -4667, -300, 3620, 0 },
    { SCENE_SPOT00, -3837, 81, 5537, 0 },
    { SCENE_SPOT00, -5093, -226, 6661, 0 },
    { SCENE_SPOT00, -6588, -79, 5053, 0 },
    { SCENE_SPOT00, -7072, -500, 7538, 0 },
    { SCENE_SPOT00, -6139, -500, 8910, 0 },
    { SCENE_SPOT00, -8497, -300, 7802, 0 },
    { SCENE_SPOT00, -5481, -499, 12127, 0 },
    { SCENE_SPOT00, -4808, -700, 13583, 0 },
    { SCENE_SPOT00, -3416, -490, 12092, 0 },
    { SCENE_SPOT00, -2915, 100, 8339, 0 },
    { SCENE_SPOT00, -2277, -500, 13247, 0 },
    { SCENE_SPOT00, -1026, -500, 12101, 0 },
    { SCENE_SPOT00, 1427, -500, 13341, 0 },
    { SCENE_SPOT00, -200, -486, 10205, 0 },
    { SCENE_SPOT00, -1469, 100, 7496, 0 },
    { SCENE_SPOT00, -995, 168, 5652, 0 },
    { SCENE_SPOT00, 1938, 89, 6232, 0 },
    { SCENE_SPOT00, 1387, -105, 9206, 0 },
    { SCENE_SPOT00, 1571, -223, 7701, 0 },
    { SCENE_SPOT00, 3893, -121, 7068, 0 },
    { SCENE_SPOT00, 3179, 373, 5221, 0 },
    { SCENE_SPOT00, 4678, -20, 3869, 0 },
    { SCENE_SPOT00, 3460, 246, 4207, 0 },
    { SCENE_SPOT00, 3686, 128, 2366, 0 },
    { SCENE_SPOT00, 1791, 18, 152, 0 },
    { SCENE_SPOT00, 3667, -16, 1399, 0 },
    { SCENE_SPOT00, 1827, -15, 983, 0 },
    { SCENE_SPOT00, 1574, 399, 4318, 0 },
    { SCENE_SPOT00, 716, 95, 3391, 0 },
    { SCENE_SPOT00, -1189, -41, 4739, 0 },
    { SCENE_SPOT00, -1976, -171, 4172, 0 },
    { SCENE_SPOT00, 1314, 391, 5665, 0 },
    { SCENE_SPOT00, 112, 0, 1959, 0 },
    { SCENE_SPOT00, -3011, -111, 9397, 0 },
    { SCENE_SPOT00, -5674, -270, 8585, 0 },
    { SCENE_SPOT00, -8861, -300, 7836, 0 },
    { SCENE_SPOT00, -6056, -500, 7810, 0 },
    { SCENE_SPOT00, -7306, -500, 5994, 0 },
    { SCENE_SPOT00, -7305, -500, 7605, 0 },
    { SCENE_SPOT00, -7439, -300, 7600, 0 },
    { SCENE_SPOT00, -7464, -300, 6268, 0 },
    { SCENE_SPOT00, -8080, -300, 7553, 0 },
    { SCENE_SPOT00, -8091, -300, 7349, 0 },
    { SCENE_SPOT00, -8785, -300, 7383, 0 },
    { SCENE_SPOT00, -8745, -300, 7508, 0 },
    { SCENE_SPOT00, -8777, -300, 7788, 0 },
    { SCENE_SPOT00, -8048, -299, 7738, 0 },
    { SCENE_SPOT00, -7341, -184, 7730, 0 },
    { SCENE_SPOT00, -6410, -288, 7824, 0 },
    { SCENE_SPOT00, -6326, -290, 8205, 0 },
    { SCENE_SPOT00, -6546, -292, 8400, 0 },
    { SCENE_SPOT00, -7533, -180, 8459, 0 },
    { SCENE_SPOT00, -8024, -295, 7903, 0 },
    { SCENE_SPOT00, -8078, -308, 7994, 0 },
    { SCENE_SPOT00, -9425, -287, 7696, 0 },
    { SCENE_SPOT00, -9322, -292, 7577, 0 },
    { SCENE_SPOT00, -9602, -199, 7160, 0 },
    { SCENE_SPOT00, -9307, -300, 7758, 0 },
    { SCENE_SPOT00, -9230, -300, 7642, 0 },
    { SCENE_SPOT00, -7556, -499, 8695, 0 },
    { SCENE_SPOT00, -6438, -500, 8606, 0 },
    { SCENE_SPOT00, -6078, -500, 8258, 0 },
    { SCENE_SPOT00, -6233, -500, 7613, 0 },
    { SCENE_SPOT00, -5035, -205, 7814, 0 },
    { SCENE_SPOT00, -5971, -500, 8501, 0 },
    { SCENE_SPOT00, -5724, -498, 10123, 0 },
    { SCENE_SPOT00, -5094, -392, 11106, 0 },
    { SCENE_SPOT00, -5105, -393, 11312, 0 },
    { SCENE_SPOT00, -4477, -314, 11132, 0 },
    { SCENE_SPOT00, -3867, -380, 11419, 0 },
    { SCENE_SPOT00, -2952, -500, 11944, 0 },
    { SCENE_SPOT00, -2871, -488, 11743, 0 },
    { SCENE_SPOT00, -3829, -372, 11327, 0 },
    { SCENE_SPOT00, -4439, -293, 10989, 0 },
    { SCENE_SPOT00, -5014, -381, 11086, 0 },
    { SCENE_SPOT00, -5113, -188, 10968, 0 },
    { SCENE_SPOT00, -5269, -188, 11156, 0 },
    { SCENE_SPOT00, -5596, -178, 9972, 0 },
    { SCENE_SPOT00, -5801, -288, 8518, 0 },
    { SCENE_SPOT00, -4910, -178, 7931, 0 },
    { SCENE_SPOT00, -3586, 73, 8140, 0 },
    { SCENE_SPOT00, -4487, -106, 9362, 0 },
    { SCENE_SPOT00, -4339, -112, 6412, 0 },
    { SCENE_SPOT00, -3417, 105, 8194, 0 },
    { SCENE_SPOT00, -4505, -20, 6608, 0 },
    { SCENE_SPOT00, -5038, -199, 6603, 0 },
    { SCENE_SPOT00, -4481, 1, 6448, 0 },
    { SCENE_SPOT00, -5032, -168, 6418, 0 },
    { SCENE_SPOT00, -5256, -700, 14329, 0 },
    { SCENE_SPOT00, -5749, -820, 15380, 0 },
    { SCENE_SPOT00, -3122, -700, 13608, 0 },
    { SCENE_SPOT00, -3758, -525, 13228, 0 },
    { SCENE_SPOT00, -3670, -500, 13123, 0 },
    { SCENE_SPOT00, -2924, -500, 13526, 0 },
    { SCENE_SPOT00, 1389, -115, 9370, 0 },
    { SCENE_SPOT00, 548, -116, 8889, 0 },
    { SCENE_SPOT00, -106, -107, 8530, 0 },
    { SCENE_SPOT00, -1608, 85, 7646, 0 },
    { SCENE_SPOT00, -5300, -700, 13772, 0 },
    { SCENE_SPOT00, -5114, -700, 13400, 0 },
    { SCENE_SPOT00, -4561, -700, 13700, 0 },
    { SCENE_SPOT00, -4762, -700, 14084, 0 },
    { SCENE_SPOT00, -2954, 100, 8216, 0 },
    { SCENE_SPOT00, 1460, -104, 9246, 0 },
    { SCENE_SPOT00, 629, -105, 8791, 0 },
    { SCENE_SPOT00, -10, -90, 8419, 0 },
    { SCENE_SPOT00, -1462, 100, 7504, 0 },
    { SCENE_SPOT00, -3018, -500, 12493, 0 },
    { SCENE_SPOT00, -2994, -311, 10331, 0 },
    { SCENE_SPOT00, -4006, -700, 14152, 0 },
    { SCENE_SPOT00, -4341, -500, 12743, 0 },
    { SCENE_SPOT00, -5879, -497, 6799, 0 },
    { SCENE_SPOT00, 22, -473, 10103, 0 },
    { SCENE_SPOT00, -1389, -192, 8874, 0 },
    { SCENE_SPOT00, -4, 92, 6866, 0 },
    { SCENE_SPOT00, 483, 104, 6637, 0 },
    { SCENE_SPOT00, 1580, 183, 5987, 0 },
    { SCENE_SPOT00, 1548, 308, 5077, 0 },
    { SCENE_SPOT00, 1511, 399, 4267, 0 },
    { SCENE_SPOT00, 1358, 385, 4271, 0 },
    { SCENE_SPOT00, 1379, 395, 5063, 0 },
    { SCENE_SPOT00, 1360, 394, 5870, 0 },
    { SCENE_SPOT00, 813, 283, 6194, 0 },
    { SCENE_SPOT00, -57, 101, 6743, 0 },
    { SCENE_SPOT00, 91, 325, 5143, 0 },
    { SCENE_SPOT00, 1425, -214, 7659, 0 },
    { SCENE_SPOT00, 3487, -19, 880, 0 },
    { SCENE_SPOT00, 2933, 152, 2094, 0 },
    { SCENE_SPOT00, 2888, -145, 6862, 0 },
    { SCENE_SPOT00, 1511, 67, 6471, 0 },
    { SCENE_SPOT00, 4051, -47, 1722, 0 },
    { SCENE_SPOT00, -7335, -500, 8627, 0 },
    { SCENE_SPOT00, -7728, -462, 8498, 0 },
    { SCENE_SPOT00, -7791, -446, 8832, 0 },
    { SCENE_SPOT00, -2915, -435, 11334, 0 },
    { SCENE_SPOT00, 165, -278, 3352, 0 },

    // Lake Hylia
    { SCENE_SPOT06, -2109, -882, 1724, 0 },
    { SCENE_SPOT06, -328, -1238, 2705, 0 },
    { SCENE_SPOT06, -3092, -1033, 3527, 0 },

    // Gerudo Valley
    { SCENE_SPOT09, 2687, -269, 753, 0 },
    { SCENE_SPOT09, 2066, -132, 317, 0 },
    { SCENE_SPOT09, 523, -8, 635, 0 },
    { SCENE_SPOT09, 558, 36, -323, 0 },
    { SCENE_SPOT09, 615, 51, -839, 0 },
    { SCENE_SPOT09, -614, 32, 29, 0 },
    { SCENE_SPOT09, -639, -3, 553, 0 },
    { SCENE_SPOT09, -540, 10, -889, 0 },
    { SCENE_SPOT09, -1666, 58, 378, 0 },
    { SCENE_SPOT09, -3044, 210, -648, 0 },

    // Gerudo's Fortress
    { SCENE_SPOT12, -678, 21, -623, 0 },
    { SCENE_SPOT12, 149, 333, -2499, 0 },
    { SCENE_SPOT12, 499, 581, -547, 0 },
    { SCENE_SPOT12, 3187, 1413, -3775, 0 },
    { SCENE_SPOT12, 3198, 1413, 307, 0 },
    { SCENE_SPOT12, 3380, 1413, -1200, 0 },
    { SCENE_SPOT12, -966, 1, -56, 0 },
    { SCENE_SPOT12, -966, 24, -761, 0 },
    { SCENE_SPOT12, -694, 174, -2820, 0 },

    // Lon Lon Ranch
    { SCENE_SPOT20, 1039, 0, 2051, 0 },
    { SCENE_SPOT20, -1443, 0, 1429, 0 },
    { SCENE_SPOT20, 856, 0, -918, 0 }, // Hardcoded to always load in lon lon
    { SCENE_SPOT20, 882, 0, -2256, 0 },
    { SCENE_SPOT20, -1003, 0, -755, 0 }, // Hardcoded to always load in lon lon
    { SCENE_SPOT20, -2254, 0, -629, 0 },
    { SCENE_SPOT20, 907, 0, -2336, 0 },
};

typedef struct {
    s16 zMin;
    s16 zMax;

    s16 xMin;
    s16 xMax;
    s16 xOffset;

    s16 angle;
    s16 angleRange;

    Vec3s pos;
} BridgeJumpPoint;

static BridgeJumpPoint sBridgeJumps[] = {
    { -195, -40, 225, 120, 360, -0x4000, 0x7D0, -270, -52, -117 },
    { -195, -40, -240, -120, -360, 0x4000, 0x7D0, 270, -52, -117 },
};

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 speed;
    s16 angle;
} RaceWaypoint;

typedef struct {
    s32 numWaypoints;
    RaceWaypoint* waypoints;
} RaceInfo;

static RaceWaypoint sIngoRaceWaypoints[] = {
    { 1056, 1, -1540, 11, 0x2A8D },  { 1593, 1, -985, 10, 0xFC27 },   { 1645, 1, -221, 11, 0xE891 },
    { 985, 1, 403, 10, 0xBB9C },     { -1023, 1, 354, 11, 0xA37D },   { -1679, 1, -213, 10, 0x889C },
    { -1552, 1, -1008, 11, 0x638D }, { -947, -1, -1604, 10, 0x4002 },
};

static RaceInfo sIngoRace = { 8, sIngoRaceWaypoints };
static s32 sAnimSoundFrames[] = { 0, 16 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 300, ICHAIN_STOP),
};

static u8 sResetNoInput[] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 };

static s32 sIdleAnimIds[] = { 1, 3, 0, 3, 1, 0 };

static s16 sIngoAnimations[] = { 7, 6, 2, 2, 1, 1, 0, 0, 0, 0 };

void EnHorse_CsMoveInit(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsJumpInit(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsRearingInit(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_WarpMoveInit(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsWarpRearingInit(EnHorse* this, PlayState* play, CsCmdActorAction* action);

static EnHorseCsFunc sCutsceneInitFuncs[] = {
    NULL,
    EnHorse_CsMoveInit,
    EnHorse_CsJumpInit,
    EnHorse_CsRearingInit,
    EnHorse_WarpMoveInit,
    EnHorse_CsWarpRearingInit,
};

void EnHorse_CsMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsJump(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsRearing(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsWarpMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorAction* action);
void EnHorse_CsWarpRearing(EnHorse* this, PlayState* play, CsCmdActorAction* action);

static EnHorseCsFunc sCutsceneActionFuncs[] = {
    NULL, EnHorse_CsMoveToPoint, EnHorse_CsJump, EnHorse_CsRearing, EnHorse_CsWarpMoveToPoint, EnHorse_CsWarpRearing,
};

typedef struct {
    s32 csAction;
    s32 csFuncIdx;
} CsActionEntry;

static CsActionEntry sCsActionTable[] = {
    { 36, 1 }, { 37, 2 }, { 38, 3 }, { 64, 4 }, { 65, 5 },
};

static RaceWaypoint sHbaWaypoints[] = {
    { 3600, 1413, -5055, 11, 0x8001 }, { 3360, 1413, -5220, 5, 0xC000 }, { 3100, 1413, -4900, 5, 0x0000 },
    { 3600, 1413, -4100, 11, 0x0000 }, { 3600, 1413, 360, 11, 0x0000 },
};

static RaceInfo sHbaInfo = { 5, sHbaWaypoints };

static EnHorseActionFunc sActionFuncs[] = {
    EnHorse_Frozen,
    EnHorse_Inactive,
    EnHorse_Idle,
    EnHorse_FollowPlayer,
    EnHorse_UpdateIngoRace,
    EnHorse_MountedIdle,
    EnHorse_MountedIdleWhinneying,
    EnHorse_MountedTurn,
    EnHorse_MountedWalk,
    EnHorse_MountedTrot,
    EnHorse_MountedGallop,
    EnHorse_MountedRearing,
    EnHorse_Stopping,
    EnHorse_Reverse,
    EnHorse_LowJump,
    EnHorse_HighJump,
    EnHorse_BridgeJump,
    EnHorse_CutsceneUpdate,
    EnHorse_UpdateHorsebackArchery,
    EnHorse_FleePlayer,
};

s32 EnHorse_BgCheckBridgeJumpPoint(EnHorse* this, PlayState* play) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (play->sceneNum != SCENE_SPOT09) {
        return false;
    }
    if (this->actor.speedXZ < 12.8f) {
        return false;
    }
    if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
        return false;
    }

    for (i = 0; i < 2; i++) {
        xMin = sBridgeJumps[i].xMin;
        xMax = (xMin + sBridgeJumps[i].xMax) + sBridgeJumps[i].xOffset;
        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }
        if (sBridgeJumps[i].zMin < this->actor.world.pos.z && this->actor.world.pos.z < sBridgeJumps[i].zMax) {
            if (xMin < this->actor.world.pos.x && this->actor.world.pos.x < xMax) {
                if (sBridgeJumps[i].angle - sBridgeJumps[i].angleRange < this->actor.world.rot.y &&
                    this->actor.world.rot.y < sBridgeJumps[i].angle + sBridgeJumps[i].angleRange) {
                    return true;
                }
            }
        }
    }
    return false;
}

void EnHorse_StartBridgeJump(EnHorse* this, PlayState* play);

s32 EnHorse_CheckBridgeJumps(EnHorse* this, PlayState* play) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (this->actor.speedXZ < 12.8f) {
        return false;
    }

    for (i = 0; i != 2; i++) {
        xMin = sBridgeJumps[i].xMin;
        xMax = sBridgeJumps[i].xMax + xMin;

        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }

        if (sBridgeJumps[i].zMin < this->actor.world.pos.z && this->actor.world.pos.z < sBridgeJumps[i].zMax) {
            if (xMin < this->actor.world.pos.x && this->actor.world.pos.x < xMax) {
                if (sBridgeJumps[i].angle - sBridgeJumps[i].angleRange < this->actor.world.rot.y &&
                    this->actor.world.rot.y < sBridgeJumps[i].angle + sBridgeJumps[i].angleRange) {
                    this->bridgeJumpIdx = i;
                    EnHorse_StartBridgeJump(this, play);
                    return true;
                }
            }
        }
    }

    return false;
}

void EnHorse_RaceWaypointPos(RaceWaypoint* waypoints, s32 idx, Vec3f* pos) {
    pos->x = waypoints[idx].x;
    pos->y = waypoints[idx].y;
    pos->z = waypoints[idx].z;
}

void EnHorse_RotateToPoint(EnHorse* this, PlayState* play, Vec3f* pos, s16 turnAmount) {
    func_8006DD9C(&this->actor, pos, turnAmount);
}

void EnHorse_UpdateIngoRaceInfo(EnHorse* this, PlayState* play, RaceInfo* raceInfo) {
    Vec3f curWaypointPos;
    Vec3f prevWaypointPos;
    f32 playerDist;
    f32 sp50;
    s16 relPlayerYaw;
    f32 px;
    f32 pz;
    f32 d;
    f32 dist;
    s32 prevWaypoint;

    EnHorse_RaceWaypointPos(raceInfo->waypoints, this->curRaceWaypoint, &curWaypointPos);
    Math3D_RotateXZPlane(&curWaypointPos, raceInfo->waypoints[this->curRaceWaypoint].angle, &px, &pz, &d);
    if (((this->actor.world.pos.x * px) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= raceInfo->numWaypoints) {
            this->curRaceWaypoint = 0;
        }
    }

    EnHorse_RaceWaypointPos(raceInfo->waypoints, this->curRaceWaypoint, &curWaypointPos);

    prevWaypoint = this->curRaceWaypoint - 1;
    if (prevWaypoint < 0) {
        prevWaypoint = raceInfo->numWaypoints - 1;
    }
    EnHorse_RaceWaypointPos(raceInfo->waypoints, prevWaypoint, &prevWaypointPos);
    Math3D_PointDistToLine2D(this->actor.world.pos.x, this->actor.world.pos.z, prevWaypointPos.x, prevWaypointPos.z,
                             curWaypointPos.x, curWaypointPos.z, &dist);
    EnHorse_RotateToPoint(this, play, &curWaypointPos, 400);

    if (dist < 90000.0f) {
        playerDist = this->actor.xzDistToPlayer;
        if (playerDist < 130.0f || this->jntSph.elements[0].info.ocElemFlags & 2) {
            if (Math_SinS(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                this->actor.world.rot.y = this->actor.world.rot.y - 280;
            } else {
                this->actor.world.rot.y = this->actor.world.rot.y + 280;
            }
        } else if (playerDist < 300.0f) {
            if (Math_SinS(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                this->actor.world.rot.y = this->actor.world.rot.y + 280;
            } else {
                this->actor.world.rot.y = this->actor.world.rot.y - 280;
            }
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    sp50 = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
    relPlayerYaw = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
    if (sp50 <= 200.0f || (fabsf(Math_SinS(relPlayerYaw)) < 0.8f && Math_CosS(relPlayerYaw) > 0.0f)) {
        if (this->actor.speedXZ < this->ingoHorseMaxSpeed) {
            this->actor.speedXZ += 0.47f;
        } else {
            this->actor.speedXZ -= 0.47f;
        }
        this->ingoRaceFlags |= 1;
        return;
    }

    if (this->actor.speedXZ < raceInfo->waypoints[this->curRaceWaypoint].speed) {
        this->actor.speedXZ = this->actor.speedXZ + 0.4f;
    } else {
        this->actor.speedXZ = this->actor.speedXZ - 0.4f;
    }
    this->ingoRaceFlags &= ~0x1;
}

void EnHorse_PlayWalkingSound(EnHorse* this) {
    if (sAnimSoundFrames[this->soundTimer] < this->curFrame) {
        if (this->soundTimer == 0 && (sAnimSoundFrames[1] < this->curFrame)) {
            return;
        }

        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_WALK, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        if (++this->soundTimer > 1) {
            this->soundTimer = 0;
        }
    }
}

void EnHorse_PlayTrottingSound(EnHorse* this) {
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

void EnHorse_PlayGallopingSound(EnHorse* this) {
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
}

f32 EnHorse_SlopeSpeedMultiplier(EnHorse* this, PlayState* play) {
    f32 multiplier = 1.0f;

    if (Math_CosS(this->actor.shape.rot.x) < 0.939262f && Math_SinS(this->actor.shape.rot.x) < 0.0f) {
        multiplier = 0.7f;
    }
    return multiplier;
}

void func_80A5BB90(PlayState* play, Vec3f* vec, Vec3f* arg2, f32* arg3) {
    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, vec, arg2, arg3);
}

s32 func_80A5BBBC(PlayState* play, EnHorse* this, Vec3f* pos) {
    Vec3f sp24;
    f32 sp20;
    f32 eyeDist;

    func_80A5BB90(play, pos, &sp24, &sp20);
    if (fabsf(sp20) < 0.008f) {
        return false;
    }
    eyeDist = Math3D_Vec3f_DistXYZ(pos, &play->view.eye);
    return func_800314D4(play, &this->actor, &sp24, sp20) || eyeDist < 100.0f;
}

void EnHorse_IdleAnimSounds(EnHorse* this, PlayState* play) {
    if (this->animationIdx == ENHORSE_ANIM_IDLE &&
        ((this->curFrame > 35.0f && this->type == HORSE_EPONA) ||
         (this->curFrame > 28.0f && this->type == HORSE_HNI)) &&
        !(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
        this->stateFlags |= ENHORSE_SANDDUST_SOUND;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    } else if (this->animationIdx == ENHORSE_ANIM_REARING && this->curFrame > 25.0f &&
               !(this->stateFlags & ENHORSE_LAND2_SOUND)) {
        this->stateFlags |= ENHORSE_LAND2_SOUND;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

s32 EnHorse_Spawn(EnHorse* this, PlayState* play) {
    f32 minDist = 1e38f;
    s32 spawn = false;
    f32 dist;
    s32 i;
    Player* player;
    Vec3f spawnPos;

    for (i = 0; i < 169; i++) {
        if (sHorseSpawns[i].scene == play->sceneNum) {
            player = GET_PLAYER(play);
            if (play->sceneNum != SCENE_SPOT20 ||
                //! Same flag checked twice
                (Flags_GetEventChkInf(0x18) && ((gSaveContext.eventInf[0] & 0xF) != 6 || Flags_GetEventChkInf(0x18))) ||
                // always load two spawns inside lon lon
                ((sHorseSpawns[i].pos.x == 856 && sHorseSpawns[i].pos.y == 0 && sHorseSpawns[i].pos.z == -918) ||
                 (sHorseSpawns[i].pos.x == -1003 && sHorseSpawns[i].pos.y == 0 && sHorseSpawns[i].pos.z == -755))) {

                spawnPos.x = sHorseSpawns[i].pos.x;
                spawnPos.y = sHorseSpawns[i].pos.y;
                spawnPos.z = sHorseSpawns[i].pos.z;
                dist = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &spawnPos);

                if (play->sceneNum) {}
                if (!(minDist < dist) && !func_80A5BBBC(play, this, &spawnPos)) {
                    minDist = dist;
                    this->actor.world.pos.x = sHorseSpawns[i].pos.x;
                    this->actor.world.pos.y = sHorseSpawns[i].pos.y;
                    this->actor.world.pos.z = sHorseSpawns[i].pos.z;
                    this->actor.prevPos = this->actor.world.pos;
                    this->actor.world.rot.y = sHorseSpawns[i].angle;
                    this->actor.shape.rot.y = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor);
                    spawn = true;
                    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &this->actor.world.pos,
                                                 &this->actor.projectedPos, &this->actor.projectedW);
                }
            }
        }
    }

    return spawn;
}

void EnHorse_ResetCutscene(EnHorse* this, PlayState* play) {
    this->cutsceneAction = -1;
    this->cutsceneFlags = 0;
}

void EnHorse_ResetRace(EnHorse* this, PlayState* play) {
    this->inRace = false;
}

s32 EnHorse_PlayerCanMove(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & 1) || func_8002DD78(GET_PLAYER(play)) == 1 || (player->stateFlags1 & 0x100000) ||
        ((this->stateFlags & ENHORSE_FLAG_19) && !this->inRace) || this->action == ENHORSE_ACT_HBA ||
        player->actor.flags & ACTOR_FLAG_8 || play->csCtx.state != 0) {
        return false;
    }
    return true;
}

void EnHorse_ResetHorsebackArchery(EnHorse* this, PlayState* play) {
    this->unk_39C = 0;
    this->hbaStarted = 0;
    this->hbaFlags = 0;
}

void EnHorse_ClearDustFlags(u16* dustFlags) {
    *dustFlags = 0;
}

void EnHorse_Init(Actor* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;

    AREG(6) = 0;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    EnHorse_ClearDustFlags(&this->dustFlags);
    DREG(53) = 0;
    this->riderPos = this->actor.world.pos;
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    this->riderPos.y = this->riderPos.y + 70.0f;

    if (DREG(4) == 0) {
        DREG(4) = 70;
    }

    if (this->actor.params & 0x8000) {
        this->actor.params &= ~0x8000;
        this->type = HORSE_HNI;

        if ((this->bankIndex = Object_GetIndex(&play->objectCtx, OBJECT_HNI)) < 0) {
            Actor_Kill(&this->actor);
            return;
        }

        do {
        } while (!Object_IsLoaded(&play->objectCtx, this->bankIndex));

        this->actor.objBankIndex = this->bankIndex;
        Actor_SetObjectDependency(play, &this->actor);
        this->boostSpeed = 12;
    } else {
        this->type = HORSE_EPONA;
        this->boostSpeed = 14;
    }

    // params was -1
    if (this->actor.params == 0x7FFF) {
        this->actor.params = 1;
    }

    if (play->sceneNum == SCENE_SOUKO) {
        this->stateFlags = ENHORSE_UNRIDEABLE;
    } else if (play->sceneNum == SCENE_SPOT12 && this->type == HORSE_HNI) {
        this->stateFlags = ENHORSE_FLAG_18 | ENHORSE_UNRIDEABLE;
    } else {
        if (this->actor.params == 3) {
            this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP | ENHORSE_UNRIDEABLE;
        } else if (this->actor.params == 6) {
            this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP;
            if (Flags_GetEventChkInf(0x18) || DREG(1) != 0) {
                this->stateFlags &= ~ENHORSE_CANT_JUMP;
                this->stateFlags |= ENHORSE_FLAG_26;
            } else if (gSaveContext.eventInf[0] & 0x40 && this->type == HORSE_HNI) {
                this->stateFlags |= ENHORSE_FLAG_21 | ENHORSE_FLAG_20;
            }
        } else if (this->actor.params == 1) {
            this->stateFlags = ENHORSE_FLAG_7;
        } else {
            this->stateFlags = 0;
        }
    }

    if (play->sceneNum == SCENE_SPOT20 && (gSaveContext.eventInf[0] & 0xF) == 6 &&
        Flags_GetEventChkInf(0x18) == 0 && !DREG(1)) {
        this->stateFlags |= ENHORSE_FLAG_25;
    }

    Actor_SetScale(&this->actor, 0.01f);
    this->actor.gravity = -3.5f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    this->action = ENHORSE_ACT_IDLE;
    this->actor.speedXZ = 0.0f;
    Collider_InitCylinder(play, &this->cyl1);
    Collider_SetCylinder(play, &this->cyl1, &this->actor, &sCylinderInit1);
    Collider_InitCylinder(play, &this->cyl2);
    Collider_SetCylinder(play, &this->cyl2, &this->actor, &sCylinderInit2);
    Collider_InitJntSph(play, &this->jntSph);
    Collider_SetJntSph(play, &this->jntSph, &this->actor, &sJntSphInit, &this->jntSphList);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, DamageTable_Get(0xB), &D_80A65F38);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->playerControlled = false;

    if ((play->sceneNum == SCENE_SPOT20) && (gSaveContext.sceneSetupIndex < 4)) {
        if (this->type == HORSE_HNI) {
            if (this->actor.world.rot.z == 0 || !IS_DAY) {
                Actor_Kill(&this->actor);
                return;
            }
            if (Flags_GetEventChkInf(0x18)) {
                Actor_Kill(&this->actor);
                return;
            }
            if (this->actor.world.rot.z != 5) {
                Actor_Kill(&this->actor);
                return;
            }
        } else if (!Flags_GetEventChkInf(0x18) && !DREG(1) && !IS_DAY) {
            Actor_Kill(&this->actor);
            return;
        }
    } else if (play->sceneNum == SCENE_MALON_STABLE) {
        if (IS_DAY || Flags_GetEventChkInf(0x18) || DREG(1) != 0 || !LINK_IS_ADULT) {
            Actor_Kill(&this->actor);
            return;
        }
        this->stateFlags |= ENHORSE_UNRIDEABLE;
    }

    Skin_Init(play, &this->skin, sSkeletonHeaders[this->type], sAnimationHeaders[this->type][ENHORSE_ANIM_IDLE]);
    this->animationIdx = ENHORSE_ANIM_IDLE;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
    this->numBoosts = 6;
    this->blinkTimer = this->postDrawFunc = this->boostRegenTime = 0;
    EnHorse_ResetCutscene(this, play);
    EnHorse_ResetRace(this, play);
    EnHorse_ResetHorsebackArchery(this, play);

    if (this->actor.params == 2) {
        EnHorse_InitInactive(this);
    } else if (this->actor.params == 3) {
        EnHorse_InitIngoHorse(this);
        this->rider =
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_IN, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, 1, 1, true);
        if (this->rider == NULL) {
            //__assert("this->race.rider != NULL");
            ASSERT(this->rider == NULL);
        }
        if (!(gSaveContext.eventInf[0] & 0x40)) {
            this->ingoHorseMaxSpeed = 12.07f;
        } else {
            this->ingoHorseMaxSpeed = 12.625f;
        }
    } else if (this->actor.params == 7) {
        EnHorse_InitCutscene(this, play);
    } else if (this->actor.params == 8) {
        EnHorse_InitHorsebackArchery(this);
        Interface_InitHorsebackArchery(play);
    } else if (play->sceneNum == SCENE_SPOT20 && !Flags_GetEventChkInf(0x18) && !DREG(1)) {
        EnHorse_InitFleePlayer(this);
    } else {
        if (play->sceneNum == SCENE_SOUKO) {
            EnHorse_ResetIdleAnimation(this);
        } else if (play->sceneNum == SCENE_SPOT12 && this->type == HORSE_HNI) {
            EnHorse_ResetIdleAnimation(this);
        } else {
            EnHorse_StartIdleRidable(this);
        }
    }
    this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
}

void EnHorse_Destroy(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_StopSfxByPos(&this->unk_21C);
    }
    Skin_Free(play, &this->skin);
    Collider_DestroyCylinder(play, &this->cyl1);
    Collider_DestroyCylinder(play, &this->cyl2);
    Collider_DestroyJntSph(play, &this->jntSph);
}

void EnHorse_RotateToPlayer(EnHorse* this, PlayState* play) {
    EnHorse_RotateToPoint(this, play, &GET_PLAYER(play)->actor.world.pos, 400);
    if (this->stateFlags & ENHORSE_OBSTACLE) {
        this->actor.world.rot.y += 800.0f;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnHorse_Freeze(EnHorse* this) {
    if (this->action != ENHORSE_ACT_CS_UPDATE && this->action != ENHORSE_ACT_HBA) {
        if (sResetNoInput[this->actor.params] != 0 && this->actor.params != 4) {
            this->noInputTimer = 0;
            this->noInputTimerMax = 0;
        }
        this->prevAction = this->action;
        this->action = ENHORSE_ACT_FROZEN;
        this->cyl1.base.ocFlags1 &= ~OC1_ON;
        this->cyl2.base.ocFlags1 &= ~OC1_ON;
        this->jntSph.base.ocFlags1 &= ~OC1_ON;
        this->animationIdx = ENHORSE_ANIM_IDLE;
    }
}

void EnHorse_ChangeIdleAnimation(EnHorse* this, s32 arg1, f32 arg2);
void EnHorse_StartMountedIdleResetAnim(EnHorse* this);
void EnHorse_StartMountedIdle(EnHorse* this);
void EnHorse_StartGalloping(EnHorse* this);

void EnHorse_Frozen(EnHorse* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;
    this->noInputTimer--;
    if (this->noInputTimer < 0) {
        this->cyl1.base.ocFlags1 |= OC1_ON;
        this->cyl2.base.ocFlags1 |= OC1_ON;
        this->jntSph.base.ocFlags1 |= OC1_ON;
        if (this->playerControlled == true) {
            this->stateFlags &= ~ENHORSE_FLAG_7;
            if (this->actor.params == 4) {
                EnHorse_StartMountedIdleResetAnim(this);
            } else if (this->actor.params == 9) {
                this->actor.params = 5;
                if (play->csCtx.state != 0) {
                    EnHorse_StartMountedIdle(this);
                } else {
                    this->actor.speedXZ = 8.0f;
                    EnHorse_StartGalloping(this);
                }
            } else if (this->prevAction == 2) {
                EnHorse_StartMountedIdle(this);
            } else {
                EnHorse_StartMountedIdleResetAnim(this);
            }
            if (this->actor.params != 0) {
                this->actor.params = 0;
                return;
            }
        } else {
            if (this->prevAction == 5) {
                EnHorse_ChangeIdleAnimation(this, 0, 0);
                return;
            }
            if (this->prevAction == 6) {
                EnHorse_ChangeIdleAnimation(this, 0, 0);
                return;
            }
            EnHorse_ChangeIdleAnimation(this, 0, 0);
        }
    }
}

void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle);

void EnHorse_UpdateSpeed(EnHorse* this, PlayState* play, f32 brakeDecel, f32 brakeAngle, f32 minStickMag,
                         f32 decel, f32 baseSpeed, s16 turnSpeed) {
    s16* stickAnglePtr; // probably fake
    f32 stickMag;
    s16 stickAngle;
    f32 temp_f12;
    f32 traction;
    s16 turn;

    if (!EnHorse_PlayerCanMove(this, play)) {
        if (this->actor.speedXZ > 8) {
            this->actor.speedXZ -= decel;
        } else if (this->actor.speedXZ < 0) {
            this->actor.speedXZ = 0;
        }

        return;
    }

    stickAnglePtr = &stickAngle;

    baseSpeed *= EnHorse_SlopeSpeedMultiplier(this, play);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (Math_CosS(stickAngle) <= brakeAngle) {
        this->actor.speedXZ -= brakeDecel;
        this->actor.speedXZ = this->actor.speedXZ < 0.0f ? 0.0f : this->actor.speedXZ;
        return;
    }

    if (stickMag < minStickMag) {
        this->stateFlags &= ~ENHORSE_BOOST;
        this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        this->actor.speedXZ -= decel;
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ = 0.0f;
        }

        return;
    }

    if (this->stateFlags & ENHORSE_BOOST) {
        if ((16 - this->boostTimer) > 0) {
            this->actor.speedXZ =
                (EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed - this->actor.speedXZ) /
                    (16 - this->boostTimer) +
                this->actor.speedXZ;
        } else {
            this->actor.speedXZ = EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed;
        }

        if ((EnHorse_SlopeSpeedMultiplier(this, play) * this->boostSpeed) <= this->actor.speedXZ) {
            this->stateFlags &= ~ENHORSE_BOOST;
            this->stateFlags |= ENHORSE_BOOST_DECEL;
        }

    } else if (this->stateFlags & ENHORSE_BOOST_DECEL) {
        if (baseSpeed < this->actor.speedXZ) {
            temp_f12 = this->actor.speedXZ;
            this->actor.speedXZ = temp_f12 - 0.06f;
        } else if (this->actor.speedXZ < baseSpeed) {
            this->actor.speedXZ = baseSpeed;
            this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        }
    } else {
        this->actor.speedXZ +=
            (this->actor.speedXZ <= baseSpeed * (1.0f / 54.0f) * stickMag ? 1.0f : -1.0f) * 50.0f * 0.01f;
        if (baseSpeed < this->actor.speedXZ) {
            this->actor.speedXZ = this->actor.speedXZ - decel;
            if (this->actor.speedXZ < baseSpeed) {
                this->actor.speedXZ = baseSpeed;
            }
        }
    }

    temp_f12 = *stickAnglePtr * (1 / 32236.f);
    traction = 2.2f - (this->actor.speedXZ * (1.0f / this->boostSpeed));
    turn = *stickAnglePtr * temp_f12 * temp_f12 * traction;
    turn = CLAMP(turn, -turnSpeed * traction, turnSpeed * traction);
    this->actor.world.rot.y += turn;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnHorse_StartMountedIdleResetAnim(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartMountedIdle(this);
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
}

void EnHorse_StartMountedIdle(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACT_MOUNTED_IDLE;
    this->animationIdx = ENHORSE_ANIM_IDLE;
    if ((this->curFrame > 35.0f && this->type == HORSE_EPONA) || (this->curFrame > 28.0f && this->type == HORSE_HNI)) {
        if (!(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
            this->stateFlags |= ENHORSE_SANDDUST_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_StartReversingInterruptable(EnHorse* this);
void EnHorse_StartTurning(EnHorse* this);
void EnHorse_StartWalkingFromIdle(EnHorse* this);
void EnHorse_MountedIdleAnim(EnHorse* this);
void EnHorse_StartReversing(EnHorse* this);
void EnHorse_StartWalkingInterruptable(EnHorse* this);
void EnHorse_MountedIdleWhinney(EnHorse* this);
void EnHorse_StartWalking(EnHorse* this);

void EnHorse_MountedIdle(EnHorse* this, PlayState* play) {
    f32 mag;
    s16 angle = 0;

    this->actor.speedXZ = 0;
    EnHorse_StickDirection(&this->curStick, &mag, &angle);
    if (mag > 10.0f && EnHorse_PlayerCanMove(this, play) == true) {
        if (Math_CosS(angle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(this);
        } else if (Math_CosS(angle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(this);
        } else {
            EnHorse_StartWalkingFromIdle(this);
        }
    }
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_MountedIdleAnim(this);
    }
}

void EnHorse_MountedIdleAnim(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_MountedIdleWhinney(this);
}

void EnHorse_MountedIdleWhinney(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING;
    this->animationIdx = ENHORSE_ANIM_WHINNEY;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorse_MountedIdleWhinneying(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle = 0;

    this->actor.speedXZ = 0;
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f && EnHorse_PlayerCanMove(this, play) == true) {
        if (Math_CosS(stickAngle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(this);
        } else if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(this);
        } else {
            EnHorse_StartWalkingFromIdle(this);
        }
    }
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_StartMountedIdleResetAnim(this);
    }
}

void EnHorse_StartTurning(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TURN;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTurn(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 clampedYaw;
    s16 stickAngle;

    this->actor.speedXZ = 0;
    EnHorse_PlayWalkingSound(this);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f) {
        if (!EnHorse_PlayerCanMove(this, play)) {
            EnHorse_StartMountedIdleResetAnim(this);
        } else if (Math_CosS(stickAngle) <= -0.5f) {
            EnHorse_StartReversingInterruptable(this);
        } else if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            clampedYaw = CLAMP(stickAngle, -800.0f, 800.0f);
            this->actor.world.rot.y = this->actor.world.rot.y + clampedYaw;
            this->actor.shape.rot.y = this->actor.world.rot.y;
        } else {
            EnHorse_StartWalkingInterruptable(this);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (Math_CosS(stickAngle) <= 0.7071) { // cos(45 degrees)
            EnHorse_StartTurning(this);
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }
}

void EnHorse_StartWalkingFromIdle(EnHorse* this) {
    EnHorse_StartWalkingInterruptable(this);

    if (!(this->stateFlags & ENHORSE_FLAG_8) && !(this->stateFlags & ENHORSE_FLAG_9)) {
        this->stateFlags |= ENHORSE_FLAG_9;
        this->waitTimer = 8;
        return;
    }
    this->waitTimer = 0;
}

void EnHorse_StartWalkingInterruptable(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    EnHorse_StartWalking(this);
}

void EnHorse_StartWalking(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedWalkingReset(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
}

void EnHorse_StartTrotting(EnHorse* this);

void EnHorse_MountedWalk(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_PlayWalkingSound(this);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (this->noInputTimerMax == 0.0f ||
        (this->noInputTimer > 0.0f && this->noInputTimer < this->noInputTimerMax - 20.0f)) {
        EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 3.0f, 400);
    } else {
        this->actor.speedXZ = 3.0f;
    }

    if (this->actor.speedXZ == 0.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartMountedIdleResetAnim(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    } else if (this->actor.speedXZ > 3.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        EnHorse_StartTrotting(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    }

    if (this->noInputTimer > 0.0f) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0.0f) {
            this->noInputTimerMax = 0;
        }
    }

    if (this->waitTimer <= 0) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.75f;
        if (SkelAnime_Update(&this->skin.skelAnime) || this->actor.speedXZ == 0.0f) {
            if (this->noInputTimer <= 0.0f) {
                if (this->actor.speedXZ > 3.0f) {
                    EnHorse_StartTrotting(this);
                    this->noInputTimer = 0;
                    this->noInputTimerMax = 0;
                } else if ((stickMag < 10.0f) || (Math_CosS(stickAngle) <= -0.5f)) {
                    EnHorse_StartMountedIdleResetAnim(this);
                    this->noInputTimer = 0;
                    this->noInputTimerMax = 0;
                } else {
                    EnHorse_MountedWalkingReset(this);
                }
            }
        }
    } else {
        this->actor.speedXZ = 0.0f;
        this->waitTimer--;
    }
}

void EnHorse_StartTrotting(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TROT;
    this->animationIdx = ENHORSE_ANIM_TROT;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedTrotReset(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TROT;
    this->animationIdx = ENHORSE_ANIM_TROT;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
}

void EnHorse_StartGallopingInterruptable(EnHorse* this);

void EnHorse_MountedTrot(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 6.0f, 400);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (this->actor.speedXZ < 3.0f) {
        EnHorse_StartWalkingInterruptable(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.375f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_PlayTrottingSound(this);
        func_800AA000(0.0f, 60, 8, 255);
        if (this->actor.speedXZ >= 6.0f) {
            EnHorse_StartGallopingInterruptable(this);
        } else if (this->actor.speedXZ < 3.0f) {
            EnHorse_StartWalkingInterruptable(this);
        } else {
            EnHorse_MountedTrotReset(this);
        }
    }
}

void EnHorse_StartGallopingInterruptable(EnHorse* this) {
    this->noInputTimerMax = 0;
    this->noInputTimer = 0;
    EnHorse_StartGalloping(this);
}

void EnHorse_StartGalloping(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->unk_234 = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedGallopReset(EnHorse* this) {
    this->noInputTimerMax = 0;
    this->noInputTimer = 0;
    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->unk_234 = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
}

void EnHorse_JumpLanding(EnHorse* this, PlayState* play) {
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y += y * 0.01f;
    this->postDrawFunc = NULL;
}

void EnHorse_StartBraking(EnHorse* this, PlayState* play);

void EnHorse_MountedGallop(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);

    if (this->noInputTimer <= 0.0f) {
        EnHorse_UpdateSpeed(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 8.0f, 0x190);
    } else if (this->noInputTimer > 0.0f) {
        this->noInputTimer -= 1;
        this->actor.speedXZ = 8.0f;
    }
    if (this->actor.speedXZ < 6.0f) {
        EnHorse_StartTrotting(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.3f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(this);
        func_800AA000(0, 120, 8, 255);
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (stickMag >= 10.0f && Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartBraking(this, play);
            } else if (this->actor.speedXZ < 6.0f) {
                EnHorse_StartTrotting(this);
            } else {
                EnHorse_MountedGallopReset(this);
            }
            return;
        }
        EnHorse_MountedGallopReset(this);
    }
}

void EnHorse_StartRearing(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_REARING;
    this->animationIdx = ENHORSE_ANIM_REARING;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    func_800AA000(0.0f, 180, 20, 100);
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_MountedRearing(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    this->actor.speedXZ = 0;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            func_800AA000(0, 180, 20, 100);
        }
    }

    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (EnHorse_PlayerCanMove(this, play) == true) {
            if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
                EnHorse_StartReversing(this);
            } else if (this->stateFlags & ENHORSE_FORCE_WALKING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_WALKING;
                EnHorse_StartWalking(this);
            } else if (Math_CosS(stickAngle) <= -0.5f) {
                EnHorse_StartReversingInterruptable(this);
            } else {
                EnHorse_StartMountedIdleResetAnim(this);
            }
            return;
        }
        EnHorse_StartMountedIdleResetAnim(this);
    }
}

void EnHorse_StartBraking(EnHorse* this, PlayState* play) {
    this->action = ENHORSE_ACT_STOPPING;
    this->animationIdx = ENHORSE_ANIM_STOPPING;

    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SLIP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->stateFlags |= ENHORSE_STOPPING_NEIGH_SOUND;
    this->stateFlags &= ~ENHORSE_BOOST;
}

void EnHorse_Stopping(EnHorse* this, PlayState* play) {
    if (this->actor.speedXZ > 0.0f) {
        this->actor.speedXZ = this->actor.speedXZ - 0.6f;
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ = 0.0f;
        }
    }

    if (this->stateFlags & ENHORSE_STOPPING_NEIGH_SOUND && this->skin.skelAnime.curFrame > 29.0f) {
        this->actor.speedXZ = 0.0f;
        if (Rand_ZeroOne() > 0.5) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            func_800AA000(0.0f, 180, 20, 100);
            this->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }

    if (this->skin.skelAnime.curFrame > 29.0f) {
        this->actor.speedXZ = 0.0f;
    } else if (this->actor.speedXZ > 3.0f && this->stateFlags & ENHORSE_FORCE_REVERSING) {
        this->actor.speedXZ = 3.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
            this->noInputTimer = 100;
            this->noInputTimerMax = 100;
            EnHorse_StartReversing(this);
            this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
        } else {
            EnHorse_StartMountedIdleResetAnim(this);
        }
    }
}

void EnHorse_StartReversingInterruptable(EnHorse* this) {
    this->noInputTimerMax = 0;
    this->noInputTimer = 0;
    EnHorse_StartReversing(this);
}

void EnHorse_StartReversing(EnHorse* this) {
    this->action = ENHORSE_ACT_REVERSE;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->soundTimer = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_LOOP, -3.0f);
}

void EnHorse_Reverse(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;
    s16 turnAmount;
    Player* player = GET_PLAYER(play);

    EnHorse_PlayWalkingSound(this);
    EnHorse_StickDirection(&this->curStick, &stickMag, &stickAngle);
    if (EnHorse_PlayerCanMove(this, play) == true) {
        if (this->noInputTimerMax == 0.0f ||
            (this->noInputTimer > 0.0f && this->noInputTimer < this->noInputTimerMax - 20.0f)) {
            if (stickMag < 10.0f && this->noInputTimer <= 0.0f) {
                EnHorse_StartMountedIdleResetAnim(this);
                this->actor.speedXZ = 0.0f;
                return;
            }
            if (stickMag < 10.0f) {
                stickAngle = -0x7FFF;
            } else if (Math_CosS(stickAngle) > -0.5f) {
                this->noInputTimerMax = 0;
                EnHorse_StartMountedIdleResetAnim(this);
                this->actor.speedXZ = 0.0f;
                return;
            }
        } else if (stickMag < 10.0f) {
            stickAngle = -0x7FFF;
        }
    } else if (player->actor.flags & ACTOR_FLAG_8) {
        EnHorse_StartMountedIdleResetAnim(this);
        this->actor.speedXZ = 0.0f;
        return;
    } else {
        stickAngle = -0x7FFF;
    }

    this->actor.speedXZ = -2.0f;
    turnAmount = 0x7FFF - stickAngle;
    turnAmount = CLAMP(turnAmount, -1200.0f, 1200.0f);
    this->actor.world.rot.y += turnAmount;
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->noInputTimer > 0.0f) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0.0f) {
            this->noInputTimerMax = 0;
        }
    }
    this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.5f * 1.5f;
    if (SkelAnime_Update(&this->skin.skelAnime) && (f32)this->noInputTimer <= 0.0f &&
        EnHorse_PlayerCanMove(this, play) == true) {
        if (stickMag > 10.0f && Math_CosS(stickAngle) <= -0.5f) {
            this->noInputTimerMax = 0;
            EnHorse_StartReversingInterruptable(this);
        } else if (stickMag < 10.0f) {
            this->noInputTimerMax = 0;
            EnHorse_StartMountedIdleResetAnim(this);
        } else {
            EnHorse_StartReversing(this);
        }
    }
}

void EnHorse_StartLowJump(EnHorse* this, PlayState* play);

void EnHorse_LowJumpInit(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartLowJump(this, play);
}

void EnHorse_StartLowJump(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_LOW_JUMP;
    this->animationIdx = ENHORSE_ANIM_LOW_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;

    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_Stub1(EnHorse* this) {
}

void EnHorse_LowJump(EnHorse* this, PlayState* play) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speedXZ = 12.0f;
    if (curFrame > 17.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0) {
            this->actor.velocity.y = -6.0f;
        }
        if (this->actor.world.pos.y < this->actor.floorHeight + 90.0f) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        (curFrame > 17.0f && this->actor.world.pos.y < this->actor.floorHeight - this->actor.velocity.y + 80.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_80028A54(play, 25.0f, &this->actor.world.pos);
        EnHorse_JumpLanding(this, play);
    }
}

void EnHorse_StartHighJump(EnHorse* this, PlayState* play);

void EnHorse_HighJumpInit(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_StartHighJump(this, play);
}

void EnHorse_StartHighJump(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_HIGH_JUMP;
    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->jumpStartY = this->actor.world.pos.y;
    this->postDrawFunc = NULL;

    this->actor.gravity = 0;
    this->actor.velocity.y = 0.0f;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_Stub2(EnHorse* this) {
}

void EnHorse_HighJump(EnHorse* this, PlayState* play) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speedXZ = 13.0f;
    if (curFrame > 23.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0) {
            this->actor.velocity.y = -10.5f;
        }

        if (this->actor.world.pos.y < this->actor.floorHeight + 90.0f) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime) ||
        (curFrame > 23.0f && this->actor.world.pos.y < this->actor.floorHeight - this->actor.velocity.y + 80.0f)) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_80028A54(play, 25.0f, &this->actor.world.pos);
        EnHorse_JumpLanding(this, play);
    }
}

void EnHorse_InitInactive(EnHorse* this) {
    this->cyl1.base.ocFlags1 &= ~OC1_ON;
    this->cyl2.base.ocFlags1 &= ~OC1_ON;
    this->jntSph.base.ocFlags1 &= ~OC1_ON;
    this->action = ENHORSE_ACT_INACTIVE;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->stateFlags |= ENHORSE_INACTIVE;
    this->followTimer = 0;
}

void EnHorse_SetFollowAnimation(EnHorse* this, PlayState* play);

void EnHorse_Inactive(EnHorse* this, PlayState* play2) {
    PlayState* play = play2;

    if (DREG(53) != 0 && this->type == HORSE_EPONA) {
        DREG(53) = 0;
        if (EnHorse_Spawn(this, play) != 0) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            this->stateFlags &= ~ENHORSE_INACTIVE;
            gSaveContext.horseData.scene = play->sceneNum;

            // Focus the camera on Epona
            Camera_SetParam(play->cameraPtrs[0], 8, this);
            Camera_ChangeSetting(play->cameraPtrs[0], 0x38);
            Camera_SetCameraData(play->cameraPtrs[0], 4, NULL, NULL, 0x51, 0, 0);
        }
    }
    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        this->followTimer = 0;
        EnHorse_SetFollowAnimation(this, play);
        this->actor.params = 0;
        this->cyl1.base.ocFlags1 |= OC1_ON;
        this->cyl2.base.ocFlags1 |= OC1_ON;
        this->jntSph.base.ocFlags1 |= OC1_ON;
    }
}

void EnHorse_PlayIdleAnimation(EnHorse* this, s32 anim, f32 morphFrames, f32 startFrame) {
    this->action = ENHORSE_ACT_IDLE;
    this->actor.speedXZ = 0.0f;
    if (anim != ENHORSE_ANIM_IDLE && anim != ENHORSE_ANIM_WHINNEY && anim != ENHORSE_ANIM_REARING) {
        anim = ENHORSE_ANIM_IDLE;
    }
    if (anim != this->animationIdx) {
        this->animationIdx = anim;
        if (this->animationIdx == ENHORSE_ANIM_IDLE) {
            this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
        } else if (this->animationIdx == ENHORSE_ANIM_WHINNEY) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        } else if (this->animationIdx == ENHORSE_ANIM_REARING) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
            this->stateFlags &= ~ENHORSE_LAND2_SOUND;
        }

        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    }
}

void EnHorse_ChangeIdleAnimation(EnHorse* this, s32 anim, f32 morphFrames) {
    EnHorse_PlayIdleAnimation(this, anim, morphFrames, this->curFrame);
}

void EnHorse_ResetIdleAnimation(EnHorse* this) {
    this->animationIdx = ENHORSE_ANIM_WALK; // this forces anim 0 to play from the beginning
    EnHorse_PlayIdleAnimation(this, this->animationIdx, 0, 0);
}

void EnHorse_StartIdleRidable(EnHorse* this) {
    EnHorse_ResetIdleAnimation(this);
    this->stateFlags &= ~ENHORSE_UNRIDEABLE;
}

void EnHorse_StartMovingAnimation(EnHorse* this, s32 arg1, f32 arg2, f32 arg3);

void EnHorse_Idle(EnHorse* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;
    EnHorse_IdleAnimSounds(this, play);

    if (DREG(53) && this->type == HORSE_EPONA) {
        DREG(53) = 0;
        if (!func_80A5BBBC(play, this, &this->actor.world.pos)) {
            if (EnHorse_Spawn(this, play)) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
                this->followTimer = 0;
                EnHorse_SetFollowAnimation(this, play);
                Camera_SetParam(play->cameraPtrs[0], 8, this);
                Camera_ChangeSetting(play->cameraPtrs[0], 0x38);
                Camera_SetCameraData(play->cameraPtrs[0], 4, NULL, NULL, 0x51, 0, 0);
            }
        } else {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
            this->followTimer = 0;
            EnHorse_StartMovingAnimation(this, 6, -3.0f, 0.0f);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        s32 idleAnimIdx = 0;

        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            if (this->animationIdx == ENHORSE_ANIM_WHINNEY) {
                idleAnimIdx = 1;
            } else if (this->animationIdx == ENHORSE_ANIM_REARING) {
                idleAnimIdx = 2;
            }
        }

        // Play one of the two other idle animations
        EnHorse_PlayIdleAnimation(this, sIdleAnimIds[(Rand_ZeroOne() > 0.5f ? 0 : 1) + idleAnimIdx * 2], 0.0f, 0.0f);
    }
}

void EnHorse_StartMovingAnimation(EnHorse* this, s32 animId, f32 morphFrames, f32 startFrame) {
    this->action = ENHORSE_ACT_FOLLOW_PLAYER;
    this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
    if (animId != ENHORSE_ANIM_TROT && animId != ENHORSE_ANIM_GALLOP && animId != ENHORSE_ANIM_WALK) {
        animId = ENHORSE_ANIM_WALK;
    }
    if (this->animationIdx != animId) {
        this->animationIdx = animId;
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, startFrame,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         0.0f);
    }
}

void EnHorse_SetFollowAnimation(EnHorse* this, PlayState* play) {
    s32 animId = ENHORSE_ANIM_WALK;
    f32 distToPlayer;

    distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
    if (distToPlayer > 400.0f) {
        animId = ENHORSE_ANIM_GALLOP;
    } else if (!(distToPlayer <= 300.0f)) {
        if (distToPlayer <= 400.0f) {
            animId = ENHORSE_ANIM_TROT;
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else if (distToPlayer < 300.0f) {
            animId = ENHORSE_ANIM_WALK;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        if (distToPlayer > 300.0f) {
            animId = ENHORSE_ANIM_TROT;
        } else {
            animId = ENHORSE_ANIM_WALK;
        }
    }
    EnHorse_StartMovingAnimation(this, animId, -3.0f, 0.0f);
}

void EnHorse_FollowPlayer(EnHorse* this, PlayState* play) {
    f32 distToPlayer;
    f32 angleDiff;

    DREG(53) = 0;
    distToPlayer = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);

    // First rotate if the player is behind
    if ((this->playerDir == PLAYER_DIR_BACK_R || this->playerDir == PLAYER_DIR_BACK_L) &&
        (distToPlayer > 300.0f && !(this->stateFlags & ENHORSE_TURNING_TO_PLAYER))) {
        this->animationIdx = ENHORSE_ANIM_REARING;
        this->stateFlags |= ENHORSE_TURNING_TO_PLAYER;
        this->angleToPlayer = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor);
        angleDiff = (f32)this->angleToPlayer - (f32)this->actor.world.rot.y;
        if (angleDiff > 32767.f) {
            angleDiff -= 32767.0f;
        } else if (angleDiff < -32767) {
            angleDiff += 32767;
        }

        this->followPlayerTurnSpeed =
            angleDiff / Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]);
        Animation_PlayOnce(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx]);
        this->skin.skelAnime.playSpeed = 1.0f;
        this->stateFlags &= ~ENHORSE_LAND2_SOUND;
        this->unk_21C = this->unk_228;
    } else if (this->stateFlags & ENHORSE_TURNING_TO_PLAYER) {
        this->actor.world.rot.y = this->actor.world.rot.y + this->followPlayerTurnSpeed;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (this->curFrame > 25.0f) {
            if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
                this->stateFlags |= ENHORSE_LAND2_SOUND;
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }
        }
    } else {
        EnHorse_RotateToPlayer(this, play);
    }

    if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        this->actor.speedXZ = 8;
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.3f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        this->actor.speedXZ = 6;
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.375f;
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        this->actor.speedXZ = 3;
        EnHorse_PlayWalkingSound(this);
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.75f;
    } else {
        this->actor.speedXZ = 0;
        this->skin.skelAnime.playSpeed = 1.0f;
    }

    if (!(this->stateFlags & ENHORSE_TURNING_TO_PLAYER) && ++this->followTimer > 300) {
        EnHorse_StartIdleRidable(this);
        this->unk_21C = this->unk_228;

        if (this->stateFlags & ENHORSE_DRAW) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
            EnHorse_PlayGallopingSound(this);
        } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
            EnHorse_PlayTrottingSound(this);
        }
        this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
        if (distToPlayer < 100.0f) {
            EnHorse_StartIdleRidable(this);
        } else {
            EnHorse_SetFollowAnimation(this, play);
        }
    }
}

void EnHorse_UpdateIngoHorseAnim(EnHorse* this);

void EnHorse_InitIngoHorse(EnHorse* this) {
    this->curRaceWaypoint = 0;
    this->soundTimer = 0;
    this->actor.speedXZ = 0.0f;
    EnHorse_UpdateIngoHorseAnim(this);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_IT_INGO_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
}

void EnHorse_SetIngoAnimation(s32 idx, f32 curFrame, s32 arg2, s16* animIdxOut, f32* curFrameOut) {
    *animIdxOut = sIngoAnimations[idx];
    *curFrameOut = curFrame;
    if (idx == 3 || idx == 7 || idx == 8 || idx == 4) {
        *curFrameOut = 0.0f;
    }
    if (arg2 == 1) {
        if (idx == 5) {
            *animIdxOut = 4;
            *curFrameOut = curFrame;
        } else if (idx == 6) {
            *animIdxOut = 3;
            *curFrameOut = curFrame;
        }
    }
}

void EnHorse_UpdateIngoHorseAnim(EnHorse* this) {
    s32 animChanged = 0;
    f32 animSpeed;

    this->action = ENHORSE_ACT_INGO_RACE;
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
    if (this->actor.speedXZ == 0.0f) {
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speedXZ <= 3.0f) {
        if (this->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_WALK;
    } else if (this->actor.speedXZ <= 6.0f) {
        if (this->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (this->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speedXZ * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speedXZ * 0.25f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speedXZ * 0.2f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                         sPlaybackSpeeds[this->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                         sPlaybackSpeeds[this->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void EnHorse_UpdateIngoRace(EnHorse* this, PlayState* play) {
    f32 playSpeed;

    if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
        EnHorse_IdleAnimSounds(this, play);
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(this);
    }

    EnHorse_UpdateIngoRaceInfo(this, play, &sIngoRace);
    if (!this->inRace) {
        this->actor.speedXZ = 0.0f;
        this->rider->speedXZ = 0.0f;
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateIngoHorseAnim(this);
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speedXZ * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speedXZ * 0.25f;
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speedXZ * 0.2f;
    } else {
        playSpeed = 1.0f;
    }
    this->skin.skelAnime.playSpeed = playSpeed;
    if (SkelAnime_Update(&this->skin.skelAnime) ||
        (this->animationIdx == ENHORSE_ANIM_IDLE && this->actor.speedXZ != 0.0f)) {
        EnHorse_UpdateIngoHorseAnim(this);
    }

    if (this->stateFlags & ENHORSE_INGO_WON) {
        ((EnIn*)this->rider)->animationIdx = 7;
        ((EnIn*)this->rider)->unk_1E0 = 0;
        return;
    }

    EnHorse_SetIngoAnimation(this->animationIdx, this->skin.skelAnime.curFrame, this->ingoRaceFlags & 1,
                             &((EnIn*)this->rider)->animationIdx, &((EnIn*)this->rider)->unk_1E0);
}

void EnHorse_CsMoveInit(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 1;
    Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                               this->actor.speedXZ * 0.3f);
}

void EnHorse_CsPlayHighJumpAnim(EnHorse* this, PlayState* play);

void EnHorse_CsMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    Vec3f endPos;
    f32 speed = 8.0f;

    endPos.x = action->endPos.x;
    endPos.y = action->endPos.y;
    endPos.z = action->endPos.z;
    if (Math3D_Vec3f_DistXYZ(&endPos, &this->actor.world.pos) > speed) {
        EnHorse_RotateToPoint(this, play, &endPos, 400);
        this->actor.speedXZ = speed;
        this->skin.skelAnime.playSpeed = speed * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(this);
        func_800AA000(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                                   this->actor.speedXZ * 0.3f);
    }
}

void EnHorse_CsSetAnimHighJump(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    EnHorse_CsPlayHighJumpAnim(this, play);
}

void EnHorse_CsPlayHighJumpAnim(EnHorse* this, PlayState* play) {
    f32 curFrame;
    f32 y;
    Vec3s* jointTable;

    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.5f, curFrame,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
}

void EnHorse_CsJumpInit(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    EnHorse_CsSetAnimHighJump(this, play);
    this->cutsceneAction = 2;
    this->cutsceneFlags &= ~1;
}

void EnHorse_CsJump(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    f32 temp_f2;

    if (this->cutsceneFlags & 1) {
        EnHorse_CsMoveToPoint(this, play, action);
        return;
    }
    temp_f2 = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speedXZ = 13.0f;
    if (temp_f2 > 19.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -10.5f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        Vec3s* jointTable;
        f32 y;

        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }
    if (SkelAnime_Update(&this->skin.skelAnime) ||
        (temp_f2 > 19.0f && this->actor.world.pos.y < (this->actor.floorHeight - this->actor.velocity.y) + 80.0f)) {
        Vec3s* jointTable;
        f32 y;

        this->cutsceneFlags |= 1;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.velocity.y = 0;
        this->actor.world.pos.y = this->actor.floorHeight;
        func_80028A54(play, 25.0f, &this->actor.world.pos);
        this->animationIdx = ENHORSE_ANIM_GALLOP;
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                                   sPlaybackSpeeds[6]);
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->riderPos.y += y * 0.01f;
        this->postDrawFunc = NULL;
    }
}

void EnHorse_CsRearingInit(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->animationIdx = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 3;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsRearing(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->actor.speedXZ = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), 0, 0.0f);
        }
    }
}

void EnHorse_WarpMoveInit(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->actor.world.pos.x = action->startPos.x;
    this->actor.world.pos.y = action->startPos.y;
    this->actor.world.pos.z = action->startPos.z;
    this->actor.prevPos = this->actor.world.pos;
    this->actor.world.rot.y = action->urot.y;
    this->actor.shape.rot = this->actor.world.rot;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 4;
    Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                               this->actor.speedXZ * 0.3f);
}

void EnHorse_CsWarpMoveToPoint(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    Vec3f endPos;
    f32 speed = 8.0f;

    endPos.x = action->endPos.x;
    endPos.y = action->endPos.y;
    endPos.z = action->endPos.z;
    if (Math3D_Vec3f_DistXYZ(&endPos, &this->actor.world.pos) > speed) {
        EnHorse_RotateToPoint(this, play, &endPos, 400);
        this->actor.speedXZ = speed;
        this->skin.skelAnime.playSpeed = speed * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speedXZ = 0.0f;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        EnHorse_PlayGallopingSound(this);
        func_800AA000(0.0f, 120, 8, 255);
        Animation_PlayOnceSetSpeed(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                                   this->actor.speedXZ * 0.3f);
    }
}

void EnHorse_CsWarpRearingInit(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->actor.world.pos.x = action->startPos.x;
    this->actor.world.pos.y = action->startPos.y;
    this->actor.world.pos.z = action->startPos.z;
    this->actor.prevPos = this->actor.world.pos;
    this->actor.world.rot.y = action->urot.y;
    this->actor.shape.rot = this->actor.world.rot;
    this->animationIdx = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 5;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void EnHorse_CsWarpRearing(EnHorse* this, PlayState* play, CsCmdActorAction* action) {
    this->actor.speedXZ = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), 0, 0.0f);
        }
    }
}

void EnHorse_InitCutscene(EnHorse* this, PlayState* play) {
    this->playerControlled = false;
    this->action = ENHORSE_ACT_CS_UPDATE;
    this->cutsceneAction = 0;
    this->actor.speedXZ = 0.0f;
}

s32 EnHorse_GetCutsceneFunctionIndex(s32 csAction) {
    s32 numActions = ARRAY_COUNT(sCsActionTable); // prevents unrolling
    s32 i;

    for (i = 0; i < numActions; i++) {
        if (csAction == sCsActionTable[i].csAction) {
            return sCsActionTable[i].csFuncIdx;
        }
        if (csAction < sCsActionTable[i].csAction) {
            return 0;
        }
    }
    return 0;
}

void EnHorse_CutsceneUpdate(EnHorse* this, PlayState* play) {
    s32 csFunctionIdx;
    CsCmdActorAction* linkCsAction = play->csCtx.linkAction;

    if (play->csCtx.state == 3) {
        this->playerControlled = 1;
        this->actor.params = 10;
        this->action = ENHORSE_ACT_IDLE;
        EnHorse_Freeze(this);
        return;
    }
    if (linkCsAction != 0 && (uint32_t)(uintptr_t)linkCsAction != 0xABABABAB) {
        csFunctionIdx = EnHorse_GetCutsceneFunctionIndex(linkCsAction->action); 
        if (csFunctionIdx != 0) {
            if (this->cutsceneAction != csFunctionIdx) {
                if (this->cutsceneAction == 0) {
                    this->actor.world.pos.x = linkCsAction->startPos.x;
                    this->actor.world.pos.y = linkCsAction->startPos.y;
                    this->actor.world.pos.z = linkCsAction->startPos.z;
                    this->actor.world.rot.y = linkCsAction->urot.y;
                    this->actor.shape.rot = this->actor.world.rot;
                    this->actor.prevPos = this->actor.world.pos;
                }
                this->cutsceneAction = csFunctionIdx;
                sCutsceneInitFuncs[this->cutsceneAction](this, play, linkCsAction);
            }
            sCutsceneActionFuncs[this->cutsceneAction](this, play, linkCsAction);
        }
    }
}

s32 EnHorse_UpdateHbaRaceInfo(EnHorse* this, PlayState* play, RaceInfo* raceInfo) {
    Vec3f pos;
    f32 px;
    f32 pz;
    f32 d;

    EnHorse_RaceWaypointPos(raceInfo->waypoints, this->curRaceWaypoint, &pos);
    Math3D_RotateXZPlane(&pos, raceInfo->waypoints[this->curRaceWaypoint].angle, &px, &pz, &d);

    if (this->curRaceWaypoint >= raceInfo->numWaypoints - 1 &&
        Math3D_Vec3f_DistXYZ(&pos, &this->actor.world.pos) < DREG(8)) {
        this->hbaFlags |= 2;
    }

    if (((this->actor.world.pos.x * px) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= raceInfo->numWaypoints) {
            this->hbaFlags |= 1;
            return 1;
        }
    }

    if (!(this->hbaFlags & 1)) {
        EnHorse_RotateToPoint(this, play, &pos, 800);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.speedXZ < raceInfo->waypoints[this->curRaceWaypoint].speed && !(this->hbaFlags & 1)) {
        this->actor.speedXZ += 0.4f;
    } else {
        this->actor.speedXZ -= 0.4f;
        if (this->actor.speedXZ < 0.0f) {
            this->actor.speedXZ = 0.0f;
        }
    }
    return 0;
}

void EnHorse_UpdateHbaAnim(EnHorse* this);

void EnHorse_InitHorsebackArchery(EnHorse* this) {
    this->hbaStarted = 0;
    this->soundTimer = 0;
    this->curRaceWaypoint = 0;
    this->hbaTimer = 0;
    this->actor.speedXZ = 0.0f;
    EnHorse_UpdateHbaAnim(this);
}

void EnHorse_UpdateHbaAnim(EnHorse* this) {
    s32 animChanged = 0;
    f32 animSpeed;

    this->action = ENHORSE_ACT_HBA;
    if (this->actor.speedXZ == 0.0f) {
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speedXZ <= 3.0f) {
        if (this->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_WALK;
    } else if (this->actor.speedXZ <= 6.0f) {
        if (this->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (this->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speedXZ * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speedXZ * 0.25f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        func_800AA000(0.0f, 60, 8, 255);
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speedXZ * 0.2f;
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        func_800AA000(0.0f, 120, 8, 255);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                         sPlaybackSpeeds[this->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         -3.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx],
                         sPlaybackSpeeds[this->animationIdx] * animSpeed * 1.5f, 0,
                         Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void EnHorse_UpdateHorsebackArchery(EnHorse* this, PlayState* play) {
    f32 playSpeed;
    s32 sp20;

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        EnHorse_PlayWalkingSound(this);
    }
    if (play->interfaceCtx.hbaAmmo == 0) {
        this->hbaTimer++;
    }

    sp20 = func_800F5A58(NA_BGM_HORSE_GOAL);
    EnHorse_UpdateHbaRaceInfo(this, play, &sHbaInfo);
    if (this->hbaFlags & 1 || this->hbaTimer >= 46) {
        if (sp20 != 1 && gSaveContext.minigameState != 3) {
            gSaveContext.cutsceneIndex = 0;
            play->nextEntranceIndex = 0x3B0;
            play->sceneLoadFlag = 0x14;
            play->fadeTransition = 0x20;
        }
    }

    if (play->interfaceCtx.hbaAmmo != 0) {
        if (!(this->hbaFlags & 2)) {
            if (gSaveContext.infTable[25] & 1) {
                if ((s32)gSaveContext.minigameScore >= 1500) {
                    this->hbaFlags |= 4;
                }
            } else {
                if ((s32)gSaveContext.minigameScore >= 1000) {
                    this->hbaFlags |= 4;
                }
            }
        }
    }

    if ((play->interfaceCtx.hbaAmmo == 0) || (this->hbaFlags & 2)) {
        if (this->hbaFlags & 4) {
            this->hbaFlags &= ~4;
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HORSE_GOAL);
        }
    }

    if (!this->hbaStarted) {
        this->actor.speedXZ = 0.0f;
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            EnHorse_UpdateHbaAnim(this);
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speedXZ * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speedXZ * 0.25f;
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speedXZ * 0.2f;
    } else {
        playSpeed = 1.0f;
    }

    this->skin.skelAnime.playSpeed = playSpeed;
    if (SkelAnime_Update(&this->skin.skelAnime) ||
        (this->animationIdx == ENHORSE_ANIM_IDLE && this->actor.speedXZ != 0.0f)) {
        EnHorse_UpdateHbaAnim(this);
    }
}

void EnHorse_InitFleePlayer(EnHorse* this) {
    this->action = ENHORSE_ACT_FLEE_PLAYER;
    this->stateFlags |= ENHORSE_UNRIDEABLE;
    this->actor.speedXZ = 0.0f;
}

void EnHorse_FleePlayer(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToHome;
    f32 playerDistToHome;
    f32 distToPlayer;
    s32 nextAnim = this->animationIdx;
    s32 animFinished;
    s16 yaw;

    if (DREG(53) || this->type == HORSE_HNI) {
        EnHorse_StartIdleRidable(this);
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }

    distToHome = Math3D_Vec3f_DistXYZ(&this->actor.home.pos, &this->actor.world.pos);
    playerDistToHome = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos);
    distToPlayer = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.world.pos);

    // Run home
    if (playerDistToHome > 300.0f) {
        if (distToHome > 150.0f) {
            this->actor.speedXZ += 0.4f;
            if (this->actor.speedXZ > 8.0f) {
                this->actor.speedXZ = 8.0f;
            }
        } else {
            this->actor.speedXZ -= 0.47f;
            if (this->actor.speedXZ < 0.0f) {
                this->actor.speedXZ = 0.0f;
            }
        }
    } else {
        // Run away from Link
        if (distToPlayer < 300.0f) {
            this->actor.speedXZ += 0.4f;
            if (this->actor.speedXZ > 8.0f) {
                this->actor.speedXZ = 8.0f;
            }
        } else {
            this->actor.speedXZ -= 0.47f;
            if (this->actor.speedXZ < 0.0f) {
                this->actor.speedXZ = 0.0f;
            }
        }
    }

    if (this->actor.speedXZ >= 6.0f) { // hoof it
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.3f;
        nextAnim = ENHORSE_ANIM_GALLOP;
    } else if (this->actor.speedXZ >= 3.0f) { // trot
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.375f;
        nextAnim = ENHORSE_ANIM_TROT;
    } else if (this->actor.speedXZ > 0.1f) { // walk
        this->skin.skelAnime.playSpeed = this->actor.speedXZ * 0.75f;
        nextAnim = ENHORSE_ANIM_WALK;
        EnHorse_PlayWalkingSound(this);
    } else { // idle
        nextAnim = Rand_ZeroOne() > 0.5f ? 1 : 0;
        EnHorse_IdleAnimSounds(this, play);
        this->skin.skelAnime.playSpeed = 1.0f;
    }

    // Turn away from Link, or towards home
    if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
        if (playerDistToHome < 300.0f) {
            yaw = player->actor.shape.rot.y;
            yaw += (Actor_WorldYawTowardActor(&this->actor, &player->actor) > 0 ? 1 : -1) * 0x3FFF;
        } else {
            yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (yaw > 400) {
            this->actor.world.rot.y += 400;
        } else if (yaw < -400) {
            this->actor.world.rot.y -= 400;
        } else {
            this->actor.world.rot.y += yaw;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    animFinished = SkelAnime_Update(&this->skin.skelAnime);

    if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
        if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
            this->animationIdx = nextAnim;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
            if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
                EnHorse_PlayGallopingSound(this);
            } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
                EnHorse_PlayTrottingSound(this);
            }
            return;
        }
    }

    if (animFinished) {
        if (nextAnim == ENHORSE_ANIM_GALLOP) {
            EnHorse_PlayGallopingSound(this);
        } else if (nextAnim == ENHORSE_ANIM_TROT) {
            EnHorse_PlayTrottingSound(this);
        }

        if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
            if (nextAnim != this->animationIdx) {
                this->animationIdx = nextAnim;
                Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            } else {
                if (Rand_ZeroOne() > 0.5f) {
                    this->animationIdx = ENHORSE_ANIM_IDLE;
                    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
                } else {
                    this->animationIdx = ENHORSE_ANIM_WHINNEY;
                    this->unk_21C = this->unk_228;
                    if (this->stateFlags & ENHORSE_DRAW) {
                        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
                }
                Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                                 Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            }
        }

        if (nextAnim != this->animationIdx) {
            this->animationIdx = nextAnim;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             0.0f);
        }
        return;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        if (nextAnim == ENHORSE_ANIM_IDLE || nextAnim == ENHORSE_ANIM_WHINNEY) {
            this->animationIdx = nextAnim;
            Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        }
    }
}

void EnHorse_BridgeJumpInit(EnHorse* this, PlayState* play) {
    f32 y;

    func_80028A54(play, 25.0f, &this->actor.world.pos);
    this->action = ENHORSE_ACT_BRIDGE_JUMP;
    this->stateFlags |= ENHORSE_JUMPING;
    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    y = this->skin.skelAnime.jointTable->y;
    y = y * 0.01f;
    this->bridgeJumpStart = this->actor.world.pos;
    this->bridgeJumpStart.y += y;
    this->bridgeJumpYVel =
        (((sBridgeJumps[this->bridgeJumpIdx].pos.y + 48.7f) - this->bridgeJumpStart.y) - -360.0f) / 30.0f;
    this->riderPos.y -= y;
    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    this->bridgeJumpRelAngle = this->actor.world.rot.y - sBridgeJumps[this->bridgeJumpIdx].angle;
    this->bridgeJumpTimer = 0;
    this->actor.gravity = 0.0f;
    this->actor.speedXZ = 0;
    Animation_Change(&this->skin.skelAnime, sAnimationHeaders[this->type][this->animationIdx], 1.5f, 0.0f,
                     Animation_GetLastFrame(sAnimationHeaders[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }
    Audio_PlaySoundGeneral(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    func_800AA000(0.0f, 170, 10, 10);
    this->postDrawFunc = NULL;
}

void EnHorse_StartBridgeJump(EnHorse* this, PlayState* play) {
    this->postDrawFunc = EnHorse_BridgeJumpInit;
    if (this->bridgeJumpIdx == 0) {
        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFieldFortressCs);
        gSaveContext.cutsceneTrigger = 1;
    } else {
        play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFortressToFieldCs);
        gSaveContext.cutsceneTrigger = 1;
    }
}

void EnHorse_BridgeJumpMove(EnHorse* this, PlayState* play) {
    f32 interp;
    f32 timeSq;

    interp = this->bridgeJumpTimer / 30.0f;
    timeSq = (this->bridgeJumpTimer * this->bridgeJumpTimer);

    this->actor.world.pos.x =
        ((sBridgeJumps[this->bridgeJumpIdx].pos.x - this->bridgeJumpStart.x) * interp) + this->bridgeJumpStart.x;
    this->actor.world.pos.z =
        ((sBridgeJumps[this->bridgeJumpIdx].pos.z - this->bridgeJumpStart.z) * interp) + this->bridgeJumpStart.z;

    this->actor.world.pos.y =
        (this->bridgeJumpStart.y + (this->bridgeJumpYVel * this->bridgeJumpTimer) + (-0.4f * timeSq));

    this->actor.world.rot.y = this->actor.shape.rot.y =
        (sBridgeJumps[this->bridgeJumpIdx].angle + ((1.0f - interp) * this->bridgeJumpRelAngle));
    this->skin.skelAnime.curFrame = 23.0f * interp;
    SkelAnime_Update(&this->skin.skelAnime);
    if (this->bridgeJumpTimer < 30) {
        this->stateFlags |= ENHORSE_FLAG_24;
    }
}

void EnHorse_CheckBridgeJumpLanding(EnHorse* this, PlayState* play) {
    this->actor.speedXZ = 8.0f;
    this->skin.skelAnime.playSpeed = 1.5f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = sBridgeJumps[this->bridgeJumpIdx].pos.y;
        func_80028A54(play, 25.0f, &this->actor.world.pos);
        EnHorse_JumpLanding(this, play);
        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_800AA000(0.0f, 255, 10, 80);
    }
}

void EnHorse_BridgeJump(EnHorse* this, PlayState* play) {
    this->bridgeJumpTimer++;
    if (this->bridgeJumpTimer < 30) {
        EnHorse_BridgeJumpMove(this, play);
        return;
    }
    EnHorse_CheckBridgeJumpLanding(this, play);
}

void EnHorse_Vec3fOffset(Vec3f* src, s16 yaw, f32 dist, f32 height, Vec3f* dst) {
    dst->x = src->x + Math_SinS(yaw) * dist;
    dst->y = src->y + height;
    dst->z = src->z + Math_CosS(yaw) * dist;
}

s32 EnHorse_CalcFloorHeight(EnHorse* this, PlayState* play, Vec3f* pos, CollisionPoly** floorPoly,
                            f32* floorHeight) {
    s32 bgId;
    f32 waterY;
    WaterBox* waterBox;

    *floorPoly = NULL;
    *floorHeight = BgCheck_EntityRaycastFloor3(&play->colCtx, floorPoly, &bgId, pos);

    if (*floorHeight == BGCHECK_Y_MIN) {
        return 1; // No floor
    }

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, pos->x, pos->z, &waterY, &waterBox) == 1 &&
        *floorHeight < waterY) {
        return 2; // Water
    }

    if ((*floorPoly)->normal.y * COLPOLY_NORMAL_FRAC < 0.81915206f || // cos(35 degrees)
        SurfaceType_IsHorseBlocked(&play->colCtx, *floorPoly, bgId) ||
        func_80041D4C(&play->colCtx, *floorPoly, bgId) == 7) {
        return 3; // Horse blocked surface
    }
    return 0;
}

/**
 * obstacleType:
 *  1: Water in front
 *  2: Water behind?
 *  3: ?
 *  4: Obstructed in front
 *  5: Obstructed behind
 */
void EnHorse_ObstructMovement(EnHorse* this, PlayState* play, s32 obstacleType, s32 galloping) {
    if (this->action == ENHORSE_ACT_CS_UPDATE || EnHorse_BgCheckBridgeJumpPoint(this, play)) {
        return;
    }

    this->actor.world.pos = this->lastPos;
    this->actor.shape.rot.y = this->lastYaw;
    this->actor.world.rot.y = this->lastYaw;
    this->stateFlags |= ENHORSE_OBSTACLE;

    if (!this->playerControlled) {
        if (this->animationIdx != ENHORSE_ANIM_REARING) {
            return;
        }
    } else if (this->action != ENHORSE_ACT_MOUNTED_REARING) {
        if (this->stateFlags & ENHORSE_JUMPING) {
            this->stateFlags &= ~ENHORSE_JUMPING;
            this->actor.gravity = -3.5f;
            this->actor.world.pos.y = this->actor.floorHeight;
        }
        if (obstacleType == 1 || obstacleType == 4) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
        } else if (obstacleType == 2 || obstacleType == 5) {
            this->stateFlags |= ENHORSE_FORCE_WALKING;
        }
        if (galloping == true) {
            EnHorse_StartRearing(this);
        }
    }
}

void EnHorse_CheckFloors(EnHorse* this, PlayState* play) {
    s32 status;
    CollisionPoly* frontFloor;
    CollisionPoly* backFloor;
    s16 floorSlope;
    Vec3f frontPos;
    Vec3f backPos;
    Vec3f pos;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 galloping = this->actor.speedXZ > 8;
    f32 dist;
    f32 waterHeight;
    WaterBox* waterBox;
    s32 pad;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                &waterHeight, &waterBox) == 1 &&
        this->actor.floorHeight < waterHeight) {
        EnHorse_ObstructMovement(this, play, 1, galloping);
        return;
    }

    EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, 30.0f, 60.0f, &frontPos);
    status = EnHorse_CalcFloorHeight(this, play, &frontPos, &frontFloor, &this->yFront);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(this, play, 4, galloping);
        return;
    }
    if (status == 2) {
        EnHorse_ObstructMovement(this, play, 4, galloping);
        return;
    }
    if (status == 3) {
        EnHorse_ObstructMovement(this, play, 4, galloping);
        return;
    }

    EnHorse_Vec3fOffset(&this->actor.world.pos, this->actor.shape.rot.y, -30.0f, 60.0f, &backPos);
    status = EnHorse_CalcFloorHeight(this, play, &backPos, &backFloor, &this->yBack);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        EnHorse_ObstructMovement(this, play, 5, galloping);
        return;
    }
    if (status == 2) {
        EnHorse_ObstructMovement(this, play, 5, galloping);
        return;
    }
    if (status == 3) {
        EnHorse_ObstructMovement(this, play, 5, galloping);
        return;
    }

    floorSlope = Math_FAtan2F(this->yBack - this->yFront, 60.0f) * (0x8000 / M_PI);
    if (this->actor.floorPoly != 0) {
        nx = this->actor.floorPoly->normal.x * COLPOLY_NORMAL_FRAC;
        ny = this->actor.floorPoly->normal.y * COLPOLY_NORMAL_FRAC;
        nz = this->actor.floorPoly->normal.z * COLPOLY_NORMAL_FRAC;
        pos = frontPos;
        pos.y = this->yFront;
        dist = Math3D_DistPlaneToPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if ((frontFloor != this->actor.floorPoly) && (this->actor.speedXZ >= 0.0f)) {
            if ((!(this->stateFlags & ENHORSE_JUMPING) && dist < -40.0f) ||
                (this->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
                EnHorse_ObstructMovement(this, play, 4, galloping);
                return;
            }
        }

        pos = backPos;
        pos.y = this->yBack;
        dist = Math3D_DistPlaneToPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if (((backFloor != this->actor.floorPoly) && (this->actor.speedXZ <= 0.0f) &&
             !(this->stateFlags & ENHORSE_JUMPING) && (dist < -40.0f)) ||
            (this->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
            EnHorse_ObstructMovement(this, play, 5, galloping);
            return;
        }

        if (ny < 0.81915206f || // cos(35 degrees)
            SurfaceType_IsHorseBlocked(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ||
            func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == 7) {
            if ((this->actor.speedXZ >= 0.0f)) {
                EnHorse_ObstructMovement(this, play, 4, galloping);
            } else {
                EnHorse_ObstructMovement(this, play, 5, galloping);
            }
            return;
        }

        if (this->stateFlags & ENHORSE_JUMPING) {
            this->actor.shape.rot.x = 0;
            return;
        }

        if (this->actor.floorHeight + 4.0f < this->actor.world.pos.y) {
            this->actor.shape.rot.x = 0;
            return;
        }

        if (fabsf(floorSlope) > 8191.0f) {
            return;
        }

        this->actor.shape.rot.x = floorSlope;
        this->actor.shape.yOffset =
            (this->yFront + (((this->yBack - this->yFront) * 20.0f) / 45.0f)) - this->actor.floorHeight;
    }
}

s32 EnHorse_GetMountSide(EnHorse* this, PlayState* play);

void EnHorse_MountDismount(EnHorse* this, PlayState* play) {
    s32 pad[2];
    s32 mountSide;
    Player* player = GET_PLAYER(play);

    mountSide = EnHorse_GetMountSide(this, play);
    if (mountSide != 0 && !(this->stateFlags & ENHORSE_UNRIDEABLE) && player->rideActor == NULL) {
        Actor_SetRideActor(play, &this->actor, mountSide);
    }

    if (this->playerControlled == false && Actor_IsMounted(play, &this->actor) == true) {
        this->noInputTimer = 55;
        this->noInputTimerMax = 55;
        this->playerControlled = 1;
        EnHorse_Freeze(this);
    } else if (this->playerControlled == true && Actor_NotMounted(play, &this->actor) == true) {
        this->noInputTimer = 35;
        this->noInputTimerMax = 35;
        this->stateFlags &= ~ENHORSE_UNRIDEABLE;
        this->playerControlled = 0;
        EnHorse_Freeze(this);
    }
}

void EnHorse_StickDirection(Vec2f* curStick, f32* stickMag, s16* angle) {
    f32 dist;
    f32 y;
    f32 x;

    x = curStick->x;
    y = curStick->y;
    dist = sqrtf(SQ(x) + SQ(y));

    *stickMag = dist;
    if (dist > 60.0f) {
        *stickMag = 60.0f;
    } else {
        *stickMag = *stickMag;
    }

    *angle = Math_FAtan2F(-curStick->x, curStick->y) * (32768.0f / M_PI);
}

void EnHorse_UpdateStick(EnHorse* this, PlayState* play) {
    this->lastStick = this->curStick;
    this->curStick.x = play->state.input[0].rel.stick_x;
    this->curStick.y = play->state.input[0].rel.stick_y;
}

void EnHorse_ResolveCollision(EnHorse* this, PlayState* play, CollisionPoly* colPoly) {
    f32 dist;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 offset;

    nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
    ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
    nz = COLPOLY_GET_NORMAL(colPoly->normal.z);
    if (!(Math_CosS(this->actor.world.rot.y -
                    (s16)(Math_FAtan2F(colPoly->normal.x, colPoly->normal.z) * (0x8000 / M_PI)) - 0x7FFF) <
          0.7071f)) { // cos(45 degrees)
        dist = Math3D_DistPlaneToPos(nx, ny, nz, colPoly->dist, &this->actor.world.pos);
        offset = (1.0f / sqrtf(SQ(nx) + SQ(nz)));
        offset = (30.0f - dist) * offset;
        this->actor.world.pos.x += offset * nx;
        this->actor.world.pos.z += offset * nz;
    }
}

void EnHorse_BgCheckSlowMoving(EnHorse* this, PlayState* play) {
    f32 yOffset;
    Vec3f start;
    Vec3f end;
    Vec3f intersect;
    CollisionPoly* colPoly;
    s32 bgId;

    if (play->sceneNum == SCENE_SPOT20) {
        yOffset = 19.0f;
    } else {
        yOffset = 40.0f;
    }
    Math_Vec3f_Copy(&start, &this->actor.world.pos);
    start.y = start.y + yOffset;

    Math_Vec3f_Copy(&end, &start);
    end.x += 30.0f * Math_SinS(this->actor.world.rot.y);
    end.y += 30.0f * Math_SinS(-this->actor.shape.rot.x);
    end.z += 30.0f * Math_CosS(this->actor.world.rot.y);
    if (BgCheck_EntityLineTest1(&play->colCtx, &start, &end, &intersect, &colPoly, 1, 0, 0, 1, &bgId) != 0) {
        EnHorse_ResolveCollision(this, play, colPoly);
    }
}

void EnHorse_HighJumpInit(EnHorse* this, PlayState* play);
void EnHorse_Stub2(EnHorse* this);
void EnHorse_Stub1(EnHorse* this);

void EnHorse_UpdateBgCheckInfo(EnHorse* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f obstaclePos;
    f32 pad3;
    f32 intersectDist;
    CollisionPoly* wall = NULL;
    CollisionPoly* obstacleFloor = NULL;
    s32 bgId;
    f32 obstacleHeight;
    f32 behindObstacleHeight;
    f32 ny;
    s32 movingFast;
    s32 pad5;
    DynaPolyActor* dynaPoly;
    Vec3f intersect;
    Vec3f obstacleTop;

    Actor_UpdateBgCheckInfo(play, &this->actor, play->sceneNum == SCENE_SPOT20 ? 19.0f : 40.0f, 35.0f, 100.0f,
                            29);

    if (EnHorse_BgCheckBridgeJumpPoint(this, play)) {
        return;
    }

    // void 0 trick required to match, but is surely not real. revisit at a later time
    if (this->actor.bgCheckFlags & 8 && Math_CosS(this->actor.wallYaw - ((void)0, this->actor.world).rot.y) < -0.3f) {
        if (this->actor.speedXZ > 4.0f) {
            this->actor.speedXZ -= 1.0f;
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        }
    }

    if (this->stateFlags & ENHORSE_JUMPING || !this->playerControlled) {
        return;
    }

    if (this->actor.speedXZ < 0.0f) {
        return;
    }

    // Braking or rearing from obstacle
    if (this->action == ENHORSE_ACT_STOPPING || this->action == ENHORSE_ACT_MOUNTED_REARING) {
        return;
    }

    if (this->actor.speedXZ > 8.0f) {
        if (this->actor.speedXZ < 12.8f) {
            intersectDist = 160.0f;
            movingFast = 0;
        } else {
            intersectDist = 230.0f;
            movingFast = 1;
        }
    } else {
        EnHorse_BgCheckSlowMoving(this, play);
        return;
    }

    startPos = this->actor.world.pos;
    startPos.y += 19.0f;
    endPos = startPos;
    endPos.x += (intersectDist * Math_SinS(this->actor.world.rot.y));
    endPos.y += (intersectDist * Math_SinS(-this->actor.shape.rot.x));
    endPos.z += (intersectDist * Math_CosS(this->actor.world.rot.y));
    intersect = endPos;
    wall = NULL;
    if (BgCheck_EntityLineTest1(&play->colCtx, &startPos, &endPos, &intersect, &wall, 1, 0, 0, 1, &bgId) == 1) {
        intersectDist = sqrt(Math3D_Vec3fDistSq(&startPos, &intersect));
        this->stateFlags |= ENHORSE_OBSTACLE;
    }

    if (wall != NULL) {
        if (intersectDist < 30.0f) {
            EnHorse_ResolveCollision(this, play, wall);
        }
        if ((Math_CosS(this->actor.world.rot.y - (s16)(Math_FAtan2F(wall->normal.x, wall->normal.z) * (0x8000 / M_PI)) -
                       0x7FFF) < 0.5f) ||
            SurfaceType_IsHorseBlocked(&play->colCtx, wall, bgId) != 0) {
            return;
        }

        // too close to jump
        if ((movingFast == false && intersectDist < 80.0f) || (movingFast == true && intersectDist < 150.0f)) {
            if (movingFast == false) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
                EnHorse_StartBraking(this, play);
            }
            return;
        }

        dynaPoly = DynaPoly_GetActor(&play->colCtx, bgId);
        if ((this->stateFlags & ENHORSE_FLAG_26) && ((dynaPoly && dynaPoly->actor.id != 0x108) || dynaPoly == 0)) {
            if (movingFast == false) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
                EnHorse_StartBraking(this, play);
            }
            return;
        }
    }

    // Get obstacle's height
    intersectDist += 5.0f;
    obstaclePos = startPos;
    obstaclePos.x += intersectDist * Math_SinS(this->actor.world.rot.y);
    obstaclePos.y = this->actor.world.pos.y + 120.0f;
    obstaclePos.z += intersectDist * Math_CosS(this->actor.world.rot.y);
    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }
    obstacleHeight = obstacleTop.y - this->actor.world.pos.y;
    if (this->actor.floorPoly == NULL || obstacleFloor == NULL) {
        return;
    }

    if (Math3D_DistPlaneToPos(this->actor.floorPoly->normal.x * COLPOLY_NORMAL_FRAC,
                              this->actor.floorPoly->normal.y * COLPOLY_NORMAL_FRAC,
                              this->actor.floorPoly->normal.z * COLPOLY_NORMAL_FRAC, this->actor.floorPoly->dist,
                              &obstacleTop) < -40.0f &&
        Math3D_DistPlaneToPos(
            obstacleFloor->normal.x * COLPOLY_NORMAL_FRAC, obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC,
            obstacleFloor->normal.z * COLPOLY_NORMAL_FRAC, obstacleFloor->dist, &this->actor.world.pos) > 40.0f) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
        this->stateFlags |= ENHORSE_OBSTACLE;
        return;
    }

    ny = obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC;
    if (ny < 0.81915206f || // cos(35 degrees)
        (SurfaceType_IsHorseBlocked(&play->colCtx, obstacleFloor, bgId) != 0) ||
        (func_80041D4C(&play->colCtx, obstacleFloor, bgId) == 7)) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
        return;
    }

    if (wall == NULL || obstacleTop.y < intersect.y || (this->stateFlags & ENHORSE_CANT_JUMP)) {
        return;
    }

    obstaclePos = startPos;
    obstaclePos.y = this->actor.world.pos.y + 120.0f;
    if (movingFast == false) {
        obstaclePos.x += (276.0f * Math_SinS(this->actor.world.rot.y));
        obstaclePos.z += (276.0f * Math_CosS(this->actor.world.rot.y));
    } else {
        obstaclePos.x += (390.0f * Math_SinS(this->actor.world.rot.y));
        obstaclePos.z += (390.0f * Math_CosS(this->actor.world.rot.y));
    }

    obstacleTop = obstaclePos;
    obstacleTop.y = BgCheck_EntityRaycastFloor3(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }

    behindObstacleHeight = obstacleTop.y - this->actor.world.pos.y;

    if (obstacleFloor == NULL) {
        return;
    }

    ny = obstacleFloor->normal.y * COLPOLY_NORMAL_FRAC;
    if (ny < 0.81915206f || // cos(35 degrees)
        SurfaceType_IsHorseBlocked(&play->colCtx, obstacleFloor, bgId) ||
        func_80041D4C(&play->colCtx, obstacleFloor, bgId) == 7) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
    } else if (behindObstacleHeight < -DREG(4)) { // -70
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            EnHorse_StartBraking(this, play);
        }
    } else if (movingFast == false && obstacleHeight > 19.0f && obstacleHeight <= 40.0f) {
        EnHorse_Stub1(this);
        this->postDrawFunc = EnHorse_LowJumpInit;
    } else if ((movingFast == true && this->actor.speedXZ < 13.8f && obstacleHeight > 19.0f &&
                obstacleHeight <= 72.0f) ||
               (this->actor.speedXZ > 13.8f && obstacleHeight <= 112.0f)) {

        EnHorse_Stub2(this);
        this->postDrawFunc = EnHorse_HighJumpInit;
    }
}

void EnHorse_CheckBoost(EnHorse* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;
    s32 pad;

    if (this->action == ENHORSE_ACT_MOUNTED_WALK || this->action == ENHORSE_ACT_MOUNTED_TROT ||
        this->action == ENHORSE_ACT_MOUNTED_GALLOP) {
        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) && (play->interfaceCtx.unk_1EE == 8)) {
            if (!(this->stateFlags & ENHORSE_BOOST) && !(this->stateFlags & ENHORSE_FLAG_8) &&
                !(this->stateFlags & ENHORSE_FLAG_9)) {
                if (this->numBoosts > 0) {
                    func_800AA000(0.0f, 180, 20, 100);
                    this->stateFlags |= ENHORSE_BOOST;
                    this->stateFlags |= ENHORSE_FIRST_BOOST_REGEN;
                    this->stateFlags |= ENHORSE_FLAG_8;
                    if (!CVar_GetS32("gInfiniteEpona", 0)) { this->numBoosts--; }
                    this->boostTimer = 0;
                    if (this->numBoosts == 0) {
                        this->boostRegenTime = 140;
                        return;
                    }
                    if (this->type == HORSE_EPONA) {
                        if (this->stateFlags & ENHORSE_FIRST_BOOST_REGEN) {
                            this->boostRegenTime = 60;
                            this->stateFlags &= ~ENHORSE_FIRST_BOOST_REGEN;
                        } else {
                            this->boostRegenTime = 8; // Never happens
                        }
                    } else {
                        this->boostRegenTime = 70;
                    }
                    return;
                }
                this->unk_21C = this->unk_228;
                if (this->stateFlags & ENHORSE_DRAW) {
                    if (Rand_ZeroOne() < 0.1f) {
                        Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                    }
                }
            }
        }
    }
}

void EnHorse_RegenBoost(EnHorse* this, PlayState* play) {
    if (this->numBoosts < 6 && this->numBoosts > 0) {
        this->boostRegenTime--;
        this->boostTimer++;

        if (this->boostRegenTime <= 0) {
            this->numBoosts = this->numBoosts + 1;

            if (!EN_HORSE_CHECK_4(this)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CARROT_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }

            if (this->numBoosts < 6) {
                this->boostRegenTime = 11;
            }
        }
    } else if (this->numBoosts == 0) {
        this->boostRegenTime--;
        this->boostTimer++;

        if (this->boostRegenTime <= 0) {
            this->boostRegenTime = 0;
            this->numBoosts = 6;

            if (!EN_HORSE_CHECK_4(this)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CARROT_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    }

    if (this->boostTimer == 8 && Rand_ZeroOne() < 0.25f) {
        this->unk_21C = this->unk_228;
        if (this->stateFlags & ENHORSE_DRAW) {
            Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }
    play->interfaceCtx.numHorseBoosts = this->numBoosts;
}

void EnHorse_UpdatePlayerDir(EnHorse* this, PlayState* play) {
    EnHorse* pad;
    s16 angle;
    f32 s;
    f32 c;

    angle = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
    s = Math_SinS(angle);
    c = Math_CosS(angle);
    if (s > 0.8660254f) { // sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_L;
    } else if (s < -0.8660254f) { // -sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_R;
    } else {
        if (c > 0.0f) {
            if (s > 0) {
                this->playerDir = PLAYER_DIR_FRONT_L;
            } else {
                this->playerDir = PLAYER_DIR_FRONT_R;
            }
        } else {
            if (s > 0) {
                this->playerDir = PLAYER_DIR_BACK_L;
            } else {
                this->playerDir = PLAYER_DIR_BACK_R;
            }
        }
    }
}

void EnHorse_TiltBody(EnHorse* this, PlayState* play) {
    f32 speed;
    f32 rollDiff;
    s32 targetRoll;
    s16 turnVel;

    speed = this->actor.speedXZ / this->boostSpeed;
    turnVel = this->actor.shape.rot.y - this->lastYaw;
    targetRoll = -((s16)((1820.0f * speed) * (turnVel / 480.00003f)));
    rollDiff = targetRoll - this->actor.world.rot.z;

    if (fabsf(targetRoll) < 100.0f) {
        this->actor.world.rot.z = 0;
    } else if (fabsf(rollDiff) < 100.0f) {
        this->actor.world.rot.z = targetRoll;
    } else if (rollDiff > 0.0f) {
        this->actor.world.rot.z += 100;
    } else {
        this->actor.world.rot.z -= 100;
    }

    this->actor.shape.rot.z = this->actor.world.rot.z;
}

s32 EnHorse_UpdateConveyors(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 conveyorDir;

    if ((this->actor.floorPoly == NULL) && (this != (EnHorse*)player->rideActor)) {
        return 0;
    }
    conveyorDir = SurfaceType_GetConveyorDirection(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    conveyorDir = (conveyorDir << 10) - this->actor.world.rot.y;
    if (conveyorDir > 800.0f) {
        this->actor.world.rot.y += 800.0f;
    } else if (conveyorDir < -800.0f) {
        this->actor.world.rot.y -= 800.0f;
    } else {
        this->actor.world.rot.y += conveyorDir;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    return 1;
}

s32 EnHorse_RandInt(f32 range) {
    return Rand_ZeroOne() * range;
}

void EnHorse_Update(Actor* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;
    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
    Vec3f dustVel = { 0.0f, 1.0f, 0.0f };
    Player* player = GET_PLAYER(play);

    this->lastYaw = thisx->shape.rot.y;
    EnHorse_UpdateStick(this, play);
    EnHorse_UpdatePlayerDir(this, play);

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        EnHorse_MountDismount(this, play);
    }

    if (this->stateFlags & ENHORSE_FLAG_19) {
        if (this->stateFlags & ENHORSE_FLAG_20 && this->inRace == true) {
            this->stateFlags &= ~ENHORSE_FLAG_20;
            EnHorse_StartRearing(this);
        } else if (!(this->stateFlags & ENHORSE_FLAG_20) && this->stateFlags & ENHORSE_FLAG_21 &&
                   this->action != ENHORSE_ACT_MOUNTED_REARING && this->inRace == true) {
            this->stateFlags &= ~ENHORSE_FLAG_21;
            EnHorse_StartRearing(this);
        }
    }

    sActionFuncs[this->action](this, play);
    this->stateFlags &= ~ENHORSE_OBSTACLE;
    this->curFrame = this->skin.skelAnime.curFrame;
    this->lastPos = thisx->world.pos;
    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        if (this->action == ENHORSE_ACT_MOUNTED_GALLOP || this->action == ENHORSE_ACT_MOUNTED_TROT ||
            this->action == ENHORSE_ACT_MOUNTED_WALK) {
            EnHorse_CheckBoost(this, play);
        }
        if (this->playerControlled == true) {
            EnHorse_RegenBoost(this, play);
        }
        Actor_MoveForward(thisx);
        if (this->action == ENHORSE_ACT_INGO_RACE) {
            if (this->rider != NULL) {
                this->rider->world.pos.x = thisx->world.pos.x;
                this->rider->world.pos.y = thisx->world.pos.y + 10.0f;
                this->rider->world.pos.z = thisx->world.pos.z;
                this->rider->shape.rot.x = thisx->shape.rot.x;
                this->rider->shape.rot.y = thisx->shape.rot.y;
            }
        }
        if (this->jntSph.elements[0].info.ocElemFlags & 2) {
            if (thisx->speedXZ > 6.0f) {
                thisx->speedXZ -= 1.0f;
            }
        }
        if (this->jntSph.base.acFlags & 2) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Audio_PlaySoundGeneral(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
        if (this->action != ENHORSE_ACT_INGO_RACE) {
            EnHorse_TiltBody(this, play);
        }
        Collider_UpdateCylinder(thisx, &this->cyl1);
        Collider_UpdateCylinder(thisx, &this->cyl2);

        // Required to match
        this->cyl1.dim.pos.x = this->cyl1.dim.pos.x + (s16)(Math_SinS(thisx->shape.rot.y) * 11.0f);
        this->cyl1.dim.pos.z = this->cyl1.dim.pos.z + (s16)(Math_CosS(thisx->shape.rot.y) * 11.0f);
        this->cyl2.dim.pos.x = this->cyl2.dim.pos.x + (s16)(Math_SinS(thisx->shape.rot.y) * -18.0f);
        this->cyl2.dim.pos.z = this->cyl2.dim.pos.z + (s16)(Math_CosS(thisx->shape.rot.y) * -18.0f);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->cyl1.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->cyl1.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->cyl2.base);
        if ((player->stateFlags1 & 1) && player->rideActor != NULL) {
            if (play->sceneNum != SCENE_SPOT20 ||
                (play->sceneNum == SCENE_SPOT20 && (thisx->world.pos.z < -2400.0f))) {
                EnHorse_UpdateConveyors(this, play);
            }
        }
        if (!(this->stateFlags & ENHORSE_FLAG_24)) {
            EnHorse_UpdateBgCheckInfo(this, play);
            EnHorse_CheckFloors(this, play);
            if (thisx->world.pos.y < this->yFront && thisx->world.pos.y < this->yBack) {
                if (this->yBack < this->yFront) {
                    thisx->world.pos.y = this->yBack;
                } else {
                    thisx->world.pos.y = this->yFront;
                }
            }

        } else {
            this->stateFlags &= ~ENHORSE_FLAG_24;
        }

        if (play->sceneNum == SCENE_SPOT09 && (gSaveContext.eventChkInf[9] & 0xF) != 0xF) {
            EnHorse_CheckBridgeJumps(this, play);
        }

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 70.0f;
        if ((Rand_ZeroOne() < 0.025f) && this->blinkTimer == 0) {
            this->blinkTimer++;
        } else if (this->blinkTimer > 0) {
            this->blinkTimer++;
            if (this->blinkTimer >= 4) {
                this->blinkTimer = 0;
            }
        }

        if (thisx->speedXZ == 0.0f && !(this->stateFlags & ENHORSE_FLAG_19)) {
            thisx->colChkInfo.mass = 0xFF;
        } else {
            thisx->colChkInfo.mass = 0xFE;
        }

        if (thisx->speedXZ >= 5.0f) {
            this->cyl1.base.atFlags |= 1;
        } else {
            this->cyl1.base.atFlags &= ~1;
        }

        if (gSaveContext.entranceIndex != 343 || gSaveContext.sceneSetupIndex != 9) {
            if (this->dustFlags & 1) {
                this->dustFlags &= ~1;
                func_800287AC(play, &this->frontRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (this->dustFlags & 2) {
                this->dustFlags &= ~2;
                func_800287AC(play, &this->frontLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (this->dustFlags & 4) {
                this->dustFlags &= ~4;
                func_800287AC(play, &this->backRightHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            } else if (this->dustFlags & 8) {
                this->dustFlags &= ~8;
                func_800287AC(play, &this->backLeftHoof, &dustVel, &dustAcc, EnHorse_RandInt(100) + 200,
                              EnHorse_RandInt(10) + 30, EnHorse_RandInt(20) + 30);
            }
        }
        this->stateFlags &= ~ENHORSE_DRAW;
    }
}

s32 EnHorse_PlayerDirToMountSide(EnHorse* this, PlayState* play, Player* player) {
    if (this->playerDir == PLAYER_DIR_SIDE_L) {
        return -1;
    }
    if (this->playerDir == PLAYER_DIR_SIDE_R) {
        return 1;
    }
    return 0;
}

s32 EnHorse_MountSideCheck(EnHorse* this, PlayState* play, Player* player) {
    s32 mountSide;

    if (Actor_WorldDistXZToActor(&this->actor, &player->actor) > 75.0f) {
        return 0;
    } else if (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) > 30.0f) {
        return 0;
    } else if (Math_CosS(Actor_WorldYawTowardActor(&player->actor, &this->actor) - player->actor.world.rot.y) <
               0.17364818f) { // cos(80 degrees)
        return 0;
    } else {
        mountSide = EnHorse_PlayerDirToMountSide(this, play, player);
        if (mountSide == -1) {
            return -1;
        }
        if (mountSide == 1) {
            return 1;
        }
    }
    return 0;
}

s32 EnHorse_GetMountSide(EnHorse* this, PlayState* play) {
    if (this->action != ENHORSE_ACT_IDLE) {
        return 0;
    }
    if ((this->animationIdx != ENHORSE_ANIM_IDLE) && (this->animationIdx != ENHORSE_ANIM_WHINNEY)) {
        return 0;
    }
    return EnHorse_MountSideCheck(this, play, GET_PLAYER(play));
}

void EnHorse_RandomOffset(Vec3f* src, f32 dist, Vec3f* dst) {
    dst->x = (Rand_ZeroOne() * (dist * 2.0f) + src->x) - dist;
    dst->y = (Rand_ZeroOne() * (dist * 2.0f) + src->y) - dist;
    dst->z = (Rand_ZeroOne() * (dist * 2.0f) + src->z) - dist;
}

void EnHorse_PostDraw(Actor* thisx, PlayState* play, Skin* skin) {
    EnHorse* this = (EnHorse*)thisx;
    s32 pad;
    Vec3f sp94 = { 0.0f, 0.0f, 0.0f };
    Vec3f hoofOffset = { 5.0f, -4.0f, 5.0f };
    Vec3f riderOffset = { 600.0f, -1670.0f, 0.0f };
    Vec3f sp70;
    Vec3f sp64 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp58 = { 0.0f, -1.0f, 0.0f };

    f32 frame = this->skin.skelAnime.curFrame;
    Vec3f center;
    Vec3f newCenter;
    s32 i;
    Vec3f sp2C;
    f32 sp28;

    if (!(this->stateFlags & ENHORSE_CALC_RIDER_POS)) {
        Skin_GetLimbPos(skin, 30, &riderOffset, &this->riderPos);
        this->riderPos.x = this->riderPos.x - this->actor.world.pos.x;
        this->riderPos.y = this->riderPos.y - this->actor.world.pos.y;
        this->riderPos.z = this->riderPos.z - this->actor.world.pos.z;
    } else {
        this->stateFlags &= ~ENHORSE_CALC_RIDER_POS;
    }

    Skin_GetLimbPos(skin, 13, &sp94, &sp2C);
    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sp2C, &this->unk_228, &sp28);
    if ((this->animationIdx == ENHORSE_ANIM_IDLE && this->action != ENHORSE_ACT_FROZEN) &&
        ((frame > 40.0f && frame < 45.0f && this->type == HORSE_EPONA) ||
         (frame > 28.0f && frame < 33.0f && this->type == HORSE_HNI))) {
        if (Rand_ZeroOne() < 0.6f) {
            this->dustFlags |= 1;
            Skin_GetLimbPos(skin, 28, &hoofOffset, &this->frontRightHoof);
            this->frontRightHoof.y = this->frontRightHoof.y - 5.0f;
        }
    } else {
        if (this->action == ENHORSE_ACT_STOPPING) {
            if ((frame > 10.0f && frame < 13.0f) || (frame > 25.0f && frame < 33.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    this->dustFlags |= 2;
                    Skin_GetLimbPos(skin, 20, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &this->frontLeftHoof);
                }
                if (Rand_ZeroOne() < 0.7f) {
                    this->dustFlags |= 1;
                    Skin_GetLimbPos(skin, 28, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &this->frontRightHoof);
                }
            }

            if ((frame > 6.0f && frame < 10.0f) || (frame > 23.0f && frame < 29.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    this->dustFlags |= 8;
                    Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &this->backLeftHoof);
                }
            }

            if ((frame > 7.0f && frame < 14.0f) || (frame > 26.0f && frame < 30.0f)) {
                if (Rand_ZeroOne() < 0.7f) {
                    this->dustFlags |= 4;
                    Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                    EnHorse_RandomOffset(&sp70, 10.0f, &this->backRightHoof);
                }
            }
        } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
            if ((frame > 14.0f) && (frame < 16.0f)) {
                this->dustFlags |= 1;
                Skin_GetLimbPos(skin, 28, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 5.0f, &this->frontRightHoof);
            } else if (frame > 8.0f && frame < 10.0f) {
                this->dustFlags |= 2;
                Skin_GetLimbPos(skin, 20, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->frontLeftHoof);
            } else if (frame > 1.0f && frame < 3.0f) {
                this->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backRightHoof);
            } else if ((frame > 26.0f) && (frame < 28.0f)) {
                this->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backLeftHoof);
            }
        } else if (this->action == ENHORSE_ACT_LOW_JUMP && frame > 6.0f &&
                   Rand_ZeroOne() < 1.0f - (frame - 6.0f) * (1.0f / 17.0f)) {
            if (Rand_ZeroOne() < 0.5f) {
                this->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backLeftHoof);
            }
            if (Rand_ZeroOne() < 0.5f) {
                this->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backRightHoof);
            }
        } else if (this->action == ENHORSE_ACT_HIGH_JUMP && frame > 5.0f &&
                   Rand_ZeroOne() < 1.0f - (frame - 5.0f) * (1.0f / 25.0f)) {
            if (Rand_ZeroOne() < 0.5f) {
                this->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backLeftHoof);
            }
            if (Rand_ZeroOne() < 0.5f) {
                this->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backRightHoof);
            }
        } else if (this->action == ENHORSE_ACT_BRIDGE_JUMP && Rand_ZeroOne() < 0.5f) {
            if (Rand_ZeroOne() < 0.5f) {
                this->dustFlags |= 8;
                Skin_GetLimbPos(skin, 37, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backLeftHoof);
            } else {
                this->dustFlags |= 4;
                Skin_GetLimbPos(skin, 45, &hoofOffset, &sp70);
                EnHorse_RandomOffset(&sp70, 10.0f, &this->backRightHoof);
            }
        }
    }

    for (i = 0; i < this->jntSph.count; i++) {
        center.x = this->jntSph.elements[i].dim.modelSphere.center.x;
        center.y = this->jntSph.elements[i].dim.modelSphere.center.y;
        center.z = this->jntSph.elements[i].dim.modelSphere.center.z;

        Skin_GetLimbPos(skin, this->jntSph.elements[i].dim.limb, &center, &newCenter);
        this->jntSph.elements[i].dim.worldSphere.center.x = newCenter.x;
        this->jntSph.elements[i].dim.worldSphere.center.y = newCenter.y;
        this->jntSph.elements[i].dim.worldSphere.center.z = newCenter.z;
        this->jntSph.elements[i].dim.worldSphere.radius =
            this->jntSph.elements[i].dim.modelSphere.radius * this->jntSph.elements[i].dim.scale;
    }

    //! @bug Setting colliders in a draw function allows for duplicate entries to be added to their respective lists
    //! under certain conditions, like when pausing and unpausing the game.
    //! Actors will draw for a couple of frames between the pauses, but some important logic updates will not occur.
    //! In the case of OC, this can cause unwanted effects such as a very large amount of displacement being applied to
    //! a colliding actor.
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->jntSph.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->jntSph.base);
}

// unused
static s32 D_80A667DC[] = { 0, 3, 7, 14 };

s32 EnHorse_OverrideLimbDraw(Actor* thisx, PlayState* play, s32 limbIndex, Skin* arg3) {
    static void* eyeTextures[] = {
        gEponaEyeOpenTex,
        gEponaEyeHalfTex,
        gEponaEyeClosedTex,
    };
    static u8 eyeBlinkIndexes[] = { 0, 1, 2, 1 };
    EnHorse* this = (EnHorse*)thisx;
    s32 drawOriginalLimb = true;

    OPEN_DISPS(play->state.gfxCtx);
    if (limbIndex == 13 && this->type == HORSE_EPONA) {
        u8 index = eyeBlinkIndexes[this->blinkTimer];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[index]));
    } else if (this->type == HORSE_HNI && this->stateFlags & ENHORSE_FLAG_18 && limbIndex == 30) {
        Skin_DrawLimb(play->state.gfxCtx, &this->skin, limbIndex, gHorseIngoGerudoSaddleDL, 0);
        drawOriginalLimb = false;
    }
    CLOSE_DISPS(play->state.gfxCtx);
    return drawOriginalLimb;
}

void EnHorse_Draw(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        this->stateFlags |= ENHORSE_DRAW;
        if (this->stateFlags & ENHORSE_JUMPING) {
            func_800A6360(thisx, play, &this->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, false);
        } else {
            func_800A6360(thisx, play, &this->skin, EnHorse_PostDraw, EnHorse_OverrideLimbDraw, true);
        }
        if (this->postDrawFunc != NULL) {
            this->postDrawFunc(this, play);
        }
    }
}

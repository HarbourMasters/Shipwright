/*
 * File: z_en_bigslime.c
 * Overlay: ovl_En_Bigslime
 * Description: Mad Jelly & Gekko Miniboss: Fused Jellies & Gekko
 */

#include "z_en_bigslime.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/object_bigslime/object_bigslime.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "BenPort.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnBigslime_Init(Actor* thisx, PlayState* play2);
void EnBigslime_Destroy(Actor* thisx, PlayState* play);
void EnBigslime_UpdateGekko(Actor* thisx, PlayState* play);
void EnBigslime_DrawGekko(Actor* thisx, PlayState* play);

void EnBigslime_EndCutscene(EnBigslime* this, PlayState* play);
void EnBigslime_CutsceneStartBattle(EnBigslime* this, PlayState* play);
void EnBigslime_SetupCutsceneNoticePlayer(EnBigslime* this);
void EnBigslime_CutsceneNoticePlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetupCallMinislime(EnBigslime* this, PlayState* play);
void EnBigslime_CallMinislime(EnBigslime* this, PlayState* play);
void EnBigslime_MoveOnCeiling(EnBigslime* this, PlayState* play);
void EnBigslime_SetupDrop(EnBigslime* this);
void EnBigslime_Drop(EnBigslime* this, PlayState* play);
void EnBigslime_SetTargetVtxToWideCone(EnBigslime* this);
void EnBigslime_SquishFlat(EnBigslime* this, PlayState* play);
void EnBigslime_SetupSquishFlat(EnBigslime* this);
void EnBigslime_SetTargetVtxToThinCone(EnBigslime* this);
void EnBigslime_SetTargetVtxToInverseCone(EnBigslime* this);
void EnBigslime_SetTargetVtxToStaticVtx(EnBigslime* this);
void EnBigslime_SetupRise(EnBigslime* this);
void EnBigslime_Rise(EnBigslime* this, PlayState* play);
void EnBigslime_CutsceneGrabPlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetupAttackPlayerInBigslime(EnBigslime* this);
void EnBigslime_AttackPlayerInBigslime(EnBigslime* this, PlayState* play);
void EnBigslime_SetupWindupThrowPlayer(EnBigslime* this);
void EnBigslime_WindupThrowPlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetupSetDynamicVtxThrowPlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetDynamicVtxThrowPlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetupFrozenGround(EnBigslime* this);
void EnBigslime_FrozenGround(EnBigslime* this, PlayState* play);
void EnBigslime_Freeze(EnBigslime* this, PlayState* play);
void EnBigslime_Melt(EnBigslime* this, PlayState* play);
void EnBigslime_SetupFrozenFall(EnBigslime* this);
void EnBigslime_FrozenFall(EnBigslime* this, PlayState* play);
void EnBigslime_SetupJumpGekko(EnBigslime* this);
void EnBigslime_JumpGekko(EnBigslime* this, PlayState* play);
void EnBigslime_SetupIdleLookAround(EnBigslime* this);
void EnBigslime_IdleLookAround(EnBigslime* this, PlayState* play);
void EnBigslime_SetupIdleNoticePlayer(EnBigslime* this);
void EnBigslime_IdleNoticePlayer(EnBigslime* this, PlayState* play);
void EnBigslime_SetupThrowMinislime(EnBigslime* this);
void EnBigslime_DamageGekko(EnBigslime* this, PlayState* play);
void EnBigslime_StunGekko(EnBigslime* this, PlayState* play);
void EnBigslime_CutsceneFormBigslime(EnBigslime* this, PlayState* play);
void EnBigslime_SetupFormBigslime(EnBigslime* this);
void EnBigslime_FormBigslime(EnBigslime* this, PlayState* play);
void EnBigslime_CutsceneDefeat(EnBigslime* this, PlayState* play);
void EnBigslime_SetupGekkoDespawn(EnBigslime* this, PlayState* play);
void EnBigslime_GekkoDespawn(EnBigslime* this, PlayState* play);
void EnBigslime_SetupFrogSpawn(EnBigslime* this, PlayState* play);
void EnBigslime_FrogSpawn(EnBigslime* this, PlayState* play);
void EnBigslime_SetupDespawn(EnBigslime* this);
void EnBigslime_Despawn(EnBigslime* this, PlayState* play);
void EnBigslime_SetupInitEntrance(EnBigslime* this);
void EnBigslime_InitEntrance(EnBigslime* this, PlayState* play);
void EnBigslime_ThrowMinislime(EnBigslime* this, PlayState* play);
void EnBigslime_SetupCutscene(EnBigslime* this);
void EnBigslime_PlayCutscene(EnBigslime* this, PlayState* play);
void EnBigslime_AddIceShardEffect(EnBigslime* this, PlayState* play);
void EnBigslime_UpdateBigslime(Actor* thisx, PlayState* play);
void EnBigslime_DrawBigslime(Actor* thisx, PlayState* play);
void EnBigslime_DrawShatteringEffects(EnBigslime* this, PlayState* play);

/*
 * Bigslime Spherical Vtx Data:
 *     - The vtx data for Bigslime is put directly into the overlay
 *     - This vtx is directly manipulated in a significant number of functions in this overlay
 *     - The initialized vtx data has the following properties: (some of this data may change as overlay functions are
 * called)
 *           -- It is a sphere with radius 1000
 *           -- There are 162 vertices numbered top to bottom along the y-axis,
 *              with vertex 0 being the top apex, and vertex 161 being the bottom apex
 *           -- There are 4 entire copies of the sphere
 *              --- The first 3 copies are identical
 *              --- All 4 copies have identical coordinates and number of vertices
 *              --- See below (variants) for the uses of these copies
 *
 * Vtx Rings:
 *     - The vtx mesh was designed to be separated into distint "rings" of vertices and faces
 *       from top to bottom at discrete y-intervals
 *     - These different rings of vertices lie in the xzPlane along discrete y-direction steps of the sphere
 *          -- There are the single apex nodes at the top (vertex 0 y-coordiante 1000) and bottom (vertex 161
 * y-coordiante -1000)
 *          -- There are a cluster of vertices (vertex 1-5) which all have y-coordinate 962
 *             and form a ring of vertices in the xz-plane
 *          -- There are a cluster of vertices (vertex 6-15) which all have y-coordinate around 870
 *             and form a ring of vertices in the xz-plane
 *          -- So on and so forth (See sVtxRingStartIndex)
 *          -- BIGSLIME_NUM_RING_VTX: There are a total of 13 rings of vertices including the two single apex vertex
 *          -- BIGSLIME_NUM_RING_FACES: There are a total of 12 rings of faces between the rings of vertices
 *     - A collider is assigned to each of the 12 rings of faces for bigslime
 *
 * Three Variants of the Spherical Vtx:
 *      Static (1 copy):
 *          - Fixed mesh, no values are ever modified
 *          - Used as reference to store the original vertices
 *          - Never drawn, only used in calculations
 *      Dynamic (2 copies):
 *          - Coordinate are updated and deformed, mesh is drawn as the deforming Fused Jelly.
 *          - Initially Identical to Static.
 *          - Two copies are used that alternate every frame and copies over coordinates/normals immediately.
 *            One copy is the version that is drawn, and the other copy is updated in the background
 *          - Alpha values remain fixed
 *      Target (1 copy):
 *          - Used to define the target vertices/shape for the dynamic vertices to smoothly step to.
 *          - When frozen:
 *              -- The current target vertices are set to the current dynamic vertices
 *              -- These are the vertices that are used to draw the frozen Fused Jelly
 *              -- The alpha values are used for the frozen effects alpha
 */

// Reference data: used to store the original vertices

#include "overlays/ovl_En_Bigslime/ovl_En_Bigslime.h"

static Vtx* sBigslimeStaticVtxData;
// Dynamic data: used to draw the real shape and has 2 states
static Vtx* sBigslimeDynamicVtxData[2];

// Target data: used to define the shape the dynamic vertices morph to
static Vtx* sBigslimeTargetVtxData;

/*
 * Triangle face
 * Connectivity matrix for mesh above. Covers every triangle in the sphere
 * 320 triangles x 3 vertices per triangle
 * Purpose: indices to calculate/update normal vector for lighting calculations
 * "EnBigslimeTri" struct based on "Tri" struct from gbi.h, but without the flag
 */
static EnBigslimeTri sBigslimeTri[BIGSLIME_NUM_FACES] = {
    { 0, 2, 1 },       { 1, 7, 6 },       { 1, 2, 7 },       { 2, 8, 7 },       { 6, 17, 16 },     { 6, 7, 17 },
    { 8, 19, 18 },     { 7, 8, 18 },      { 7, 18, 17 },     { 9, 21, 20 },     { 0, 3, 2 },       { 2, 9, 8 },
    { 2, 3, 9 },       { 3, 10, 9 },      { 8, 20, 19 },     { 8, 9, 20 },      { 10, 22, 21 },    { 9, 10, 21 },
    { 0, 4, 3 },       { 3, 11, 10 },     { 3, 4, 11 },      { 4, 12, 11 },     { 10, 23, 22 },    { 10, 11, 23 },
    { 12, 25, 24 },    { 11, 12, 24 },    { 11, 24, 23 },    { 13, 27, 26 },    { 0, 5, 4 },       { 4, 13, 12 },
    { 4, 5, 13 },      { 5, 14, 13 },     { 12, 26, 25 },    { 12, 13, 26 },    { 14, 28, 27 },    { 13, 14, 27 },
    { 0, 1, 5 },       { 5, 15, 14 },     { 5, 1, 15 },      { 1, 6, 15 },      { 14, 29, 28 },    { 14, 15, 29 },
    { 6, 16, 30 },     { 15, 6, 30 },     { 15, 30, 29 },    { 16, 32, 31 },    { 16, 17, 32 },    { 17, 33, 32 },
    { 17, 18, 33 },    { 18, 34, 33 },    { 18, 19, 34 },    { 19, 35, 34 },    { 19, 20, 36 },    { 20, 37, 36 },
    { 20, 21, 37 },    { 21, 38, 37 },    { 21, 22, 38 },    { 22, 39, 38 },    { 19, 36, 35 },    { 22, 40, 39 },
    { 22, 23, 40 },    { 23, 41, 40 },    { 23, 24, 41 },    { 24, 42, 41 },    { 24, 25, 42 },    { 25, 43, 42 },
    { 25, 26, 44 },    { 26, 45, 44 },    { 26, 27, 45 },    { 27, 46, 45 },    { 27, 28, 46 },    { 28, 47, 46 },
    { 25, 44, 43 },    { 28, 48, 47 },    { 28, 29, 48 },    { 29, 49, 48 },    { 29, 30, 49 },    { 30, 50, 49 },
    { 30, 16, 50 },    { 16, 31, 50 },    { 31, 32, 51 },    { 32, 52, 51 },    { 32, 33, 52 },    { 33, 53, 52 },
    { 33, 34, 53 },    { 34, 54, 53 },    { 34, 35, 54 },    { 35, 55, 54 },    { 35, 36, 55 },    { 36, 56, 55 },
    { 36, 37, 56 },    { 37, 57, 56 },    { 37, 38, 57 },    { 38, 58, 57 },    { 38, 39, 58 },    { 39, 59, 58 },
    { 39, 40, 59 },    { 40, 60, 59 },    { 40, 41, 60 },    { 41, 61, 60 },    { 41, 42, 61 },    { 42, 62, 61 },
    { 42, 43, 62 },    { 43, 63, 62 },    { 51, 72, 71 },    { 51, 52, 72 },    { 52, 73, 72 },    { 52, 53, 73 },
    { 53, 74, 73 },    { 53, 54, 74 },    { 54, 75, 74 },    { 54, 55, 75 },    { 55, 76, 75 },    { 55, 56, 76 },
    { 56, 77, 76 },    { 56, 57, 77 },    { 57, 78, 77 },    { 57, 58, 78 },    { 58, 79, 78 },    { 58, 59, 79 },
    { 59, 80, 79 },    { 59, 60, 80 },    { 60, 81, 80 },    { 60, 61, 81 },    { 61, 82, 81 },    { 61, 62, 82 },
    { 62, 83, 82 },    { 62, 63, 83 },    { 71, 72, 91 },    { 72, 92, 91 },    { 72, 73, 92 },    { 73, 93, 92 },
    { 73, 74, 93 },    { 74, 94, 93 },    { 74, 75, 94 },    { 75, 95, 94 },    { 75, 76, 95 },    { 76, 96, 95 },
    { 76, 77, 96 },    { 77, 97, 96 },    { 77, 78, 97 },    { 78, 98, 97 },    { 78, 79, 98 },    { 79, 99, 98 },
    { 79, 80, 99 },    { 80, 100, 99 },   { 80, 81, 100 },   { 81, 101, 100 },  { 81, 82, 101 },   { 82, 102, 101 },
    { 82, 83, 102 },   { 83, 103, 102 },  { 91, 112, 111 },  { 91, 92, 112 },   { 92, 113, 112 },  { 92, 93, 113 },
    { 93, 114, 113 },  { 93, 94, 114 },   { 94, 115, 114 },  { 94, 95, 115 },   { 95, 116, 115 },  { 95, 96, 116 },
    { 96, 117, 116 },  { 96, 97, 117 },   { 97, 118, 117 },  { 97, 98, 118 },   { 98, 119, 118 },  { 98, 99, 119 },
    { 99, 120, 119 },  { 99, 100, 120 },  { 100, 121, 120 }, { 100, 101, 121 }, { 101, 122, 121 }, { 101, 102, 122 },
    { 102, 123, 122 }, { 102, 103, 123 }, { 43, 44, 63 },    { 44, 64, 63 },    { 44, 45, 64 },    { 45, 65, 64 },
    { 45, 46, 65 },    { 46, 66, 65 },    { 46, 47, 66 },    { 47, 67, 66 },    { 47, 48, 67 },    { 48, 68, 67 },
    { 48, 49, 68 },    { 49, 69, 68 },    { 49, 50, 69 },    { 50, 70, 69 },    { 50, 31, 70 },    { 31, 51, 70 },
    { 63, 84, 83 },    { 63, 64, 84 },    { 64, 85, 84 },    { 64, 65, 85 },    { 65, 86, 85 },    { 65, 66, 86 },
    { 66, 87, 86 },    { 66, 67, 87 },    { 67, 88, 87 },    { 67, 68, 88 },    { 68, 89, 88 },    { 68, 69, 89 },
    { 69, 90, 89 },    { 69, 70, 90 },    { 70, 71, 90 },    { 70, 51, 71 },    { 83, 84, 103 },   { 84, 104, 103 },
    { 84, 85, 104 },   { 85, 105, 104 },  { 85, 86, 105 },   { 86, 106, 105 },  { 86, 87, 106 },   { 87, 107, 106 },
    { 87, 88, 107 },   { 88, 108, 107 },  { 88, 89, 108 },   { 89, 109, 108 },  { 89, 90, 109 },   { 90, 110, 109 },
    { 90, 71, 110 },   { 71, 91, 110 },   { 103, 124, 123 }, { 103, 104, 124 }, { 104, 125, 124 }, { 104, 105, 125 },
    { 105, 126, 125 }, { 105, 106, 126 }, { 106, 127, 126 }, { 106, 107, 127 }, { 107, 128, 127 }, { 107, 108, 128 },
    { 108, 129, 128 }, { 108, 109, 129 }, { 109, 130, 129 }, { 109, 110, 130 }, { 110, 111, 130 }, { 110, 91, 111 },
    { 161, 160, 156 }, { 155, 146, 160 }, { 146, 156, 160 }, { 146, 147, 156 }, { 144, 145, 155 }, { 145, 146, 155 },
    { 145, 131, 146 }, { 131, 147, 146 }, { 131, 132, 147 }, { 134, 135, 149 }, { 161, 156, 157 }, { 147, 148, 156 },
    { 148, 157, 156 }, { 148, 149, 157 }, { 132, 133, 147 }, { 133, 148, 147 }, { 133, 134, 148 }, { 134, 149, 148 },
    { 161, 157, 158 }, { 149, 150, 157 }, { 150, 158, 157 }, { 150, 151, 158 }, { 135, 136, 149 }, { 136, 150, 149 },
    { 136, 137, 150 }, { 137, 151, 150 }, { 137, 138, 151 }, { 140, 141, 153 }, { 161, 158, 159 }, { 151, 152, 158 },
    { 152, 159, 158 }, { 152, 153, 159 }, { 138, 139, 151 }, { 139, 152, 151 }, { 139, 140, 152 }, { 140, 153, 152 },
    { 161, 159, 160 }, { 153, 154, 159 }, { 154, 160, 159 }, { 154, 155, 160 }, { 141, 142, 153 }, { 142, 154, 153 },
    { 142, 143, 154 }, { 143, 155, 154 }, { 143, 144, 155 }, { 111, 131, 145 }, { 111, 112, 131 }, { 112, 132, 131 },
    { 112, 113, 132 }, { 113, 114, 132 }, { 114, 133, 132 }, { 114, 115, 133 }, { 115, 116, 134 }, { 116, 135, 134 },
    { 116, 117, 135 }, { 117, 118, 135 }, { 118, 136, 135 }, { 118, 119, 136 }, { 115, 134, 133 }, { 119, 137, 136 },
    { 119, 120, 137 }, { 120, 138, 137 }, { 120, 121, 138 }, { 121, 122, 138 }, { 122, 139, 138 }, { 122, 123, 139 },
    { 139, 123, 140 }, { 140, 123, 124 }, { 140, 124, 141 }, { 141, 124, 125 }, { 141, 125, 126 }, { 141, 126, 142 },
    { 142, 127, 143 }, { 143, 127, 128 }, { 143, 128, 144 }, { 144, 128, 129 }, { 144, 129, 130 }, { 144, 130, 145 },
    { 142, 126, 127 }, { 130, 111, 145 },
};

ActorProfile En_Bigslime_Profile = {
    ACTOR_EN_BIGSLIME,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_BIGSLIME,
    sizeof(EnBigslime),
    (ActorFunc)EnBigslime_Init,
    (ActorFunc)EnBigslime_Destroy,
    (ActorFunc)EnBigslime_UpdateGekko,
    (ActorFunc)EnBigslime_DrawGekko,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 60, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 10, 100, 100, 50 };

typedef enum {
    /* 0x0 */ BIGSLIME_DMGEFF_NONE,
    /* 0x0 */ BIGSLIME_DMGEFF_STUN,
    /* 0x2 */ BIGSLIME_DMGEFF_FIRE,
    /* 0x3 */ BIGSLIME_DMGEFF_ICE,
    /* 0x4 */ BIGSLIME_DMGEFF_LIGHT,
    /* 0x5 */ BIGSLIME_DMGEFF_ELECTRIC_STUN,
    /* 0xD */ BIGSLIME_DMGEFF_DEKU_STUN = 0xD,
    /* 0xE */ BIGSLIME_DMGEFF_HOOKSHOT,
    /* 0xF */ BIGSLIME_DMGEFF_BREAK_ICE
} BigslimeDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BIGSLIME_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Zora boomerang */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, BIGSLIME_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Sword          */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Fire arrow     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, BIGSLIME_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, BIGSLIME_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Deku spin      */ DMG_ENTRY(0, BIGSLIME_DMGEFF_DEKU_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
    /* Zora punch     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BIGSLIME_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, BIGSLIME_DMGEFF_BREAK_ICE),
};

// Start vtx numbers for each ring in the vtx sphere
static s32 sVtxRingStartIndex[] = {
    0, 1, 6, 16, 31, 51, 71, 91, 111, 131, 146, 156, 161, BIGSLIME_NUM_VTX,
};

static AnimationHeader* sGekkoAttackAnimations[] = {
    &gGekkoJabPunchAnim,
    &gGekkoHookPunchAnim,
    &gGekkoKickAnim,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_GEKKO_GIANT_SLIME, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(lockOnArrowOffset, -13221, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_STOP),
};

void EnBigslime_Init(Actor* thisx, PlayState* play2) {
    static s32 sIsFrogReturnedFlags[] = {
        WEEKEVENTREG_32_40,
        WEEKEVENTREG_32_80,
        WEEKEVENTREG_33_01,
        WEEKEVENTREG_33_02,
    };
    PlayState* play = play2;
    EnBigslime* this = (EnBigslime*)thisx;
    s32 i;

    sBigslimeStaticVtxData = ResourceMgr_LoadVtxByName(sBigslimeStaticVtx);
    sBigslimeDynamicVtxData[0] = ResourceMgr_LoadVtxByName(sBigslimeDynamicState0Vtx);
    sBigslimeDynamicVtxData[1] = ResourceMgr_LoadVtxByName(sBigslimeDynamicState1Vtx);
    sBigslimeTargetVtxData = ResourceMgr_LoadVtxByName(sBigslimeTargetVtx);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGekkoSkel, &gGekkoLookAroundAnim, this->jointTable, this->morphTable,
                       GEKKO_LIMB_MAX);

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        Collider_InitAndSetCylinder(play, &this->bigslimeCollider[i], &this->actor, &sCylinderInit);
    }

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    Collider_InitAndSetCylinder(play, &this->gekkoCollider, &this->actor, &sCylinderInit);
    this->gekkoCollider.base.colMaterial = COL_MATERIAL_HIT6;
    this->gekkoCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
    this->gekkoCollider.base.atFlags &= ~AT_ON;
    this->gekkoCollider.base.ocFlags1 &= ~OC1_NO_PUSH;
    this->actor.params = CLAMP(this->actor.params, 1, 4);

    if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        Actor_Kill(&this->actor);
        if (!CHECK_WEEKEVENTREG(sIsFrogReturnedFlags[this->actor.params - 1])) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_MINIFROG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, this->actor.params);
        }
        return;
    }

    this->csId = this->actor.csId;
    this->actor.scale.x = this->actor.scale.z = 0.15f;
    this->actor.scale.y = 0.075f;
    this->vtxScaleX = this->vtxScaleZ = 0.015000001f;
    this->actor.home.pos.x = GBT_ROOM_5_CENTER_X;
    this->actor.home.pos.y = GBT_ROOM_5_MAX_Y - 75.0f;
    this->actor.home.pos.z = GBT_ROOM_5_CENTER_Z;
    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i] = (EnMinislime*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MINISLIME,
                                                              0.0f, 0.0f, 0.0f, 0, 0, 0, i);
        if (this->minislime[i] == NULL) {
            for (i = i - 1; i >= 0; i--) {
                Actor_Kill(&this->minislime[i]->actor);
            }

            Actor_Kill(&this->actor);
            return;
        }
    }

    this->minislimeFrozenTexAnim = Lib_SegmentedToVirtual(gMinislimeFrozenTexAnim);
    this->bigslimeFrozenTexAnim = Lib_SegmentedToVirtual(gBigslimeFrozenTexAnim);
    this->iceShardTexAnim = Lib_SegmentedToVirtual(gBigslimeIceShardTexAnim);
    this->actor.world.pos.y = GBT_ROOM_5_MIN_Y;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.shape.shadowAlpha = 255;
    this->gekkoScale = 0.007f;
    this->actor.shape.rot.y = 0;
    this->minislimeToThrow = this->minislime[0];
    EnBigslime_SetupInitEntrance(this);
}

void EnBigslime_Destroy(Actor* thisx, PlayState* play) {
    EnBigslime* this = (EnBigslime*)thisx;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        Collider_DestroyCylinder(play, &this->bigslimeCollider[i]);
    }

    Collider_DestroyCylinder(play, &this->gekkoCollider);
    AudioSfx_StopByPos(&this->gekkoProjectedPos);
}

void EnBigslime_DynamicVtxCopyState(EnBigslime* this) {
    Vtx* dynamicVtxDest;
    Vtx* dynamicVtxSrc;
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtxDest = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        dynamicVtxSrc = &sBigslimeDynamicVtxData[this->dynamicVtxState ^ 1][i];
        for (j = 0; j < 3; j++) {
            dynamicVtxDest->n.ob[j] = dynamicVtxSrc->n.ob[j];
            dynamicVtxDest->n.n[j] = dynamicVtxSrc->n.n[j];
        }
    }
}

void EnBigslime_Vec3fNormalize(Vec3f* vec) {
    f32 magnitude = Math3D_Vec3fMagnitude(vec);

    if (magnitude > 1.0f) {
        Math_Vec3f_Scale(vec, 1.0f / magnitude);
    }
}

/**
 * Updates the surface normal vector of each vertex in the fused jelly mesh
 * each frame as the dynamic vtx is constantly changing shape
 */
void EnBigslime_UpdateSurfaceNorm(EnBigslime* this) {
    Vec3f vtxNorm[BIGSLIME_NUM_VTX];
    Vec3f vecTriEdge1;
    Vec3f vecTriEdge2;
    Vec3f vecTriNorm;
    Vec3f* vtxNormAddr;
    Vtx* dynamicVtx;
    Vtx* dynamicVtx12;
    Vtx* dynamicVtx0;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        Math_Vec3f_Copy(&vtxNorm[i], &gZeroVec3f);
    }

    for (i = 0; i < BIGSLIME_NUM_FACES; i++) {
        dynamicVtx0 = &sBigslimeDynamicVtxData[this->dynamicVtxState][sBigslimeTri[i].v[0]];
        dynamicVtx12 = &sBigslimeDynamicVtxData[this->dynamicVtxState][sBigslimeTri[i].v[1]];
        vecTriEdge1.x = dynamicVtx12->n.ob[0] - dynamicVtx0->n.ob[0];
        vecTriEdge1.y = dynamicVtx12->n.ob[1] - dynamicVtx0->n.ob[1];
        vecTriEdge1.z = dynamicVtx12->n.ob[2] - dynamicVtx0->n.ob[2];

        dynamicVtx12 = &sBigslimeDynamicVtxData[this->dynamicVtxState][sBigslimeTri[i].v[2]];
        vecTriEdge2.x = dynamicVtx12->n.ob[0] - dynamicVtx0->n.ob[0];
        vecTriEdge2.y = dynamicVtx12->n.ob[1] - dynamicVtx0->n.ob[1];
        vecTriEdge2.z = dynamicVtx12->n.ob[2] - dynamicVtx0->n.ob[2];

        Math3D_Vec3f_Cross(&vecTriEdge1, &vecTriEdge2, &vecTriNorm);
        EnBigslime_Vec3fNormalize(&vecTriNorm);

        Math_Vec3f_Sum(&vtxNorm[sBigslimeTri[i].v[0]], &vecTriNorm, &vtxNorm[sBigslimeTri[i].v[0]]);
        Math_Vec3f_Sum(&vtxNorm[sBigslimeTri[i].v[1]], &vecTriNorm, &vtxNorm[sBigslimeTri[i].v[1]]);
        Math_Vec3f_Sum(&vtxNorm[sBigslimeTri[i].v[2]], &vecTriNorm, &vtxNorm[sBigslimeTri[i].v[2]]);
    }

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        vtxNormAddr = &vtxNorm[i];
        EnBigslime_Vec3fNormalize(vtxNormAddr);
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        dynamicVtx->n.n[0] = vtxNormAddr->x * 120.0f;
        dynamicVtx->n.n[1] = vtxNormAddr->y * 120.0f;
        dynamicVtx->n.n[2] = vtxNormAddr->z * 120.0f;
    }
}

void EnBigslime_GetMaxMinVertices(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    Vtx* dynamicVtx;
    s16 vtxMaxX = 0;
    s16 vtxMaxY = 0;
    s16 vtxMaxZ = 0;
    s16 vtxMinX = 0;
    s16 vtxMinY = 0;
    s16 vtxMinZ = 0;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];

        if (vtxMaxX < dynamicVtx->n.ob[0]) {
            vtxMaxX = dynamicVtx->n.ob[0];
        } else if (vtxMinX > dynamicVtx->n.ob[0]) {
            vtxMinX = dynamicVtx->n.ob[0];
        }

        if (vtxMaxY < dynamicVtx->n.ob[1]) {
            vtxMaxY = dynamicVtx->n.ob[1];
        } else if (vtxMinY > dynamicVtx->n.ob[1]) {
            vtxMinY = dynamicVtx->n.ob[1];
        }

        if (vtxMaxZ < dynamicVtx->n.ob[2]) {
            vtxMaxZ = dynamicVtx->n.ob[2];
        } else if (vtxMinZ > dynamicVtx->n.ob[2]) {
            vtxMinZ = dynamicVtx->n.ob[2];
        }
    }

    vtxMax->x = vtxMaxX * this->actor.scale.x;
    vtxMax->y = vtxMaxY * this->actor.scale.y;
    vtxMax->z = vtxMaxZ * this->actor.scale.z;

    vtxMin->x = vtxMinX * this->actor.scale.x;
    vtxMin->y = vtxMinY * this->actor.scale.y;
    vtxMin->z = vtxMinZ * this->actor.scale.z;
}

void EnBigslime_UpdateScale(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    if (vtxMin->x < vtxMax->x) {
        this->vtxScaleX = vtxMax->x * (0.100000005f / BIGSLIME_RADIUS_F);
    } else {
        this->vtxScaleX = -vtxMin->x * (0.100000005f / BIGSLIME_RADIUS_F);
    }

    if (vtxMin->z < vtxMax->z) {
        this->vtxScaleZ = vtxMax->z * (0.100000005f / BIGSLIME_RADIUS_F);
    } else {
        this->vtxScaleZ = -vtxMin->z * (0.100000005f / BIGSLIME_RADIUS_F);
    }
}

/**
 * Checks the world position against all walls, the floor, and the ceiling to ensure
 * bigslime is not outside this range. Accounts only for the world position, and the
 * maximum and minimum verticies. If any boundaries are crossed, the world position
 * and bgCheckFlags are updated accordingly.
 *
 * Differs from EnBigslime_CheckVtxWallBoundaries by checking and updating
 * a single world position instead of checking and updating individual vertices
 */
void EnBigslime_CheckRoomBoundaries(EnBigslime* this, Vec3f* vtxMax, Vec3f* vtxMin) {
    f32 worldPosX;
    f32 vtxMaxX;
    f32 vtxMinX;
    f32 worldPosZ;
    f32 vtxMaxZ;
    f32 vtxMinZ;

    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING);
    if ((this->actor.world.pos.y + vtxMax->y) > GBT_ROOM_5_MAX_Y) {
        this->actor.world.pos.y = GBT_ROOM_5_MAX_Y - vtxMax->y;
        this->actor.bgCheckFlags |= BGCHECKFLAG_CEILING;
    }

    if ((this->actor.world.pos.y + vtxMin->y) <= GBT_ROOM_5_MIN_Y) {
        this->actor.world.pos.y = GBT_ROOM_5_MIN_Y - vtxMin->y;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND_TOUCH;
        }

        this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
    } else {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }

    if ((this->actionFunc != EnBigslime_Freeze) || !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        worldPosX = this->actor.world.pos.x;
        vtxMaxX = vtxMax->x;
        if (GBT_ROOM_5_MAX_X < (worldPosX + vtxMaxX)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
            this->actor.world.pos.x = GBT_ROOM_5_MAX_X - vtxMaxX;
        } else {
            vtxMinX = vtxMin->x;
            if ((worldPosX + vtxMinX) < GBT_ROOM_5_MIN_X) {
                this->actor.world.pos.x = GBT_ROOM_5_MIN_X - vtxMinX;
                this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
            }
        }

        worldPosZ = this->actor.world.pos.z;
        vtxMaxZ = vtxMax->z;
        if (GBT_ROOM_5_MAX_Z < (worldPosZ + vtxMaxZ)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
            this->actor.world.pos.z = GBT_ROOM_5_MAX_Z - vtxMaxZ;
        } else {
            vtxMinZ = vtxMin->z;
            if ((worldPosZ + vtxMinZ) < GBT_ROOM_5_MIN_Z) {
                this->actor.world.pos.z = GBT_ROOM_5_MIN_Z - vtxMinZ;
                this->actor.bgCheckFlags |= BGCHECKFLAG_WALL;
            }
        }
    }
}

void EnBigslime_UpdateBigslimeCollider(EnBigslime* this, PlayState* play) {
    Vtx* dynamicVtx;
    f32 xzDist;
    s16 vtxRingMaxY[BIGSLIME_NUM_RING_VTX];
    s16 vtxRingMinY[BIGSLIME_NUM_RING_VTX];
    f32 vtxRingMaxXZDist[BIGSLIME_NUM_RING_VTX];
    Cylinder16* dim;
    f32 maxScaleXScaleZ = (this->actor.scale.z < this->actor.scale.x ? this->actor.scale.x : this->actor.scale.z);
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_RING_VTX; i++) {
        vtxRingMaxY[i] = -30000;
        vtxRingMinY[i] = 30000;
        vtxRingMaxXZDist[i] = 0.0f;

        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][j];
            if (vtxRingMaxY[i] < dynamicVtx->n.ob[1]) {
                vtxRingMaxY[i] = dynamicVtx->n.ob[1];
            }

            if (vtxRingMinY[i] > dynamicVtx->n.ob[1]) {
                vtxRingMinY[i] = dynamicVtx->n.ob[1];
            }

            xzDist = sqrtf(SQ(dynamicVtx->n.ob[0]) + SQ(dynamicVtx->n.ob[2]));
            if (vtxRingMaxXZDist[i] < xzDist) {
                vtxRingMaxXZDist[i] = xzDist;
            }
        }
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        dim = &this->bigslimeCollider[i].dim;
        dim->pos.y = (vtxRingMinY[i + 1] * this->actor.scale.y) + this->actor.world.pos.y;
        dim->height = (vtxRingMaxY[i] - vtxRingMinY[i + 1]) * this->actor.scale.y;
        dim->radius = maxScaleXScaleZ *
                      (vtxRingMaxXZDist[i] < vtxRingMaxXZDist[i + 1] ? vtxRingMaxXZDist[i + 1] : vtxRingMaxXZDist[i]);
        dim->height = CLAMP_MIN(dim->height, 1);
        dim->pos.x = this->actor.world.pos.x;
        dim->pos.z = this->actor.world.pos.z;
    }

    if (this->bigslimeCollider[0].base.atFlags & AT_ON) {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->bigslimeCollider[i].base);
        }
    }

    if (this->bigslimeCollider[0].base.acFlags & AC_ON) {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bigslimeCollider[i].base);
        }
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bigslimeCollider[i].base);
    }
}

void EnBigslime_AddWavySurface(EnBigslime* this) {
    s32 randInt;
    f32 randFloat;
    s32 i;

    this->wavySurfaceTimer = 24;
    randInt = (s32)(Rand_ZeroOne() * 6.0f) >> 1;
    randFloat = (Rand_ZeroOne() * 40.0f) + 50.0f;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        if ((i % 6) == 0) {
            randFloat = (Rand_ZeroOne() * 40.0f) + 50.0f;
        }

        this->vtxSurfacePerturbation[i] = Math_SinF((randInt + i) * (M_PIf / 3)) * randFloat * 0.001f;
    }
}

void EnBigslime_UpdateWavySurface(EnBigslime* this) {
    f32 vtxSurfaceWave;
    Vtx* staticVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    if (this->wavySurfaceTimer == 0) {
        EnBigslime_AddWavySurface(this);
    }

    this->wavySurfaceTimer--;
    vtxSurfaceWave = Math_SinF(this->wavySurfaceTimer * (M_PIf / 12));
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        staticVtx = &sBigslimeStaticVtxData[i];
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        for (j = 0; j < 3; j++) {
            // Formula: dynamicVtx = staticVtx * (1 + sin * perturbation)
            dynamicVtx->n.ob[j] =
                staticVtx->n.ob[j] + (s32)(vtxSurfaceWave * staticVtx->n.ob[j] * this->vtxSurfacePerturbation[i]);
        }
    }
}

void EnBigslime_InitFallMinislime(EnBigslime* this) {
    EnMinislime* minislime;
    s32 i;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        minislime = this->minislime[i];
        minislime->actor.params = MINISLIME_INIT_FALL;
        minislime->actor.world.pos.x = Rand_CenteredFloat(80.0f) + (GBT_ROOM_5_MIN_X + ((i % 4) + 1) * 192.0f);
        minislime->actor.world.pos.y = Rand_ZeroFloat(250.0f) + (GBT_ROOM_5_CENTER_Y + 50.0f);
        minislime->actor.world.pos.z = Rand_CenteredFloat(80.0f) + (GBT_ROOM_5_MIN_Z + ((i / 4) + 1) * 192.0f);
    }

    this->minislimeState = MINISLIME_ACTIVE_INIT_STATE;
}

void EnBigslime_SetMinislimeBreakLocation(EnBigslime* this) {
    // Minislime spawn at these vertices when Frozen Bigslime Shatters
    static s32 minislimeSpawnVtx[] = {
        7,  9,  11,  13,  15,  // 5 equally spaced vertices on vtx ring 2 from top
        52, 56, 60,  64,  68,  // 5 equally spaced vertices on vtx ring 5 from top
        94, 98, 102, 106, 110, // 5 equally spaced vertices on vtx ring 7 from top
    };
    Vtx* dynamicVtx;
    EnMinislime* minislime;
    s32 i;

    this->minislimeState = MINISLIME_ACTIVE_CONTINUE_STATE;
    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][minislimeSpawnVtx[i]];
        minislime = this->minislime[i];
        minislime->actor.params = MINISLIME_BREAK_BIGSLIME;
        minislime->actor.world.pos.x = (dynamicVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
        minislime->actor.world.pos.y = (dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
        minislime->actor.world.pos.z = (dynamicVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
        minislime->actor.world.rot.x = 0x1000 + 0x333 * (MINISLIME_NUM_SPAWN - i);
    }
}

void EnBigslime_SetPlayerParams(EnBigslime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags2 & PLAYER_STATE2_80) {
        player->actor.parent = NULL;
        player->av2.actionVar2 = 100;
        func_800B8D98(play, &this->actor, 10.0f, this->actor.world.rot.y, 10.0f);
    }
}

void EnBigslime_EndThrowMinislime(EnBigslime* this) {
    if (this->actionFunc == EnBigslime_ThrowMinislime &&
        this->minislimeToThrow->actor.params == MINISLIME_SETUP_GEKKO_THROW) {
        this->minislimeToThrow->actor.params = MINISLIME_IDLE;
    }
}

void EnBigslime_BreakIntoMinislime(EnBigslime* this, PlayState* play) {
    s32 i;
    s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, 20000);
    Quake_SetPerturbations(quakeIndex, 15, 0, 0, 0);
    Quake_SetDuration(quakeIndex, 15);

    Rumble_Request(this->actor.xyzDistToPlayerSq, 180, 20, 100);

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->gekkoCollider.base.acFlags &= ~(AC_ON | AC_HIT);

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
    }

    EnBigslime_SetMinislimeBreakLocation(this);
    this->actor.update = EnBigslime_UpdateGekko;
    this->actor.draw = EnBigslime_DrawGekko;
    this->actor.gravity = -2.0f;
    EnBigslime_SetPlayerParams(this, play);
    EnBigslime_EndCutscene(this, play);
    this->actor.colChkInfo.mass = 50;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER);
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    this->actor.hintId = TATL_HINT_ID_GEKKO_GIANT_SLIME;
    this->gekkoRot.x = 0;
    this->gekkoRot.y = 0;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->formBigslimeTimer = 2;
    EnBigslime_AddIceShardEffect(this, play);
    Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_BREAK);
    EnBigslime_SetupJumpGekko(this);
}

/**
 * Smoothly moves the camera to a side view and keeps it there
 * as bigslime grabs player and the Gekko melee-attacks player
 */
void EnBigslime_UpdateCameraGrabPlayer(EnBigslime* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamEye;
    Vec3f subCamAt;

    Math_Vec3f_Copy(&subCamEye, &subCam->eye);
    Math_Vec3f_Copy(&subCamAt, &subCam->at);

    Math_StepToF(&subCamEye.x, (Math_SinS(this->subCamYawGrabPlayer) * 250.0f) + this->actor.world.pos.x, 10.0f);
    Math_StepToF(&subCamEye.y, GBT_ROOM_5_MIN_Y + 187.5f, 10.0f);
    Math_StepToF(&subCamEye.z, (Math_CosS(this->subCamYawGrabPlayer) * 250.0f) + this->actor.world.pos.z, 10.0f);

    Math_StepToF(&subCamAt.x, this->actor.world.pos.x, 10.0f);
    Math_StepToF(&subCamAt.y, GBT_ROOM_5_MIN_Y + 87.5f, 10.0f);
    Math_StepToF(&subCamAt.z, this->actor.world.pos.z, 10.0f);

    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
}

/**
 * Takes the camera eye and instantaneously moves it 10% closer to the focus point "at".
 * This gives the camera a "jerk" feeling
 * Used everytime player is hit inside of bigslime while being grabbed
 */
void EnBigslime_JerkCameraPlayerHit(EnBigslime* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamEye;

    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &subCamEye);
    Math_Vec3f_Scale(&subCamEye, 0.9f);
    Math_Vec3f_Sum(&subCamEye, &subCam->at, &subCamEye);
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCamEye);
}

/**
 * Performs the camera motion for the intro cutscene as the player enters the room
 * and the battle starts. Positions the camera slightly offset from player,
 * then zooms into the Gekko until the Gekko calls the minislimes down from the ceiling
 */
void EnBigslime_UpdateCameraIntroCs(EnBigslime* this, PlayState* play, s32 noticeTimer) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamEye;
    f32 zoom = (noticeTimer * 19.0f) + 67.0f;
    s16 yawOffset = this->actor.yawTowardsPlayer + (noticeTimer * 0x31);

    subCamEye.x = Math_SinS(yawOffset) * zoom + subCam->at.x;
    subCamEye.z = Math_CosS(yawOffset) * zoom + subCam->at.z;
    subCamEye.y = subCam->at.y + -4.0f + (noticeTimer * 2.0f);

    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCamEye);
}

/**
 * Takes the camera and makes the focus point (at) point at bigslime, who is on the
 * center of the roof. This is used when the minislimes merges into bigslime.
 */
void EnBigslime_UpdateCameraFormingBigslime(EnBigslime* this, PlayState* play) {
    Play_SetCameraAtEye(play, this->subCamId, &this->actor.focus.pos, &Play_GetCamera(play, this->subCamId)->eye);
}

void EnBigslime_EndCutscene(EnBigslime* this, PlayState* play) {
    Camera* subCam;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        subCam = Play_GetCamera(play, this->subCamId);
        Play_SetCameraAtEye(play, CAM_ID_MAIN, &subCam->at, &subCam->eye);
        this->subCamId = SUB_CAM_ID_DONE;
        CutsceneManager_Stop(this->csId);
        this->csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
        Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_END);
    }
}

void EnBigslime_Scale(EnBigslime* this, s16 pitch, f32 xzScale, f32 yScale) {
    this->actor.scale.x = ((Math_SinS(pitch) * xzScale) + 1.5f) * 0.1f; // =  0.15f + 0.1f * xzScale * sin(pitch)
    this->actor.scale.y = ((Math_CosS(pitch) * yScale) + 0.75f) * 0.1f; // = 0.075f + 0.1f * yScale  * cos(pitch)
    this->actor.scale.z = 0.3f - this->actor.scale.x;                   // =  0.15f - 0.1f * xzScale * sin(pitch)
}

/**
 * Set the params used by the floor shockwave when bigslime shatters into minislime
 */
void EnBigslime_InitShockwave(EnBigslime* this, PlayState* play) {
    play->envCtx.lightSettingOverride = 3;
    Math_Vec3f_Copy(&this->frozenPos, &this->actor.world.pos);
    this->frozenPos.y = GBT_ROOM_5_MIN_Y;
    this->shockwaveAlpha = 235;
    this->shockwaveScale = 0.025f;
}

/**
 * Restores bigslime behaviour to the state before it was frozen
 */
void EnBigslime_SetTargetVtxFromPreFrozen(EnBigslime* this) {
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actionFunc = this->actionFuncStored;
    this->skelAnime.playSpeed = 1.0f;
    if (this->actionFunc == EnBigslime_SquishFlat) {
        this->bigslimeCollider[0].base.atFlags |= AT_ON;
        EnBigslime_SetTargetVtxToWideCone(this);
    } else if (this->actionFunc == EnBigslime_Rise) {
        if (this->riseCounter == 0) {
            EnBigslime_SetTargetVtxToThinCone(this);
        } else if (this->riseCounter == 1) {
            EnBigslime_SetTargetVtxToInverseCone(this);
        } else {
            EnBigslime_SetTargetVtxToStaticVtx(this);
        }
    }
}

/**
 * Plays the standard Gekko sound effects without reverb
 */
void EnBigslime_GekkoSfxOutsideBigslime(EnBigslime* this, u16 sfxId) {
    Audio_PlaySfx_AtPos(&this->gekkoProjectedPos, sfxId);
}

/**
 * Adds reverb to Gekko sound effects when enclosed by bigslime
 */
void EnBigslime_GekkoSfxInsideBigslime(EnBigslime* this, u16 sfxId) {
    Audio_PlaySfx_Underwater(&this->gekkoProjectedPos, sfxId);
}

void EnBigslime_GekkoFreeze(EnBigslime* this) {
    this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->gekkoCollider.base.colMaterial = COL_MATERIAL_HIT3;
    this->gekkoCollider.elem.elemMaterial = ELEM_MATERIAL_UNK0;
    this->stunTimer = 2;
    this->gekkoDrawDmgEffScale = 0.75f;
    this->gekkoDrawDmgEffFrozenSteamScale = 1.125f;
    this->gekkoDrawDmgEffAlpha = 1.0f;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
}

void EnBigslime_GekkoThaw(EnBigslime* this, PlayState* play) {
    if (this->gekkoDrawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->gekkoCollider.base.colMaterial = COL_MATERIAL_HIT6;
        this->gekkoCollider.elem.elemMaterial = ELEM_MATERIAL_UNK1;
        this->gekkoDrawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->gekkoBodyPartsPos, GEKKO_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    }
}

void EnBigslime_SetupCutsceneStartBattle(EnBigslime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Camera* subCam = Play_GetCamera(play, this->subCamId);

    play->envCtx.lightSettingOverride = 4;
    Animation_PlayLoop(&this->skelAnime, &gGekkoLookAroundAnim);

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;

    Math_Vec3f_Copy(&subCam->at, &this->actor.focus.pos);
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);

    player->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    player->actor.world.pos.x = Math_SinS(this->actor.yawTowardsPlayer) * 347.0f + this->actor.world.pos.x;
    player->actor.world.pos.z = Math_CosS(this->actor.yawTowardsPlayer) * 347.0f + this->actor.world.pos.z;

    EnBigslime_UpdateCameraIntroCs(this, play, 25);

    this->gekkoRot.y = this->actor.yawTowardsPlayer + 0x8000;
    this->isInitJump = false;
    this->actionFunc = EnBigslime_CutsceneStartBattle;
}

void EnBigslime_CutsceneStartBattle(EnBigslime* this, PlayState* play) {
    if (this->isAnimFinished) {
        EnBigslime_SetupCutsceneNoticePlayer(this);
    } else if (!this->isInitJump && Math_ScaledStepToS(&this->gekkoRot.y, this->actor.yawTowardsPlayer, 0x200)) {
        Animation_PlayOnce(&this->skelAnime, &gGekkoSurpriseJumpAnim);
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP);
        this->isInitJump = true;
    }
}

void EnBigslime_SetupCutsceneNoticePlayer(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoNervousIdleAnim);
    this->noticeTimer = 25;
    this->actionFunc = EnBigslime_CutsceneNoticePlayer;
}

void EnBigslime_CutsceneNoticePlayer(EnBigslime* this, PlayState* play) {
    if (this->noticeTimer != 0) {
        this->noticeTimer--;
    }

    EnBigslime_UpdateCameraIntroCs(this, play, this->noticeTimer);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
    }

    if (this->noticeTimer == 0) {
        EnBigslime_SetupCallMinislime(this, play);
    }
}

void EnBigslime_SetupCallMinislime(EnBigslime* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoCallAnim, 5.0f);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_GREET);
    this->callTimer = 0;
    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
    this->actionFunc = EnBigslime_CallMinislime;
}

void EnBigslime_CallMinislime(EnBigslime* this, PlayState* play) {
    if (this->callTimer > 0) {
        this->callTimer--;
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 4.0f)) {
            EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
        }

        if (this->callTimer == 0) {
            EnBigslime_EndCutscene(this, play);
            this->formBigslimeTimer = 2;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            EnBigslime_SetupIdleNoticePlayer(this);
        }
    } else if (this->isAnimFinished) {
        Animation_PlayLoop(&this->skelAnime, &gGekkoNervousIdleAnim);
        EnBigslime_UpdateCameraIntroCs(this, play, 25);
        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
        EnBigslime_InitFallMinislime(this);
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        this->callTimer = 35;
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
    }
}

void EnBigslime_SetupMoveOnCeiling(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 20.0f;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->actor.speed = 0.0f;
        this->ceilingMoveTimer = 20;
    } else {
        this->ceilingMoveTimer = 320;
        this->actor.speed = 5.0f;
    }

    this->wavySurfaceTimer = 0;
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    this->actionFunc = EnBigslime_MoveOnCeiling;
}

void EnBigslime_MoveOnCeiling(EnBigslime* this, PlayState* play) {
    s16 pitch; // polar (zenith) angle

    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    this->ceilingMoveTimer--;
    pitch = this->ceilingMoveTimer * 0x800;
    EnBigslime_Scale(this, pitch, 0.04f, 0.04f);
    EnBigslime_UpdateWavySurface(this);

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (this->ceilingMoveTimer == 0) {
            EnBigslime_EndCutscene(this, play);
            this->ceilingMoveTimer = 320;
        }
    } else if ((this->actor.xzDistToPlayer < 250.0f) || (this->ceilingMoveTimer == 0)) {
        EnBigslime_SetupDrop(this);
    } else {
        this->actor.speed = (fabsf(Math_SinS(pitch)) * 3.0f) + 5.0f;
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 0x800, 0x80);
        this->gekkoRot.y = this->actor.world.rot.y;
    }
}

void EnBigslime_SetupDrop(EnBigslime* this) {
    this->actor.velocity.y = 0.0f;
    this->ceilingDropTimer = 30;
    this->actor.speed = 0.0f;
    this->actionFunc = EnBigslime_Drop;
}

void EnBigslime_Drop(EnBigslime* this, PlayState* play) {
    Vtx* staticVtx;
    Vtx* dynamicVtx;
    s32 i;

    Math_ScaledStepToS(&this->gekkoRot.x, 0x4000, 0x400);
    if (this->ceilingDropTimer != 0) {
        s32 requiredScopeTemp;

        this->ceilingDropTimer--;
        this->ceilingMoveTimer--;
        EnBigslime_UpdateWavySurface(this);
        EnBigslime_Scale(this, this->ceilingMoveTimer * 0x4000, 0.2f, 0.15f);
        this->actor.scale.z = this->actor.scale.x;
        if (this->ceilingDropTimer == 0) {
            this->actor.gravity = -2.0f;
            EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DOWN);
            this->rotation = 0;
        }
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnBigslime_SetupSquishFlat(this);
    } else {
        Math_StepToF(&this->actor.scale.x, 0.15f, 0.0025f);
        Math_StepToF(&this->actor.scale.y, 0.080000006f, 0.0025f);
        Math_StepToF(&this->actor.scale.z, 0.15f, 0.0025f);
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            staticVtx = &sBigslimeStaticVtxData[i];
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
            if (i > 145) {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1] * 0.9f, 5);
            } else if (i < 16) {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1] * 1.3f, 5);
            } else {
                Math_StepToS(&dynamicVtx->n.ob[1], staticVtx->n.ob[1], 5);
            }
            Math_StepToS(&dynamicVtx->n.ob[0], staticVtx->n.ob[0], 5);
            Math_StepToS(&dynamicVtx->n.ob[2], staticVtx->n.ob[2], 5);
        }
    }
}

/**
 * Checks each individual vertex to see if it is outside the walls of the room.
 * If it is, updates the vertex to be back inside the boundary of the room.
 * Also raises the vertex in the y-direction 100 units in model-space for a single wall
 * and 200 unites in the y-direction for two walls i.e a corner.
 *
 * Differs from EnBigslime_CheckRoomBoundaries by checking and updating
 * individual vertices instead of checking and updating a single world position
 */
void EnBigslime_CheckVtxWallBoundaries(EnBigslime* this) {
    Vtx* dynamicVtx;
    f32 vtxX;
    f32 vtxZ;
    s32 collisionCounter;
    s32 i;
    s16 updateVtxY;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        vtxX = dynamicVtx->n.ob[0] * this->actor.scale.x;
        vtxZ = dynamicVtx->n.ob[2] * this->actor.scale.z;
        collisionCounter = 0;
        if ((this->actor.world.pos.x + vtxX) > GBT_ROOM_5_MAX_X) {
            dynamicVtx->n.ob[0] = (GBT_ROOM_5_MAX_X - this->actor.world.pos.x) / this->actor.scale.x;
            collisionCounter++;
        } else if ((this->actor.world.pos.x + vtxX) < GBT_ROOM_5_MIN_X) {
            dynamicVtx->n.ob[0] = (GBT_ROOM_5_MIN_X - this->actor.world.pos.x) / this->actor.scale.x;
            collisionCounter++;
        }

        if ((this->actor.world.pos.z + vtxZ) > GBT_ROOM_5_MAX_Z) {
            dynamicVtx->n.ob[2] = (GBT_ROOM_5_MAX_Z - this->actor.world.pos.z) / this->actor.scale.z;
            collisionCounter++;
        } else if ((this->actor.world.pos.z + vtxZ) < GBT_ROOM_5_MIN_Z) {
            dynamicVtx->n.ob[2] = (GBT_ROOM_5_MIN_Z - this->actor.world.pos.z) / this->actor.scale.z;
            collisionCounter++;
        }

        if (dynamicVtx->n.ob[1] != -BIGSLIME_RADIUS_S) {
            updateVtxY = collisionCounter * 100;
            dynamicVtx->n.ob[1] += updateVtxY;
        }
    }
}

/**
 * This sets the target vertices into a wide cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = -1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, 1000, 0)
 *      - It is wide because it is smooth and thicker near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToWideCone(EnBigslime* this) {
    Vtx* staticVtx;
    Vtx* targetVtx;
    s16 vtxY;
    f32 xzDist;
    f32 xzScaleVtx;
    s32 i;
    s32 j;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        vtxY = (((Math_CosF(i * (M_PIf / 6)) + 1.0f) * 0.925f) + -0.85f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            xzDist = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));

            if (xzDist > 1.0f) {
                xzScaleVtx = (BIGSLIME_RADIUS_F / (5.0f * xzDist)) * i;
            } else {
                xzScaleVtx = 200.0f * i; // only taken when i = 0, making 200.0f useless
            }

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = vtxY;
        }
    }

    for (; i < BIGSLIME_NUM_RING_VTX; i++) {
        vtxY = ((Math_CosF((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PIf / 16)) * 0.05f) + -1.0f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            xzDist = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            if (xzDist > 1.0f) {
                xzScaleVtx = (BIGSLIME_RADIUS_F / (8.0f * xzDist)) * (14 - i);
            } else {
                // only taken when j = 161, but gets immediately written over (bottom vtx of the sphere)
                xzScaleVtx = 125.0f * (14 - i);
            }

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = vtxY;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
}

void EnBigslime_SetupSquishFlat(EnBigslime* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_JUMP2);
    this->squishFlatTimer = 20;
    this->actor.scale.x = 0.2f;
    this->actor.scale.z = 0.2f;
    this->actor.scale.y = 0.05f;
    this->actor.world.pos.y = GBT_ROOM_5_MIN_Y + 50.0f;
    EnBigslime_SetTargetVtxToWideCone(this);
    EnBigslime_CheckVtxWallBoundaries(this);
    this->bigslimeCollider[0].base.atFlags |= AT_ON;
    this->actionFunc = EnBigslime_SquishFlat;
}

/**
 * Squishes flat like a pancake to try and grab player
 *
 * Notes:
 *     - The vtx shape of Fused Jelly starts from a wide cone shape
 *     - The squishing occurs throught the large changes in &this->actor.scale
 */
void EnBigslime_SquishFlat(EnBigslime* this, PlayState* play) {
    Player* player;
    Vtx* dynamicVtx;
    Vtx* targetVtx;
    s32 i;

    this->squishFlatTimer--;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    Math_SmoothStepToF(&this->actor.scale.x, 0.35f, 0.4f, 0.035f, 0.0034999999f);
    Math_SmoothStepToF(&this->actor.scale.y, 0.030000001f, 0.4f, 0.0030000003f, 0.0003f);
    this->actor.scale.z = this->actor.scale.x;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        targetVtx = &sBigslimeTargetVtxData[i];
        Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
        Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 2, 600, 3);
    }

    EnBigslime_CheckVtxWallBoundaries(this);

    // Checks if any collider is interacting with player. If so, player is grabbed
    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.atFlags & AT_HIT) {
            break;
        }
    }

    if (i != BIGSLIME_NUM_RING_FACES) {
        player = GET_PLAYER(play);
        if (play->grabPlayer(play, player)) {
            player->actor.parent = &this->actor;
            EnBigslime_SetupCutscene(this);
            return;
        }
    }

    if (this->squishFlatTimer == 0) {
        EnBigslime_SetupRise(this);
    }
}

/**
 * This sets the target vertices into a thin cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = -1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, 1000, 0)
 *      - It is thin because it is smooth and thinner near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToThinCone(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* staticVtx;
    f32 xzDistVtx;
    f32 xzScaleVtx;
    s32 i;
    s32 j;
    s16 upperSphereCos;
    f32 lowerSphereCos;
    s32 targetVtxY;

    // Top vtx of the sphere
    sBigslimeTargetVtxData[0].n.ob[1] = BIGSLIME_RADIUS_S;
    sBigslimeTargetVtxData[0].n.ob[0] = 0;
    sBigslimeTargetVtxData[0].n.ob[2] = 0;

    for (i = 1; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        upperSphereCos = (((Math_CosF((i - 1) * (M_PIf / 5)) + 1.0f) * 0.925f) + -0.85f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            xzDistVtx = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            xzDistVtx = (BIGSLIME_RADIUS_F / (5.0f * xzDistVtx)) * i;
            // xzDistVtx is always less than 500.0f
            xzScaleVtx =
                xzDistVtx < 500.0f ? xzDistVtx * 0.75f : BIGSLIME_RADIUS_F - ((BIGSLIME_RADIUS_F - xzDistVtx) * 0.75f);

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = upperSphereCos;
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        lowerSphereCos = Math_CosF((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PIf / BIGSLIME_NUM_RING_FACES));
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            targetVtx->n.ob[0] = staticVtx->n.ob[0];
            targetVtx->n.ob[2] = staticVtx->n.ob[2];
            targetVtxY = TRUNCF_BINANG(((lowerSphereCos * 0.05f) + -1.0f) * BIGSLIME_RADIUS_F);
            targetVtx->n.ob[1] = targetVtxY;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
}

/**
 * This sets the target vertices into a thin inverse cone shape
 *
 * Notes:
 *      - The flat surface of the cone is in the xz-plane at y = +1000
 *      - The radius of the flat surface of the cone is 1000 units
 *      - The apex/point of the cone is at the point (0, -1000, 0)
 *      - It is thin because it is smooth and thinner near the apex than a standard cone
 */
void EnBigslime_SetTargetVtxToInverseCone(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* staticVtx;
    f32 xzDistVtx;
    f32 upperSphereCos;
    s16 lowerSphereCos;
    f32 xzScaleVtx;
    s32 i;
    s32 j;
    s32 vtxY;

    // Top vtx of the sphere
    sBigslimeTargetVtxData[0].n.ob[1] = BIGSLIME_RADIUS_S;
    sBigslimeTargetVtxData[0].n.ob[0] = 0;
    sBigslimeTargetVtxData[0].n.ob[2] = 0;

    for (i = 1; i < BIGSLIME_NUM_RING_FACES / 2; i++) {
        upperSphereCos = Math_CosF((i - 1) * (M_PIf / 10));
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            targetVtx->n.ob[0] = staticVtx->n.ob[0];
            targetVtx->n.ob[2] = staticVtx->n.ob[2];
            vtxY = TRUNCF_BINANG(((upperSphereCos * 0.1f) + 0.9f) * BIGSLIME_RADIUS_F);
            targetVtx->n.ob[1] = vtxY;
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        lowerSphereCos =
            (((Math_CosF((i - BIGSLIME_NUM_RING_FACES / 2) * (M_PIf / 5)) + 1) * 0.925f) + -1.0f) * BIGSLIME_RADIUS_F;
        for (j = sVtxRingStartIndex[i]; j < sVtxRingStartIndex[i + 1]; j++) {
            staticVtx = &sBigslimeStaticVtxData[j];
            targetVtx = &sBigslimeTargetVtxData[j];
            xzDistVtx = sqrtf(SQ(staticVtx->n.ob[0]) + SQ(staticVtx->n.ob[2]));
            xzDistVtx = (BIGSLIME_RADIUS_F / (6.0f * xzDistVtx)) * (BIGSLIME_NUM_RING_FACES - i);
            // xzDistVtx is always less than 500.0f
            xzScaleVtx =
                xzDistVtx < 500.0f ? xzDistVtx * 0.75f : BIGSLIME_RADIUS_F - ((BIGSLIME_RADIUS_F - xzDistVtx) * 0.75f);

            targetVtx->n.ob[0] = staticVtx->n.ob[0] * xzScaleVtx;
            targetVtx->n.ob[2] = staticVtx->n.ob[2] * xzScaleVtx;
            targetVtx->n.ob[1] = lowerSphereCos;
        }
    }

    // Bottom vtx of the sphere
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[1] = -BIGSLIME_RADIUS_S;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[0] = 0;
    sBigslimeTargetVtxData[BIGSLIME_NUM_VTX - 1].n.ob[2] = 0;
}

void EnBigslime_SetTargetVtxToStaticVtx(EnBigslime* this) {
    s32 i;
    Vtx* staticVtx;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        staticVtx = &sBigslimeStaticVtxData[i];
        sBigslimeTargetVtxData[i].n.ob[0] = staticVtx->n.ob[0];
        sBigslimeTargetVtxData[i].n.ob[2] = staticVtx->n.ob[2];
        sBigslimeTargetVtxData[i].n.ob[1] = staticVtx->n.ob[1];
    }
}

void EnBigslime_SetupRise(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_ABOVE);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_UTSUBO_APPEAR_TRG);
    Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_JUMP1);
    this->riseCounter = 0;
    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    EnBigslime_SetTargetVtxToThinCone(this);
    this->actor.world.rot.y = this->gekkoRot.y;
    this->actionFunc = EnBigslime_Rise;
}

/**
 * Moves Fused Jelly from a squish pancake on the floor to the ceiling
 *
 * Notes:
 *      - When riseCounter == 0, Fused Jelly is un-squishing into a thin cone shape
 *      - When riseCounter == 1, Fused Jelly is deforming into an inverse thin cone shape
 *      - When riseCounter == 2 to 9, Fused Jelly is returning to its default spherical shape
 *      - When riseCounter == 10, Fused Jelly starts moving on the ceiling
 */
void EnBigslime_Rise(EnBigslime* this, PlayState* play) {
    Vtx* dynamicVtx;
    Vtx* targetVtx;
    s32 i;

    if (this->riseCounter < 2) {
        Math_SmoothStepToF(&this->actor.scale.x, 0.13f, 0.2f, 0.039f, 0.0038999997f);
        Math_ScaledStepToS(&this->gekkoRot.x, -0x4000, 0x400);
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, 0.15f, 0.4f, 0.015000001f, 0.0015f);
        Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    }

    this->actor.scale.z = this->actor.scale.x;
    EnBigslime_CheckVtxWallBoundaries(this);
    if (this->riseCounter == 0) {
        if (Math_SmoothStepToF(&this->actor.scale.y, 0.3f, 0.5f, 0.015000001f, 0.00075f) < 0.01f) {
            this->riseCounter = 1;
            EnBigslime_SetTargetVtxToInverseCone(this);
        }

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtxData[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 550, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }

    } else if (this->riseCounter == 1) {
        if (Math_SmoothStepToF(&this->actor.scale.y, 0.075f, 0.4f, 0.0075000003f, 0.00075f) < 0.01f) {
            EnBigslime_SetTargetVtxToStaticVtx(this);
            this->riseCounter++;
        }

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtxData[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 50, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }

        this->actor.world.pos.y = GBT_ROOM_5_MAX_Y - (this->actor.scale.y * BIGSLIME_RADIUS_F);
    } else if (this->riseCounter == 10) {
        EnBigslime_SetupMoveOnCeiling(this);
    } else {
        this->riseCounter++;
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
            targetVtx = &sBigslimeTargetVtxData[i];
            Math_SmoothStepToS(&dynamicVtx->n.ob[1], targetVtx->n.ob[1], 5, 550, 3);
            Math_SmoothStepToS(&dynamicVtx->n.ob[0], targetVtx->n.ob[0], 5, 40, 5);
            Math_SmoothStepToS(&dynamicVtx->n.ob[2], targetVtx->n.ob[2], 5, 40, 5);
        }
    }

    EnBigslime_CheckVtxWallBoundaries(this);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 10, 0x800, 0x80);
}

void EnBigslime_SetupCutsceneGrabPlayer(EnBigslime* this, PlayState* play) {
    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
    s16 yaw;

    Play_SetCameraAtEye(play, this->subCamId, &mainCam->at, &mainCam->eye);
    this->grabPlayerTimer = 15;
    this->wavySurfaceTimer = 0;
    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    yaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.world.rot.y;

    if (yaw > 0x4000) {
        this->subCamYawGrabPlayer = -0x2000;
    } else if (yaw > 0) {
        this->subCamYawGrabPlayer = -0x6000;
    } else if (yaw < -0x4000) {
        this->subCamYawGrabPlayer = 0x2000;
    } else {
        this->subCamYawGrabPlayer = 0x6000;
    }

    this->subCamYawGrabPlayer += this->actor.world.rot.y;
    Animation_PlayLoop(&this->skelAnime, &gGekkoBoxingStanceAnim);
    Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_EAT);
    this->actionFunc = EnBigslime_CutsceneGrabPlayer;
}

void EnBigslime_CutsceneGrabPlayer(EnBigslime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 invgrabPlayerTimer;
    f32 magPosXZOffset;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    player->av2.actionVar2 = 0;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    EnBigslime_UpdateCameraGrabPlayer(this, play);
    if (this->grabPlayerTimer > 0) {
        invgrabPlayerTimer = 1.0f / this->grabPlayerTimer;

        this->actor.scale.x = F32_LERPIMP(this->actor.scale.x, 0.15f, invgrabPlayerTimer);
        this->actor.scale.y = F32_LERPIMP(this->actor.scale.y, 0.075f, invgrabPlayerTimer);
        this->actor.scale.z = this->actor.scale.x;

        player->actor.world.pos.x = F32_LERPIMP(player->actor.world.pos.x, this->actor.world.pos.x, invgrabPlayerTimer);
        player->actor.world.pos.z = F32_LERPIMP(player->actor.world.pos.z, this->actor.world.pos.z, invgrabPlayerTimer);
        player->actor.world.pos.y = F32_LERPIMP(
            player->actor.world.pos.y, this->actor.world.pos.y + this->actor.scale.y * -500.0f, invgrabPlayerTimer);

        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];

            // loop over x, y, z
            for (j = 0; j < 3; j++) {
                // Linearly interpolate dynamicVtx --> staticVtx
                dynamicVtx->n.ob[j] +=
                    TRUNCF_BINANG((sBigslimeStaticVtxData[i].n.ob[j] - dynamicVtx->n.ob[j]) * invgrabPlayerTimer);
            }
        }

        invgrabPlayerTimer = (15 - this->grabPlayerTimer) / 15.0f;
        magPosXZOffset = invgrabPlayerTimer * -50.0f;
        this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * magPosXZOffset;
        this->gekkoPosOffset.y = invgrabPlayerTimer * -40.0f;
        this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * magPosXZOffset;
        this->grabPlayerTimer--;
        if (this->grabPlayerTimer == 0) {
            EnBigslime_SetupAttackPlayerInBigslime(this);
        }
    }
}

void EnBigslime_SetupAttackPlayerInBigslime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, sGekkoAttackAnimations[(s32)Rand_ZeroFloat(3.0f) % 3]);
    this->numGekkoMeleeAttacks = (s32)Rand_ZeroFloat(3.0f) + 1;
    this->numGekkoPosGrabPlayer = 6;
    this->actionFunc = EnBigslime_AttackPlayerInBigslime;
}

void EnBigslime_AttackPlayerInBigslime(EnBigslime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 pitch = this->scaleFactor * 0x3333; // polar (zenith) angle

    player->av2.actionVar2 = 0;
    Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    EnBigslime_UpdateCameraGrabPlayer(this, play);
    EnBigslime_UpdateWavySurface(this);

    if (this->scaleFactor != 0) {
        this->scaleFactor--;
    }

    // Checks to see if it's the frame of impact
    if (((this->skelAnime.animation == &gGekkoJabPunchAnim) &&
         Animation_OnFrame(&this->skelAnime, 2.0f)) || // Jab punch makes impact on frame 2 of animation
        ((this->skelAnime.animation == &gGekkoHookPunchAnim) &&
         Animation_OnFrame(&this->skelAnime, 9.0f)) || // Hook Punch makes impact on frames 9 of animation
        ((this->skelAnime.animation == &gGekkoKickAnim) &&
         Animation_OnFrame(&this->skelAnime, 2.0f))) { // Kick makes impact on frame 2 of animation
        this->scaleFactor = 10;
        player->actor.world.pos.x += 20.0f * Math_SinS(this->gekkoRot.y);
        player->actor.world.pos.z += 20.0f * Math_CosS(this->gekkoRot.y);
        EnBigslime_JerkCameraPlayerHit(this, play);
        if (this->skelAnime.animation == &gGekkoKickAnim) {
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_KICK);
        } else {
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH1);
        }
    } else {
        Math_StepToF(&player->actor.world.pos.x, this->actor.world.pos.x, 4.0f);
        Math_StepToF(&player->actor.world.pos.z, this->actor.world.pos.z, 4.0f);
    }

    EnBigslime_Scale(this, pitch, ((this->scaleFactor * 0.08f) + 0.2f) * 0.15f,
                     ((this->scaleFactor * 0.08f) + 0.2f) * 0.05f);
    player->actor.world.pos.y = this->actor.world.pos.y + (this->actor.scale.y * -500.0f);
    if (this->isAnimFinished) {
        this->numGekkoMeleeAttacks--;
        if (this->numGekkoMeleeAttacks == 0) {
            this->numGekkoPosGrabPlayer--;

            if ((gSaveContext.save.saveInfo.playerData.health < 5) || (this->numGekkoPosGrabPlayer == 0)) {
                this->numGekkoPosGrabPlayer = 0;
                this->gekkoRot.y = this->actor.world.rot.y;
                this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
                this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
                EnBigslime_SetupWindupThrowPlayer(this);
                return;
            }

            play->damagePlayer(play, -4);
            Player_PlaySfx(player, player->ageProperties->voiceSfxIdOffset + NA_SE_VO_LI_DAMAGE_S);
            this->gekkoRot.y += (s16)(Rand_S16Offset(0x4000, 0x4000) * (Rand_ZeroOne() < 0.5f ? -1 : 1));
            this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
            this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
            this->numGekkoMeleeAttacks = (s32)Rand_ZeroFloat(3.0f) + 1;
        }
        Animation_PlayOnce(&this->skelAnime, sGekkoAttackAnimations[((s32)Rand_ZeroFloat(3.0f) % 3)]);
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_B_SLIME_PUNCH_MOVE - SFX_FLAG);
}

/**
 * Calculates the surface perturbation (multiplicative offset from the static vertices)
 * used to update dynamic vertices when bigslime is about to throw/eject player from inside
 * at the end of the grab-player cutscene
 *
 * A unit normal vector (unitVec) is used to represent the direction player will be thrown.
 * This unit vector has the following spherical coordinates:
 *     - radius = 1
 *     - yaw (azimuthal angle) = this->actor.world.rot.y
 *     - pitch (polar/zenith angle) = M_PI / 4
 *
 * This unit normal vector is converted into x-y-z coordinates and dot-producted with
 * the model coordinates of each individual vertex. The surface perturbation is then
 * set to be linearly proportional to this dot product
 *
 * This leads to the bending shape observed during windup as player is about being thrown out of bigslime
 */
void EnBigslime_SetupWindupThrowPlayer(EnBigslime* this) {
    Vtx* dynamicVtx;
    f32 dotXYZ;
    f32 unitVecX = Math_SinS(this->actor.world.rot.y) * M_SQRT1_2f;
    f32 unitVecZ = Math_CosS(this->actor.world.rot.y) * M_SQRT1_2f;
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];

        // vector dot product between each dynamicVtx and the unit normal vector describing player's thrown direction
        dotXYZ = (dynamicVtx->n.ob[0] * unitVecX + dynamicVtx->n.ob[1] * M_SQRT1_2f + dynamicVtx->n.ob[2] * unitVecZ) *
                 0.001f;
        if (dotXYZ < 0.01f) {
            this->vtxSurfacePerturbation[i] = 0.0f;
        } else {
            this->vtxSurfacePerturbation[i] = dotXYZ * 0.5f;
        }
    }

    this->windupPunchTimer = 27;
    Animation_PlayOnce(&this->skelAnime, &gGekkoWindupPunchAnim);
    EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH2);
    this->actionFunc = EnBigslime_WindupThrowPlayer;
}

void EnBigslime_WindupThrowPlayer(EnBigslime* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 scale;
    f32 invWindupPunchTimer;
    Vtx* dynamicVtx;
    Vtx* staticVtx;
    s32 i;
    s32 j;

    this->windupPunchTimer--;
    EnBigslime_UpdateCameraGrabPlayer(this, play);
    if (this->windupPunchTimer > 0) {
        invWindupPunchTimer = 1.0f / this->windupPunchTimer;
        scale = Math_CosF(this->windupPunchTimer * (M_PIf / 27)) + 1.0f;
        player->actor.world.pos.y = this->actor.world.pos.y + (this->actor.scale.y * -500.0f);

        // Linearly interpolate gekkoRot.y --> this->actor.world.rot.y
        this->gekkoRot.y += TRUNCF_BINANG((s16)(this->actor.world.rot.y - this->gekkoRot.y) * invWindupPunchTimer);
        this->gekkoPosOffset.x = Math_SinS(this->gekkoRot.y) * -50.0f;
        this->gekkoPosOffset.z = Math_CosS(this->gekkoRot.y) * -50.0f;
    } else {
        if (this->windupPunchTimer == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_REVERSE);
            EnBigslime_GekkoSfxInsideBigslime(this, NA_SE_EN_FROG_PUNCH1);
        }

        scale = 0.5f - Math_CosF(-this->windupPunchTimer * (M_PIf / 5)) * 0.5f;
        if (this->windupPunchTimer == -5) {
            if (player->stateFlags2 & PLAYER_STATE2_80) {
                player->actor.parent = NULL;
                player->av2.actionVar2 = 100;
            }

            player->actor.velocity.y = 0.0f;
            func_800B8D50(play, &this->actor, 10.0f, this->actor.world.rot.y, 10.0f, 4);
            EnBigslime_SetupSetDynamicVtxThrowPlayer(this, play);
        }

        player->actor.world.pos.x =
            (Math_SinS(this->actor.world.rot.y) * (1060.5f * this->actor.scale.x) * scale) + this->actor.world.pos.x;
        player->actor.world.pos.z =
            (Math_CosS(this->actor.world.rot.y) * (1060.5f * this->actor.scale.z) * scale) + this->actor.world.pos.z;
        player->actor.world.pos.y =
            this->actor.world.pos.y + (this->actor.scale.y * -500.0f) + (1060.5f * this->actor.scale.y * scale);
        invWindupPunchTimer = 1.0f / (this->windupPunchTimer + 6);
        // this->gekkoPosOffset *= (1 - invWindupPunchTimer)
        this->gekkoPosOffset.x -= this->gekkoPosOffset.x * invWindupPunchTimer;
        this->gekkoPosOffset.y -= this->gekkoPosOffset.y * invWindupPunchTimer;
        this->gekkoPosOffset.z -= this->gekkoPosOffset.z * invWindupPunchTimer;
        scale = 2.0f - (3.0f * scale);
    }

    // Deforming Bigslime during the final windup punch while grabbing player using vtxSurfacePerturbation
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        staticVtx = &sBigslimeStaticVtxData[i];
        if (this->vtxSurfacePerturbation[i] != 0.0f) {
            if (this->windupPunchTimer > 0) {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Linearly interpolate dynamicVtx --> staticVtx * (1 - scale * vtxSurfacePerturbation)
                    dynamicVtx->n.ob[j] += TRUNCF_BINANG(
                        ((staticVtx->n.ob[j] - (s32)(scale * staticVtx->n.ob[j] * this->vtxSurfacePerturbation[i])) -
                         dynamicVtx->n.ob[j]) *
                        invWindupPunchTimer);
                }
            } else {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Directly set dynamicVtx --> staticVtx * (1 - scale * vtxSurfacePerturbation)
                    dynamicVtx->n.ob[j] =
                        staticVtx->n.ob[j] - (s32)((scale * staticVtx->n.ob[j]) * this->vtxSurfacePerturbation[i]);
                }
            }
        } else {
            if (this->windupPunchTimer > 0) {
                // loop over x, y, z
                for (j = 0; j < 3; j++) {
                    // Linearly interpolate dynamicVtx --> staticVtx
                    dynamicVtx->n.ob[j] +=
                        TRUNCF_BINANG((staticVtx->n.ob[j] - dynamicVtx->n.ob[j]) * invWindupPunchTimer);
                }
            }
        }
    }
}

void EnBigslime_SetupSetDynamicVtxThrowPlayer(EnBigslime* this, PlayState* play) {
    this->grabPlayerTimer = 10;
    EnBigslime_SetTargetVtxToWideCone(this);
    EnBigslime_CheckVtxWallBoundaries(this);
    EnBigslime_EndCutscene(this, play);
    this->actionFunc = EnBigslime_SetDynamicVtxThrowPlayer;
}

/**
 * Restores bigslime to wide cone after player is thrown
 */
void EnBigslime_SetDynamicVtxThrowPlayer(EnBigslime* this, PlayState* play) {
    f32 invThrowPlayerTimer;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    this->throwPlayerTimer--;
    if (this->throwPlayerTimer > 0) {
        invThrowPlayerTimer = 1.0f / this->throwPlayerTimer;
        for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
            targetVtx = &sBigslimeTargetVtxData[i];
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];

            // loop over x, y, z
            for (j = 0; j < 3; j++) {
                // Linearly interpolate dynamicVtx --> targetVtx
                dynamicVtx->n.ob[j] += TRUNCF_BINANG((targetVtx->n.ob[j] - dynamicVtx->n.ob[j]) * invThrowPlayerTimer);
            }
        }

        this->actor.scale.x = F32_LERPIMP(this->actor.scale.x, 0.2f, invThrowPlayerTimer);
        this->actor.scale.y = F32_LERPIMP(this->actor.scale.y, 0.05f, invThrowPlayerTimer);
        this->actor.scale.z = this->actor.scale.x;
        EnBigslime_CheckVtxWallBoundaries(this);
    }

    if (this->throwPlayerTimer == 0) {
        EnBigslime_SetupRise(this);
    }
}

/**
 * Sets the frozen effect as a seed on the bottom 4 nodes of a sphere
 * The frozen effect is initially set to be dimmer the farther from the initial seed you are
 */
void EnBigslime_SetupFreeze(EnBigslime* this) {
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;
    s32 j;

    this->actor.speed = 0.0f;
    this->freezeTimer = 40;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    } else if ((this->actionFunc == EnBigslime_Rise) && (this->riseCounter < 2)) {
        this->actor.gravity = -2.0f;
    }

    this->bigslimeCollider[0].base.atFlags &= ~AT_ON;
    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;

    // Resets frozen effect alpha to 0
    for (i = 0; i < BIGSLIME_NUM_VTX; i++) {
        dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][i];
        targetVtx = &sBigslimeTargetVtxData[i];
        for (j = 0; j < 3; j++) {
            targetVtx->n.ob[j] = dynamicVtx->n.ob[j];
        }
        targetVtx->n.a = 0;
    }

    // Initalizes frozen effect alpha near bottom of sphere by increasing levels of alpha
    for (i = 0; i < 20; i++) {
        sBigslimeTargetVtxData[i + 138].n.a = 10 * i;
    }

    // Initalizes/seeds frozen effect alpha in bottom 4 nodes in vtx sphere to highest level of alpha
    for (i = 0; i < 4; i++) {
        sBigslimeTargetVtxData[i + 158].n.a = 200;
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 &= ~OC1_NO_PUSH;
    }

    this->actionFuncStored = this->actionFunc;
    this->actionFunc = EnBigslime_Freeze;
}

/**
 * Propogates the frozen effect from the seed at the bottom out through all vertices
 */
void EnBigslime_Freeze(EnBigslime* this, PlayState* play) {
    f32 randFloat;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 vtxIceUpdate;
    s32 vtxIceSeed;
    s32 j;

    if (this->freezeTimer) {
        this->freezeTimer--;
    }

    this->skelAnime.playSpeed = this->freezeTimer / 40.0f;
    vtxIceSeed = this->freezeTimer * 4;
    for (vtxIceUpdate = 0; vtxIceUpdate < 4; vtxIceUpdate++, vtxIceSeed++) {
        if (vtxIceSeed < BIGSLIME_NUM_VTX) {
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][vtxIceSeed];
            targetVtx = &sBigslimeTargetVtxData[vtxIceSeed];
            randFloat = Rand_CenteredFloat(40.0f);
            dynamicVtx->n.ob[0] += TRUNCF_BINANG(randFloat / this->actor.scale.x);
            dynamicVtx->n.ob[1] += TRUNCF_BINANG(randFloat / this->actor.scale.y);
            dynamicVtx->n.ob[2] += TRUNCF_BINANG(randFloat / this->actor.scale.z);
            if (((dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y) < GBT_ROOM_5_MIN_Y) {
                dynamicVtx->n.ob[1] = ((GBT_ROOM_5_MIN_Y - this->actor.world.pos.y) / this->actor.scale.y) - 1.0f;
            }

            for (j = 0; j < 3; j++) {
                targetVtx->n.ob[j] = dynamicVtx->n.ob[j];
            }
        }
    }

    for (vtxIceUpdate = 4; vtxIceUpdate < BIGSLIME_NUM_VTX; vtxIceUpdate++) {
        sBigslimeTargetVtxData[vtxIceUpdate - 4].n.a = sBigslimeTargetVtxData[vtxIceUpdate].n.a;
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->freezeTimer == 0) {
            EnBigslime_BreakIntoMinislime(this, play);
        } else {
            this->bigslimeCollider[0].base.acFlags |= AC_ON;
            EnBigslime_AddIceShardEffect(this, play);
            EnBigslime_SetupSquishFlat(this);
        }
    } else if (this->freezeTimer == 0) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            EnBigslime_SetupFrozenFall(this);
        } else {
            EnBigslime_SetupFrozenGround(this);
        }
    }
}

void EnBigslime_SetupFrozenGround(EnBigslime* this) {
    this->freezeTimer = 80;
    this->bigslimeCollider[0].base.acFlags |= AC_ON;
    this->actionFunc = EnBigslime_FrozenGround;
}

void EnBigslime_FrozenGround(EnBigslime* this, PlayState* play) {
    f32 invFreezerTimer;
    s32 randSign;
    f32 randFloat;

    this->freezeTimer--;
    if (this->freezeTimer == 0) {
        EnBigslime_AddIceShardEffect(this, play);
        EnBigslime_SetTargetVtxFromPreFrozen(this);
    } else if (this->freezeTimer == 40) {
        Math_Vec3f_Copy(&this->frozenPos, &this->actor.world.pos);
    } else if ((this->freezeTimer < 20) || ((this->freezeTimer < 40) && ((this->freezeTimer % 2) != 0))) {
        invFreezerTimer = 1.0f / this->freezeTimer;

        // clang-format off
        randFloat = Rand_ZeroFloat(4.0f * invFreezerTimer);
        randSign = Rand_ZeroOne() < 0.5f ? -1 : 1; \
        this->actor.world.pos.x = randSign * (1.0f + invFreezerTimer + randFloat) + this->frozenPos.x;
        // clang-format on

        randFloat = Rand_ZeroFloat(4.0f * invFreezerTimer);
        randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;
        this->actor.world.pos.z = randSign * (1.0f + invFreezerTimer + randFloat) + this->frozenPos.z;
    }
}

void EnBigslime_SetupMelt(EnBigslime* this) {
    s32 i;

    this->bigslimeCollider[0].base.acFlags &= ~AC_ON;
    for (i = 0; i < 2; i++) {
        sBigslimeTargetVtxData[i].n.a = 0;
    }

    for (i = 0; i < 20; i++) {
        sBigslimeTargetVtxData[i + 2].n.a = 10 * i;
    }

    this->meltCounter = 0;
    this->actionFunc = EnBigslime_Melt;
}

void EnBigslime_Melt(EnBigslime* this, PlayState* play) {
    static Vec3f sIceSmokeVelocity = { 0.0f, 2.0f, 0.0f };
    Vec3f iceSmokePos;
    Vtx* targetVtx;
    Vtx* dynamicVtx;
    s32 i;

    this->meltCounter++;
    if ((this->meltCounter < 70) && ((this->meltCounter % 2) != 0)) {
        dynamicVtx =
            &sBigslimeDynamicVtxData[this->dynamicVtxState][(s32)Rand_ZeroFloat(BIGSLIME_NUM_VTX) % BIGSLIME_NUM_VTX];
        iceSmokePos.x = (dynamicVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
        iceSmokePos.y = (dynamicVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
        iceSmokePos.z = (dynamicVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
        EffectSsIceSmoke_Spawn(play, &iceSmokePos, &sIceSmokeVelocity, &gZeroVec3f, 600);
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
    for (i = 159; i >= 0; i--) {
        sBigslimeTargetVtxData[i + 2].n.a = sBigslimeTargetVtxData[i].n.a;
    }

    if (this->meltCounter == 100) {
        EnBigslime_SetTargetVtxFromPreFrozen(this);
    } else if (this->meltCounter == 50) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }
}

void EnBigslime_SetupFrozenFall(EnBigslime* this) {
    s32 i;

    this->bigslimeCollider[0].base.atFlags |= AT_ON;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actionFunc = EnBigslime_FrozenFall;
}

void EnBigslime_FrozenFall(EnBigslime* this, PlayState* play) {
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.atFlags & AT_HIT) {
            break;
        }
    }

    if (i != BIGSLIME_NUM_RING_FACES) {
        func_800B8D50(play, &this->actor, 7.0f, this->actor.yawTowardsPlayer, 5.0f, 0x10);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnBigslime_BreakIntoMinislime(this, play);
    }
}

void EnBigslime_SetupJumpGekko(EnBigslime* this) {
    Animation_PlayLoop(&this->skelAnime, &gGekkoJumpForwardAnim);
    this->actor.speed = 8.0f;
    this->jumpTimer = 100;
    this->actor.world.rot.y = this->gekkoRot.y;
    this->gekkoYaw = this->actor.yawTowardsPlayer + 0x8000;
    this->actionFunc = EnBigslime_JumpGekko;
}

void EnBigslime_JumpGekko(EnBigslime* this, PlayState* play) {
    s16 yaw;
    s16 yawDiff;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->gekkoCollider.base.acFlags |= AC_ON;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    this->jumpTimer--;
    if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP);
    } else if (Animation_OnFrame(&this->skelAnime, 11.0f)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EV_WALK_WATER);
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        ((this->skelAnime.curFrame > 1.0f) && (this->skelAnime.curFrame < 12.0f))) {
        this->actor.speed = 8.0f;
    } else {
        this->actor.speed = 0.0f;
    }

    if (Math_SmoothStepToS(&this->actor.world.rot.y, this->gekkoYaw, 5, 0x1000, 0x80) == 0) {
        if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 240.0f) {
            yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
            yawDiff = yaw - (s16)(this->actor.yawTowardsPlayer + 0x8000);
            this->gekkoYaw =
                ABS_ALT(yawDiff) < 0x3000 ? yaw : (yawDiff / 2) + (s16)(this->actor.yawTowardsPlayer + 0x8000);
        } else {
            this->gekkoYaw = this->actor.yawTowardsPlayer + 0x8000;
        }
    }

    this->gekkoRot.y = this->actor.world.rot.y;
    if (this->jumpTimer == 0) {
        if (this->formBigslimeTimer == 0) {
            EnBigslime_SetupCutscene(this);
        } else {
            this->formBigslimeTimer--;
            EnBigslime_SetupIdleLookAround(this);
        }
    } else if ((this->actor.xzDistToPlayer > 300.0f) && (this->gekkoCollider.base.ocFlags1 & OC1_HIT)) {
        if ((this->gekkoCollider.base.oc->params == MINISLIME_IDLE) &&
            (this->gekkoCollider.base.oc->id == ACTOR_EN_MINISLIME)) {
            this->gekkoCollider.base.oc->params = MINISLIME_SETUP_GEKKO_THROW;
            this->minislimeToThrow = (EnMinislime*)this->gekkoCollider.base.oc;
            EnBigslime_SetupThrowMinislime(this);
        }
    }
}

void EnBigslime_SetupIdleLookAround(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoNervousIdleAnim);
    this->idleTimer = 60;
    this->actor.speed = 0.0f;
    if (BINANG_SUB(Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), this->gekkoRot.y) > 0) {
        this->gekkoYaw = this->gekkoRot.y + (Rand_Next() >> 20) + 0x2000;
    } else {
        this->gekkoYaw = this->gekkoRot.y - (Rand_Next() >> 20) - 0x2000;
    }
    this->actionFunc = EnBigslime_IdleLookAround;
}

void EnBigslime_IdleLookAround(EnBigslime* this, PlayState* play) {
    s16 yawDiff;

    this->idleTimer--;
    if (this->isAnimFinished) {
        if (Rand_ZeroOne() < 0.25f) {
            Animation_PlayOnce(&this->skelAnime, &gGekkoLookAroundAnim);
        } else {
            Animation_PlayOnce(&this->skelAnime, &gGekkoNervousIdleAnim);
        }
    }

    if ((this->skelAnime.animation == &gGekkoNervousIdleAnim) &&
        Math_ScaledStepToS(&this->gekkoRot.y, this->gekkoYaw, 0x400)) {
        if (BINANG_SUB(Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), this->gekkoRot.y) > 0) {
            this->gekkoYaw = this->gekkoRot.y + (Rand_Next() >> 20) + 0x2000;
        } else {
            this->gekkoYaw = this->gekkoRot.y - (Rand_Next() >> 20) - 0x2000;
        }
    }

    yawDiff = this->actor.yawTowardsPlayer - this->gekkoRot.y;
    if ((this->idleTimer == 0) || ((ABS_ALT(yawDiff) < 0x2800) && (this->actor.xzDistToPlayer < 240.0f))) {
        EnBigslime_SetupIdleNoticePlayer(this);
    }
}

void EnBigslime_SetupIdleNoticePlayer(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoSurpriseJumpAnim);
    this->gekkoYaw = this->gekkoRot.y + 0x8000;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_MID);
    this->actionFunc = EnBigslime_IdleNoticePlayer;
}

void EnBigslime_IdleNoticePlayer(EnBigslime* this, PlayState* play) {
    s16* yaw = &this->gekkoRot.y;

    if (this->skelAnime.curFrame > 10.0f) {
        Math_ScaledStepToS(yaw, this->gekkoYaw, 0x800);
    }
    if (this->isAnimFinished) {
        EnBigslime_SetupJumpGekko(this);
    }
}

void EnBigslime_SetupThrowMinislime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoWindupPunchAnim);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_HOLD_SLIME);
    this->actor.speed = 0.0f;
    this->actionFunc = EnBigslime_ThrowMinislime;
}

void EnBigslime_ThrowMinislime(EnBigslime* this, PlayState* play) {
    s16 jumpTimerStored;

    Math_ScaledStepToS(&this->gekkoRot.y, this->actor.yawTowardsPlayer, 0x300);
    if (Animation_OnFrame(&this->skelAnime, 27.0f) &&
        (this->minislimeToThrow->actor.params == MINISLIME_SETUP_GEKKO_THROW)) {
        EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_THROW_SLIME);
        this->minislimeToThrow->actor.params = MINISLIME_GEKKO_THROW;
    }
    if (this->isAnimFinished) {
        jumpTimerStored = this->jumpTimer; // Stores jumpTimer so it doesn't get overwritten back to 100
        EnBigslime_SetupJumpGekko(this);
        this->jumpTimer = jumpTimerStored;
    }
}

void EnBigslime_SetupDamageGekko(EnBigslime* this, s32 isNotFrozen) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gGekkoDamagedAnim, -3.0f);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->damageSpinTimer = 20;
    this->actor.speed = 10.0f;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;
    if (isNotFrozen) {
        func_800BE504(&this->actor, &this->gekkoCollider);
    }

    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DAMAGE);
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
    }

    this->actionFunc = EnBigslime_DamageGekko;
}

/**
 * Spins Gekko around as it takes damage
 */
void EnBigslime_DamageGekko(EnBigslime* this, PlayState* play) {
    s32 damageSpinTimer;

    this->damageSpinTimer--;
    damageSpinTimer = CLAMP_MAX(this->damageSpinTimer, 10);
    this->gekkoRot.y += 0x300 * damageSpinTimer;
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (this->damageSpinTimer == 0) {
        EnBigslime_SetupCutscene(this);
    }
}

void EnBigslime_SetupStunGekko(EnBigslime* this) {
    if ((this->skelAnime.animation == &gGekkoJumpForwardAnim) ||
        (this->skelAnime.animation == &gGekkoSurpriseJumpAnim)) {
        this->skelAnime.curFrame = 1.0f;
    }
    this->actionFunc = EnBigslime_StunGekko;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speed = 0.0f;
}

void EnBigslime_StunGekko(EnBigslime* this, PlayState* play) {
    this->stunTimer--;
    if (this->stunTimer == 0) {
        if (this->gekkoDrawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            EnBigslime_GekkoThaw(this, play);
            EnBigslime_SetupDamageGekko(this, false);
        } else {
            this->gekkoCollider.base.acFlags &= ~AC_ON;
            EnBigslime_SetupCutscene(this);
        }
    }
}

void EnBigslime_SetupCutsceneFormBigslime(EnBigslime* this) {
    Animation_PlayOnce(&this->skelAnime, &gGekkoJumpUpAnim);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->actor.world.rot.x = -Actor_WorldPitchTowardPoint(&this->actor, &this->actor.home.pos);
    this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->actionFunc = EnBigslime_CutsceneFormBigslime;
    this->actor.speed = 0.0f;
}

void EnBigslime_CutsceneFormBigslime(EnBigslime* this, PlayState* play) {
    EnBigslime_UpdateCameraFormingBigslime(this, play);
    Math_ScaledStepToS(&this->gekkoRot.y, this->actor.world.rot.y, 0x800);
    if (Animation_OnFrame(&this->skelAnime, 18.0f)) {
        EnBigslime_SetupFormBigslime(this);
    }
}

void EnBigslime_SetupFormBigslime(EnBigslime* this) {
    s32 i;

    this->actor.gravity = 0.0f;
    this->gekkoRot.x = 0x4000 - this->actor.world.rot.x;
    this->formBigslimeTimer = 0;
    this->wavySurfaceTimer = 0;
    this->actor.speed = 25.0f;
    this->gekkoRot.y = this->actor.world.rot.y;
    this->formBigslimeCutsceneTimer = 2;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_FORM_BIGSLIME;
    }

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actor.update = EnBigslime_UpdateBigslime;
    this->actor.draw = EnBigslime_DrawBigslime;
    Actor_SetScale(&this->actor, 0.0f);
    this->vtxScaleX = 0.0f;
    this->vtxScaleZ = 0.0f;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_JUMP_ABOVE);
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_UTSUBO_APPEAR_TRG);
    this->actionFunc = EnBigslime_FormBigslime;
}

void EnBigslime_FormBigslime(EnBigslime* this, PlayState* play) {
    f32 xzScale;
    f32 yScaleFactor;
    s32 i;

    EnBigslime_UpdateWavySurface(this);
    EnBigslime_UpdateCameraFormingBigslime(this, play);
    if (this->formBigslimeCutsceneTimer < 0) {
        Math_ScaledStepToS(&this->gekkoRot.x, 0, 0x400);
    } else if (this->actor.world.pos.y > (GBT_ROOM_5_MAX_Y - 100.0f)) {
        this->gekkoRot.x -= 0x4000;
        Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        Animation_PlayLoop(&this->skelAnime, &gGekkoSwimForwardAnim);
        this->formBigslimeCutsceneTimer--;
        Actor_PlaySfx(&this->actor, NA_SE_EN_B_SLIME_COMBINE);
    } else if (this->isAnimFinished) {
        this->formBigslimeCutsceneTimer--;
        if (this->formBigslimeCutsceneTimer == 0) {
            Animation_PlayLoop(&this->skelAnime, &gGekkoSwimUpAnim);
        }
    }

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        if (this->minislime[i]->actor.params == MINISLIME_SETUP_DISAPPEAR) {
            this->minislime[i]->actor.params = MINISLIME_DISAPPEAR;
            this->minislimeCounter++;
        }
    }

    if (this->minislimeCounter > 0) {
        yScaleFactor = sqrtf(this->minislimeCounter * (2.0f / 30.0f)) * 0.6f + 0.4f;
        xzScale = 0.15f * yScaleFactor;
        this->actor.scale.x = xzScale;
        this->actor.scale.y = 0.079f * yScaleFactor;
        this->actor.scale.z = xzScale;
    }

    if (this->minislimeCounter == MINISLIME_NUM_SPAWN) {
        this->minislimeState = MINISLIME_INACTIVE_STATE;
        this->actor.hintId = TATL_HINT_ID_MAD_JELLY;
        EnBigslime_SetupMoveOnCeiling(this);
    }
}

void EnBigslime_SetupCutsceneDefeat(EnBigslime* this, PlayState* play) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    s32 i;
    s16 yawOffset;
    AnimationHeader* anim = ResourceMgr_LoadAnimByName(gGekkoDamagedAnim);

    Animation_Change(&this->skelAnime, anim, 0.5f, 0.0f, Animation_GetLastFrame(&anim->common), ANIMMODE_ONCE_INTERP,
                     0.0f);
    this->gekkoCollider.base.acFlags &= ~AC_ON;
    this->defeatTimer = 60;
    this->actor.speed = 10.0f;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 0.0f;
    EnBigslime_GekkoSfxOutsideBigslime(this, NA_SE_EN_FROG_DEAD);

    // Points the camera at the defeated Gekko
    subCamAt.x = this->actor.world.pos.x;
    subCamAt.y = this->actor.world.pos.y + 40.0f;
    subCamAt.z = this->actor.world.pos.z;

    if (BINANG_SUB(Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos), this->actor.world.rot.y) > 0) {
        yawOffset = this->actor.world.rot.y + 0x4000;
    } else {
        yawOffset = this->actor.world.rot.y - 0x4000;
    }

    // Moves the camera to the side of player and Gekko
    subCamEye.x = (Math_SinS(yawOffset) * 250.0f) + subCamAt.x;
    subCamEye.y = subCamAt.y + 60.0f;
    subCamEye.z = (Math_CosS(yawOffset) * 250.0f) + subCamAt.z;
    Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_DEFEAT_IDLE;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    EnBigslime_GekkoThaw(this, play);
    this->actionFunc = EnBigslime_CutsceneDefeat;
}

void EnBigslime_CutsceneDefeat(EnBigslime* this, PlayState* play) {
    s32 defeatTimer;
    Camera* subCam;
    Vec3f subCamAt;

    this->defeatTimer--;
    defeatTimer = CLAMP_MAX(this->defeatTimer, 10);
    this->gekkoRot.y += 0x300 * defeatTimer;
    if (Math_StepToF(&this->actor.speed, 0.0f, 0.5f)) {
        EnBigslime_SetupGekkoDespawn(this, play);
    } else {
        // Continue for the camera to follow Gekko as it spins in defeat
        subCam = Play_GetCamera(play, this->subCamId);
        subCamAt.x = this->actor.world.pos.x;
        subCamAt.y = this->actor.world.pos.y + 40.0f;
        subCamAt.z = this->actor.world.pos.z;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCam->eye);
    }
}

void EnBigslime_SetupGekkoDespawn(EnBigslime* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    f32 magnitude;
    f32 invMagnitude;

    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &this->subCamDistToFrog);
    magnitude = Math3D_Vec3fMagnitude(&this->subCamDistToFrog);
    if (magnitude > 78.0f) {
        invMagnitude = 1.0f / magnitude;
        magnitude -= 77.0f;
        magnitude /= 20.0f;
        Math_Vec3f_Scale(&this->subCamDistToFrog, magnitude * invMagnitude);
    } else {
        Math_Vec3f_Copy(&this->subCamDistToFrog, &gZeroVec3f);
    }

    this->shockwaveAlpha = 0;
    this->despawnTimer = 20;
    this->actionFunc = EnBigslime_GekkoDespawn;
}

void EnBigslime_GekkoDespawn(EnBigslime* this, PlayState* play) {
    Vec3f subCamEye;
    Vec3f subCamAt;
    Camera* subCam;

    this->despawnTimer--;
    this->gekkoScale = this->despawnTimer * 0.00035000002f;
    if (this->despawnTimer == 0) {
        EnBigslime_SetupFrogSpawn(this, play);
    } else {
        subCam = Play_GetCamera(play, this->subCamId);
        Math_Vec3f_Copy(&subCamAt, &subCam->at);
        Math_Vec3f_Diff(&subCam->eye, &this->subCamDistToFrog, &subCamEye);
        subCamEye.y -= 1.8f;
        subCamAt.y -= 1.7f;
        Play_SetCameraAtEye(play, this->subCamId, &subCamAt, &subCamEye);
    }
}

void EnBigslime_SetupFrogSpawn(EnBigslime* this, PlayState* play) {
    static Color_RGBA8 sDustPrimColor = { 250, 250, 250, 255 };
    static Color_RGBA8 sDustEnvColor = { 180, 180, 180, 255 };
    static Vec3f sHahenAccel = { 0.0f, -0.5f, 0.0f };
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f* worldPos;
    Vec3f dustPos;
    Vec3f hahenVel;
    s16 yaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
    s16 yawReverse = yaw + 0x8000;
    s32 i;

    this->gekkoCollider.base.ocFlags1 &= ~OC1_ON;

    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_MINIFROG, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0, yawReverse, 0, this->actor.params);

    dustPos.x = (Math_SinS(yawReverse) * 20.0f) + this->actor.world.pos.x;
    dustPos.y = this->actor.world.pos.y + 20.0f;
    worldPos = &this->actor.world.pos;
    dustPos.z = (Math_CosS(yawReverse) * 20.0f) + this->actor.world.pos.z;

    SoundSource_PlaySfxAtFixedWorldPos(play, worldPos, 40, NA_SE_EN_NPC_APPEAR);

    // dust cloud where the red frog appears
    func_800B0DE0(play, &dustPos, &gZeroVec3f, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 500, 50);

    for (i = 0; i < 25; i++) {
        hahenVel.x = Rand_CenteredFloat(5.0f);
        hahenVel.y = Rand_ZeroFloat(3.0f) + 4.0f;
        hahenVel.z = Rand_CenteredFloat(5.0f);
        EffectSsHahen_Spawn(play, worldPos, &hahenVel, &sHahenAccel, 0, Rand_S16Offset(12, 3), HAHEN_OBJECT_DEFAULT, 10,
                            0);
    }

    this->spawnFrogTimer = 40;
    Math_Vec3f_Diff(&subCam->eye, &subCam->at, &this->subCamDistToFrog);
    this->actionFunc = EnBigslime_FrogSpawn;
}

void EnBigslime_FrogSpawn(EnBigslime* this, PlayState* play) {
    Camera* subCam = Play_GetCamera(play, this->subCamId);
    Vec3f subCamEye;
    f32 subCamZoom;

    this->spawnFrogTimer--;

    // Zoom the camera in and out at the newly spawned red frog
    subCamZoom =
        Math_SinF(this->spawnFrogTimer * (M_PIf / 5)) * ((0.04f * (this->spawnFrogTimer * 0.1f)) + 0.02f) + 1.0f;
    subCamEye.x = subCam->at.x + (this->subCamDistToFrog.x * subCamZoom);
    subCamEye.z = subCam->at.z + (this->subCamDistToFrog.z * subCamZoom);
    subCamEye.y = subCam->at.y + (this->subCamDistToFrog.y * subCamZoom);
    Play_SetCameraAtEye(play, this->subCamId, &subCam->at, &subCamEye);

    if (this->spawnFrogTimer == 0) {
        EnBigslime_EndCutscene(this, play);
        EnBigslime_SetupDespawn(this);
    }
}

void EnBigslime_SetupDespawn(EnBigslime* this) {
    s32 i;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        this->minislime[i]->actor.params = MINISLIME_DEFEAT_MELT;
    }

    this->isDespawned = false;
    this->gekkoCollider.base.ocFlags1 &= ~OC1_ON;
    this->actionFunc = EnBigslime_Despawn;
}

void EnBigslime_Despawn(EnBigslime* this, PlayState* play) {
    s32 i;
    s32 counter = 0;

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        if (this->minislime[i]->actor.shape.shadowAlpha == 0) {
            counter++;
        }
    }

    if (!this->isDespawned) {
        Flags_SetClearTemp(play, play->roomCtx.curRoom.num);
        this->isDespawned = true;
    }

    if (counter == MINISLIME_NUM_SPAWN) {
        for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
            this->minislime[i]->actor.params = MINISLIME_DESPAWN;
        }

        Actor_Kill(&this->actor);
    }
}

void EnBigslime_SetupInitEntrance(EnBigslime* this) {
    this->actionFunc = EnBigslime_InitEntrance;
}

void EnBigslime_InitEntrance(EnBigslime* this, PlayState* play) {
    if (play->roomCtx.prevRoom.num == -1) {
        EnBigslime_SetupCutscene(this);
    }
}

void EnBigslime_SetupCutscene(EnBigslime* this) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR) {
        CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
    }

    if (this->actor.colChkInfo.health == 0) {
        this->csId = this->actor.csId;
    }

    CutsceneManager_Queue(this->csId);
    this->actionFuncStored = this->actionFunc;
    this->actionFunc = EnBigslime_PlayCutscene;
    this->actor.speed = 0.0f;
}

void EnBigslime_PlayCutscene(EnBigslime* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_DOOR) {
        CutsceneManager_Stop(CS_ID_GLOBAL_DOOR);
        CutsceneManager_Queue(this->csId);
    } else if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, &this->actor);
        if (this->actionFuncStored != EnBigslime_SquishFlat) {
            Player_SetCsAction(play, &this->actor, PLAYER_CSACTION_WAIT);
        }

        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->csId);
        if (this->actor.colChkInfo.health == 0) {
            EnBigslime_SetupCutsceneDefeat(this, play);
        } else if ((this->actionFuncStored == EnBigslime_DamageGekko) ||
                   (this->actionFuncStored == EnBigslime_JumpGekko) ||
                   (this->actionFuncStored == EnBigslime_StunGekko)) {
            EnBigslime_SetupCutsceneFormBigslime(this);
        } else if (this->actionFuncStored == EnBigslime_SquishFlat) {
            EnBigslime_SetupCutsceneGrabPlayer(this, play);
        } else {
            EnBigslime_SetupCutsceneStartBattle(this, play);
        }
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

void EnBigslime_ApplyDamageEffectBigslime(EnBigslime* this, PlayState* play) {
    s32 i;

    for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
        if (this->bigslimeCollider[i].base.acFlags & AC_HIT) {
            this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
            if (this->actionFunc == EnBigslime_FrozenGround) {
                if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_BREAK_ICE) {
                    EnBigslime_BreakIntoMinislime(this, play);
                    break;
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                    EnBigslime_SetPlayerParams(this, play);
                    EnBigslime_SetupMelt(this);
                    break;
                }
            } else {
                if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                    EnMinislime* minislime;

                    play->envCtx.lightSettingOverride = 2;
                    EnBigslime_SetPlayerParams(this, play);
                    this->rotation = 0;
                    EnBigslime_SetupFreeze(this);
                    minislime = (EnMinislime*)SubS_FindActor(play, NULL, ACTORCAT_ITEMACTION, ACTOR_ARROW_ICE);
                    if (minislime != NULL) {
                        minislime->shakeRefPos.z = -100.0f;
                    }
                    break;
                }

                if ((this->actor.colChkInfo.damageEffect != BIGSLIME_DMGEFF_STUN) && (this->itemDropTimer == 0)) {
                    f32 randFloat = Rand_ZeroOne();

                    if (randFloat < 0.15f) {
                        Item_DropCollectible(play, &this->actor.world.pos, ITEM00_ARROWS_10);
                    } else if (randFloat < 0.3f) {
                        Item_DropCollectible(play, &this->actor.world.pos, ITEM00_MAGIC_JAR_SMALL);
                    }
                    this->itemDropTimer = 40;
                }
            }
        }
    }

    for (; i < BIGSLIME_NUM_RING_FACES; i++) {
        this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
    }
}

void EnBigslime_ApplyDamageEffectGekko(EnBigslime* this, PlayState* play) {
    if (this->gekkoCollider.base.acFlags & AC_HIT) {
        this->gekkoCollider.base.acFlags &= ~AC_HIT;
        if ((this->gekkoDrawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->gekkoCollider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            EnBigslime_EndThrowMinislime(this);
            if (this->actor.colChkInfo.damageEffect != BIGSLIME_DMGEFF_HOOKSHOT) {
                if (Actor_ApplyDamage(&this->actor) == 0) {
                    func_800BE504(&this->actor, &this->gekkoCollider);
                    Audio_RestorePrevBgm();
                    Enemy_StartFinishingBlow(play, &this->actor);
                    this->gekkoCollider.base.acFlags &= ~AC_ON;
                    EnBigslime_GekkoThaw(this, play);
                    if ((this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) ||
                        (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_LIGHT)) {
                        this->gekkoDrawDmgEffAlpha = 4.0f;
                        this->gekkoDrawDmgEffScale = 0.75f;
                        if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                            this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                        } else {
                            this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.x,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.y,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                        CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        }
                    } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                        EnBigslime_GekkoFreeze(this);
                    }
                    EnBigslime_SetupCutscene(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ELECTRIC_STUN) {
                    this->stunTimer = 40;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                    this->gekkoDrawDmgEffScale = 0.75f;
                    this->gekkoDrawDmgEffAlpha = 2.0f;
                    EnBigslime_SetupStunGekko(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_STUN ||
                           this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_DEKU_STUN) {
                    this->stunTimer = 40;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    EnBigslime_SetupStunGekko(this);
                } else if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_ICE) {
                    EnBigslime_GekkoFreeze(this);
                    func_800BE504(&this->actor, &this->gekkoCollider);
                    EnBigslime_SetupStunGekko(this);
                } else {
                    EnBigslime_GekkoThaw(this, play);
                    if ((this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) ||
                        (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_LIGHT)) {
                        this->gekkoDrawDmgEffAlpha = 3.0f;
                        this->gekkoDrawDmgEffScale = 0.75f;
                        if (this->actor.colChkInfo.damageEffect == BIGSLIME_DMGEFF_FIRE) {
                            this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                        } else {
                            this->gekkoDrawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.x,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.y,
                                        this->gekkoCollider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                        CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        }
                    }
                    EnBigslime_SetupDamageGekko(this, true);
                }
            }
        }
    }
}

/**
 * Adds ice shard effects and calls EnBigslime_InitShockwave
 */
void EnBigslime_AddIceShardEffect(EnBigslime* this, PlayState* play) {
    Vtx* targetVtx = &sBigslimeTargetVtxData[0];
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;
    f32 randFloat;
    s16 randPitch; // Elevation Angle
    s16 vtxX;
    s16 vtxZ;
    f32 xzDist;

    for (i = 0; i < BIGSLIME_NUM_VTX; i++, targetVtx++) {
        if (targetVtx->n.a > 0) {
            iceShardEffect = &this->iceShardEffect[i];
            iceShardEffect->pos.x = (targetVtx->n.ob[0] * this->actor.scale.x) + this->actor.world.pos.x;
            iceShardEffect->pos.y = (targetVtx->n.ob[1] * this->actor.scale.y) + this->actor.world.pos.y;
            iceShardEffect->pos.z = (targetVtx->n.ob[2] * this->actor.scale.z) + this->actor.world.pos.z;
            iceShardEffect->rot.x = (s32)Rand_Next() >> 0x10;
            iceShardEffect->rot.y = (s32)Rand_Next() >> 0x10;
            iceShardEffect->rot.z = (s32)Rand_Next() >> 0x10;
            iceShardEffect->isEnabled = true;
            randPitch = Rand_S16Offset(0x1000, 0x3000);
            vtxZ = targetVtx->n.ob[2];
            vtxX = targetVtx->n.ob[0];
            xzDist = sqrtf(SQ(vtxZ) + SQ(vtxX));
            randFloat = Rand_ZeroFloat(5.0f) + 14.0f;
            if (xzDist > 1.0f) {
                xzDist = Math_CosS(randPitch) * randFloat / xzDist;
            } else {
                xzDist = Math_CosS(randPitch) * randFloat;
            }

            iceShardEffect->velocity.x = targetVtx->n.ob[0] * xzDist;
            iceShardEffect->velocity.y = Math_SinS(randPitch) * randFloat;
            iceShardEffect->velocity.z = targetVtx->n.ob[2] * xzDist;
            iceShardEffect->scale = 0.001f * (Rand_ZeroFloat(6.0f) + 2.0f);
        }
    }

    Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_BROKEN);
    EnBigslime_InitShockwave(this, play);
}

/**
 * Updates ice shard effects, shockwave effects, and actor damage draw effects
 */
void EnBigslime_UpdateEffects(EnBigslime* this) {
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;

    // Update ice shards
    for (i = 0; i < BIGSLIME_NUM_ICE_SHARD; i++) {
        iceShardEffect = &this->iceShardEffect[i];
        if (iceShardEffect->isEnabled > false) {
            iceShardEffect->velocity.y += -1.0f;
            Math_Vec3f_Sum(&iceShardEffect->pos, &iceShardEffect->velocity, &iceShardEffect->pos);
            if (iceShardEffect->pos.y < (GBT_ROOM_5_MIN_Y - 20.0f)) {
                iceShardEffect->isEnabled = false;
            }
            iceShardEffect->rot.x += (s16)((Rand_Next() >> 0x17) + 0x700);
            iceShardEffect->rot.y += (s16)((Rand_Next() >> 0x17) + 0x900);
            iceShardEffect->rot.z += (s16)((Rand_Next() >> 0x17) + 0xB00);
        }
    }

    // update shockwave
    if (this->shockwaveAlpha > 0) {
        this->shockwaveAlpha = (this->shockwaveAlpha - 10 < 0) ? 0 : (this->shockwaveAlpha - 10);
        this->shockwaveScale += 0.0013f;
    }

    // update actor damage draw effects
    if (this->gekkoDrawDmgEffAlpha > 0.0f) {
        if ((this->gekkoDrawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (this->actionFunc != EnBigslime_PlayCutscene)) {
            Math_StepToF(&this->gekkoDrawDmgEffAlpha, 0.0f, 0.05f);
            this->gekkoDrawDmgEffScale = 0.375f * (this->gekkoDrawDmgEffAlpha + 1.0f);
            this->gekkoDrawDmgEffScale = CLAMP_MAX(this->gekkoDrawDmgEffScale, 0.75f);
        } else if (!Math_StepToF(&this->gekkoDrawDmgEffFrozenSteamScale, 0.75f, 0.01875f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

void EnBigslime_UpdateBigslime(Actor* thisx, PlayState* play) {
    EnBigslime* this = (EnBigslime*)thisx;
    s32 i;
    Vec3f vtxMax;
    Vec3f vtxMin;

    if (play->envCtx.lightSettingOverride == 3) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }

    Audio_SetSfxUnderwaterReverb(true);
    this->dynamicVtxState ^= 1;
    EnBigslime_DynamicVtxCopyState(this);
    this->isAnimFinished = SkelAnime_Update(&this->skelAnime);
    if (this->actionFunc != EnBigslime_PlayCutscene) {
        EnBigslime_ApplyDamageEffectBigslime(this, play);
    } else {
        for (i = 0; i < BIGSLIME_NUM_RING_FACES; i++) {
            this->bigslimeCollider[i].base.acFlags &= ~AC_HIT;
        }
    }

    this->actionFunc(this, play);

    if (this->actionFunc != EnBigslime_FormBigslime) {
        Actor_MoveWithGravity(&this->actor);
    } else {
        Actor_MoveWithoutGravity(&this->actor);
    }

    if (this->actionFunc != EnBigslime_JumpGekko) {
        EnBigslime_GetMaxMinVertices(this, &vtxMax, &vtxMin);
        EnBigslime_UpdateScale(this, &vtxMax, &vtxMin);
        EnBigslime_CheckRoomBoundaries(this, &vtxMax, &vtxMin);
        EnBigslime_UpdateSurfaceNorm(this);
        EnBigslime_UpdateBigslimeCollider(this, play);
    }

    EnBigslime_UpdateEffects(this);

    if (this->itemDropTimer > 0) {
        this->itemDropTimer--;
    }
}

void EnBigslime_UpdateGekko(Actor* thisx, PlayState* play) {
    static s32 isGekkoOnGround = false;
    EnBigslime* this = (EnBigslime*)thisx;
    Player* player;
    s32 pad;

    if (play->envCtx.lightSettingOverride == 3) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }

    Audio_SetSfxUnderwaterReverb(false);
    this->isAnimFinished = SkelAnime_Update(&this->skelAnime);
    if (this->actionFunc != EnBigslime_PlayCutscene) {
        EnBigslime_ApplyDamageEffectGekko(this, play);
    } else {
        this->gekkoCollider.base.acFlags &= ~AC_HIT;
    }

    this->actionFunc(this, play);
    if (this->actionFunc != EnBigslime_FormBigslime) {
        Actor_MoveWithGravity(&this->actor);
    } else {
        Actor_MoveWithoutGravity(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 40.0f, 80.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    this->gekkoCollider.dim.pos.x = TRUNCF_BINANG(this->actor.world.pos.x);
    this->gekkoCollider.dim.pos.z = TRUNCF_BINANG(this->actor.world.pos.z);
    if (this->gekkoCollider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->gekkoCollider.base);
    }

    if ((this->actor.update == EnBigslime_UpdateGekko) && (this->gekkoCollider.base.ocFlags1 & OC1_ON)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->gekkoCollider.base);
    }

    EnBigslime_UpdateEffects(this);

    if ((this->actionFunc != EnBigslime_StunGekko) &&
        (this->gekkoCollider.dim.pos.y < TRUNCF_BINANG(3.0f + GBT_ROOM_5_MIN_Y))) {
        Vec3f vtxNorm;

        if (((play->gameplayFrames % 4) == 0) || !isGekkoOnGround) {
            player = GET_PLAYER(play);
            vtxNorm.x = this->actor.world.pos.x;
            vtxNorm.z = this->actor.world.pos.z;
            vtxNorm.y = player->actor.world.pos.y + player->actor.depthInWater;
            EffectSsGRipple_Spawn(play, &vtxNorm, 150, 550, 0);
            isGekkoOnGround = true;
        }
    } else {
        isGekkoOnGround = false;
    }
}

/**
 * Related to transforming and drawing shadows
 */
void EnBigslime_SetSysMatrix(Vec3f* pos, PlayState* play, Gfx* shadowDList, f32 scaleX, f32 scalez, f32 scaleY,
                             s16 rotation, f32 alpha) {
    f32 yDistMinY;
    f32 zx;
    MtxF* sysMatrix = Matrix_GetCurrent();

    yDistMinY = pos->y - scaleY - GBT_ROOM_5_MIN_Y;
    yDistMinY = CLAMP((yDistMinY), 0.0f, (GBT_ROOM_5_CENTER_Y - GBT_ROOM_5_MIN_Y) / 2);
    zx = 1.0f - (yDistMinY * (1.0f / 1550.0f));

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP, SETUPDL_44);
    sysMatrix->xx = zx;
    sysMatrix->yy = 1.0f;
    sysMatrix->zz = zx;
    sysMatrix->zx = sysMatrix->yx = 0.0f;
    sysMatrix->zy = sysMatrix->xy = 0.0f;
    sysMatrix->yz = sysMatrix->xz = 0.0f;
    sysMatrix->xw = pos->x;
    sysMatrix->yw = GBT_ROOM_5_MIN_Y;
    sysMatrix->zw = pos->z;
    sysMatrix->wx = sysMatrix->wy = sysMatrix->wz = 0.0f;
    sysMatrix->ww = 1.0f;

    Matrix_RotateYS(rotation, MTXMODE_APPLY);
    Matrix_Scale(scaleX, 1.0f, scalez, MTXMODE_APPLY);
    if (shadowDList != gBigslimeShadowDL) {
        gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                          COMBINED);
    }

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (u8)(alpha * zx));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, shadowDList);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigslime_DrawMinislime(EnBigslime* this, PlayState* play2) {
    EnMinislime* minislime;
    PlayState* play = play2;
    s32 pad;
    s32 currIndex;
    s32 i;
    Lights* lights;
    s32 indices[MINISLIME_NUM_SPAWN];

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        indices[i] = i;
    }

    // Insertion sorting algorithm based on z projected Pos
    for (i = 0; i < MINISLIME_NUM_SPAWN - 1; i++) {
        if (this->minislime[indices[i]]->actor.projectedPos.z < this->minislime[indices[i + 1]]->actor.projectedPos.z) {
            currIndex = indices[i];
            indices[i] = indices[i + 1];
            indices[i + 1] = currIndex;
            if (i != 0) {
                i -= 2;
            }
        }
    }

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < MINISLIME_NUM_SPAWN; i++) {
        minislime = this->minislime[indices[i]];
        FrameInterpolation_RecordOpenChild(minislime, indices[i]);
        lights = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);
        Lights_BindAll(lights, play->lightCtx.listHead, &minislime->actor.world.pos, play);
        Lights_Draw(lights, play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        func_800B8118(&minislime->actor, play, 0);
        Matrix_SetTranslateRotateYXZ(minislime->actor.world.pos.x, minislime->actor.world.pos.y,
                                     minislime->actor.world.pos.z, &minislime->actor.shape.rot);
        Matrix_Scale(minislime->actor.scale.x, minislime->actor.scale.y, minislime->actor.scale.z, MTXMODE_APPLY);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, minislime->actor.shape.shadowAlpha);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, &gMinislimeNormalDL);
        if (minislime->frozenAlpha > 0) {
            Matrix_Translate(0.0f, (0.1f - minislime->frozenScale) * -4000.0f, 0.0f, MTXMODE_APPLY);
            Matrix_Scale(0.1f, minislime->frozenScale, 0.1f, MTXMODE_APPLY);
            AnimatedMat_Draw(play, this->minislimeFrozenTexAnim);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, minislime->frozenAlpha);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, &gMinislimeFrozenDL);
        }

        EnBigslime_SetSysMatrix(&minislime->actor.world.pos, play, gBigslimeShadowDL,
                                (minislime->actor.scale.x * 0.4f) * 0.1f, (minislime->actor.scale.z * 0.4f) * 0.1f,
                                minislime->actor.scale.y * 400.0f, minislime->actor.shape.rot.y,
                                minislime->actor.shape.shadowAlpha * (175.0f / 255.0f));
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBigslime_DrawBigslime(Actor* thisx, PlayState* play) {
    // 28 equidistance-spaced vtx Points (uniformally over the sphere)
    static EnBigslimeBubbles bubblesInfo[] = {
        { 0, 0.3f },    { 6, 0.1f },    { 12, 0.45f }, { 18, 0.5f },   { 24, 0.6f },   { 30, 0.2f },   { 36, 0.4f },
        { 42, 0.25f },  { 48, 0.35f },  { 54, 0.65f }, { 60, 0.25f },  { 66, 0.55f },  { 72, 0.3f },   { 78, 0.1f },
        { 84, 0.45f },  { 90, 0.5f },   { 96, 0.6f },  { 102, 0.2f },  { 108, 0.4f },  { 114, 0.15f }, { 120, 0.35f },
        { 126, 0.65f }, { 132, 0.25f }, { 138, 0.3f }, { 144, 0.15f }, { 150, 0.45f }, { 156, 0.3f },  { 161, 0.25f },
    };
    EnBigslime* this = (EnBigslime*)thisx;
    EnBigslimeBubbles* bubblesInfoPtr;
    Vtx* dynamicVtx;
    MtxF* billboardMtxF;
    s32 i;

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);

    OPEN_DISPS(play->state.gfxCtx);

    // Draw Bigslime
    gSPSegment(POLY_XLU_DISP++, 0x09, sBigslimeDynamicVtxData[this->dynamicVtxState]);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gBigslimeNormalMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, gBigslimeModelDL);

    // Draw frozen Bigslime
    if ((this->actionFunc == EnBigslime_Freeze) || (this->actionFunc == EnBigslime_FrozenGround) ||
        (this->actionFunc == EnBigslime_FrozenFall) || (this->actionFunc == EnBigslime_Melt)) {
        AnimatedMat_Draw(play, this->bigslimeFrozenTexAnim);
        gSPSegment(POLY_XLU_DISP++, 0x09, sBigslimeTargetVtxData);
        gSPDisplayList(POLY_XLU_DISP++, gBigslimeFrozenMaterialDL);
        gSPDisplayList(POLY_XLU_DISP++, gBigslimeModelDL);
    }

    // Draw bubbles inside Bigslime
    if (this->actor.scale.x > 0.0f) {
        Matrix_Put(&play->billboardMtxF);
        Matrix_Scale(0.0050000003f, 0.0050000003f, 0.0050000003f, MTXMODE_APPLY);
        billboardMtxF = Matrix_GetCurrent();

        for (i = 0; i < 28; i++) {
            bubblesInfoPtr = &bubblesInfo[i];
            dynamicVtx = &sBigslimeDynamicVtxData[this->dynamicVtxState][bubblesInfoPtr->v];
            billboardMtxF->xw =
                dynamicVtx->n.ob[0] * this->actor.scale.x * bubblesInfoPtr->scaleVtx + this->actor.world.pos.x;
            billboardMtxF->yw =
                dynamicVtx->n.ob[1] * this->actor.scale.y * bubblesInfoPtr->scaleVtx + this->actor.world.pos.y;
            billboardMtxF->zw =
                dynamicVtx->n.ob[2] * this->actor.scale.z * bubblesInfoPtr->scaleVtx + this->actor.world.pos.z;
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gBigslimeBubbleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);

    EnBigslime_SetSysMatrix(&this->actor.world.pos, play, gBigslimeShadowDL, this->vtxScaleX, this->vtxScaleZ,
                            this->actor.scale.y * BIGSLIME_RADIUS_F, this->rotation, 175.0f);
    EnBigslime_DrawGekko(&this->actor, play);
}

static s8 sLimbToBodyParts[GEKKO_LIMB_MAX] = {
    BODYPART_NONE,                  // GEKKO_LIMB_NONE
    BODYPART_NONE,                  // GEKKO_LIMB_ROOT
    GEKKO_BODYPART_WAIST,           // GEKKO_LIMB_WAIST
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_THIGH
    GEKKO_BODYPART_LEFT_SHIN,       // GEKKO_LIMB_LEFT_SHIN
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_ANKLE
    GEKKO_BODYPART_LEFT_FOOT,       // GEKKO_LIMB_LEFT_FOOT
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_THIGH
    GEKKO_BODYPART_RIGHT_SHIN,      // GEKKO_LIMB_RIGHT_SHIN
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_ANKLE
    GEKKO_BODYPART_RIGHT_FOOT,      // GEKKO_LIMB_RIGHT_FOOT
    BODYPART_NONE,                  // GEKKO_LIMB_TORSO
    GEKKO_BODYPART_LEFT_UPPER_ARM,  // GEKKO_LIMB_LEFT_UPPER_ARM
    GEKKO_BODYPART_LEFT_FOREARM,    // GEKKO_LIMB_LEFT_FOREARM
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_WRIST
    GEKKO_BODYPART_LEFT_HAND,       // GEKKO_LIMB_LEFT_HAND
    GEKKO_BODYPART_RIGHT_UPPER_ARM, // GEKKO_LIMB_RIGHT_UPPER_ARM
    GEKKO_BODYPART_RIGHT_FOREARM,   // GEKKO_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_WRIST
    GEKKO_BODYPART_RIGHT_HAND,      // GEKKO_LIMB_RIGHT_HAND
    BODYPART_NONE,                  // GEKKO_LIMB_HEAD
    GEKKO_BODYPART_JAW,             // GEKKO_LIMB_JAW
    BODYPART_NONE,                  // GEKKO_LIMB_LEFT_EYE
    BODYPART_NONE,                  // GEKKO_LIMB_RIGHT_EYE
};

// Some kind of offset for the position of the Gekkos right foot
static Vec3f sRightFootOffsetRef = { 1500.0f, 2200.0f, 0.0f };

void EnBigslime_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBigslime* this = (EnBigslime*)thisx;
    Vec3f rightFootOffset;

    if (limbIndex == GEKKO_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
        this->actor.focus.rot.y = this->gekkoRot.y;
    }

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->gekkoBodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == GEKKO_LIMB_RIGHT_ANKLE) {
        Matrix_MultVec3f(&sRightFootOffsetRef, &rightFootOffset);
        this->gekkoCollider.dim.pos.y = rightFootOffset.y;
    }
}

void EnBigslime_DrawGekko(Actor* thisx, PlayState* play) {
    static Color_RGBA8 sGekkoDamageColor = { 255, 0, 0, 0 };
    static Color_RGBA8 sGekkoStunColor = { 0, 0, 255, 0 };
    Vec3f gekkoPos;
    EnBigslime* this = (EnBigslime*)thisx;
    s32 pad;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->actionFunc == EnBigslime_DamageGekko) {
        func_800AE434(play, &sGekkoDamageColor, this->damageSpinTimer, 20);
    } else if ((this->actionFunc == EnBigslime_CutsceneDefeat) || (this->actionFunc == EnBigslime_GekkoDespawn)) {
        func_800AE434(play, &sGekkoDamageColor, 20, 20);
    } else if (this->actionFunc == EnBigslime_StunGekko) {
        if (this->gekkoDrawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            func_800AE434(play, &sGekkoDamageColor, this->stunTimer, 80);
        } else if (this->gekkoDrawDmgEffType == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL) {
            func_800AE434(play, &sGekkoStunColor, this->stunTimer, 40);
        } else {
            func_800AE434(play, &sGekkoStunColor, this->stunTimer, 40);
        }
    }

    OPEN_DISPS(play->state.gfxCtx);

    Math_Vec3f_Sum(&this->actor.world.pos, &this->gekkoPosOffset, &gekkoPos);
    Matrix_SetTranslateRotateYXZ(gekkoPos.x, gekkoPos.y, gekkoPos.z, &this->gekkoRot);
    Matrix_Scale(this->gekkoScale, this->gekkoScale, this->gekkoScale, MTXMODE_APPLY);
    SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &gekkoPos, &this->gekkoProjectedPos);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnBigslime_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);

    if ((this->actionFunc == EnBigslime_DamageGekko) || (this->actionFunc == EnBigslime_CutsceneDefeat) ||
        (this->actionFunc == EnBigslime_GekkoDespawn) || (this->actionFunc == EnBigslime_StunGekko)) {
        func_800AE5A0(play);
    }

    EnBigslime_SetSysMatrix(&gekkoPos, play, gCircleShadowDL, this->gekkoScale * (550.0f / 7.0f),
                            this->gekkoScale * (550.0f / 7.0f), 0.0f, 0, 255.0f);

    if (this->minislimeState != MINISLIME_INACTIVE_STATE) {
        EnBigslime_DrawMinislime(this, play);
    }

    EnBigslime_DrawShatteringEffects(this, play);

    Actor_DrawDamageEffects(play, &this->actor, this->gekkoBodyPartsPos, GEKKO_BODYPART_MAX,
                            this->gekkoScale * (999.99991f / 7.0f) * this->gekkoDrawDmgEffScale,
                            this->gekkoDrawDmgEffFrozenSteamScale, this->gekkoDrawDmgEffAlpha,
                            this->gekkoDrawDmgEffType);
}

void EnBigslime_DrawShatteringEffects(EnBigslime* this, PlayState* play) {
    EnBigslimeIceShardEffect* iceShardEffect;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    // Draw Shockwave
    if (this->shockwaveAlpha > 0) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, this->shockwaveAlpha);
        gSPSegment(POLY_XLU_DISP++, 0x0D,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, play->gameplayFrames % 128, (u8)(play->gameplayFrames * 8),
                                      32, 64, 1, (-play->gameplayFrames * 2) % 64, 0, 16, 16, 1, 8, -2, 0));
        Matrix_Translate(this->frozenPos.x, this->frozenPos.y, this->frozenPos.z, MTXMODE_NEW);
        Matrix_Scale(this->shockwaveScale, this->shockwaveScale, this->shockwaveScale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gBigslimeShockwaveDL);
    }

    // Draw Ice Shards
    AnimatedMat_Draw(play, this->iceShardTexAnim);
    gSPDisplayList(POLY_XLU_DISP++, gBigslimeIceShardDL);

    for (i = 0; i < BIGSLIME_NUM_ICE_SHARD; i++) {
        iceShardEffect = &this->iceShardEffect[i];
        FrameInterpolation_RecordOpenChild(iceShardEffect, i);
        if (iceShardEffect->isEnabled > false) {
            Matrix_SetTranslateRotateYXZ(iceShardEffect->pos.x, iceShardEffect->pos.y, iceShardEffect->pos.z,
                                         &iceShardEffect->rot);
            Matrix_Scale(iceShardEffect->scale, iceShardEffect->scale, iceShardEffect->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gBigslimeIceShardVtxDL);
        }
        FrameInterpolation_RecordCloseChild();
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

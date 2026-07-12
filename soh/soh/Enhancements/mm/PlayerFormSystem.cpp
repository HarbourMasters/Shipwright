#include "PlayerFormSystem.h"
#include "soh/Enhancements/mm/forms/FormsCommon.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/ResourceManagerHelpers.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/mm/objects/object_link_goron/object_link_goron.h"
#include "assets/mm/objects/object_link_zora/object_link_zora.h"
#include "soh/Enhancements/mm/MMPlayerAnims.h"

void SkelAnime_InitLink(PlayState* play, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg,
                        LinkAnimationHeader* animation, s32 flags, Vec3s* jointTable, Vec3s* morphTable,
                        s32 limbBufCount);
extern PlayState* gPlayState;
}

static const char* sFormSkeletons[PLAYER_SHIPFORM_MAX] = {
    nullptr, // human uses gPlayerSkelHeaders[linkAge]
    gLinkGoronSkel,
    gLinkZoraSkel,
};

static const char* sFormWaitAnims[PLAYER_SHIPFORM_MAX] = {
    nullptr, // human uses gPlayerAnim_link_normal_wait
    gPlayerAnim_pg_wait,
    gPlayerAnim_pz_wait,
};

static u8 sFormMass[PLAYER_SHIPFORM_MAX] = {
    50,  // human (vanilla value, unused)
    200, // Goron (MM sPlayerMass)
    80,  // Zora
};

// Per-form player properties, transcribed from MM's sPlayerAgeProperties (2Ship
// mm/src/overlays/actors/ovl_player_actor/z_player.c) GORON/ZORA rows, reshaped into OOT's
// PlayerAgeProperties layout. MM's climb/chest-open animations come from the user's mm.o2r;
// animations MM shares with OOT (time travel, freestanding climb) use OOT's own assets.
// unk_92/unk_94 (voice/surface sfx offsets) hold MM bank offsets that don't exist in OOT's
// audio, so they are patched to the current age's values in PlayerForm_EnforceInvariants.
static PlayerAgeProperties sFormAgeProperties[PLAYER_SHIPFORM_MAX] = {
    {},
    // PLAYER_SHIPFORM_GORON
    {
        70.0f,                 // ceilingCheckHeight
        90.0f,                 // unk_04 (shadow scale)
        0.74f,                 // unk_08
        111.0f,                // unk_0C
        70.0f,                 // unk_10
        79.4f,                 // unk_14
        59.0f,                 // unk_18
        41.0f,                 // unk_1C
        19.0f,                 // unk_20
        36.0f,                 // unk_24
        50.0f,                 // unk_28
        56.0f,                 // unk_2C
        68.0f,                 // unk_30
        70.0f,                 // unk_34
        19.5f,                 // wallCheckRadius
        18.2f,                 // unk_3C
        80.0f,                 // unk_40
        { 0x17, 0xF3B, 0xDF }, // unk_44
        {
            { 0x18, 0xF3B, 0xDF },
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0xF3B, 0xDF },
            { 0x17, 0x14CF, 0xDF },
        }, // unk_4A
        {
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0x1AF2, 0xDF },
            { 0x17, 0x14CF, 0xDF },
            { 0x18, 0x1AF2, 0xDF },
        }, // unk_62
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
        }, // unk_7A
        {
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },                                                          // unk_86
        0,                                                          // unk_92 (patched per age)
        0x80,                                                       // unk_94 (patched per age)
        (LinkAnimationHeader*)gPlayerAnim_pg_Tbox_open,             // unk_98
        (LinkAnimationHeader*)gPlayerAnim_link_demo_back_to_past,   // unk_9C
        (LinkAnimationHeader*)gPlayerAnim_link_demo_return_to_past, // unk_A0
        (LinkAnimationHeader*)gPlayerAnim_pg_climb_startA,          // unk_A4
        (LinkAnimationHeader*)gPlayerAnim_pg_climb_startB,          // unk_A8
        { (LinkAnimationHeader*)gPlayerAnim_pg_climb_upL, (LinkAnimationHeader*)gPlayerAnim_pg_climb_upR,
          (LinkAnimationHeader*)gPlayerAnim_pg_climb_upL, (LinkAnimationHeader*)gPlayerAnim_pg_climb_upR }, // unk_AC
        { (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_sideL,
          (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { (LinkAnimationHeader*)gPlayerAnim_pg_climb_endAL,
          (LinkAnimationHeader*)gPlayerAnim_pg_climb_endAR }, // unk_C4
        { (LinkAnimationHeader*)gPlayerAnim_pg_climb_endBR,
          (LinkAnimationHeader*)gPlayerAnim_pg_climb_endBL }, // unk_CC
    },
    // PLAYER_SHIPFORM_ZORA
    {
        56.0f,                   // ceilingCheckHeight
        90.0f,                   // unk_04 (shadow scale)
        1.0f,                    // unk_08
        111.0f,                  // unk_0C
        70.0f,                   // unk_10
        79.4f,                   // unk_14
        59.0f,                   // unk_18
        41.0f,                   // unk_1C
        19.0f,                   // unk_20
        36.0f,                   // unk_24
        50.0f,                   // unk_28
        56.0f,                   // unk_2C
        68.0f,                   // unk_30
        70.0f,                   // unk_34
        18.0f,                   // wallCheckRadius
        23.0f,                   // unk_3C
        70.0f,                   // unk_40
        { 0x17, 0x1323, -0x6D }, // unk_44
        {
            { 0x17, 0x1323, -0x58 },
            { 0x17, 0x18B7, -0x6D },
            { 0x17, 0x1323, -0x58 },
            { 0x17, 0x18B7, -0x6D },
        }, // unk_4A
        {
            { 0x17, 0x18B7, -0x6D },
            { 0x18, 0x1EDA, -0x58 },
            { 0x17, 0x18B7, -0x6D },
            { 0x18, 0x1EDA, -0x58 },
        }, // unk_62
        {
            { 8, 0x1256, 0x17C },
            { 9, 0x17EA, 0x167 },
        }, // unk_7A
        {
            { -0x638, 0x1256, 0x17C },
            { -0x637, 0x17EA, 0x167 },
        },                                                          // unk_86
        0,                                                          // unk_92 (patched per age)
        0x80,                                                       // unk_94 (patched per age)
        (LinkAnimationHeader*)gPlayerAnim_pz_Tbox_open,             // unk_98
        (LinkAnimationHeader*)gPlayerAnim_link_demo_back_to_past,   // unk_9C
        (LinkAnimationHeader*)gPlayerAnim_link_demo_return_to_past, // unk_A0
        (LinkAnimationHeader*)gPlayerAnim_pz_climb_startA,          // unk_A4
        (LinkAnimationHeader*)gPlayerAnim_pz_climb_startB,          // unk_A8
        { (LinkAnimationHeader*)gPlayerAnim_pz_climb_upL, (LinkAnimationHeader*)gPlayerAnim_pz_climb_upR,
          (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_upL,
          (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_upR }, // unk_AC
        { (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_sideL,
          (LinkAnimationHeader*)gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { (LinkAnimationHeader*)gPlayerAnim_pz_climb_endAL,
          (LinkAnimationHeader*)gPlayerAnim_pz_climb_endAR }, // unk_C4
        { (LinkAnimationHeader*)gPlayerAnim_pz_climb_endBR,
          (LinkAnimationHeader*)gPlayerAnim_pz_climb_endBL }, // unk_CC
    },
};

// Form currently baked into the player's skeleton
static PlayerShipForm sAppliedForm = PLAYER_SHIPFORM_HUMAN;
// The vanilla sAgeProperties row (static in z_player.c, so captured rather than referenced)
static PlayerAgeProperties* sVanillaAgeProperties = nullptr;

PlayerShipForm PlayerForm_GetApplied() {
    return sAppliedForm;
}

bool PlayerForm_AssetsAvailable() {
    // mods archives can't change while the game is running, so resolve once
    static bool available = ResourceMgr_FileExists(sFormSkeletons[PLAYER_SHIPFORM_GORON]) &&
                            ResourceMgr_FileExists(sFormSkeletons[PLAYER_SHIPFORM_ZORA]);
    return available;
}

PlayerShipForm PlayerForm_GetTarget() {
    if (gPlayState == nullptr || !PlayerForm_AssetsAvailable()) {
        return PLAYER_SHIPFORM_HUMAN;
    }

    if (!CVarGetInteger(CVAR_TRANSFORMATION_MASKS_NAME, 0)) {
        return PLAYER_SHIPFORM_HUMAN;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return PLAYER_SHIPFORM_HUMAN;
    }
    switch (player->currentMask) {
        case PLAYER_MASK_GORON:
            return PLAYER_SHIPFORM_GORON;
        case PLAYER_MASK_ZORA:
            return PLAYER_SHIPFORM_ZORA;
        default:
            return PLAYER_SHIPFORM_HUMAN;
    }
}

void PlayerForm_EnforceInvariants(Player* player) {
    if (sAppliedForm == PLAYER_SHIPFORM_HUMAN) {
        if (sVanillaAgeProperties != nullptr && (player->ageProperties == &sFormAgeProperties[PLAYER_SHIPFORM_GORON] ||
                                                 player->ageProperties == &sFormAgeProperties[PLAYER_SHIPFORM_ZORA])) {
            player->ageProperties = sVanillaAgeProperties;
            player->actor.shape.shadowScale = sVanillaAgeProperties->unk_04;
        }
        return;
    }

    PlayerAgeProperties* props = &sFormAgeProperties[sAppliedForm];

    // MM's voice/surface sfx banks don't exist in OOT, so keep the current age's offsets
    props->unk_92 = LINK_IS_ADULT ? 0 : 0x20;
    props->unk_94 = LINK_IS_ADULT ? 0x80 : 0;

    if (player->ageProperties != props) {
        if (player->ageProperties != &sFormAgeProperties[PLAYER_SHIPFORM_GORON] &&
            player->ageProperties != &sFormAgeProperties[PLAYER_SHIPFORM_ZORA]) {
            sVanillaAgeProperties = player->ageProperties;
        }
        player->ageProperties = props;
    }
    // The Goron roll manages its own (smaller) ball shadow
    if (!GoronForm_IsRolling()) {
        player->actor.shape.shadowScale = props->unk_04;
    }

    // Vanilla resets mass to 50 every frame at the end of Player_UpdateCommon (z_player.c),
    // before this runs from OnPlayerUpdate; MASS_IMMOVABLE (cutscene/dead) is left alone
    if (player->actor.colChkInfo.mass == 50) {
        player->actor.colChkInfo.mass = sFormMass[sAppliedForm];
    }
}

void PlayerForm_Apply(PlayState* play, Player* player, PlayerShipForm form) {
    FlexSkeletonHeader* skel = form == PLAYER_SHIPFORM_HUMAN ? gPlayerSkelHeaders[gSaveContext.linkAge]
                                                             : (FlexSkeletonHeader*)sFormSkeletons[form];
    LinkAnimationHeader* waitAnim = form == PLAYER_SHIPFORM_HUMAN ? (LinkAnimationHeader*)gPlayerAnim_link_normal_wait
                                                                  : (LinkAnimationHeader*)sFormWaitAnims[form];
    // Matches Player_InitCommon's sSkeletonBaseTransl
    static Vec3s sSkeletonBaseTransl = { -57, 3377, 0 };

    ResourceMgr_UnregisterSkeleton(&player->skelAnime);
    ResourceMgr_UnregisterSkeleton(&player->upperSkelAnime);
    SkelAnime_InitLink(play, &player->skelAnime, skel, waitAnim, 9, player->jointTable, player->morphTable,
                       PLAYER_LIMB_MAX);
    player->skelAnime.baseTransl = sSkeletonBaseTransl;
    SkelAnime_InitLink(play, &player->upperSkelAnime, skel, waitAnim, 9, player->upperJointTable,
                       player->upperMorphTable, PLAYER_LIMB_MAX);
    player->upperSkelAnime.baseTransl = sSkeletonBaseTransl;

    sAppliedForm = form;
    PlayerForm_EnforceInvariants(player);
}

static void RegisterPlayerFormSystem() {
    // Substitute the form skeleton wherever the player skeleton is initialized: Player_InitCommon
    // (scene loads, so a transformed player never has to be re-initialized after the fact) and the
    // pause menu player preview (player == NULL there).
    REGISTER_VB_SHOULD(VB_PLAYER_INIT_SKELETON, {
        Player* player = va_arg(args, Player*);
        FlexSkeletonHeader** skelHeader = va_arg(args, FlexSkeletonHeader**);

        // Player_InitCommon also runs for other Player-struct actors (e.g. Dark Link); only
        // the real player transforms
        if (player != NULL && gPlayState != NULL && player != GET_PLAYER(gPlayState)) {
            return;
        }

        PlayerShipForm target = PlayerForm_GetTarget();
        if (target != PLAYER_SHIPFORM_HUMAN) {
            *skelHeader = (FlexSkeletonHeader*)sFormSkeletons[target];
        }
        if (player != NULL) {
            sAppliedForm = target;
        }
    });

    // OOT scales anim root translations by a hardcoded 0.64 as child (its child skeleton is
    // authored smaller than the animations); the MM form skeletons are full-size, so that
    // scale draws their root too low and sinks the feet into the floor. MM instead scales by
    // the form's own translation scale for every form (mm z_player_lib.c
    // Player_OverrideLimbDrawGameplayCommon), which is what unk_08 holds here.
    REGISTER_VB_SHOULD(VB_PLAYER_SCALE_ANIM_TRANSLATION, {
        Player* player = va_arg(args, Player*);
        f32* scale = va_arg(args, f32*);

        if (sAppliedForm != PLAYER_SHIPFORM_HUMAN && gPlayState != nullptr && player == GET_PLAYER(gPlayState)) {
            *scale = sFormAgeProperties[sAppliedForm].unk_08;
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPlayerFormSystem);

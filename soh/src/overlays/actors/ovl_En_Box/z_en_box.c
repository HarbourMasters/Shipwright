#include "z_en_box.h"
#include "objects/object_box/object_box.h"

#define FLAGS 0

// movement flags

/*
set on init unless treasure flag is set
if clear, chest moves (Actor_MoveForward) (falls, likely)
ends up cleared from SWITCH_FLAG_FALL types when switch flag is set
*/
#define ENBOX_MOVE_IMMOBILE (1 << 0)
/*
set in the logic for SWITCH_FLAG_FALL types
otherwise unused
*/
#define ENBOX_MOVE_UNUSED (1 << 1)
/*
set with 50% chance on init for SWITCH_FLAG_FALL types
only used for SWITCH_FLAG_FALL types
ends up "blinking" (set/clear every frame) once switch flag is set,
if some collision-related condition (?) is met
only used for signum of z rotation
*/
#define ENBOX_MOVE_FALL_ANGLE_SIDE (1 << 2)
/*
when set, gets cleared next EnBox_Update call and clip to the floor
*/
#define ENBOX_MOVE_STICK_TO_GROUND (1 << 4)

typedef enum {
    ENBOX_STATE_0, // waiting for player near / player available / player ? (IDLE)
    ENBOX_STATE_1, // used only temporarily, maybe "player is ready" ?
    ENBOX_STATE_2  // waiting for something message context-related
} EnBoxStateUnk1FB;

void EnBox_Init(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Update(Actor* thisx, GlobalContext* globalCtx);
void EnBox_Draw(Actor* thisx, GlobalContext* globalCtx);

void EnBox_FallOnSwitchFlag(EnBox*, GlobalContext*);
void func_809C9700(EnBox*, GlobalContext*);
void EnBox_AppearOnSwitchFlag(EnBox*, GlobalContext*);
void EnBox_AppearOnRoomClear(EnBox*, GlobalContext*);
void EnBox_AppearInit(EnBox*, GlobalContext*);
void EnBox_AppearAnimation(EnBox*, GlobalContext*);
void EnBox_WaitOpen(EnBox*, GlobalContext*);
void EnBox_Open(EnBox*, GlobalContext*);

const ActorInit En_Box_InitVars = {
    ACTOR_EN_BOX,
    ACTORCAT_CHEST,
    FLAGS,
    OBJECT_BOX,
    sizeof(EnBox),
    (ActorFunc)EnBox_Init,
    (ActorFunc)EnBox_Destroy,
    (ActorFunc)EnBox_Update,
    (ActorFunc)EnBox_Draw,
    NULL,
};

static AnimationHeader* sAnimations[4] = { &gTreasureChestAnim_00024C, &gTreasureChestAnim_000128,
                                           &gTreasureChestAnim_00043C, &gTreasureChestAnim_00043C };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_STOP),
};

static UNK_TYPE sUnused;

void EnBox_SetupAction(EnBox* this, EnBoxActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnBox_ClipToGround(EnBox* this, GlobalContext* globalCtx) {
    f32 newY;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f pos;

    pos = this->dyna.actor.world.pos;
    pos.y += 1.0f;
    newY = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &poly, &bgId, &this->dyna.actor, &pos);
    if (newY != BGCHECK_Y_MIN) {
        this->dyna.actor.world.pos.y = newY;
    }
}

void EnBox_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    EnBox* this = (EnBox*)thisx;
    AnimationHeader* anim;
    CollisionHeader* colHeader;
    f32 animFrameStart;
    f32 endFrame;

    animFrameStart = 0.0f;
    anim = sAnimations[((void)0, gSaveContext.linkAge)];
    colHeader = NULL;
    endFrame = Animation_GetLastFrame(anim);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gTreasureChestCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    func_8003ECA8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);

    this->movementFlags = 0;
    this->type = thisx->params >> 12 & 0xF;
    this->iceSmokeTimer = 0;
    this->unk_1FB = ENBOX_STATE_0;
    this->dyna.actor.gravity = -5.5f;
    this->switchFlag = this->dyna.actor.world.rot.z;
    this->dyna.actor.minVelocityY = -50.0f;

    if (globalCtx) {} // helps the compiler store globalCtx2 into s1

    if (Flags_GetTreasure(globalCtx, this->dyna.actor.params & 0x1F)) {
        this->alpha = 255;
        this->iceSmokeTimer = 100;
        EnBox_SetupAction(this, EnBox_Open);
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
        animFrameStart = endFrame;
    } else if ((this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG || this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) &&
               !Flags_GetSwitch(globalCtx, this->switchFlag)) {
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        if (Rand_ZeroOne() < 0.5f) {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        this->unk_1A8 = -12;
        EnBox_SetupAction(this, EnBox_FallOnSwitchFlag);
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if ((this->type == ENBOX_TYPE_ROOM_CLEAR_BIG || this->type == ENBOX_TYPE_ROOM_CLEAR_SMALL) &&
               !Flags_GetClear(globalCtx, this->dyna.actor.room)) {
        EnBox_SetupAction(this, EnBox_AppearOnRoomClear);
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if (this->type == ENBOX_TYPE_9 || this->type == ENBOX_TYPE_10) {
        EnBox_SetupAction(this, func_809C9700);
        this->dyna.actor.flags |= ACTOR_FLAG_25;
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_4;
    } else if (this->type == ENBOX_TYPE_SWITCH_FLAG_BIG && !Flags_GetSwitch(globalCtx, this->switchFlag)) {
        EnBox_SetupAction(this, EnBox_AppearOnSwitchFlag);
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_4;
    } else {
        if (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6) {
            this->dyna.actor.flags |= ACTOR_FLAG_7;
        }
        EnBox_SetupAction(this, EnBox_WaitOpen);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
    }

    this->dyna.actor.world.rot.y += 0x8000;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    SkelAnime_Init(globalCtx, &this->skelanime, &gTreasureChestSkel, anim, this->jointTable, this->morphTable, 5);
    Animation_Change(&this->skelanime, anim, 1.5f, animFrameStart, endFrame, ANIMMODE_ONCE, 0.0f);

    switch (this->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_SetScale(&this->dyna.actor, 0.005f);
            Actor_SetFocus(&this->dyna.actor, 20.0f);
            break;
        default:
            Actor_SetScale(&this->dyna.actor, 0.01f);
            Actor_SetFocus(&this->dyna.actor, 40.0f);
    }
}

void EnBox_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* this = (EnBox*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void EnBox_RandomDustKinematic(EnBox* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    f32 randomRadius = Rand_ZeroOne() * 25.0f;
    s16 randomAngle = Rand_ZeroOne() * 0x10000;

    *pos = this->dyna.actor.world.pos;
    pos->x += Math_SinS(randomAngle) * randomRadius;
    pos->z += Math_CosS(randomAngle) * randomRadius;

    velocity->y = 1.0f;
    velocity->x = Math_SinS(randomAngle);
    velocity->z = Math_CosS(randomAngle);

    accel->x = 0.0f;
    accel->y = 0.0f;
    accel->z = 0.0f;
}

/**
 * Spawns dust randomly around the chest when the chest hits the ground after falling (FALL types)
 */
void EnBox_SpawnDust(EnBox* this, GlobalContext* globalCtx) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        EnBox_RandomDustKinematic(this, &pos, &velocity, &accel);
        func_8002873C(globalCtx, &pos, &velocity, &accel, 100, 30, 15);
    }
}

/**
 * Used while the chest is falling (FALL types)
 */
void EnBox_Fall(EnBox* this, GlobalContext* globalCtx) {
    f32 yDiff;

    this->alpha = 255;
    this->movementFlags &= ~ENBOX_MOVE_IMMOBILE;
    if (this->dyna.actor.bgCheckFlags & 1) {
        this->movementFlags |= ENBOX_MOVE_UNUSED;
        if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
            this->movementFlags &= ~ENBOX_MOVE_FALL_ANGLE_SIDE;
        } else {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        if (this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG) {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.55f;
        } else {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.65f;
        }
        if (this->dyna.actor.velocity.y < 5.5f) {
            this->dyna.actor.shape.rot.z = 0;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            EnBox_SetupAction(this, EnBox_WaitOpen);
            OnePointCutscene_EndCutscene(globalCtx, this->unk_1AC);
        }
        Audio_PlaySoundGeneral(NA_SE_EV_COFFIN_CAP_BOUND, &this->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        EnBox_SpawnDust(this, globalCtx);
    }
    yDiff = this->dyna.actor.world.pos.y - this->dyna.actor.floorHeight;
    if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
        this->dyna.actor.shape.rot.z = yDiff * 50.0f;
    } else {
        this->dyna.actor.shape.rot.z = -yDiff * 50.0f;
    }
}

void EnBox_FallOnSwitchFlag(EnBox* this, GlobalContext* globalCtx) {
    s32 treasureFlag = this->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&this->dyna.actor, globalCtx);
    }

    if (this->unk_1A8 >= 0) {
        EnBox_SetupAction(this, EnBox_Fall);
        this->unk_1AC = OnePointCutscene_Init(globalCtx, 4500, 9999, &this->dyna.actor, MAIN_CAM);
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
    } else if (this->unk_1A8 >= -11) {
        this->unk_1A8++;
    } else if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
        this->unk_1A8++;
    }
}

// used for types 9, 10
void func_809C9700(EnBox* this, GlobalContext* globalCtx) {
    s32 treasureFlag = this->dyna.actor.params & 0x1F;
    Player* player = GET_PLAYER(globalCtx);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&this->dyna.actor, globalCtx);
    }

    if (Math3D_Vec3fDistSq(&this->dyna.actor.world.pos, &player->actor.world.pos) > 22500.0f) {
        this->unk_1FB = ENBOX_STATE_0;
    } else {
        if (this->unk_1FB == ENBOX_STATE_0) {
            if (!(player->stateFlags2 & 0x1000000)) {
                player->stateFlags2 |= 0x800000;
                return;
            }
            this->unk_1FB = ENBOX_STATE_1;
        }

        if (this->unk_1FB == ENBOX_STATE_1) {
            func_8010BD58(globalCtx, OCARINA_ACTION_FREE_PLAY);
            this->unk_1FB = ENBOX_STATE_2;
        } else if (this->unk_1FB == ENBOX_STATE_2 && globalCtx->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            if ((globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_LULLABY && this->type == ENBOX_TYPE_9) ||
                (globalCtx->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS && this->type == ENBOX_TYPE_10)) {
                this->dyna.actor.flags &= ~ACTOR_FLAG_25;
                EnBox_SetupAction(this, EnBox_AppearInit);
                OnePointCutscene_Attention(globalCtx, &this->dyna.actor);
                this->unk_1A8 = 0;
                this->unk_1FB = ENBOX_STATE_0;
            } else {
                this->unk_1FB = ENBOX_STATE_0;
            }
        }
    }
}

void EnBox_AppearOnSwitchFlag(EnBox* this, GlobalContext* globalCtx) {
    s32 treasureFlag = this->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&this->dyna.actor, globalCtx);
    }

    if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
        OnePointCutscene_Attention(globalCtx, &this->dyna.actor);
        EnBox_SetupAction(this, EnBox_AppearInit);
        this->unk_1A8 = -30;
    }
}

void EnBox_AppearOnRoomClear(EnBox* this, GlobalContext* globalCtx) {
    s32 treasureFlag = this->dyna.actor.params & 0x1F;

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        func_8002F5F0(&this->dyna.actor, globalCtx);
    }

    if (Flags_GetTempClear(globalCtx, this->dyna.actor.room) && !Player_InCsMode(globalCtx)) {
        Flags_SetClear(globalCtx, this->dyna.actor.room);
        EnBox_SetupAction(this, EnBox_AppearInit);
        OnePointCutscene_Attention(globalCtx, &this->dyna.actor);
        if (OnePointCutscene_CheckForCategory(globalCtx, this->dyna.actor.category)) {
            this->unk_1A8 = 0;
        } else {
            this->unk_1A8 = -30;
        }
    }
}

/**
 * The chest is ready to appear, possibly waiting for camera/cutscene-related stuff to happen
 */
void EnBox_AppearInit(EnBox* this, GlobalContext* globalCtx) {
    if (func_8005B198() == this->dyna.actor.category || this->unk_1A8 != 0) {
        EnBox_SetupAction(this, EnBox_AppearAnimation);
        this->unk_1A8 = 0;
        Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_DEMO_KANKYO, this->dyna.actor.home.pos.x,
                    this->dyna.actor.home.pos.y, this->dyna.actor.home.pos.z, 0, 0, 0, 0x0011);
        Audio_PlaySoundGeneral(NA_SE_EV_TRE_BOX_APPEAR, &this->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
    }
}

void EnBox_AppearAnimation(EnBox* this, GlobalContext* globalCtx) {
    func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);

    if (this->unk_1A8 < 0) {
        this->unk_1A8++;
    } else if (this->unk_1A8 < 40) {
        this->unk_1A8++;
        this->dyna.actor.world.pos.y += 1.25f;
    } else if (this->unk_1A8 < 60) {
        this->alpha += 12;
        this->unk_1A8++;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    } else {
        EnBox_SetupAction(this, EnBox_WaitOpen);
    }
}

typedef enum {
    LINKS_POCKET,
    QUEEN_GOHMA,
    KING_DODONGO,
    BARINADE,
    PHANTOM_GANON,
    VOLVAGIA,
    MORPHA,
    BONGO_BONGO,
    TWINROVA,
    SONG_FROM_IMPA,
    SONG_FROM_MALON,
    SONG_FROM_SARIA,
    SONG_FROM_ROYAL_FAMILYS_TOMB,
    SONG_FROM_OCARINA_OF_TIME,
    SONG_FROM_WINDMILL,
    SHEIK_IN_FOREST,
    SHEIK_IN_CRATER,
    SHEIK_IN_ICE_CAVERN,
    SHEIK_AT_COLOSSUS,
    SHEIK_IN_KAKARIKO,
    SHEIK_AT_TEMPLE,
    KF_MIDOS_TOP_LEFT_CHEST,
    KF_MIDOS_TOP_RIGHT_CHEST,
    KF_MIDOS_BOTTOM_LEFT_CHEST,
    KF_MIDOS_BOTTOM_RIGHT_CHEST,
    KF_KOKIRI_SWORD_CHEST,
    KF_STORMS_GROTTO_CHEST,
    LW_OCARINA_MEMORY_GAME,
    LW_TARGET_IN_WOODS,
    LW_NEAR_SHORTCUTS_GROTTO_CHEST,
    DEKU_THEATER_SKULL_MASK,
    DEKU_THEATER_MASK_OF_TRUTH,
    LW_SKULL_KID,
    LW_DEKU_SCRUB_NEAR_BRIDGE,
    LW_DEKU_SCRUB_GROTTO_FRONT,
    SFM_WOLFOS_GROTTO_CHEST,
    HF_NEAR_MARKET_GROTTO_CHEST,
    HF_TEKTITE_GROTTO_FREESTANDING_POH,
    HF_SOUTHEAST_GROTTO_CHEST,
    HF_OPEN_GROTTO_CHEST,
    HF_DEKU_SCRUB_GROTTO,
    MARKET_SHOOTING_GALLERY_REWARD,
    MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,
    MARKET_BOMBCHU_BOWLING_SECOND_PRIZE,
    MARKET_LOST_DOG,
    MARKET_TREASURE_CHEST_GAME_REWARD,
    MARKET_10_BIG_POES,
    TOT_LIGHT_ARROWS_CUTSCENE,
    HC_GREAT_FAIRY_REWARD,
    LLR_TALONS_CHICKENS,
    LLR_FREESTANDING_POH,
    KAK_ANJU_AS_CHILD,
    KAK_ANJU_AS_ADULT,
    KAK_IMPAS_HOUSE_FREESTANDING_POH,
    KAK_WINDMILL_FREESTANDING_POH,
    KAK_MAN_ON_ROOF,
    KAK_OPEN_GROTTO_CHEST,
    KAK_REDEAD_GROTTO_CHEST,
    KAK_SHOOTING_GALLERY_REWARD,
    KAK_10_GOLD_SKULLTULA_REWARD,
    KAK_20_GOLD_SKULLTULA_REWARD,
    KAK_30_GOLD_SKULLTULA_REWARD,
    KAK_40_GOLD_SKULLTULA_REWARD,
    KAK_50_GOLD_SKULLTULA_REWARD,
    GRAVEYARD_SHIELD_GRAVE_CHEST,
    GRAVEYARD_HEART_PIECE_GRAVE_CHEST,
    GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST,
    GRAVEYARD_FREESTANDING_POH,
    GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR,
    GRAVEYARD_HOOKSHOT_CHEST,
    GRAVEYARD_DAMPE_RACE_FREESTANDING_POH,
    DMT_FREESTANDING_POH,
    DMT_CHEST,
    DMT_STORMS_GROTTO_CHEST,
    DMT_GREAT_FAIRY_REWARD,
    DMT_BIGGORON,
    GC_DARUNIAS_JOY,
    GC_POT_FREESTANDING_POH,
    GC_ROLLING_GORON_AS_CHILD,
    GC_ROLLING_GORON_AS_ADULT,
    GC_MAZE_LEFT_CHEST,
    GC_MAZE_RIGHT_CHEST,
    GC_MAZE_CENTER_CHEST,
    DMC_VOLCANO_FREESTANDING_POH,
    DMC_WALL_FREESTANDING_POH,
    DMC_UPPER_GROTTO_CHEST,
    DMC_GREAT_FAIRY_REWARD,
    ZR_OPEN_GROTTO_CHEST,
    ZR_FROGS_IN_THE_RAIN,
    ZR_FROGS_OCARINA_GAME,
    ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH,
    ZR_NEAR_DOMAIN_FREESTANDING_POH,
    ZD_DIVING_MINIGAME,
    ZD_CHEST,
    ZD_KING_ZORA_THAWED,
    ZF_GREAT_FAIRY_REWARD,
    ZF_ICEBERG_FREESTANDING_POH,
    ZF_BOTTOM_FREESTANDING_POH,
    LH_UNDERWATER_ITEM,
    LH_CHILD_FISHING,
    LH_ADULT_FISHING,
    LH_LAB_DIVE,
    LH_FREESTANDING_POH,
    LH_SUN,
    GV_CRATE_FREESTANDING_POH,
    GV_WATERFALL_FREESTANDING_POH,
    GV_CHEST,
    GF_CHEST,
    GF_HBA_1000_POINTS,
    GF_HBA_1500_POINTS,
    WASTELAND_CHEST,
    COLOSSUS_GREAT_FAIRY_REWARD,
    COLOSSUS_FREESTANDING_POH,
    OGC_GREAT_FAIRY_REWARD,
    DEKU_TREE_MAP_CHEST,
    DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST,
    DEKU_TREE_SLINGSHOT_CHEST,
    DEKU_TREE_COMPASS_CHEST,
    DEKU_TREE_COMPASS_ROOM_SIDE_CHEST,
    DEKU_TREE_BASEMENT_CHEST,
    DEKU_TREE_QUEEN_GOHMA_HEART,
    DODONGOS_CAVERN_MAP_CHEST,
    DODONGOS_CAVERN_COMPASS_CHEST,
    DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST,
    DODONGOS_CAVERN_BOMB_BAG_CHEST,
    DODONGOS_CAVERN_END_OF_BRIDGE_CHEST,
    DODONGOS_CAVERN_BOSS_ROOM_CHEST,
    DODONGOS_CAVERN_KING_DODONGO_HEART,
    JABU_JABUS_BELLY_BOOMERANG_CHEST,
    JABU_JABUS_BELLY_MAP_CHEST,
    JABU_JABUS_BELLY_COMPASS_CHEST,
    JABU_JABUS_BELLY_BARINADE_HEART,
    BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
    BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,
    BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,
    BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,
    BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
    BOTTOM_OF_THE_WELL_COMPASS_CHEST,
    BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,
    BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
    BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,
    BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,
    BOTTOM_OF_THE_WELL_MAP_CHEST,
    BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,
    BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,
    BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
    FOREST_TEMPLE_FIRST_ROOM_CHEST,
    FOREST_TEMPLE_FIRST_STALFOS_CHEST,
    FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
    FOREST_TEMPLE_MAP_CHEST,
    FOREST_TEMPLE_WELL_CHEST,
    FOREST_TEMPLE_EYE_SWITCH_CHEST,
    FOREST_TEMPLE_BOSS_KEY_CHEST,
    FOREST_TEMPLE_FLOORMASTER_CHEST,
    FOREST_TEMPLE_RED_POE_CHEST,
    FOREST_TEMPLE_BOW_CHEST,
    FOREST_TEMPLE_BLUE_POE_CHEST,
    FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,
    FOREST_TEMPLE_BASEMENT_CHEST,
    FOREST_TEMPLE_PHANTOM_GANON_HEART,
    FIRE_TEMPLE_NEAR_BOSS_CHEST,
    FIRE_TEMPLE_FLARE_DANCER_CHEST,
    FIRE_TEMPLE_BOSS_KEY_CHEST,
    FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
    FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
    FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST,
    FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
    FIRE_TEMPLE_MAP_CHEST,
    FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST,
    FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST,
    FIRE_TEMPLE_SCARECROW_CHEST,
    FIRE_TEMPLE_COMPASS_CHEST,
    FIRE_TEMPLE_MEGATON_HAMMER_CHEST,
    FIRE_TEMPLE_HIGHEST_GORON_CHEST,
    FIRE_TEMPLE_VOLVAGIA_HEART,
    WATER_TEMPLE_COMPASS_CHEST,
    WATER_TEMPLE_MAP_CHEST,
    WATER_TEMPLE_CRACKED_WALL_CHEST,
    WATER_TEMPLE_TORCHES_CHEST,
    WATER_TEMPLE_BOSS_KEY_CHEST,
    WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
    WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
    WATER_TEMPLE_LONGSHOT_CHEST,
    WATER_TEMPLE_RIVER_CHEST,
    WATER_TEMPLE_DRAGON_CHEST,
    WATER_TEMPLE_MORPHA_HEART,
    SHADOW_TEMPLE_MAP_CHEST,
    SHADOW_TEMPLE_HOVER_BOOTS_CHEST,
    SHADOW_TEMPLE_COMPASS_CHEST,
    SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST,
    SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,
    SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST,
    SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,
    SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,
    SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,
    SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,
    SHADOW_TEMPLE_FREESTANDING_KEY,
    SHADOW_TEMPLE_WIND_HINT_CHEST,
    SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,
    SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
    SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,
    SHADOW_TEMPLE_BOSS_KEY_CHEST,
    SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
    SHADOW_TEMPLE_BONGO_BONGO_HEART,
    SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,
    SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
    SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST,
    SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,
    SPIRIT_TEMPLE_MAP_CHEST,
    SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
    SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
    SPIRIT_TEMPLE_COMPASS_CHEST,
    SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
    SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,
    SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST,
    SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,
    SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
    SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
    SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST,
    SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,
    SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,
    SPIRIT_TEMPLE_BOSS_KEY_CHEST,
    SPIRIT_TEMPLE_TOPMOST_CHEST,
    SPIRIT_TEMPLE_TWINROVA_HEART,
    ICE_CAVERN_MAP_CHEST,
    ICE_CAVERN_COMPASS_CHEST,
    ICE_CAVERN_FREESTANDING_POH,
    ICE_CAVERN_IRON_BOOTS_CHEST,
    GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,
    GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST,
    GERUDO_TRAINING_GROUND_STALFOS_CHEST,
    GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST,
    GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,
    GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST,
    GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,
    GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST,
    GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST,
    GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST,
    GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,
    GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST,
    GERUDO_TRAINING_GROUND_FREESTANDING_KEY,
    GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST,
    GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,
    GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST,
    GERUDO_TRAINING_GROUND_BEAMOS_CHEST,
    GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,
    GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,
    GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST,
    GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,
    GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,
    GANONS_CASTLE_FOREST_TRIAL_CHEST,
    GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,
    GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST,
    GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,
    GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
    GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
    GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST,
    GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,
    GANONS_TOWER_BOSS_KEY_CHEST,
} Check;

Check GetCheckFromSceneAndParams(s16 sceneNum, s16 actorParams) {
    switch(sceneNum) {
        case 40:
            switch(actorParams) {
                case 22944:
                    return KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
        case 85:
            switch(actorParams) {
                case 1248:
                    return KF_KOKIRI_SWORD_CHEST;
            }
        case 62:
            switch(actorParams) {
                case 22988:
                    return KF_STORMS_GROTTO_CHEST;
                case 22964:
                    return LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case 31409:
                    return SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return HF_NEAR_MARKET_GROTTO_CHEST;
                case 22978:
                    return HF_SOUTHEAST_GROTTO_CHEST;
                case 22947:
                    return HF_OPEN_GROTTO_CHEST;
                case 22984:
                    return KAK_OPEN_GROTTO_CHEST;
                case 31434:
                    return KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return DMT_STORMS_GROTTO_CHEST;
                case 23802:
                    return DMC_UPPER_GROTTO_CHEST;
                case 22985:
                    return ZR_OPEN_GROTTO_CHEST;
            }
        case 64:
            switch(actorParams) {
                case 21824:
                    return GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
        case 63:
            switch(actorParams) {
                case -22592:
                    return GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
        case 65:
            switch(actorParams) {
                case -32736:
                    return GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
        case 72:
            switch(actorParams) {
                case 4352:
                    return GRAVEYARD_HOOKSHOT_CHEST;
            }
        case 96:
            switch(actorParams) {
                case 23201:
                    return DMT_CHEST;
            }
        case 98:
            switch(actorParams) {
                case 23232:
                    return GC_MAZE_LEFT_CHEST;
                case 23201:
                    return GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return GC_MAZE_CENTER_CHEST;
            }
        case 88:
            switch(actorParams) {
                case -18496:
                    return ZD_CHEST;
            }
        case 90:
            switch(actorParams) {
                case 23200:
                    return GV_CHEST;
            }
        case 93:
            switch(actorParams) {
                case 1984:
                    return GF_CHEST;
            }
        case 94:
            switch(actorParams) {
                case -30048:
                    return WASTELAND_CHEST;
            }
        case 0:
            switch(actorParams) {
                case 2083:
                    return DEKU_TREE_MAP_CHEST;
                case 22789:
                    return DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return DEKU_TREE_BASEMENT_CHEST;
            }
        case 1:
            switch(actorParams) {
                case 2088:
                    return DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 20512:
                    return DODONGOS_CAVERN_BOSS_ROOM_CHEST;
            }
        case 2:
            switch(actorParams) {
                case 4289:
                    return JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return JABU_JABUS_BELLY_COMPASS_CHEST;
            }
    }
}

s32 GetCustomizedItemId(s16 actorParams, s16 sceneNum) {
    s32 itemId = actorParams >> 5 & 0x7F;
    Check check = GetCheckFromSceneAndParams(sceneNum, actorParams);
    switch(check) {
        case KF_MIDOS_TOP_LEFT_CHEST:
            itemId = GI_SWORD_KOKIRI;
            break;
        case KF_MIDOS_TOP_RIGHT_CHEST:
            itemId = GI_SHIELD_DEKU;
            break;
        case KF_MIDOS_BOTTOM_LEFT_CHEST:
            itemId = GI_BOMBCHUS_20;
            break;
        case KF_MIDOS_BOTTOM_RIGHT_CHEST:
            itemId = GI_ICE_TRAP;
            break;
    }
    return 0 - itemId;
}

/**
 * Chest is ready to be open
 */
void EnBox_WaitOpen(EnBox* this, GlobalContext* globalCtx) {
    f32 frameCount;
    AnimationHeader* anim;
    s32 linkAge;
    s32 pad;
    Vec3f sp4C;
    Player* player;

    this->alpha = 255;
    this->movementFlags |= ENBOX_MOVE_IMMOBILE;
    if (this->unk_1F4 != 0) { // unk_1F4 is modified by player code
        linkAge = gSaveContext.linkAge;
        anim = sAnimations[(this->unk_1F4 < 0 ? 2 : 0) + linkAge];
        frameCount = Animation_GetLastFrame(anim);
        Animation_Change(&this->skelanime, anim, 1.5f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
        EnBox_SetupAction(this, EnBox_Open);
        if (this->unk_1F4 > 0) {
            switch (this->type) {
                case ENBOX_TYPE_SMALL:
                case ENBOX_TYPE_6:
                case ENBOX_TYPE_ROOM_CLEAR_SMALL:
                case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
                    break;
                default:
                    Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_DEMO_TRE_LGT,
                                       this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                       this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                                       this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 0xFFFF);
                    Audio_PlayFanfare(NA_BGM_OPEN_TRE_BOX | 0x900);
            }
        }
        osSyncPrintf("Actor_Environment_Tbox_On() %d\n", this->dyna.actor.params & 0x1F);
        Flags_SetTreasure(globalCtx, this->dyna.actor.params & 0x1F);
    } else {
        player = GET_PLAYER(globalCtx);
        func_8002DBD0(&this->dyna.actor, &sp4C, &player->actor.world.pos);
        if (sp4C.z > -50.0f && sp4C.z < 0.0f && fabsf(sp4C.y) < 10.0f && fabsf(sp4C.x) < 20.0f &&
            Player_IsFacingActor(&this->dyna.actor, 0x3000, globalCtx)) {
            func_8002F554(&this->dyna.actor, globalCtx, GetCustomizedItemId(this->dyna.actor.params, globalCtx->sceneNum));
        }
        if (Flags_GetTreasure(globalCtx, this->dyna.actor.params & 0x1F)) {
            EnBox_SetupAction(this, EnBox_Open);
        }
    }
}

/**
 * Plays an animation to its end, playing sounds at key points
 */
void EnBox_Open(EnBox* this, GlobalContext* globalCtx) {
    u16 sfxId;

    this->dyna.actor.flags &= ~ACTOR_FLAG_7;

    if (SkelAnime_Update(&this->skelanime)) {
        if (this->unk_1F4 > 0) {
            if (this->unk_1F4 < 120) {
                this->unk_1F4++;
            } else {
                Math_StepToF(&this->unk_1B0, 0.0f, 0.05f);
            }
        } else {
            if (this->unk_1F4 > -120) {
                this->unk_1F4--;
            } else {
                Math_StepToF(&this->unk_1B0, 0.0f, 0.05f);
            }
        }
    } else {
        sfxId = 0;

        if (Animation_OnFrame(&this->skelanime, 30.0f)) {
            sfxId = NA_SE_EV_TBOX_UNLOCK;
        } else if (Animation_OnFrame(&this->skelanime, 90.0f)) {
            sfxId = NA_SE_EV_TBOX_OPEN;
        }

        if (sfxId != 0) {
            Audio_PlaySoundGeneral(sfxId, &this->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        if (this->skelanime.jointTable[3].z > 0) {
            this->unk_1B0 = (0x7D00 - this->skelanime.jointTable[3].z) * 0.00006f;
            if (this->unk_1B0 < 0.0f) {
                this->unk_1B0 = 0.0f;
            } else if (this->unk_1B0 > 1.0f) {
                this->unk_1B0 = 1.0f;
            }
        }
    }
}

void EnBox_SpawnIceSmoke(EnBox* this, GlobalContext* globalCtx) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    f32 f0;

    this->iceSmokeTimer++;
    func_8002F974(&this->dyna.actor, NA_SE_EN_MIMICK_BREATH - SFX_FLAG);
    if (Rand_ZeroOne() < 0.3f) {
        f0 = 2.0f * Rand_ZeroOne() - 1.0f;
        pos = this->dyna.actor.world.pos;
        if (this->type == ENBOX_TYPE_SMALL || this->type == ENBOX_TYPE_6 || this->type == ENBOX_TYPE_ROOM_CLEAR_SMALL ||
            this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) {
            pos.x += f0 * 10.0f * Math_SinS(this->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 10.0f * Math_CosS(this->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * Rand_ZeroOne() - 1.0f;
            vel.x = f0 * 0.8f * Math_SinS(this->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 0.8f * Math_CosS(this->dyna.actor.world.rot.y);
        } else {
            pos.x += f0 * 20.0f * Math_SinS(this->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 20.0f * Math_CosS(this->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * Rand_ZeroOne() - 1.0f;
            vel.x = f0 * 1.6f * Math_SinS(this->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 1.6f * Math_CosS(this->dyna.actor.world.rot.y);
        }
        EffectSsIceSmoke_Spawn(globalCtx, &pos, &vel, &accel, 150);
    }
}

void EnBox_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* this = (EnBox*)thisx;

    if (this->movementFlags & ENBOX_MOVE_STICK_TO_GROUND) {
        this->movementFlags &= ~ENBOX_MOVE_STICK_TO_GROUND;
        EnBox_ClipToGround(this, globalCtx);
    }

    this->actionFunc(this, globalCtx);

    if (!(this->movementFlags & ENBOX_MOVE_IMMOBILE)) {
        Actor_MoveForward(&this->dyna.actor);
        Actor_UpdateBgCheckInfo(globalCtx, &this->dyna.actor, 0.0f, 0.0f, 0.0f, 0x1C);
    }

    switch (this->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_SetFocus(&this->dyna.actor, 20.0f);
            break;
        default:
            Actor_SetFocus(&this->dyna.actor, 40.0f);
    }

    if ((this->dyna.actor.params >> 5 & 0x7F) == 0x7C && this->actionFunc == EnBox_Open &&
        this->skelanime.curFrame > 45 && this->iceSmokeTimer < 100) {
        EnBox_SpawnIceSmoke(this, globalCtx);
    }
}

void EnBox_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnBox* this = (EnBox*)thisx;
    s32 pad;

    if (limbIndex == 1) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_box.c", 1492),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (this->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestFrontDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestFrontDL);
        }
    } else if (limbIndex == 3) {
        gSPMatrix((*gfx)++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_box.c", 1502),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        if (this->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestSideAndLidDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestSideAndTopDL);
        }
    }
}

Gfx* EnBox_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = Graph_Alloc(gfxCtx, sizeof(Gfx));
    ASSERT(dList != NULL, "gfxp != NULL", "../z_en_box.c", 1528);

    dListHead = dList;
    gSPEndDisplayList(dListHead++);

    return dList;
}

// set render mode with a focus on transparency
Gfx* func_809CA4A0(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1546);

    dList = dListHead;
    gDPSetRenderMode(dListHead++,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA),
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* func_809CA518(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1564);

    dList = dListHead;
    gDPSetRenderMode(dListHead++,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL |
                         GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA),
                     G_RM_AA_ZB_OPA_SURF2);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void EnBox_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnBox* this = (EnBox*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_box.c", 1581);

    /*
    this->dyna.actor.flags & ACTOR_FLAG_7 is set by Init (if type is 4 or 6)
    and cleared by Open
    */
    if ((this->alpha == 255 && !(this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6)) ||
        (!CHECK_FLAG_ALL(this->dyna.actor.flags, ACTOR_FLAG_7) &&
         (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, EnBox_EmptyDList(globalCtx->state.gfxCtx));
        func_80093D18(globalCtx->state.gfxCtx);
        POLY_OPA_DISP = SkelAnime_Draw(globalCtx, this->skelanime.skeleton, this->skelanime.jointTable, NULL,
                                       EnBox_PostLimbDraw, this, POLY_OPA_DISP);
    } else if (this->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        func_80093D84(globalCtx->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        if (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6) {
            gSPSegment(POLY_XLU_DISP++, 0x08, func_809CA518(globalCtx->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, func_809CA4A0(globalCtx->state.gfxCtx));
        }
        POLY_XLU_DISP = SkelAnime_Draw(globalCtx, this->skelanime.skeleton, this->skelanime.jointTable, NULL,
                                       EnBox_PostLimbDraw, this, POLY_XLU_DISP);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_box.c", 1639);
}

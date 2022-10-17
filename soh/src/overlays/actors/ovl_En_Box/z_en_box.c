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
void EnBox_CreateExtraChestTextures();
void EnBox_UpdateSizeAndTexture(EnBox*, GlobalContext*);

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
GetItemEntry sItem;

Gfx gSkullTreasureChestChestSideAndLidDL[116] = {0};
Gfx gGoldTreasureChestChestSideAndLidDL[116] = {0};
Gfx gKeyTreasureChestChestSideAndLidDL[116] = {0};
Gfx gSkullTreasureChestChestFrontDL[128] = {0};
Gfx gGoldTreasureChestChestFrontDL[128] = {0};
Gfx gKeyTreasureChestChestFrontDL[128] = {0};
u8 hasCreatedRandoChestTextures = 0;

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

    EnBox_UpdateSizeAndTexture(this, globalCtx);
    // For SOH we spawn a chest actor instead of rendering the object from scratch for forest boss
    // key chest, and it's up on the wall so disable gravity for it.
    if (globalCtx->sceneNum == SCENE_BMORI1 && this->dyna.actor.params == 10222) {
        this->movementFlags = ENBOX_MOVE_IMMOBILE;
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

        // treasure chest game rando
        if (Randomizer_GetSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
            if (gSaveContext.n64ddFlag && globalCtx->sceneNum == 16 && (this->dyna.actor.params & 0x60) != 0x20) {
                if((this->dyna.actor.params & 0xF) < 2) {
                    Flags_SetCollectible(globalCtx, 0x1B);
                }
                if((this->dyna.actor.params & 0xF) >= 2 && (this->dyna.actor.params & 0xF) < 4) {
                    Flags_SetCollectible(globalCtx, 0x1C);
                }
                if((this->dyna.actor.params & 0xF) >= 4 && (this->dyna.actor.params & 0xF) < 6) {
                    Flags_SetCollectible(globalCtx, 0x1D);
                }
                if((this->dyna.actor.params & 0xF) >= 6 && (this->dyna.actor.params & 0xF) < 8) {
                    Flags_SetCollectible(globalCtx, 0x1E);
                }
                if((this->dyna.actor.params & 0xF) >= 8 && (this->dyna.actor.params & 0xF) < 10) {
                    Flags_SetCollectible(globalCtx, 0x1F);
                }
            }
        }
    } else {
        player = GET_PLAYER(globalCtx);
        func_8002DBD0(&this->dyna.actor, &sp4C, &player->actor.world.pos);
        if (sp4C.z > -50.0f && sp4C.z < 0.0f && fabsf(sp4C.y) < 10.0f && fabsf(sp4C.x) < 20.0f &&
            Player_IsFacingActor(&this->dyna.actor, 0x3000, globalCtx)) {
            sItem = Randomizer_GetItemFromActor(this->dyna.actor.id, globalCtx->sceneNum, this->dyna.actor.params, this->dyna.actor.params >> 5 & 0x7F);
            GetItemEntry blueRupee = ItemTable_RetrieveEntry(MOD_NONE, GI_RUPEE_BLUE);
            
            // RANDOTODO treasure chest game rando
            if (Randomizer_GetSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                if (gSaveContext.n64ddFlag && globalCtx->sceneNum == 16 && (this->dyna.actor.params & 0x60) != 0x20) {
                    if((this->dyna.actor.params & 0xF) < 2) {
                        if(Flags_GetCollectible(globalCtx, 0x1B)) {
                            sItem = blueRupee;
                        }
                    }
                    if((this->dyna.actor.params & 0xF) >= 2 && (this->dyna.actor.params & 0xF) < 4) {
                        if(Flags_GetCollectible(globalCtx, 0x1C)) {
                            sItem = blueRupee;
                        }
                    }
                    if((this->dyna.actor.params & 0xF) >= 4 && (this->dyna.actor.params & 0xF) < 6) {
                        if(Flags_GetCollectible(globalCtx, 0x1D)) {
                            sItem = blueRupee;
                        }
                    }
                    if((this->dyna.actor.params & 0xF) >= 6 && (this->dyna.actor.params & 0xF) < 8) {
                        if(Flags_GetCollectible(globalCtx, 0x1E)) {
                            sItem = blueRupee;
                        }
                    }
                    if((this->dyna.actor.params & 0xF) >= 8 && (this->dyna.actor.params & 0xF) < 10) {
                        if(Flags_GetCollectible(globalCtx, 0x1F)) {
                            sItem = blueRupee;
                        }
                    }
                }
            }
            // Chests need to have a negative getItemId in order to not immediately give their item
            // when approaching.
            if (gSaveContext.n64ddFlag) {
                sItem.getItemId = 0 - sItem.getItemId;
                sItem.getItemFrom = ITEM_FROM_CHEST;
                GiveItemEntryFromActorWithFixedRange(&this->dyna.actor, globalCtx, sItem);
            } else {
                func_8002F554(&this->dyna.actor, globalCtx, -(this->dyna.actor.params >> 5 & 0x7F));
            }
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

    EnBox_UpdateSizeAndTexture(this, globalCtx);

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

    if (((!gSaveContext.n64ddFlag && ((this->dyna.actor.params >> 5 & 0x7F) == 0x7C)) ||
         (gSaveContext.n64ddFlag && ABS(sItem.getItemId) == RG_ICE_TRAP)) && 
        this->actionFunc == EnBox_Open && this->skelanime.curFrame > 45 &&
        this->iceSmokeTimer < 100) EnBox_SpawnIceSmoke(this, globalCtx);
}

void EnBox_UpdateSizeAndTexture(EnBox* this, GlobalContext* globalCtx) {
    if (CVar_GetS32("gChestSizeAndTextureMatchesContents", 0) && globalCtx->sceneNum != SCENE_TAKARAYA) {
        EnBox_CreateExtraChestTextures();
        GetItemEntry getItemEntry = GET_ITEM_NONE;

        if (gSaveContext.n64ddFlag) {
            getItemEntry = Randomizer_GetItemFromActorWithoutObtainabilityCheck(this->dyna.actor.id, globalCtx->sceneNum, this->dyna.actor.params, this->dyna.actor.params >> 5 & 0x7F);
        } else {
            getItemEntry = ItemTable_RetrieveEntry(MOD_NONE, this->dyna.actor.params >> 5 & 0x7F);
        }

        GetItemCategory getItemCategory = getItemEntry.getItemCategory;
        // If they don't have bombchu's yet consider the bombchu item major
        if (getItemEntry.gid == GID_BOMBCHU && INV_CONTENT(ITEM_BOMBCHU) != ITEM_BOMBCHU) {
            getItemCategory = ITEM_CATEGORY_MAJOR;
        }

        switch (getItemCategory) {
            case ITEM_CATEGORY_JUNK:
            case ITEM_CATEGORY_SMALL_KEY:
            case ITEM_CATEGORY_SKULLTULA_TOKEN:
                Actor_SetScale(&this->dyna.actor, 0.005f);
                Actor_SetFocus(&this->dyna.actor, 20.0f);
                break;
            default:
                Actor_SetScale(&this->dyna.actor, 0.01f);
                Actor_SetFocus(&this->dyna.actor, 40.0f);
                break;
        }

        switch (getItemCategory) {
            case ITEM_CATEGORY_MAJOR:
                this->boxBodyDL = gGoldTreasureChestChestFrontDL;
                this->boxLidDL = gGoldTreasureChestChestSideAndLidDL;
                break;
            case ITEM_CATEGORY_SKULLTULA_TOKEN:
                this->boxBodyDL = gSkullTreasureChestChestFrontDL;
                this->boxLidDL = gSkullTreasureChestChestSideAndLidDL;
                break;
            case ITEM_CATEGORY_SMALL_KEY:
                this->boxBodyDL = gKeyTreasureChestChestFrontDL;
                this->boxLidDL = gKeyTreasureChestChestSideAndLidDL;
                break;
            case ITEM_CATEGORY_BOSS_KEY:
                this->boxBodyDL = gTreasureChestBossKeyChestFrontDL;
                this->boxLidDL = gTreasureChestBossKeyChestSideAndTopDL;
                break;
            case ITEM_CATEGORY_LESSER:
            case ITEM_CATEGORY_JUNK:
            default:
                this->boxBodyDL = gTreasureChestChestFrontDL;
                this->boxLidDL = gTreasureChestChestSideAndLidDL;
                break;
        }
    } else {
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

        if (this->type != ENBOX_TYPE_DECORATED_BIG) {
            this->boxBodyDL = gTreasureChestChestFrontDL;
            this->boxLidDL = gTreasureChestChestSideAndLidDL;
        } else {
            this->boxBodyDL = gTreasureChestBossKeyChestFrontDL;
            this->boxLidDL = gTreasureChestBossKeyChestSideAndTopDL;
        }
    }
}

void EnBox_CreateExtraChestTextures() {
    if (hasCreatedRandoChestTextures) return;
    Gfx gTreasureChestChestTextures[] = {
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gSkullTreasureChestFrontTex")),
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gSkullTreasureChestSideAndTopTex")),
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gGoldTreasureChestFrontTex")),
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gGoldTreasureChestSideAndTopTex")),
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gKeyTreasureChestFrontTex")),
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ResourceMgr_LoadFileRaw("assets/objects/object_box/gKeyTreasureChestSideAndTopTex")),
    };

    Gfx* frontCmd = ResourceMgr_LoadGfxByName(gTreasureChestChestFrontDL);
    int frontIndex = 0;
    while (frontCmd->words.w0 >> 24 != G_ENDDL) {
        gSkullTreasureChestChestFrontDL[frontIndex] = *frontCmd;
        gGoldTreasureChestChestFrontDL[frontIndex] = *frontCmd;
        gKeyTreasureChestChestFrontDL[frontIndex] = *frontCmd;
        frontIndex++;
        ++frontCmd;
    }
    gSkullTreasureChestChestFrontDL[frontIndex] = *frontCmd;
    gGoldTreasureChestChestFrontDL[frontIndex] = *frontCmd;
    gKeyTreasureChestChestFrontDL[frontIndex] = *frontCmd;

    gSkullTreasureChestChestFrontDL[5] = gTreasureChestChestTextures[0];
    gSkullTreasureChestChestFrontDL[23] = gTreasureChestChestTextures[1];
    gSkullTreasureChestChestFrontDL[37] = gTreasureChestChestTextures[0];
    gSkullTreasureChestChestFrontDL[50] = gTreasureChestChestTextures[1];
    gGoldTreasureChestChestFrontDL[5] = gTreasureChestChestTextures[2];
    gGoldTreasureChestChestFrontDL[23] = gTreasureChestChestTextures[3];
    gGoldTreasureChestChestFrontDL[37] = gTreasureChestChestTextures[2];
    gGoldTreasureChestChestFrontDL[50] = gTreasureChestChestTextures[3];
    gKeyTreasureChestChestFrontDL[5] = gTreasureChestChestTextures[4];
    gKeyTreasureChestChestFrontDL[23] = gTreasureChestChestTextures[5];
    gKeyTreasureChestChestFrontDL[37] = gTreasureChestChestTextures[4];
    gKeyTreasureChestChestFrontDL[50] = gTreasureChestChestTextures[5];

    Gfx* sideCmd = ResourceMgr_LoadGfxByName(gTreasureChestChestSideAndLidDL);
    int sideIndex = 0;
    while (sideCmd->words.w0 >> 24 != G_ENDDL) {
        gSkullTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;
        gGoldTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;
        gKeyTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;
        sideIndex++;
        ++sideCmd;
    }
    gSkullTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;
    gGoldTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;
    gKeyTreasureChestChestSideAndLidDL[sideIndex] = *sideCmd;

    gSkullTreasureChestChestSideAndLidDL[5] = gTreasureChestChestTextures[0];
    gSkullTreasureChestChestSideAndLidDL[29] = gTreasureChestChestTextures[1];
    gSkullTreasureChestChestSideAndLidDL[45] = gTreasureChestChestTextures[0];
    gGoldTreasureChestChestSideAndLidDL[5] = gTreasureChestChestTextures[2];
    gGoldTreasureChestChestSideAndLidDL[29] = gTreasureChestChestTextures[3];
    gGoldTreasureChestChestSideAndLidDL[45] = gTreasureChestChestTextures[2];
    gKeyTreasureChestChestSideAndLidDL[5] = gTreasureChestChestTextures[4];
    gKeyTreasureChestChestSideAndLidDL[29] = gTreasureChestChestTextures[5];
    gKeyTreasureChestChestSideAndLidDL[45] = gTreasureChestChestTextures[4];

    hasCreatedRandoChestTextures = 1;
}

void EnBox_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnBox* this = (EnBox*)thisx;
    s32 pad;

    if (limbIndex == 1) {
        gSPMatrix((*gfx)++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfx)++, this->boxBodyDL);
    } else if (limbIndex == 3) {
        gSPMatrix((*gfx)++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList((*gfx)++, this->boxLidDL);
    }
}

Gfx* EnBox_EmptyDList(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = Graph_Alloc(gfxCtx, sizeof(Gfx));
    ASSERT(dList != NULL);

    dListHead = dList;
    gSPEndDisplayList(dListHead++);

    return dList;
}

// set render mode with a focus on transparency
Gfx* func_809CA4A0(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = Graph_Alloc(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL);

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
    ASSERT(dListHead != NULL);

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

    OPEN_DISPS(globalCtx->state.gfxCtx);

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

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

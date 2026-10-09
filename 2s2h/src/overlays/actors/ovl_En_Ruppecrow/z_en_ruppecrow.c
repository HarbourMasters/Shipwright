/*
 * File: z_en_ruppecrow.c
 * Overlay: ovl_En_Ruppecrow
 * Description: Guay Circling Clock Town
 */

#include "z_en_ruppecrow.h"
#include "objects/object_crow/object_crow.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

typedef enum EnRuppecrowEffect {
    /* 0x00 */ ENRUPPECROW_EFFECT_NONE = 0,
    /* 0x0A */ ENRUPPECROW_EFFECT_ICE = 10,
    /* 0x14 */ ENRUPPECROW_EFFECT_LIGHT = 20
} EnRuppecrowEffect;

void EnRuppecrow_Init(Actor* thisx, PlayState* play2);
void EnRuppecrow_Destroy(Actor* thisx, PlayState* play);
void EnRuppecrow_Update(Actor* thisx, PlayState* play);
void EnRuppecrow_Draw(Actor* thisx, PlayState* play);

void EnRuppecrow_FallToDespawn(EnRuppecrow* this, PlayState* play);
void EnRuppecrow_HandleSongCutscene(EnRuppecrow* this, PlayState* play);
void EnRuppecrow_FlyWhileDroppingRupees(EnRuppecrow* this, PlayState* play);
void EnRuppecrow_FlyToDespawn(EnRuppecrow* this, PlayState* play);

ActorProfile En_Ruppecrow_Profile = {
    /**/ ACTOR_EN_RUPPECROW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_CROW,
    /**/ sizeof(EnRuppecrow),
    /**/ EnRuppecrow_Init,
    /**/ EnRuppecrow_Destroy,
    /**/ EnRuppecrow_Update,
    /**/ EnRuppecrow_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 30, 30 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

s32 EnRuppecrow_UpdateCollision(EnRuppecrow* this, PlayState* play) {
    s32 pad;

    this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y =
        sJntSphInit.elements[0].dim.modelSphere.center.y + this->actor.world.pos.y;
    this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 25.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);

    return true;
}

s32 EnRuppecrow_HasReachedPointClockwise(EnRuppecrow* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

s32 EnRuppecrow_HasReachedPointCounterClockwise(EnRuppecrow* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[0].x - points[1].x;
        diffZ = points[0].z - points[1].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 2].x - points[count - 1].x;
        diffZ = points[count - 2].z - points[count - 1].z;
    } else {
        diffX = points[index - 1].x - points[index + 1].x;
        diffZ = points[index - 1].z - points[index + 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

f32 EnRuppecrow_GetPointDirection(Path* path, s32 pointIndex, PosRot* world, Vec3s* direction) {
    Vec3s* points;
    Vec3f point;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[pointIndex];
        VEC_SET(point, points->x, points->y, points->z);
    }

    direction->y = Math_Vec3f_Yaw(&world->pos, &point);
    direction->x = Math_Vec3f_Pitch(&world->pos, &point);

    return point.y - world->pos.y;
}

void EnRuppecrow_ShatterIce(EnRuppecrow* this, PlayState* play) {
    if (this->currentEffect == ENRUPPECROW_EFFECT_ICE) {
        this->currentEffect = ENRUPPECROW_EFFECT_NONE;
        this->unk_2C8 = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENRUPPECROW_BODYPART_MAX, 0x2, 0.2f, 0.2f);
    }
}

s32 EnRuppecrow_CanSpawnBlueRupees(PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (player->transformation) {
        case PLAYER_FORM_DEKU:
            return false;

        case PLAYER_FORM_GORON:
            return true;

        case PLAYER_FORM_ZORA:
            return false;

        case PLAYER_FORM_HUMAN:
            if (player->stateFlags1 & PLAYER_STATE1_800000) {
                return true;
            } else {
                return false;
            }

        default:
            return false;
    }
}

void EnRuppecrow_UpdateRupees(EnRuppecrow* this) {
    EnItem00* rupee;
    s16 rupeeIndex;

    for (rupeeIndex = 0; rupeeIndex < ENRUPPECROW_RUPEE_COUNT; rupeeIndex++) {
        rupee = this->rupees[rupeeIndex];
        if ((rupee != NULL) && (rupee->unk152 == 0)) {
            Actor_Kill(&rupee->actor);
        }
    }
}

void EnRuppecrow_SpawnRupee(EnRuppecrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xOffset;
    EnItem00* rupee;
    s16 rupeeIndex = this->rupeeIndex;

    if (!(player->stateFlags3 & PLAYER_STATE3_1000)) {
        xOffset = (this->rupeeIndex & 1) ? 10.0f : -10.0f;
    } else {
        xOffset = 0.0f;
    }

    if (!GameInteractor_Should(VB_GUAY_DROP_RUPEE, true, this)) {
        this->rupeeIndex++;
        return;
    }

    if (EnRuppecrow_CanSpawnBlueRupees(play) && (this->rupeeIndex % 5) == 4) {
        if (this->rupeeIndex == 19) {
            rupee = (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, this->actor.world.pos.x + xOffset,
                                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ITEM00_RUPEE_RED);
            this->rupees[rupeeIndex] = rupee;
            this->rupees[rupeeIndex]->actor.gravity = -5.0f;
            this->rupees[rupeeIndex]->actor.velocity.y = 0.0f;
            Actor_PlaySfx(&this->actor, NA_SE_EV_RUPY_FALL);
            rupee = this->rupees[rupeeIndex];
            rupee->unk152 = 60;
            this->rupees[rupeeIndex]->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        } else {
            rupee =
                (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, this->actor.world.pos.x + xOffset,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ITEM00_RUPEE_BLUE);
            this->rupees[rupeeIndex] = rupee;
            this->rupees[rupeeIndex]->actor.gravity = -5.0f;
            this->rupees[rupeeIndex]->actor.velocity.y = 0.0f;
            Actor_PlaySfx(&this->actor, NA_SE_EV_RUPY_FALL);
            rupee = this->rupees[rupeeIndex];
            rupee->unk152 = 60;
            this->rupees[rupeeIndex]->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
    } else if (this->rupeeIndex == 19) {
        rupee = (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, this->actor.world.pos.x + xOffset,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ITEM00_RUPEE_RED);
        this->rupees[rupeeIndex] = rupee;
        this->rupees[rupeeIndex]->actor.gravity = -5.0f;
        this->rupees[rupeeIndex]->actor.velocity.y = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EV_RUPY_FALL);
        rupee = this->rupees[rupeeIndex];
        rupee->unk152 = 60;
        this->rupees[rupeeIndex]->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else {
        rupee = (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, this->actor.world.pos.x + xOffset,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, ITEM00_RUPEE_GREEN);
        this->rupees[rupeeIndex] = rupee;
        this->rupees[rupeeIndex]->actor.gravity = -5.0f;
        this->rupees[rupeeIndex]->actor.velocity.y = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EV_RUPY_FALL);
        rupee = this->rupees[rupeeIndex];
        rupee->unk152 = 60;
        this->rupees[rupeeIndex]->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }

    this->rupeeIndex++;
}

void EnRuppecrow_UpdatePosition(EnRuppecrow* this, PlayState* play) {
    Vec3s nextPointDirection;

    EnRuppecrow_GetPointDirection(this->path, this->currentPoint, &this->actor.world, &nextPointDirection);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        nextPointDirection.y = this->actor.wallYaw;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, nextPointDirection.y, 0x4, 0x3E8, 0x1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_SmoothStepToS(&this->actor.world.rot.x, -nextPointDirection.x, 0x4, 0x3E8, 0x1);

    if (this->isGoingCounterClockwise & 1) {
        if (EnRuppecrow_HasReachedPointCounterClockwise(this, this->path, this->currentPoint)) {
            if (this->currentPoint <= 0) {
                this->currentPoint = this->path->count - 1;
            } else {
                this->currentPoint--;
            }

            if ((this->actionFunc == EnRuppecrow_FlyWhileDroppingRupees) &&
                (!EnRuppecrow_CanSpawnBlueRupees(play) || (this->currentPoint % -2) == 0)) {
                EnRuppecrow_SpawnRupee(this, play);
            }
        }
    } else if (EnRuppecrow_HasReachedPointClockwise(this, this->path, this->currentPoint)) {
        if (this->currentPoint >= this->path->count - 1) {
            this->currentPoint = 0;
        } else {
            this->currentPoint++;
        }

        if ((this->actionFunc == EnRuppecrow_FlyWhileDroppingRupees) &&
            (!EnRuppecrow_CanSpawnBlueRupees(play) || (this->currentPoint % -2) == 0)) {
            EnRuppecrow_SpawnRupee(this, play);
        }
    }
}

s32 EnRuppecrow_CheckPlayedMatchingSong(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) {
        switch (player->transformation) {
            case PLAYER_FORM_DEKU:
                if (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA) {
                    return true;
                }
                break;

            case PLAYER_FORM_GORON:
                if (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY) {
                    return true;
                }
                break;

            case PLAYER_FORM_ZORA:
                if (play->msgCtx.lastPlayedSong == OCARINA_SONG_NEW_WAVE) {
                    return true;
                }
                break;

            case PLAYER_FORM_HUMAN:
                if (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA) {
                    return true;
                } else if (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY) {
                    return true;
                } else if (play->msgCtx.lastPlayedSong == OCARINA_SONG_NEW_WAVE) {
                    return true;
                }
                break;

            default:
                break;
        }
    }

    return false;
}

void EnRuppecrow_UpdateSpeed(EnRuppecrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (player->transformation) {
        case PLAYER_FORM_DEKU:
            this->speedModifier = 7.0f;
            break;

        case PLAYER_FORM_GORON:
            if (player->stateFlags3 & PLAYER_STATE3_1000) { // Goron Link is curled
                this->speedModifier = 19.0f;
            } else {
                this->speedModifier = 7.0f;
            }
            break;

        case PLAYER_FORM_ZORA:
            this->speedModifier = 7.0f;
            break;

        case PLAYER_FORM_HUMAN:
            if (player->stateFlags1 & PLAYER_STATE1_800000) {
                this->speedModifier = 16.0f;
            } else {
                this->speedModifier = 7.0f;
            }
            break;

        default:
            break;
    }

    this->skelAnime.playSpeed = this->speedModifier / 6.0f;
    if (this->actor.xzDistToPlayer > 800.0f) {
        this->speedModifier *= 0.5f;
    } else if (this->actor.xzDistToPlayer > 500.0f) {
        this->speedModifier *= 0.8f;
    } else if (this->actor.xzDistToPlayer < 150.0f) {
        this->speedModifier *= 1.2f;
    }
}

void EnRuppecrow_HandleDeath(EnRuppecrow* this) {
    f32 scale;

    this->actor.speed *= Math_CosS(this->actor.world.rot.x);
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);

    this->actor.shape.yOffset = 0.0f;
    this->actor.lockOnArrowOffset = 0.0f;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;

    scale = this->actor.scale.x * 100.0f;
    this->actor.world.pos.y += 20.0f * scale;

    Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_DEAD);

    this->unk_2CC = 0.5f;
    if (this->actor.colChkInfo.damageEffect == 0x3) {
        this->currentEffect = ENRUPPECROW_EFFECT_ICE;
        this->unk_2C8 = 1.0f;
        this->iceSfxTimer = 0.75f;
    } else if (this->actor.colChkInfo.damageEffect == 0x4) {
        this->currentEffect = ENRUPPECROW_EFFECT_LIGHT;
        this->unk_2C8 = 5.0f;
    } else if (this->actor.colChkInfo.damageEffect == 0x2) {
        this->currentEffect = ENRUPPECROW_EFFECT_NONE;
        this->unk_2C8 = 5.0f;
    }

    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.speed = 0.0f;
    }

    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = EnRuppecrow_FallToDespawn;
}

void EnRuppecrow_UpdateDamage(EnRuppecrow* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elements[0].base);

        if (this->actor.colChkInfo.damageEffect != 0x1) {
            this->actor.colChkInfo.health = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            Enemy_StartFinishingBlow(play, &this->actor);
            EnRuppecrow_HandleDeath(this);
        }
    }
}

void EnRuppecrow_HandleSong(EnRuppecrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnRuppecrow_UpdatePosition(this, play);
    if ((this->actor.xzDistToPlayer < 1000.0f) && EnRuppecrow_CheckPlayedMatchingSong(play)) {
        // If Link is in front, the guay will turn around and go the other way
        if (Actor_IsFacingPlayer(&this->actor, 0x4000)) {
            this->isGoingCounterClockwise |= 1;
            if (this->currentPoint > 0) {
                this->currentPoint--;
            } else {
                this->currentPoint = this->path->count - 1;
            }
        }

        CutsceneManager_Queue(this->actor.csId);
        this->actionFunc = EnRuppecrow_HandleSongCutscene;
    }

    if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
        Math_ApproachF(&this->actor.speed, 0.0f, 0.1f, 1.0f);
    } else {
        Math_ApproachF(&this->actor.speed, 6.0f, 0.1f, 0.1f);
    }

    Actor_MoveWithoutGravity(&this->actor);
    this->yOffset += 0x1000;
    this->actor.shape.yOffset = Math_SinS(this->yOffset) * 500.0f;

    if ((play->state.frames % 43) == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_CRY);
    }
}

void EnRuppecrow_HandleSongCutscene(EnRuppecrow* this, PlayState* play) {
    EnRuppecrow_UpdatePosition(this, play);

    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_Start(this->actor.csId, &this->actor);
        EnRuppecrow_UpdateSpeed(this, play);
        this->actionFunc = EnRuppecrow_FlyWhileDroppingRupees;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }

    Actor_MoveWithoutGravity(&this->actor);
}

void EnRuppecrow_FlyWhileDroppingRupees(EnRuppecrow* this, PlayState* play) {
    EnRuppecrow_UpdatePosition(this, play);

    if (this->rupeeIndex >= ENRUPPECROW_RUPEE_COUNT) {
        // Finished spawning rupees; fly up and then despawn
        this->speedModifier = 6.0f;

        //! @bug: Source of the "Termina Field Guay Glitch"; guay will no longer fall if killed after this point
        this->actor.gravity = 0.0f;

        Math_ApproachF(&this->actor.speed, 6.0f, 0.2f, 0.5f);
        Math_ApproachF(&this->actor.velocity.y, 3.0f, 0.2f, 0.5f);

        this->actionFunc = EnRuppecrow_FlyToDespawn;
        this->skelAnime.playSpeed = 1.0f;
        Actor_MoveWithGravity(&this->actor);
    } else {
        if (CutsceneManager_GetCurrentCsId() != this->actor.csId) {
            EnRuppecrow_UpdateSpeed(this, play);
            Math_ApproachF(&this->actor.speed, this->speedModifier, 0.2f, 0.5f);
        }

        Actor_MoveWithoutGravity(&this->actor);
        this->yOffset += 0x1000;
        this->actor.shape.yOffset = Math_SinS(this->yOffset) * 500.0f;

        if ((play->state.frames % 43) == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_CRY);
        }
    }
}

void EnRuppecrow_FlyToDespawn(EnRuppecrow* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.rot.y *= -0x1;
    }

    Math_ApproachF(&this->actor.speed, this->speedModifier, 0.1f, 0.1f);
    Math_ApproachF(&this->actor.velocity.y, 3.0f, 0.2f, 0.5f);

    if ((this->actor.world.pos.y > 1000.0f) || (this->actor.xzDistToPlayer > 2000.0f)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->yOffset += 0x800;
    this->actor.shape.yOffset = Math_SinS(this->yOffset) * 500.0f;

    Actor_MoveWithGravity(&this->actor);

    if ((play->state.frames % 43) == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_CRY);
    }
}

void EnRuppecrow_FallToDespawn(EnRuppecrow* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);

    if (this->currentEffect != ENRUPPECROW_EFFECT_ICE) {
        Math_StepToF(&this->unk_2C8, 0.0f, 0.05f);
        this->unk_2CC = (this->unk_2C8 + 1.0f) * 0.25f;
        this->unk_2CC = CLAMP_MAX(this->unk_2CC, 0.5f);
    } else if (!Math_StepToF(&this->iceSfxTimer, 0.5f, 0.0125f)) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
    }

    this->actor.colorFilterTimer = 40;
    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->currentEffect != ENRUPPECROW_EFFECT_ICE) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x200);
            this->actor.shape.rot.z += 0x1780;
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN)) {
            EnRuppecrow_ShatterIce(this, play);
            func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, (this->actor.scale.x * 10000.0f), 0,
                          0);

            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 0xB, NA_SE_EN_EXTINCT);
            Actor_Kill(&this->actor);
            return;
        }
    }

    Actor_MoveWithGravity(&this->actor);
}

void EnRuppecrow_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnRuppecrow* this = (EnRuppecrow*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable,
                       OBJECT_CROW_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);

    Collider_InitJntSph(play, &this->collider);
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    Actor_SetScale(&this->actor, 0.01f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;

    this->path = SubS_GetPathByIndex(play, ENRUPPECROW_GET_PATH_INDEX(&this->actor), ENRUPPECROW_PATH_INDEX_NONE);
    if (this->path != NULL) {
        this->actionFunc = EnRuppecrow_HandleSong;
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnRuppecrow_Destroy(Actor* thisx, PlayState* play) {
    EnRuppecrow* this = (EnRuppecrow*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnRuppecrow_Update(Actor* thisx, PlayState* play) {
    EnRuppecrow* this = (EnRuppecrow*)thisx;

    EnRuppecrow_UpdateDamage(this, play);
    this->actionFunc(this, play);
    EnRuppecrow_UpdateRupees(this);
    this->actor.focus.pos = this->actor.world.pos;
    SkelAnime_Update(&this->skelAnime);
    EnRuppecrow_UpdateCollision(this, play);
}

void EnRuppecrow_Draw(Actor* thisx, PlayState* play) {
    EnRuppecrow* this = (EnRuppecrow*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}

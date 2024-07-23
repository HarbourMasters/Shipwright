/*
 * File: z_en_bb.c
 * Overlay: ovl_En_Bb
 * Description: Bubble (Flying Skull Enemy)
 */

#include "z_en_bb.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_Bb/object_Bb.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_PLAY_HIT_SFX)

#define vBombHopPhase actionVar1
#define vTrailIdx actionVar1
#define vTrailMaxAlpha actionVar2
#define vMoveAngleY actionVar2
#define vFlameTimer actionVar2

typedef enum {
    /* 0 */ BB_DAMAGE,
    /* 1 */ BB_KILL,
    /* 2 */ BB_FLAME_TRAIL,
    /* 3 */ BB_DOWN,
    /* 4 */ BB_STUNNED,
    /* 5 */ BB_UNUSED,
    /* 6 */ BB_BLUE,
    /* 7 */ BB_RED,
    /* 8 */ BB_WHITE,
    /* 9 */ BB_GREEN
} EnBbAction;

typedef enum {
    /* 0 */ BBMOVE_NORMAL,
    /* 1 */ BBMOVE_NOCLIP,
    /* 2 */ BBMOVE_HIDDEN
} EnBbMoveMode;

typedef enum {
    /* 0 */ BBBLUE_NORMAL,
    /* 1 */ BBBLUE_AGGRO
} EnBbBlueActionState;

typedef enum {
    /* 0 */ BBRED_WAIT,
    /* 1 */ BBRED_ATTACK,
    /* 2 */ BBRED_HIDE
} EnBbRedActionState;

typedef enum {
    /* 0 */ BBGREEN_FLAME_ON,
    /* 1 */ BBGREEN_FLAME_OFF
} EnBbGreenActionState;

// Main functions

void EnBb_Init(Actor* thisx, PlayState* play);
void EnBb_Destroy(Actor* thisx, PlayState* play);
void EnBb_Update(Actor* this, PlayState* play);
void EnBb_Draw(Actor* this, PlayState* play);

// Helper functions

void EnBb_FaceWaypoint(EnBb* this);
void EnBb_SetWaypoint(EnBb* this, PlayState* play);

// Action functions

void EnBb_SetupFlameTrail(EnBb* this);
void EnBb_FlameTrail(EnBb* this, PlayState* play);

void EnBb_SetupDeath(EnBb* this, PlayState* play);
void EnBb_Death(EnBb* this, PlayState* play);

void EnBb_Damage(EnBb* this, PlayState* play);

void EnBb_SetupBlue(EnBb* this);
void EnBb_Blue(EnBb* this, PlayState* play);

void EnBb_SetupDown(EnBb* this);
void EnBb_Down(EnBb* this, PlayState* play);

void EnBb_SetupRed(PlayState* play, EnBb* this);
void EnBb_Red(EnBb* this, PlayState* play);

void EnBb_SetupWhite(PlayState* play, EnBb* this);
void EnBb_White(EnBb* this, PlayState* play);

void EnBb_InitGreen(EnBb* this, PlayState* play);
void EnBb_Green(EnBb* this, PlayState* play);

void EnBb_Stunned(EnBb* this, PlayState* play);

static DamageTable sDamageTableBlueGreen = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0xC),
    /* Light arrow   */ DMG_ENTRY(4, 0xB),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(3, 0x8),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable sDamageTableRed = {
    /* Deku nut      */ DMG_ENTRY(0, 0xD),
    /* Deku stick    */ DMG_ENTRY(0, 0xD),
    /* Slingshot     */ DMG_ENTRY(0, 0xD),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xD),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xD),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(2, 0xE),
    /* Ice arrow     */ DMG_ENTRY(4, 0x9),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0x9),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static DamageTable sDamageTableWhite = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0xE),
    /* Slingshot     */ DMG_ENTRY(1, 0xE),
    /* Explosive     */ DMG_ENTRY(2, 0xA),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(2, 0xE),
    /* Hammer swing  */ DMG_ENTRY(2, 0xA),
    /* Hookshot      */ DMG_ENTRY(0, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(4, 0x5),
    /* Ice arrow     */ DMG_ENTRY(2, 0xE),
    /* Light arrow   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xE),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xE),
    /* Fire magic    */ DMG_ENTRY(4, 0x7),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
    /* Shield        */ DMG_ENTRY(0, 0xA),
    /* Mirror Ray    */ DMG_ENTRY(0, 0xA),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x6),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xA),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

const ActorInit En_Bb_InitVars = {
    ACTOR_EN_BB,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BB,
    sizeof(EnBb),
    (ActorFunc)EnBb_Init,
    (ActorFunc)EnBb_Destroy,
    (ActorFunc)EnBb_Update,
    (ActorFunc)EnBb_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, -120, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(targetArrowOffset, 10, ICHAIN_STOP),
};

void EnBb_SetupAction(EnBb* this, EnBbActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

Actor* EnBb_FindExplosive(PlayState* play, EnBb* this, f32 range) {
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dist;

    while (explosive != NULL) {
        if (explosive->params != 0) {
            explosive = explosive->next;
            continue;
        }
        dist = Actor_WorldDistXYZToActor(&this->actor, explosive);
        if ((explosive->params == 0) && (dist <= range)) {
            return explosive;
        }
        explosive = explosive->next;
    }
    return NULL;
}

void EnBb_SpawnFlameTrail(PlayState* play, EnBb* this, s16 startAtZero) {
    EnBb* now = this;
    EnBb* next;
    s32 i;

    for (i = 0; i < 5; i++) {
        next = (EnBb*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BB, this->actor.world.pos.x,
                                  this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0, true);
        if (next != NULL) {
            now->actor.child = &next->actor;
            next->actor.parent = &now->actor;
            next->targetActor = &this->actor;
            next->vTrailIdx = i + 1;
            next->actor.scale.x = 1.0f;
            next->vTrailMaxAlpha = next->flamePrimAlpha = 255 - (i * 40);
            next->flameScaleY = next->actor.scale.y = 0.8f - (i * 0.075f);
            next->flameScaleX = next->actor.scale.z = 1.0f - (i * 0.094f);
            if (startAtZero) {
                next->flamePrimAlpha = 0;
                next->flameScaleY = next->flameScaleX = 0.0f;
            }
            next->flameScrollMod = i + 1;
            next->timer = 2 * i + 2;
            next->flameEnvColor.r = 255;
            now = next;
        }
    }
}

void EnBb_KillFlameTrail(EnBb* this) {
    Actor* actor = &this->actor;

    while (actor->child != NULL) {
        Actor* nextActor = actor->child;

        if (nextActor->id == ACTOR_EN_BB) {
            nextActor->parent = NULL;
            actor->child = NULL;
            nextActor->params = ENBB_KILL_TRAIL;
        }
        actor = nextActor;
    }
    this->actor.child = NULL;
}

void EnBb_Init(Actor* thisx, PlayState* play) {
    EffectBlureInit1 blureInit;
    s32 pad;
    EnBb* this = (EnBb*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &object_Bb_Skel_001A30, &object_Bb_Anim_000444, this->jointTable,
                   this->morphTable, 16);
    this->unk_254 = 0;
    thisx->colChkInfo.health = 4;
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, thisx, &sJntSphInit, this->elements);

    this->actionState = thisx->params >> 8;

    if (thisx->params & 0x80) {
        thisx->params |= 0xFF00;
    }
    if (thisx->params <= ENBB_BLUE) {
        ActorShape_Init(&thisx->shape, 200.0f, ActorShadow_DrawCircle, 35.0f);
    }
    if (thisx->params & 0xFF00) {
        this->timer = 0;
        this->flameScaleY = 80.0f;
        this->flameScaleX = 100.0f;
        this->collider.elements[0].info.toucherFlags = TOUCH_ON | TOUCH_SFX_HARD;
        this->collider.elements[0].info.toucher.dmgFlags = 0xFFCFFFFF;
        this->collider.elements[0].info.toucher.damage = 8;
        this->bobSize = this->actionState * 20.0f;
        this->flamePrimAlpha = 255;
        this->moveMode = BBMOVE_NORMAL;
        Actor_SetScale(thisx, 0.01f);
        switch (thisx->params) {
            case ENBB_BLUE:
                thisx->naviEnemyId = 0x1C;
                thisx->colChkInfo.damageTable = &sDamageTableBlueGreen;
                this->flamePrimBlue = this->flameEnvColor.b = 255;
                thisx->world.pos.y += 50.0f;
                EnBb_SetupBlue(this);
                thisx->flags |= ACTOR_FLAG_ARROW_DRAGGABLE;
                break;
            case ENBB_RED:
                thisx->naviEnemyId = 0x24;
                thisx->colChkInfo.damageTable = &sDamageTableRed;
                this->flameEnvColor.r = 255;
                this->collider.elements[0].info.toucher.effect = 1;
                EnBb_SetupRed(play, this);
                break;
            case ENBB_WHITE:
                thisx->naviEnemyId = 0x1D;
                thisx->colChkInfo.damageTable = &sDamageTableWhite;
                this->path = this->actionState;
                blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] =
                    blureInit.p1StartColor[3] = blureInit.p2StartColor[0] = blureInit.p2StartColor[1] =
                        blureInit.p2StartColor[2] = blureInit.p2StartColor[3] = blureInit.p1EndColor[0] =
                            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] =
                                blureInit.p2EndColor[1] = blureInit.p2EndColor[2] = 255;

                blureInit.p1EndColor[3] = 0;
                blureInit.p2EndColor[3] = 0;
                blureInit.elemDuration = 16;
                blureInit.unkFlag = 0;
                blureInit.calcMode = 2;

                Effect_Add(play, &this->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
                EnBb_SetupWhite(play, this);
                EnBb_SetWaypoint(this, play);
                EnBb_FaceWaypoint(this);
                thisx->flags |= ACTOR_FLAG_ARROW_DRAGGABLE;
                break;
            case ENBB_GREEN_BIG:
                this->path = this->actionState >> 4;
                this->collider.elements[0].dim.modelSphere.radius = 0x16;
                Actor_SetScale(thisx, 0.03f);
            case ENBB_GREEN:
                thisx->naviEnemyId = 0x1E;
                this->bobSize = (this->actionState & 0xF) * 20.0f;
                thisx->colChkInfo.damageTable = &sDamageTableBlueGreen;
                this->flameEnvColor.g = 255;
                thisx->colChkInfo.health = 1;

                EnBb_InitGreen(this, play);
                break;
        }
        thisx->focus.pos = thisx->world.pos;
    } else {
        EnBb_SetupFlameTrail(this);
    }
    this->collider.elements[0].dim.worldSphere.radius =
        this->collider.elements[0].dim.modelSphere.radius * this->collider.elements[0].dim.scale;
}

void EnBb_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;

    Collider_DestroyJntSph(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnBb_SetupFlameTrail(EnBb* this) {
    this->action = BB_FLAME_TRAIL;
    this->moveMode = BBMOVE_NOCLIP;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.speedXZ = 0.0f;
    EnBb_SetupAction(this, EnBb_FlameTrail);
}

void EnBb_FlameTrail(EnBb* this, PlayState* play) {
    if (this->actor.params == ENBB_KILL_TRAIL) {
        if (this->actor.parent == NULL) {
            EnBb_SetupDeath(this, play);
        }
    } else {
        if (this->timer == 0) {
            if (((EnBb*)this->targetActor)->flameScaleY != 0.0f) {
                Math_SmoothStepToF(&this->flameScaleY, this->actor.scale.y, 1.0f, this->actor.scale.y * 0.1f, 0.0f);
                Math_SmoothStepToF(&this->flameScaleX, this->actor.scale.z, 1.0f, this->actor.scale.z * 0.1f, 0.0f);
                if (this->flamePrimAlpha != this->vTrailMaxAlpha) {
                    this->flamePrimAlpha += 10;
                    if (this->vTrailMaxAlpha < this->flamePrimAlpha) {
                        this->flamePrimAlpha = this->vTrailMaxAlpha;
                    }
                }
            } else {
                if (!this->flamePrimAlpha) {
                    Actor_Kill(&this->actor);
                    return;
                } else if (this->flamePrimAlpha <= 20) {
                    this->flamePrimAlpha = 0;
                } else {
                    this->flamePrimAlpha -= 20;
                }
            }
            this->actor.world.pos = this->actor.parent->prevPos;
        } else {
            this->timer--;
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.parent->world.rot.y;
        }
    }
    if (this->actor.parent != NULL) {
        this->actor.velocity.y = this->actor.parent->velocity.y;
    }
}

void EnBb_SetupDeath(EnBb* this, PlayState* play) {
    if (this->actor.params <= ENBB_BLUE) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.speedXZ = -7.0f;
        this->timer = 5;
        this->actor.shape.rot.x += 0x4E20;
        EffectSsDeadSound_SpawnStationary(play, &this->actor.projectedPos, NA_SE_EN_BUBLE_DEAD, 1, 1, 0x28);
    }
    this->action = BB_KILL;
    EnBb_SetupAction(this, EnBb_Death);

    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

void EnBb_Death(EnBb* this, PlayState* play) {
    s16 enpartType = 3;
    Vec3f sp40 = { 0.0f, 0.5f, 0.0f };
    Vec3f sp34 = { 0.0f, 0.0f, 0.0f };

    if (this->actor.params <= ENBB_BLUE) {
        Math_SmoothStepToF(&this->flameScaleY, 0.0f, 1.0f, 30.0f, 0.0f);
        Math_SmoothStepToF(&this->flameScaleX, 0.0f, 1.0f, 30.0f, 0.0f);
        if (this->timer != 0) {
            this->timer--;
            this->actor.shape.rot.x -= 0x4E20;
            return;
        }

        if (this->bodyBreak.val == BODYBREAK_STATUS_FINISHED) {
            BodyBreak_Alloc(&this->bodyBreak, 12, play);
        }

        if ((this->dmgEffect == 7) || (this->dmgEffect == 5)) {
            enpartType = 11;
        }

        if (!BodyBreak_SpawnParts(&this->actor, &this->bodyBreak, play, enpartType)) {
            return;
        }
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xD0);
    } else {
        if (this->flamePrimAlpha) {
            if (this->flamePrimAlpha <= 20) {
                this->flamePrimAlpha = 0;
            } else {
                this->flamePrimAlpha -= 20;
            }
            return;
        }
    }
    Actor_Kill(&this->actor);
}

void EnBb_SetupDamage(EnBb* this) {
    this->action = BB_DAMAGE;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
    if (this->actor.params > ENBB_GREEN) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if ((this->actor.bgCheckFlags & 8) == 0) {
            this->actor.speedXZ = -7.0f;
        }
        this->actor.shape.yOffset = 1500.0f;
    }
    if (this->actor.params == ENBB_RED) {
        EnBb_KillFlameTrail(this);
    }
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0xC);
    this->timer = 5;
    EnBb_SetupAction(this, EnBb_Damage);
}

void EnBb_Damage(EnBb* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f);
    if (this->actor.speedXZ == 0.0f) {
        this->actor.shape.yOffset = 200.0f;
        EnBb_SetupDown(this);
    }
}

void EnBb_SetupBlue(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
    this->actor.speedXZ = (Rand_ZeroOne() * 0.5f) + 0.5f;
    this->timer = (Rand_ZeroOne() * 20.0f) + 40.0f;
    this->unk_264 = (Rand_ZeroOne() * 30.0f) + 180.0f;
    this->targetActor = NULL;
    this->action = BB_BLUE;
    EnBb_SetupAction(this, EnBb_Blue);
}

void EnBb_Blue(EnBb* this, PlayState* play) {
    Actor* explosive;
    s16 moveYawToWall;
    s16 thisYawToWall;
    s16 afterHitAngle;

    Math_SmoothStepToF(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
    Math_SmoothStepToF(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
        Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight + 50.0f + this->flyHeightMod, 1.0f, 0.5f,
                           0.0f);
    }
    SkelAnime_Update(&this->skelAnime);
    if (Math_CosF(this->bobPhase) == 0.0f) {
        if (this->charge) {
            this->bobSpeedMod = Rand_ZeroOne() * 2.0f;
        } else {
            this->bobSpeedMod = Rand_ZeroOne() * 4.0f;
        }
    }
    this->actor.world.pos.y += Math_CosF(this->bobPhase) * (1.0f + this->bobSpeedMod);
    this->bobPhase += 0.2f;
    Math_SmoothStepToF(&this->actor.speedXZ, this->maxSpeed, 1.0f, 0.5f, 0.0f);

    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) > 300.0f) {
        this->vMoveAngleY = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        Math_SmoothStepToS(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x7D0, 0);
    } else {
        this->timer--;
        if (this->timer <= 0) {
            this->charge ^= true;
            this->flyHeightMod = (s16)(Math_CosF(this->bobPhase) * 10.0f);
            this->actor.speedXZ = 0.0f;
            if (this->charge && (this->targetActor == NULL)) {
                this->vMoveAngleY = this->actor.world.rot.y;
                if (this->actor.xzDistToPlayer < 200.0f) {
                    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000184);
                    this->vMoveAngleY = this->actor.yawTowardsPlayer;
                }
                this->maxSpeed = (Rand_ZeroOne() * 1.5f) + 6.0f;
                this->timer = (Rand_ZeroOne() * 5.0f) + 20.0f;
                this->actionState = BBBLUE_NORMAL;
            } else {
                Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
                this->maxSpeed = (Rand_ZeroOne() * 1.5f) + 1.0f;
                this->timer = (Rand_ZeroOne() * 20.0f) + 40.0f;
                this->vMoveAngleY = Math_SinF(this->bobPhase) * 65535.0f;
            }
        }
        if ((this->actor.xzDistToPlayer < 150.0f) && (this->actionState != BBBLUE_NORMAL)) {
            if (!this->charge) {
                Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000184);
                this->maxSpeed = (Rand_ZeroOne() * 1.5f) + 6.0f;
                this->timer = (Rand_ZeroOne() * 5.0f) + 20.0f;
                this->vMoveAngleY = this->actor.yawTowardsPlayer;
                this->actionState = this->charge = true; // Sets actionState to BBBLUE_AGGRO
            }
        } else if (this->actor.xzDistToPlayer < 200.0f) {
            this->vMoveAngleY = this->actor.yawTowardsPlayer;
        }
        if (this->targetActor == NULL) {
            explosive = EnBb_FindExplosive(play, this, 300.0f);
        } else if (this->targetActor->params == 0) {
            explosive = this->targetActor;
        } else {
            explosive = NULL;
        }
        if (explosive != NULL) {
            this->vMoveAngleY = Actor_WorldYawTowardActor(&this->actor, explosive);
            if ((this->vBombHopPhase == 0) && (explosive != this->targetActor)) {
                this->vBombHopPhase = -0x8000;
                this->targetActor = explosive;
                this->actor.speedXZ *= 0.5f;
            }
            Math_SmoothStepToS(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x1388, 0);
            Math_SmoothStepToF(&this->actor.world.pos.x, explosive->world.pos.x, 1.0f, 1.5f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.y, explosive->world.pos.y + 40.0f, 1.0f, 1.5f, 0.0f);
            Math_SmoothStepToF(&this->actor.world.pos.z, explosive->world.pos.z, 1.0f, 1.5f, 0.0f);
        } else {
            this->targetActor = NULL;
        }
        if (this->vBombHopPhase != 0) {
            this->actor.world.pos.y += -Math_CosS(this->vBombHopPhase) * 10.0f;
            this->vBombHopPhase += 0x1000;
            Math_SmoothStepToS(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x7D0, 0);
        }
        thisYawToWall = this->actor.wallYaw - this->actor.world.rot.y;
        moveYawToWall = this->actor.wallYaw - this->vMoveAngleY;
        if ((this->targetActor == NULL) && (this->actor.bgCheckFlags & 8) &&
            (ABS(thisYawToWall) > 0x4000 || ABS(moveYawToWall) > 0x4000)) {
            this->vMoveAngleY = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
            Math_SmoothStepToS(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0xBB8, 0);
        }
    }
    Math_SmoothStepToS(&this->actor.world.rot.y, this->vMoveAngleY, 1, 0x3E8, 0);
    if ((this->collider.base.acFlags & AC_HIT) || (this->collider.base.atFlags & AT_HIT)) {
        this->vMoveAngleY = this->actor.yawTowardsPlayer + 0x8000;
        if (this->collider.base.acFlags & AC_HIT) {
            afterHitAngle = -0x8000;
        } else {
            afterHitAngle = 0x4000;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_BITE);
            if (play->gameplayFrames & 1) {
                afterHitAngle = -0x4000;
            }
        }
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + afterHitAngle;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.atFlags &= ~AT_HIT;
    }

    if (this->maxSpeed >= 6.0f) {
        if ((s32)this->skelAnime.curFrame == 0 || (s32)this->skelAnime.curFrame == 5) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_MOUTH);
        } else if ((s32)this->skelAnime.curFrame == 2 || (s32)this->skelAnime.curFrame == 7) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_WING);
        }
    } else {
        if ((s32)this->skelAnime.curFrame == 5) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_WING);
        }
    }
    if (((s32)this->skelAnime.curFrame == 0) && (Rand_ZeroOne() < 0.1f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnBb_SetupDown(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
    this->action = BB_DOWN;
    this->timer = 200;
    this->actor.colorFilterTimer = 0;
    this->actor.bgCheckFlags &= ~1;
    this->actor.speedXZ = 3.0f;
    this->flameScaleX = 0.0f;
    this->flameScaleY = 0.0f;
    this->actor.gravity = -2.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_DOWN);
    EnBb_SetupAction(this, EnBb_Down);
}

void EnBb_Down(EnBb* this, PlayState* play) {
    s16 yawDiff = this->actor.world.rot.y - this->actor.wallYaw;

    SkelAnime_Update(&this->skelAnime);
    if (this->actor.bgCheckFlags & 8) {
        if (ABS(yawDiff) > 0x4000) {
            this->actor.world.rot.y = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
        }
        this->actor.bgCheckFlags &= ~8;
    }
    if (this->actor.bgCheckFlags & 3) {
        if (this->actor.params == ENBB_RED) {
            s32 floorType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

            if ((floorType == 2) || (floorType == 3) || (floorType == 9)) {
                this->moveMode = BBMOVE_HIDDEN;
                this->timer = 10;
                this->actionState++;
                this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                this->action = BB_RED;
                EnBb_SetupAction(this, EnBb_Red);
                return;
            }
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.7f;
        } else {
            this->actor.velocity.y = 10.0f;
        }
        this->actor.bgCheckFlags &= ~1;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
        Math_SmoothStepToS(&this->actor.world.rot.y, -this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if ((s32)this->skelAnime.curFrame == 5) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_WING);
    }
    if (this->timer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_UP);
        switch (this->actor.params) {
            case ENBB_BLUE:
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                EnBb_SetupBlue(this);
                break;
            case ENBB_RED:
                if (this->actor.velocity.y == 10.0f) {
                    EnBb_SetupRed(play, this);
                    EnBb_SpawnFlameTrail(play, this, true);
                }
                break;
            case ENBB_WHITE:
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                EnBb_SetupWhite(play, this);
                this->actor.world.pos.y -= 60.0f;
                break;
        }
    } else {
        this->timer--;
    }
}

void EnBb_SetupRed(PlayState* play, EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000184);
    if (this->action == BB_DOWN) {
        this->actor.speedXZ = 5.0f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 16.0f;
        this->actionState = BBRED_ATTACK;
        this->timer = 0;
        this->moveMode = BBMOVE_NORMAL;
        this->actor.bgCheckFlags &= ~1;
    } else {
        this->actor.colChkInfo.health = 4;
        this->timer = 0;
        this->actionState = BBRED_WAIT;
        this->moveMode = BBMOVE_HIDDEN;
        this->actor.world.pos.y -= 80.0f;
        this->actor.home.pos = this->actor.world.pos;
        this->actor.velocity.y = this->actor.gravity = this->actor.speedXZ = 0.0f;
        this->actor.bgCheckFlags &= ~1;
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    }
    this->action = BB_RED;
    EnBb_SetupAction(this, EnBb_Red);
}

void EnBb_Red(EnBb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 floorType;
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    switch (this->actionState) {
        case BBRED_WAIT:
            if ((Actor_WorldDistXYZToActor(&this->actor, &player->actor) <= 250.0f) && (ABS(yawDiff) <= 0x4000) &&
                (this->timer == 0)) {
                this->actor.speedXZ = 5.0f;
                this->actor.gravity = -1.0f;
                this->actor.velocity.y = 18.0f;
                this->moveMode = BBMOVE_NOCLIP;
                this->timer = 7;
                this->actor.bgCheckFlags &= ~1;
                this->actionState++;
                EnBb_SpawnFlameTrail(play, this, false);
            }
            break;
        case BBRED_ATTACK:
            if (this->timer == 0) {
                this->moveMode = BBMOVE_NORMAL;
                this->actor.flags |= ACTOR_FLAG_TARGETABLE;
            }
            this->bobPhase += Rand_ZeroOne();
            Math_SmoothStepToF(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
            Math_SmoothStepToF(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
            if (this->actor.bgCheckFlags & 8) {
                yawDiff = this->actor.world.rot.y - this->actor.wallYaw;
                if (ABS(yawDiff) > 0x4000) {
                    this->actor.world.rot.y =
                        this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
                }
                this->actor.bgCheckFlags &= ~8;
            }
            if (this->actor.bgCheckFlags & 1) {
                floorType = func_80041D4C(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
                if ((floorType == 2) || (floorType == 3) || (floorType == 9)) {
                    this->moveMode = BBMOVE_HIDDEN;
                    this->timer = 10;
                    this->actionState++;
                    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                } else {
                    this->actor.velocity.y *= -1.06f;
                    if (this->actor.velocity.y > 13.0f) {
                        this->actor.velocity.y = 13.0f;
                    }
                    this->actor.world.rot.y = Math_SinF(this->bobPhase) * 65535.0f;
                }
                this->actor.bgCheckFlags &= ~1;
            }
            this->actor.shape.rot.y = this->actor.world.rot.y;
            if (Actor_GetCollidedExplosive(play, &this->collider.base) != NULL) {
                EnBb_SetupDown(this);
            }
            break;
        case BBRED_HIDE:
            if (this->timer == 0) {
                this->actor.speedXZ = 0.0f;
                this->actor.gravity = 0.0f;
                this->actor.velocity.y = 0.0f;
                this->actionState = BBRED_WAIT;
                this->timer = 120;
                this->actor.world.pos = this->actor.home.pos;
                this->actor.shape.rot = this->actor.world.rot = this->actor.home.rot;
                EnBb_KillFlameTrail(this);
            }
            break;
    }
    if (this->actionState != BBRED_WAIT) {
        if (((s32)this->skelAnime.curFrame == 0) || ((s32)this->skelAnime.curFrame == 5)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_MOUTH);
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLEFALL_FIRE - SFX_FLAG);
    }
}

void EnBb_FaceWaypoint(EnBb* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &this->waypointPos);
}

void EnBb_SetWaypoint(EnBb* this, PlayState* play) {
    Path* path = &play->setupPathList[this->path];
    Vec3s* point;

    if (this->waypoint == (s16)(path->count - 1)) {
        this->waypoint = 0;
    } else {
        this->waypoint++;
    }
    point = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
    this->waypointPos.x = point->x;
    this->waypointPos.y = point->y;
    this->waypointPos.z = point->z;
}

void EnBb_SetupWhite(PlayState* play, EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
    this->actor.speedXZ = 0.0f;
    this->actor.world.pos.y += 60.0f;
    this->flameScaleX = 100.0f;
    this->action = BB_WHITE;
    this->waypoint = 0;
    this->timer = (Rand_ZeroOne() * 30.0f) + 40.0f;
    this->maxSpeed = 7.0f;
    EnBb_SetupAction(this, EnBb_White);
}

void EnBb_White(EnBb* this, PlayState* play) {
    if (this->actor.speedXZ == 0.0f) {
        f32 distL1;
        f32 vx;
        f32 vz;
        s16 pitch = Math_Vec3f_Pitch(&this->actor.world.pos, &this->waypointPos);
        f32 vy = Math_SinS(pitch) * this->maxSpeed;
        f32 vxz = Math_CosS(pitch) * this->maxSpeed;

        vx = Math_SinS(this->actor.shape.rot.y) * vxz;
        vz = Math_CosS(this->actor.shape.rot.y) * vxz;
        distL1 = Math_SmoothStepToF(&this->actor.world.pos.x, this->waypointPos.x, 1.0f, ABS(vx), 0.0f);
        distL1 += Math_SmoothStepToF(&this->actor.world.pos.y, this->waypointPos.y, 1.0f, ABS(vy), 0.0f);
        distL1 += Math_SmoothStepToF(&this->actor.world.pos.z, this->waypointPos.z, 1.0f, ABS(vz), 0.0f);
        this->bobPhase += (0.05f + (Rand_ZeroOne() * 0.01f));
        if (distL1 == 0.0f) {
            this->timer--;
            if (this->timer == 0) {
                EnBb_SetWaypoint(this, play);
                EnBb_FaceWaypoint(this);
                Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000184);
                this->timer = Rand_ZeroOne() * 30.0f + 40.0f;
            } else {
                if (this->moveMode != BBMOVE_NORMAL) {
                    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
                }
                this->actor.world.rot.y += 0x1F40;
            }
            this->moveMode = BBMOVE_NORMAL;
            this->maxSpeed = 0.0f;
        } else {
            this->moveMode = BBMOVE_NOCLIP;
            this->maxSpeed = 10.0f;
        }
        if (this->collider.base.atFlags & AT_HIT) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_BITE);
            this->collider.base.atFlags &= ~AT_HIT;
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    } else if (Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
        EnBb_FaceWaypoint(this);
    }
    SkelAnime_Update(&this->skelAnime);
    if (((s32)this->skelAnime.curFrame == 0) && (Rand_ZeroOne() <= 0.1f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }

    if ((this->maxSpeed != 0.0f) && (((s32)this->skelAnime.curFrame == 0) || ((s32)this->skelAnime.curFrame == 5))) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_MOUTH);
    } else if (((s32)this->skelAnime.curFrame == 2) || ((s32)this->skelAnime.curFrame == 7)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_WING);
    }
}

void EnBb_InitGreen(EnBb* this, PlayState* play) {
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };

    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
    this->moveMode = BBMOVE_NOCLIP;
    this->actionState = BBGREEN_FLAME_ON;
    this->bobPhase = Rand_ZeroOne();
    this->actor.shape.rot.x = this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.params == ENBB_GREEN_BIG) {
        EnBb_SetWaypoint(this, play);
        EnBb_FaceWaypoint(this);
    }
    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_RotateZ(this->bobPhase, MTXMODE_APPLY);
    bobOffset.y = this->bobSize;
    Matrix_MultVec3f(&bobOffset, &this->actor.world.pos);
    this->targetActor = NULL;
    this->action = BB_GREEN;
    this->actor.speedXZ = 0.0f;
    this->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
    EnBb_SetupAction(this, EnBb_Green);
}

void EnBb_SetupGreen(EnBb* this) {
    Animation_PlayLoop(&this->skelAnime, &object_Bb_Anim_000444);
    this->moveMode = BBMOVE_NOCLIP;
    this->actionState = BBGREEN_FLAME_ON;
    this->targetActor = NULL;
    this->action = BB_GREEN;
    this->actor.speedXZ = 0.0f;
    this->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    EnBb_SetupAction(this, EnBb_Green);
}

void EnBb_Green(EnBb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f bobOffset = { 0.0f, 0.0f, 0.0f };
    Vec3f nextPos = player->actor.world.pos;

    nextPos.y += 30.0f;
    if (this->actor.params == ENBB_GREEN_BIG) {
        if (this->actor.speedXZ == 0.0f) {
            s16 pitch = Math_Vec3f_Pitch(&this->actor.home.pos, &this->waypointPos);
            s16 yaw = Math_Vec3f_Yaw(&this->actor.home.pos, &this->waypointPos);
            f32 vy = Math_SinS(pitch) * this->maxSpeed;
            f32 vxz = Math_CosS(pitch) * this->maxSpeed;
            f32 vz;
            f32 vx;
            f32 distL1;

            Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 1, 0x3E8, 0);
            vx = Math_SinS(this->actor.world.rot.y) * vxz;
            distL1 = Math_CosS(this->actor.world.rot.y) * vxz;
            vz = Math_SmoothStepToF(&this->actor.home.pos.x, this->waypointPos.x, 1.0f, ABS(vx), 0.0f);
            vz += Math_SmoothStepToF(&this->actor.home.pos.y, this->waypointPos.y, 1.0f, ABS(vy), 0.0f);
            vz += Math_SmoothStepToF(&this->actor.home.pos.z, this->waypointPos.z, 1.0f, ABS(distL1), 0.0f);
            this->bobPhase += (0.05f + (Rand_ZeroOne() * 0.01f));
            if (vz == 0.0f) {
                EnBb_SetWaypoint(this, play);
            }
            this->moveMode = BBMOVE_NOCLIP;
            this->maxSpeed = 10.0f;
            if (this->collider.base.atFlags & AT_HIT) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_BITE);
                this->collider.base.atFlags &= ~AT_HIT;
            }
            if (Math_CosF(this->bobPhase) == 0.0f) {
                if (this->charge) {
                    this->bobSpeedMod = Rand_ZeroOne();
                } else {
                    this->bobSpeedMod = Rand_ZeroOne() * 3.0f;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_LAUGH);
                }
            }
            this->actor.shape.rot.y = this->actor.world.rot.y;
        } else if (Math_SmoothStepToF(&this->actor.speedXZ, 0.0f, 1.0f, 0.5f, 0.0f) == 0.0f) {
            EnBb_FaceWaypoint(this);
        }
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.x, Math_Vec3f_Pitch(&this->actor.world.pos, &nextPos), 1, 0xFA0, 0);
    }
    SkelAnime_Update(&this->skelAnime);
    if (Math_CosF(this->bobPhase) <= 0.002f) {
        this->bobSpeedMod = Rand_ZeroOne() * 0.05f;
    }
    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
    Matrix_RotateZ(this->bobPhase, MTXMODE_APPLY);
    bobOffset.y = this->bobSize;
    Matrix_MultVec3f(&bobOffset, &nextPos);
    Math_SmoothStepToF(&this->actor.world.pos.x, nextPos.x, 1.0f, this->bobPhase * 0.75f, 0.0f);
    Math_SmoothStepToF(&this->actor.world.pos.y, nextPos.y, 1.0f, this->bobPhase * 0.75f, 0.0f);
    Math_SmoothStepToF(&this->actor.world.pos.z, nextPos.z, 1.0f, this->bobPhase * 0.75f, 0.0f);
    this->bobPhase += 0.1f + this->bobSpeedMod;
    if (Actor_GetCollidedExplosive(play, &this->collider.base) || (--this->vFlameTimer == 0)) {
        this->actionState++;
        this->timer = (Rand_ZeroOne() * 30.0f) + 60.0f;
        if (this->vFlameTimer != 0) {
            this->collider.base.acFlags &= ~AC_HIT;
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_DOWN);
    }
    if (this->actionState != BBGREEN_FLAME_ON) {
        this->timer--;
        if (this->timer == 0) {
            this->actionState = BBGREEN_FLAME_ON;
            this->vFlameTimer = (Rand_ZeroOne() * 30.0f) + 180.0f;
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_UP);
        }
        Math_SmoothStepToF(&this->flameScaleY, 0.0f, 1.0f, 10.0f, 0.0f);
        Math_SmoothStepToF(&this->flameScaleX, 0.0f, 1.0f, 10.0f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->flameScaleY, 80.0f, 1.0f, 10.0f, 0.0f);
        Math_SmoothStepToF(&this->flameScaleX, 100.0f, 1.0f, 10.0f, 0.0f);
    }
    if ((s32)this->skelAnime.curFrame == 5) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_WING);
    }
    if (((s32)this->skelAnime.curFrame == 0) && (Rand_ZeroOne() < 0.1f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_LAUGH);
    }
}

void EnBb_SetupStunned(EnBb* this) {
    this->action = BB_STUNNED;
    if (this->actor.params != ENBB_WHITE) {
        if (this->actor.params != ENBB_RED) {
            if (this->actor.params > ENBB_GREEN) {
                this->actor.gravity = -2.0f;
                this->actor.shape.yOffset = 1500.0f;
            }
            this->actor.speedXZ = 0.0f;
            this->flameScaleX = 0.0f;
            this->flameScaleY = 0.0f;
        } else {
            EnBb_KillFlameTrail(this);
        }
    }
    switch (this->dmgEffect) {
        case 8:
            Actor_SetColorFilter(&this->actor, -0x8000, 0xC8, 0, 0x50);
            break;
        case 9:
            this->fireIceTimer = 0x30;
        case 15:
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Actor_SetColorFilter(&this->actor, 0, 0xB4, 0, 0x50);
            break;
    }
    this->actor.bgCheckFlags &= ~1;
    EnBb_SetupAction(this, EnBb_Stunned);
}

void EnBb_Stunned(EnBb* this, PlayState* play) {
    s16 yawDiff = this->actor.world.rot.y - this->actor.wallYaw;

    if (this->actor.bgCheckFlags & 8) {
        if (ABS(yawDiff) > 0x4000) {
            this->actor.world.rot.y = this->actor.wallYaw + this->actor.wallYaw - this->actor.world.rot.y - 0x8000;
        }
        this->actor.bgCheckFlags &= ~8;
    }
    if (this->actor.bgCheckFlags & 2) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.4f;
        } else {
            this->actor.velocity.y = 0.0f;
        }
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
    }
    if (this->actor.colorFilterTimer == 0) {
        this->actor.shape.yOffset = 200.0f;
        if (this->actor.colChkInfo.health != 0) {
            if ((this->actor.params == ENBB_GREEN) || (this->actor.params == ENBB_GREEN_BIG)) {
                EnBb_SetupGreen(this);
            } else if (this->actor.params == ENBB_WHITE) {
                this->action = BB_WHITE;
                EnBb_SetupAction(this, EnBb_White);
            } else {
                EnBb_SetupDown(this);
            }
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
            EnBb_SetupDeath(this, play);
        }
    }
}

void EnBb_CollisionCheck(EnBb* this, PlayState* play) {
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;
        if (this->action != BB_DOWN) {
            if (this->actor.params >= ENBB_RED) {
                this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
                if (this->actor.params == ENBB_RED) {
                    EnBb_KillFlameTrail(this);
                }
                EnBb_SetupDown(this);
                return;
            }
            this->actionVar2 = 1;
        }
    }
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->dmgEffect = this->actor.colChkInfo.damageEffect;
        Actor_SetDropFlag(&this->actor, &this->collider.elements[0].info, 0);
        switch (this->dmgEffect) {
            case 7:
                this->actor.freezeTimer = this->collider.elements[0].info.acHitInfo->toucher.damage;
            case 5:
                this->fireIceTimer = 0x30;
                //! @bug
                //! Setting fireIceTimer here without calling Actor_SetColorFilter causes a crash if the bubble is
                //! killed in a single hit by an attack with damage effect 5 or 7 while actor updating is halted. Using
                //! Din's Fire on a white bubble will do just that. The mechanism is complex and described below.
                goto block_15;
            case 6:
                this->actor.freezeTimer = this->collider.elements[0].info.acHitInfo->toucher.damage;
                break;
            case 8:
            case 9:
            case 15:
                if (this->action != BB_STUNNED) {
                    Actor_ApplyDamage(&this->actor);
                    EnBb_SetupStunned(this);
                }
                break;
            default:
            block_15:
                if ((this->dmgEffect == 14) || (this->dmgEffect == 12) || (this->dmgEffect == 11) ||
                    (this->dmgEffect == 10) || (this->dmgEffect == 7) || (this->dmgEffect == 5)) {
                    if ((this->action != BB_DOWN) || (this->timer < 190)) {
                        Actor_ApplyDamage(&this->actor);
                    }
                    if ((this->action != BB_DOWN) && (this->actor.params != ENBB_WHITE)) {
                        EnBb_SetupDown(this);
                    }
                } else {
                    if (((this->action == BB_DOWN) && (this->timer < 190)) ||
                        ((this->actor.params != ENBB_WHITE) && (this->flameScaleX < 20.0f))) {
                        Actor_ApplyDamage(&this->actor);
                    } else {
                        this->collider.base.acFlags |= AC_HIT;
                    }
                }
                if (this->actor.colChkInfo.health == 0) {
                    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                    if (this->actor.params == ENBB_RED) {
                        EnBb_KillFlameTrail(this);
                    }
                    EnBb_SetupDeath(this, play);
                    //! @bug
                    //! Because Din's Fire kills the bubble in a single hit, Actor_SetColorFilter is never called and
                    //! colorFilterParams is never set. And because Din's Fire halts updating during its cutscene,
                    //! EnBb_Death doesn't kill the bubble on the next frame like it should. This combines with
                    //! the bug in EnBb_Draw below to crash the game.
                } else if ((this->actor.params == ENBB_WHITE) &&
                           ((this->action == BB_WHITE) || (this->action == BB_STUNNED))) {
                    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0xC);
                    this->actor.speedXZ = -8.0f;
                    this->maxSpeed = 0.0f;
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
                } else if (((this->action == BB_DOWN) && (this->timer < 190)) ||
                           ((this->actor.params != ENBB_WHITE) && (this->flameScaleX < 20.0f))) {
                    EnBb_SetupDamage(this);
                }
            case 13:
                break;
        }
    }
}

void EnBb_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBb* this = (EnBb*)thisx;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    Vec3f sp40 = { 0.0f, -0.6f, 0.0f };
    Color_RGBA8 sp3C = { 0, 0, 255, 255 };
    Color_RGBA8 sp38 = { 0, 0, 0, 0 };
    f32 sp34 = -15.0f;

    if (this->actor.params <= ENBB_BLUE) {
        EnBb_CollisionCheck(this, play);
    }
    if (this->actor.colChkInfo.damageEffect != 0xD) {
        this->actionFunc(this, play);
        if ((this->actor.params <= ENBB_BLUE) && (this->actor.speedXZ >= -6.0f) &&
            ((this->actor.flags & ACTOR_FLAG_DRAGGED_BY_ARROW) == 0)) {
            Actor_MoveForward(&this->actor);
        }
        if (this->moveMode == BBMOVE_NORMAL) {
            if ((this->actor.world.pos.y - 20.0f) <= this->actor.floorHeight) {
                sp34 = 20.0f;
            }
            Actor_UpdateBgCheckInfo(play, &this->actor, sp34, 25.0f, 20.0f, 5);
        }
        this->actor.focus.pos = this->actor.world.pos;
        this->collider.elements->dim.worldSphere.center.x = this->actor.world.pos.x;
        this->collider.elements->dim.worldSphere.center.y =
            this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y);
        this->collider.elements->dim.worldSphere.center.z = this->actor.world.pos.z;

        if ((this->action > BB_KILL) && ((this->actor.speedXZ != 0.0f) || (this->action == BB_GREEN))) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        if ((this->action > BB_FLAME_TRAIL) &&
            ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) &&
            (this->moveMode != BBMOVE_HIDDEN)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void EnBb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnBb* this = (EnBb*)thisx;

    BodyBreak_SetInfo(&this->bodyBreak, limbIndex, 4, 15, 15, dList, BODYBREAK_OBJECT_DEFAULT);
}

static Vec3f sFireIceOffsets[] = {
    { 13.0f, 10.0f, 0.0f }, { 5.0f, 25.0f, 5.0f },   { -5.0f, 25.0f, 5.0f },  { -13.0f, 10.0f, 0.0f },
    { 5.0f, 25.0f, -5.0f }, { -5.0f, 25.0f, -5.0f }, { 0.0f, 10.0f, -13.0f }, { 5.0f, 0.0f, 5.0f },
    { 5.0f, 0.0f, -5.0f },  { 0.0f, 10.0f, 13.0f },  { -5.0f, 0.0f, 5.0f },   { -5.0f, 0.0f, -5.0f },
};

void EnBb_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBb* this = (EnBb*)thisx;
    Vec3f blureBase1 = { 0.0f, 5000.0f, 0.0f };
    Vec3f blureBase2 = { 0.0f, 2000.0f, 0.0f };
    Vec3f blureVtx1;
    Vec3f blureVtx2;

    OPEN_DISPS(play->state.gfxCtx);

    blureBase1.z = this->maxSpeed * 80.0f;
    blureBase2.z = this->maxSpeed * 80.0f;
    if (this->moveMode != BBMOVE_HIDDEN) {
        if (this->actor.params <= ENBB_BLUE) {
            Gfx_SetupDL_25Opa(play->state.gfxCtx);
            SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, EnBb_PostLimbDraw,
                              this);

            if (this->fireIceTimer != 0) {
                this->actor.colorFilterTimer++;
                //! @bug:
                //! The purpose of this is to counteract Actor_UpdateAll decrementing colorFilterTimer. However,
                //! the above bugs mean unk_2A8 can be nonzero without damage effects ever having been set.
                //! This routine will then increment colorFilterTimer, and on the next frame Actor_Draw will try
                //! to draw the unset colorFilterParams. This causes a divide-by-zero error, crashing the game.
                this->fireIceTimer--;
                if ((this->fireIceTimer % 4) == 0) {
                    Vec3f sp70;
                    s32 index = this->fireIceTimer >> 2;

                    sp70.x = this->actor.world.pos.x + sFireIceOffsets[index].x;
                    sp70.y = this->actor.world.pos.y + sFireIceOffsets[index].y;
                    sp70.z = this->actor.world.pos.z + sFireIceOffsets[index].z;

                    if ((this->dmgEffect != 7) && (this->dmgEffect != 5)) {
                        EffectSsEnIce_SpawnFlyingVec3f(play, &this->actor, &sp70, 0x96, 0x96, 0x96, 0xFA, 0xEB,
                                                       0xF5, 0xFF, 0.8f);
                    } else {
                        sp70.y -= 17.0f;
                        EffectSsEnFire_SpawnVec3f(play, &this->actor, &sp70, 0x28, 1, 0, -1);
                    }
                }
            }
            Matrix_Translate(0.0f, this->flameScaleX * -40.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(0.0f, -40.0f, 0.0f, MTXMODE_APPLY);
        }
        if (this->actor.params != ENBB_WHITE) {
            Gfx_SetupDL_25Xlu(play->state.gfxCtx);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0,
                                        ((play->gameplayFrames + (this->flameScrollMod * 10)) *
                                         (-20 - (this->flameScrollMod * -2))) %
                                            0x200,
                                        0x20, 0x80));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, this->flamePrimBlue, this->flamePrimAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, this->flameEnvColor.r, this->flameEnvColor.g, this->flameEnvColor.b, 0);
            Matrix_RotateY(((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000)) *
                               (M_PI / 0x8000),
                           MTXMODE_APPLY);
            Matrix_Scale(this->flameScaleX * 0.01f, this->flameScaleY * 0.01f, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        } else {
            Matrix_MultVec3f(&blureBase1, &blureVtx1);
            Matrix_MultVec3f(&blureBase2, &blureVtx2);
            if ((this->maxSpeed != 0.0f) && (this->action == BB_WHITE) && !(play->gameplayFrames & 1) &&
                (this->actor.colChkInfo.health != 0)) {
                EffectBlure_AddVertex(Effect_GetByIndex(this->blureIdx), &blureVtx1, &blureVtx2);
            } else if (this->action != BB_WHITE) {
                EffectBlure_AddSpace(Effect_GetByIndex(this->blureIdx));
            }
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

/*
 * File: z_bg_po_event.c
 * Overlay: ovl_Bg_Po_Event
 * Description: Poe sisters' paintings and puzzle blocks
 */

#include "z_bg_po_event.h"
#include "objects/object_po_sisters/object_po_sisters.h"

#define FLAGS 0

void BgPoEvent_Init(Actor* thisx, GlobalContext* globalCtx);
void BgPoEvent_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgPoEvent_Update(Actor* thisx, GlobalContext* globalCtx);
void BgPoEvent_Draw(Actor* thisx, GlobalContext* globalCtx);
void BgPoEvent_Reset(void);

void BgPoEvent_BlockWait(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockShake(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockFall(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockIdle(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockPush(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockReset(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_BlockSolved(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_AmyWait(BgPoEvent* this, GlobalContext* globalCtx); // Amy is the green Poe
void BgPoEvent_AmyPuzzle(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_PaintingEmpty(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_PaintingAppear(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_PaintingPresent(BgPoEvent* this, GlobalContext* globalCtx);
void BgPoEvent_PaintingBurn(BgPoEvent* this, GlobalContext* globalCtx);

const ActorInit Bg_Po_Event_InitVars = {
    ACTOR_BG_PO_EVENT,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_PO_SISTERS,
    sizeof(BgPoEvent),
    (ActorFunc)BgPoEvent_Init,
    (ActorFunc)BgPoEvent_Destroy,
    (ActorFunc)BgPoEvent_Update,
    (ActorFunc)BgPoEvent_Draw,
    (ActorResetFunc)BgPoEvent_Reset,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f } } },
    },
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 25.0f, 33.0f, 0.0f }, { -25.0f, -33.0f, 0.0f }, { 25.0f, -33.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static u8 sBlocksAtRest = 0;

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

static u8 sPuzzleState;

void BgPoEvent_InitPaintings(BgPoEvent* this, GlobalContext* globalCtx) {
    static s16 paintingPosX[] = { -1302, -866, 1421, 985 };
    static s16 paintingPosY[] = { 1107, 1091 };
    static s16 paintingPosZ[] = { -3384, -3252 };
    ColliderTrisElementInit* item;
    Vec3f* vtxVec;
    s32 i1;
    s32 i2;
    Vec3f sp9C[3];
    f32 coss;
    f32 sins;
    f32 scaleY;
    s32 phi_t2;
    Actor* newPainting;

    sins = Math_SinS(this->dyna.actor.shape.rot.y);
    coss = Math_CosS(this->dyna.actor.shape.rot.y);
    if (this->type == 4) {
        sins *= 2.4f;
        scaleY = 1.818f;
        coss *= 2.4f;
    } else {
        scaleY = 1.0f;
    }
    for (i1 = 0; i1 < sTrisInit.count; i1++) {
        item = &sTrisInit.elements[i1];
        if (1) {} // This section looks like a macro of some sort.
        for (i2 = 0; i2 < 3; i2++) {
            vtxVec = &item->dim.vtx[i2];
            sp9C[i2].x = (vtxVec->x * coss) + (this->dyna.actor.home.pos.x + (sins * vtxVec->z));
            sp9C[i2].y = (vtxVec->y * scaleY) + this->dyna.actor.home.pos.y;
            sp9C[i2].z = this->dyna.actor.home.pos.z + (coss * vtxVec->z) - (vtxVec->x * sins);
        }
        Collider_SetTrisVertices(&this->collider, i1, &sp9C[0], &sp9C[1], &sp9C[2]);
    }
    if ((this->type != 4) && (this->index != 2)) {
        phi_t2 = (this->type == 2) ? this->index : this->index + 2;
        newPainting = Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_BG_PO_EVENT,
                                         paintingPosX[phi_t2], paintingPosY[this->index], paintingPosZ[this->index], 0,
                                         this->dyna.actor.shape.rot.y + 0x8000, 0,
                                         ((this->index + 1) << 0xC) + (this->type << 8) + this->dyna.actor.params);
        if (newPainting == NULL) {
            Actor_Kill(&this->dyna.actor);
            return;
        }
        if (this->index == 0) {
            if (this->dyna.actor.child->child == NULL) {
                Actor_Kill(&this->dyna.actor);
                return;
            }
            this->dyna.actor.parent = this->dyna.actor.child->child;
            this->dyna.actor.child->child->child = &this->dyna.actor;
        }
    }
    this->timer = 0;
    if (this->type == 4) {
        sPuzzleState = 0;
        this->actionFunc = BgPoEvent_AmyWait;
    } else {
        sPuzzleState = (s32)(Rand_ZeroOne() * 3.0f) % 3;
        this->actionFunc = BgPoEvent_PaintingEmpty;
    }
}

void BgPoEvent_InitBlocks(BgPoEvent* this, GlobalContext* globalCtx) {
    static s16 blockPosX[] = { 2149, 1969, 1909 };
    static s16 blockPosZ[] = { -1410, -1350, -1530 };
    Actor* newBlock;
    CollisionHeader* colHeader = NULL;
    s32 bgId;

    this->dyna.actor.flags |= ACTOR_FLAG_4 | ACTOR_FLAG_5;
    CollisionHeader_GetVirtual(&gPoSistersAmyBlockCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    if ((this->type == 0) && (this->index != 3)) {
        newBlock = Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_BG_PO_EVENT,
                                      blockPosX[this->index], this->dyna.actor.world.pos.y, blockPosZ[this->index], 0,
                                      this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z - 0x4000,
                                      ((this->index + 1) << 0xC) + (this->type << 8) + this->dyna.actor.params);
        if (newBlock == NULL) {
            Actor_Kill(&this->dyna.actor);
            return;
        }
        if (this->index == 0) {
            if (this->dyna.actor.child->child == NULL) {
                Actor_Kill(&this->dyna.actor);
                return;
            }
            if (this->dyna.actor.child->child->child == NULL) {
                Actor_Kill(&this->dyna.actor);
                Actor_Kill(this->dyna.actor.child);
                return;
            }
            this->dyna.actor.parent = this->dyna.actor.child->child->child;
            this->dyna.actor.child->child->child->child = &this->dyna.actor;
        }
    }
    this->dyna.actor.world.pos.y = 833.0f;
    this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor4(&globalCtx->colCtx, &this->dyna.actor.floorPoly, &bgId,
                                                               &this->dyna.actor, &this->dyna.actor.world.pos);
    this->actionFunc = BgPoEvent_BlockWait;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgPoEvent_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->type = (thisx->params >> 8) & 0xF;
    this->index = (thisx->params >> 0xC) & 0xF;
    thisx->params &= 0x3F;

    if (this->type >= 2) {
        Collider_InitTris(globalCtx, &this->collider);
        Collider_SetTris(globalCtx, &this->collider, thisx, &sTrisInit, this->colliderItems);
        if (Flags_GetSwitch(globalCtx, thisx->params)) {
            Actor_Kill(thisx);
        } else {
            BgPoEvent_InitPaintings(this, globalCtx);
        }
    } else {
        DynaPolyActor_Init(&this->dyna, DPM_UNK);
        if (Flags_GetSwitch(globalCtx, thisx->params)) {
            Actor_Kill(thisx);
        } else {
            BgPoEvent_InitBlocks(this, globalCtx);
        }
    }
}

void BgPoEvent_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    if (this->type >= 2) {
        Collider_DestroyTris(globalCtx, &this->collider);
    } else {
        DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        if ((this->type == 1) && (gSaveContext.timer1Value > 0)) {
            gSaveContext.timer1State = 0xA;
        }
    }
}

void BgPoEvent_BlockWait(BgPoEvent* this, GlobalContext* globalCtx) {
    this->dyna.actor.world.pos.y = 833.0f;
    if (sPuzzleState == 0x3F) {
        if (this->type == 1) {
            OnePointCutscene_Init(globalCtx, 3150, 65, NULL, MAIN_CAM);
        }
        this->timer = 45;
        this->actionFunc = BgPoEvent_BlockShake;
    } else if (this->dyna.actor.xzDistToPlayer > 50.0f) {
        if (this->type != 1) {
            sPuzzleState |= (1 << this->index);
        } else {
            sPuzzleState |= 0x10;
        }
    } else if (this->type != 1) {
        sPuzzleState &= ~(1 << this->index);
    } else {
        sPuzzleState &= ~0x10;
    }
}

void BgPoEvent_BlockShake(BgPoEvent* this, GlobalContext* globalCtx) {
    DECR(this->timer);
    if (this->timer < 15) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + 2.0f * ((this->timer % 3) - 1);
        if (!(this->timer % 4)) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
        }
    }
    if (this->timer == 0) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        sPuzzleState = 0;
        this->timer = 60;
        this->actionFunc = BgPoEvent_BlockFall;
    }
}

void BgPoEvent_CheckBlock(BgPoEvent* this) {
    s32 phi_v1;
    s32 phi_a1;
    s32 phi_t0;
    s32 phi_a3;

    if ((this->index == 3) || (this->index == 1)) {
        phi_v1 = this->dyna.actor.world.pos.z;
        phi_a1 = this->dyna.actor.child->world.pos.z;
        if (this->index == 3) {
            phi_a3 = this->dyna.actor.world.pos.x;
            phi_t0 = this->dyna.actor.child->world.pos.x;
        } else { // this->index == 1
            phi_a3 = this->dyna.actor.child->world.pos.x;
            phi_t0 = this->dyna.actor.world.pos.x;
        }
    } else {
        phi_v1 = this->dyna.actor.world.pos.x;
        phi_a1 = this->dyna.actor.child->world.pos.x;
        if (this->index == 0) {
            phi_a3 = this->dyna.actor.world.pos.z;
            phi_t0 = this->dyna.actor.child->world.pos.z;
        } else { // this->index == 2
            phi_a3 = this->dyna.actor.child->world.pos.z;
            phi_t0 = this->dyna.actor.world.pos.z;
        }
    }
    if ((phi_v1 == phi_a1) && ((phi_t0 - phi_a3) == 60)) {
        sPuzzleState |= (1 << this->index);
    } else {
        sPuzzleState &= ~(1 << this->index);
    }
}

void BgPoEvent_BlockFall(BgPoEvent* this, GlobalContext* globalCtx) {
    static s32 firstFall = 0;

    this->dyna.actor.velocity.y++;
    if (Math_StepToF(&this->dyna.actor.world.pos.y, 433.0f, this->dyna.actor.velocity.y)) {
        this->dyna.actor.flags &= ~ACTOR_FLAG_5;
        this->dyna.actor.velocity.y = 0.0f;
        sBlocksAtRest++;
        if (this->type != 1) {
            BgPoEvent_CheckBlock(this);
        } else {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
            func_80033E88(&this->dyna.actor, globalCtx, 5, 5);
            func_80088B34(this->timer);
            if (firstFall == 0) {
                firstFall = 1;
            } else {
                func_8002DF54(globalCtx, &GET_PLAYER(globalCtx)->actor, 7);
            }
        }
        this->direction = 0;
        this->actionFunc = BgPoEvent_BlockIdle;
    }
}

void BgPoEvent_BlockIdle(BgPoEvent* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    Actor* amy;

    if (sPuzzleState == 0xF) {
        this->actionFunc = BgPoEvent_BlockSolved;
        if ((this->type == 0) && (this->index == 0)) {
            amy =
                Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, this->dyna.actor.world.pos.x + 30.0f,
                            this->dyna.actor.world.pos.y - 30.0f, this->dyna.actor.world.pos.z + 30.0f, 0,
                            this->dyna.actor.shape.rot.y, 0, this->dyna.actor.params + 0x300);
            if (amy != NULL) {
                OnePointCutscene_Init(globalCtx, 3170, 30, amy, MAIN_CAM);
            }
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            gSaveContext.timer1State = 0xA;
        }
    } else {
        if ((gSaveContext.timer1Value == 0) && (sBlocksAtRest == 5)) {
            player->stateFlags2 &= ~0x10;
            sPuzzleState = 0x10;
            sBlocksAtRest = 0;
        }
        if ((sPuzzleState == 0x40) || ((sPuzzleState == 0x10) && !Player_InCsMode(globalCtx))) {
            this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z;
            this->actionFunc = BgPoEvent_BlockReset;
            if (sPuzzleState == 0x10) {
                sPuzzleState = 0x40;
                Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_RISING);
                func_8002DF54(globalCtx, &player->actor, 8);
            }
        } else if (this->dyna.unk_150 != 0.0f) {
            if (this->direction == 0) {
                if (func_800435D8(globalCtx, &this->dyna, 0x1E, 0x32, -0x14) != 0) {
                    sBlocksAtRest--;
                    this->direction = (this->dyna.unk_150 >= 0.0f) ? 1.0f : -1.0f;
                    this->actionFunc = BgPoEvent_BlockPush;
                } else {
                    player->stateFlags2 &= ~0x10;
                    this->dyna.unk_150 = 0.0f;
                }
            } else {
                player->stateFlags2 &= ~0x10;
                this->dyna.unk_150 = 0.0f;
                DECR(this->direction);
            }
        } else {
            this->direction = 0;
        }
    }
}

static f32 blockPushDist = 0.0f;
void BgPoEvent_BlockPush(BgPoEvent* this, GlobalContext* globalCtx) {
    f32 displacement;
    s32 blockStop;
    Player* player = GET_PLAYER(globalCtx);

    this->dyna.actor.speedXZ += CVar_GetS32("gFasterBlockPush", 0) != 0 ? 0.5f : 0.1f;
    this->dyna.actor.speedXZ = CLAMP_MAX(this->dyna.actor.speedXZ, 2.0f);
    blockStop = Math_StepToF(&blockPushDist, 20.0f, this->dyna.actor.speedXZ);
    displacement = this->direction * blockPushDist;
    this->dyna.actor.world.pos.x = (Math_SinS(this->dyna.unk_158) * displacement) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = (Math_CosS(this->dyna.unk_158) * displacement) + this->dyna.actor.home.pos.z;
    if (blockStop) {
        player->stateFlags2 &= ~0x10;
        if ((this->dyna.unk_150 > 0.0f) && (func_800435D8(globalCtx, &this->dyna, 0x1E, 0x32, -0x14) == 0)) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }
        this->dyna.unk_150 = 0.0f;
        this->dyna.actor.home.pos.x = this->dyna.actor.world.pos.x;
        this->dyna.actor.home.pos.z = this->dyna.actor.world.pos.z;
        blockPushDist = 0.0f;
        this->dyna.actor.speedXZ = 0.0f;
        this->direction = CVar_GetS32("gFasterBlockPush", 0) != 0 ? 3 : 5;
        sBlocksAtRest++;
        this->actionFunc = BgPoEvent_BlockIdle;
        if (this->type == 1) {
            return;
        }
        BgPoEvent_CheckBlock(this);
        BgPoEvent_CheckBlock((BgPoEvent*)this->dyna.actor.parent);
    }
    func_8002F974(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

void BgPoEvent_BlockReset(BgPoEvent* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
        this->dyna.unk_150 = 0.0f;
    }
    if (Math_StepToF(&this->dyna.actor.world.pos.y, 493.0f, 1.0f) &&
        Math_ScaledStepToS(&this->dyna.actor.shape.rot.z, this->dyna.actor.world.rot.z - 0x4000, 0x400)) {

        this->index = (this->index + 1) % 4;
        this->actionFunc = BgPoEvent_BlockFall;
        sPuzzleState = 0;
        if (this->type == 1) {
            this->timer += 10;
            this->timer = CLAMP_MAX(this->timer, 120);
        }
    }
}

void BgPoEvent_BlockSolved(BgPoEvent* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~0x10;
    }
    if (Math_StepToF(&this->dyna.actor.world.pos.y, 369.0f, 2.0f)) {
        sPuzzleState = 0x20;
        Actor_Kill(&this->dyna.actor);
    }
}

void BgPoEvent_AmyWait(BgPoEvent* this, GlobalContext* globalCtx) {
    if (this->collider.base.acFlags & AC_HIT) {
        sPuzzleState |= 0x20;
        this->timer = 5;
        Actor_SetColorFilter(&this->dyna.actor, 0x4000, 0xFF, 0, 5);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EN_PO_LAUGH2);
        this->actionFunc = BgPoEvent_AmyPuzzle;
    }
}

void BgPoEvent_AmyPuzzle(BgPoEvent* this, GlobalContext* globalCtx) {
    Vec3f pos;

    if (sPuzzleState == 0xF) {
        pos.x = this->dyna.actor.world.pos.x - 5.0f;
        pos.y = Rand_CenteredFloat(120.0f) + this->dyna.actor.world.pos.y;
        pos.z = Rand_CenteredFloat(120.0f) + this->dyna.actor.world.pos.z;
        EffectSsDeadDb_Spawn(globalCtx, &pos, &sZeroVec, &sZeroVec, 170, 0, 200, 255, 100, 170, 0, 255, 0, 1, 9, true);
    } else if (sPuzzleState == 0x20) {
        Actor_Kill(&this->dyna.actor);
    } else {
        DECR(this->timer);
    }
}

s32 BgPoEvent_NextPainting(BgPoEvent* this) {
    if ((this->dyna.actor.parent != NULL) && (this->dyna.actor.child != NULL)) {
        if (Rand_ZeroOne() < 0.5f) {
            sPuzzleState = ((BgPoEvent*)this->dyna.actor.parent)->index;
        } else {
            sPuzzleState = ((BgPoEvent*)this->dyna.actor.child)->index;
        }
    } else if (this->dyna.actor.parent != NULL) {
        sPuzzleState = ((BgPoEvent*)this->dyna.actor.parent)->index;
    } else if (this->dyna.actor.child != NULL) {
        sPuzzleState = ((BgPoEvent*)this->dyna.actor.child)->index;
    } else {
        return false;
    }
    return true;
}

void BgPoEvent_PaintingEmpty(BgPoEvent* this, GlobalContext* globalCtx) {
    if (sPuzzleState == this->index) {
        this->timer = 255;
        this->actionFunc = BgPoEvent_PaintingAppear;
    }
}

void BgPoEvent_PaintingAppear(BgPoEvent* this, GlobalContext* globalCtx) {
    this->timer -= 20;
    if (this->timer <= 0) {
        this->timer = 1000;
        this->actionFunc = BgPoEvent_PaintingPresent;
    }
}

void BgPoEvent_PaintingVanish(BgPoEvent* this, GlobalContext* globalCtx) {
    this->timer += 20;
    if (this->timer >= 255) {
        BgPoEvent_NextPainting(this);
        this->actionFunc = BgPoEvent_PaintingEmpty;
    }
}

void BgPoEvent_PaintingPresent(BgPoEvent* this, GlobalContext* globalCtx) {
    Actor* thisx = &this->dyna.actor;
    Player* player = GET_PLAYER(globalCtx);

    DECR(this->timer);

    if (((this->timer == 0) || ((thisx->xzDistToPlayer < 150.0f) && (thisx->yDistToPlayer < 50.0f)) ||
         (func_8002DD78(player) && (thisx->xzDistToPlayer < 320.0f) &&
          ((this->index != 2) ? (thisx->yDistToPlayer < 100.0f) : (thisx->yDistToPlayer < 0.0f)) &&
          Player_IsFacingActor(thisx, 0x2000, globalCtx))) &&
        ((thisx->parent != NULL) || (thisx->child != NULL))) {
        /*The third condition in the || is checking if
            1) Link is holding a ranged weapon
            2) Link is too close in the xz plane
            3) Link is too close in the y direction. The painting
               under the balcony allows him to be closer.
            4) Link is within 45 degrees of facing the painting. */
        this->timer = 0;
        Audio_PlayActorSound2(thisx, NA_SE_EN_PO_LAUGH);
        this->actionFunc = BgPoEvent_PaintingVanish;
    } else if (this->collider.base.acFlags & AC_HIT) {
        if (!BgPoEvent_NextPainting(this)) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_PO_SISTERS, thisx->world.pos.x,
                        thisx->world.pos.y - 40.0f, thisx->world.pos.z, 0, thisx->shape.rot.y, 0,
                        thisx->params + ((this->type - 1) << 8));
            OnePointCutscene_Init(globalCtx, 3160, 80, thisx, MAIN_CAM);
            func_80078884(NA_SE_SY_CORRECT_CHIME);

        } else {
            Audio_PlayActorSound2(thisx, NA_SE_EN_PO_LAUGH2);
            OnePointCutscene_Init(globalCtx, 3160, 35, thisx, MAIN_CAM);
        }
        if (thisx->parent != NULL) {
            thisx->parent->child = NULL;
            thisx->parent = NULL;
        }
        if (thisx->child != NULL) {
            thisx->child->parent = NULL;
            thisx->child = NULL;
        }
        this->timer = 20;
        this->actionFunc = BgPoEvent_PaintingBurn;
    }
}

void BgPoEvent_PaintingBurn(BgPoEvent* this, GlobalContext* globalCtx) {
    Vec3f sp54;

    this->timer--;
    sp54.x = (Math_SinS(this->dyna.actor.shape.rot.y) * 5.0f) + this->dyna.actor.world.pos.x;
    sp54.y = Rand_CenteredFloat(66.0f) + this->dyna.actor.world.pos.y;
    sp54.z = Rand_CenteredFloat(50.0f) + this->dyna.actor.world.pos.z;
    if (this->timer >= 0) {
        if (this->type == 2) {
            EffectSsDeadDb_Spawn(globalCtx, &sp54, &sZeroVec, &sZeroVec, 100, 0, 255, 255, 150, 170, 255, 0, 0, 1, 9,
                                 true);
        } else {
            EffectSsDeadDb_Spawn(globalCtx, &sp54, &sZeroVec, &sZeroVec, 100, 0, 200, 255, 255, 170, 50, 100, 255, 1, 9,
                                 true);
        }
    }
    if (this->timer == 0) {
        this->dyna.actor.draw = NULL;
    }
    if (this->timer < -60) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgPoEvent_Update(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;

    this->actionFunc(this, globalCtx);
    if ((this->actionFunc == BgPoEvent_AmyWait) || (this->actionFunc == BgPoEvent_PaintingPresent)) {
        CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    }
}

void BgPoEvent_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Gfx* displayLists[] = {
        gPoSistersAmyBlockDL,     gPoSistersAmyBethBlockDL, gPoSistersJoellePaintingDL,
        gPoSistersBethPaintingDL, gPoSistersAmyPaintingDL,
    };
    s32 pad;
    BgPoEvent* this = (BgPoEvent*)thisx;
    u8 alpha;
    Vec3f sp58;
    Vec3f sp4C;
    f32 sp48;
    s32 pad2;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1481);
    func_80093D18(globalCtx->state.gfxCtx);
    if ((this->type == 3) || (this->type == 2)) {
        if (this->actionFunc == BgPoEvent_PaintingEmpty) {
            alpha = 255;
        } else if (this->actionFunc == BgPoEvent_PaintingPresent) {
            alpha = 0;
        } else {
            alpha = this->timer;
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, alpha);
    }
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1501),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, displayLists[this->type]);
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_po_event.c", 1508);

    if ((this->type == 0) || (this->type == 1)) {
        sp48 = (833.0f - this->dyna.actor.world.pos.y) * 0.0025f;
        if (!(sp48 > 1.0f)) {
            sp58.x = this->dyna.actor.world.pos.x;
            sp58.y = this->dyna.actor.world.pos.y - 30.0f;
            sp58.z = this->dyna.actor.world.pos.z;
            sp4C.y = 1.0f;
            sp4C.x = sp4C.z = (sp48 * 0.3f) + 0.4f;
            func_80033C30(&sp58, &sp4C, (u8)(155.0f + sp48 * 100.0f), globalCtx);
        }
    }
}

void BgPoEvent_Reset(void) {
    sBlocksAtRest = 0;
    sPuzzleState = 0;
    blockPushDist = 0.0f;
}
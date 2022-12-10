#include "global.h"
#include "vt.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include <objects/object_d_hsblock/object_d_hsblock.h>

#define FLAGS ACTOR_FLAG_4

void EnAObj_Init(Actor* thisx, PlayState* play);
void EnAObj_Destroy(Actor* thisx, PlayState* play);
void EnAObj_Update(Actor* thisx, PlayState* play);
void EnAObj_Draw(Actor* thisx, PlayState* play);

void EnAObj_WaitFinishedTalking(EnAObj* this, PlayState* play);
void EnAObj_WaitTalk(EnAObj* this, PlayState* play);
void EnAObj_BlockRot(EnAObj* this, PlayState* play);
void EnAObj_BoulderFragment(EnAObj* this, PlayState* play);
void EnAObj_Block(EnAObj* this, PlayState* play);

void EnAObj_SetupWaitTalk(EnAObj* this, s16 type);
void EnAObj_SetupBlockRot(EnAObj* this, s16 type);
void EnAObj_SetupBoulderFragment(EnAObj* this, s16 type);
void EnAObj_SetupBlock(EnAObj* this, s16 type);

const ActorInit En_A_Obj_InitVars = {
    ACTOR_EN_A_OBJ,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnAObj),
    (ActorFunc)EnAObj_Init,
    (ActorFunc)EnAObj_Destroy,
    (ActorFunc)EnAObj_Update,
    (ActorFunc)EnAObj_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 25, 60, 0, { 0, 0, 0 } },
};

//extern CollisionHeader D_06000730; // gHookshotTargetCol ?

static CollisionHeader* sColHeaders[] = {
    &gLargerCubeCol,       // A_OBJ_GRASS_CLUMP, A_OBJ_TREE_STUMP
    &gLargerCubeCol,       // A_OBJ_BLOCK_LARGE, A_OBJ_BLOCK_HUGE
    &gSmallerFlatBlockCol, // unused
    &gLargerFlatBlockCol,  // A_OBJ_BLOCK_SMALL_ROT, A_OBJ_BLOCK_LARGE_ROT
    &gSmallerCubeCol,      // unused
    //&D_06000730,           // A_OBJ_UNKNOWN_6 // OTRTODO
};

static Gfx* sDLists[] = {
    gFlatBlockDL,
    gFlatBlockDL,
    gFlatBlockDL,
    gFlatRotBlockDL,
    gFlatRotBlockDL,
    gSmallCubeDL,
    gHookshotPostDL, /* gHookshotPostDL ? */ // 0x06000210, // OTRTODO!
    gGrassBladesDL,
    gTreeStumpDL,
    gSignRectangularDL,
    gSignDirectionalDL,
    gBoulderFragmentsDL,
};

void EnAObj_SetupAction(EnAObj* this, EnAObjActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnAObj_Init(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    s32 pad;
    EnAObj* this = (EnAObj*)thisx;
    f32 shadowScale = 6.0f;

    this->textId = (thisx->params >> 8) & 0xFF;
    thisx->params &= 0xFF;

    switch (thisx->params) {
        case A_OBJ_BLOCK_SMALL:
            Actor_SetScale(thisx, 0.025f);
            break;
        case A_OBJ_BLOCK_LARGE:
            Actor_SetScale(thisx, 0.05f);
            break;
        case A_OBJ_BLOCK_HUGE:
        case A_OBJ_CUBE_SMALL:
        case A_OBJ_UNKNOWN_6:
            Actor_SetScale(thisx, 0.1f);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
            Actor_SetScale(thisx, 0.005f);
            break;
        case A_OBJ_BLOCK_LARGE_ROT:
        default:
            Actor_SetScale(thisx, 0.01f);
            break;
    }

    if (thisx->params >= A_OBJ_SIGNPOST_OBLONG) {
        shadowScale = 12.0f;
    }

    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, shadowScale);

    thisx->focus.pos = thisx->world.pos;
    this->dyna.bgId = BGACTOR_NEG_ONE;
    this->dyna.unk_160 = 0;
    this->dyna.unk_15C = DPM_UNK;
    thisx->uncullZoneDownward = 1200.0f;
    thisx->uncullZoneScale = 200.0f;

    switch (thisx->params) {
        case A_OBJ_BLOCK_LARGE:
        case A_OBJ_BLOCK_HUGE:
            this->dyna.bgId = 1;
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_BG);
            EnAObj_SetupBlock(this, thisx->params);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
        case A_OBJ_BLOCK_LARGE_ROT:
            this->dyna.bgId = 3;
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_BG);
            EnAObj_SetupBlockRot(this, thisx->params);
            break;
        case A_OBJ_UNKNOWN_6:
            // clang-format off
            thisx->flags |= ACTOR_FLAG_0; this->dyna.bgId = 5; this->focusYoffset = 10.0f;
            // clang-format on
            thisx->gravity = -2.0f;
            EnAObj_SetupWaitTalk(this, thisx->params);
            break;
        case A_OBJ_GRASS_CLUMP:
        case A_OBJ_TREE_STUMP:
            this->dyna.bgId = 0;
            EnAObj_SetupWaitTalk(this, thisx->params);
            break;
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            thisx->textId = (this->textId & 0xFF) | 0x300;
            // clang-format off
            thisx->flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3; thisx->targetArrowOffset = 500.0f;
            // clang-format on
            this->focusYoffset = 45.0f;
            EnAObj_SetupWaitTalk(this, thisx->params);
            Collider_InitCylinder(play, &this->collider);
            Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
            thisx->targetMode = 0;
            break;
        case A_OBJ_BOULDER_FRAGMENT:
            thisx->gravity = -1.5f;
            EnAObj_SetupBoulderFragment(this, thisx->params);
            break;
        default:
            thisx->gravity = -2.0f;
            EnAObj_SetupWaitTalk(this, thisx->params);
            break;
    }

    if (thisx->params <= A_OBJ_BLOCK_LARGE_ROT) { // A_OBJ_BLOCK_*
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
    }

    if (this->dyna.bgId != BGACTOR_NEG_ONE) {
        CollisionHeader_GetVirtual(sColHeaders[this->dyna.bgId], &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    }
}

void EnAObj_Destroy(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);

    switch (this->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            Collider_DestroyCylinder(play, &this->collider);
            break;
    }
}

void EnAObj_WaitFinishedTalking(EnAObj* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->dyna.actor, play)) {
        EnAObj_SetupWaitTalk(this, this->dyna.actor.params);
    }
}

void EnAObj_SetupWaitTalk(EnAObj* this, s16 type) {
    EnAObj_SetupAction(this, EnAObj_WaitTalk);
}

void EnAObj_WaitTalk(EnAObj* this, PlayState* play) {
    s16 relYawTowardsPlayer;

    if (this->dyna.actor.textId != 0) {
        relYawTowardsPlayer = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;
        if (ABS(relYawTowardsPlayer) < 0x2800 ||
            (this->dyna.actor.params == A_OBJ_SIGNPOST_ARROW && ABS(relYawTowardsPlayer) > 0x5800)) {
            if (Actor_ProcessTalkRequest(&this->dyna.actor, play)) {
                EnAObj_SetupAction(this, EnAObj_WaitFinishedTalking);
            } else {
                func_8002F2F4(&this->dyna.actor, play);
            }
        }
    }
}

void EnAObj_SetupBlockRot(EnAObj* this, s16 type) {
    this->rotateState = 0;
    this->rotateWaitTimer = 10;
    this->dyna.actor.world.rot.y = 0;
    this->dyna.actor.shape.rot = this->dyna.actor.world.rot;
    EnAObj_SetupAction(this, EnAObj_BlockRot);
}

void EnAObj_BlockRot(EnAObj* this, PlayState* play) {
    if (this->rotateState == 0) {
        if (this->dyna.unk_160 != 0) {
            this->rotateState++;
            this->rotateForTimer = 20;

            if ((s16)(this->dyna.actor.yawTowardsPlayer + 0x4000) < 0) {
                this->rotSpeedX = -0x3E8;
            } else {
                this->rotSpeedX = 0x3E8;
            }

            if (this->dyna.actor.yawTowardsPlayer < 0) {
                this->rotSpeedY = -this->rotSpeedX;
            } else {
                this->rotSpeedY = this->rotSpeedX;
            }
        }
    } else {
        if (this->rotateWaitTimer != 0) {
            this->rotateWaitTimer--;
        } else {
            this->dyna.actor.shape.rot.y += this->rotSpeedY;
            this->dyna.actor.shape.rot.x += this->rotSpeedX;
            this->rotateForTimer--;
            this->dyna.actor.gravity = -1.0f;

            if (this->rotateForTimer == 0) {
                this->dyna.actor.world.pos = this->dyna.actor.home.pos;
                this->rotateState = 0;
                this->rotateWaitTimer = 10;
                this->dyna.actor.velocity.y = 0.0f;
                this->dyna.actor.gravity = 0.0f;
                this->dyna.actor.shape.rot = this->dyna.actor.world.rot;
            }
        }
    }
}

void EnAObj_SetupBoulderFragment(EnAObj* this, s16 type) {
    EnAObj_SetupAction(this, EnAObj_BoulderFragment);
}

void EnAObj_BoulderFragment(EnAObj* this, PlayState* play) {
    Math_SmoothStepToF(&this->dyna.actor.speedXZ, 1.0f, 1.0f, 0.5f, 0.0f);
    this->dyna.actor.shape.rot.x += this->dyna.actor.world.rot.x >> 1;
    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z >> 1;

    if (this->dyna.actor.speedXZ != 0.0f && this->dyna.actor.bgCheckFlags & 0x8) {
        this->dyna.actor.world.rot.y =
            this->dyna.actor.wallYaw - this->dyna.actor.world.rot.y + this->dyna.actor.wallYaw - 0x8000;
        this->dyna.actor.bgCheckFlags &= ~0x8;
    }

    if (this->dyna.actor.bgCheckFlags & 0x2) {
        if (this->dyna.actor.velocity.y < -8.0f) {
            this->dyna.actor.velocity.y *= -0.6f;
            this->dyna.actor.speedXZ *= 0.6f;
            this->dyna.actor.bgCheckFlags &= ~0x3;
        } else {
            Actor_Kill(&this->dyna.actor);
        }
    }
}

void EnAObj_SetupBlock(EnAObj* this, s16 type) {
    this->dyna.actor.uncullZoneDownward = 1200.0f;
    this->dyna.actor.uncullZoneScale = 720.0f;
    EnAObj_SetupAction(this, EnAObj_Block);
}

void EnAObj_Block(EnAObj* this, PlayState* play) {
    this->dyna.actor.speedXZ += this->dyna.unk_150;
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    this->dyna.actor.speedXZ = CLAMP(this->dyna.actor.speedXZ, -2.5f, 2.5f);

    Math_SmoothStepToF(&this->dyna.actor.speedXZ, 0.0f, 1.0f, 1.0f, 0.0f);

    if (this->dyna.actor.speedXZ != 0.0f) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }

    this->dyna.unk_154 = 0.0f;
    this->dyna.unk_150 = 0.0f;
}

void EnAObj_Update(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    this->actionFunc(this, play);
    Actor_MoveForward(&this->dyna.actor);

    if (this->dyna.actor.gravity != 0.0f) {
        if (this->dyna.actor.params != A_OBJ_BOULDER_FRAGMENT) {
            Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 5.0f, 40.0f, 0.0f, 0x1D);
        } else {
            Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 5.0f, 20.0f, 0.0f, 0x1D);
        }
    }

    this->dyna.actor.focus.pos = this->dyna.actor.world.pos;
    this->dyna.actor.focus.pos.y += this->focusYoffset;

    switch (this->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            break;
    }
}

void EnAObj_Draw(Actor* thisx, PlayState* play) {
    s32 type = thisx->params;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    if (type >= A_OBJ_MAX) {
        type = A_OBJ_BOULDER_FRAGMENT;
    }

    if (thisx->params == A_OBJ_BOULDER_FRAGMENT) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 1, 60, 60, 60, 50);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[type]);

    CLOSE_DISPS(play->state.gfxCtx);
}

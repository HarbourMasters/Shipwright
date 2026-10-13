/*
 * File: z_en_kanban.c
 * Overlay: ovl_En_Kanban
 * Description: Square signpost
 */

#include "z_en_kanban.h"
#include "objects/object_kanban/object_kanban.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnKanban_Init(Actor* thisx, PlayState* play);
void EnKanban_Destroy(Actor* thisx, PlayState* play);
void EnKanban_Update(Actor* thisx, PlayState* play);
void EnKanban_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Kanban_Profile = {
    /**/ ACTOR_EN_KANBAN,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_KANBAN,
    /**/ sizeof(EnKanban),
    /**/ EnKanban_Init,
    /**/ EnKanban_Destroy,
    /**/ EnKanban_Update,
    /**/ EnKanban_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF3CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 5, { 0, 0, 0 } },
};

static u16 sPartFlags[] = {
    PART_UPPER_SIDE_LEFT,  PART_LEFT_SIDE_UPPER, PART_LEFT_SIDE_LOWER,  PART_RIGHT_SIDE_UPPER,
    PART_RIGHT_SIDE_LOWER, PART_LOWER_SIDE_LEFT, PART_UPPER_SIDE_RIGHT, PART_LOWER_SIDE_RIGHT,
    PART_POST_UPPER,       PART_POST_LOWER,      PART_POST_STAND,
};

static Vec3f sPieceOffsets[] = {
    /* WHOLE_SIGN   */ { 0.0f, 44.0f, 0.0f },
    /* UPPER_HALF   */ { 0.0f, 50.0f, 0.0f },
    /* LOWER_HALF   */ { 0.0f, 38.0f, 0.0f },
    /* RIGHT_HALF  */ { 10.0f, 44.0f, 0.0f },
    /* LEFT_HALF  */ { -10.0f, 44.0f, 0.0f },
    /* 2ND_QUAD   */ { -10.0f, 50.0f, 0.0f },
    /* 1ST_QUAD    */ { 10.0f, 50.0f, 0.0f },
    /* 3RD_QUAD   */ { -10.0f, 38.0f, 0.0f },
    /* 4TH_QUAD    */ { 10.0f, 38.0f, 0.0f },
    /* UPPER_LEFT  */ { -7.5f, 51.0f, 0.0f },
    /* LEFT_UPPER */ { -12.5f, 48.0f, 0.0f },
    /* LEFT_LOWER */ { -12.5f, 40.0f, 0.0f },
    /* LOWER_LEFT  */ { -7.5f, 37.0f, 0.0f },
    /* UPPER_RIGHT  */ { 7.5f, 51.0f, 0.0f },
    /* RIGHT_UPPER */ { 12.5f, 48.0f, 0.0f },
    /* RIGHT_LOWER */ { 12.5f, 40.0f, 0.0f },
    /* LOWER_RIGHT  */ { 7.5f, 37.0f, 0.0f },
    /* POST_UPPER   */ { 0.0f, 50.0f, 0.0f },
    /* POST_LOWER   */ { 0.0f, 38.0f, 0.0f },
};

static Vec3f sPieceSizes[] = {
    /* WHOLE_SIGN  */ { 1500.0f, 1000.0f, 0.0f },
    /* UPPER_HALF  */ { 1500.0f, 500.0f, 0.0f },
    /* LOWER_HALF  */ { 1500.0f, 500.0f, 0.0f },
    /* RIGHT_HALF  */ { 700.0f, 1000.0f, 0.0f },
    /* LEFT_HALF   */ { 700.0f, 1000.0f, 0.0f },
    /* 2ND_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 1ST_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 3RD_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* 4TH_QUAD    */ { 700.0f, 500.0f, 0.0f },
    /* UPPER_LEFT  */ { 700.0f, 500.0f, 0.0f },
    /* LEFT_UPPER  */ { 700.0f, 500.0f, 0.0f },
    /* LEFT_LOWER  */ { 700.0f, 500.0f, 0.0f },
    /* LOWER_LEFT  */ { 700.0f, 500.0f, 0.0f },
    /* UPPER_RIGHT */ { 700.0f, 500.0f, 0.0f },
    /* RIGHT_UPPER */ { 700.0f, 500.0f, 0.0f },
    /* RIGHT_LOWER */ { 700.0f, 500.0f, 0.0f },
    /* LOWER_RIGHT */ { 700.0f, 500.0f, 0.0f },
    /* POST_UPPER  */ { 200.0f, 500.0f, 0.0f },
    /* POST_LOWER  */ { 200.0f, 500.0f, 0.0f },
};

static u8 sCutTypes[] = {
    /* 1H_OVER     */ CUT_VERT_L, /* 2H_OVER     */ CUT_VERT_L,
    /* 1H_COMBO    */ CUT_DIAG_R, /* 2H_COMBO    */ CUT_DIAG_R,
    /* 1H_LEFT     */ CUT_HORIZ,  /* 2H_LEFT     */ CUT_HORIZ,
    /* 1H_COMBO    */ CUT_HORIZ,  /* 2H_COMBO    */ CUT_HORIZ,
    /* 1H_RIGHT    */ CUT_HORIZ,  /* 2H_RIGHT    */ CUT_HORIZ,
    /* 1H_COMBO    */ CUT_HORIZ,  /* 2H_COMBO    */ CUT_HORIZ,
    /* 1H_STAB     */ CUT_POST,   /* 2H_STAB     */ CUT_POST,
    /* 1H_COMBO    */ CUT_POST,   /* 2H_COMBO    */ CUT_POST,
    /* FLIP_START  */ CUT_VERT_L, /* JUMP_START  */ CUT_VERT_L,
    /* FLIP_END    */ CUT_VERT_L, /* JUMP_END    */ CUT_VERT_L,
    /*             */ CUT_VERT_L, /*             */ CUT_VERT_L,
    /*             */ CUT_HORIZ,  /*             */ CUT_HORIZ,
    /*             */ CUT_HORIZ,  /*             */ CUT_HORIZ,
    /*             */ CUT_HORIZ,  /*             */ CUT_HORIZ,
    /* BACK_LEFT   */ CUT_HORIZ,  /* BACK_RIGHT  */ CUT_HORIZ,
    /* OVER_HAMMER */ CUT_POST,   /* SIDE_HAMMER */ CUT_POST,
    /* 1H_SPIN_ATK */ CUT_POST,   /* 2H_SPIN_ATK */ CUT_POST,
    /* 1H_BIG_SPIN */ CUT_POST,   /* 2H_BIG_SPIN */ CUT_POST,
};

static u16 sCutFlags[] = {
    /* CUT_POST   */ ALL_PARTS,       /* CUT_VERT_L */ LEFT_HALF,
    /* CUT_HORIZ  */ UPPER_HALF,      /* CUT_DIAG_L */ UPPERLEFT_HALF,
    /* CUT_DIAG_R */ UPPERRIGHT_HALF, /* CUT_VERT_R */ RIGHT_HALF,
};

void func_80954960(EnKanban* this) {
    f32 nx;
    f32 ny;
    f32 nz;

    if (this->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);

        this->floorRot.x = -Math_FAtan2F(-nz * ny, 1.0f);
        this->floorRot.z = Math_FAtan2F(-nx * ny, 1.0f);
    }
}

void EnKanban_Init(Actor* thisx, PlayState* play) {
    EnKanban* this = (EnKanban*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    if (this->actor.params != ENKANBAN_PIECE) {
        this->actor.attentionRangeType = ATTENTION_RANGE_0;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->unk_19A = Rand_ZeroFloat(1.9f);
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

        if (this->actor.params == ENKANBAN_FISHING) {
            if (LINK_IS_CHILD) {
                this->actor.textId = 0x409D;
            } else {
                this->actor.textId = 0x4090;
            }
        } else {
            this->actor.textId = this->actor.params | ENKANBAN_FISHING;
        }

        this->bounceX = 1;
        this->partFlags = 0xFFFF;
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 50.0f, UPDBGCHECKINFO_FLAG_4);
        func_80954960(this);
    }
}

void EnKanban_Destroy(Actor* thisx, PlayState* play) {
    EnKanban* this = (EnKanban*)thisx;

    if (this->actionState == ENKANBAN_SIGN) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_80954BE8(EnKanban* this, PlayState* play) {
    s16 yaw;

    if (!this->msgFlag) {
        if (this->msgTimer == 0) {
            yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
            if (ABS_ALT(yaw) < 0x2800) {
                if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                    this->msgFlag = true;
                } else {
                    Actor_OfferTalk(&this->actor, play, 68.0f);
                }
            }
        } else {
            this->msgTimer--;
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->msgFlag = false;
        this->msgTimer = 20;
    }
}

void EnKanban_Update(Actor* thisx, PlayState* play) {
    u8 bounced = false;
    EnKanban* this = (EnKanban*)thisx;
    s32 pad;
    FloorType floorType;
    f32 phi_f0;
    Player* player = GET_PLAYER(play);
    Vec3f offset;
    EnKanban* piece;
    EnKanban* signpost;

    this->frameCount++;

    switch (this->actionState) {
        case ENKANBAN_SIGN:
            if (this->invincibilityTimer != 0) {
                this->invincibilityTimer--;
            }

            if (this->zTargetTimer != 0) {
                this->zTargetTimer--;
            }

            if (DECR(this->unk_1A0) == 0) {
                this->unk_19C = NULL;
            }

            if (this->zTargetTimer == 1) {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            if (this->partFlags == 0xFFFF) {
                func_80954BE8(this, play);
            }

            if ((this->invincibilityTimer == 0) && (this->collider.base.acFlags & AC_HIT)) {
                this->collider.base.acFlags &= ~AC_HIT;
                if (this->unk_19C != this->collider.base.ac) {
                    this->unk_19C = this->collider.base.ac;
                    this->unk_1A0 = 3;
                    this->invincibilityTimer = 6;

                    piece = (EnKanban*)Actor_SpawnAsChild(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_KANBAN, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, this->actor.shape.rot.x,
                        this->actor.shape.rot.y, this->actor.shape.rot.z, ENKANBAN_PIECE);
                    if (piece != NULL) {
                        ColliderElement* acHitElem = this->collider.elem.acHitElem;
                        s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                        u8 i;

                        if (acHitElem->atDmgInfo.dmgFlags & 0x200) {
                            this->cutType = sCutTypes[player->meleeWeaponAnimation];
                        } else if (acHitElem->atDmgInfo.dmgFlags & 0x10) {
                            this->invincibilityTimer = 0;
                            this->cutType = this->unk_19A + 3;
                            this->unk_19A = 1 - this->unk_19A;
                            if (this->unk_199 == 0) {
                                this->unk_199++;
                                Item_DropCollectibleRandom(play, NULL, &this->actor.focus.pos, 0x60);
                            }
                        } else {
                            this->cutType = 0;
                        }

                        if ((ABS_ALT(yawDiff) > 0x4000) && !(acHitElem->atDmgInfo.dmgFlags & 0x10)) {
                            if (this->cutType == 4) {
                                this->cutType = 3;
                            } else if (this->cutType == 1) {
                                this->cutType = 5;
                            }
                        }

                        piece->partFlags = sCutFlags[this->cutType] & this->partFlags;
                        if (piece->partFlags == 0) {
                            Actor_Kill(&piece->actor);
                            return;
                        }

                        piece->partCount = 0;
                        for (i = 0; i < 11; i++) {
                            if (sPartFlags[i] & piece->partFlags) {
                                piece->partCount++;
                            }
                        }

                        this->partFlags &= ~sCutFlags[this->cutType];
                        if ((this->partFlags & 0x3FF) == 0) {
                            this->zTargetTimer = 10;
                        }

                        if ((piece->partFlags & PART_UPPER_SIDE_LEFT) && (piece->partFlags & PART_LOWER_SIDE_RIGHT)) {
                            piece->pieceType = PIECE_WHOLE_SIGN;
                        } else if ((piece->partFlags & PART_LEFT_SIDE_UPPER) &&
                                   (piece->partFlags & PART_RIGHT_SIDE_UPPER)) {
                            piece->pieceType = PIECE_UPPER_HALF;
                        } else if ((piece->partFlags & PART_LEFT_SIDE_LOWER) &&
                                   (piece->partFlags & PART_RIGHT_SIDE_LOWER)) {
                            piece->pieceType = PIECE_LOWER_HALF;
                        } else if ((piece->partFlags & PART_UPPER_SIDE_RIGHT) &&
                                   (piece->partFlags & PART_LOWER_SIDE_RIGHT)) {
                            piece->pieceType = PIECE_RIGHT_HALF;
                        } else if ((piece->partFlags & PART_UPPER_SIDE_LEFT) &&
                                   (piece->partFlags & PART_LOWER_SIDE_LEFT)) {
                            piece->pieceType = PIECE_LEFT_HALF;
                        } else if ((piece->partFlags & PART_UPPER_SIDE_LEFT) &&
                                   (piece->partFlags & PART_LEFT_SIDE_UPPER)) {
                            piece->pieceType = PIECE_2ND_QUAD;
                        } else if ((piece->partFlags & PART_UPPER_SIDE_RIGHT) &&
                                   (piece->partFlags & PART_RIGHT_SIDE_UPPER)) {
                            piece->pieceType = PIECE_1ST_QUAD;
                        } else if ((piece->partFlags & PART_LEFT_SIDE_LOWER) &&
                                   (piece->partFlags & PART_LOWER_SIDE_LEFT)) {
                            piece->pieceType = PIECE_3RD_QUAD;
                        } else if ((piece->partFlags & PART_RIGHT_SIDE_LOWER) &&
                                   (piece->partFlags & PART_LOWER_SIDE_RIGHT)) {
                            piece->pieceType = PIECE_4TH_QUAD;
                        } else if (piece->partFlags & PART_UPPER_SIDE_LEFT) {
                            piece->pieceType = PIECE_UPPER_SIDE_LEFT;
                        } else if (piece->partFlags & PART_LEFT_SIDE_UPPER) {
                            piece->pieceType = PIECE_LEFT_SIDE_UPPER;
                        } else if (piece->partFlags & PART_LEFT_SIDE_LOWER) {
                            piece->pieceType = PIECE_LEFT_SIDE_LOWER;
                        } else if (piece->partFlags & PART_LOWER_SIDE_LEFT) {
                            piece->pieceType = PIECE_LOWER_SIDE_LEFT;
                        } else if (piece->partFlags & PART_UPPER_SIDE_RIGHT) {
                            piece->pieceType = PIECE_UPPER_SIDE_RIGHT;
                        } else if (piece->partFlags & PART_RIGHT_SIDE_UPPER) {
                            piece->pieceType = PIECE_RIGHT_SIDE_UPPER;
                        } else if (piece->partFlags & PART_RIGHT_SIDE_LOWER) {
                            piece->pieceType = PIECE_RIGHT_SIDE_LOWER;
                        } else if (piece->partFlags & PART_LOWER_SIDE_RIGHT) {
                            piece->pieceType = PIECE_LOWER_SIDE_RIGHT;
                        } else if (piece->partFlags & PART_POST_UPPER) {
                            piece->pieceType = PIECE_POST_UPPER;
                        } else if (piece->partFlags & PART_POST_LOWER) {
                            piece->pieceType = PIECE_POST_LOWER;
                        } else {
                            piece->pieceType = PIECE_OTHER;
                        }

                        if (piece->pieceType == PIECE_OTHER) {
                            piece->pieceType = PIECE_WHOLE_SIGN;
                        }

                        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
                        Matrix_MultVec3f(&sPieceOffsets[piece->pieceType], &offset);
                        piece->actor.world.pos.x += offset.x;
                        piece->actor.world.pos.y += offset.y;
                        piece->actor.world.pos.z += offset.z;

                        piece->offset.x = -sPieceOffsets[piece->pieceType].x / this->actor.scale.x;
                        piece->offset.y = -sPieceOffsets[piece->pieceType].y / this->actor.scale.x;
                        piece->offset.z = -sPieceOffsets[piece->pieceType].z / this->actor.scale.x;

                        piece->pieceWidth = sPieceSizes[piece->pieceType].x;
                        piece->pieceHeight = sPieceSizes[piece->pieceType].y;
                        piece->actionState = ENKANBAN_AIR;
                        piece->actor.gravity = -1.0f;
                        piece->actor.world.rot.y =
                            BINANG_ROT180((s32)Rand_CenteredFloat(0x4000) + this->actor.yawTowardsPlayer);

                        if ((acHitElem->atDmgInfo.dmgFlags & 0x10) || (acHitElem->atDmgInfo.dmgFlags & 8) ||
                            (acHitElem->atDmgInfo.dmgFlags & 0x80000000)) {
                            piece->actor.velocity.y = Rand_ZeroFloat(3.0f) + 6.0f;
                            piece->actor.speed = Rand_ZeroFloat(4.0f) + 6.0f;
                        } else {
                            piece->actor.velocity.y = Rand_ZeroFloat(2.0f) + 3.0f;
                            piece->actor.speed = Rand_ZeroFloat(2.0f) + 3.0f;
                        }

                        if (piece->partCount >= 4) {
                            piece->bounceX = Rand_ZeroFloat(10.0f) + 6.0f;
                            piece->bounceZ = Rand_ZeroFloat(10.0f) + 6.0f;
                        } else {
                            piece->bounceX = Rand_ZeroFloat(7.0f) + 3.0f;
                            piece->bounceZ = Rand_ZeroFloat(7.0f) + 3.0f;
                        }

                        piece->spinVel.y = Rand_CenteredFloat(0x1800);

                        if (Rand_ZeroOne() < 0.5f) {
                            piece->direction = 1;
                        } else {
                            piece->direction = -1;
                        }
                        piece->airTimer = 100;
                        piece->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                        piece->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
                        this->cutMarkTimer = 5;
                        Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_STRIKE);
                    }
                }
            }

            this->actor.focus.pos = this->actor.world.pos;
            this->actor.focus.pos.y += 44.0f;

            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

            if (this->actor.xzDistToPlayer > 500.0f) {
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->partFlags = 0xFFFF;
            }

            if (this->cutMarkTimer != 0) {
                if (this->cutMarkTimer >= 5) {
                    this->cutMarkAlpha += 255;
                    if (this->cutMarkAlpha > 255) {
                        this->cutMarkAlpha = 255;
                    }
                } else {
                    this->cutMarkAlpha -= 65;
                    if (this->cutMarkAlpha < 0) {
                        this->cutMarkAlpha = 0;
                    }
                }
                this->cutMarkTimer--;
            }
            break;

        case ENKANBAN_AIR:
        case ENKANBAN_UNUSED: {
            f32 tempX;
            f32 tempY;
            f32 tempZ;
            f32 tempWaterDepth;
            s32 pad2;
            u16 bgCheckFlags;

            if (this->unk_198 != 0) {
                this->actor.velocity.y = -2.0f;
                Actor_UpdatePos(&this->actor);
            } else {
                Actor_MoveWithGravity(&this->actor);
            }

            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 10.0f, 50.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

            tempX = this->actor.world.pos.x;
            tempY = this->actor.world.pos.y;
            tempZ = this->actor.world.pos.z;
            bgCheckFlags = this->actor.bgCheckFlags;
            tempWaterDepth = this->actor.depthInWater;
            this->actor.world.pos.z += ((this->actor.world.pos.y - this->actor.floorHeight) * -50.0f) / 100.0f;

            Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 50.0f, UPDBGCHECKINFO_FLAG_4);
            func_80954960(this);

            this->actor.world.pos.x = tempX;
            this->actor.world.pos.y = tempY;
            this->actor.world.pos.z = tempZ;
            this->actor.bgCheckFlags = bgCheckFlags;
            this->actor.depthInWater = tempWaterDepth;

            if (1) {
                u8 onGround = (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND);

                if (this->spinXFlag != 0) {
                    this->spinRot.x += this->spinVel.x;
                    this->spinVel.x -= 0x800;
                    if ((this->spinRot.x <= 0) && onGround) {
                        this->spinRot.x = 0;
                        this->spinVel.x = 0;
                    }
                } else {
                    this->spinRot.x -= this->spinVel.x;
                    this->spinVel.x -= 0x800;
                    if ((this->spinRot.x >= 0) && onGround) {
                        this->spinRot.x = 0;
                        this->spinVel.x = 0;
                    }
                }

                if (this->spinVel.x < -0xC00) {
                    this->spinVel.x = -0xC00;
                }

                if (this->spinZFlag != 0) {
                    this->spinRot.z += this->spinVel.z;
                    this->spinVel.z -= 0x800;
                    if ((this->spinRot.z <= 0) && onGround) {
                        this->spinRot.z = 0;
                        this->spinVel.z = 0;
                    }
                } else {
                    this->spinRot.z -= this->spinVel.z;
                    this->spinVel.z -= 0x800;
                    if ((this->spinRot.z >= 0) && onGround) {
                        this->spinRot.z = 0;
                        this->spinVel.z = 0;
                    }
                }

                if (this->spinVel.z < -0xC00) {
                    this->spinVel.z = -0xC00;
                }

                if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                        Actor_PlaySfx(&this->actor, NA_SE_EV_WOODPLATE_BOUND);
                    }
                    this->actor.speed *= -0.5f;
                }

                if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
                    this->actionState = ENKANBAN_WATER;
                    Actor_PlaySfx(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
                    this->bounceX = this->bounceZ = 0;
                    this->actor.world.pos.y += this->actor.depthInWater;
                    EffectSsGSplash_Spawn(play, &this->actor.world.pos, NULL, NULL, 0, (this->partCount * 20) + 300);
                    EffectSsGRipple_Spawn(play, &this->actor.world.pos, 150, 650, 0);
                    EffectSsGRipple_Spawn(play, &this->actor.world.pos, 300, 800, 5);
                    this->actor.velocity.y = 0.0f;
                    this->actor.gravity = 0.0f;
                    break;
                }

                if (onGround) {
                    floorType = SurfaceType_GetFloorType(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

                    if ((floorType == FLOOR_TYPE_15) || (floorType == FLOOR_TYPE_14)) {
                        this->unk_197 = 1;
                    } else if (floorType == FLOOR_TYPE_5) {
                        this->unk_197 = -1;
                    }

                    if (this->bounceCount <= 0) {
                        this->bounceCount++;
                        if (this->unk_197 != 0) {
                            this->actor.velocity.y = 0.0f;
                        } else {
                            this->actor.velocity.y *= -0.3f;
                            this->actor.world.rot.y += TRUNCF_BINANG(Rand_CenteredFloat(0x4000));
                        }
                        bounced = true;
                    } else {
                        this->actor.velocity.y = 0.0f;
                    }

                    if (this->unk_197 != 0) {
                        Vec3f spC8;

                        if (this->unk_197 > 0) {
                            this->actor.speed = 0.0f;
                        } else if ((this->floorRot.x > 0.1f) || (this->floorRot.z > 0.1f)) {
                            this->airTimer = 10;
                            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                                this->actionState = ENKANBAN_GROUND;
                                this->actor.speed = 0.0f;
                                goto nextCase;
                            }

                            Matrix_RotateXFNew(this->floorRot.x);
                            Matrix_RotateZF(this->floorRot.z, MTXMODE_APPLY);
                            Matrix_MultVecY(KREG(20) + 10.0f, &spC8);
                            Math_ApproachF(&this->actor.velocity.x, spC8.x, 0.5f, (KREG(21) * 0.01f) + 0.1f);
                            Math_ApproachF(&this->actor.velocity.z, spC8.z, 0.5f, (KREG(21) * 0.01f) + 0.3f);
                            this->actor.world.rot.y = Math_Atan2S(spC8.x, spC8.z);
                            this->unk_198 = 1;
                            this->actor.speed = sqrtf(SQXZ(this->actor.velocity));
                        } else {
                            this->unk_198 = 0;
                            Math_ApproachZeroF(&this->actor.speed, 1, 0.1f);
                        }
                    } else {
                        this->actor.speed *= 0.7f;
                    }

                    if (this->spinRot.x == 0) {
                        if (this->bounceX != 0) {
                            bounced = true;
                            if (this->unk_197 != 0) {
                                this->spinRot.x = 0;
                                this->bounceX = 0;
                            } else {
                                this->spinVel.x = this->bounceX << 9;
                                if (this->bounceX != 0) {
                                    this->bounceX -= 5;
                                    if (this->bounceX <= 0) {
                                        this->bounceX = 0;
                                    }
                                }

                                if (Rand_ZeroOne() < 0.5f) {
                                    this->spinXFlag = 1;
                                } else {
                                    this->spinXFlag = 0;
                                }
                            }
                        }
                    }

                    if (this->spinRot.z == 0) {
                        if (this->bounceZ != 0) {
                            bounced = 1;
                            if (this->unk_197 != 0) {
                                this->spinRot.z = 0;
                                this->bounceZ = 0;
                            } else {
                                this->spinVel.z = this->bounceZ << 9;
                                if (this->bounceZ != 0) {
                                    this->bounceZ -= 5;
                                    if (this->bounceZ <= 0) {
                                        this->bounceZ = 0;
                                    }
                                }

                                if (Rand_ZeroOne() < 0.5f) {
                                    this->spinZFlag = 1;
                                } else {
                                    this->spinZFlag = 0;
                                }
                            }
                        }
                    }

                    Math_ApproachS(&this->actor.shape.rot.x, this->direction * 0x4000, 1, 0x2000);
                } else {
                    this->actor.shape.rot.y += this->spinVel.y;
                    this->actor.shape.rot.x += this->direction * 0x7D0;
                }
            }

            if (bounced) {
                if (this->unk_197 > 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_SNOW);
                } else {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_WOODPLATE_BOUND);
                }
            }

            if (bounced && (this->unk_197 >= 0)) {
                static Color_RGBA8 D_80957548 = { 185, 140, 70, 255 };
                static Color_RGBA8 D_8095754C = { 255, 255, 255, 255 };
                s16 dustCount;
                s16 j;
                Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                Vec3f accel;
                Vec3f pos;
                Color_RGBA8 primColor;
                Color_RGBA8 envColor;
                s32 pad3;

                if (this->unk_197 != 0) {
                    primColor = D_8095754C;
                    envColor = D_8095754C;
                } else {
                    primColor = D_80957548;
                    envColor = D_80957548;
                }

                accel.x = 0.0f;
                accel.y = 0.1f;
                accel.z = 0.0f;

                pos.y = this->actor.floorHeight + 3.0f;
                dustCount = this->partCount * 0.5f;

                for (j = 0; j < dustCount + 3; j++) {
                    pos.x = Rand_CenteredFloat((this->partCount * 0.5f) + 20.0f) + this->actor.world.pos.x;
                    pos.z = Rand_CenteredFloat((this->partCount * 0.5f) + 20.0f) + this->actor.world.pos.z;
                    func_800B0F18(play, &pos, &velocity, &accel, &primColor, &envColor, 100, 5,
                                  Rand_ZeroFloat(5.0f) + 14.0f);
                }
            }

            if (DECR(this->airTimer) == 0) {
                this->actionState = ENKANBAN_GROUND;
            }
        }
        nextCase:
        // fallthrough
        case ENKANBAN_GROUND:
        case ENKANBAN_WATER:
            signpost = (EnKanban*)this->actor.parent;

            if (signpost->partFlags == 0xFFFF) {
                Actor_Kill(&this->actor);
            }

            phi_f0 = 0.0f;
            if (this->unk_197 > 0) {
                phi_f0 = -150.0f;
            }

            Math_ApproachF(&this->actor.shape.yOffset, 100.0f + phi_f0, 1.0f, 5.0f);

            if (this->actionState == ENKANBAN_WATER) {
                s16 rippleDelay;
                s32 rippleScale;

                if ((player->actor.speed > 0.0f) && (player->actor.world.pos.y < this->actor.world.pos.y) &&
                    (this->actor.xyzDistToPlayerSq < SQ(50.0f))) {
                    Math_ApproachF(&this->actor.speed, player->actor.speed, 1.0f, 0.2f);
                    if (this->actor.speed > 1.0f) {
                        this->actor.speed = 1.0f;
                    }

                    if (Math_SmoothStepToS(&this->actor.world.rot.y, BINANG_ROT180(this->actor.yawTowardsPlayer), 1,
                                           0x1000, 0) > 0) {
                        this->spinVel.y = this->actor.speed * 1000.0f;
                    } else {
                        this->spinVel.y = this->actor.speed * -1000.0f;
                    }
                }

                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    this->actor.speed = 0.0f;
                }

                Actor_MoveWithGravity(&this->actor);

                if (this->actor.speed != 0.0f) {
                    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 50.0f,
                                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                        this->actor.speed *= -0.5f;
                        if (this->spinVel.y > 0) {
                            this->spinVel.y = -2000;
                        } else {
                            this->spinVel.y = 2000;
                        }
                    }
                    Math_ApproachZeroF(&this->actor.speed, 1.0f, 0.15f);
                }
                this->actor.shape.rot.y += this->spinVel.y;
                Math_ApproachS(&this->spinVel.y, 0, 1, 0x3A);
                Math_ApproachS(&this->actor.shape.rot.x, this->direction * 0x4000, 2, 0x1000);
                Math_ApproachS(&this->spinRot.x, Math_SinS(this->frameCount * 2500) * 500.0f, 2, 0x1000);
                Math_ApproachS(&this->spinRot.z, Math_CosS(this->frameCount * 3000) * 500.0f, 2, 0x1000);
                Math_ApproachZeroF(&this->floorRot.x, 0.5f, 0.2f);
                Math_ApproachZeroF(&this->floorRot.z, 0.5f, 0.2f);

                if (fabsf(this->actor.speed) > 1.0f) {
                    rippleDelay = 0;
                } else if (fabsf(this->actor.speed) > 0.5f) {
                    rippleDelay = 3;
                } else {
                    rippleDelay = 7;
                }

                if (!(this->frameCount & rippleDelay)) {
                    if (this->partCount < 3) {
                        rippleScale = 0;
                    } else if (this->partCount < 6) {
                        rippleScale = 100;
                    } else {
                        rippleScale = 200;
                    }
                    EffectSsGRipple_Spawn(play, &this->actor.world.pos, rippleScale, rippleScale + 500, 0);
                }
            } else if ((play->actorCtx.unk2 != 0) && (this->actor.xyzDistToPlayerSq < SQ(100.0f))) {
                f32 hammerStrength = (100.0f - sqrtf(this->actor.xyzDistToPlayerSq)) * 0.05f;

                this->actionState = ENKANBAN_AIR;
                this->actor.gravity = -1.0f;
                this->actor.world.rot.y = Rand_CenteredFloat(0x10000);
                if (this->partCount >= 4) {
                    this->bounceX = Rand_ZeroFloat(10.0f) + 6.0f;
                    this->bounceZ = Rand_ZeroFloat(10.0f) + 6.0f;
                    this->actor.velocity.y = 2.0f + hammerStrength;
                    this->actor.speed = Rand_ZeroFloat(1.0f);
                } else {
                    this->bounceX = Rand_ZeroFloat(7.0f) + 3.0f;
                    this->bounceZ = Rand_ZeroFloat(7.0f) + 3.0f;
                    this->actor.velocity.y = 3.0f + hammerStrength;
                    this->actor.speed = Rand_ZeroFloat(1.5f);
                }

                this->spinVel.y = Rand_CenteredFloat(0x1800);

                if (Rand_ZeroOne() < 0.5f) {
                    this->direction = 1;
                } else {
                    this->direction = -1;
                }
                this->airTimer = 70;
            }

            if (this->bounceX == 0) {
                Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
                f32 dx;
                f32 dy;
                f32 dz;

                while (explosive != NULL) {
                    if (explosive->params != 1) {
                        explosive = explosive->next;
                        continue;
                    }

                    dx = this->actor.world.pos.x - explosive->world.pos.x;
                    dy = this->actor.world.pos.y - explosive->world.pos.y;
                    dz = this->actor.world.pos.z - explosive->world.pos.z;

                    if (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f) {
                        f32 bombStrength = (100.0f - sqrtf(SQ(dx) + SQ(dy) + SQ(dz))) * 0.05f;

                        this->actionState = ENKANBAN_AIR;
                        this->actor.gravity = -1.0f;
                        this->actor.world.rot.y = Math_Atan2S(dx, dz);

                        if (this->partCount >= 4) {
                            this->bounceX = Rand_ZeroFloat(10.0f) + 6.0f;
                            this->bounceZ = Rand_ZeroFloat(10.0f) + 6.0f;
                            this->actor.velocity.y = 2.5f + bombStrength;
                            this->actor.speed = 3.0f + bombStrength;
                        } else {
                            this->bounceX = Rand_ZeroFloat(7.0f) + 3.0f;
                            this->bounceZ = Rand_ZeroFloat(7.0f) + 3.0f;
                            this->actor.velocity.y = 5.0f + bombStrength;
                            this->actor.speed = 4.0f + bombStrength;
                        }

                        this->spinVel.y = Rand_CenteredFloat(0x1800);

                        if (Rand_ZeroOne() < 0.5f) {
                            this->direction = 1;
                        } else {
                            this->direction = -1;
                        }
                        this->airTimer = 70;
                    }

                    explosive = explosive->next;
                }
            }

            switch (this->ocarinaFlag) {
                case 0:
                    if (play->msgCtx.ocarinaMode == OCARINA_MODE_ACTIVE) {
                        this->ocarinaFlag = 1;
                    }
                    break;

                case 1:
                    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_END) &&
                        (play->msgCtx.lastPlayedSong == OCARINA_SONG_HEALING)) {
                        this->actionState = ENKANBAN_REPAIR;
                        this->bounceX = 1;
                        Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                    }
                    break;

                default:
                    break;
            }
            break;

        case ENKANBAN_REPAIR: {
            f32 distX;
            f32 distY;
            f32 distZ;
            s16 pDiff;
            s16 yDiff;
            s16 rDiff;

            signpost = (EnKanban*)this->actor.parent;
            signpost->invincibilityTimer = 5;

            if (signpost->partFlags == 0xFFFF) {
                Actor_Kill(&this->actor);
            }

            Matrix_RotateYS(signpost->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_MultVec3f(&sPieceOffsets[this->pieceType], &offset);
            distX =
                Math_SmoothStepToF(&this->actor.world.pos.x, signpost->actor.world.pos.x + offset.x, 1.0f, 3.0f, 0.0f);
            distY =
                Math_SmoothStepToF(&this->actor.world.pos.y, signpost->actor.world.pos.y + offset.y, 1.0f, 3.0f, 0.0f);
            distZ =
                Math_SmoothStepToF(&this->actor.world.pos.z, signpost->actor.world.pos.z + offset.z, 1.0f, 3.0f, 0.0f);
            pDiff = Math_SmoothStepToS(&this->actor.shape.rot.x, signpost->actor.shape.rot.x, 1, 0x200, 0);
            yDiff = Math_SmoothStepToS(&this->actor.shape.rot.y, signpost->actor.shape.rot.y, 1, 0x200, 0);
            rDiff = Math_SmoothStepToS(&this->actor.shape.rot.z, signpost->actor.shape.rot.z, 1, 0x200, 0);
            Math_ApproachS(&this->spinRot.x, 0, 1, 0x200);
            Math_ApproachS(&this->spinRot.z, 0, 1, 0x200);
            Math_ApproachZeroF(&this->floorRot.x, 1.0f, 0.05f);
            Math_ApproachZeroF(&this->floorRot.z, 1.0f, 0.05f);
            Math_ApproachZeroF(&this->actor.shape.yOffset, 1.0f, 2.0f);
            if (((distX + distY + distZ) == 0.0f) &&
                ((pDiff + yDiff + rDiff + this->spinRot.x + this->spinRot.z) == 0) && (this->floorRot.x == 0.0f) &&
                (this->floorRot.z == 0.0f)) {
                signpost->partFlags |= this->partFlags;
                signpost->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_Kill(&this->actor);
                return;
            }
            break;
        }

        default:
            break;
    }
}

static Gfx* sDisplayLists[] = {
    gSignUpperSideLeftModelDL,  gSignLeftSideUpperModelDL, gSignLeftSideLowerModelDL,  gSignRightSideUpperModelDL,
    gSignRightSideLowerModelDL, gSignLowerSideLeftModelDL, gSignUpperSideRightModelDL, gSignLowerSideRightModelDL,
    gSignPostUpperModelDL,      gSignPostLowerModelDL,     gSignPostStandModelDL,
};

#include "z_en_kanban_gfx.inc"

static f32 sCutAngles[] = {
    /* CUT_POST   */ 0.50f * M_PIf,
    /* CUT_VERT_L */ 0.00f * M_PIf,
    /* CUT_HORIZ  */ 0.50f * M_PIf,
    /* CUT_DIAG_L */ 0.66f * M_PIf,
    /* CUT_DIAG_R */ 0.34f * M_PIf,
    /* CUT_VERT_R */ 0.00f * M_PIf,
    /*            */ 0.00f * M_PIf,
    /*            */ 0.00f * M_PIf,
};

#include "overlays/ovl_En_Kanban/ovl_En_Kanban.h"

void EnKanban_Draw(Actor* thisx, PlayState* play) {
    EnKanban* this = (EnKanban*)thisx;
    f32 zShift;
    f32 zShift2;
    s32 i;
    Player* player = GET_PLAYER(play);
    u8* shadowTex = GRAPH_ALLOC(play->state.gfxCtx, ARRAY_COUNT(sShadowTexFlags));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gSignMaterialDL);

    if (this->actionState != ENKANBAN_SIGN) {
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateXFApply(this->floorRot.x);
        Matrix_RotateZF(this->floorRot.z, MTXMODE_APPLY);
        Matrix_Translate(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);

        zShift = fabsf(Math_SinS(this->spinRot.x) * this->pieceHeight);
        zShift2 = fabsf(Math_SinS(this->spinRot.z) * this->pieceWidth);
        zShift = CLAMP_MIN(zShift, zShift2);
        zShift *= -(f32)this->direction;

        Matrix_Translate(0.0f, 0.0f, zShift, MTXMODE_APPLY);
        Matrix_RotateXS(this->spinRot.x, MTXMODE_APPLY);
        Matrix_RotateYS(this->spinRot.z, MTXMODE_APPLY);
        Matrix_Translate(this->offset.x, this->offset.y, this->offset.z - 100.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        for (i = 0; i < ARRAY_COUNT(sPartFlags); i++) {
            if (sPartFlags[i] & this->partFlags) {
                gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
            }
        }
    } else {
        f32 phi_f0 = 0.0f;

        if ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_DEKU)) {
            phi_f0 = -15.0f;
        }
        this->actor.world.pos.y = this->actor.home.pos.y + phi_f0;
        Matrix_Translate(0.0f, 0.0f, -100.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        if (this->partFlags == 0xFFFF) {
            gSPDisplayList(POLY_OPA_DISP++, gSignRectangularDL);
        } else {
            for (i = 0; i < ARRAY_COUNT(sPartFlags); i++) {
                if (sPartFlags[i] & this->partFlags) {
                    gSPDisplayList(POLY_OPA_DISP++, sDisplayLists[i]);
                }
            }
        }

        if (this->cutMarkAlpha != 0) {
            f32 cutOffset = (this->cutType == CUT_POST) ? -1200.0f : 0.0f;

            Matrix_Translate(0.0f, 4400.0f + cutOffset, 200.0f, MTXMODE_APPLY);
            Matrix_RotateZF(sCutAngles[this->cutType], MTXMODE_APPLY);
            Matrix_Scale(0.0f, 10.0f, 2.0f, MTXMODE_APPLY);

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 255, 255, 255, this->cutMarkAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 150, 0);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gSignParticleDL);
        }
    }

    if ((this->actor.projectedPos.z <= 400.0f) && (this->actor.projectedPos.z > 0.0f) &&
        (this->actor.floorHeight > -3000.0f) && ((this->bounceX != 0) || (this->bounceZ != 0))) {
        u16 dayTime = CURRENT_TIME;
        f32 shadowAlpha;

        if (dayTime >= CLOCK_TIME(12, 0)) {
            dayTime = (DAY_LENGTH - 1) - dayTime;
        }

        shadowAlpha = (dayTime * 0.00275f) + 10.0f;
        if (this->actor.projectedPos.z > 300.0f) {
            shadowAlpha *= (400.0f - this->actor.projectedPos.z) * 0.01f;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 0, (s8)shadowAlpha);

        if ((player->transformation == PLAYER_FORM_HUMAN) || (player->transformation == PLAYER_FORM_DEKU)) {
            zShift = 0.0f;
        } else {
            zShift = ((this->actor.world.pos.y - this->actor.floorHeight) * -50.0f) / 100.0f;
        }

        Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z + zShift,
                         MTXMODE_NEW);
        Matrix_RotateXFApply(this->floorRot.x);
        Matrix_RotateZF(this->floorRot.z, MTXMODE_APPLY);
        Matrix_Scale(this->actor.scale.x, 0.0f, this->actor.scale.z, MTXMODE_APPLY);

        if (this->actionState == ENKANBAN_SIGN) {
            Matrix_RotateXFApply(-M_PIf / 5);
        }

        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateXS(this->spinRot.x, MTXMODE_APPLY);
        Matrix_RotateYS(this->spinRot.z, MTXMODE_APPLY);
        Matrix_Translate(this->offset.x, this->offset.y, this->offset.z, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        for (i = 0; i < ARRAY_COUNT(sShadowTexFlags); i++) {
            if (sShadowTexFlags[i] & this->partFlags) {
                shadowTex[i] = 0xFF;
            } else {
                shadowTex[i] = 0;
            }
        }

        gSPInvalidateTexCache(POLY_XLU_DISP++, shadowTex);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(shadowTex));
        gSPDisplayList(POLY_XLU_DISP++, gEnKanban_D_80957DE0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

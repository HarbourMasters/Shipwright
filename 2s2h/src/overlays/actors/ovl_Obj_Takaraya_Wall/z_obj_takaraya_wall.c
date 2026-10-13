/*
 * File: z_obj_takaraya_wall.c
 * Overlay: ovl_Obj_Takaraya_Wall
 * Description: Treasure Chest Shop board manager
 */

/**
 * The board is laid out as follows:
 *
 *             Back
 *         7 6 5 4 3 2 1 0
 *       0 x x x x x x x x <-- Chest spawns in this row
 *       1 x x x x x x x x
 *       2 x x x x x x x x
 *       3 x x x x x x x x
 *       4 x x x x x x x x
 * Left  5 x x x x x x x x   Right
 *       6 x x x x x x x x
 *       7 x x x x x x x x
 *       8 x x x x x x x x
 *       9 x x x x x x x x
 *      10 x x x x x x x x
 *             Front
 *
 */

#include "z_obj_takaraya_wall.h"
#include "objects/object_takaraya_objects/object_takaraya_objects.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjTakarayaWall_Init(Actor* thisx, PlayState* play);
void ObjTakarayaWall_Destroy(Actor* thisx, PlayState* play);
void ObjTakarayaWall_Update(Actor* thisx, PlayState* play2);
void ObjTakarayaWall_Draw(Actor* thisx, PlayState* play);

void ObjTakarayaWall_Manage(ObjTakarayaWall* this, PlayState* play);

ActorProfile Obj_Takaraya_Wall_Profile = {
    /**/ ACTOR_OBJ_TAKARAYA_WALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TAKARAYA_OBJECTS,
    /**/ sizeof(ObjTakarayaWall),
    /**/ ObjTakarayaWall_Init,
    /**/ ObjTakarayaWall_Destroy,
    /**/ ObjTakarayaWall_Update,
    /**/ ObjTakarayaWall_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 120, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

#define TAKARAYA_WALL_ROWS 11
#define TAKARAYA_WALL_COLUMNS 8

typedef struct ObjTakarayaWallCell {
    /* 0x0 */ u8 row;
    /* 0x1 */ u8 column;
} ObjTakarayaWallCell; // size = 0x2

typedef enum ObjTakarayaWallCellState {
    /* 0 */ TAKARAYA_WALL_INACTIVE,
    /* 1 */ TAKARAYA_WALL_RISING,
    /* 2 */ TAKARAYA_WALL_FALLING
} ObjTakarayaWallCellState;

s32 sTakarayaWallPathStackDepth;
ObjTakarayaWallCell sTakarayaWallPathStack[TAKARAYA_WALL_ROWS * TAKARAYA_WALL_COLUMNS];
s32 sTakarayaWallSuccessfulPathStackDepth; // Set but never used
s32 sTakarayaWallPathReachedFront;
Vec3f sTakarayaWallAudioPositions[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];
u8 sTakarayaWallStates[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];

// -10.0f height means the wall does not raise (the cell is open)
f32 sTakarayaWallHeights[TAKARAYA_WALL_ROWS][TAKARAYA_WALL_COLUMNS];

// Note: returns true if the cell is valid
s32 ObjTakarayaWall_PosToCell(Vec3f* pos, s32* row, s32* column) {
    *row = ((s32)pos->x - -1680) / 120;
    *column = ((s32)pos->z - 0) / 120;

    if ((*row < 0) || (*row >= TAKARAYA_WALL_ROWS) || (*column < 0) || (*column >= TAKARAYA_WALL_COLUMNS)) {
        return false;
    }
    return true;
}

/**
 * Checks if the given row and column is valid and is open, i.e does not raise up.
 */
s32 ObjTakarayaWall_IsCellOpen(s32 row, s32 column) {
    if ((row < 0) || (row >= TAKARAYA_WALL_ROWS) || (column < 0) || (column >= TAKARAYA_WALL_COLUMNS) ||
        (sTakarayaWallHeights[row][column] == 0.0f)) {
        return false;
    }

    return true;
}

typedef enum TakarayaWallDirection {
    /* 0 */ TAKARAYA_WALL_DIRECTION_BACK,
    /* 1 */ TAKARAYA_WALL_DIRECTION_RIGHT,
    /* 2 */ TAKARAYA_WALL_DIRECTION_FRONT,
    /* 3 */ TAKARAYA_WALL_DIRECTION_LEFT
} TakarayaWallDirection;

/**
 * Checks nearby spaces if the adjacent space or any of the three adjacent spaces to that
 * space are closed (i.e the wall rises up).
 */
s32 ObjTakarayaWall_CanCarvePath(s32 row, s32 column, TakarayaWallDirection direction) {
    s32 adjacentSpaceRow;
    s32 adjacentSpaceColumn;
    s32 row1;
    s32 column1;
    s32 row2;
    s32 column2;
    s32 row3;
    s32 column3;

    if ((direction == TAKARAYA_WALL_DIRECTION_FRONT) || (direction == TAKARAYA_WALL_DIRECTION_BACK)) {
        if (direction == TAKARAYA_WALL_DIRECTION_BACK) {
            adjacentSpaceRow = row - 1;
            row3 = row - 2;
        } else { // TAKARAYA_WALL_DIRECTION_FRONT
            adjacentSpaceRow = row + 1;
            row3 = row + 2;
        }
        adjacentSpaceColumn = column;
        column3 = column;
        row1 = adjacentSpaceRow;
        column1 = column + 1;
        row2 = adjacentSpaceRow;
        column2 = column - 1;
    } else {
        if (direction == TAKARAYA_WALL_DIRECTION_RIGHT) {
            adjacentSpaceColumn = column - 1;
            column3 = column - 2;
        } else { // TAKARAYA_WALL_DIRECTION_LEFT
            adjacentSpaceColumn = column + 1;
            column3 = column + 2;
        }
        adjacentSpaceRow = row;
        row3 = row;
        row1 = row + 1;
        column1 = adjacentSpaceColumn;
        row2 = row - 1;
        column2 = adjacentSpaceColumn;
    }

    if ((adjacentSpaceRow < 0) || (adjacentSpaceRow >= TAKARAYA_WALL_ROWS) || (adjacentSpaceColumn < 0) ||
        (adjacentSpaceColumn >= TAKARAYA_WALL_COLUMNS) ||
        (sTakarayaWallHeights[adjacentSpaceRow][adjacentSpaceColumn] == -10.0f) ||
        ObjTakarayaWall_IsCellOpen(row1, column1) || ObjTakarayaWall_IsCellOpen(row2, column2) ||
        ObjTakarayaWall_IsCellOpen(row3, column3)) {
        return false;
    }

    return true;
}

void ObjTakarayaWall_CarvePath(s32 row, s32 column) {
    s32 carveDirectionFlags = 0;
    s32 carveDirectionNum = 0;
    s32 randMode;
    s32 direction;

    if (ObjTakarayaWall_CanCarvePath(row, column, TAKARAYA_WALL_DIRECTION_BACK)) {
        carveDirectionNum++;
        carveDirectionFlags |= (1 << TAKARAYA_WALL_DIRECTION_BACK);
    }

    if (ObjTakarayaWall_CanCarvePath(row, column, TAKARAYA_WALL_DIRECTION_RIGHT)) {
        carveDirectionNum++;
        carveDirectionFlags |= (1 << TAKARAYA_WALL_DIRECTION_RIGHT);
    }

    if (ObjTakarayaWall_CanCarvePath(row, column, TAKARAYA_WALL_DIRECTION_FRONT)) {
        carveDirectionNum++;
        carveDirectionFlags |= (1 << TAKARAYA_WALL_DIRECTION_FRONT);
    }

    if (ObjTakarayaWall_CanCarvePath(row, column, TAKARAYA_WALL_DIRECTION_LEFT)) {
        carveDirectionNum++;
        carveDirectionFlags |= (1 << TAKARAYA_WALL_DIRECTION_LEFT);
    }

    if (carveDirectionNum == 0) {
        sTakarayaWallPathStackDepth--;
        if (!sTakarayaWallPathReachedFront) {
            sTakarayaWallSuccessfulPathStackDepth--;
        }

        return;
    }

    // Randomly select direction to carve, each possible direction has an equal probability of getting carved out
    randMode = (s32)Rand_ZeroFloat(carveDirectionNum) % carveDirectionNum;

    if (randMode == 0) {
        direction = TAKARAYA_WALL_DIRECTION_BACK;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction++;
        }
    } else if (randMode == 1) {
        direction = TAKARAYA_WALL_DIRECTION_LEFT;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction--;
        }
    } else if (randMode == 2) {
        direction = TAKARAYA_WALL_DIRECTION_BACK;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction++;
        }
        direction++;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction++;
        }
    } else {
        direction = TAKARAYA_WALL_DIRECTION_LEFT;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction--;
        }
        direction--;
        while (!((1 << direction) & carveDirectionFlags)) {
            direction--;
        }
    }

    if (direction == TAKARAYA_WALL_DIRECTION_BACK) {
        row--;
    } else if (direction == TAKARAYA_WALL_DIRECTION_FRONT) {
        row++;
    } else if (direction == TAKARAYA_WALL_DIRECTION_RIGHT) {
        column--;
    } else { // TAKARAYA_WALL_DIRECTION_LEFT
        column++;
    }

    sTakarayaWallHeights[row][column] = -10.0f;
    sTakarayaWallPathStack[sTakarayaWallPathStackDepth].row = row;
    sTakarayaWallPathStack[sTakarayaWallPathStackDepth].column = column;
    sTakarayaWallPathStackDepth++;

    if (!sTakarayaWallPathReachedFront) {
        sTakarayaWallSuccessfulPathStackDepth++;
        if (row == (TAKARAYA_WALL_ROWS - 1)) {
            sTakarayaWallPathReachedFront = true;
        }
    }
}

void ObjTakarayaWall_Init(Actor* thisx, PlayState* play) {
    ObjTakarayaWall* this = (ObjTakarayaWall*)thisx;
    Actor* chest;
    s32 column;
    s32 i;
    s32 j;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
        for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
            sTakarayaWallHeights[i][j] = 0.0f;
        }
    }

    Collider_InitAndSetCylinder(play, &this->columnCollider, &this->actor, &sCylinderInit);
    Collider_InitAndSetCylinder(play, &this->rowCollider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    column = (s32)Rand_ZeroFloat(TAKARAYA_WALL_COLUMNS) % TAKARAYA_WALL_COLUMNS;
    chest = Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_BOX, -1635.0f, 0.0f, (column * 120) + 60, 0,
                                          0x4000, this->actor.shape.rot.z, this->actor.params, this->actor.csId,
                                          HALFDAYBIT_ALL, NULL);
    Flags_SetSwitch(play, TAKARAYA_WALL_GET_SWTICH_FLAG(&this->actor));
    this->actor.shape.rot.z = 0;

    if (chest != NULL) {
        chest->cullingVolumeDistance = 2000.0f;
    }

    sTakarayaWallHeights[0][column] = -10.0f;
    sTakarayaWallPathStack[sTakarayaWallPathStackDepth].row = 0;
    sTakarayaWallPathStack[sTakarayaWallPathStackDepth].column = column;
    sTakarayaWallPathStackDepth++;
    sTakarayaWallSuccessfulPathStackDepth++;

    do {
        ObjTakarayaWall_CarvePath(sTakarayaWallPathStack[sTakarayaWallPathStackDepth - 1].row,
                                  sTakarayaWallPathStack[sTakarayaWallPathStackDepth - 1].column);
    } while (sTakarayaWallPathStackDepth > 1);

    for (j = 1; j < TAKARAYA_WALL_COLUMNS - 1; j++) {
        if (sTakarayaWallHeights[TAKARAYA_WALL_ROWS - 1][j] == -10.0f) {
            break;
        }
    }

    // If either corner in the front is part of the success path, and there is no other opening in the front,
    // open up the space next to the open corner since the Treasure Chest Shop scene blocks direct access to the
    // front corners.
    if (j == (TAKARAYA_WALL_COLUMNS - 1)) {
        if (sTakarayaWallHeights[TAKARAYA_WALL_ROWS - 1][0] == -10.0f) {
            sTakarayaWallHeights[TAKARAYA_WALL_ROWS - 1][1] = -10.0f;
        } else {
            sTakarayaWallHeights[TAKARAYA_WALL_ROWS - 1][TAKARAYA_WALL_COLUMNS - 2] = -10.0f;
        }
    }

    this->actionFunc = ObjTakarayaWall_Manage;
}

void ObjTakarayaWall_Destroy(Actor* thisx, PlayState* play) {
    ObjTakarayaWall* this = (ObjTakarayaWall*)thisx;
    s32 i;
    s32 j;

    Collider_DestroyCylinder(play, &this->columnCollider);
    Collider_DestroyCylinder(play, &this->rowCollider);

    for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
        for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
            AudioSfx_StopByPos(&sTakarayaWallAudioPositions[i][j]);
        }
    }
}

void ObjTakarayaWall_Manage(ObjTakarayaWall* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    ColliderCylinder* collider;
    s32 playerRowBehind;
    s32 playerRow;
    s32 playerColumn;
    s32 columnColliderActive;
    s32 rowColliderActive;
    s32 playerCellXPos;
    s32 playerCellZPos;
    s32 i;
    s32 j;
    s32 playerColumnRight;
    s32 playerColumnLeft;
    s32 playerRowInFront;

    rowColliderActive = false;
    columnColliderActive = false;
    playerRowBehind = playerRowInFront = -1;
    playerColumnLeft = playerColumnRight = -1;

    ObjTakarayaWall_PosToCell(&player->actor.world.pos, &playerRow, &playerColumn);
    playerCellXPos = (((s32)player->actor.world.pos.x - -1800) % 120) - 60;
    playerCellZPos = (((s32)player->actor.world.pos.z - -120) % 120) - 60;

    if (playerCellXPos > 20) {
        playerRowBehind = playerRow + 1;
    } else if (playerCellXPos < -20) {
        playerRowInFront = playerRow - 1;
    }

    if (playerCellZPos > 20) {
        playerColumnLeft = playerColumn + 1;
    } else if (playerCellZPos < -20) {
        playerColumnRight = playerColumn - 1;
    }

    for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
        for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
            if (sTakarayaWallHeights[i][j] >= 0.0f) {
                if (((j == playerColumn) && ((i == playerRowInFront) || (i == playerRowBehind))) ||
                    ((i == playerRow) && ((j == playerColumnRight) || (j == playerColumnLeft)))) {
                    if (Math_StepToF(&sTakarayaWallHeights[i][j], 120.0f, 15.0f)) {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_INACTIVE;
                    } else {
                        sTakarayaWallStates[i][j] = TAKARAYA_WALL_RISING;
                    }
                    if (j == playerColumn) {
                        columnColliderActive = true;
                    } else if (i == playerRow) {
                        rowColliderActive = true;
                    }
                } else if ((i != playerRow) || (j != playerColumn)) {
                    Math_SmoothStepToF(&sTakarayaWallHeights[i][j], 0.0f, 0.2f, 5.0f, 1.0f);
                    sTakarayaWallStates[i][j] = TAKARAYA_WALL_FALLING;
                }
            }
        }
    }

    collider = &this->columnCollider;
    collider->dim.radius = 200;
    if (!columnColliderActive) {
        collider->dim.pos.y = (s32)this->actor.world.pos.y - 240;
    } else {
        collider->dim.pos.y = (s32)this->actor.world.pos.y;
        collider->dim.pos.z = (s32)player->actor.world.pos.z;
        if (playerCellXPos > 0) {
            collider->dim.pos.x = ((((s32)player->actor.world.pos.x - -1680) / 120) * 120) - 1380;
        } else {
            collider->dim.pos.x = ((((s32)player->actor.world.pos.x - -1680) / 120) * 120) - 1860;
        }
    }

    collider = &this->rowCollider;
    collider->dim.radius = 200;
    if (!rowColliderActive) {
        collider->dim.pos.y = (s32)this->actor.world.pos.y - 240;
    } else {
        collider->dim.pos.y = (s32)this->actor.world.pos.y;
        collider->dim.pos.x = (s32)player->actor.world.pos.x;
        if (playerCellZPos > 0) {
            collider->dim.pos.z = (((s32)player->actor.world.pos.z / 120) * 120) + 300;
        } else {
            collider->dim.pos.z = (((s32)player->actor.world.pos.z / 120) * 120) - 180;
        }
    }
}

void ObjTakarayaWall_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjTakarayaWall* this = (ObjTakarayaWall*)thisx;

    this->actionFunc(this, play);

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->columnCollider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->rowCollider.base);
}

void ObjTakarayaWall_Draw(Actor* thisx, PlayState* play) {
    Vec3f audioPos;
    ObjTakarayaWall* this = (ObjTakarayaWall*)thisx;
    MtxF* mtx;
    Gfx* gfx;
    s32 i;
    s32 j;

    OPEN_DISPS(play->state.gfxCtx);

    mtx = Matrix_GetCurrent();

    gfx = POLY_OPA_DISP;
    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);
    gDPSetPrimColor(gfx++, 0, 0, 255, 255, 255, 255);

    for (i = 0; i < TAKARAYA_WALL_ROWS; i++) {
        for (j = 0; j < TAKARAYA_WALL_COLUMNS; j++) {
            if (sTakarayaWallHeights[i][j] > 0.0f) {
                FrameInterpolation_RecordOpenChild(this, i * TAKARAYA_WALL_COLUMNS + j);
                mtx->xw = (i * 120) - 1620;
                mtx->yw = sTakarayaWallHeights[i][j] + (this->actor.world.pos.y - 120.0f);
                mtx->zw = (j * 120) + 60;

                MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);

                if (((i + j) % 2) != 0) {
                    gSPDisplayList(gfx++, gTreasureChestShopWallWhiteDL);
                } else {
                    gSPDisplayList(gfx++, gTreasureChestShopWallBlackDL);
                }

                if (sTakarayaWallStates[i][j] != TAKARAYA_WALL_INACTIVE) {
                    audioPos.x = mtx->xw;
                    audioPos.y = mtx->yw;
                    audioPos.z = mtx->zw;

                    SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &audioPos,
                                                &sTakarayaWallAudioPositions[i][j]);

                    if (sTakarayaWallStates[i][j] == TAKARAYA_WALL_RISING) {
                        Audio_PlaySfx_AtPos(&sTakarayaWallAudioPositions[i][j], NA_SE_EV_ROCK_CUBE_RISING - SFX_FLAG);
                    } else { // TAKARAYA_WALL_FALLING
                        Audio_PlaySfx_AtPos(&sTakarayaWallAudioPositions[i][j], NA_SE_EV_ROCK_CUBE_FALL - SFX_FLAG);
                    }
                }

                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}

/*
 * File: z_obj_makeoshihiki.c
 * Overlay: ovl_Obj_Makeoshihiki
 * Description: Push Block puzzle (Hardcoded)
 */

#include "z_obj_makeoshihiki.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_5

void ObjMakeoshihiki_Init(Actor* thisx, PlayState* play);
void ObjMakeoshihiki_Draw(Actor* thisx, PlayState* play);

const ActorInit Obj_Makeoshihiki_InitVars = {
    ACTOR_OBJ_MAKEOSHIHIKI,       ACTORCAT_PROP,           FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP, sizeof(ObjMakeoshihiki), (ActorFunc)ObjMakeoshihiki_Init,
    (ActorFunc)Actor_Noop,        (ActorFunc)Actor_Noop,   (ActorFunc)ObjMakeoshihiki_Draw,
};

typedef struct {
    /* 0x00 */ Vec3f posVecs[3];
    /* 0x24 */ u8 unk_24[3];
    /* 0x27 */ u8 color;
    /* 0x28 */ u8 type;
    /* 0x2A */ s16 rotY;
} BlockConfig; // size = 0x2C

static BlockConfig sBlocks[] = {
    { { { 660.0f, 460.0f, 660.0f }, { 660.0f, 457.0f, 540.0f }, { 780.0f, 454.0f, 540.0f } },
      0x00,
      0x00,
      0x03,
      0xFF,
      PUSHBLOCK_LARGE_START_ON,
      0x0000 },
    { { { -605.0f, -820.0f, -290.0f }, { -365.0f, -905.0f, -290.0f }, { -365.0f, -905.0f, -290.0f } },
      0x00,
      0x03,
      0x00,
      0xFF,
      PUSHBLOCK_SMALL_START_ON,
      0x0000 }
};

static u32 sFlags[3][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 } };

static void (*sFlagSwitchFuncs[])(PlayState* play, s32 flag) = { Flags_UnsetSwitch, Flags_SetSwitch };

void ObjMakeoshihiki_Init(Actor* thisx, PlayState* play) {
    BlockConfig* block = &sBlocks[thisx->home.rot.z & 1];
    s32 typeIdx;
    Vec3f* spawnPos;

    if (!((thisx->params >> 6) & 1) && Flags_GetSwitch(play, thisx->params & 0x3F)) {
        typeIdx = 1;
    } else if (!((thisx->params >> 0xE) & 1) && Flags_GetSwitch(play, (thisx->params >> 8) & 0x3F)) {
        typeIdx = 2;
    } else {
        typeIdx = 0;
    }

    spawnPos = &block->posVecs[typeIdx];

    if (Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_OBJ_OSHIHIKI, spawnPos->x, spawnPos->y,
                           spawnPos->z, 0, block->rotY, 0,
                           ((block->color << 6) & 0xC0) | (block->type & 0xF) | 0xFF00) == NULL) {
        // "Push-pull block failure"
        osSyncPrintf(VT_COL(RED, WHITE));
        osSyncPrintf("Ｅｒｒｏｒ : 押し引きブロック発生失敗(%s %d)\n", __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
        Actor_Kill(thisx);
        return;
    }
    if (block->unk_24[typeIdx] & 2) {
        ((ObjOshihiki*)thisx->child)->cantMove = true;
    }
    thisx->world.rot.z = thisx->shape.rot.z = 0;
    osSyncPrintf("(%s)(arg_data %04xF)(angleZ %d)\n", __FILE__, thisx->params, thisx->home.rot.z);
}

void ObjMakeoshihiki_Draw(Actor* thisx, PlayState* play) {
    BlockConfig* block = &sBlocks[thisx->home.rot.z & 1];
    s32 i;
    s32 sfxCond1;
    s32 sfxCond2;
    s32 cond;
    s32 cond2;

    for (i = 0; i < 3; i++) {
        if (Math3D_Vec3fDistSq(&thisx->child->world.pos, &block->posVecs[i]) < 0.001f) {
            if (block->unk_24[i] & 1) {
                if ((thisx->params >> 6) & 1) {
                    sfxCond1 = false;
                } else {
                    if (Flags_GetSwitch(play, thisx->params & 0x3F)) {
                        cond = true;
                    } else {
                        cond = false;
                    }
                    sfxCond1 = sFlags[i][0] ^ cond;
                }

                if ((thisx->params >> 0xE) & 1) {
                    sfxCond2 = false;
                } else {
                    if (Flags_GetSwitch(play, (thisx->params >> 8) & 0x3F)) {
                        cond2 = true;
                    } else {
                        cond2 = false;
                    }
                    sfxCond2 = sFlags[i][1] ^ cond2;
                }

                if (sfxCond1 || sfxCond2) {
                    func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }

            sFlagSwitchFuncs[sFlags[i][0]](play, thisx->params & 0x3F);
            sFlagSwitchFuncs[sFlags[i][1]](play, (thisx->params >> 8) & 0x3F);

            if (block->unk_24[i] & 2) {
                ((ObjOshihiki*)thisx->child)->cantMove = true;
            }

            break;
        }
    }
}

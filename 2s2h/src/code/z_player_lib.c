/**
 * File: z_player_lib.c
 * Description: Set of library functions to interact with the Player system
 */

#include "global.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#include "objects/gameplay_keep/gameplay_keep.h"

// Assets for each player form
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_goron/object_link_goron.h"
#include "objects/object_link_zora/object_link_zora.h"
#include "objects/object_link_nuts/object_link_nuts.h"
#include "objects/object_link_child/object_link_child.h"

// Assets for each mask
#include "objects/object_mask_truth/object_mask_truth.h"
#include "objects/object_mask_kerfay/object_mask_kerfay.h"
#include "objects/object_mask_yofukasi/object_mask_yofukasi.h"
#include "objects/object_mask_rabit/object_mask_rabit.h"
#include "objects/object_mask_ki_tan/object_mask_ki_tan.h"
#include "objects/object_mask_json/object_mask_json.h"
#include "objects/object_mask_romerny/object_mask_romerny.h"
#include "objects/object_mask_zacho/object_mask_zacho.h"
#include "objects/object_mask_posthat/object_mask_posthat.h"
#include "objects/object_mask_meoto/object_mask_meoto.h"
#include "objects/object_mask_bigelf/object_mask_bigelf.h"
#include "objects/object_mask_gibudo/object_mask_gibudo.h"
#include "objects/object_mask_gero/object_mask_gero.h"
#include "objects/object_mask_dancer/object_mask_dancer.h"
#include "objects/object_mask_skj/object_mask_skj.h"
#include "objects/object_mask_stone/object_mask_stone.h"
#include "objects/object_mask_bree/object_mask_bree.h"
#include "objects/object_mask_bakuretu/object_mask_bakuretu.h"
#include "objects/object_mask_bu_san/object_mask_bu_san.h"
#include "objects/object_mask_kyojin/object_mask_kyojin.h"
#include "objects/object_mask_boy/object_mask_boy.h"
#include "objects/object_mask_goron/object_mask_goron.h"
#include "objects/object_mask_zora/object_mask_zora.h"
#include "objects/object_mask_nuts/object_mask_nuts.h"

// Assets for other actors
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"

// Play as Kafei waist DL override
#include "objects/object_test3/object_test3.h"

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <libultraship/bridge/resourcebridge.h>

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
} struct_801F58B0; // size = 0x1C

struct_801F58B0 D_801F58B0[3][3];

#define D_801F59B0_LEN 2

Vec3f D_801F59B0[D_801F59B0_LEN];

s32 D_801F59C8[D_801F59B0_LEN];

typedef struct BunnyEarKinematics {
    /* 0x0 */ Vec3s rot;
    /* 0x6 */ Vec3s angVel;
} BunnyEarKinematics; // size = 0xC

BunnyEarKinematics sBunnyEarKinematics;

Vec3f* sPlayerCurBodyPartPos;

s32 D_801F59E0;

s32 sPlayerLod;

Vec3f sPlayerGetItemRefPos;

PlayerModelType sPlayerLeftHandType;
PlayerModelType sPlayerRightHandType;

void func_80127B64(struct_801F58B0 arg0[], s32 count, Vec3f* arg2);

s32 func_801226E0(PlayState* play, s32 arg1) {
    if (arg1 == 0) {
        Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B));
        if (play->sceneId == SCENE_KAKUSIANA) {
            return 1;
        }
    }

    gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

s32 Player_InitOverrideInput(PlayState* play, PlayerOverrideInputEntry* inputEntry, u32 numPoints,
                             Vec3s* targetPosList) {
    inputEntry->numPoints = numPoints;
    inputEntry->curPoint = 0;
    inputEntry->targetPosList = targetPosList;
    return 1;
}

s32 Player_UpdateOverrideInput(PlayState* play, PlayerOverrideInputEntry* inputEntry, f32 distXZRange) {
    if (inputEntry->curPoint < inputEntry->numPoints) {
        Player* player = GET_PLAYER(play);
        Vec3f targetPos;
        s32 pad;
        s16 yaw;
        f32 distXZ;

        Math_Vec3s_ToVec3f(&targetPos, &inputEntry->targetPosList[inputEntry->curPoint]);
        yaw = Math_Vec3f_Yaw(&player->actor.world.pos, &targetPos);

        Actor_SetControlStickData(play, &play->actorCtx.overrideInput, distXZRange, yaw);
        play->actorCtx.isOverrideInputOn = true;

        distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &targetPos);

        if ((fabsf(player->actor.world.pos.y - targetPos.y) < 50.0f) && (distXZ < distXZRange)) {
            inputEntry->curPoint++;
        }

        return false;
    }

    return true;
}

void func_80122868(PlayState* play, Player* player) {
    OPEN_DISPS(play->state.gfxCtx);

    if (player->invincibilityTimer > 0) {
        s32 phi_v0 = 50 - player->invincibilityTimer;

        phi_v0 = CLAMP(phi_v0, 8, 40);

        player->unk_B5F += phi_v0;
        POLY_OPA_DISP =
            Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 0, 0, 4000 - (s32)(Math_CosS(player->unk_B5F << 8) * 2000.0f));
    } else if (gSaveContext.jinxTimer != 0) {
        player->unk_B5F += 10;
        POLY_OPA_DISP =
            Gfx_SetFog(POLY_OPA_DISP, 0, 0, 255, 0, 0, 4000 - (s32)(Math_CosS(player->unk_B5F << 8) * 2000.0f));
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_801229A0(PlayState* play, Player* player) {
    OPEN_DISPS(play->state.gfxCtx);

    if ((gSaveContext.jinxTimer != 0) || (player->invincibilityTimer > 0)) {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// Update function
void Player_DoNothing(Actor* thisx, PlayState* play) {
}

s16 sMaskObjectIds[PLAYER_MASK_MAX - 1] = {
    OBJECT_MASK_TRUTH,    // PLAYER_MASK_TRUTH
    OBJECT_MASK_KERFAY,   // PLAYER_MASK_KAFEIS_MASK
    OBJECT_MASK_YOFUKASI, // PLAYER_MASK_ALL_NIGHT
    OBJECT_MASK_RABIT,    // PLAYER_MASK_BUNNY
    OBJECT_MASK_KI_TAN,   // PLAYER_MASK_KEATON
    OBJECT_MASK_JSON,     // PLAYER_MASK_GARO
    OBJECT_MASK_ROMERNY,  // PLAYER_MASK_ROMANI
    OBJECT_MASK_ZACHO,    // PLAYER_MASK_CIRCUS_LEADER
    OBJECT_MASK_POSTHAT,  // PLAYER_MASK_POSTMAN
    OBJECT_MASK_MEOTO,    // PLAYER_MASK_COUPLE
    OBJECT_MASK_BIGELF,   // PLAYER_MASK_GREAT_FAIRY
    OBJECT_MASK_GIBUDO,   // PLAYER_MASK_GIBDO
    OBJECT_MASK_GERO,     // PLAYER_MASK_DON_GERO
    OBJECT_MASK_DANCER,   // PLAYER_MASK_KAMARO
    OBJECT_MASK_SKJ,      // PLAYER_MASK_CAPTAIN
    OBJECT_MASK_STONE,    // PLAYER_MASK_STONE
    OBJECT_MASK_BREE,     // PLAYER_MASK_BREMEN
    OBJECT_MASK_BAKURETU, // PLAYER_MASK_BLAST
    OBJECT_MASK_BU_SAN,   // PLAYER_MASK_SCENTS
    OBJECT_MASK_KYOJIN,   // PLAYER_MASK_GIANT
    OBJECT_MASK_BOY,      // PLAYER_MASK_FIERCE_DEITY
    OBJECT_MASK_GORON,    // PLAYER_MASK_GORON
    OBJECT_MASK_ZORA,     // PLAYER_MASK_ZORA
    OBJECT_MASK_NUTS,     // PLAYER_MASK_DEKU
};

// Load mask object?
/**
 * Notes:
 *
 * player->maskObjectLoadState seems to be able to take 3 possible values
 *    0: The mask object is loaded.
 *    1: The mask object must be changed (and the DMA request has not been sent yet)
 *    2: Waiting for the DMA request to complete.
 */
void func_801229FC(Player* player) {
    if (player->maskObjectLoadState == 1) {
        s16 objectId = sMaskObjectIds[(u8)player->maskId - 1];

        // #region 2S2H [Port] Consider the masks already loaded
        // osCreateMesgQueue(&player->maskObjectLoadQueue, &player->maskObjectLoadMsg, 1);
        // DmaMgr_SendRequestImpl(&player->maskDmaRequest, player->maskObjectSegment, gObjectTable[objectId].vromStart,
        //                        gObjectTable[objectId].vromEnd - gObjectTable[objectId].vromStart, 0,
        //                        &player->maskObjectLoadQueue, OS_MESG_PTR(NULL));
        player->maskObjectLoadState++;
    } else if (player->maskObjectLoadState == 2) {
        // if (osRecvMesg(&player->maskObjectLoadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
        if (true) {
            // #endregion
            player->maskObjectLoadState = 0;

            if (player->currentMask == PLAYER_MASK_GREAT_FAIRY) {
                s32 i;

                for (i = 0; i < ARRAY_COUNT(D_801F58B0); i++) {
                    func_80127B64(D_801F58B0[i], ARRAY_COUNT(D_801F58B0[i]),
                                  &player->bodyPartsPos[PLAYER_BODYPART_HEAD]);
                }
            }
        }
    } else if ((player->currentMask != PLAYER_MASK_NONE) && (player->currentMask != (u8)player->maskId)) {
        player->maskObjectLoadState = 1;
        player->maskId = player->currentMask;
    } else if (player->currentMask == PLAYER_MASK_CIRCUS_LEADER) {
        s32 i;

        for (i = 0; i < D_801F59B0_LEN; i++) {
            D_801F59C8[i] += Rand_S16Offset(4, 23) + (s32)(fabsf(player->speedXZ) * 50.0f);
        }
    }
}

void func_80122BA4(PlayState* play, struct_80122D44_arg1* arg1, s32 arg2, s32 alpha) {
    if (arg2 == arg1->unk_00) {
        s32 index;

        arg1->unk_01--;
        if (arg1->unk_01 < 0) {
            arg1->unk_01 = 3;
        }

        index = arg1->unk_01;
        arg1->unk_04[index].unk_00 = arg1->unk_00;
        arg1->unk_04[index].alpha = alpha;
        Matrix_Get(&arg1->unk_04[index].mf);

        arg1->unk_00 = 0;
    }
}

void func_80122C20(PlayState* play, struct_80122D44_arg1* arg1) {
    struct_80122D44_arg1_unk_04* temp_v1 = &arg1->unk_04[0];
    s32 phi_a1;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(arg1->unk_04); i++, temp_v1++) {
        // Can't be `temp_v1->alpha != 0`
        if (temp_v1->alpha) {
            phi_a1 = (temp_v1->unk_00 == 3) ? (255 / 3) : (255 / 5);
            if (phi_a1 >= temp_v1->alpha) {
                temp_v1->alpha = 0;
            } else {
                temp_v1->alpha -= phi_a1;
            }
        }
    }
}

typedef struct {
    /* 0x0 */ Color_RGB8 color;
    /* 0x4 */ Gfx* dList;
} struct_801BFDD0; // size = 0x8

struct_801BFDD0 D_801BFDD0[] = {
    { { 180, 200, 255 }, gLinkGoronCurledDL },
    { { 155, 0, 0 }, gLinkGoronRollingSpikesAndEffectDL },
    { { 255, 0, 0 }, gLinkGoronGoronPunchEffectDL },
};

void func_80122D44(PlayState* play, struct_80122D44_arg1* arg1) {
    struct_801BFDD0* temp_s3;
    struct_80122D44_arg1_unk_04* phi_s2 = arg1->unk_04;
    s32 phi_s6 = false;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(arg1->unk_04); i++) {
        if ((phi_s2->alpha != 0) && (phi_s2->alpha != 255)) {
            temp_s3 = &D_801BFDD0[phi_s2->unk_00 - 1];
            Matrix_Put(&phi_s2->mf);

            gDPPipeSync(POLY_XLU_DISP++);

            if (!phi_s6 && phi_s2->unk_00 == 2) {
                AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(&object_link_goron_Matanimheader_013138));
                phi_s6 = true;
            }

            Scene_SetRenderModeXlu(play, 1, 2);
            gDPSetEnvColor(POLY_XLU_DISP++, temp_s3->color.r, temp_s3->color.g, temp_s3->color.b, phi_s2->alpha);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, temp_s3->dList);
        }

        phi_s2++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u8 sMaskItemIds[PLAYER_MASK_MAX - 1] = {
    ITEM_MASK_TRUTH,         // PLAYER_MASK_TRUTH
    ITEM_MASK_KAFEIS_MASK,   // PLAYER_MASK_KAFEIS_MASK
    ITEM_MASK_ALL_NIGHT,     // PLAYER_MASK_ALL_NIGHT
    ITEM_MASK_BUNNY,         // PLAYER_MASK_BUNNY
    ITEM_MASK_KEATON,        // PLAYER_MASK_KEATON
    ITEM_MASK_GARO,          // PLAYER_MASK_GARO
    ITEM_MASK_ROMANI,        // PLAYER_MASK_ROMANI
    ITEM_MASK_CIRCUS_LEADER, // PLAYER_MASK_CIRCUS_LEADER
    ITEM_MASK_POSTMAN,       // PLAYER_MASK_POSTMAN
    ITEM_MASK_COUPLE,        // PLAYER_MASK_COUPLE
    ITEM_MASK_GREAT_FAIRY,   // PLAYER_MASK_GREAT_FAIRY
    ITEM_MASK_GIBDO,         // PLAYER_MASK_GIBDO
    ITEM_MASK_DON_GERO,      // PLAYER_MASK_DON_GERO
    ITEM_MASK_KAMARO,        // PLAYER_MASK_KAMARO
    ITEM_MASK_CAPTAIN,       // PLAYER_MASK_CAPTAIN
    ITEM_MASK_STONE,         // PLAYER_MASK_STONE
    ITEM_MASK_BREMEN,        // PLAYER_MASK_BREMEN
    ITEM_MASK_BLAST,         // PLAYER_MASK_BLAST
    ITEM_MASK_SCENTS,        // PLAYER_MASK_SCENTS
    ITEM_MASK_GIANT,         // PLAYER_MASK_GIANT
    ITEM_MASK_FIERCE_DEITY,  // PLAYER_MASK_FIERCE_DEITY
    ITEM_MASK_GORON,         // PLAYER_MASK_GORON
    ITEM_MASK_ZORA,          // PLAYER_MASK_ZORA
    ITEM_MASK_DEKU,          // PLAYER_MASK_DEKU
};

u8 Player_MaskIdToItemId(s32 maskIdMinusOne) {
    return sMaskItemIds[maskIdMinusOne];
}

s32 Player_GetCurMaskItemId(PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->currentMask != PLAYER_MASK_NONE) {
        return Player_MaskIdToItemId(player->currentMask - 1);
    }

    return ITEM_NONE;
}

void func_80122F28(Player* player) {
    if ((player->actor.category == ACTORCAT_PLAYER) &&
        !(player->stateFlags1 & (PLAYER_STATE1_400 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_200000 |
                                 PLAYER_STATE1_800000 | PLAYER_STATE1_20000000)) &&
        !(player->stateFlags2 & PLAYER_STATE2_1)) {
        if (player->doorType <= PLAYER_DOORTYPE_TALKING) {
            CutsceneManager_Queue(CS_ID_GLOBAL_TALK);
        } else {
            CutsceneManager_Queue(CS_ID_GLOBAL_DOOR);
        }
    }
}

bool func_80122F9C(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags2 & PLAYER_STATE2_80000) && (player->av1.actionVar1 == 2);
}

bool func_80122FCC(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags2 & PLAYER_STATE2_80000) &&
           ((player->av1.actionVar1 == 1) || (player->av1.actionVar1 == 3));
}

void func_8012300C(PlayState* play, s32 arg1) {
    Player* player = GET_PLAYER(play);

    player->unk_B2B = arg1;
}

// Update function
void func_8012301C(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    this->av1.actionVar1++;

    if (this->av1.actionVar1 == 2) {
        s16 objectId = gPlayerFormObjectIds[GET_PLAYER_FORM];

        gActorOverlayTable[ACTOR_PLAYER].profile->objectId = objectId;
        func_8012F73C(&play->objectCtx, this->actor.objectSlot, objectId);
        this->actor.objectSlot = Object_GetSlot(&play->objectCtx, GAMEPLAY_KEEP);
    } else if (this->av1.actionVar1 >= 3) {
        s32 objectSlot = Object_GetSlot(&play->objectCtx, gActorOverlayTable[ACTOR_PLAYER].profile->objectId);

        if (Object_IsLoaded(&play->objectCtx, objectSlot)) {
            this->actor.objectSlot = objectSlot;
            this->actor.shape.rot.z = GET_PLAYER_FORM + 1;
            this->actor.init = PlayerCall_Init;
            this->actor.update = PlayerCall_Update;
            this->actor.draw = PlayerCall_Draw;
            gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
        }
    }
}

FlexSkeletonHeader* gPlayerSkeletons[PLAYER_FORM_MAX] = {
    &gLinkFierceDeitySkel, &gLinkGoronSkel, &gLinkZoraSkel, &gLinkDekuSkel, &gLinkHumanSkel,
};

s16 D_801BFE14[PLAYER_BOOTS_MAX][18] = {
    // PLAYER_BOOTS_FIERCE_DEITY
    {
        200,  // REG(19)
        666,  // REG(30)
        200,  // REG(32)
        700,  // REG(34)
        366,  // REG(35)
        200,  // REG(36)
        600,  // REG(37)
        175,  // REG(38)
        60,   // REG(39)
        800,  // R_DECELERATE_RATE
        1000, // R_RUN_SPEED_LIMIT
        -100, // REG(68)
        600,  // REG(69)
        590,  // IREG(66)
        800,  // IREG(67)
        125,  // IREG(68)
        300,  // IREG(69)
        65,   // MREG(95)
    },
    // PLAYER_BOOTS_HYLIAN
    {
        200,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        800,  // REG(34)
        500,  // REG(35)
        400,  // REG(36)
        800,  // REG(37)
        400,  // REG(38)
        120,  // REG(39)
        800,  // R_DECELERATE_RATE
        550,  // R_RUN_SPEED_LIMIT
        -100, // REG(68)
        600,  // REG(69)
        540,  // IREG(66)
        750,  // IREG(67)
        125,  // IREG(68)
        400,  // IREG(69)
        200,  // MREG(95)
    },
    // PLAYER_BOOTS_GIANT
    {
        100,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        800,  // REG(34)
        250,  // REG(35)
        200,  // REG(36)
        800,  // REG(37)
        200,  // REG(38)
        90,   // REG(39)
        800,  // R_DECELERATE_RATE
        350,  // R_RUN_SPEED_LIMIT
        -80,  // REG(68)
        600,  // REG(69)
        540,  // IREG(66)
        750,  // IREG(67)
        60,   // IREG(68)
        200,  // IREG(69)
        200,  // MREG(95)
    },
    // PLAYER_BOOTS_DEKU
    {
        200,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        700,  // REG(34)
        550,  // REG(35)
        270,  // REG(36)
        600,  // REG(37)
        1000, // REG(38)
        120,  // REG(39)
        800,  // R_DECELERATE_RATE
        600,  // R_RUN_SPEED_LIMIT
        -100, // REG(68)
        600,  // REG(69)
        590,  // IREG(66)
        750,  // IREG(67)
        125,  // IREG(68)
        200,  // IREG(69)
        130,  // MREG(95)
    },
    // PLAYER_BOOTS_ZORA_LAND
    {
        200,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        700,  // REG(34)
        550,  // REG(35)
        270,  // REG(36)
        700,  // REG(37)
        300,  // REG(38)
        120,  // REG(39)
        800,  // R_DECELERATE_RATE
        600,  // R_RUN_SPEED_LIMIT
        -100, // REG(68)
        600,  // REG(69)
        590,  // IREG(66)
        750,  // IREG(67)
        125,  // IREG(68)
        200,  // IREG(69)
        130,  // MREG(95)
    },
    // PLAYER_BOOTS_ZORA_UNDERWATER
    {
        200,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        700,  // REG(34)
        550,  // REG(35)
        270,  // REG(36)
        700,  // REG(37)
        300,  // REG(38)
        120,  // REG(39)
        800,  // R_DECELERATE_RATE
        600,  // R_RUN_SPEED_LIMIT
        -100, // REG(68)
        600,  // REG(69)
        590,  // IREG(66)
        750,  // IREG(67)
        125,  // IREG(68)
        200,  // IREG(69)
        130,  // MREG(95)
    },
    // PLAYER_BOOTS_GORON
    {
        200,  // REG(19)
        1000, // REG(30)
        300,  // REG(32)
        700,  // REG(34)
        550,  // REG(35)
        270,  // REG(36)
        700,  // REG(37)
        200,  // REG(38)
        120,  // REG(39)
        800,  // R_DECELERATE_RATE
        600,  // R_RUN_SPEED_LIMIT
        -140, // REG(68)
        600,  // REG(69)
        590,  // IREG(66)
        750,  // IREG(67)
        125,  // IREG(68)
        200,  // IREG(69)
        130,  // MREG(95)
    },
    // PLAYER_BOOTS_7
    {
        80,  // REG(19)
        800, // REG(30)
        150, // REG(32)
        700, // REG(34)
        480, // REG(35)
        270, // REG(36)
        600, // REG(37)
        50,  // REG(38)
        120, // REG(39)
        800, // R_DECELERATE_RATE
        300, // R_RUN_SPEED_LIMIT
        -40, // REG(68)
        400, // REG(69)
        540, // IREG(66)
        270, // IREG(67)
        25,  // IREG(68)
        0,   // IREG(69)
        80,  // MREG(95)
    },
};

// OoT's Player_SetBootData
void func_80123140(PlayState* play, Player* player) {
    s16* bootRegs;
    PlayerBoots currentBoots;
    f32 scale;

    if ((player->actor.id == ACTOR_PLAYER) && (player->transformation == PLAYER_FORM_FIERCE_DEITY)) {
        REG(27) = 1200;
    } else {
        REG(27) = 2000;
    }

    REG(48) = 370;

    currentBoots = player->currentBoots;
    if (currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) {
        if (player->stateFlags1 & PLAYER_STATE1_8000000) {
            currentBoots++;
        }
        if (player->transformation == PLAYER_FORM_GORON) {
            REG(48) = 200;
        }
    } else if (currentBoots == PLAYER_BOOTS_GIANT) {
        REG(48) = 170;
    }

    bootRegs = D_801BFE14[currentBoots];
    REG(19) = bootRegs[0];
    REG(30) = bootRegs[1];
    REG(32) = bootRegs[2];
    REG(34) = bootRegs[3];
    REG(35) = bootRegs[4];
    REG(36) = bootRegs[5];
    REG(37) = bootRegs[6];
    REG(38) = bootRegs[7];
    REG(39) = bootRegs[8];
    R_DECELERATE_RATE = bootRegs[9];
    R_RUN_SPEED_LIMIT = bootRegs[10];
    REG(68) = bootRegs[11]; // gravity
    REG(69) = bootRegs[12];
    IREG(66) = bootRegs[13];
    IREG(67) = bootRegs[14];
    IREG(68) = bootRegs[15];
    IREG(69) = bootRegs[16];
    MREG(95) = bootRegs[17];

    if (play->roomCtx.curRoom.type == ROOM_TYPE_INDOORS) {
        R_RUN_SPEED_LIMIT = 500;
    }

    if ((player->actor.id == ACTOR_PLAYER) && (player->transformation == PLAYER_FORM_FIERCE_DEITY)) {
        scale = 0.015f;
    } else {
        scale = 0.01f;
    }

    Actor_SetScale(&player->actor, scale);
}

// #region 2S2H [Port] Determine if we're using a custom model for link
uint8_t Player_IsCustomLinkModel(Player* player) {
    return ((player->transformation == PLAYER_FORM_HUMAN) && ResourceGetIsCustomByName(gLinkHumanSkel)) ||
           ((player->transformation == PLAYER_FORM_DEKU) && ResourceGetIsCustomByName(gLinkDekuSkel)) ||
           ((player->transformation == PLAYER_FORM_GORON) && ResourceGetIsCustomByName(gLinkGoronSkel)) ||
           ((player->transformation == PLAYER_FORM_ZORA) && ResourceGetIsCustomByName(gLinkZoraSkel)) ||
           ((player->transformation == PLAYER_FORM_FIERCE_DEITY) && ResourceGetIsCustomByName(gLinkFierceDeitySkel)) ||

           // special case for kafei model swaps when using Play as Kafei enhancement
           ((player->transformation == PLAYER_FORM_HUMAN) && CVarGetInteger("gModes.PlayAsKafei", 0) &&
            ResourceGetIsCustomByName(gKafeiSkel));
}
// #endregion

bool Player_InBlockingCsMode(PlayState* play, Player* player) {
    return (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_200 | PLAYER_STATE1_20000000)) ||
           (player->csAction != PLAYER_CSACTION_NONE) || (play->transitionTrigger == TRANS_TRIGGER_START) ||
           (play->transitionMode != TRANS_MODE_OFF) || (player->stateFlags1 & PLAYER_STATE1_1) ||
           (player->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT) || play->actorCtx.isOverrideInputOn;
}

bool Player_InCsMode(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return Player_InBlockingCsMode(play, player) || (player->unk_AA5 == PLAYER_UNKAA5_5);
}

/**
 * Checks if Player is currently locked onto a hostile actor.
 * `PLAYER_STATE3_HOSTILE_LOCK_ON` controls Player's "battle" response to hostile actors.
 *
 * Note that within Player, `Player_UpdateHostileLockOn` exists, which updates the flag and also returns the check.
 * Player can use this function instead if the flag should be checked, but not updated.
 */
bool Player_CheckHostileLockOn(Player* player) {
    return player->stateFlags3 & PLAYER_STATE3_HOSTILE_LOCK_ON;
}

/**
 * This function checks for friendly (non-hostile) Z-Target related states.
 * For hostile related lock-on states, see `Player_UpdateHostileLockOn` and `Player_CheckHostileLockOn`.
 *
 * Note that `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will include all `focusActor` use cases that relate to
 * friendly actors. This function can return true when talking to an actor, for example.
 * Despite that, this function is only relevant in the context of actor lock-on, which is a subset of actor focus.
 * This is why the function name states `FriendlyLockOn` instead of `FriendlyActorFocus`.
 *
 * There is a special case that allows hostile actors to be treated as "friendly" if Player is carrying another actor
 * See relevant code in `Player_UpdateZTargeting` for more details.
 *
 * Additionally, `PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE` will be set very briefly in some conditions when
 * a lock-on is forced to release. In these niche cases, this function will apply to both friendly and hostile actors.
 * Overall, it is safe to assume that this specific state flag is not very relevant for this function's use cases.
 */
bool Player_FriendlyLockOnOrParallel(Player* player) {
    return player->stateFlags1 &
           (PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE);
}

// Unused
bool func_80123448(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags1 & PLAYER_STATE1_400000) &&
           ((player->transformation != PLAYER_FORM_HUMAN) ||
            (!Player_FriendlyLockOnOrParallel(player) && (player->focusActor == NULL)));
}

// TODO: Player_IsGoronOrDeku is a temporary name until we have more info on this function.
// Hypothesis: this function checks if the current form would crouch when he tries to use the shield
bool Player_IsGoronOrDeku(Player* player) {
    return (player->transformation == PLAYER_FORM_GORON) || (player->transformation == PLAYER_FORM_DEKU);
}

bool func_801234D4(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->stateFlags2 & PLAYER_STATE2_8) || (player->actor.speed != 0.0f) ||
           ((player->transformation != PLAYER_FORM_ZORA) && (player->stateFlags1 & PLAYER_STATE1_8000000)) ||
           ((player->transformation == PLAYER_FORM_ZORA) && (player->stateFlags1 & PLAYER_STATE1_8000000) &&
            (!(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
             (player->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER)));
}

bool func_80123590(PlayState* play, Actor* actor) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (player->heldActor == actor)) {
        player->interactRangeActor = NULL;
        player->heldActor = NULL;
        player->actor.child = NULL;
        player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        return true;
    }

    return false;
}

s32 func_801235DC(PlayState* play, f32 arg1, s16 arg2) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags3 & PLAYER_STATE3_1000) {
        player->unk_B08 = arg1;
        player->unk_B0C += arg1 * 0.05f;
        player->yaw = arg2;
        player->actor.home.rot.y = arg2;
        player->actor.shape.rot.y = arg2;
        player->unk_B8C = 4;
        player->invincibilityTimer = 20;

        return true;
    }

    return false;
}

// #region 2S2H [Dpad]
ItemId Player_Dpad_GetItemOnButton(PlayState* play, Player* player, DpadEquipSlot slot) {
    if (slot >= EQUIP_SLOT_D_MAX) {
        return ITEM_NONE;
    }

    ItemId item = DPAD_BTN_ITEM(slot);
    GameInteractor_Should(VB_GET_ITEM_ON_BUTTON, item, slot, &item);
    return item;
}
// #endregion

ItemId Player_GetItemOnButton(PlayState* play, Player* player, EquipSlot slot) {
    if (slot >= EQUIP_SLOT_A) {
        return ITEM_NONE;
    }

    if (slot == EQUIP_SLOT_B) {
        ItemId item = Inventory_GetBtnBItem(play);

        GameInteractor_Should(VB_GET_ITEM_ON_BUTTON, item, slot, &item);

        if (item >= ITEM_FD) {
            return item;
        }

        if ((player->currentMask == PLAYER_MASK_BLAST) &&
            (play->interfaceCtx.bButtonPlayerDoAction == DO_ACTION_EXPLODE)) {
            return ITEM_F0;
        }

        if ((player->currentMask == PLAYER_MASK_BREMEN) &&
            (play->interfaceCtx.bButtonPlayerDoAction == DO_ACTION_MARCH)) {
            return ITEM_F1;
        }

        if ((player->currentMask == PLAYER_MASK_KAMARO) &&
            (play->interfaceCtx.bButtonPlayerDoAction == DO_ACTION_DANCE)) {
            return ITEM_F2;
        }

        return item;
    }

    if (slot == EQUIP_SLOT_C_LEFT) {
        ItemId item = C_BTN_ITEM(EQUIP_SLOT_C_LEFT);
        GameInteractor_Should(VB_GET_ITEM_ON_BUTTON, item, slot, &item);
        return item;
    }

    if (slot == EQUIP_SLOT_C_DOWN) {
        ItemId item = C_BTN_ITEM(EQUIP_SLOT_C_DOWN);
        GameInteractor_Should(VB_GET_ITEM_ON_BUTTON, item, slot, &item);
        return item;
    }

    // EQUIP_SLOT_C_RIGHT
    {
        ItemId item = C_BTN_ITEM(EQUIP_SLOT_C_RIGHT);
        GameInteractor_Should(VB_GET_ITEM_ON_BUTTON, item, slot, &item);
        return item;
    }
}

// #region 2S2H [Dpad]
u16 sDItemButtons[] = {
    BTN_DRIGHT,
    BTN_DLEFT,
    BTN_DDOWN,
    BTN_DUP,
};
// #endregion

u16 sCItemButtons[] = {
    BTN_CLEFT,
    BTN_CDOWN,
    BTN_CRIGHT,
};

PlayerItemAction func_80123810(PlayState* play) {
    Player* player = GET_PLAYER(play);
    PlayerItemAction itemAction;
    ItemId itemId;
    s32 i;

    if (gSaveContext.save.unk_06 == 0) {
        if (CHECK_BTN_ANY(CONTROLLER1(&play->state)->press.button, BTN_A | BTN_B)) {
            play->interfaceCtx.bButtonInterfaceDoActionActive = false;
            play->interfaceCtx.bButtonInterfaceDoAction = 0;
            Interface_SetHudVisibility(play->msgCtx.hudVisibility);
            return PLAYER_IA_MINUS1;
        }
    } else {
        gSaveContext.save.unk_06--;
    }

    for (i = 0; i < ARRAY_COUNT(sCItemButtons); i++) {
        if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, sCItemButtons[i])) {
            i++;
            itemId = Player_GetItemOnButton(play, player, i);

            play->interfaceCtx.bButtonInterfaceDoActionActive = false;
            play->interfaceCtx.bButtonInterfaceDoAction = 0;
            Interface_SetHudVisibility(play->msgCtx.hudVisibility);

            if ((itemId >= ITEM_FD) || ((itemAction = play->unk_18794(play, player, itemId)) <= PLAYER_IA_MINUS1)) {
                Audio_PlaySfx(NA_SE_SY_ERROR);
                return PLAYER_IA_MINUS1;
            } else {
                s32 pad;

                player->heldItemButton = i;
                return itemAction;
            }
        }
    }
    // #region 2S2H [Dpad]
    if (CVarGetInteger("gEnhancements.Dpad.DpadEquips", 0)) {
        for (i = 0; i < ARRAY_COUNT(sDItemButtons); i++) {
            if (CHECK_BTN_ALL(CONTROLLER1(&play->state)->press.button, sDItemButtons[i])) {
                itemId = Player_Dpad_GetItemOnButton(play, player, i);

                play->interfaceCtx.bButtonInterfaceDoActionActive = 0;
                play->interfaceCtx.bButtonInterfaceDoAction = 0;
                Interface_SetHudVisibility(play->msgCtx.hudVisibility);

                if ((itemId >= ITEM_FD) || ((itemAction = play->unk_18794(play, player, itemId)) <= PLAYER_IA_MINUS1)) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    return PLAYER_IA_MINUS1;
                } else {
                    s32 pad;

                    player->heldItemButton = DPAD_TO_HELD_ITEM(i);
                    return itemAction;
                }
            }
        }
    }
    // #endregion

    return PLAYER_IA_NONE;
}

// Used to map item actions to model groups
u8 sActionModelGroups[PLAYER_IA_MAX] = {
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_NONE
    PLAYER_MODELGROUP_13,             // PLAYER_IA_LAST_USED
    PLAYER_MODELGROUP_DEKU_STICK,     // PLAYER_IA_FISHING_ROD
    PLAYER_MODELGROUP_ONE_HAND_SWORD, // PLAYER_IA_SWORD_KOKIRI
    PLAYER_MODELGROUP_ONE_HAND_SWORD, // PLAYER_IA_SWORD_RAZOR
    PLAYER_MODELGROUP_ONE_HAND_SWORD, // PLAYER_IA_SWORD_GILDED
    PLAYER_MODELGROUP_TWO_HAND_SWORD, // PLAYER_IA_SWORD_TWO_HANDED
    PLAYER_MODELGROUP_DEKU_STICK,     // PLAYER_IA_DEKU_STICK
    PLAYER_MODELGROUP_ZORA_BOOMERANG, // PLAYER_IA_ZORA_BOOMERANG
    PLAYER_MODELGROUP_BOW,            // PLAYER_IA_BOW
    PLAYER_MODELGROUP_BOW,            // PLAYER_IA_BOW_FIRE
    PLAYER_MODELGROUP_BOW,            // PLAYER_IA_BOW_ICE
    PLAYER_MODELGROUP_BOW,            // PLAYER_IA_BOW_LIGHT
    PLAYER_MODELGROUP_HOOKSHOT,       // PLAYER_IA_HOOKSHOT
    PLAYER_MODELGROUP_EXPLOSIVES,     // PLAYER_IA_BOMB
    PLAYER_MODELGROUP_EXPLOSIVES,     // PLAYER_IA_POWDER_KEG
    PLAYER_MODELGROUP_EXPLOSIVES,     // PLAYER_IA_BOMBCHU
    PLAYER_MODELGROUP_8,              // PLAYER_IA_11
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_DEKU_NUT
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_PICTOGRAPH_BOX
    PLAYER_MODELGROUP_INSTRUMENT,     // PLAYER_IA_OCARINA
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_EMPTY
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_FISH
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_SPRING_WATER
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_HOT_SPRING_WATER
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_ZORA_EGG
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_DEKU_PRINCESS
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_GOLD_DUST
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_1C
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_SEAHORSE
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_MUSHROOM
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_HYLIAN_LOACH
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_BUG
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_POE
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_BIG_POE
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_POTION_RED
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_POTION_BLUE
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_POTION_GREEN
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_MILK
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_MILK_HALF
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_CHATEAU
    PLAYER_MODELGROUP_BOTTLE,         // PLAYER_IA_BOTTLE_FAIRY
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MOONS_TEAR
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_DEED_LAND
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_ROOM_KEY
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_LETTER_TO_KAFEI
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MAGIC_BEANS
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_DEED_SWAMP
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_DEED_MOUNTAIN
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_DEED_OCEAN
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_32
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_LETTER_MAMA
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_34
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_35
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_PENDANT_OF_MEMORIES
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_37
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_38
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_39
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_TRUTH
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_KAFEIS_MASK
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_ALL_NIGHT
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_BUNNY
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_KEATON
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_GARO
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_ROMANI
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_CIRCUS_LEADER
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_POSTMAN
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_COUPLE
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_GREAT_FAIRY
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_GIBDO
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_DON_GERO
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_KAMARO
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_CAPTAIN
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_STONE
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_BREMEN
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_BLAST
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_SCENTS
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_GIANT
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_FIERCE_DEITY
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_GORON
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_ZORA
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_MASK_DEKU
    PLAYER_MODELGROUP_DEFAULT,        // PLAYER_IA_LENS_OF_TRUTH
};

PlayerModelGroup Player_ActionToModelGroup(Player* player, PlayerItemAction itemAction) {
    PlayerModelGroup modelGroup = sActionModelGroups[itemAction];

    if ((modelGroup == PLAYER_MODELGROUP_ONE_HAND_SWORD) && Player_IsGoronOrDeku(player)) {
        return PLAYER_MODELGROUP_1;
    }
    return modelGroup;
}

u8 D_801BFF90[PLAYER_FORM_MAX] = {
    PLAYER_BOOTS_FIERCE_DEITY, // PLAYER_FORM_FIERCE_DEITY
    PLAYER_BOOTS_GORON,        // PLAYER_FORM_GORON
    PLAYER_BOOTS_ZORA_LAND,    // PLAYER_FORM_ZORA
    PLAYER_BOOTS_DEKU,         // PLAYER_FORM_DEKU
    PLAYER_BOOTS_HYLIAN,       // PLAYER_FORM_HUMAN
};

u8 sPlayerStrengths[PLAYER_FORM_MAX] = {
    PLAYER_STRENGTH_HUMAN, // PLAYER_FORM_FIERCE_DEITY
    PLAYER_STRENGTH_GORON, // PLAYER_FORM_GORON
    PLAYER_STRENGTH_ZORA,  // PLAYER_FORM_ZORA
    PLAYER_STRENGTH_DEKU,  // PLAYER_FORM_DEKU
    PLAYER_STRENGTH_HUMAN, // PLAYER_FORM_HUMAN
};

typedef struct {
    /* 0x0 */ u8 flag;
    /* 0x2 */ u16 textId;
} EnvHazardTextTriggerEntry; // size = 0x4

// These textIds are OoT remnants. The corresponding text entries are not present in this game, and so these don't point
// to anything relevant.
EnvHazardTextTriggerEntry sEnvHazardTextTriggers[] = {
    { ENV_HAZARD_TEXT_TRIGGER_HOTROOM, 0x26FC },    // PLAYER_ENV_HAZARD_HOTROOM - 1
    { ENV_HAZARD_TEXT_TRIGGER_UNDERWATER, 0x26FD }, // PLAYER_ENV_HAZARD_UNDERWATER_FLOOR - 1
    { 0, 0 },                                       // PLAYER_ENV_HAZARD_SWIMMING - 1
    { ENV_HAZARD_TEXT_TRIGGER_UNDERWATER, 0x26FD }, // PLAYER_ENV_HAZARD_UNDERWATER_FREE - 1
};

PlayerModelIndices gPlayerModelTypes[PLAYER_MODELGROUP_MAX] = {
    /* PLAYER_MODELGROUP_0 */
    { PLAYER_ANIMTYPE_2, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_SHIELD, PLAYER_MODELTYPE_SHEATH_12,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_1 */
    { PLAYER_ANIMTYPE_1, PLAYER_MODELTYPE_LH_ONE_HAND_SWORD, PLAYER_MODELTYPE_RH_CLOSED, PLAYER_MODELTYPE_SHEATH_15,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_ONE_HAND_SWORD */
    { PLAYER_ANIMTYPE_1, PLAYER_MODELTYPE_LH_ONE_HAND_SWORD, PLAYER_MODELTYPE_RH_SHIELD, PLAYER_MODELTYPE_SHEATH_13,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_DEFAULT */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_4 */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_TWO_HAND_SWORD */
    { PLAYER_ANIMTYPE_3, PLAYER_MODELTYPE_LH_TWO_HAND_SWORD, PLAYER_MODELTYPE_RH_CLOSED, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_BOW */
    { PLAYER_ANIMTYPE_4, PLAYER_MODELTYPE_LH_CLOSED, PLAYER_MODELTYPE_RH_BOW, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_EXPLOSIVES */
    { PLAYER_ANIMTYPE_5, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_8 */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_4, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_HOOKSHOT */
    { PLAYER_ANIMTYPE_4, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_HOOKSHOT, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_DEKU_STICK */
    { PLAYER_ANIMTYPE_3, PLAYER_MODELTYPE_LH_CLOSED, PLAYER_MODELTYPE_RH_CLOSED, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_INSTRUMENT */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_OPEN, PLAYER_MODELTYPE_RH_INSTRUMENT, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_BOTTLE */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_BOTTLE, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_13 */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_ONE_HAND_SWORD, PLAYER_MODELTYPE_RH_OPEN, PLAYER_MODELTYPE_SHEATH_15,
      PLAYER_MODELTYPE_WAIST },
    /* PLAYER_MODELGROUP_ZORA_BOOMERANG */
    { PLAYER_ANIMTYPE_DEFAULT, PLAYER_MODELTYPE_LH_CLOSED, PLAYER_MODELTYPE_RH_CLOSED, PLAYER_MODELTYPE_SHEATH_14,
      PLAYER_MODELTYPE_WAIST },
};

/* DLists groups start */
// Note: Only the ones with size 2 * PLAYER_FORM_MAX

Gfx* gPlayerWaistDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityWaistDL, gLinkFierceDeityWaistDL, gLinkGoronWaistDL, gLinkGoronWaistDL, gLinkZoraWaistDL,
    gLinkZoraWaistDL,        gLinkDekuWaistDL,        gLinkDekuWaistDL,  gLinkHumanWaistDL, gLinkHumanWaistDL,
};

Gfx* gPlayerHandHoldingShields[2 * (PLAYER_SHIELD_MAX - 1)] = {
    gLinkHumanRightHandHoldingHerosShieldDL,
    gLinkHumanRightHandHoldingHerosShieldDL,
    gLinkHumanRightHandHoldingMirrorShieldDL,
    gLinkHumanRightHandHoldingMirrorShieldDL,
};

Gfx* gPlayerSheath12DLs[2 * PLAYER_FORM_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    gLinkDekuSheathDL,
    gLinkDekuSheathDL,
    gLinkHumanSheathEmptyDL,
    gLinkHumanSheathEmptyDL,
};

Gfx* gPlayerSheath13DLs[2 * PLAYER_FORM_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    gLinkDekuSheathDL,
    gLinkDekuSheathDL,
    gLinkHumanSheathEmptyDL,
    gLinkHumanSheathEmptyDL,
};

Gfx* gPlayerSheath14DLs[2 * PLAYER_FORM_MAX] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    gLinkDekuSheathDL,
    gLinkDekuSheathDL,
    gLinkHumanSheathEmptyDL,
    gLinkHumanSheathEmptyDL,
};

Gfx* gPlayerShields[] = {
    gLinkHumanHerosShieldWithMtxDL,
    gLinkHumanHerosShieldWithMtxDL,
    gLinkHumanMirrorShieldWithMtxDL,
    gLinkHumanMirrorShieldWithMtxDL,
};

Gfx* gPlayerSheathedSwords[] = {
    gLinkHumanSheathedKokiriSwordDL, gLinkHumanSheathedKokiriSwordDL, gLinkHumanSheathedRazorSwordDL,
    gLinkHumanSheathedRazorSwordDL,  gLinkHumanSheathedGildedSwordDL, gLinkHumanSheathedGildedSwordDL,
};

Gfx* gPlayerSwordSheaths[] = {
    gLinkHumanKokiriSwordSheathDL, gLinkHumanKokiriSwordSheathDL, gLinkHumanRazorSwordSheathDL,
    gLinkHumanRazorSwordSheathDL,  gLinkHumanGildedSwordSheathDL, gLinkHumanGildedSwordSheathDL,
};

Gfx* gPlayerLeftHandTwoHandSwordDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandHoldingSwordDL,
    gLinkFierceDeityLeftHandHoldingSwordDL,
    gLinkGoronLeftHandOpenDL,
    gLinkGoronLeftHandOpenDL,
    gLinkZoraLeftHandOpenDL,
    gLinkZoraLeftHandOpenDL,
    gLinkDekuLeftHandDL,
    gLinkDekuLeftHandDL,
    gLinkHumanLeftHandHoldingGreatFairysSwordDL,
    gLinkHumanLeftHandHoldingGreatFairysSwordDL,
};

Gfx* gPlayerLeftHandOpenDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandDL, gLinkFierceDeityLeftHandDL, gLinkGoronLeftHandOpenDL, gLinkGoronLeftHandOpenDL,
    gLinkZoraLeftHandOpenDL,    gLinkZoraLeftHandOpenDL,    gLinkDekuLeftHandDL,      gLinkDekuLeftHandDL,
    gLinkHumanLeftHandOpenDL,   gLinkHumanLeftHandOpenDL,
};

Gfx* gPlayerLeftHandClosedDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandDL, gLinkFierceDeityLeftHandDL, gLinkGoronLeftHandClosedDL, gLinkGoronLeftHandClosedDL,
    gLinkZoraLeftHandClosedDL,  gLinkZoraLeftHandClosedDL,  gLinkDekuLeftHandDL,        gLinkDekuLeftHandDL,
    gLinkHumanLeftHandClosedDL, gLinkHumanLeftHandClosedDL,
};

Gfx* gPlayerLeftHandOneHandSwordDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandDL,
    gLinkFierceDeityLeftHandDL,
    gLinkGoronLeftHandOpenDL,
    gLinkGoronLeftHandOpenDL,
    gLinkZoraLeftHandClosedDL,
    gLinkZoraLeftHandClosedDL,
    gLinkDekuLeftHandDL,
    gLinkDekuLeftHandDL,
    gLinkHumanLeftHandHoldingKokiriSwordDL,
    gLinkHumanLeftHandHoldingKokiriSwordDL,
};

Gfx* D_801C018C[] = {
    // EQUIP_VALUE_SWORD_KOKIRI
    gLinkHumanLeftHandHoldingKokiriSwordDL,
    gLinkHumanLeftHandHoldingKokiriSwordDL,
    // EQUIP_VALUE_SWORD_RAZOR
    gLinkHumanLeftHandHoldingRazorSwordDL,
    gLinkHumanLeftHandHoldingRazorSwordDL,
    // EQUIP_VALUE_SWORD_GILDED
    gLinkHumanLeftHandHoldingGildedSwordDL,
    gLinkHumanLeftHandHoldingGildedSwordDL,
};

Gfx* gPlayerRightHandOpenDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL, gLinkFierceDeityRightHandDL, gLinkGoronRightHandOpenDL, gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,    gLinkZoraRightHandOpenDL,    gLinkDekuRightHandDL,      gLinkDekuRightHandDL,
    gLinkHumanRightHandOpenDL,   gLinkHumanRightHandOpenDL,
};

Gfx* gPlayerRightHandClosedDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL, gLinkFierceDeityRightHandDL, gLinkGoronRightHandClosedDL, gLinkGoronRightHandClosedDL,
    gLinkZoraRightHandClosedDL,  gLinkZoraRightHandClosedDL,  gLinkDekuRightHandDL,        gLinkDekuRightHandDL,
    gLinkHumanRightHandClosedDL, gLinkHumanRightHandClosedDL,
};

Gfx* gPlayerRightHandBowDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL,
    gLinkFierceDeityRightHandDL,
    gLinkGoronRightHandOpenDL,
    gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkDekuRightHandDL,
    gLinkDekuRightHandDL,
    gLinkHumanRightHandHoldingBowDL,
    gLinkHumanRightHandHoldingBowDL,
};

Gfx* gPlayerRightHandInstrumentDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL,
    gLinkFierceDeityRightHandDL,
    gLinkGoronRightHandOpenDL,
    gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkDekuRightHandDL,
    gLinkDekuRightHandDL,
    gLinkHumanRightHandHoldingOcarinaDL,
    gLinkHumanRightHandHoldingOcarinaDL,
};

Gfx* gPlayerRightHandHookshotDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL,
    gLinkFierceDeityRightHandDL,
    gLinkGoronRightHandOpenDL,
    gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkDekuRightHandDL,
    gLinkDekuRightHandDL,
    gLinkHumanRightHandHoldingHookshotDL,
    gLinkHumanRightHandHoldingHookshotDL,
};

Gfx* gPlayerLeftHandBottleDLs[2 * PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandDL,
    gLinkFierceDeityLeftHandDL,
    gLinkGoronLeftHandHoldBottleDL,
    gLinkGoronLeftHandHoldBottleDL,
    gLinkZoraLeftHandHoldBottleDL,
    gLinkZoraLeftHandHoldBottleDL,
    gLinkDekuLeftHandDL,
    gLinkDekuLeftHandDL,
    gLinkHumanLeftHandHoldBottleDL,
    gLinkHumanLeftHandHoldBottleDL,
};

/* DLists groups end */

Gfx* sPlayerFirstPersonLeftForearmDLs[PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftForearmDL, gLinkGoronLeftForearmDL, gLinkZoraLeftForearmDL,
    gLinkDekuLeftForearmDL,        gLinkHumanLeftForearmDL,
};

Gfx* sPlayerFirstPersonLeftHandDLs[PLAYER_FORM_MAX] = {
    gLinkFierceDeityLeftHandDL, gLinkGoronLeftHandOpenDL,   gLinkZoraLeftHandClosedDL,
    gLinkDekuLeftHandDL,        gLinkHumanLeftHandClosedDL,
};

Gfx* sPlayerFirstPersonRightShoulderDLs[PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightShoulderDL, gLinkGoronRightShoulderDL, gLinkZoraRightShoulderDL,
    gLinkDekuRightShoulderDL,        gLinkHumanRightShoulderDL,
};

Gfx* sPlayerFirstPersonRightHandDLs[PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL,
    //! @bug This is in the middle of a texture in the link_goron object. It has the same offset as a link_nuts dlist
    // 2S2H [Port] Put the DList that makes the most sense here. In reality nothing makes sense since goron has no first
    // person items, but we need something here to prevent a crash.
    gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkDekuRightHandDL,
    object_link_child_DL_018490,
};

Gfx* sPlayerFirstPersonRightHandHookshotDLs[PLAYER_FORM_MAX] = {
    gLinkFierceDeityRightHandDL,
    //! @bug This is in the middle of a texture in the link_goron object. It has the same offset as a link_nuts dlist
    // 2S2H [Port] Put the DList that makes the most sense here. In reality nothing makes sense since goron has no first
    // person items, but we need something here to prevent a crash.
    gLinkGoronRightHandOpenDL,
    gLinkZoraRightHandOpenDL,
    gLinkDekuRightHandDL,
    object_link_child_DL_017B40,
};

// Indexed by model types (left hand, right hand, sheath or waist)
Gfx** sPlayerDListGroups[PLAYER_MODELTYPE_MAX] = {
    gPlayerLeftHandOpenDLs,         // PLAYER_MODELTYPE_LH_OPEN
    gPlayerLeftHandClosedDLs,       // PLAYER_MODELTYPE_LH_CLOSED
    gPlayerLeftHandOneHandSwordDLs, // PLAYER_MODELTYPE_LH_ONE_HAND_SWORD
    gPlayerLeftHandTwoHandSwordDLs, // PLAYER_MODELTYPE_LH_TWO_HAND_SWORD
    gPlayerLeftHandOpenDLs,         // PLAYER_MODELTYPE_LH_4
    gPlayerLeftHandBottleDLs,       // PLAYER_MODELTYPE_LH_BOTTLE
    gPlayerRightHandOpenDLs,        // PLAYER_MODELTYPE_RH_OPEN
    gPlayerRightHandClosedDLs,      // PLAYER_MODELTYPE_RH_CLOSED
    gPlayerRightHandClosedDLs,      // PLAYER_MODELTYPE_RH_SHIELD
    gPlayerRightHandBowDLs,         // PLAYER_MODELTYPE_RH_BOW
    gPlayerRightHandInstrumentDLs,  // PLAYER_MODELTYPE_RH_INSTRUMENT
    gPlayerRightHandHookshotDLs,    // PLAYER_MODELTYPE_RH_HOOKSHOT
    gPlayerSheath12DLs,             // PLAYER_MODELTYPE_SHEATH_12
    gPlayerSheath13DLs,             // PLAYER_MODELTYPE_SHEATH_13
    gPlayerSheath14DLs,             // PLAYER_MODELTYPE_SHEATH_14
    gPlayerSheath14DLs,             // PLAYER_MODELTYPE_SHEATH_15
    gPlayerWaistDLs,                // PLAYER_MODELTYPE_WAIST
    NULL,                           // PLAYER_MODELTYPE_17
};

struct_80124618 D_801C0340[] = {
    { 0, { 0, 0, 0 } },       { 5, { 0, 0, 0 } },        { 7, { 100, 100, 100 } },
    { 9, { 110, 110, 110 } }, { 11, { 100, 100, 100 } },
};
struct_80124618 D_801C0368[] = {
    { 0, { 0, 0, 0 } },       { 4, { 0, 0, 0 } },      { 6, { 120, 150, 60 } },   { 8, { 130, 80, 160 } },
    { 9, { 100, 100, 100 } }, { 10, { 90, 100, 90 } }, { 11, { 100, 100, 100 } },
};
struct_80124618 D_801C03A0[] = {
    { 0, { 100, 100, 100 } },
    { 2, { 120, 120, 120 } },
    { 6, { 90, 90, 90 } },
    { 7, { 93, 93, 93 } },
};
struct_80124618 D_801C03C0[] = {
    { 0, { 200, 100, 110 } },
    { 2, { 90, 100, 100 } },
    { 3, { 100, 100, 100 } },
    { 7, { 100, 100, 100 } },
};
struct_80124618 D_801C03E0[] = {
    { 0, { 100, 100, 110 } }, { 2, { 60, 100, 80 } },    { 3, { 130, 105, 110 } },
    { 7, { 130, 105, 110 } }, { 10, { 100, 100, 100 } }, { 19, { 100, 100, 100 } },
};
struct_80124618 D_801C0410[] = {
    { 0, { 0, 0, 0 } },
    { 2, { 80, 110, 80 } },
    { 3, { 100, 100, 100 } },
};
struct_80124618 D_801C0428[] = {
    { 0, { 0, 0, 0 } },      { 6, { 0, 0, 0 } },        { 7, { 60, 60, 50 } },     { 8, { 120, 130, 100 } },
    { 9, { 100, 120, 80 } }, { 11, { 100, 100, 100 } }, { 13, { 100, 100, 100 } },
};
struct_80124618 D_801C0460[] = {
    { 0, { 100, 100, 100 } }, { 5, { 100, 100, 100 } }, { 7, { 100, 74, 80 } },
    { 8, { 100, 180, 130 } }, { 10, { 100, 80, 80 } },  { 13, { 100, 100, 100 } },
};
struct_80124618 D_801C0490[] = {
    { 0, { 100, 100, 100 } },  { 1, { 100, 100, 100 } },  { 2, { 90, 100, 105 } },  { 4, { 110, 100, 100 } },
    { 5, { 90, 100, 105 } },   { 6, { 100, 100, 100 } },  { 7, { 90, 100, 105 } },  { 8, { 100, 100, 100 } },
    { 9, { 90, 100, 105 } },   { 10, { 100, 100, 100 } }, { 11, { 90, 100, 105 } }, { 12, { 110, 100, 100 } },
    { 13, { 100, 100, 100 } }, { 14, { 90, 100, 105 } },  { 15, { 90, 100, 105 } }, { 17, { 100, 100, 100 } },
};
struct_80124618 D_801C0510[] = {
    { 0, { 100, 100, 100 } }, { 4, { 100, 100, 100 } }, { 5, { 90, 110, 100 } },
    { 6, { 110, 105, 100 } }, { 8, { 100, 100, 100 } },
};
struct_80124618 D_801C0538[] = {
    { 0, { 100, 100, 100 } }, { 5, { 100, 100, 100 } },  { 6, { 0, 0, 0 } },
    { 8, { 100, 100, 100 } }, { 14, { 100, 100, 100 } },
};
struct_80124618 D_801C0560[] = {
    { 0, { 100, 100, 100 } },
    { 2, { 95, 95, 100 } },
    { 3, { 105, 105, 100 } },
    { 5, { 102, 102, 102 } },
    //! @bug When playing zora guitar, this is passed into func_80124618 with a curFrame of 6, and func_80124618
    // will overflow it into the next struct until it hits the value with 9
    // #region 2S2H [Port] We are adding the next two animation stop values here to prevent OOB reads
    { 0, { 100, 100, 100 } },
    { 9, { 100, 100, 100 } },
    // #endregion
};
struct_80124618 D_801C0580[] = {
    { 0, { 100, 100, 100 } }, { 9, { 100, 100, 100 } }, { 10, { 150, 150, 150 } },
    { 12, { 0, 0, 0 } },      { 14, { 0, 0, 0 } },
};
struct_80124618 D_801C05A8[] = {
    { 0, { 100, 100, 100 } },
    { 6, { 100, 100, 100 } },
    { 7, { 0, 0, 0 } },
    { 17, { 0, 0, 0 } },
};
struct_80124618 D_801C05C8[] = {
    { 0, { 0, 0, 0 } },
    { 17, { 50, 50, 50 } },
};
struct_80124618 D_801C05D8[] = {
    { 0, { 0, 0, 0 } },
    { 5, { 0, 0, 0 } },
    { 9, { 100, 100, 100 } },
};
struct_80124618 D_801C05F0[] = {
    { 0, { 100, 100, 100 } },
    { 5, { 100, 100, 100 } },
    { 9, { 0, 0, 0 } },
};
struct_80124618 D_801C0608[] = {
    { 0, { 100, 100, 100 } },
    { 10, { 100, 100, 100 } },
    { 32, { 100, 115, 105 } },
    { 58, { 100, 101, 100 } },
};
struct_80124618 D_801C0628[] = {
    { 0, { 100, 100, 100 } },  { 5, { 100, 100, 100 } },  { 15, { 100, 100, 105 } }, { 25, { 100, 100, 105 } },
    { 34, { 100, 100, 100 } }, { 46, { 100, 100, 100 } }, { 57, { 100, 100, 105 } }, { 67, { 100, 100, 105 } },
    { 76, { 100, 100, 100 } }, { 78, { 100, 100, 100 } },
};
struct_80124618 D_801C0678[] = {
    { 0, { 0, 0, 0 } },
    { 6, { 0, 0, 0 } },
    { 8, { 100, 100, 100 } },
    { 11, { 100, 100, 100 } },
};
struct_80124618 D_801C0698[] = {
    { 0, { 0, 0, 0 } },
    { 7, { 1120, 112, 112 } },
    { 8, { 140, 168, 168 } },
    { 11, { 140, 140, 140 } },
};
struct_80124618 D_801C06B8[] = {
    { 0, { 100, 100, 100 } }, { 7, { 100, 100, 100 } }, { 8, { 70, 30, 70 } }, { 10, { 0, 0, 0 } }, { 14, { 0, 0, 0 } },
};
struct_80124618 D_801C06E0[] = {
    { 0, { 140, 140, 140 } },
    { 1, { 0, 0, 0 } },
    { 14, { 0, 0, 0 } },
};
struct_80124618 D_801C06F8[] = {
    { 0, { 100, 100, 100 } },
    { 5, { 100, 100, 100 } },
    { 6, { 0, 0, 0 } },
    { 10, { 0, 0, 0 } },
};
struct_80124618 D_801C0718[] = {
    { 0, { 140, 140, 140 } },
    { 1, { 0, 0, 0 } },
    { 10, { 0, 0, 0 } },
};
struct_80124618 D_801C0730[] = {
    { 0, { 100, 100, 100 } },
    { 13, { 100, 100, 100 } },
};
struct_80124618 D_801C0740[] = {
    { 0, { 140, 140, 140 } },
    { 13, { 140, 140, 140 } },
};
struct_80124618 D_801C0750[] = {
    { 0, { 100, 100, 100 } },  { 5, { 100, 100, 100 } },  { 6, { 200, 200, 200 } },
    { 10, { 200, 200, 200 } }, { 11, { 100, 100, 100 } },
};

// alpha values
u8 D_801C0778[] = {
    0, 0, 0, 0, 0, 100, 200, 255, 255, 255, 200, 100,
};

struct_80124618 D_801C0784[] = {
    { 0, { 100, 100, 100 } },  { 14, { 100, 100, 100 } }, { 15, { 200, 200, 200 } },
    { 16, { 200, 200, 200 } }, { 18, { 100, 100, 100 } },
};

u8 D_801C07AC[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 200, 255, 255, 255, 200, 100, 0,
};

void Player_SetModelsForHoldingShield(Player* player) {
    if (player->stateFlags1 & PLAYER_STATE1_400000) {
        if ((player->itemAction <= PLAYER_IA_MINUS1) || (player->itemAction == player->heldItemAction)) {
            if (!Player_IsHoldingTwoHandedWeapon(player)) {
                if (!Player_IsGoronOrDeku(player)) {
                    D_801F59E0 = player->transformation * 2;
                    player->rightHandType = PLAYER_MODELTYPE_RH_SHIELD;
                    player->rightHandDLists = &sPlayerDListGroups[PLAYER_MODELTYPE_RH_SHIELD][D_801F59E0];

                    if (player->sheathType == PLAYER_MODELTYPE_SHEATH_14) {
                        player->sheathType = PLAYER_MODELTYPE_SHEATH_12;
                    } else if (player->sheathType == PLAYER_MODELTYPE_SHEATH_15) {
                        player->sheathType = PLAYER_MODELTYPE_SHEATH_13;
                    }

                    player->sheathDLists = &sPlayerDListGroups[player->sheathType][D_801F59E0];
                    player->modelAnimType = PLAYER_ANIMTYPE_2;
                    player->itemAction = PLAYER_IA_MINUS1;
                }
            }
        }
    }
}

void Player_SetModels(Player* player, PlayerModelGroup modelGroup) {
    PlayerModelIndices* playerModelTypes;

    D_801F59E0 = player->transformation * 2;
    player->leftHandType = gPlayerModelTypes[modelGroup].leftHandType;
    player->rightHandType = gPlayerModelTypes[modelGroup].rightHandType;
    player->sheathType = gPlayerModelTypes[modelGroup].sheathType;

    if (player->sheathType == PLAYER_MODELTYPE_SHEATH_14) {
        if (CUR_FORM_EQUIP(EQUIP_SLOT_B) == ITEM_NONE) {
            player->sheathType = PLAYER_MODELTYPE_SHEATH_15;
        }
    }

    playerModelTypes = &gPlayerModelTypes[modelGroup];

    player->leftHandDLists = &sPlayerDListGroups[playerModelTypes->leftHandType][D_801F59E0];
    player->rightHandDLists = &sPlayerDListGroups[playerModelTypes->rightHandType][D_801F59E0];
    player->sheathDLists = &sPlayerDListGroups[playerModelTypes->sheathType][D_801F59E0];
    player->waistDLists = &sPlayerDListGroups[playerModelTypes->waistType][D_801F59E0];

    Player_SetModelsForHoldingShield(player);
}

void Player_SetModelGroup(Player* player, PlayerModelGroup modelGroup) {
    player->modelGroup = modelGroup;

    if (modelGroup == PLAYER_MODELGROUP_1) {
        player->modelAnimType = PLAYER_ANIMTYPE_DEFAULT;
    } else {
        player->modelAnimType = gPlayerModelTypes[modelGroup].modelAnimType;
    }

    if (player->modelAnimType < PLAYER_ANIMTYPE_3) {
        if (((player->transformation != PLAYER_FORM_FIERCE_DEITY) && (player->transformation != PLAYER_FORM_HUMAN)) ||
            (player->currentShield == PLAYER_SHIELD_NONE)) {
            player->modelAnimType = PLAYER_ANIMTYPE_DEFAULT;
        }
    }

    Player_SetModels(player, modelGroup);
}

void func_80123C58(Player* player) {
    player->itemAction = player->heldItemAction;
    Player_SetModelGroup(player, Player_ActionToModelGroup(player, player->heldItemAction));
    player->unk_AA5 = PLAYER_UNKAA5_0;
}

void Player_SetEquipmentData(PlayState* play, Player* player) {
    if (player->csAction != PLAYER_CSACTION_134) {
        player->currentShield = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
        if ((player->transformation != PLAYER_FORM_ZORA) || ((player->currentBoots != PLAYER_BOOTS_ZORA_LAND) &&
                                                             (player->currentBoots != PLAYER_BOOTS_ZORA_UNDERWATER))) {
            player->currentBoots = D_801BFF90[player->transformation];
        }
        Player_SetModelGroup(player, Player_ActionToModelGroup(player, player->heldItemAction));
        func_80123140(play, player);
        if (player->unk_B62 != 0) {
            player->unk_B62 = 1;
        }
    }
}

void Player_UpdateBottleHeld(PlayState* play, Player* player, ItemId itemId, PlayerItemAction itemAction) {
    // #region 2S2H [Dpad]
    if (IS_HELD_DPAD(player->heldItemButton)) {
        Inventory_Dpad_UpdateBottleItem(play, itemId, HELD_ITEM_TO_DPAD(player->heldItemButton));
    } else {
        Inventory_UpdateBottleItem(play, itemId, player->heldItemButton);
    }
    // #endregion

    if (itemId != ITEM_BOTTLE) {
        player->heldItemId = itemId;
        player->heldItemAction = itemAction;
    }

    player->itemAction = itemAction;
}

void Player_ReleaseLockOn(Player* player) {
    player->focusActor = NULL;
    player->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
}

/**
 * This function aims to clear Z-Target related state when it isn't in use.
 * It also handles setting a specific free fall related state that is interntwined with Z-Targeting.
 */
void Player_ClearZTargeting(Player* player) {
    if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        (player->stateFlags1 & (PLAYER_STATE1_200000 | PLAYER_STATE1_800000 | PLAYER_STATE1_8000000)) ||
        (!(player->stateFlags1 & (PLAYER_STATE1_40000 | PLAYER_STATE1_80000)) &&
         ((player->actor.world.pos.y - player->actor.floorHeight) < 100.0f))) {
        player->stateFlags1 &=
            ~(PLAYER_STATE1_Z_TARGETING | PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL |
              PLAYER_STATE1_40000 | PLAYER_STATE1_80000 | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE);
    } else if (!(player->stateFlags1 & (PLAYER_STATE1_40000 | PLAYER_STATE1_80000 | PLAYER_STATE1_200000))) {
        player->stateFlags1 |= PLAYER_STATE1_80000;
    } else if ((player->stateFlags1 & PLAYER_STATE1_40000) && (player->transformation == PLAYER_FORM_DEKU)) {
        player->stateFlags1 &= ~(PLAYER_STATE1_Z_TARGETING | PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS |
                                 PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE);
    }

    Player_ReleaseLockOn(player);
}

/**
 * Sets the "auto lock-on actor" to lock onto an actor without Player's input.
 * This function will first release any existing lock-on or (try to) release parallel.
 *
 * When using Switch Targeting, it is not possible to carry an auto lock-on actor into a normal
 * lock-on when the auto lock-on is finished.
 * This is because the `PLAYER_STATE2_LOCK_ON_WITH_SWITCH` flag is never set with an auto lock-on.
 * With Hold Targeting it is possible to keep the auto lock-on going by keeping the Z button held down.
 *
 * The auto lock-on is considered "friendly" even if the actor is actually hostile. If the auto lock-on is hostile,
 * Player's battle response will not occur (if he is actionable) and the camera behaves differently.
 * When transitioning from auto lock-on to normal lock-on (with Hold Targeting) there will be a noticeable change
 * when it switches from "friendly" mode to "hostile" mode.
 */
void Player_SetAutoLockOnActor(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);

    Player_ClearZTargeting(this);
    this->focusActor = actor;
    this->autoLockOnActor = actor;
    this->stateFlags1 |= PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS;
    Camera_SetViewParam(Play_GetCamera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, actor);
    Camera_ChangeMode(Play_GetCamera(play, CAM_ID_MAIN), CAM_MODE_FOLLOWTARGET);
}

bool func_80123F14(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->stateFlags1 & PLAYER_STATE1_800000;
}

s32 Player_SetBButtonAmmo(PlayState* play, s32 ammo) {
    play->bButtonAmmoPlusOne = ammo + 1;

    return 1;
}

bool Player_IsBurningStickInRange(PlayState* play, Vec3f* pos, f32 xzRange, f32 yRange) {
    Player* player = GET_PLAYER(play);
    Vec3f diff;
    s32 pad;

    if ((player->heldItemAction == PLAYER_IA_DEKU_STICK) && (player->unk_B28 != 0)) {
        Math_Vec3f_Diff(&player->meleeWeaponInfo[0].tip, pos, &diff);
        return (SQXZ(diff) <= SQ(xzRange)) && (0.0f <= diff.y) && (diff.y <= yRange);
    }

    return false;
}

u8 Player_GetStrength(void) {
    return sPlayerStrengths[GET_PLAYER_FORM];
}

PlayerMask Player_GetMask(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return player->currentMask;
}

// Unused
void Player_RemoveMask(PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->currentMask = PLAYER_MASK_NONE;
}

bool Player_HasMirrorShieldEquipped(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->transformation == PLAYER_FORM_HUMAN) && (player->currentShield == PLAYER_SHIELD_MIRROR_SHIELD);
}

bool Player_IsHoldingMirrorShield(PlayState* play) {
    Player* player = GET_PLAYER(play);

    return (player->transformation == PLAYER_FORM_HUMAN) && (player->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
           (player->currentShield == PLAYER_SHIELD_MIRROR_SHIELD);
}

bool Player_IsHoldingHookshot(Player* player) {
    return player->heldItemAction == PLAYER_IA_HOOKSHOT;
}

bool func_801240DC(Player* player) {
    return Player_IsHoldingHookshot(player) && (player->heldActor == NULL);
}

PlayerBButtonSword Player_BButtonSwordFromIA(Player* player, PlayerItemAction itemAction) {
    PlayerBButtonSword bButtonSword = GET_B_SWORD_FROM_IA(itemAction);

    if (player->transformation != PLAYER_FORM_GORON) {
        if ((GET_B_SWORD_FROM_IA(itemAction) > PLAYER_B_SWORD_NONE) &&
            (GET_B_SWORD_FROM_IA(itemAction) < PLAYER_B_SWORD_MAX)) {
            return bButtonSword;
        }
    }

    return PLAYER_B_SWORD_NONE;
}

PlayerBButtonSword Player_GetHeldBButtonSword(Player* player) {
    return Player_BButtonSwordFromIA(player, player->heldItemAction);
}

PlayerMeleeWeapon Player_MeleeWeaponFromIA(PlayerItemAction itemAction) {
    PlayerMeleeWeapon weapon = GET_MELEE_WEAPON_FROM_IA(itemAction);

    if ((weapon > PLAYER_MELEEWEAPON_NONE) && (weapon < PLAYER_MELEEWEAPON_MAX)) {
        return weapon;
    }

    return PLAYER_MELEEWEAPON_NONE;
}

PlayerMeleeWeapon Player_GetMeleeWeaponHeld(Player* player) {
    return Player_MeleeWeaponFromIA(player->heldItemAction);
}

s32 Player_IsHoldingTwoHandedWeapon(Player* player) {
    // Relies on the itemActions for two-handed weapons being contiguous.
    if ((player->heldItemAction >= PLAYER_IA_SWORD_TWO_HANDED) && (player->heldItemAction <= PLAYER_IA_DEKU_STICK)) {
        return true;
    }

    return false;
}

PlayerBottle Player_BottleFromIA(Player* player, PlayerItemAction itemAction) {
    PlayerBottle bottle = GET_BOTTLE_FROM_IA(itemAction);

    if ((bottle > PLAYER_BOTTLE_NONE) && (bottle < PLAYER_BOTTLE_MAX)) {
        return bottle;
    }

    return PLAYER_BOTTLE_NONE;
}

PlayerBottle Player_GetBottleHeld(Player* Player) {
    return Player_BottleFromIA(Player, Player->heldItemAction);
}

PlayerExplosive Player_ExplosiveFromIA(Player* player, PlayerItemAction itemAction) {
    PlayerExplosive explosive = GET_EXPLOSIVE_FROM_IA(itemAction);

    if ((explosive > PLAYER_EXPLOSIVE_NONE) && (explosive < PLAYER_EXPLOSIVE_MAX)) {
        return explosive;
    }

    return PLAYER_EXPLOSIVE_NONE;
}

PlayerExplosive Player_GetExplosiveHeld(Player* player) {
    return Player_ExplosiveFromIA(player, player->heldItemAction);
}

// Note this function maps PLAYER_IA_LAST_USED to PLAYER_SWORD_KOKIRI
PlayerSword Player_SwordFromIA(Player* player, PlayerItemAction itemAction) {
    PlayerSword sword = PLAYER_SWORD_KOKIRI;

    if ((itemAction == PLAYER_IA_LAST_USED) ||
        (sword = GET_SWORD_FROM_IA(itemAction), ((sword > PLAYER_SWORD_NONE) && (sword < PLAYER_SWORD_MAX)))) {
        return sword;
    }

    return PLAYER_SWORD_NONE;
}

bool func_801242B4(Player* player) {
    return (player->stateFlags1 & PLAYER_STATE1_8000000) && (player->currentBoots < PLAYER_BOOTS_ZORA_UNDERWATER);
}

s32 Player_GetEnvironmentalHazard(PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnvHazardTextTriggerEntry* triggerEntry;
    s32 envHazard;

    if (play->roomCtx.curRoom.environmentType == ROOM_ENV_HOT) {
        envHazard = PLAYER_ENV_HAZARD_HOTROOM - 1;
    } else if ((player->transformation != PLAYER_FORM_ZORA) && (player->underwaterTimer > 80)) {
        envHazard = PLAYER_ENV_HAZARD_UNDERWATER_FREE - 1;
    } else if (player->stateFlags1 & PLAYER_STATE1_8000000) {
        if ((player->transformation == PLAYER_FORM_ZORA) && (player->currentBoots >= PLAYER_BOOTS_ZORA_UNDERWATER) &&
            (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            envHazard = PLAYER_ENV_HAZARD_UNDERWATER_FLOOR - 1;
        } else {
            envHazard = PLAYER_ENV_HAZARD_SWIMMING - 1;
        }
    } else {
        return PLAYER_ENV_HAZARD_NONE;
    }

    triggerEntry = &sEnvHazardTextTriggers[envHazard];
    if (!Player_InCsMode(play)) {
        if ((triggerEntry->flag) && !(gSaveContext.envHazardTextTriggerFlags & triggerEntry->flag) &&
            (envHazard == (PLAYER_ENV_HAZARD_HOTROOM - 1))) {
            Message_StartTextbox(play, triggerEntry->textId, NULL);
            gSaveContext.envHazardTextTriggerFlags |= triggerEntry->flag;
        }
    }

    return envHazard + 1;
}

void Player_UpdateBunnyEars(Player* player) {
    Vec3s force;
    s16 angle;

    sBunnyEarKinematics.angVel.x -= sBunnyEarKinematics.angVel.x >> 3;
    sBunnyEarKinematics.angVel.y -= sBunnyEarKinematics.angVel.y >> 3;
    sBunnyEarKinematics.angVel.x += -sBunnyEarKinematics.rot.x >> 2;
    sBunnyEarKinematics.angVel.y += -sBunnyEarKinematics.rot.y >> 2;

    angle = player->actor.world.rot.y - player->actor.shape.rot.y;
    force.x = (s32)(player->actor.speed * -200.0f * Math_CosS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;
    force.y = (s32)(player->actor.speed * 100.0f * Math_SinS(angle) * (Rand_CenteredFloat(2.0f) + 10.0f)) & 0xFFFF;

    sBunnyEarKinematics.angVel.x += force.x >> 2;
    sBunnyEarKinematics.angVel.y += force.y >> 2;

    if (sBunnyEarKinematics.angVel.x > 0x1770) {
        sBunnyEarKinematics.angVel.x = 0x1770;
    } else if (sBunnyEarKinematics.angVel.x < -0x1770) {
        sBunnyEarKinematics.angVel.x = -0x1770;
    }

    if (sBunnyEarKinematics.angVel.y > 0x1770) {
        sBunnyEarKinematics.angVel.y = 0x1770;
    } else if (sBunnyEarKinematics.angVel.y < -0x1770) {
        sBunnyEarKinematics.angVel.y = -0x1770;
    }

    sBunnyEarKinematics.rot.x += sBunnyEarKinematics.angVel.x;
    sBunnyEarKinematics.rot.y += sBunnyEarKinematics.angVel.y;

    if (sBunnyEarKinematics.rot.x < 0) {
        sBunnyEarKinematics.rot.z = sBunnyEarKinematics.rot.x >> 1;
    } else {
        sBunnyEarKinematics.rot.z = 0;
    }
}

void func_80124618(struct_80124618 arg0[], f32 curFrame, Vec3f* arg2) {
    s32 currentFrame = curFrame;
    f32 temp_f0;
    f32 temp_f14;
    f32 progress;
    s16 temp_v1;

    do {
        temp_v1 = arg0[1].unk_0;
        arg0++;
    } while (temp_v1 < currentFrame);

    temp_f0 = arg0[-1].unk_0;
    progress = (curFrame - temp_f0) / (temp_v1 - temp_f0);

    temp_f14 = arg0[-1].unk_2.x;
    arg2->x = LERPIMP(temp_f14, arg0->unk_2.x, progress) * 0.01f;

    temp_f14 = arg0[-1].unk_2.y;
    arg2->y = LERPIMP(temp_f14, arg0->unk_2.y, progress) * 0.01f;

    temp_f14 = arg0[-1].unk_2.z;
    arg2->z = LERPIMP(temp_f14, arg0->unk_2.z, progress) * 0.01f;
}

struct_80124618 D_801C07C0[] = {
    { 0, { 100, 100, 100 } },  { 10, { 100, 100, 100 } }, { 11, { 100, 120, 120 } },
    { 12, { 100, 120, 120 } }, { 14, { 100, 100, 100 } }, { 19, { 100, 100, 100 } },
};

struct_80124618 D_801C07F0[] = {
    { 0, { 40, 60, 70 } },    { 3, { 40, 60, 70 } },    { 4, { 75, 90, 85 } },
    { 5, { 110, 120, 100 } }, { 7, { 100, 100, 100 } }, { 8, { 100, 100, 100 } },
};
struct_80124618 D_801C0820[] = {
    { 0, { 100, 100, 100 } },
    { 7, { 100, 100, 100 } },
    { 12, { 40, 60, 70 } },
};
struct_80124618 D_801C0838[] = {
    { 0, { 40, 60, 70 } },
    { 7, { 40, 60, 70 } },
    { 12, { 100, 100, 100 } },
};

Gfx gCullBackDList[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};
Gfx gCullFrontDList[] = {
    gsSPSetGeometryMode(G_CULL_FRONT),
    gsSPEndDisplayList(),
};

/**
 * Link's eyes and mouth textures are placed at the exact same place in all player form's respective object files.
 * This allows the array to only contain the symbols for one file and have it apply to all of them. This is a problem
 * for shiftability, and changes will need to be made in the code to account for this in a modding scenario. The symbols
 * from human Link's object are used here.
 *
 * Note that some player forms do not use the eyes and mouth textures loaded into segments 0x08 and 0x09 respectively.
 * Therefore, the segment will point at garbage data, but this does not cause issues as the data is not read from.
 */
TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX] = {
    // PLAYER_FORM_FIERCE_DEITY
    {
        NULL, // PLAYER_EYES_OPEN
        NULL, // PLAYER_EYES_HALF
        NULL, // PLAYER_EYES_CLOSED
        NULL, // PLAYER_EYES_RIGHT
        NULL, // PLAYER_EYES_LEFT
        NULL, // PLAYER_EYES_UP
        NULL, // PLAYER_EYES_DOWN
        NULL, // PLAYER_EYES_WINCING
    },
    // PLAYER_FORM_GORON
    // Note: use PLAYER_EYES_WINCING to access `gLinkGoronEyesSurprisedTex`. See `Player_DrawImpl`.
    {
        gLinkGoronEyesOpenTex,      // PLAYER_EYES_OPEN
        gLinkGoronEyesHalfTex,      // PLAYER_EYES_HALF
        gLinkGoronEyesClosedTex,    // PLAYER_EYES_CLOSED
        gLinkGoronEyesSurprisedTex, // PLAYER_EYES_RIGHT
        NULL,                       // PLAYER_EYES_LEFT
        NULL,                       // PLAYER_EYES_UP
        NULL,                       // PLAYER_EYES_DOWN
        NULL,                       // PLAYER_EYES_WINCING
    },
    // PLAYER_FORM_ZORA
    {
        gLinkZoraEyesOpenTex,    // PLAYER_EYES_OPEN
        gLinkZoraEyesHalfTex,    // PLAYER_EYES_HALF
        gLinkZoraEyesClosedTex,  // PLAYER_EYES_CLOSED
        gLinkZoraEyesRightTex,   // PLAYER_EYES_RIGHT
        gLinkZoraEyesLeftTex,    // PLAYER_EYES_LEFT
        gLinkZoraEyesUpTex,      // PLAYER_EYES_UP
        gLinkZoraEyesDownTex,    // PLAYER_EYES_DOWN
        gLinkZoraEyesWincingTex, // PLAYER_EYES_WINCING
    },
    // PLAYER_FORM_DEKU
    {
        NULL, // PLAYER_EYES_OPEN
        NULL, // PLAYER_EYES_HALF
        NULL, // PLAYER_EYES_CLOSED
        NULL, // PLAYER_EYES_RIGHT
        NULL, // PLAYER_EYES_LEFT
        NULL, // PLAYER_EYES_UP
        NULL, // PLAYER_EYES_DOWN
        NULL, // PLAYER_EYES_WINCING
    },
    // PLAYER_FORM_HUMAN
    {
        gLinkHumanEyesOpenTex,    // PLAYER_EYES_OPEN
        gLinkHumanEyesHalfTex,    // PLAYER_EYES_HALF
        gLinkHumanEyesClosedTex,  // PLAYER_EYES_CLOSED
        gLinkHumanEyesRightTex,   // PLAYER_EYES_RIGHT
        gLinkHumanEyesLeftTex,    // PLAYER_EYES_LEFT
        gLinkHumanEyesUpTex,      // PLAYER_EYES_UP
        gLinkHumanEyesDownTex,    // PLAYER_EYES_DOWN
        gLinkHumanEyesWincingTex, // PLAYER_EYES_WINCING
    },
};

TexturePtr sPlayerMouthTextures[PLAYER_FORM_MAX][PLAYER_MOUTH_MAX] = {
    // PLAYER_FORM_FIERCE_DEITY
    {
        NULL, // PLAYER_MOUTH_CLOSED
        NULL, // PLAYER_MOUTH_HALF
        NULL, // PLAYER_MOUTH_OPEN
        NULL, // PLAYER_MOUTH_SMILE
    },
    // PLAYER_FORM_GORON
    {
        NULL, // PLAYER_MOUTH_CLOSED
        NULL, // PLAYER_MOUTH_HALF
        NULL, // PLAYER_MOUTH_OPEN
        NULL, // PLAYER_MOUTH_SMILE
    },
    // PLAYER_FORM_ZORA
    {
        gLinkZoraMouthClosedTex, // PLAYER_MOUTH_CLOSED
        gLinkZoraMouthHalfTex,   // PLAYER_MOUTH_HALF
        gLinkZoraMouthOpenTex,   // PLAYER_MOUTH_OPEN
        gLinkZoraMouthSmileTex,  // PLAYER_MOUTH_SMILE
    },
    // PLAYER_FORM_DEKU
    {
        NULL, // PLAYER_MOUTH_CLOSED
        NULL, // PLAYER_MOUTH_HALF
        NULL, // PLAYER_MOUTH_OPEN
        NULL, // PLAYER_MOUTH_SMILE
    },
    // PLAYER_FORM_HUMAN
    {
        gLinkHumanMouthClosedTex, // PLAYER_MOUTH_CLOSED
        gLinkHumanMouthHalfTex,   // PLAYER_MOUTH_HALF
        gLinkHumanMouthOpenTex,   // PLAYER_MOUTH_OPEN
        gLinkHumanMouthSmileTex,  // PLAYER_MOUTH_SMILE
    },
};

PlayerFaceIndices sPlayerFaces[PLAYER_FACE_MAX] = {
    // The first 6 faces defined must be default blinking faces. See relevant code in `Player_UpdateCommon`.
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL
    { PLAYER_EYES_HALF, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_BLINKING_HALF
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_NEUTRAL_BLINKING_CLOSED

    // This duplicate set of blinking faces is defined because Player will choose between the first and second set
    // based on gameplayFrames. See relevant code in `Player_UpdateCommon`.
    // This, in theory, allows for psuedo-random variance in the faces used. But in practice, duplicate faces are used.
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_2
    { PLAYER_EYES_HALF, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_BLINKING_HALF_2
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_NEUTRAL_BLINKING_CLOSED_2

    // Additional faces. Most faces are encoded within animations.
    { PLAYER_EYES_LEFT, PLAYER_MOUTH_CLOSED },  // PLAYER_FACE_LOOK_LEFT
    { PLAYER_EYES_UP, PLAYER_MOUTH_HALF },      // PLAYER_FACE_SURPRISED
    { PLAYER_EYES_WINCING, PLAYER_MOUTH_OPEN }, // PLAYER_FACE_HURT
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_OPEN },    // PLAYER_FACE_GASP
    { PLAYER_EYES_RIGHT, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_LOOK_RIGHT
    { PLAYER_EYES_LEFT, PLAYER_MOUTH_CLOSED },  // PLAYER_FACE_LOOK_LEFT_2
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_OPEN },  // PLAYER_FACE_EYES_CLOSED_MOUTH_OPEN
    { PLAYER_EYES_HALF, PLAYER_MOUTH_HALF },    // PLAYER_FACE_OPENING
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_OPEN },    // PLAYER_FACE_EYES_AND_MOUTH_OPEN
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_SMILE },   // PLAYER_FACE_SMILE
};

// Note the correct pointer to pass as the jointTable is the jointTable pointer from the SkelAnime struct, not the
// buffer from the Player struct itself since that one may be misaligned.
void Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod,
                     PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw,
                     PostLimbDrawFlex postLimbDraw, Actor* actor) {
    s32 eyeIndex = GET_EYE_INDEX_FROM_JOINT_TABLE(jointTable);
    s32 mouthIndex = GET_MOUTH_INDEX_FROM_JOINT_TABLE(jointTable);
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;

    // 2S2H [Port] Hess crash fix
    if (eyeIndex >= PLAYER_EYES_MAX) {
        Ship_HandleConsoleCrashAsReset();
        eyeIndex = 0;
    }

    // If the eyes index provided by the animation is negative, use the value provided by the `face` argument instead
    if (eyeIndex < 0) {
        eyeIndex = sPlayerFaces[face].eyeIndex;
    }

    if (playerForm == PLAYER_FORM_GORON) {
        // Goron does not have the eye textures to look in different directions
        if ((eyeIndex >= PLAYER_EYES_RIGHT) && (eyeIndex <= PLAYER_EYES_DOWN)) {
            eyeIndex = PLAYER_EYES_OPEN;
        } else if (eyeIndex == PLAYER_EYES_WINCING) {
            // Goron form puts a surpised expression where the eyes-right normally goes
            eyeIndex = PLAYER_EYES_RIGHT;
        }
    }

    // Only Human, Zora, and Goron will read the eye textures in the head limb display list.
    // Fierce Deity and Deku will point this segment to garbage data, but it will be unread from.
    gSPSegment(&gfx[0], 0x08, Lib_SegmentedToVirtual(sPlayerEyesTextures[playerForm][eyeIndex]));

    // 2S2H [Port] Hess crash fix
    if (mouthIndex >= PLAYER_MOUTH_MAX) {
        Ship_HandleConsoleCrashAsReset();
        mouthIndex = 0;
    }

    // If the mouth index provided by the animation is negative, use the value provided by the `face` argument instead
    if (mouthIndex < 0) {
        mouthIndex = sPlayerFaces[face].mouthIndex;
    }

    // Only Human and Zora will read the mouth textures in the head limb display list.
    // Goron, Fierce Deity, and Deku will point this segment to garbage data, but it will be unread from.
    gSPSegment(&gfx[1], 0x09, Lib_SegmentedToVirtual(sPlayerMouthTextures[playerForm][mouthIndex]));

    POLY_OPA_DISP = &gfx[2];

    D_801F59E0 = playerForm * 2;
    // 2S2H [Port] Disable LOD when a custom player model is used
    if (Player_IsCustomLinkModel((Player*)actor)) {
        lod = 0;
    }
    sPlayerLod = lod;
    SkelAnime_DrawFlexLod(play, skeleton, jointTable, dListCount, overrideLimbDraw, postLimbDraw, actor, lod);

    CLOSE_DISPS(play->state.gfxCtx);
}

Vec3f D_801C08C0[PLAYER_FORM_MAX] = {
    { 1304.0f, 0.0f, 0.0f }, // PLAYER_FORM_FIERCE_DEITY
    { 1156.0f, 0.0f, 0.0f }, // PLAYER_FORM_GORON
    { 1406.0f, 0.0f, 0.0f }, // PLAYER_FORM_ZORA
    { 408.0f, 0.0f, 0.0f },  // PLAYER_FORM_DEKU
    { 695.0f, 0.0f, 0.0f },  // PLAYER_FORM_HUMAN
};

f32 D_801C08FC[PLAYER_FORM_MAX] = {
    1265.0f, // PLAYER_FORM_FIERCE_DEITY
    1056.0f, // PLAYER_FORM_GORON
    1506.0f, // PLAYER_FORM_ZORA
    359.0f,  // PLAYER_FORM_DEKU
    826.0f,  // PLAYER_FORM_HUMAN
};
f32 D_801C0910[PLAYER_FORM_MAX] = {
    170.0416f,  // PLAYER_FORM_FIERCE_DEITY
    133.63359f, // PLAYER_FORM_GORON
    197.68358f, // PLAYER_FORM_ZORA
    16.646399f, // PLAYER_FORM_DEKU
    48.302498f, // PLAYER_FORM_HUMAN
};
f32 D_801C0924[PLAYER_FORM_MAX] = {
    10.019104f,  // PLAYER_FORM_FIERCE_DEITY
    22.120003f,  // PLAYER_FORM_GORON
    -29.12001f,  // PLAYER_FORM_ZORA
    3.7582989f,  // PLAYER_FORM_DEKU
    -19.925102f, // PLAYER_FORM_HUMAN
};
f32 D_801C0938[PLAYER_FORM_MAX] = {
    5.0f, // PLAYER_FORM_FIERCE_DEITY
    4.0f, // PLAYER_FORM_GORON
    1.0f, // PLAYER_FORM_ZORA
    1.0f, // PLAYER_FORM_DEKU
    3.0f, // PLAYER_FORM_HUMAN
};

/**
 * Adjusts player's legs to slopes and spawns flames when running on lava
 */
void Player_AdjustSingleLeg(PlayState* play, Player* player, SkelAnime* skelAnime, Vec3f* pos, Vec3s* rot,
                            s32 thighLimbIndex, s32 shinLimbIndex, s32 footLimbIndex) {
    CollisionPoly* poly;
    s32 bgId;
    f32 yIntersect;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f footprintPos;
    f32 sp74;
    f32 sp70;
    f32 sp7C;

    if ((player->stateFlags3 & PLAYER_STATE3_1) || !(player->actor.scale.y >= 0.0f) ||
        (player->stateFlags1 & PLAYER_STATE1_DEAD) || play->soaringCsOrSoTCsPlaying) {
        return;
    }

    sp74 = D_801C0910[player->transformation];
    sp70 = D_801C0924[player->transformation];
    sp7C = D_801C0938[player->transformation] - player->unk_AB8;
    Matrix_Push();
    Matrix_TranslateRotateZYX(pos, rot);
    Matrix_MultZero(&sp90);
    Matrix_TranslateRotateZYX(&D_801C08C0[player->transformation], &skelAnime->jointTable[shinLimbIndex]);
    Matrix_Translate(D_801C08FC[player->transformation], 0.0f, 0.0f, MTXMODE_APPLY);
    Matrix_MultZero(&sp84);
    Matrix_MultVecY(-300.0f, &footprintPos);
    Matrix_Pop();

    footprintPos.y += 15.0f;
    yIntersect = BgCheck_EntityRaycastFloor3(&play->colCtx, &poly, &bgId, &footprintPos) + sp7C +
                 (player->unk_ABC * player->actor.scale.y);
    if (sp84.y < yIntersect) {
        f32 diffX = sp84.x - sp90.x;
        f32 diffY = sp84.y - sp90.y;
        f32 diffZ = sp84.z - sp90.z;
        f32 distance;
        f32 sp58;
        f32 sp54;
        f32 temp_f20;
        f32 sp4C;
        f32 sp48;
        s16 temp_f8;
        s32 temp_v0_4;
        s16 phi_t1;

        distance = sqrtf(SQ(diffX) + SQ(diffY) + SQ(diffZ));

        sp58 = (SQ(distance) + sp70) / (2.0f * distance);

        temp_f20 = sp74 - SQ(sp58);
        temp_f20 = (temp_f20 < 0.0f) ? 0.0f : sqrtf(temp_f20);

        sp4C = Math_FAtan2F(temp_f20, sp58);
        distance = sqrtf(SQ(diffX) + SQ(yIntersect - sp90.y) + SQ(diffZ));
        sp58 = (SQ(distance) + sp70) / (2.0f * distance);
        sp54 = distance - sp58;

        temp_f20 = sp74 - SQ(sp58);
        temp_f20 = (temp_f20 < 0.0f) ? 0.0f : sqrtf(temp_f20);

        sp48 = Math_FAtan2F(temp_f20, sp58);
        phi_t1 = (M_PIf - (Math_FAtan2F(sp54, temp_f20) + ((M_PIf / 2.0f) - sp48))) * (0x8000 / M_PIf);
        phi_t1 = -skelAnime->jointTable[shinLimbIndex].z + phi_t1;
        temp_f8 = (sp48 - sp4C) * (0x8000 / M_PIf);

        if ((s16)(ABS_ALT(skelAnime->jointTable[shinLimbIndex].x) + ABS_ALT(skelAnime->jointTable[shinLimbIndex].y)) <
            0) {
            phi_t1 = BINANG_ROT180(phi_t1);
        }

        rot->z -= temp_f8;

        skelAnime->jointTable[thighLimbIndex].z -= temp_f8;
        skelAnime->jointTable[shinLimbIndex].z += phi_t1;
        skelAnime->jointTable[footLimbIndex].z = (skelAnime->jointTable[footLimbIndex].z + temp_f8) - phi_t1;

        temp_v0_4 = SurfaceType_GetFloorType(&play->colCtx, poly, bgId);
        if ((temp_v0_4 >= 2) && (temp_v0_4 < 4) && !SurfaceType_IsWallDamage(&play->colCtx, poly, bgId)) {
            footprintPos.y = yIntersect;
            EffectSsGFire_Spawn(play, &footprintPos);
        }
    }
}

void Player_DrawHookshotReticle(PlayState* play, Player* player, f32 hookshotDistance) {
    static Vec3f D_801C094C = { -500.0f, -100.0f, 0.0f };
    CollisionPoly* poly;
    s32 bgId;
    Vec3f sp7C;
    Vec3f sp70;
    Vec3f pos;

    D_801C094C.z = 0.0f;
    Matrix_MultVec3f(&D_801C094C, &sp7C);
    D_801C094C.z = hookshotDistance;
    Matrix_MultVec3f(&D_801C094C, &sp70);

    if (BgCheck_AnyLineTest3(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId)) {
        if (!func_800B90AC(play, &player->actor, poly, bgId, &pos) ||
            BgCheck_ProjectileLineTest(&play->colCtx, &sp7C, &sp70, &pos, &poly, true, true, true, true, &bgId)) {
            Vec3f sp58;
            f32 sp54;
            f32 scale;

            OPEN_DISPS(play->state.gfxCtx);

            OVERLAY_DISP = Gfx_SetupDL(OVERLAY_DISP, SETUPDL_7);

            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &sp58, &sp54);

            scale = (sp54 < 200.0f) ? 0.08f : (sp54 / 200.0f) * 0.08f;

            Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
            Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx);

            gSPSegment(OVERLAY_DISP++, 0x06, play->objectCtx.slots[player->actor.objectSlot].segment);
            gSPDisplayList(OVERLAY_DISP++, gHookshotReticleDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

s32 D_801C0958 = false;

Gfx** D_801C095C[] = {
    gPlayerLeftHandClosedDLs,
    gPlayerLeftHandOpenDLs,
};
Gfx** D_801C0964[] = {
    gPlayerRightHandClosedDLs,
    gPlayerRightHandOpenDLs,
};

void func_80124F18(s16* arg0, f32* arg1, s16 arg2, f32 arg3, f32 arg4) {
    if (*arg0 < arg2) {
        *arg1 += arg3;
    } else {
        *arg1 -= arg3;
    }

    *arg1 = CLAMP(*arg1, -arg4, arg4);
    *arg0 += TRUNCF_BINANG(*arg1);
    if (((arg2 - *arg0) * TRUNCF_BINANG(*arg1)) < 0) {
        *arg0 = arg2;
    }
}

void func_80124FF0(f32 arg0, s16 arg1, Vec3f* arg2, s16 arg3, Vec3f* arg4, Vec3f* arg5, s16* arg6, f32* arg7, f32 arg8,
                   s16 arg9, s16* arg10, f32* arg11, s32 arg12) {
    Vec3f sp44;
    f32 sp40;
    s16 temp_v0;
    s16 sp3C;
    f32 temp_f14;
    f32 sp34 = Math_CosS(arg1) * arg0;
    f32 sp30 = Math_SinS(arg1) * -arg0;

    arg5->x = (Math_SinS(arg3) * sp30) + arg2->x;
    arg5->y = arg2->y + sp34;
    arg5->z = (Math_CosS(arg3) * sp30) + arg2->z;

    Math_Vec3f_Diff(arg5, arg4, &sp44);
    sp40 = sqrtf(SQXZ(sp44));

    sp3C = (sp40 <= 1.0f) ? arg3 : Math_Atan2S_XY(sp44.z, sp44.x);
    sp40 = (Math_CosS(sp3C - arg3) * sp40) + arg8;

    if (ABS_ALT(BINANG_SUB(sp3C, arg3)) > 0x4000) {
        sp3C = BINANG_ROT180(sp3C);
    }
    sp3C -= arg3;

    temp_v0 = Math_Atan2S_XY(sp44.y, sp40);
    temp_v0 = CLAMP(temp_v0, (s16)-arg9, arg9);
    //! FAKE:
    if (sp3C) {}

    func_80124F18(arg6, arg7, temp_v0, 20.0f, 2000.0f);
}

void func_801251C4(Player* player, Vec3f* arg1) {
    Vec3f sp4C;
    Vec3f sp40;

    sp4C.x = player->actor.world.pos.x;
    sp4C.y = player->actor.world.pos.y + 60.0f;
    sp4C.z = player->actor.world.pos.z;
    func_80124FF0(-20.0f, player->unk_B8C, &sp4C, player->actor.shape.rot.y,
                  &player->bodyPartsPos[PLAYER_BODYPART_WAIST], arg1, &player->unk_B90, &player->unk_B10[0], 0.0f,
                  0x1F40, &player->unk_B92, &player->unk_B10[1], 0);

    sp40.x =
        (player->bodyPartsPos[PLAYER_BODYPART_LEFT_FOOT].x + player->bodyPartsPos[PLAYER_BODYPART_RIGHT_FOOT].x) / 2.0f;
    sp40.y =
        (player->bodyPartsPos[PLAYER_BODYPART_LEFT_FOOT].y + player->bodyPartsPos[PLAYER_BODYPART_RIGHT_FOOT].y) / 2.0f;
    sp40.z =
        (player->bodyPartsPos[PLAYER_BODYPART_LEFT_FOOT].z + player->bodyPartsPos[PLAYER_BODYPART_RIGHT_FOOT].z) / 2.0f;
    func_80124FF0(-20.0f, player->unk_B90, arg1, (player->actor.shape.rot.y + player->unk_B92), &sp40, arg1,
                  &player->unk_B94, &player->unk_B10[2], -1.9f, 0x1F40, &player->unk_B96, &player->unk_B10[3], 0);
}

void func_80125318(Vec3f* arg0, Vec3s* arg1) {
    arg0->x = 0.0f;
    arg0->y = 0.0f;
    arg0->z = 0.0f;
    arg1->x = 0;
    arg1->y = 0;
    arg1->z = 0;
}

void func_80125340(void) {
    Matrix_Push();
    D_801C0958 = true;
}

void func_8012536C(void) {
    if (D_801C0958) {
        Matrix_Pop();
        D_801C0958 = false;
    }
}

void Player_DrawZoraShield(PlayState* play, Player* player) {
    u8* phi_a0;
    Vtx* vtx;
    Gfx* dList;
    f32 scale = player->unk_B62 * (10.0f / 51.0f);
    s32 i;

    // 2S2H [Port] Graph allocated buffer to be used with ResourceMgr_LoadArrayByNameAsU8 to avoid
    // needing to use malloc/free with Scalar based Array resources
    u8* alphaU8Buffer = GRAPH_ALLOC(play->state.gfxCtx, 80 * sizeof(u8));

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(&object_link_zora_Matanimheader_012A80));
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    // clang-format off
    vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(&object_link_zora_Vtx_011210)); phi_a0 = ResourceMgr_LoadArrayByNameAsU8(Lib_SegmentedToVirtual(&object_link_zora_U8_011710), alphaU8Buffer);
    // clang-format on

    // ARRAY_COUNT(object_link_zora_Vtx_011210)
    for (i = 0; i < 80; i++) {
        // Editing the Vtxs in object itself
        vtx->v.cn[3] = (*phi_a0 * player->unk_B62) >> 8;
        vtx++;
        phi_a0++;
    }

    dList = POLY_XLU_DISP;

    MATRIX_FINALIZE_AND_LOAD(&dList[0], play->state.gfxCtx);
    gSPDisplayList(&dList[1], object_link_zora_DL_011760);

    POLY_XLU_DISP = &dList[2];

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80125500(PlayState* play, Player* player, s32 limbIndex, Vec3f* pos, Vec3s* rot) {
    if (limbIndex == PLAYER_LIMB_LEFT_THIGH) {
        Player_AdjustSingleLeg(play, player, &player->skelAnime, pos, rot, PLAYER_LIMB_LEFT_THIGH,
                               PLAYER_LIMB_LEFT_SHIN, PLAYER_LIMB_LEFT_FOOT);
    } else if (limbIndex == PLAYER_LIMB_RIGHT_THIGH) {
        Player_AdjustSingleLeg(play, player, &player->skelAnime, pos, rot, PLAYER_LIMB_RIGHT_THIGH,
                               PLAYER_LIMB_RIGHT_SHIN, PLAYER_LIMB_RIGHT_FOOT);
    }
}

s32 Player_OverrideLimbDrawGameplayCommon(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                          Actor* thisx) {
    s32 pad;
    Player* player = (Player*)thisx;

    if (limbIndex == PLAYER_LIMB_ROOT) {
        sPlayerLeftHandType = player->leftHandType;
        sPlayerRightHandType = player->rightHandType;

        // `sPlayerCurBodyPartPos` is incremented before each usage, so it starts at index -1.
        // What determines if a limb corresponds to a body part is whether or not it has a non-NULL `dList`.
        // Note: The increment would not be done for the root limb, even if it had a non-NULL `dList`.
        //       So if the root limb had a non-NULL `dList` (which is not the case in vanilla),
        //       an out-of-bounds write to `bodyPartsPos` would occur.
        sPlayerCurBodyPartPos = &player->bodyPartsPos[0] - 1;

        if (player->transformation != PLAYER_FORM_FIERCE_DEITY) {
            if (!(player->skelAnime.movementFlags & ANIM_FLAG_4) || (player->skelAnime.movementFlags & ANIM_FLAG_1)) {
                pos->x *= player->ageProperties->unk_08;
                pos->z *= player->ageProperties->unk_08;
            }
            if (!(player->skelAnime.movementFlags & ANIM_FLAG_4) ||
                (player->skelAnime.movementFlags & ANIM_FLAG_UPDATE_Y)) {
                pos->y *= player->ageProperties->unk_08;
            }
        }

        pos->y -= player->unk_AB8;
        if ((player->transformation == PLAYER_FORM_ZORA) && (player->stateFlags3 & PLAYER_STATE3_8000)) {
            Matrix_Translate(pos->x, ((Math_CosS(player->unk_AAA) - 1.0f) * 200.0f) + pos->y, pos->z, MTXMODE_APPLY);
            Matrix_RotateXS(player->unk_AAA, MTXMODE_APPLY);

            if (player->unk_B62 != 0) {
                Matrix_Push();
                Matrix_RotateZS(player->unk_B8E, MTXMODE_APPLY);
                Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
                Matrix_RotateXS(-0x8000, MTXMODE_APPLY);
                Matrix_Translate(0.0f, 0.0f, -4000.0f, MTXMODE_APPLY);
                Player_DrawZoraShield(play, player);
                Matrix_Pop();
            }

            Matrix_RotateZS(player->unk_B86[1], MTXMODE_APPLY);
            Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
            func_80125318(pos, rot);
        } else if (player->stateFlags3 & PLAYER_STATE3_2000) {
            Vec3f sp54;

            func_801251C4(player, &sp54);
            sp54.x -= player->actor.world.pos.x;
            sp54.y -= player->actor.world.pos.y;
            sp54.z -= player->actor.world.pos.z;
            Matrix_Translate(pos->x + sp54.x, pos->y + sp54.y, pos->z + sp54.z, MTXMODE_APPLY);
            Matrix_RotateXS(player->unk_B94, MTXMODE_APPLY);
            Matrix_RotateZS(player->unk_B8E, MTXMODE_APPLY);
            player->upperLimbRot.x = player->unk_B90 - player->unk_B94;
            Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
            func_80125318(pos, rot);
        } else if (player->unk_AAA != 0) {
            Matrix_Translate(pos->x, ((Math_CosS(player->unk_AAA) - 1.0f) * 200.0f) + pos->y, pos->z, MTXMODE_APPLY);
            Matrix_RotateXS(player->unk_AAA, MTXMODE_APPLY);
            Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
            func_80125318(pos, rot);
        }
    } else {
        if (*dList != NULL) {
            sPlayerCurBodyPartPos++;
        }

        if (limbIndex == PLAYER_LIMB_HEAD) {
            CosmeticEditor_ApplySillyLimbScale(CVAR_COSMETIC("Silly.LinkHeadScale"), pos);

            rot->x += player->headLimbRot.z;
            rot->y -= player->headLimbRot.y;
            rot->z += player->headLimbRot.x;

            if (player->transformation == PLAYER_FORM_DEKU) {
                EnArrow* bubble = NULL;

                if (((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_drinkend))) ||
                    (BEN_ANIM_EQUAL(player->skelAnimeUpper.animation, gPlayerAnim_pn_tamahaki)) ||
                    ((player->stateFlags3 & PLAYER_STATE3_40) && ((bubble = (EnArrow*)player->heldActor) != NULL))) {
                    Matrix_TranslateRotateZYX(pos, rot);
                    func_80125340();
                    func_80125318(pos, rot);

                    if (GameInteractor_Should(VB_DEKU_COMMON_HEAD_OVERRIDE_HELD_ACTOR, (bubble != NULL), bubble)) {
                        s32 requiredScopeTemp[2];

                        player->unk_AF0[0].x = 1.0f - (bubble->bubble.unk_144 * 0.03f);
                        player->unk_AF0[0].y = 1.0f - (bubble->bubble.unk_144 * 0.01f);
                        player->unk_AF0[0].z = 1.0f - (bubble->bubble.unk_144 * 0.04f);
                        rot->z = bubble->bubble.unk_144 * 320.0f;
                    } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_drinkend)) {
                        func_80124618(D_801C03E0, player->skelAnime.curFrame, player->unk_AF0);
                    } else {
                        func_80124618(D_801C03C0, player->skelAnimeUpper.curFrame, player->unk_AF0);
                    }

                    Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);
                }
            }
        } else if (limbIndex == PLAYER_LIMB_TORSO) {
            if ((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_gakkistart)) ||
                (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_wait)) ||
                (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchC)) ||
                (player->transformation == PLAYER_FORM_ZORA)) {
                Matrix_TranslateRotateZYX(pos, rot);
                if (player->transformation == PLAYER_FORM_GORON) {
                    func_80125340();
                }

                func_80125318(pos, rot);
                if ((player->transformation != PLAYER_FORM_ZORA) ||
                    (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_wait))) {
                    struct_80124618* phi_a0 =
                        (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_wait))
                            ? D_801C0608
                            : ((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_wait))
                                   ? D_801C0628
                                   : ((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchC))
                                          ? D_801C07C0
                                          : D_801C0460));

                    func_80124618(phi_a0, player->skelAnime.curFrame, player->unk_AF0);
                } else {
                    player->unk_AF0[0].y = 1.0f;
                }

                if (player->transformation == PLAYER_FORM_ZORA) {
                    player->unk_AF0[0].y *= 1.0f + (0.29999995f * player->unk_B10[0]);
                    player->unk_AF0[0].x = 1.0f;
                    player->unk_AF0[0].z = 1.0f;
                }
                Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);
            }
        } else if (limbIndex == PLAYER_LIMB_UPPER_ROOT) {
            s16 rotX;
            s16 rotZ = 0x44C;

            if (player->upperLimbYawSecondary != 0) {
                Matrix_RotateZS(rotZ, MTXMODE_APPLY);
                Matrix_RotateYS(player->upperLimbYawSecondary, MTXMODE_APPLY);
            }

            if (player->upperLimbRot.y != 0) {
                Matrix_RotateYS(player->upperLimbRot.y, MTXMODE_APPLY);
            }

            rotX = player->upperLimbRot.x;
            if ((player->transformation == PLAYER_FORM_DEKU) && (player->stateFlags3 & PLAYER_STATE3_40)) {
                if (GameInteractor_Should(VB_DEKU_COMMON_UPPER_LIMB_OVERRIDE_HELD_ACTOR, (player->heldActor != NULL),
                                          player->heldActor)) {
                    rotX += TRUNCF_BINANG(((EnArrow*)(player->heldActor))->bubble.unk_144 * -470.0f);
                }
            }

            Matrix_RotateXS(rotX, MTXMODE_APPLY);
            if (player->upperLimbRot.z != 0) {
                Matrix_RotateZS(player->upperLimbRot.z, MTXMODE_APPLY);
            }
        } else {
            func_80125500(play, player, limbIndex, pos, rot);
        }
    }

    return false;
}

void func_80125CE0(Player* player, struct_80124618* arg1, Vec3f* pos, Vec3s* rot) {
    Matrix_TranslateRotateZYX(pos, rot);
    func_80125318(pos, rot);
    func_80124618(arg1, player->skelAnime.curFrame, player->unk_AF0);
    Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);
}

s32 Player_OverrideLimbDrawGameplayDefault(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           Actor* actor) {
    Player* player = (Player*)actor;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, &player->actor)) {
        if (limbIndex == PLAYER_LIMB_LEFT_HAND) {
            Gfx** leftHandDLists = player->leftHandDLists;
            EquipValueSword swordEquipValue;

            if (player->stateFlags3 & PLAYER_STATE3_2000) {
                rot->z -= player->unk_B8C;
            } else if ((sPlayerLeftHandType == PLAYER_MODELTYPE_LH_4) &&
                       (player->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN)) {
                leftHandDLists = &gPlayerLeftHandOpenDLs[D_801F59E0];
                sPlayerLeftHandType = PLAYER_MODELTYPE_LH_OPEN;
            } else if ((player->leftHandType == PLAYER_MODELTYPE_LH_OPEN) && (player->actor.speed > 2.0f) &&
                       !(player->stateFlags1 & PLAYER_STATE1_8000000)) {
                leftHandDLists = &gPlayerLeftHandClosedDLs[D_801F59E0];
                sPlayerLeftHandType = PLAYER_MODELTYPE_LH_CLOSED;
            } else if ((player->leftHandType == PLAYER_MODELTYPE_LH_ONE_HAND_SWORD) &&
                       (player->transformation == PLAYER_FORM_HUMAN) &&
                       ((swordEquipValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD),
                         swordEquipValue != EQUIP_VALUE_SWORD_NONE))) {
                leftHandDLists = &D_801C018C[2 * ((swordEquipValue - 1) ^ 0)];
            } else {
                s32 handIndex = GET_LEFT_HAND_INDEX_FROM_JOINT_TABLE(player->skelAnime.jointTable);

                if (handIndex != 0) {
                    handIndex = (handIndex >> 12) - 1;
                    if (handIndex >= 2) {
                        handIndex = 0;
                    }
                    leftHandDLists = &D_801C095C[handIndex][D_801F59E0];
                }
            }

            *dList = leftHandDLists[sPlayerLod];
            CosmeticEditor_SplitHeldItem(dList, CVAR_COSMETIC("Silly.LinkSwordScale"));

            if (player->transformation == PLAYER_FORM_GORON) {
                if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchA)) {
                    func_80125CE0(player, D_801C0750, pos, rot);
                }
            } else if (player->transformation == PLAYER_FORM_ZORA) {
                if ((player->stateFlags1 & PLAYER_STATE1_2) || (player->stateFlags1 & PLAYER_STATE1_400) ||
                    func_801242B4(player)) {
                    *dList = gLinkZoraLeftHandOpenDL;
                } else {
                    s32 phi_a1 = (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_gakkistart)) &&
                                 (player->skelAnime.curFrame >= 6.0f);

                    if (phi_a1 || (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_gakkiplay))) {
                        *dList = object_link_zora_DL_00E2A0;
                        func_80125CE0(player, phi_a1 ? D_801C0538 : D_801C0560, pos, rot);
                    }
                }
            }
        } else if (limbIndex == PLAYER_LIMB_RIGHT_HAND) {
            if ((player->transformation == PLAYER_FORM_ZORA) &&
                (((player->stateFlags1 & PLAYER_STATE1_2)) || (player->stateFlags1 & PLAYER_STATE1_400) ||
                 func_801242B4(player))) {
                *dList = gLinkZoraRightHandOpenDL;
            } else {
                Gfx** rightHandDLists = player->rightHandDLists;

                if (player->stateFlags3 & PLAYER_STATE3_2000) {
                    rot->z -= player->unk_B8C;
                }

                if (sPlayerRightHandType == PLAYER_MODELTYPE_RH_SHIELD) {
                    if (player->transformation == PLAYER_FORM_HUMAN) {
                        if (player->currentShield != PLAYER_SHIELD_NONE) {
                            //! FAKE:
                            rightHandDLists = &gPlayerHandHoldingShields[2 * ((player->currentShield - 1) ^ 0)];
                        }
                    }
                } else if ((player->rightHandType == PLAYER_MODELTYPE_RH_OPEN) && (player->actor.speed > 2.0f) &&
                           !(player->stateFlags1 & PLAYER_STATE1_8000000)) {
                    rightHandDLists = &gPlayerRightHandClosedDLs[D_801F59E0];
                    sPlayerRightHandType = PLAYER_MODELTYPE_RH_CLOSED;
                } else {
                    s32 handIndex = GET_RIGHT_HAND_INDEX_FROM_JOINT_TABLE(player->skelAnime.jointTable);

                    if (handIndex != 0) {
                        handIndex = (handIndex >> 8) - 1;
                        // 2S2H [Port] Hess crash fix
                        if (handIndex >= 2) {
                            Ship_HandleConsoleCrashAsReset();
                            handIndex = 0;
                        }
                        rightHandDLists = &D_801C0964[handIndex][D_801F59E0];
                    }
                }

                *dList = rightHandDLists[sPlayerLod];
                CosmeticEditor_SplitHeldItem(dList, CVAR_COSMETIC("Silly.LinkShieldScale"));

                if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchB)) {
                    func_80125CE0(player, D_801C0784, pos, rot);
                }
            }
        } else if (limbIndex == PLAYER_LIMB_SHEATH) {
            Gfx** sheathDLists = player->sheathDLists;

            if (player->transformation == PLAYER_FORM_HUMAN) {
                EquipValueSword swordEquipValue = GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);

                if (swordEquipValue != EQUIP_VALUE_SWORD_NONE) {
                    if ((player->sheathType == PLAYER_MODELTYPE_SHEATH_14) ||
                        (player->sheathType == PLAYER_MODELTYPE_SHEATH_12)) {
                        sheathDLists = &gPlayerSheathedSwords[2 * ((swordEquipValue - 1) ^ 0)];
                    } else {
                        sheathDLists = &gPlayerSwordSheaths[2 * ((swordEquipValue - 1) ^ 0)];
                    }
                }
            }

            *dList = sheathDLists[sPlayerLod];
        } else if (limbIndex == PLAYER_LIMB_WAIST) {
            if (!Player_IsCustomLinkModel(player)) {
                *dList = player->waistDLists[sPlayerLod];
            }
        } else if (limbIndex == PLAYER_LIMB_HAT) {
            if (player->transformation == PLAYER_FORM_ZORA) {
                Matrix_Scale((player->unk_B10[0] * 1) + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
            }
        }
    }

    return false;
}

s32 Player_OverrideLimbDrawGameplayFirstPerson(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                               Actor* actor) {
    Player* player = (Player*)actor;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, actor)) {
        if (player->unk_AA5 != PLAYER_UNKAA5_3) {
            *dList = NULL;
        } else if (limbIndex == PLAYER_LIMB_LEFT_FOREARM) {
            *dList = sPlayerFirstPersonLeftForearmDLs[player->transformation];
        } else if (limbIndex == PLAYER_LIMB_LEFT_HAND) {
            *dList = sPlayerFirstPersonLeftHandDLs[player->transformation];
        } else if (limbIndex == PLAYER_LIMB_RIGHT_SHOULDER) {
            *dList = sPlayerFirstPersonRightShoulderDLs[player->transformation];
        } else if (limbIndex == PLAYER_LIMB_RIGHT_HAND) {
            if (Player_IsHoldingHookshot(player)) {
                *dList = sPlayerFirstPersonRightHandHookshotDLs[player->transformation];
            } else {
                *dList = sPlayerFirstPersonRightHandDLs[player->transformation];
            }
        } else {
            *dList = NULL;
        }
    }

    return false;
}

// Unused remnant of OoT
s32 Player_OverrideLimbDrawGameplayCrawling(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                            Actor* thisx) {
    Player* player = (Player*)thisx;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, thisx)) {
        *dList = NULL;
    }

    return false;
}

s32 func_80126440(PlayState* play, ColliderQuad* collider, WeaponInfo* weaponInfo, Vec3f* newTip, Vec3f* newBase) {
    if (!weaponInfo->active) {
        if (collider != NULL) {
            Collider_ResetQuadAT(play, &collider->base);
        }
        Math_Vec3f_Copy(&weaponInfo->tip, newTip);
        Math_Vec3f_Copy(&weaponInfo->base, newBase);
        weaponInfo->active = true;

        return true;
    }

    if ((weaponInfo->tip.x == newTip->x) && (weaponInfo->tip.y == newTip->y) && (weaponInfo->tip.z == newTip->z)) {
        if ((weaponInfo->base.x == newBase->x) && (weaponInfo->base.y == newBase->y) &&
            (weaponInfo->base.z == newBase->z)) {
            if (collider != NULL) {
                Collider_ResetQuadAT(play, &collider->base);
            }

            return false;
        }
    }

    if (collider != NULL) {
        Collider_SetQuadVertices(collider, newBase, newTip, &weaponInfo->base, &weaponInfo->tip);
        CollisionCheck_SetAT(play, &play->colChkCtx, &collider->base);
    }

    Math_Vec3f_Copy(&weaponInfo->base, newBase);
    Math_Vec3f_Copy(&weaponInfo->tip, newTip);
    weaponInfo->active = true;

    return true;
}

u8 sPlayerShieldColMaterials[PLAYER_SHIELD_MAX] = {
    COL_MATERIAL_METAL, // PLAYER_SHIELD_NONE
    COL_MATERIAL_METAL, // PLAYER_SHIELD_HEROS_SHIELD
    COL_MATERIAL_METAL, // PLAYER_SHIELD_MIRROR_SHIELD
};

void Player_UpdateShieldCollider(PlayState* play, Player* player, ColliderQuad* collider, Vec3f quadSrc[4]) {
    if (player->stateFlags1 & PLAYER_STATE1_400000) {
        Vec3f quadDest[4];

        player->shieldQuad.base.colMaterial = sPlayerShieldColMaterials[player->currentShield];
        Matrix_MultVec3f(&quadSrc[0], &quadDest[0]);
        Matrix_MultVec3f(&quadSrc[1], &quadDest[1]);
        Matrix_MultVec3f(&quadSrc[2], &quadDest[2]);
        Matrix_MultVec3f(&quadSrc[3], &quadDest[3]);
        Collider_SetQuadVertices(collider, &quadDest[0], &quadDest[1], &quadDest[2], &quadDest[3]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &collider->base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &collider->base);
    }
}

Vec3f D_801C0970[] = {
    { 0.0f, 400.0f, 0.0f },
    { 0.0f, 1400.0f, -1000.0f },
    { 0.0f, -400.0f, 1000.0f },
};

Vec3f D_801C0994[] = {
    { 5000.0f, 400.0f, 0.0f },
    { 5000.0f, -400.0f, 1000.0f },
    { 5000.0f, 1400.0f, -1000.0f },
};

Vec3f D_801C09B8[] = {
    { 0.0f, 750.0f, 750.0f },
    { 1500.0f, 1500.0f, 1500.0f },
    { -2500.0f, -2000.0f, -3000.0f },
};

Vec3f D_801C09DC[] = {
    { 900.0f, 300.0f, 100.0f },
    { 1300.0f, 700.0f, -300.0f },
    { 500.0f, -100.0f, 500.0f },
};

Vec3f D_801C0A00[] = {
    { -2500.0f, 1400.0f, 1100.0f },
    { -2900.0f, 1000.0f, 1500.0f },
    { -2100.0f, 1800.0f, 700.0f },
};

Vec3f D_801C0A24[] = {
    { 0.0f, 0.0f, 0.0f },
    { -800.0f, 800.0f, 800.0f },
    { -800.0f, -800.0f, -800.0f },
};

Vec3f D_801C0A48[] = {
    { 2000.0f, 0.0f, 0.0f },
    { 2800.0f, -800.0f, -800.0f },
    { 2800.0f, 800.0f, 800.0f },
};

Vec3f D_801C0A6C[] = {
    { -400.0f, 800.0f, 0.0f },
    { -5000.0f, -500.0f, -4000.0f },
    { -5000.0f, 8000.0f, 4000.0f },
};

Vec3f D_801C0A90[] = {
    { -400.0f, 1800.0f, 0.0f },
    { 5000.0f, 8000.0f, 4000.0f },
    { 5000.0f, -500.0f, -4000.0f },
};

Gfx* D_801C0AB4[] = {
    object_link_zora_DL_00CC38,
    object_link_zora_DL_00CDA0,
};

Gfx* D_801C0ABC[] = {
    object_link_zora_DL_010868,
    object_link_zora_DL_010978,
};

Vec3f D_801C0AC4[] = {
    { 5400.0f, 1700.0f, 1800.0f },
    { 5400.0f, 1700.0f, -1800.0f },
};

Vec3f D_801C0ADC[] = {
    { 5250.0f, 570.0f, 2400.0f },
    { 5250.0f, 570.0f, -2400.0f },
};

struct_80124618* D_801C0AF4[] = {
    D_801C0678,
    D_801C0698,
};

struct_80124618* D_801C0AFC[] = {
    D_801C06B8,
    D_801C06E0,
};

struct_80124618* D_801C0B04[] = {
    D_801C06F8,
    D_801C0718,
};

struct_80124618* D_801C0B0C[] = {
    D_801C0730,
    D_801C0740,
};

Gfx* D_801C0B14[] = {
    object_link_nuts_DL_008760,
    object_link_nuts_DL_008660,
};

u8 D_801C0B1C[] = {
    PLAYER_BODYPART_LEFT_HAND,
    PLAYER_BODYPART_RIGHT_HAND,
};

const char* D_801C0B20[] = {
    object_mask_truth_DL_0001A0,    // PLAYER_MASK_TRUTH
    gKafeisMaskDL,                  // PLAYER_MASK_KAFEIS_MASK
    object_mask_yofukasi_DL_000490, // PLAYER_MASK_ALL_NIGHT
    object_mask_rabit_DL_000610,    // PLAYER_MASK_BUNNY
    object_mask_ki_tan_DL_0004A0,   // PLAYER_MASK_KEATON
    object_mask_json_DL_0004C0,     // PLAYER_MASK_GARO
    object_mask_romerny_DL_0007A0,  // PLAYER_MASK_ROMANI
    object_mask_zacho_DL_000700,    // PLAYER_MASK_CIRCUS_LEADER
    object_mask_posthat_DL_000290,  // PLAYER_MASK_POSTMAN
    object_mask_meoto_DL_0005A0,    // PLAYER_MASK_COUPLE
    object_mask_bigelf_DL_0016F0,   // PLAYER_MASK_GREAT_FAIRY
    object_mask_gibudo_DL_000250,   // PLAYER_MASK_GIBDO
    gDonGeroMaskDL,                 // PLAYER_MASK_DON_GERO
    object_mask_dancer_DL_000EF0,   // PLAYER_MASK_KAMARO
    object_mask_skj_DL_0009F0,      // PLAYER_MASK_CAPTAIN
    object_mask_stone_DL_000820,    // PLAYER_MASK_STONE
    object_mask_bree_DL_0003C0,     // PLAYER_MASK_BREMEN
    object_mask_bakuretu_DL_0005C0, // PLAYER_MASK_BLAST
    object_mask_bu_san_DL_000710,   // PLAYER_MASK_SCENTS
    object_mask_kyojin_DL_000380,   // PLAYER_MASK_GIANT
    gFierceDeityMaskDL,             // PLAYER_MASK_FIERCE_DEITY
    gGoronMaskDL,                   // PLAYER_MASK_GORON
    gZoraMaskDL,                    // PLAYER_MASK_ZORA
    gDekuMaskDL,                    // PLAYER_MASK_DEKU
    object_mask_boy_DL_000900,
    object_mask_goron_DL_0014A0,
    object_mask_zora_DL_000DB0,
    object_mask_nuts_DL_001D90,
};

Vec3f D_801C0B90[D_801F59B0_LEN] = {
    { 950.0f, -800.0f, 300.0f },
    { 950.0f, -800.0f, -300.0f },
};

void func_8012669C(PlayState* play, Player* player, Vec3f* arg2, Vec3f* arg3) {
    Vec3f sp3C;
    Vec3f sp30;

    Matrix_MultVec3f(arg2, &sp3C);
    Matrix_MultVec3f(arg3, &sp30);

    if (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) {
        if (func_80126440(play, NULL, &player->meleeWeaponInfo[0], &sp3C, &sp30) &&
            (player->transformation != PLAYER_FORM_GORON) && !(player->stateFlags1 & PLAYER_STATE1_400000)) {
            EffectBlure_AddVertex(Effect_GetByIndex(player->meleeWeaponEffectIndex[0]), &player->meleeWeaponInfo[0].tip,
                                  &player->meleeWeaponInfo[0].base);
        }

        if ((player->meleeWeaponState >= PLAYER_MELEE_WEAPON_STATE_1) &&
            ((player->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) ||
             (player->stateFlags2 & PLAYER_STATE2_20000))) {
            Matrix_MultVec3f(&arg2[1], &sp3C);
            Matrix_MultVec3f(&arg3[1], &sp30);
            func_80126440(play, &player->meleeWeaponQuads[0], &player->meleeWeaponInfo[1], &sp3C, &sp30);
            Matrix_MultVec3f(&arg2[2], &sp3C);
            Matrix_MultVec3f(&arg3[2], &sp30);
            func_80126440(play, &player->meleeWeaponQuads[1], &player->meleeWeaponInfo[2], &sp3C, &sp30);
        }
    } else {
        Math_Vec3f_Copy(&player->meleeWeaponInfo[0].tip, &sp3C);
        Math_Vec3f_Copy(&player->meleeWeaponInfo[0].base, &sp30);
    }
}

void Player_DrawGetItemImpl(PlayState* play, Player* player, Vec3f* refPos, s32 drawIdPlusOne) {
    f32 sp34;

    if (player->stateFlags3 & PLAYER_STATE3_4000000) {
        sp34 = 6.0f;
    } else {
        sp34 = 14.0f;
    }

    OPEN_DISPS(play->state.gfxCtx);

    gSegments[6] = OS_K0_TO_PHYSICAL(player->giObjectSegment);

    gSPSegment(POLY_OPA_DISP++, 0x06, player->giObjectSegment);
    gSPSegment(POLY_XLU_DISP++, 0x06, player->giObjectSegment);

    Matrix_Translate((Math_SinS(player->actor.shape.rot.y) * 3.3f) + refPos->x, refPos->y + sp34,
                     (Math_CosS(player->actor.shape.rot.y) * 3.3f) + refPos->z, MTXMODE_NEW);
    Matrix_RotateZYX(0, (play->gameplayFrames * 1000), 0, MTXMODE_APPLY);
    Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);
    GetItem_Draw(play, drawIdPlusOne - 1);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_DrawGetItem(PlayState* play, Player* player) {
    if (!player->giObjectLoading || (osRecvMesg(&player->giObjectLoadQueue, NULL, OS_MESG_NOBLOCK) == 0)) {
        Vec3f refPos;
        s32 drawIdPlusOne;

        player->giObjectLoading = false;
        if ((player->actor.id == ACTOR_EN_TEST3) ||
            ((player->transformation == PLAYER_FORM_DEKU) && (player->stateFlags1 & PLAYER_STATE1_400))) {
            refPos.x = player->actor.world.pos.x;
            refPos.z = player->actor.world.pos.z;
            if (player->actor.id == ACTOR_EN_TEST3) {
                if (player->stateFlags1 & PLAYER_STATE1_400) {
                    refPos.y = player->actor.world.pos.y + 30.0f;
                } else {
                    refPos.x = player->bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].x;
                    refPos.y = player->bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].y - 6.0f;
                    refPos.z = player->bodyPartsPos[PLAYER_BODYPART_LEFT_HAND].z;
                }
            } else {
                refPos.y = player->actor.world.pos.y + 28.0f;
            }
        } else {
            Math_Vec3f_Copy(&refPos, &sPlayerGetItemRefPos);
        }

        drawIdPlusOne = ABS_ALT(player->getItemDrawIdPlusOne);
        Player_DrawGetItemImpl(play, player, &refPos, drawIdPlusOne);
    }
}

void func_80126AB4(Player* player, Vec3f** arg1) {
    if ((player->transformation == PLAYER_FORM_GORON) || (player->actor.id == ACTOR_EN_TEST3)) {
        Math_Vec3f_Copy(&player->unk_AF0[1], &player->meleeWeaponInfo[0].base);
        *arg1 = D_801C09B8;
        return;
    }

    D_801C0994[1].x = D_801C0994[0].x;

    if (player->unk_ADD >= 3) {
        player->unk_ADD++;
        D_801C0994[1].x *= 1.0f + ((9 - player->unk_ADD) * 0.1f);
    }

    D_801C0994[1].x += 1200.0f;
    D_801C0994[2].x = D_801C0994[1].x;
    *arg1 = D_801C0994;
}

void func_80126B8C(PlayState* play, Player* player) {
    Vec3f* sp1C;

    func_80126AB4(player, &sp1C);
    func_8012669C(play, player, sp1C, D_801C0970);
}

// Zora boomerangs (?)
void func_80126BD0(PlayState* play, Player* player, s32 arg2) {
    if ((player->transformation != PLAYER_FORM_ZORA) || (player->rightHandType == PLAYER_MODELTYPE_RH_HOOKSHOT)) {
        return;
    }

    if ((arg2 != 0) && (player->stateFlags1 & PLAYER_STATE1_400000)) {
        OPEN_DISPS(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_link_zora_DL_0110A8);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        Actor* zoraBoomerangActor = player->zoraBoomerangActor;
        Vec3f sp58;
        Vec3f sp4C;

        if (player->stateFlags1 & PLAYER_STATE1_ZORA_BOOMERANG_THROWN) {
            if (player->zoraBoomerangActor == NULL) {
                return;
            }
            if ((player->zoraBoomerangActor->params == arg2) ||
                (((zoraBoomerangActor->child != NULL)) && (zoraBoomerangActor->child->params == arg2))) {
                return;
            }
        }

        OPEN_DISPS(play->state.gfxCtx);

        if ((!BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_gakkiplay)) &&
            (!BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_fishswim))) {
            if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_gakkistart)) {
                func_80124618(D_801C0580, player->skelAnime.curFrame, player->unk_AF0);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_waterroll)) {
                func_80124618(D_801C05A8, player->skelAnime.curFrame, player->unk_AF0);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_swimtowait)) {
                func_80124618(D_801C05D8, player->skelAnime.curFrame, player->unk_AF0);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_bladeon)) {
                func_80124618(D_801C07F0, player->skelAnime.curFrame, player->unk_AF0);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_jumpAT)) {
                func_80124618(D_801C0820, player->skelAnime.curFrame, player->unk_AF0);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_jumpATend)) {
                func_80124618(D_801C0838, player->skelAnime.curFrame, player->unk_AF0);
            } else if (player->heldItemAction == PLAYER_IA_ZORA_BOOMERANG) {
                player->unk_AF0[0].x = 1.0f;
                player->unk_AF0[0].y = 1.0f;
                player->unk_AF0[0].z = 1.0f;
            } else {
                player->unk_AF0[0].x = 0.4f;
                player->unk_AF0[0].y = 0.6f;
                player->unk_AF0[0].z = 0.7f;
            }
            Matrix_Push();

            Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, D_801C0AB4[arg2]);

            if (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) {
                if ((((player->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_LEFT)) && (arg2 == 0)) ||
                    ((player->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_COMBO) && (arg2 != 0))) {
                    func_8012669C(play, player, D_801C0A00, D_801C09DC);
                }
            }

            Matrix_Pop();
        }

        if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_waterroll)) {
            func_80124618(D_801C05C8, player->skelAnime.curFrame, player->unk_AF0);
            D_801C05F0[0].unk_2.x = 50;
            D_801C05F0[0].unk_2.y = 50;
            D_801C05F0[0].unk_2.z = 50;
            D_801C05F0[1].unk_2 = D_801C05F0[0].unk_2;
        } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_swimtowait)) {
            func_80124618(D_801C05F0, player->skelAnime.curFrame, player->unk_AF0);
        } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pz_fishswim)) {
            player->unk_AF0[0].x = (ABS_ALT(player->unk_B8A) * 0.00003f) + 0.5f;
            player->unk_AF0[0].y = player->unk_AF0[0].x;
            player->unk_AF0[0].z = player->unk_AF0[0].x;

            D_801C05F0[0].unk_2.x = (player->unk_AF0[0].x * 100.0f);

            D_801C05F0[0].unk_2.y = D_801C05F0[0].unk_2.x;
            D_801C05F0[0].unk_2.z = D_801C05F0[0].unk_2.x;

            D_801C05F0[1].unk_2 = D_801C05F0[0].unk_2;
        } else {
            //! @bug Skips CLOSE_DISPS
            // 2S2H [Port] We need our close disps helpers called to prevent interpolation crashes
            CLOSE_DISPS_PORT_HELPERS(play->state.gfxCtx);
            return;
        }

        Matrix_Push();
        Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, D_801C0ABC[arg2]);

        Matrix_MultVec3f(&D_801C0AC4[arg2], &sp58);
        Matrix_MultVec3f(&D_801C0ADC[arg2], &sp4C);

        if (func_80126440(play, NULL, &player->meleeWeaponInfo[arg2], &sp58, &sp4C) &&
            (player->stateFlags1 & PLAYER_STATE1_8000000)) {
            EffectBlure_AddVertex(Effect_GetByIndex(player->meleeWeaponEffectIndex[arg2]),
                                  &player->meleeWeaponInfo[arg2].tip, &player->meleeWeaponInfo[arg2].base);
        }
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 func_801271B0(PlayState* play, Player* player, s32 arg2) {
    if (player->transformation == PLAYER_FORM_DEKU) {
        if ((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_kakku)) ||
            (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_kakkufinish)) ||
            (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_rakkafinish)) ||
            (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_batabata))) {
            struct_80124618** sp3C = D_801C0AF4;

            OPEN_DISPS(play->state.gfxCtx);

            if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_kakkufinish)) {
                sp3C = D_801C0AFC;
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_batabata)) {
                sp3C = D_801C0B0C;
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_rakkafinish)) {
                sp3C = D_801C0B04;
            }

            Matrix_Push();
            Matrix_Translate(0.0f, 150.0f, 0.0f, MTXMODE_APPLY);
            func_80124618(sp3C[0], player->skelAnime.curFrame, &player->unk_AF0[1]);
            Matrix_Scale(player->unk_AF0[1].x, player->unk_AF0[1].y, player->unk_AF0[1].z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, D_801C0B14[arg2]);

            Matrix_Translate(2150.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateXS(player->unk_B8A, MTXMODE_APPLY);
            func_80124618(sp3C[1], player->skelAnime.curFrame, &player->unk_AF0[1]);
            Matrix_Scale(player->unk_AF0[1].x, player->unk_AF0[1].y, player->unk_AF0[1].z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            // Close flower / Open flower
            gSPDisplayList(POLY_OPA_DISP++,
                           player->actor.velocity.y < -6.0f ? gLinkDekuClosedFlowerDL : gLinkDekuOpenFlowerDL);

            Matrix_MultZero(&player->bodyPartsPos[D_801C0B1C[arg2]]);
            Matrix_Pop();

            CLOSE_DISPS(play->state.gfxCtx);

            return true;
        }
    }

    return false;
}

// Player_SetMaskSegment?
s32 func_80127438(PlayState* play, Player* player, s32 currentMask) {
    if ((player->maskObjectLoadState == 0) && (currentMask == (u8)player->maskId)) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x0A, player->maskObjectSegment);

        CLOSE_DISPS(play->state.gfxCtx);

        return true;
    }

    return false;
}

void func_80127488(PlayState* play, Player* player, u8 alpha) {
    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, alpha);
    gSPDisplayList(POLY_XLU_DISP++, gLinkGoronGoronPunchEffectDL);

    func_80122BA4(play, &player->unk_3D0, 3, alpha);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_DrawCouplesMask(PlayState* play, Player* player) {
    gSegments[0xA] = OS_K0_TO_PHYSICAL(player->maskObjectSegment);
    AnimatedMat_DrawOpa(play, Lib_SegmentedToVirtual(&object_mask_meoto_Matanimheader_001CD8));
}

void Player_DrawCircusLeadersMask(PlayState* play, Player* player) {
    static Vec3f sBubbleVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sBubbleAccel = { 0.0f, 0.0f, 0.0f };
    Gfx* gfx;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_XLU_DISP;

    for (i = 0; i < D_801F59B0_LEN; i++) {
        f32 scaleY = (D_801F59C8[i] / 400.0f) * 0.1f;

        Matrix_MultVec3f(&D_801C0B90[i], &D_801F59B0[i]);

        //! FAKE:
        if (1) {}

        D_801F59B0[i].y += -10.0f * scaleY;

        if (D_801F59C8[i] < 400) {
            f32 scaleXZ = CLAMP_MAX(scaleY, 0.05f);

            Matrix_Push();
            Matrix_Translate(D_801F59B0[i].x, D_801F59B0[i].y, D_801F59B0[i].z, MTXMODE_NEW);
            Matrix_Scale(scaleXZ, scaleY, scaleXZ, MTXMODE_APPLY);

            gSPMatrix(&gfx[0], Matrix_Finalize(play->state.gfxCtx), G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPSegment(&gfx[1], 0x08, OS_K0_TO_PHYSICAL(SEGMENTED_TO_K0(gEffBubble1Tex)));
            gDPSetPrimColor(&gfx[2], 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(&gfx[3], 150, 150, 150, 0);
            gSPDisplayList(&gfx[4], gEffBubbleDL);
            gSPPopMatrix(&gfx[5], G_MTX_MODELVIEW);

            Matrix_Pop();

            gfx = &gfx[6];
        } else {
            f32 speedXZ = sqrtf(SQ(player->actor.velocity.x) + SQ(player->actor.velocity.z));
            s16 phi_s0 = speedXZ * 2000.0f;
            f32 temp_f20;

            sBubbleVelocity.y = speedXZ * 0.4f;
            sBubbleAccel.y = -0.3f;

            if (phi_s0 > 0x3E80) {
                phi_s0 = 0x3E80;
            }

            phi_s0 = player->actor.focus.rot.y + ((i != 0) ? phi_s0 : -phi_s0);
            temp_f20 = speedXZ * 0.2f;
            temp_f20 = CLAMP_MAX(temp_f20, 4.0f);

            sBubbleVelocity.x = -Math_SinS(phi_s0) * temp_f20;
            sBubbleVelocity.z = -Math_CosS(phi_s0) * temp_f20;

            EffectSsDtBubble_SpawnColorProfile(play, &D_801F59B0[i], &sBubbleVelocity, &sBubbleAccel, 20, 20, 3, 0);
            D_801F59C8[i] -= 400;
        }
    }

    POLY_XLU_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_DrawBlastMask(PlayState* play, Player* player) {
    static Gfx D_801C0BC0[] = {
        gsDPSetEnvColor(0, 0, 0, 255),
        gsSPEndDisplayList(),
    };
    static Gfx D_801C0BD0[] = {
        gsDPSetRenderMode(AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                              G_RM_FOG_SHADE_A,
                          AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                              GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
        gsSPEndDisplayList(),
    };

    OPEN_DISPS(play->state.gfxCtx);

    if (player->blastMaskTimer != 0) {
        s32 alpha;

        gSegments[0xA] = OS_K0_TO_PHYSICAL(player->maskObjectSegment);

        AnimatedMat_DrawOpa(play, Lib_SegmentedToVirtual(&object_mask_bakuretu_Matanimheader_0011F8));

        if (player->blastMaskTimer <= 10) {
            alpha = (player->blastMaskTimer / 10.0f) * 255;
        } else {
            alpha = 255;
        }

        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, (u8)alpha);
        gSPDisplayList(POLY_OPA_DISP++, object_mask_bakuretu_DL_000440);
        gSPSegment(POLY_OPA_DISP++, 0x09, D_801C0BD0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, (u8)(255 - alpha));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x09, D_801C0BC0);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Vec3f D_801C0BE0 = { 0.0f, 0.3f, 0.0f };
Vec3f D_801C0BEC = { 0.0f, -0.025f, 0.0f };
Color_RGBA8 D_801C0BF8 = { 250, 100, 100, 0 };
Color_RGBA8 D_801C0BFC = { 0, 0, 100, 0 };

Vec3f D_801C0C00 = { 0.0f, 20.0f, 0.0f };

Vec3f D_801C0C0C[] = {
    { 174.0f, -1269.0f, -1.0f },
    { 401.0f, -729.0f, -701.0f },
    { 401.0f, -729.0f, 699.0f },
};

Vec3f D_801C0C30[] = {
    { 74.0f, -1269.0f, -1.0f },
    { 301.0f, -729.0f, -701.0f },
    { 301.0f, -729.0f, 699.0f },
};

typedef struct struct_80128388_arg1 {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ Vec3f unk_08;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 unk_1A;
} struct_80128388_arg1; // size = 0x1C

struct_80128388_arg1 D_801C0C54[] = {
    { 0.0f, 0x0000, 0x8000, { 0.0f, 0.0f, 0.0f }, 0.0f, 0x0000, 0x0000 },
    { 16.8f, 0x0000, 0x0000, { 0.0f, 0.0f, 0.0f }, 20.0f, 0x1388, 0x1388 },
    { 30.0f, 0x0000, 0x0000, { 0.0f, 0.0f, 0.0f }, 20.0f, 0x1F40, 0x2EE0 },
};

Color_RGB8 sPlayerBottleColors[PLAYER_BOTTLE_MAX] = {
    { 255, 255, 255 }, // PLAYER_BOTTLE_EMPTY
    { 80, 80, 255 },   // PLAYER_BOTTLE_FISH
    { 136, 192, 255 }, // PLAYER_BOTTLE_SPRING_WATER
    { 136, 192, 255 }, // PLAYER_BOTTLE_HOT_SPRING_WATER
    { 184, 232, 232 }, // PLAYER_BOTTLE_ZORA_EGG
    { 248, 200, 0 },   // PLAYER_BOTTLE_DEKU_PRINCESS
    { 255, 180, 0 },   // PLAYER_BOTTLE_GOLD_DUST
    { 0, 128, 0 },     // PLAYER_BOTTLE_1C
    { 252, 238, 0 },   // PLAYER_BOTTLE_SEAHORSE
    { 131, 0, 174 },   // PLAYER_BOTTLE_MUSHROOM
    { 64, 64, 32 },    // PLAYER_BOTTLE_HYLIAN_LOACH
    { 0, 0, 255 },     // PLAYER_BOTTLE_BUG
    { 255, 0, 255 },   // PLAYER_BOTTLE_POE
    { 255, 0, 255 },   // PLAYER_BOTTLE_BIG_POE
    { 255, 0, 0 },     // PLAYER_BOTTLE_POTION_RED
    { 0, 0, 255 },     // PLAYER_BOTTLE_POTION_BLUE
    { 0, 200, 0 },     // PLAYER_BOTTLE_POTION_GREEN
    { 255, 255, 255 }, // PLAYER_BOTTLE_MILK
    { 255, 255, 255 }, // PLAYER_BOTTLE_MILK_HALF
    { 255, 255, 255 }, // PLAYER_BOTTLE_CHATEAU
    { 80, 80, 255 },   // PLAYER_BOTTLE_FAIRY
};

Vec3f D_801C0CE8[PLAYER_FORM_MAX] = {
    { 0.0f, 0.0f, 0.0f },        // PLAYER_FORM_FIERCE_DEITY
    { 300.0f, 300.0f, -230.0f }, // PLAYER_FORM_GORON
    { 0.0f, 90.0f, -50.0f },     // PLAYER_FORM_ZORA
    { 0.0f, 20.0f, -60.0f },     // PLAYER_FORM_DEKU
    { 0.0f, 0.0f, 0.0f },        // PLAYER_FORM_HUMAN
};

void Player_DrawBunnyHood(PlayState* play) {
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Mtx));
    Vec3s earRot;

    OPEN_DISPS(play->state.gfxCtx);

    // Ignoring the players actor mtx allows the bunny hood to render interpolated without issues
    FrameInterpolation_IgnoreActorMtx();

    gSPSegment(POLY_OPA_DISP++, 0x0B, mtx);

    Matrix_Push();

    f32 earLength = CVarGetFloat(CVAR_COSMETIC("Silly.BunnyHoodEarLength"), 0.0f);
    f32 earSpread = CVarGetFloat(CVAR_COSMETIC("Silly.BunnyHoodEarSpread"), 0.0f);

    earRot.x = sBunnyEarKinematics.rot.y + 0x3E2;
    earRot.y = sBunnyEarKinematics.rot.z + 0xDBE;
    earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
    Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - earLength, -240.0f - earSpread, &earRot);

    Matrix_ToMtx(mtx++);

    earRot.x = sBunnyEarKinematics.rot.y - 0x3E2;
    earRot.y = -sBunnyEarKinematics.rot.z - 0xDBE;
    earRot.z = sBunnyEarKinematics.rot.x - 0x348A;
    Matrix_SetTranslateRotateYXZ(97.0f, -1203.0f - earLength, 240.0f + earSpread, &earRot);

    Matrix_ToMtx(mtx);

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80127B64(struct_801F58B0 arg0[], s32 count, Vec3f* arg2) {
    s32 i;

    for (i = 0; i < count; i++, arg0++) {
        Math_Vec3f_Copy(&arg0->unk_00, arg2);
        Math_Vec3f_Copy(&arg0->unk_0C, &gZeroVec3f);
        arg0->unk_18 = 0;
        arg0->unk_1A = 0;
    }
}

// Draws the Great Fairy's Mask particles when a stray fairy is in the room
void Player_DrawStrayFairyParticles(PlayState* play, Vec3f* arg1) {
    Vec3f sp2C;
    f32 sp28;

    D_801C0BE0.y = Rand_ZeroFloat(0.07f) + -0.1f;
    D_801C0BEC.y = Rand_ZeroFloat(0.1f) + 0.04f;
    if (Rand_ZeroOne() < 0.5f) {
        sp28 = -1.0f;
    } else {
        sp28 = 1.0f;
    }

    D_801C0BE0.x = (Rand_ZeroFloat(0.2f) + 0.1f) * sp28;
    D_801C0BEC.x = 0.1f * sp28;
    if (Rand_ZeroOne() < 0.5f) {
        sp28 = -1.0f;
    } else {
        sp28 = 1.0f;
    }

    D_801C0BE0.z = (Rand_ZeroFloat(0.2f) + 0.1f) * sp28;
    D_801C0BEC.z = 0.1f * sp28;
    sp2C.x = arg1->x;
    sp2C.y = Rand_ZeroFloat(15.0f) + arg1->y;
    sp2C.z = arg1->z;
    EffectSsKirakira_SpawnDispersed(play, &sp2C, &D_801C0BE0, &D_801C0BEC, &D_801C0BF8, &D_801C0BFC, -50, 11);
}

void func_80127DA4(PlayState* play, struct_801F58B0 arg1[], struct_80128388_arg1 arg2[], s32 arg3, Vec3f* arg4,
                   Vec3f* arg5, u32* arg6) {
    struct_801F58B0* phi_s1 = &arg1[1];
    Vec3f spB0;
    Vec3f spA4;
    f32 f22;
    f32 f28;
    f32 f24;
    f32 f20;
    f32 f0;
    f32 sp8C = -1.0f;
    s32 i;
    s16 s0;
    s16 s2;

    Math_Vec3f_Copy(&arg1->unk_00, arg4);
    Math_Vec3f_Diff(arg5, arg4, &spB0);
    arg1->unk_18 = Math_Atan2S_XY(spB0.z, spB0.x);
    arg1->unk_1A = Math_Atan2S_XY(sqrtf(SQXZ(spB0)), spB0.y);
    i = 1;
    arg2++;

    while (i < arg3) {
        if (play->actorCtx.flags & ACTORCTX_FLAG_3) {
            if (*arg6 & 0x20) {
                sp8C = -0.2f;
            } else {
                sp8C = 0.2f;
            }

            *arg6 += 0x16;
            if (!(*arg6 & 1)) {
                Player_DrawStrayFairyParticles(play, &phi_s1->unk_00);
            }
        }
        Math_Vec3f_Sum(&phi_s1->unk_00, &phi_s1->unk_0C, &phi_s1->unk_00);

        f0 = Math_Vec3f_DistXYZAndStoreDiff(&arg1->unk_00, &phi_s1->unk_00, &spB0);
        f28 = f0 - arg2->unk_00;
        if (f0 == 0.0f) {
            spB0.x = 0.0f;
            spB0.y = arg2->unk_00;
            spB0.z = 0.0f;
        }
        f20 = sqrtf(SQXZ(spB0));

        if (f20 > 4.0f) {
            phi_s1->unk_18 = Math_Atan2S_XY(spB0.z, spB0.x);
            s2 = phi_s1->unk_18 - arg1->unk_18;

            if (ABS_ALT(s2) > 0x4000) {
                phi_s1->unk_18 = (s16)(phi_s1->unk_18 + 0x8000);
                f20 = -f20;
            }
        }

        phi_s1->unk_1A = Math_Atan2S_XY(f20, spB0.y);

        s2 = phi_s1->unk_18 - arg1->unk_18;
        s2 = CLAMP(s2, -arg2->unk_18, arg2->unk_18);
        phi_s1->unk_18 = arg1->unk_18 + s2;

        s0 = phi_s1->unk_1A - arg1->unk_1A;
        s0 = CLAMP(s0, -arg2->unk_1A, arg2->unk_1A);
        phi_s1->unk_1A = arg1->unk_1A + s0;

        f20 = Math_CosS(phi_s1->unk_1A) * arg2->unk_00;
        spA4.x = Math_SinS(phi_s1->unk_18) * f20;
        spA4.z = Math_CosS(phi_s1->unk_18) * f20;
        spA4.y = Math_SinS(phi_s1->unk_1A) * arg2->unk_00;
        Math_Vec3f_Sum(&arg1->unk_00, &spA4, &phi_s1->unk_00);
        phi_s1->unk_0C.x *= 0.9f;
        phi_s1->unk_0C.z *= 0.9f;

        f22 = Math_CosS(s0) * f28;
        f24 = Math_SinS(s0) * f28;
        phi_s1->unk_0C.y += sp8C;

        if (play->actorCtx.flags & ACTORCTX_FLAG_3) {
            phi_s1->unk_0C.y = CLAMP(phi_s1->unk_0C.y, -0.8f, 0.8f);
        } else {
            phi_s1->unk_0C.y = phi_s1->unk_0C.y; // Set to itself
            f20 = Math_SinS(arg1->unk_1A);
            phi_s1->unk_0C.y += (((f22 * Math_CosS(arg1->unk_1A)) + (f24 * f20)) * 0.2f);
            phi_s1->unk_0C.y = CLAMP(phi_s1->unk_0C.y, -2.0f, 4.0f);
        }

        f20 = (f24 * Math_CosS(arg1->unk_1A)) - (Math_SinS(arg1->unk_1A) * f22);
        f22 = Math_CosS(s2) * f20;
        f24 = Math_SinS(s2) * f20;

        f20 = Math_SinS(arg1->unk_18);

        phi_s1->unk_0C.x += (((f24 * Math_CosS(arg1->unk_18)) - (f22 * f20)) * 0.1f);
        phi_s1->unk_0C.x = CLAMP(phi_s1->unk_0C.x, -4.0f, 4.0f);

        f20 = Math_SinS(arg1->unk_18);

        phi_s1->unk_0C.z += (((f22 * Math_CosS(arg1->unk_18)) + (f24 * f20)) * -0.1f);
        phi_s1->unk_0C.z = CLAMP(phi_s1->unk_0C.z, -4.0f, 4.0f);

        arg1++;
        phi_s1++;
        i++;
        arg2++;
    }
}

void func_80128388(struct_801F58B0 arg0[], struct_80128388_arg1 arg1[], s32 arg2, Mtx** arg3) {
    struct_801F58B0* phi_s1 = &arg0[1];
    Vec3f sp58;
    Vec3s sp50;
    s32 i;

    sp58.y = 0.0f;
    sp58.z = 0.0f;
    sp50.x = 0;

    for (i = 1; i < arg2; i++) {
        sp58.x = arg1->unk_00 * 100.0f * CVarGetFloat(CVAR_COSMETIC("Silly.GreatFairyMaskHairLength"), 1.0f);
        sp50.z = arg1->unk_06 + (s16)(phi_s1->unk_1A - arg0->unk_1A);
        sp50.y = arg1->unk_04 + (s16)(phi_s1->unk_18 - arg0->unk_18);
        Matrix_TranslateRotateZYX(&sp58, &sp50);
        Matrix_ToMtx(*arg3);
        (*arg3)++;
        arg0++;
        phi_s1++;
        arg1++;
    }
}

void Player_DrawGreatFairysMask(PlayState* play, Player* player) {
    s32 pad;
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, 6 * sizeof(Mtx));
    Vec3f sp84;
    Vec3f sp78;
    Vec3f* iter = D_801C0C0C;
    Vec3f* iter2 = D_801C0C30;
    u32 sp6C = play->gameplayFrames;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0B, mtx);

    Matrix_MultVec3f(&D_801C0C00, &D_801C0C54[1].unk_08);
    Math_Vec3f_Lerp(&player->bodyPartsPos[PLAYER_BODYPART_HEAD], &player->bodyPartsPos[PLAYER_BODYPART_WAIST], 0.2f,
                    &D_801C0C54[2].unk_08);

    for (i = 0; i < ARRAY_COUNT(D_801C0C0C); i++) {
        Matrix_MultVec3f(iter, &sp84);
        Matrix_MultVec3f(iter2, &sp78);

        func_80127DA4(play, D_801F58B0[i], D_801C0C54, 3, &sp84, &sp78, &sp6C);
        sp6C += 11;

        Matrix_Push();
        Matrix_Translate(iter->x, iter->y,
                         iter->z * CVarGetFloat(CVAR_COSMETIC("Silly.GreatFairyMaskHairSpread"), 1.0f), MTXMODE_APPLY);
        func_80128388(D_801F58B0[i], D_801C0C54, 3, &mtx);
        Matrix_Pop();
        iter++;
        iter2++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 func_80128640(PlayState* play, Player* player, Gfx* dlist) {
    s32 temp_v1 = BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_cl_maskoff);
    f32 temp_f0;

    if (temp_v1 ||
        ((player->currentMask != PLAYER_MASK_NONE) &&
         (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_cl_setmask)) &&
         (temp_f0 = player->skelAnime.curFrame - 8.0f, (temp_f0 >= 0.0f)) && (temp_f0 < 4.0f)) ||
        (player->stateFlags2 & PLAYER_STATE2_1000000)) {
        s32 mask;

        if (temp_v1) {
            mask = player->prevMask;
        } else {
            mask = player->currentMask;
        }

        if (func_80127438(play, player, mask)) {
            OPEN_DISPS(play->state.gfxCtx);

            Matrix_Push();
            Matrix_Translate(-323.67f, 412.15f, -969.96f, MTXMODE_APPLY);
            Matrix_RotateZYX(-0x32BE, -0x50DE, -0x7717, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, D_801C0B20[mask - 1]);

            Matrix_Pop();

            CLOSE_DISPS(play->state.gfxCtx);
        }

    } else if (player->itemAction == PLAYER_IA_DEKU_STICK) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_Translate(-428.26f, 267.2f, -33.82f, MTXMODE_APPLY);
        Matrix_RotateZYX(-0x8000, 0, 0x4000, MTXMODE_APPLY);
        Matrix_Scale(1.0f, player->unk_B0C, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gDekuStickDL);

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (player->leftHandType == PLAYER_MODELTYPE_LH_BOTTLE) {
        PlayerBottle bottle = Player_BottleFromIA(player, player->itemAction);
        Vec3f* temp_v1_2 = &D_801C0CE8[player->transformation];

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_Translate(temp_v1_2->x, temp_v1_2->y, temp_v1_2->z, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        // Note this does not check for PLAYER_BOTTLE_NONE, which would produce an OoB access on sPlayerBottleColors.
        // Under normal circunstances it should not be a problem because of the previous
        // `player->leftHandType == PLAYER_MODELTYPE_LH_BOTTLE` check
        if (bottle != PLAYER_BOTTLE_EMPTY) {
            Color_RGB8* bottleColor = &sPlayerBottleColors[bottle];

            gDPSetEnvColor(POLY_XLU_DISP++, bottleColor->r, bottleColor->g, bottleColor->b, 0);
            gSPDisplayList(POLY_XLU_DISP++, gBottleContentsDL);
        }

        gSPDisplayList(POLY_XLU_DISP++, gBottleGlassDL);

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (dlist == object_link_zora_DL_00E2A0) { // zora guitar
        s16 sp26 = Math_SinS(player->unk_B86[0]) * (ABS_ALT(player->upperLimbRot.x) * ((f32)(IREG(52) + 20)) / 100.0f);
        s16 sp24 = Math_SinS(player->unk_B86[1]) * (ABS_ALT(player->upperLimbRot.y) * ((f32)(IREG(53) + 15)) / 100.0f);

        OPEN_DISPS(play->state.gfxCtx);

        Matrix_Push();
        Matrix_RotateXS(sp26, MTXMODE_APPLY);
        Matrix_RotateYS(sp24, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_link_zora_DL_00E088); // hand

        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        return false;
    }

    return true;
}

Vec3f sPlayerFootPos[PLAYER_FORM_MAX] = {
    { 200.0f, 300.0f, 0.0f }, // PLAYER_FORM_FIERCE_DEITY
    { 200.0f, 200.0f, 0.0f }, // PLAYER_FORM_GORON
    { 200.0f, 300.0f, 0.0f }, // PLAYER_FORM_ZORA
    { 200.0f, 150.0f, 0.0f }, // PLAYER_FORM_DEKU
    { 200.0f, 200.0f, 0.0f }, // PLAYER_FORM_HUMAN
};

void Player_SetFeetPos(PlayState* play, Player* player, s32 limbIndex) {
    Vec3f* footPos = &sPlayerFootPos[player->transformation];

    Actor_SetFeetPos(&player->actor, limbIndex, PLAYER_LIMB_LEFT_FOOT, footPos, PLAYER_LIMB_RIGHT_FOOT, footPos);
}

void Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor) {
    Player* player = (Player*)actor;

    if (*dList2 != NULL) {
        Matrix_MultZero(sPlayerCurBodyPartPos);
    }

    CosmeticEditor_DrawSplitHeldItem(play);

    if (limbIndex == PLAYER_LIMB_LEFT_HAND) {
        Math_Vec3f_Copy(&player->leftHandWorld.pos, sPlayerCurBodyPartPos);

        if ((*dList1 != NULL) && !func_801271B0(play, player, 0) && !func_80128640(play, player, *dList1) &&
            (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchA))) {
            func_80127488(play, player, D_801C0778[(s32)player->skelAnime.curFrame]);
        }

        if (player->actor.scale.y >= 0.0f) {
            Actor* heldActor;
            MtxF sp230;

            if (!Player_IsHoldingHookshot(player) && ((heldActor = player->heldActor) != NULL)) {
                if ((player->stateFlags3 & PLAYER_STATE3_40) && (player->transformation != PLAYER_FORM_DEKU)) {
                    static Vec3f D_801C0D60 = { 398.0f, 1419.0f, 244.0f };
                    static Vec3f D_801C0D6C = { 420.0f, 1210.0f, 380.0f };
                    Vec3s* temp_s1;
                    Vec3f* var_a0 = &D_801C0D60;

                    if (player->transformation == PLAYER_FORM_HUMAN) {
                        var_a0 = &D_801C0D6C;
                    }

                    Matrix_MultVec3f(var_a0, &heldActor->world.pos);
                    Matrix_RotateZYX(0x69E8, -0x5708, 0x458E, MTXMODE_APPLY);

                    Matrix_Get(&sp230);
                    temp_s1 = &heldActor->world.rot;
                    Matrix_MtxFToYXZRot(&sp230, temp_s1, false);
                    heldActor->shape.rot = *temp_s1;
                } else if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                    heldActor->world.rot.y = heldActor->shape.rot.y =
                        player->actor.shape.rot.y + player->leftHandWorld.rot.y;
                }
            } else {
                static f32 sMeleeWeaponLengths[PLAYER_MELEEWEAPON_MAX] = {
                    0.0f,    // PLAYER_MELEEWEAPON_NONE
                    3000.0f, // PLAYER_MELEEWEAPON_SWORD_KOKIRI
                    3000.0f, // PLAYER_MELEEWEAPON_SWORD_RAZOR
                    4000.0f, // PLAYER_MELEEWEAPON_SWORD_GILDED
                    5500.0f, // PLAYER_MELEEWEAPON_SWORD_TWO_HANDED
                    -1.0f,   // PLAYER_MELEEWEAPON_DEKU_STICK
                    2500.0f, // PLAYER_MELEEWEAPON_ZORA_BOOMERANG
                };

                if ((player->transformation == PLAYER_FORM_FIERCE_DEITY) ||
                    ((player->transformation != PLAYER_FORM_ZORA) &&
                     ((player->itemAction == PLAYER_IA_DEKU_STICK) ||
                      ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) &&
                       (player->meleeWeaponAnimation != PLAYER_MWA_GORON_PUNCH_RIGHT) &&
                       (player->meleeWeaponAnimation != PLAYER_MWA_GORON_PUNCH_BUTT))))) {
                    if (player->itemAction == PLAYER_IA_DEKU_STICK) {
                        D_801C0994->x = player->unk_B0C * 5000.0f;
                    } else {
                        D_801C0994->x = sMeleeWeaponLengths[Player_GetMeleeWeaponHeld(player)];
                    }
                    func_80126B8C(play, player);
                }

                Matrix_Get(&player->leftHandMf);
                Matrix_MtxFToYXZRot(&player->leftHandMf, &player->leftHandWorld.rot, false);
            }
        }
    } else if (limbIndex == PLAYER_LIMB_RIGHT_HAND) {
        Actor* heldActor = player->heldActor;
        s32 pad;

        if (*dList1 != NULL) {
            if (player->rightHandType == PLAYER_MODELTYPE_RH_BOW) {
                static Gfx* D_801C0D94 = object_link_child_DL_017818;
                static Vec3f D_801C0D98 = { -35.0f, -395.0f, 0.0f };
                // Unused
                static s32 D_801C0DA4 = 0;

                OPEN_DISPS(play->state.gfxCtx);

                Matrix_Push();
                Matrix_Translate(D_801C0D98.x, D_801C0D98.y, D_801C0D98.z, MTXMODE_APPLY);
                if ((player->stateFlags3 & PLAYER_STATE3_40) && (player->unk_B28 >= 0) && (player->unk_ACC < 0xB)) {
                    Vec3f sp20C;
                    f32 temp_fv0;
                    s32 pad2;

                    Matrix_MultZero(&sp20C);
                    temp_fv0 = Math_Vec3f_DistXYZ(sPlayerCurBodyPartPos, &sp20C);
                    player->unk_B08 = temp_fv0 - 3.0f;
                    if (temp_fv0 < 3.0f) {
                        player->unk_B08 = 0.0f;
                    } else {
                        player->unk_B08 *= 1.6f;
                        if (player->unk_B08 > 1.0f) {
                            player->unk_B08 = 1.0f;
                        }
                    }
                    player->unk_B0C = -0.5f;
                }
                Matrix_Scale(1.0f, player->unk_B08, 1.0f, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, D_801C0D94);

                Matrix_Pop();

                CLOSE_DISPS(play->state.gfxCtx);
            } else if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_punchB)) {
                func_80127488(play, player, D_801C07AC[(s32)player->skelAnime.curFrame]);
            } else {
                func_801271B0(play, player, 1);
            }
        }

        if (player->actor.scale.y >= 0.0f) {
            if (player->rightHandType == PLAYER_MODELTYPE_RH_FF) {
                Matrix_Get(&player->shieldMf);
            } else if (player->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) {
                // Coordinates of the shield quad collider vertices, in the right hand limb's own model space.
                static Vec3f sRightHandLimbModelShieldQuadVertices[4] = {
                    { -4500.0f, -3000.0f, -600.0f },
                    { 1500.0f, -3000.0f, -600.0f },
                    { -4500.0f, 3000.0f, -600.0f },
                    { 1500.0f, 3000.0f, -600.0f },
                };

                Matrix_Get(&player->shieldMf);
                Player_UpdateShieldCollider(play, player, &player->shieldQuad, sRightHandLimbModelShieldQuadVertices);
            } else if (player->rightHandType == PLAYER_MODELTYPE_RH_HOOKSHOT) {
                static Vec3f D_801C0DD8 = { 50.0f, 800.0f, 0.0f };

                Matrix_MultVec3f(&D_801C0DD8, &player->rightHandWorld.pos);

                if (heldActor != NULL) {
                    static Vec3f D_801C0DE4 = { 50.0f, 850.0f, 0.0f };
                    MtxF sp1BC;

                    Matrix_MultVec3f(&D_801C0DE4, &heldActor->world.pos);
                    Matrix_RotateZYX(0, -0x4000, -0x4000, MTXMODE_APPLY);
                    Matrix_Get(&sp1BC);
                    Matrix_MtxFToYXZRot(&sp1BC, &heldActor->world.rot, false);
                    heldActor->shape.rot = heldActor->world.rot;
                    if (func_800B7128(player)) {
                        Matrix_Translate(500.0f, 300.0f, 0.0f, MTXMODE_APPLY);
                        Player_DrawHookshotReticle(play, player, 77600.0f);
                    }
                }
            } else if (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) {
                if (player->meleeWeaponAnimation == PLAYER_MWA_GORON_PUNCH_RIGHT) {
                    func_80126B8C(play, player);
                }
            }
            if ((player->getItemDrawIdPlusOne != (GID_NONE + 1)) ||
                ((func_800B7118(player) == 0) && (heldActor != NULL))) {
                if (!(player->stateFlags1 & PLAYER_STATE1_400) && (player->getItemDrawIdPlusOne != (GID_NONE + 1)) &&
                    (player->exchangeItemAction != PLAYER_IA_NONE)) {
                    Math_Vec3f_Copy(&sPlayerGetItemRefPos, &player->leftHandWorld.pos);
                } else {
                    sPlayerGetItemRefPos.x =
                        (player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].x + player->leftHandWorld.pos.x) * 0.5f;
                    sPlayerGetItemRefPos.y =
                        (player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].y + player->leftHandWorld.pos.y) * 0.5f;
                    sPlayerGetItemRefPos.z =
                        (player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].z + player->leftHandWorld.pos.z) * 0.5f;
                }

                if (player->getItemDrawIdPlusOne == (GID_NONE + 1)) {
                    Math_Vec3f_Copy(&heldActor->world.pos, &sPlayerGetItemRefPos);
                }
            }
        }
    } else if (limbIndex == PLAYER_LIMB_LEFT_FOREARM) {
        func_80126BD0(play, player, 0);
    } else if (limbIndex == PLAYER_LIMB_RIGHT_FOREARM) {
        func_80126BD0(play, player, 1);
    } else if (limbIndex == PLAYER_LIMB_TORSO) {
        if (player->transformation == PLAYER_FORM_GORON) {
            s32 temp_a0 = BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_gakkistart);
            s32 temp_v1_3 = BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_gakkiwait);

            if ((temp_a0 || temp_v1_3 || (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pg_gakkiplay)))) {
                static Gfx* D_801C0DF0[] = {
                    object_link_goron_DL_010590, object_link_goron_DL_010368, object_link_goron_DL_010140,
                    object_link_goron_DL_00FF18, object_link_goron_DL_00FCF0,
                };
                Vec3f sp178[ARRAY_COUNT(D_801C0DF0)];
                s32 i;

                OPEN_DISPS(play->state.gfxCtx);

                if (temp_v1_3) {
                    f32* var_v0 = player->unk_B10;

                    for (i = 0; i < ARRAY_COUNT(sp178); i++, var_v0++) {
                        func_80124618(D_801C0510, *var_v0, &sp178[i]);
                    }
                } else {
                    if (temp_a0 != 0) {
                        func_8012536C();
                        func_80124618(D_801C0428, player->skelAnime.curFrame, &player->unk_AF0[1]);
                    }

                    for (i = 0; i < ARRAY_COUNT(sp178); i++) {
                        Math_Vec3f_Copy(&sp178[i], &player->unk_AF0[1]);
                    }
                }
                Matrix_Push();
                Matrix_Scale(player->unk_AF0[1].x, player->unk_AF0[1].y, player->unk_AF0[1].z, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

                gSPDisplayList(POLY_OPA_DISP++, object_link_goron_DL_00FC18);

                Matrix_Pop();

                for (i = 0; i < ARRAY_COUNT(sp178); i++) {
                    Matrix_Push();
                    Matrix_Scale(sp178[i].x, sp178[i].y, sp178[i].z, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

                    gSPDisplayList(POLY_OPA_DISP++, D_801C0DF0[i]);
                    Matrix_Pop();
                }

                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
    } else if (limbIndex == PLAYER_LIMB_HEAD) {
        if (((*dList1 != NULL) && ((u32)player->currentMask != PLAYER_MASK_NONE)) &&
            (((player->transformation == PLAYER_FORM_HUMAN) &&
              ((!BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_cl_setmask)) ||
               (player->skelAnime.curFrame >= 12.0f))) ||
             ((((player->transformation != PLAYER_FORM_HUMAN) && (player->currentMask >= PLAYER_MASK_FIERCE_DEITY)) &&
               ((player->transformation + PLAYER_MASK_FIERCE_DEITY) != player->currentMask)) &&
              (player->skelAnime.curFrame >= 10.0f)))) {
            if (func_80127438(play, player, player->currentMask)) {
                s32 maskMinusOne = player->currentMask - 1;

                OPEN_DISPS(play->state.gfxCtx);

                if (player->currentMask == PLAYER_MASK_COUPLE) {
                    Player_DrawCouplesMask(play, player);
                } else if (player->currentMask == PLAYER_MASK_CIRCUS_LEADER) {
                    Player_DrawCircusLeadersMask(play, player);
                } else if (player->currentMask == PLAYER_MASK_BLAST) {
                    Player_DrawBlastMask(play, player);
                } else if (player->currentMask == PLAYER_MASK_BUNNY) {
                    Player_DrawBunnyHood(play);
                } else if (player->currentMask == PLAYER_MASK_GREAT_FAIRY) {
                    Player_DrawGreatFairysMask(play, player);
                } else if (player->currentMask >= PLAYER_MASK_FIERCE_DEITY) {
                    static Vec2f D_801C0E04[PLAYER_FORM_MAX] = {
                        { 140.0f, -130.0f }, // PLAYER_FORM_FIERCE_DEITY
                        { 0.0f, -200.0f },   // PLAYER_FORM_GORON
                        { -160.0f, 0.0f },   // PLAYER_FORM_ZORA
                        { 220.0f, -200.0f }, // PLAYER_FORM_DEKU
                        { 0.0f, 0.0f },      // PLAYER_FORM_HUMAN
                    };
                    Vec2f* temp_s0_4 = &D_801C0E04[player->transformation];

                    Matrix_Push();
                    Matrix_Translate(temp_s0_4->x, temp_s0_4->z, 0.0f, MTXMODE_APPLY);
                    Matrix_Scale(1.0f, 1.0f - player->unk_B10[3], 1.0f - player->unk_B10[2], MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

                    Matrix_Pop();

                    if (((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_cl_setmask)) &&
                         (player->skelAnime.curFrame >= 51.0f)) ||
                        (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_cl_setmaskend))) {
                        maskMinusOne += 4;
                    }
                }

                gSPDisplayList(POLY_OPA_DISP++, D_801C0B20[maskMinusOne]);

                CLOSE_DISPS(play->state.gfxCtx);
            }
        } else if (player->transformation == PLAYER_FORM_DEKU) {
            if (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_gurd)) {
                OPEN_DISPS(play->state.gfxCtx);

                func_80124618(D_801C0410, player->skelAnime.curFrame, player->unk_AF0);
                Matrix_Push();

                Matrix_Scale(player->unk_AF0[0].x, player->unk_AF0[0].y, player->unk_AF0[0].z, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, object_link_nuts_DL_00A348);

                Matrix_Pop();

                CLOSE_DISPS(play->state.gfxCtx);
            } else {
                s32 temp_v1_5 = BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_gakkistart);

                if (temp_v1_5 || (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_gakkiplay)) ||
                    (BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_dl_kokeru))) {
                    static Gfx* D_801C0E2C[] = {
                        object_link_nuts_DL_007A28, object_link_nuts_DL_0077D0, object_link_nuts_DL_007548,
                        object_link_nuts_DL_007900, object_link_nuts_DL_0076A0,
                    };
                    Vec3f spF0[ARRAY_COUNT(D_801C0E2C)];
                    s32 i;
                    f32* temp;

                    OPEN_DISPS(play->state.gfxCtx);

                    if (temp_v1_5) {
                        Vec3f spD4;

                        func_80124618(D_801C0340, player->skelAnime.curFrame, &spD4);
                        player->arr_AF0[0] = spD4.x;
                        func_80124618(D_801C0368, player->skelAnime.curFrame, spF0);

                        for (i = 0; i < ARRAY_COUNT(spF0) - 1; i++) {
                            Math_Vec3f_Copy(&spF0[i + 1], spF0);
                        }

                        temp = &player->arr_AF0[1];
                        for (i = 0; i < ARRAY_COUNT(spF0); i++, temp++) {
                            *temp = spF0[0].x;
                        }
                    } else {
                        temp = &player->arr_AF0[1];
                        for (i = 0; i < ARRAY_COUNT(spF0); i++, temp++) {
                            spF0[i].x = *temp;
                            spF0[i].y = *temp;
                            spF0[i].z = *temp;
                        }
                    }

                    Matrix_Push();
                    Matrix_Scale(player->arr_AF0[0], player->arr_AF0[0], player->arr_AF0[0], MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    gSPDisplayList(POLY_OPA_DISP++, object_link_nuts_DL_007390);

                    Matrix_Pop();

                    for (i = 0; i < ARRAY_COUNT(spF0); i++) {
                        Matrix_Push();

                        Matrix_Scale(spF0[i].x, spF0[i].y, spF0[i].z, MTXMODE_APPLY);
                        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                        //! FAKE: (yes, all of them are required)
                        // https://decomp.me/scratch/AdU3G
                        if (1) {}
                        if (1) {}
                        if (1) {}
                        if (1) {}
                        if (1) {}
                        if (1) {}
                        gSPDisplayList(POLY_OPA_DISP++, D_801C0E2C[i]);

                        Matrix_Pop();
                    }

                    CLOSE_DISPS(play->state.gfxCtx);
                }
            }
        }

        if ((player->stateFlags1 & (PLAYER_STATE1_2 | PLAYER_STATE1_100)) && (player->av2.actionVar2 != 0)) {
            static Vec3f D_801C0E40[PLAYER_FORM_MAX] = {
                { 0.0f, 0.0f, 0.0f },        // PLAYER_FORM_FIERCE_DEITY
                { -578.3f, -1100.9f, 0.0f }, // PLAYER_FORM_GORON
                { -189.5f, -594.87f, 0.0f }, // PLAYER_FORM_ZORA
                { -570.0f, -812.0f, 0.0f },  // PLAYER_FORM_DEKU
                { -230.0f, -520.0f, 0.0f },  // PLAYER_FORM_HUMAN
            };
            Vec3f* temp_s0_7 = &D_801C0E40[player->transformation];

            OPEN_DISPS(play->state.gfxCtx);

            Matrix_Push();
            AnimatedMat_DrawXlu(play, Lib_SegmentedToVirtual(gameplay_keep_Matanimheader_054F18));
            Matrix_Translate(temp_s0_7->x, temp_s0_7->y, 0.0f, MTXMODE_APPLY);
            if (player->transformation == PLAYER_FORM_ZORA) {
                Matrix_Scale(0.7f, 0.7f, 0.7f, MTXMODE_APPLY);
            }

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, (u8)player->av2.actionVar2);
            gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_054C90);

            Matrix_Pop();

            CLOSE_DISPS(play->state.gfxCtx);
        }
        if (player->actor.scale.y >= 0.0f) {
            static Vec3f sPlayerFocusHeadLimbModelPos = { 1100.0f, -700.0f, 0.0f };
            static Vec3f D_801C0E88 = { 1600.0f, -1700.0f, -70.0f }; // unused
            Actor* spA8 = NULL;

            if (player->transformation != PLAYER_FORM_DEKU) {
                Matrix_MultVec3f(&sPlayerFocusHeadLimbModelPos, &player->actor.focus.pos);
            } else {
                static Vec3f D_801C0E94 = { 1800.0f, -300.0f, 0.0f };

                Matrix_MultVec3f(&sPlayerFocusHeadLimbModelPos, &player->actor.focus.pos);
                Matrix_MultVec3f(&D_801C0E94, sPlayerCurBodyPartPos);
                if ((BEN_ANIM_EQUAL(player->skelAnime.animation, gPlayerAnim_pn_drinkend)) ||
                    (BEN_ANIM_EQUAL(player->skelAnimeUpper.animation, gPlayerAnim_pn_tamahaki)) ||
                    ((player->stateFlags3 & PLAYER_STATE3_40) && ((spA8 = player->heldActor) != NULL))) {
                    if (spA8 != NULL) {
                        static Vec3f D_801C0EA0 = { 1300.0f, -400.0f, 0.0f };
                        MtxF sp68;

                        Matrix_Push();
                        Matrix_MultVec3f(&D_801C0EA0, &spA8->world.pos);
                        Matrix_RotateZYX(0, 0x4000, 0, MTXMODE_APPLY);
                        Matrix_Get(&sp68);
                        Matrix_MtxFToYXZRot(&sp68, &spA8->world.rot, false);
                        spA8->shape.rot = spA8->world.rot;
                        Matrix_Pop();
                    }
                }
            }
        }
    } else if ((limbIndex == PLAYER_LIMB_HAT) && (player->stateFlags3 & PLAYER_STATE3_100000)) {
        Vec3f sp5C;
        Vec3f sp50;
        s32 pad;

        Matrix_MultVecX(3000.0f, &sp5C);
        Matrix_MultVecX(2300.0f, &sp50);
        if (func_80126440(play, NULL, player->meleeWeaponInfo, &sp5C, &sp50)) {
            EffectBlure_AddVertex(Effect_GetByIndex(player->meleeWeaponEffectIndex[0]), &player->meleeWeaponInfo[0].tip,
                                  &player->meleeWeaponInfo[0].base);
        }
    } else if (limbIndex == PLAYER_LIMB_RIGHT_SHIN) {
        if (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) {
            if ((player->meleeWeaponAnimation == PLAYER_MWA_ZORA_PUNCH_KICK) ||
                (player->meleeWeaponAnimation == PLAYER_MWA_ZORA_JUMPKICK_START) ||
                (player->meleeWeaponAnimation == PLAYER_MWA_ZORA_JUMPKICK_FINISH)) {
                func_8012669C(play, player, D_801C0A48, D_801C0A24);
            }
        }
    } else if (limbIndex == PLAYER_LIMB_WAIST) {
        if (player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) {
            if (player->meleeWeaponAnimation == PLAYER_MWA_GORON_PUNCH_BUTT) {
                Math_Vec3f_Copy(&player->unk_AF0[1], &player->meleeWeaponInfo[0].base);
                func_8012669C(play, player, D_801C0A90, D_801C0A6C);
            }
        }
    } else if (limbIndex == PLAYER_LIMB_SHEATH) {
        if ((*dList1 != NULL) && (player->transformation == PLAYER_FORM_HUMAN) &&
            (player->currentShield != PLAYER_SHIELD_NONE) &&
            ((player->sheathType == PLAYER_MODELTYPE_SHEATH_14) ||
             (player->sheathType == PLAYER_MODELTYPE_SHEATH_15))) {
            CosmeticEditor_DrawBackShield(play, gPlayerShields[2 * ((player->currentShield - 1) ^ 0)]);
        }

        if (player->actor.scale.y >= 0.0f) {
            if ((player->rightHandType != PLAYER_MODELTYPE_RH_SHIELD) &&
                (player->rightHandType != PLAYER_MODELTYPE_RH_FF)) {
                static Vec3f sSheathLimbModelShieldOnBackPos = { 630.0f, 100.0f, -30.0f };
                static Vec3s sSheathLimbModelShieldOnBackZyxRot = { 0, 0, 0x7FFF };

                Matrix_TranslateRotateZYX(&sSheathLimbModelShieldOnBackPos, &sSheathLimbModelShieldOnBackZyxRot);
                Matrix_Get(&player->shieldMf);
            }
        }
    } else if (player->actor.scale.y >= 0.0f) {
        Player_SetFeetPos(play, player, limbIndex);
    }

    GameInteractor_ExecuteOnPlayerPostLimbDraw(player, limbIndex);

    func_8012536C();
}

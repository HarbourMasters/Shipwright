#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/Rando/StaticData/StaticData.h"
#include "2s2h/ShipInit.hpp"
#include "assets/2s2h_assets.h"

extern "C" {
#include "variables.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"

s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
void Player_SetAction_PreserveMoveFlags(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 arg3);
void Player_StopCutscene(Player* player);
void func_80848294(PlayState* play, Player* player);
Gfx* EnBox_SetRenderMode1(GraphicsContext* gfxCtx);
Gfx* EnBox_SetRenderMode2(GraphicsContext* gfxCtx);
Gfx* EnBox_SetRenderMode3(GraphicsContext* gfxCtx);
void EnBox_Draw(Actor* actor, PlayState* play);
void Player_DrawStrayFairyParticles(PlayState* play, Vec3f* arg1);
Gfx* ResourceMgr_LoadGfxByName(const char* path);
}

static Gfx gBoxChestBaseCopyDL[42];
static Gfx gBoxChestLidCopyDL[48];
static Gfx gBoxChestBaseOrnateCopyDL[41];
static Gfx gBoxChestLidOrnateCopyDL[38];

#define ENBOX_RC (actor->home.rot.x)
#define ENBOX_SET_ITEM(thisx, newItem) ((thisx)->params = (((thisx)->params & ~(0x7F << 5)) | ((newItem & 0x7F) << 5)))

std::vector<std::vector<RandoCheckId>> treasureGameMap = {
    { RC_UNKNOWN, RC_UNKNOWN }, // FD
    { RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_GORON, RC_UNKNOWN },
    { RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_ZORA, RC_UNKNOWN },
    { RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_DEKU, RC_UNKNOWN },
    { RC_CLOCK_TOWN_EAST_TREASURE_CHEST_GAME_HUMAN, RC_UNKNOWN },
};

void Player_Action_65_override(Player* player, PlayState* play) {
    if (PlayerAnimation_Update(play, &player->skelAnime)) {
        Player_StopCutscene(player);
        func_80848294(play, player);
    }
}

void func_80837C78_override(PlayState* play, Player* player) {
    Player_SetAction_PreserveMoveFlags(play, player, Player_Action_65_override, 0);
    player->stateFlags1 |= (PLAYER_STATE1_400 | PLAYER_STATE1_20000000);
}

void EnBox_RandoPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* actor, Gfx** gfx) {
    s32 pad;
    EnBox* enBox = (EnBox*)actor;
    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[ENBOX_RC].randoItemId, (RandoCheckId)ENBOX_RC);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;
    if (enBox->unk_1EC != 0 && actor->home.rot.z == 0) {
        actor->home.rot.z = randoItemType + 1;
    }
    if (actor->home.rot.z != 0) {
        randoItemType = (RandoItemType)(actor->home.rot.z - 1);
    }

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerOrnateTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockOrnateTex);
            break;
        case RITYPE_HEALTH:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerHealthTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockHealthTex);
            break;
        case RITYPE_LESSER:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerLesserTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockLesserTex);
            break;
        case RITYPE_MAJOR:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerMajorTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockMajorTex);
            break;
        case RITYPE_MASK:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerMaskTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockMaskTex);
            break;
        case RITYPE_SKULLTULA_TOKEN:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerSkullTokenTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockSkullTokenTex);
            break;
        case RITYPE_SMALL_KEY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerSmallKeyTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockSmallKeyTex);
            break;
        case RITYPE_STRAY_FAIRY:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerStrayFairyTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockStrayFairyTex);
            break;
        default:
            gSPSegment((*gfx)++, 0x09, (uintptr_t)gBoxChestCornerTex);
            gSPSegment((*gfx)++, 0x0A, (uintptr_t)gBoxChestLockTex);
            break;
    }

    MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx);
    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
        case RITYPE_HEALTH:
        case RITYPE_MASK:
        case RITYPE_SKULLTULA_TOKEN:
        case RITYPE_SMALL_KEY:
        case RITYPE_STRAY_FAIRY:
            if (limbIndex == OBJECT_BOX_CHEST_LIMB_01) {
                gSPDisplayList((*gfx)++, (Gfx*)gBoxChestBaseOrnateCopyDL);
            } else if (limbIndex == OBJECT_BOX_CHEST_LIMB_03) {
                gSPDisplayList((*gfx)++, (Gfx*)gBoxChestLidOrnateCopyDL);
            }
            break;
        default:
            if (limbIndex == OBJECT_BOX_CHEST_LIMB_01) {
                gSPDisplayList((*gfx)++, (Gfx*)gBoxChestBaseCopyDL);
            } else if (limbIndex == OBJECT_BOX_CHEST_LIMB_03) {
                gSPDisplayList((*gfx)++, (Gfx*)gBoxChestLidCopyDL);
            }
            break;
    }
}

void EnBox_RandoDraw(Actor* actor, PlayState* play) {
    s32 pad;
    EnBox* enBox = (EnBox*)actor;

    OPEN_DISPS(play->state.gfxCtx);

    if (enBox->unk_1F4.unk_10 != NULL) {
        enBox->unk_1F4.unk_10(&enBox->unk_1F4, play);
    }
    if (((enBox->alpha == 255) && (enBox->type != ENBOX_TYPE_BIG_INVISIBLE) &&
         (enBox->type != ENBOX_TYPE_SMALL_INVISIBLE)) ||
        (!CHECK_FLAG_ALL(enBox->dyna.actor.flags, ACTOR_FLAG_REACT_TO_LENS) &&
         ((enBox->type == ENBOX_TYPE_BIG_INVISIBLE) || (enBox->type == ENBOX_TYPE_SMALL_INVISIBLE)))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode1(play->state.gfxCtx));
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        POLY_OPA_DISP = SkelAnime_Draw(play, enBox->skelAnime.skeleton, enBox->skelAnime.jointTable, NULL,
                                       EnBox_RandoPostLimbDraw, &enBox->dyna.actor, POLY_OPA_DISP);
    } else if (enBox->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, enBox->alpha);
        if ((enBox->type == ENBOX_TYPE_BIG_INVISIBLE) || (enBox->type == ENBOX_TYPE_SMALL_INVISIBLE)) {
            gSPSegment(POLY_XLU_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode3(play->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, (uintptr_t)EnBox_SetRenderMode2(play->state.gfxCtx));
        }
        POLY_XLU_DISP = SkelAnime_Draw(play, enBox->skelAnime.skeleton, enBox->skelAnime.jointTable, NULL,
                                       EnBox_RandoPostLimbDraw, &enBox->dyna.actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// This simply prevents the player from getting an item from the chest, but still
// plays the chest opening animation and ensure the treasure chest flag is set
void Rando::ActorBehavior::InitEnBoxBehavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_CHEST, IS_RANDO, {
        EnBox* enBox = va_arg(args, EnBox*);
        Actor* actor = (Actor*)enBox;
        Player* player = GET_PLAYER(gPlayState);
        if (ENBOX_RC != RC_UNKNOWN) {
            Player_SetupWaitForPutAway(gPlayState, player, func_80837C78_override);
            *should = false;
        }
    });

    // Replace the item in the chest with a recovery heart, to prevent any other item side effects
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_BOX, IS_RANDO, [](Actor* actor, bool* should) {
        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_CHEST, ENBOX_GET_CHEST_FLAG(actor),
                                                                    gPlayState->sceneId);
        RandoCheckId randoCheckId = randoStaticCheck.randoCheckId;

        if (gPlayState->sceneId == SCENE_TAKARAYA) {
            uint8_t transformation = GET_PLAYER(gPlayState)->transformation;
            uint8_t gameNumber = Flags_GetSwitch(gPlayState, transformation) ? 1 : 0;
            randoCheckId = treasureGameMap[transformation][gameNumber];
        }

        if (randoCheckId == RC_UNKNOWN || !RANDO_SAVE_CHECKS[randoCheckId].shuffled) {
            return;
        }

        ENBOX_RC = randoCheckId;
        actor->params = ((actor->params & ~(0x7F << 5)) | ((GI_RECOVERY_HEART & 0x7F) << 5));

        if (CVarGetInteger("gRando.CSMC", 0)) {
            actor->draw = EnBox_RandoDraw;
        }
    });
}

static RegisterShipInitFunc initFunc(
    []() {
        if (gPlayState == NULL) {
            return;
        }

        Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_CHEST].first;

        while (actor != NULL) {
            if (actor->id == ACTOR_EN_BOX) {
                if (CVarGetInteger("gRando.CSMC", 0) && IS_RANDO) {
                    actor->draw = EnBox_RandoDraw;
                } else if (actor->draw == EnBox_RandoDraw) {
                    actor->draw = EnBox_Draw;
                }
            }

            actor = actor->next;
        }
    },
    { "gRando.CSMC", "IS_RANDO" });

static RegisterShipInitFunc initializeChestCopyDLs(
    []() {
        // Normal Chest
        Gfx* baseDL = ResourceMgr_LoadGfxByName(gBoxChestBaseDL);
        memcpy(gBoxChestBaseCopyDL, baseDL, sizeof(gBoxChestBaseCopyDL));
        gBoxChestBaseCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
        gBoxChestBaseCopyDL[8] = gsDPNoOp();
        gBoxChestBaseCopyDL[28] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x0A000000 | 1);
        gBoxChestBaseCopyDL[29] = gsDPNoOp();
        Gfx* lidDL = ResourceMgr_LoadGfxByName(gBoxChestLidDL);
        memcpy(gBoxChestLidCopyDL, lidDL, sizeof(gBoxChestLidCopyDL));
        gBoxChestLidCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
        gBoxChestLidCopyDL[8] = gsDPNoOp();
        gBoxChestLidCopyDL[26] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x0A000000 | 1);
        gBoxChestLidCopyDL[27] = gsDPNoOp();

        // Ornate Chest
        Gfx* baseOrnateDL = ResourceMgr_LoadGfxByName(gBoxChestBaseOrnateDL);
        memcpy(gBoxChestBaseOrnateCopyDL, baseOrnateDL, sizeof(gBoxChestBaseOrnateCopyDL));
        gBoxChestBaseOrnateCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
        gBoxChestBaseOrnateCopyDL[8] = gsDPNoOp();
        gBoxChestBaseOrnateCopyDL[25] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x0A000000 | 1);
        gBoxChestBaseOrnateCopyDL[26] = gsDPNoOp();
        Gfx* lidOrnateDL = ResourceMgr_LoadGfxByName(gBoxChestLidOrnateDL);
        memcpy(gBoxChestLidOrnateCopyDL, lidOrnateDL, sizeof(gBoxChestLidOrnateCopyDL));
        gBoxChestLidOrnateCopyDL[7] = gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, 0x09000000 | 1);
        gBoxChestLidOrnateCopyDL[8] = gsDPNoOp();
    },
    {});

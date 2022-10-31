/*
 * File: z_bg_gjyo_bridge.c
 * Overlay: ovl_Bg_Gjyo_Bridge
 * Description: Rainbow Bridge outside Ganon's Castle
 */

#include "z_bg_gjyo_bridge.h"
#include "objects/object_gjyo_objects/object_gjyo_objects.h"
#include "scenes/dungeons/ganon_tou/ganon_tou_scene.h"

#define FLAGS 0

void BgGjyoBridge_Init(Actor* thisx, GlobalContext* globalCtx);
void BgGjyoBridge_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgGjyoBridge_Update(Actor* thisx, GlobalContext* globalCtx);
void BgGjyoBridge_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808787A4(BgGjyoBridge* this, GlobalContext* globalCtx);
void BgGjyoBridge_TriggerCutscene(BgGjyoBridge* this, GlobalContext* globalCtx);
void BgGjyoBridge_SpawnBridge(BgGjyoBridge* this, GlobalContext* globalCtx);

const ActorInit Bg_Gjyo_Bridge_InitVars = {
    ACTOR_BG_GJYO_BRIDGE,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GJYO_OBJECTS,
    sizeof(BgGjyoBridge),
    (ActorFunc)BgGjyoBridge_Init,
    (ActorFunc)BgGjyoBridge_Destroy,
    (ActorFunc)BgGjyoBridge_Update,
    (ActorFunc)BgGjyoBridge_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgGjyoBridge_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;
    s32 pad;
    CollisionHeader* colHeader;

    colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gRainbowBridgeCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, thisx, colHeader);

    int bridge = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE);
    if (gSaveContext.eventChkInf[4] & 0x2000 || (gSaveContext.n64ddFlag && bridge == 0)) {
        this->actionFunc = func_808787A4;
    } else {
        this->dyna.actor.draw = NULL;
        func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = BgGjyoBridge_TriggerCutscene;
    }
}

void BgGjyoBridge_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void func_808787A4(BgGjyoBridge* this, GlobalContext* globalCtx) {
}

void LaunchBridgeCutscene(BgGjyoBridge* this, GlobalContext* globalCtx) {
    globalCtx->csCtx.segment = SEGMENTED_TO_VIRTUAL(gRainbowBridgeCs);
    gSaveContext.cutsceneTrigger = 1;
    this->actionFunc = BgGjyoBridge_SpawnBridge;
}

u8 CheckPlayerPosition(Player* player, GlobalContext* globalCtx) {
    return (player->actor.world.pos.x > -70.0f) && (player->actor.world.pos.x < 300.0f) &&
           (player->actor.world.pos.y > 1340.0f) && (player->actor.world.pos.z > 1340.0f) &&
           (player->actor.world.pos.z < 1662.0f) && !Gameplay_InCsMode(globalCtx);
}

void BgGjyoBridge_TriggerCutscene(BgGjyoBridge* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (!gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
            (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) && CheckPlayerPosition(player, globalCtx)) {
            LaunchBridgeCutscene(this, globalCtx);
        }
    } else {
        int bridge = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE);
        int bridgeStoneCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_STONE_COUNT);
        int bridgeMedallionCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT);
        int bridgeRewardCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_REWARD_COUNT);
        int bridgeDungeonCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT);
        int bridgeTokenCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_TOKEN_COUNT);

        if (CheckPlayerPosition(player, globalCtx)) {
            switch (bridge) {
                case 1:
                    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                        (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT)) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
                case 2:
                    if (CheckStoneCount() >= bridgeStoneCount) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
                case 3:
                    if (CheckMedallionCount() >= bridgeMedallionCount) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
                case 4:
                    if ((CheckMedallionCount() + CheckStoneCount()) >= bridgeRewardCount) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
                case 5:
                    if (CheckDungeonCount() >= bridgeDungeonCount) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
                case 6:
                    if (gSaveContext.inventory.gsTokens >= bridgeTokenCount) {
                        LaunchBridgeCutscene(this, globalCtx);
                    }
                    break;
            }
        }
    }
}

void BgGjyoBridge_SpawnBridge(BgGjyoBridge* this, GlobalContext* globalCtx) {
    if ((globalCtx->csCtx.state != CS_STATE_IDLE) && (globalCtx->csCtx.npcActions[2] != NULL) &&
        (globalCtx->csCtx.npcActions[2]->action == 2)) {
        this->dyna.actor.draw = BgGjyoBridge_Draw;
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        gSaveContext.eventChkInf[4] |= 0x2000;
    }
}

void BgGjyoBridge_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    this->actionFunc(this, globalCtx);
}

void BgGjyoBridge_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TexScroll(globalCtx->state.gfxCtx, globalCtx->gameplayFrames & 127,
                             globalCtx->gameplayFrames * -3 & 127, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 9,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, -globalCtx->gameplayFrames & 127, 32, 32, 1, 0,
                                globalCtx->gameplayFrames & 127, 32, 32));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gRainbowBridgeDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

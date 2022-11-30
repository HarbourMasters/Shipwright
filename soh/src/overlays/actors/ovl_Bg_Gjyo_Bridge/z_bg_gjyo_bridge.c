/*
 * File: z_bg_gjyo_bridge.c
 * Overlay: ovl_Bg_Gjyo_Bridge
 * Description: Rainbow Bridge outside Ganon's Castle
 */

#include "z_bg_gjyo_bridge.h"
#include "objects/object_gjyo_objects/object_gjyo_objects.h"
#include "scenes/dungeons/ganon_tou/ganon_tou_scene.h"

#define FLAGS 0

void BgGjyoBridge_Init(Actor* thisx, PlayState* play);
void BgGjyoBridge_Destroy(Actor* thisx, PlayState* play);
void BgGjyoBridge_Update(Actor* thisx, PlayState* play);
void BgGjyoBridge_Draw(Actor* thisx, PlayState* play);

void func_808787A4(BgGjyoBridge* this, PlayState* play);
void BgGjyoBridge_TriggerCutscene(BgGjyoBridge* this, PlayState* play);
void BgGjyoBridge_SpawnBridge(BgGjyoBridge* this, PlayState* play);

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

void BgGjyoBridge_Init(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;
    s32 pad;
    CollisionHeader* colHeader;

    colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gRainbowBridgeCol, &colHeader);

    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);

    int bridge = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE);
    if (gSaveContext.eventChkInf[4] & 0x2000 || (gSaveContext.n64ddFlag && bridge == RO_BRIDGE_ALWAYS_OPEN)) {
        this->actionFunc = func_808787A4;
    } else {
        this->dyna.actor.draw = NULL;
        func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = BgGjyoBridge_TriggerCutscene;
    }
}

void BgGjyoBridge_Destroy(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808787A4(BgGjyoBridge* this, PlayState* play) {
}

void LaunchBridgeCutscene(BgGjyoBridge* this, PlayState* play) {
    play->csCtx.segment = SEGMENTED_TO_VIRTUAL(gRainbowBridgeCs);
    gSaveContext.cutsceneTrigger = 1;
    this->actionFunc = BgGjyoBridge_SpawnBridge;
}

u8 CheckPlayerPosition(Player* player, PlayState* play) {
    return (player->actor.world.pos.x > -70.0f) && (player->actor.world.pos.x < 300.0f) &&
           (player->actor.world.pos.y > 1340.0f) && (player->actor.world.pos.z > 1340.0f) &&
           (player->actor.world.pos.z < 1662.0f) && !Play_InCsMode(play);
}

void BgGjyoBridge_TriggerCutscene(BgGjyoBridge* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!gSaveContext.n64ddFlag) {
        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
            (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) && CheckPlayerPosition(player, play)) {
            LaunchBridgeCutscene(this, play);
        }
    } else {
        int bridge = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE);
        int bridgeStoneCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_STONE_COUNT);
        int bridgeMedallionCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT);
        int bridgeRewardCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_REWARD_COUNT);
        int bridgeDungeonCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT);
        int bridgeTokenCount = Randomizer_GetSettingValue(RSK_RAINBOW_BRIDGE_TOKEN_COUNT);

        switch (bridge) {
            case RO_BRIDGE_VANILLA:
                if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                    (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT)) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
            case RO_BRIDGE_STONES:
                if (CheckStoneCount() >= bridgeStoneCount) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
            case RO_BRIDGE_MEDALLIONS:
                if (CheckMedallionCount() >= bridgeMedallionCount) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
            case RO_BRIDGE_DUNGEON_REWARDS:
                if ((CheckMedallionCount() + CheckStoneCount()) >= bridgeRewardCount) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
            case RO_BRIDGE_DUNGEONS:
                if (CheckDungeonCount() >= bridgeDungeonCount) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
            case RO_BRIDGE_TOKENS:
                if (gSaveContext.inventory.gsTokens >= bridgeTokenCount) {
                    this->actionFunc = BgGjyoBridge_SpawnBridge;
                    func_800F595C(NA_BGM_BRIDGE_TO_GANONS);
                }
                break;
        }
    }
}

void BgGjyoBridge_SpawnBridge(BgGjyoBridge* this, PlayState* play) {
    if (gSaveContext.n64ddFlag || (play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[2] != NULL) &&
        (play->csCtx.npcActions[2]->action == 2)) {
        this->dyna.actor.draw = BgGjyoBridge_Draw;
        func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
        gSaveContext.eventChkInf[4] |= 0x2000;
    }
}

void BgGjyoBridge_Update(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    this->actionFunc(this, play);
}

void BgGjyoBridge_Draw(Actor* thisx, PlayState* play) {
    BgGjyoBridge* this = (BgGjyoBridge*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 8,
               Gfx_TexScroll(play->state.gfxCtx, play->gameplayFrames & 127,
                             play->gameplayFrames * -3 & 127, 32, 32));

    gSPSegment(POLY_XLU_DISP++, 9,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, -play->gameplayFrames & 127, 32, 32, 1, 0,
                                play->gameplayFrames & 127, 32, 32));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gRainbowBridgeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

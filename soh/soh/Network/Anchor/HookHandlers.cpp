#include "Anchor.h"
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "src/overlays/actors/ovl_Bg_Bombwall/z_bg_bombwall.h"
#include "src/overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.h"
#include "src/overlays/actors/ovl_Bg_Haka_Zou/z_bg_haka_zou.h"
#include "src/overlays/actors/ovl_Bg_Hidan_Hamstep/z_bg_hidan_hamstep.h"
#include "src/overlays/actors/ovl_Bg_Hidan_Hrock/z_bg_hidan_hrock.h"
#include "src/overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"
#include "src/overlays/actors/ovl_Bg_Jya_Bombchuiwa/z_bg_jya_bombchuiwa.h"
#include "src/overlays/actors/ovl_Bg_Jya_Bombiwa/z_bg_jya_bombiwa.h"
#include "src/overlays/actors/ovl_Bg_Mizu_Bwall/z_bg_mizu_bwall.h"
#include "src/overlays/actors/ovl_Bg_Spot08_Bakudankabe/z_bg_spot08_bakudankabe.h"
#include "src/overlays/actors/ovl_Bg_Spot11_Bakudankabe/z_bg_spot11_bakudankabe.h"
#include "src/overlays/actors/ovl_Bg_Spot17_Bakudankabe/z_bg_spot17_bakudankabe.h"
#include "src/overlays/actors/ovl_Bg_Ydan_Maruta/z_bg_ydan_maruta.h"
#include "src/overlays/actors/ovl_Bg_Ydan_Sp/z_bg_ydan_sp.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
#include "src/overlays/actors/ovl_En_Si/z_en_si.h"
#include "src/overlays/actors/ovl_En_Sw/z_en_sw.h"
#include "src/overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "src/overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.h"
#include "src/overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.h"
#include "src/overlays/actors/ovl_Bg_Hidan_Dalm/z_bg_hidan_dalm.h"
#include "src/overlays/actors/ovl_Bg_Hidan_Kowarerukabe/z_bg_hidan_kowarerukabe.h"

extern PlayState* gPlayState;

void func_8086ED70(BgBombwall* bgBombwall, PlayState* play);
void BgBreakwall_Wait(BgBreakwall* bgBreakwall, PlayState* play);
void func_80883000(BgHakaZou* bgHakaZou, PlayState* play);
void func_808887C4(BgHidanHamstep* bgHidanHamstep, PlayState* play);
void func_808896B8(BgHidanHrock* bgHidanHrock, PlayState* play);
void func_8089107C(BgIceShelter* bgIceShelter, PlayState* play);
void func_808911BC(BgIceShelter* bgIceShelter);
void ObjBombiwa_Break(ObjBombiwa* objBombiwa, PlayState* play);
void ObjHamishi_Break(ObjHamishi* objHamishi, PlayState* play);
void BgJyaBombchuiwa_WaitForExplosion(BgJyaBombchuiwa* bgJyaBombchuiwa, PlayState* play);
void BgMizuBwall_Idle(BgMizuBwall* bgMizuBwall, PlayState* play);
void func_808B6BC0(BgSpot17Bakudankabe* bgSpot17Bakudankabe, PlayState* play);
void func_808BF078(BgYdanMaruta* bgYdanMaruta, PlayState* play);
void BgYdanSp_FloorWebIdle(BgYdanSp* bgYdanSp, PlayState* play);
void BgYdanSp_WallWebIdle(BgYdanSp* bgYdanSp, PlayState* play);
void BgYdanSp_BurnWeb(BgYdanSp* bgYdanSp, PlayState* play);
void EnDoor_Idle(EnDoor* enDoor, PlayState* play);
}

void Anchor::RegisterHooks() {

    // #region Hooks that are required for basic Anchor functionality

    COND_HOOK(OnSceneSpawnActors, isConnected, [&]() {
        SendPacket_UpdateClientState();

        if (IsSaveLoaded()) {
            RefreshClientActors();
        }
    });

    COND_HOOK(OnPresentFileSelect, isConnected, [&]() { SendPacket_UpdateClientState(); });

    COND_ID_HOOK(ShouldActorInit, ACTOR_PLAYER, isConnected, [&](void* actorRef, bool* should) {
        Actor* actor = (Actor*)actorRef;

        if (spawningDummyPlayerForClientId != 0) {
            SetDummyPlayerClientId(actor, spawningDummyPlayerForClientId);

            // By the time we get here, the actor was already added to the ACTORCAT_PLAYER list, so we need to move it
            Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_NPC);
            actor->id = ACTOR_EN_OE2;
            actor->category = ACTORCAT_NPC;
            actor->init = DummyPlayer_Init;
            actor->update = DummyPlayer_Update;
            actor->draw = DummyPlayer_Draw;
            actor->destroy = DummyPlayer_Destroy;
        }
    });

    COND_HOOK(OnPlayerUpdate, isConnected, [&]() {
        if (justLoadedSave) {
            justLoadedSave = false;
            SendPacket_RequestTeamState();
        }

        if (shouldRefreshActors) {
            shouldRefreshActors = false;
            RefreshClientActors();
        }

        SendPacket_PlayerUpdate();
    });

    COND_HOOK(OnGameFrameUpdate, isConnected, [&]() { ProcessIncomingPacketQueue(); });

    COND_HOOK(OnPlayerSfx, isConnected, [&](u16 sfxId) { SendPacket_PlayerSfx(sfxId); });
    COND_HOOK(OnOcarinaNote, isConnected,
              [&](uint8_t note, float modulator, int8_t bend) { SendPacket_OcarinaSfx(note, modulator, bend); });

    COND_HOOK(OnLoadGame, isConnected, [&](s16 fileNum) { justLoadedSave = true; });

    COND_HOOK(OnSaveFile, isConnected, [&](s16 fileNum, int sectionID) {
        if (sectionID == 0) {
            SendPacket_UpdateTeamState();
        }
    });

    COND_HOOK(OnFlagSet, isConnected,
              [&](s16 flagType, s16 flag) { SendPacket_SetFlag(SCENE_ID_MAX, flagType, flag); });

    COND_HOOK(OnFlagUnset, isConnected,
              [&](s16 flagType, s16 flag) { SendPacket_UnsetFlag(SCENE_ID_MAX, flagType, flag); });

    COND_HOOK(OnSceneFlagSet, isConnected,
              [&](s16 sceneNum, s16 flagType, s16 flag) { SendPacket_SetFlag(sceneNum, flagType, flag); });

    COND_HOOK(OnSceneFlagUnset, isConnected,
              [&](s16 sceneNum, s16 flagType, s16 flag) { SendPacket_UnsetFlag(sceneNum, flagType, flag); });

    COND_HOOK(OnRandoSetCheckStatus, isConnected, [&](RandomizerCheck rc, RandomizerCheckStatus status) {
        if (!isHandlingUpdateTeamState) {
            SendPacket_SetCheckStatus(rc);
        }
    });

    COND_HOOK(OnRandoSetIsSkipped, isConnected, [&](RandomizerCheck rc, bool isSkipped) {
        if (!isHandlingUpdateTeamState) {
            SendPacket_SetCheckStatus(rc);
        }
    });

    COND_HOOK(OnRandoEntranceDiscovered, isConnected,
              [&](u16 entranceIndex, u8 isReversedEntrance) { SendPacket_EntranceDiscovered(entranceIndex); });

    COND_ID_HOOK(OnBossDefeat, ACTOR_BOSS_GANON2, isConnected, [&](void* refActor) { SendPacket_GameComplete(); });

    COND_HOOK(OnItemReceive, isConnected, [&](GetItemEntry itemEntry) {
        // Handle vanilla dungeon items a bit differently
        if (itemEntry.modIndex == MOD_NONE &&
            (itemEntry.itemId >= ITEM_KEY_BOSS && itemEntry.itemId <= ITEM_KEY_SMALL)) {
            SendPacket_UpdateDungeonItems();
            return;
        }

        SendPacket_GiveItem(itemEntry.tableId, itemEntry.getItemId);
    });

    COND_HOOK(OnDungeonKeyUsed, isConnected, [&](uint16_t mapIndex) {
        // Handle vanilla dungeon items a bit differently
        SendPacket_UpdateDungeonItems();
    });

    COND_VB_SHOULD(VB_APPLY_TUNIC_COLOR, isConnected, {
        Actor* myPlayer = (Actor*)GET_PLAYER(gPlayState);
        Actor* actor = va_arg(args, Actor*);
        Color_RGB8* color = va_arg(args, Color_RGB8*);

        if (actor == myPlayer) {
            Color_RGBA8 ownColor = CVarGetColor(CVAR_REMOTE_ANCHOR("Color.Value"), { 100, 255, 100 });
            color->r = ownColor.r;
            color->g = ownColor.g;
            color->b = ownColor.b;
            return;
        }

        uint32_t clientId = Anchor::Instance->GetDummyPlayerClientId(actor);

        if (!Anchor::Instance->clients.contains(clientId)) {
            return;
        }

        AnchorClient& client = Anchor::Instance->clients[clientId];
        color->r = client.color.r;
        color->g = client.color.g;
        color->b = client.color.b;
    });

    // #endregion

    // #region Hooks that are purely to sync actor states across the clients, not super essential

    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_ITEM00, isConnected, [&](void* refActor) {
        EnItem00* actor = static_cast<EnItem00*>(refActor);

        if (Flags_GetCollectible(gPlayState, actor->collectibleFlag)) {
            Actor_Kill(&actor->actor);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_BOMBWALL, isConnected, [&](void* refActor, bool* should) {
        BgBombwall* actor = static_cast<BgBombwall*>(refActor);

        if (actor->actionFunc == func_8086ED70 && Flags_GetSwitch(gPlayState, actor->dyna.actor.params & 0x3F)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_BREAKWALL, isConnected, [&](void* refActor, bool* should) {
        BgBreakwall* actor = static_cast<BgBreakwall*>(refActor);

        if (actor->actionFunc == BgBreakwall_Wait && Flags_GetSwitch(gPlayState, actor->dyna.actor.params & 0x3F)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_HAKA_ZOU, isConnected, [&](void* refActor, bool* should) {
        BgHakaZou* actor = static_cast<BgHakaZou*>(refActor);

        if (actor->actionFunc == func_80883000 && Flags_GetSwitch(gPlayState, actor->switchFlag)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_HIDAN_HAMSTEP, isConnected, [&](void* refActor, bool* should) {
        BgHidanHamstep* actor = static_cast<BgHidanHamstep*>(refActor);

        if (actor->actionFunc == func_808887C4 && Flags_GetSwitch(gPlayState, (actor->dyna.actor.params >> 8) & 0xFF)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_HIDAN_HROCK, isConnected, [&](void* refActor, bool* should) {
        BgHidanHrock* actor = static_cast<BgHidanHrock*>(refActor);

        if (actor->actionFunc == func_808896B8 && Flags_GetSwitch(gPlayState, actor->unk_16A)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_ICE_SHELTER, isConnected, [&](void* refActor, bool* should) {
        BgIceShelter* actor = static_cast<BgIceShelter*>(refActor);

        if (actor->actionFunc == func_8089107C && Flags_GetSwitch(gPlayState, actor->dyna.actor.params & 0x3F)) {
            func_808911BC(actor);
            Audio_PlayActorSound2(&actor->dyna.actor, NA_SE_EV_ICE_MELT);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_JYA_BOMBCHUIWA, isConnected, [&](void* refActor, bool* should) {
        BgJyaBombchuiwa* actor = static_cast<BgJyaBombchuiwa*>(refActor);

        if (actor->actionFunc == BgJyaBombchuiwa_WaitForExplosion &&
            Flags_GetSwitch(gPlayState, actor->actor.params & 0x3F)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_JYA_BOMBIWA, isConnected, [&](void* refActor, bool* should) {
        BgJyaBombiwa* actor = static_cast<BgJyaBombiwa*>(refActor);

        if (Flags_GetSwitch(gPlayState, actor->dyna.actor.params & 0x3F)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_MIZU_BWALL, isConnected, [&](void* refActor, bool* should) {
        BgMizuBwall* actor = static_cast<BgMizuBwall*>(refActor);

        if (actor->actionFunc == BgMizuBwall_Idle &&
            Flags_GetSwitch(gPlayState, ((u16)actor->dyna.actor.params >> 8) & 0x3F)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_SPOT08_BAKUDANKABE, isConnected, [&](void* refActor, bool* should) {
        BgSpot08Bakudankabe* actor = static_cast<BgSpot08Bakudankabe*>(refActor);

        if (Flags_GetSwitch(gPlayState, (actor->dyna.actor.params & 0x3F))) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_SPOT11_BAKUDANKABE, isConnected, [&](void* refActor, bool* should) {
        BgSpot11Bakudankabe* actor = static_cast<BgSpot11Bakudankabe*>(refActor);

        if (Flags_GetSwitch(gPlayState, (actor->dyna.actor.params & 0x3F))) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_SPOT17_BAKUDANKABE, isConnected, [&](void* refActor, bool* should) {
        BgSpot17Bakudankabe* actor = static_cast<BgSpot17Bakudankabe*>(refActor);

        if (Flags_GetSwitch(gPlayState, (actor->dyna.actor.params & 0x3F))) {
            func_808B6BC0(actor, gPlayState);
            SoundSource_PlaySfxAtFixedWorldPos(gPlayState, &actor->dyna.actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
            Sfx_PlaySfxCentered(NA_SE_SY_CORRECT_CHIME);
            Actor_Kill(&actor->dyna.actor);
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_YDAN_MARUTA, isConnected, [&](void* refActor, bool* should) {
        BgYdanMaruta* actor = static_cast<BgYdanMaruta*>(refActor);

        if (actor->actionFunc == func_808BF078 && Flags_GetSwitch(gPlayState, actor->switchFlag)) {
            actor->collider.base.acFlags |= AC_HIT;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_BG_YDAN_SP, isConnected, [&](void* refActor, bool* should) {
        BgYdanSp* actor = static_cast<BgYdanSp*>(refActor);

        if ((actor->actionFunc == BgYdanSp_FloorWebIdle || actor->actionFunc == BgYdanSp_WallWebIdle) &&
            Flags_GetSwitch(gPlayState, actor->isDestroyedSwitchFlag)) {
            BgYdanSp_BurnWeb(actor, gPlayState);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_DOOR_SHUTTER, isConnected, [&](void* refActor, bool* should) {
        DoorShutter* actor = static_cast<DoorShutter*>(refActor);

        if (Flags_GetSwitch(gPlayState, actor->dyna.actor.params & 0x3F)) {
            DECR(actor->unk_16E);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_DOOR, isConnected, [&](void* refActor, bool* should) {
        EnDoor* actor = static_cast<EnDoor*>(refActor);

        if (actor->actionFunc == EnDoor_Idle && Flags_GetSwitch(gPlayState, actor->actor.params & 0x3F)) {
            DECR(actor->lockTimer);
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SI, isConnected, [&](void* refActor, bool* should) {
        EnSi* actor = static_cast<EnSi*>(refActor);

        if (GET_GS_FLAGS((actor->actor.params & 0x1F00) >> 8) & (actor->actor.params & 0xFF)) {
            Actor_Kill(&actor->actor);
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_SW, isConnected, [&](void* refActor, bool* should) {
        EnSw* actor = static_cast<EnSw*>(refActor);

        if (GET_GS_FLAGS((actor->actor.params & 0x1F00) >> 8) & (actor->actor.params & 0xFF)) {
            Actor_Kill(&actor->actor);
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_ITEM_B_HEART, isConnected, [&](void* refActor, bool* should) {
        ItemBHeart* actor = static_cast<ItemBHeart*>(refActor);

        if (Flags_GetCollectible(gPlayState, 0x1F)) {
            Actor_Kill(&actor->actor);
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_BOMBIWA, isConnected, [&](void* refActor, bool* should) {
        ObjBombiwa* actor = static_cast<ObjBombiwa*>(refActor);

        if (Flags_GetSwitch(gPlayState, actor->actor.params & 0x3F)) {
            ObjBombiwa_Break(actor, gPlayState);
            SoundSource_PlaySfxAtFixedWorldPos(gPlayState, &actor->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
            Actor_Kill(&actor->actor);
            *should = false;
        }
    });

    COND_ID_HOOK(ShouldActorUpdate, ACTOR_OBJ_HAMISHI, isConnected, [&](void* refActor, bool* should) {
        ObjHamishi* actor = static_cast<ObjHamishi*>(refActor);

        if (Flags_GetSwitch(gPlayState, actor->actor.params & 0x3F)) {
            ObjHamishi_Break(actor, gPlayState);
            SoundSource_PlaySfxAtFixedWorldPos(gPlayState, &actor->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
            Actor_Kill(&actor->actor);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_HAMMER_TOTEM_BREAK, isConnected, {
        BgHidanDalm* actor = va_arg(args, BgHidanDalm*);

        if (Flags_GetSwitch(gPlayState, actor->switchFlag)) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_FIRE_TEMPLE_BOMBABLE_WALL_BREAK, isConnected, {
        BgHidanKowarerukabe* actor = va_arg(args, BgHidanKowarerukabe*);

        if (Flags_GetSwitch(gPlayState, (actor->dyna.actor.params >> 8) & 0x3F)) {
            *should = true;
        }
    });

    // #endregion
}

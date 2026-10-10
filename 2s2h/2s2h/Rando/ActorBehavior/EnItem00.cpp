#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/CustomItem/CustomItem.h"

extern "C" {
#include "functions.h"
#include "variables.h"
#include "overlays/actors/ovl_Obj_Mure3/z_obj_mure3.h"
}

std::map<std::tuple<u16, u8, s16>, RandoCheckId> freestandingMap = {
    { { SCENE_HAKASHITA, 0, 0 }, RC_BENEATH_THE_GRAVEYARD_NIGHT_2_FREESTANDING_RUPEE_01 },
    { { SCENE_22DEKUCITY, 1, 17 }, RC_DEKU_PALACE_FREESTANDING_RUPEE_12 },
};

EnItem00* spawnReplacementItem(Vec3f& pos, Rando::StaticData::RandoStaticCheck& randoStaticCheck) {
    s16 itemParams = CustomItem::KILL_ON_TOUCH;
    // Freestanding PoH & HC cannot be picked up by boomerangs
    if (randoStaticCheck.randoCheckType == RCTYPE_FREESTANDING) {
        itemParams |= CustomItem::ABLE_TO_ZORA_RANG;
    }
    // Beehive contents beehive needs to be tossed to fall to the ground
    if (randoStaticCheck.randoCheckType == RCTYPE_BEEHIVE ||
        randoStaticCheck.randoCheckId == RC_TERMINA_FIELD_BIO_BABA_GROTTO) {
        itemParams |= CustomItem::TOSS_ON_SPAWN;
    }

    // If it hasn't been collected yet, spawn a dummy item
    return CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, itemParams, randoStaticCheck.randoCheckId,
        [](Actor* actor, PlayState* play) {
            auto& randoStaticCheck = Rando::StaticData::Checks[(RandoCheckId)CUSTOM_ITEM_PARAM];
            switch (randoStaticCheck.flagType) {
                case FLAG_NONE:
                    if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
                        RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].eligible = true;
                    }
                    break;
                case FLAG_CYCL_SCENE_COLLECTIBLE:
                    Flags_SetCollectible(play, randoStaticCheck.flag);
                    break;
                default:
                    break;
            }
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
}

void Rando::ActorBehavior::InitEnItem00Behavior() {
    // Identify freestanding based on the spawner's scene ID, room, and actor list index. The resulting RC is the base
    // value, with each child incrementing to it to get its own RC. The RCs must be contiguous for this to work.
    COND_VB_SHOULD(VB_OBJ_MURE3_DROP_COLLECTIBLE, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        auto it = freestandingMap.find({ gPlayState->sceneId, actor->room, GetActorListIndex(actor) });
        if (it != freestandingMap.end()) {
            s32 i = va_arg(args, s32);
            RandoCheckId randoCheckId = static_cast<RandoCheckId>(it->second + i);
            auto randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.cycleObtained) {
                ObjMure3* objMure3 = (ObjMure3*)actor;
                Vec3f spawnPos;
                spawnPos.y = objMure3->actor.world.pos.y;
                if (i < 6) { // Ring of Rupees
                    s16 yRot = objMure3->actor.world.rot.y + 0x2AAA * i;
                    spawnPos.x = (Math_SinS(yRot) * 40.0f) + objMure3->actor.world.pos.x;
                    spawnPos.z = (Math_CosS(yRot) * 40.0f) + objMure3->actor.world.pos.z;
                    objMure3->unk148[i] = spawnReplacementItem(spawnPos, randoStaticCheck);
                    objMure3->unk148[i]->actor.room = actor->room;
                } else { // Center Rupee
                    spawnPos.x = objMure3->actor.world.pos.x;
                    spawnPos.z = objMure3->actor.world.pos.z;
                    objMure3->unk160 = (Actor*)spawnReplacementItem(spawnPos, randoStaticCheck);
                    objMure3->unk160->room = actor->room;
                }
                *should = false;
            }
        }
    });

    // For freestandings that are identifiable based on collectible flags, pre-empt their normal spawns and spawn a
    // custom item in their places.
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_ITEM00, IS_RANDO, [](Actor* actor, bool* should) {
        EnItem00* item00 = (EnItem00*)actor;

        // If it's one of our items ignore it
        if (item00->actor.params == ITEM00_NOTHING) {
            return;
        }

        auto randoStaticCheck = Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE,
                                                                    ENITEM00_GET_7F00(actor), gPlayState->sceneId);
        if (randoStaticCheck.randoCheckId == RC_UNKNOWN) {
            return;
        }

        // Pots handle their own items, ignore them
        if (randoStaticCheck.randoCheckType == RCTYPE_POT) {
            return;
        }

        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId];

        if (!randoSaveCheck.shuffled || randoSaveCheck.cycleObtained) {
            return;
        }

        // Prevent the original item from spawning
        *should = false;

        EnItem00* replacementItem = spawnReplacementItem(actor->world.pos, randoStaticCheck);

        if (randoStaticCheck.randoCheckType == RCTYPE_WONDER_ITEM) {
            replacementItem->actor.draw = Rando::ActorBehavior::DrawWonderItemSparkle;
        }
    });
}

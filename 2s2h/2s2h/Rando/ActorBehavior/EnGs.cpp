#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/ShipUtils.h"
#include "2s2h/CustomMessage/CustomMessage.h"

#include <vector>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Gs/z_en_gs.h"
}

#define FIRST_GS_MESSAGE 0x20D1
#define SECOND_GS_MESSAGE 0x20C0
#define MOON_GS_MASK_MESSAGE 0x20D4

std::vector<std::string> flavorText = {
    "Good luck on your journey ...",
    "I hope you find what you're looking for ...",
    "... Evil is afoot",
    "Beware the moon's gaze",
    " .. It's dangerous to go alone",
};

// clang-format off
std::unordered_map<RandoItemId, u32> riToWeight = {
    { RI_SOUL_BOSS_MAJORA, 13 },
    { RI_MASK_DEKU, 12 },
    { RI_MASK_GORON, 12 },
    { RI_MASK_ZORA, 12 },
    { RI_MASK_BLAST, 11 },
    { RI_MASK_FIERCE_DEITY, 11 },
    { RI_SOUL_BOSS_GOHT, 10 },
    { RI_SOUL_BOSS_GYORG, 10 },
    { RI_SOUL_BOSS_ODOLWA, 10 },
    { RI_SOUL_BOSS_TWINMOLD, 10 },
    { RI_REMAINS_GOHT, 10 },
    { RI_REMAINS_GYORG, 10 },
    { RI_REMAINS_ODOLWA, 10 },
    { RI_REMAINS_TWINMOLD, 10 },
};

std::unordered_map<RandoCheckId, u32> rcToWeight = {
    { RC_PINNACLE_ROCK_REUNITE_SEAHORSE, 10 },
    { RC_GREAT_BAY_COAST_NEW_WAVE_BOSSA_NOVA, 10 },
    { RC_MOUNTAIN_VILLAGE_FROG_CHOIR, 10 },
    { RC_STOCK_POT_INN_COUPLES_MASK, 10 },
    { RC_ROMANI_RANCH_ALIENS, 10 },
    { RC_WATERFALL_RAPIDS_BEAVER_RACE_01, 8 },
    { RC_WATERFALL_RAPIDS_BEAVER_RACE_02, 8 },
    { RC_KEATON_QUIZ, 8 },
    { RC_CURIOSITY_SHOP_SPECIAL_ITEM, 8 },
    { RC_DEKU_PLAYGROUND_ALL_DAYS, 8 },
    { RC_MOON_TRIAL_ZORA_PIECE_OF_HEART, 6 },
    { RC_MOON_TRIAL_DEKU_PIECE_OF_HEART, 6 },
    { RC_MOON_TRIAL_GORON_PIECE_OF_HEART, 6 },
};

std::unordered_map<RandoItemType, u32> itemTypeToWeight = {
    { RITYPE_MAJOR, 9 },
    { RITYPE_MASK, 9 },
    { RITYPE_BOSS_KEY, 8 },
    { RITYPE_LESSER, 6 },
    { RITYPE_SMALL_KEY, 5 },
    { RITYPE_SKULLTULA_TOKEN, 3 },
    { RITYPE_STRAY_FAIRY, 3 },
    { RITYPE_HEALTH, 2 },
    { RITYPE_JUNK, 2 },
};
// clang-format on

s32 GetNormalizedCost() {
    s32 obtainedChecks = 0;
    s32 maxChecks = 0;
    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled) {
            maxChecks++;
            if (saveCheck.obtained) {
                obtainedChecks++;
            }
        }
    }

    return MAX(10, MIN(250, 10 + (obtainedChecks * (250 - 10)) / (maxChecks)));
}

EnGs* GetGossipStone() {
    Player* player = GET_PLAYER(gPlayState);
    if (player->talkActor == nullptr || player->talkActor->id != ACTOR_EN_GS) {
        return nullptr;
    }
    return (EnGs*)player->talkActor;
}

bool ShouldHintMoonMask(EnGs* enGs) {
    return RANDO_SAVE_OPTIONS[RO_HINTS_MOON_GOSSIP_STONES] && enGs != nullptr && enGs->actor.params == ENGS_0 &&
           enGs->unk_195 >= 1 && enGs->unk_195 <= 20; // 20 moon gossip stones
}

RandoCheckId GetRandomCheck(bool repeatableOnlyObtained = false) {
    EnGs* enGs = GetGossipStone();
    if (enGs == nullptr) {
        return RC_UNKNOWN;
    }

    u32 strength = RANDO_SAVE_OPTIONS[RO_HINTS_GOSSIP_STONE_STRENGTH];

    std::vector<std::pair<RandoCheckId, u32>> weightedChecks;
    u32 totalWeight = 0;

    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        auto& item = Rando::StaticData::Items[saveCheck.randoItemId];
        if (!saveCheck.shuffled || item.randoItemType == RITYPE_JUNK ||
            (repeatableOnlyObtained && saveCheck.obtained)) {
            continue;
        }

        u32 baseWeight = 1;
        if (rcToWeight.contains(randoCheckId)) {
            baseWeight = rcToWeight[randoCheckId];
        } else if (riToWeight.contains(saveCheck.randoItemId)) {
            baseWeight = riToWeight[saveCheck.randoItemId];
        } else if (itemTypeToWeight.contains(item.randoItemType)) {
            baseWeight = itemTypeToWeight[item.randoItemType];
        }

        u32 effectiveWeight = 100 + (baseWeight - 1) * strength;
        totalWeight += effectiveWeight;
        weightedChecks.push_back({ randoCheckId, totalWeight });
    }

    if (weightedChecks.empty()) {
        return RC_UNKNOWN;
    }

    if (repeatableOnlyObtained) {
        Ship_Random_Seed(gGameState->frames);
    } else {
        uint32_t seed = gPlayState->sceneId + enGs->actor.home.pos.x + enGs->actor.home.pos.z;
        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + seed);
    }

    u32 roll = Ship_Random(0, totalWeight);
    for (auto& [checkId, cumWeight] : weightedChecks) {
        if (roll < cumWeight) {
            return checkId;
        }
    }
    return weightedChecks.back().first;
}

void Rando::ActorBehavior::InitEnGsBehavior() {
    bool randomHints =
        IS_RANDO && (RANDO_SAVE_OPTIONS[RO_HINTS_GOSSIP_STONES] || RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]);
    bool moonMaskHints = IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_MOON_GOSSIP_STONES];

    COND_VB_SHOULD(VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED, randomHints, { *should = true; });

    COND_VB_SHOULD(VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED, moonMaskHints, {
        if (ShouldHintMoonMask(va_arg(args, EnGs*))) {
            *should = true;
        }
    });

    // Override the message ID so that we can control the text
    COND_VB_SHOULD(VB_GS_CONTINUE_TEXTBOX, randomHints, {
        // Moon stones have a mask hint of their own, which takes priority
        if (ShouldHintMoonMask(va_arg(args, EnGs*))) {
            return;
        }

        *should = false;
        Message_ContinueTextbox(gPlayState, SECOND_GS_MESSAGE);
    });

    COND_VB_SHOULD(VB_GS_CONTINUE_TEXTBOX, moonMaskHints, {
        if (!ShouldHintMoonMask(va_arg(args, EnGs*))) {
            return;
        }

        *should = false;
        Message_ContinueTextbox(gPlayState, MOON_GS_MASK_MESSAGE);
    });

    COND_ID_HOOK(OnOpenText, FIRST_GS_MESSAGE, randomHints, [](u16* textId, bool* loadFromMessageTable) {
        // Moon stones keep their vanilla opening line and answer with a mask hint instead
        if (ShouldHintMoonMask(GetGossipStone())) {
            return;
        }

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (RANDO_SAVE_OPTIONS[RO_HINTS_GOSSIP_STONES]) {
            RandoCheckId randoCheckId = GetRandomCheck();
            if (randoCheckId == RC_UNKNOWN) {
                return;
            }

            entry.autoFormat = false;
            auto& saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

            bool showExact = false;
            if (rcToWeight.contains(randoCheckId)) {
                showExact = true;
            }

            entry.msg = "They say %g{{item}}%w is hidden %y{{location}}%w.";

            CustomMessage::Replace(&entry.msg, "{{item}}",
                                   Rando::StaticData::GetItemName(saveCheck.randoItemId, true, randoCheckId));
            CustomMessage::Replace(&entry.msg, "{{location}}",
                                   Rando::StaticData::GetLocationNameForHint(randoCheckId, showExact));

            // Replace colors before line break calculation
            CustomMessage::ReplaceColorChars(&entry.msg);

            CustomMessage::AddLineBreaks(&entry.msg);

            if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
                entry.msg += "\x10...\x13\x12";
            }
        } else {
            entry.msg = "";
        }

        if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
            entry.msg += "Trade %r{{rupees}} Rupees%w for a hint?\x02\x11\xC2No\x11Yes";
            s32 cost = GetNormalizedCost();
            CustomMessage::Replace(&entry.msg, "{{rupees}}", std::to_string(cost));

            CustomMessage::ReplaceColorChars(&entry.msg);
        }

        CustomMessage::EnsureMessageEnd(&entry.msg);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, SECOND_GS_MESSAGE, randomHints, [](u16* textId, bool* loadFromMessageTable) {
        MessageContext* msgCtx = &gPlayState->msgCtx;
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (RANDO_SAVE_OPTIONS[RO_HINTS_PURCHASEABLE]) {
            if (msgCtx->choiceIndex == 1) {
                s32 cost = GetNormalizedCost();

                RandoCheckId randoCheckId = GetRandomCheck(true);
                if (gSaveContext.save.saveInfo.playerData.rupees < cost) {
                    entry.msg = "Foolish... You don't have enough rupees...";
                } else if (randoCheckId == RC_UNKNOWN) {
                    entry.msg = "I have no more hints for you...";
                } else {
                    RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

                    entry.msg = "Wise choice... They say %g{{item}}%w is hidden %y{{location}}%w.";

                    CustomMessage::Replace(&entry.msg, "{{item}}",
                                           Rando::StaticData::GetItemName(saveCheck.randoItemId, true, randoCheckId));
                    CustomMessage::Replace(&entry.msg, "{{location}}",
                                           Rando::StaticData::GetLocationNameForHint(randoCheckId, true));

                    gSaveContext.rupeeAccumulator -= cost;
                    cost *= 2;
                }
            } else {
                entry.msg = "Foolish... Come back later when you have more sense.";
            }
        } else {
            entry.msg = flavorText[Ship_Random(0, flavorText.size())];
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, MOON_GS_MASK_MESSAGE, moonMaskHints, [](u16* textId, bool* loadFromMessageTable) {
        EnGs* enGs = GetGossipStone();
        if (!ShouldHintMoonMask(enGs)) {
            return;
        }

        RandoItemId randoItemId = Rando::StaticData::GetItemIdFromVanillaItemId(ITEM_MASK_TRUTH + enGs->unk_195 - 1);
        RandoCheckId randoCheckId = Rando::FindItemPlacement(randoItemId);

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "They say %g{{item}}%w is hidden %y{{location}}%w...";

        CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::GetItemName(randoItemId));
        CustomMessage::Replace(&entry.msg, "{{location}}",
                               Rando::StaticData::GetLocationNameForHint(randoCheckId, false));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // Four Gossip Stone Grottos Heart Piece item grant behavior override
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        Actor* refActor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->id != ACTOR_EN_GS || *item != GI_HEART_PIECE) {
            return;
        }

        *should = false;

        refActor->parent = &player->actor;
    });
}

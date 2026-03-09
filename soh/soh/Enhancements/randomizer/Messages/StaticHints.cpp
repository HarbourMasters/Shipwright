// RANDOTODO: Consider breaking this up into the different actors
// That speak these hints, like 2Ship does with ActorBehavior

/**
 * File for registering hooks for "Static" hints, i.e. hints that
 * are always given by a specific NPC and/or for a specific item.
 */
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "z64scene.h"
#include <soh/OTRGlobals.h>

extern "C" {
extern PlayState* gPlayState;
#include <macros.h>
#include <functions.h>
#include <variables.h>
}

#define RAND_GET_HINT(rh) OTRGlobals::Instance->gRandoContext->GetHint(rh)
#define RAND_GET_ITEM_LOC(rc) OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)
#define ANY_SKULLTULA_HINTS                                                                   \
    RAND_GET_OPTION(RSK_KAK_10_SKULLS_HINT) || RAND_GET_OPTION(RSK_KAK_20_SKULLS_HINT) ||     \
        RAND_GET_OPTION(RSK_KAK_30_SKULLS_HINT) || RAND_GET_OPTION(RSK_KAK_40_SKULLS_HINT) || \
        RAND_GET_OPTION(RSK_KAK_50_SKULLS_HINT)

void BuildGanondorfHint(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD) && !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
        if (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) {
            msg = RAND_GET_HINT(RH_GANONDORF_HINT)->GetHintMessage(MF_AUTO_FORMAT, 1);
        } else {
            msg = RAND_GET_HINT(RH_GANONDORF_HINT)->GetHintMessage(MF_AUTO_FORMAT, 2);
        }
    } else {
        if (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT) {
            msg = RAND_GET_HINT(RH_GANONDORF_JOKE)->GetHintMessage(MF_AUTO_FORMAT);
        } else {
            msg = RAND_GET_HINT(RH_GANONDORF_HINT)->GetHintMessage(MF_AUTO_FORMAT, 0);
        }
    }
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildSheikMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    switch (gPlayState->sceneNum) {
        case SCENE_TEMPLE_OF_TIME:
            if (RAND_GET_OPTION(RSK_OOT_HINT) && !RAND_GET_ITEM_LOC(RC_SONG_FROM_OCARINA_OF_TIME)->HasObtained()) {
                msg = RAND_GET_HINT(RH_OOT_HINT)->GetHintMessage(MF_RAW);
            } else if (!CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_GANONS_TOWER)) {
                msg = CustomMessage(
                    "@, meet me at %gGanon's Castle%w once you obtain the %rkey to his lair%w.",
                    "@, wir treffen uns bei %gGanons Schloß%w, sobald Du den %rSchlüssel zu seinem Verlies%w hast.",
                    "Retrouve-moi au %gChâteau de Ganon%w une fois que tu auras obtenu la %rclé de son repaire%w.");
            } else {
                msg = CustomMessage("The time has come. Prepare yourself.", "Die Zeit ist gekommen.&Mach Dich bereit.",
                                    "Le moment est venu @.&Tu ferais bien de te préparer.");
            }
            break;
        case SCENE_INSIDE_GANONS_CASTLE:
            if (RAND_GET_OPTION(RSK_SHEIK_LA_HINT) && INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT) {
                msg = RAND_GET_HINT(RH_SHEIK_HINT)->GetHintMessage(MF_RAW);
            } else if (!(CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) &&
                         INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT && CUR_CAPACITY(UPG_QUIVER) >= 30 &&
                         gSaveContext.isMagicAcquired)) {
                msg = CustomMessage(
                    "You are still ill-equipped to face %rGanondorf%w."
                    "^Seek out the %cMaster Sword%w, %rsomething to hold your arrows%w, and %gmagic%w to summon the "
                    "%ylight%w.",
                    "Du bist noch nicht gewappnet um Dich %rGanondorf%w stellen zu können.^"
                    "Begib Dich auf die Suche nach dem %cMaster-Schwert%w, %retwas um Deinen Pfeilen einen Sinn zu "
                    "geben%w,^sowie %gdie Magie%w, um das %yLicht%w herauf beschwören zu können.",
                    "@, tu n'es toujours pas prêt à affronter %rGanondorf%w.^"
                    "Cherche l'%cÉpée de Légende%w, %rquelque chose pour ranger tes flèches%w et de la %gmagie%w pour "
                    "invoquer la %ylumière%w.");
            } else if (!Flags_GetEventChkInf(EVENTCHKINF_DISPELLED_GANONS_TOWER_BARRIER) &&
                       RAND_GET_OPTION(RSK_TRIAL_COUNT)) {
                msg = CustomMessage(
                    "You may have what you need to defeat %rthe Evil King%w, but the %cbarrier%w still "
                    "stands.^Complete the remaining %gtrials%w to destroy it.",
                    "Du magst das haben, was Du brauchst um %rden bösen König%w zu besiegen, aber die %cBarriere%w "
                    "steht noch.^Absolviere die verbleibenden %gPrüfungen%w um sie zu zerstören.",
                    "@, tu as peut-être ce qu'il te faut pour vaincre %rle Malin%w, mais les barrières sont toujours "
                    "actives.^Termine les épreuves restantes pour les détruire.");
            } else {
                msg = CustomMessage("If you're ready, then proceed.^Good luck.",
                                    "Wenn Du bereit bist, so schreite&voran.^Viel Glück.",
                                    "Si tu es prêt, tu peux y aller.^Bonne chance.");
            }
            break;
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildChildAltarMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_ALTAR_CHILD)->GetHintMessage();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildAdultAltarMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_ALTAR_ADULT)->GetHintMessage();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildSkulltulaPeopleMessage(uint16_t* textId, bool* loadFromMessageTable) {
    uint8_t count = 0;
    RandomizerCheck rc = RC_UNKNOWN_CHECK;
    int16_t actorParams = GET_PLAYER(gPlayState)->talkActor->params;
    if (actorParams == 1 && RAND_GET_OPTION(RSK_KAK_10_SKULLS_HINT)) {
        rc = RC_KAK_10_GOLD_SKULLTULA_REWARD;
        count = 10;
    } else if (actorParams == 2 && RAND_GET_OPTION(RSK_KAK_20_SKULLS_HINT)) {
        rc = RC_KAK_20_GOLD_SKULLTULA_REWARD;
        count = 20;
    } else if (actorParams == 3 && RAND_GET_OPTION(RSK_KAK_30_SKULLS_HINT)) {
        rc = RC_KAK_30_GOLD_SKULLTULA_REWARD;
        count = 30;
    } else if (actorParams == 4 && RAND_GET_OPTION(RSK_KAK_40_SKULLS_HINT)) {
        rc = RC_KAK_40_GOLD_SKULLTULA_REWARD;
        count = 40;
    } else if (actorParams == 5 && RAND_GET_OPTION(RSK_KAK_50_SKULLS_HINT)) {
        rc = RC_KAK_50_GOLD_SKULLTULA_REWARD;
        count = 50;
    } else {
        return;
    }
    Rando::Item& item = Rando::StaticData::RetrieveItem(RAND_GET_ITEM_LOC(rc)->GetPlacedRandomizerGet());
    CustomMessage msg = CustomMessage("Yeaaarrgh! I'm cursed!!^Please save me by destroying %y[[d]] Spiders of the "
                                      "Curse%w and I will give you my [[color]][[1]]%w!",
                                      /*german*/
                                      "Yeaaarrgh! Ich bin verflucht!^Bitte rette mich, indem Du %y[[d]] Skulltulas%w "
                                      "zerstörst und ich werde Dir dafür [[color]][[1]]%w geben!",
                                      /*french*/
                                      "Yeaaarrgh! Je suis maudit!^Détruit encore %y[[d]] Araignées de la Malédiction%w "
                                      "et j'aurai quelque chose à te donner! [[color]]([[1]])%w");
    msg.InsertNumber(count);
    msg.Replace("[[color]]", item.GetColor());
    msg.InsertNames({ item.GetName() });
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void Build100SkullsHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage("Yeaaarrgh! I'm cursed!!^Please save me by destroying %y100 Spiders of the "
                                      "Curse%w and I will give you my [[color]][[1]]%w!",
                                      /*german*/
                                      "Yeaaarrgh! Ich bin verflucht!^Bitte rette mich, indem Du %y100 Skulltulas%w "
                                      "zerstörst und ich werde Dir dafür [[color]][[1]]%w geben!",
                                      /*french*/
                                      "Yeaaarrgh! Je suis maudit!^Détruit encore %y100 Araignées de la Malédiction%w "
                                      "et j'aurai quelque chose à te donner! [[color]]([[1]])%w");
    msg.Replace("[[color]]", Rando::StaticData::RetrieveItem(
                                 RAND_GET_ITEM_LOC(RC_KAK_100_GOLD_SKULLTULA_REWARD)->GetPlacedRandomizerGet())
                                 .GetColor());
    msg.InsertNames(
        { Rando::StaticData::RetrieveItem(RAND_GET_ITEM_LOC(RC_KAK_100_GOLD_SKULLTULA_REWARD)->GetPlacedRandomizerGet())
              .GetName() });
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildDampesDiaryMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_DAMPES_DIARY)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildGregHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (gPlayState->sceneNum == SCENE_TREASURE_BOX_SHOP) {
        CustomMessage msg = RAND_GET_HINT(RH_GREG_RUPEE)->GetHintMessage(MF_AUTO_FORMAT);
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void BuildMysteriousWarpMessage() {
    CustomMessage msg = CustomMessage(
        "Warp to&%ra mysterious place?%w&" + CustomMessage::TWO_WAY_CHOICE() + "%gOK&No%w",
        "Zu&%reinem mysteriösen Ort%w?&" + CustomMessage::TWO_WAY_CHOICE() + "%gOK&No%w",
        "Se téléporter vers&%run endroit mystérieux%w?&" + CustomMessage::TWO_WAY_CHOICE() + "%rOK!&Non%w");
    msg.LoadIntoFont();
}

void BuildMinuetWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_MINUET_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildBoleroWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_BOLERO_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildSerenadeWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_SERENADE_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildRequiemWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_REQUIEM_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildNocturneWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_NOCTURNE_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildPreludeWarpMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (!RAND_GET_OPTION(RSK_WARP_SONG_HINTS)) {
        BuildMysteriousWarpMessage();
        *loadFromMessageTable = false;
        return;
    }
    CustomMessage msg = RAND_GET_HINT(RH_PRELUDE_WARP_LOC)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildFrogsHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_FROGS_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildLoachHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_LOACH_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildFishingPoleHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    if (Flags_GetRandomizerInf(RAND_INF_FISHING_POLE_FOUND)) {
        return;
    }
    CustomMessage msg = CustomMessage(
        "Sorry, but the pond is closed.&I've lost my good %rfishing pole%w...&Can't go fishing without it!",
        "Entschuldigung, aber der Teich ist zu.&Ich habe meine gute %rAngelrute%w verloren.&Ohne kann ich nicht "
        "fischen!",
        "Désolé, mais l'étang est fermé.&J'ai perdu ma bonne %rCanne à Pêche%w...&Impossible de pêcher sans elle!");
    if (RAND_GET_OPTION(RSK_FISHING_POLE_HINT)) {
        msg = msg + RAND_GET_HINT(RH_FISHING_POLE)->GetHintMessage();
    }
    if (*textId == TEXT_FISHING_POND_START_MET) {
        msg = CustomMessage("Hey, mister! I remember you!&It's been a long time!^",
                            "Hallo, mein Herr! Ich erinnere mich an Sie!&Lang ist's her!",
                            "Hé, monsieur! Je me souviens de toi!&Ça fait longtemps!") +
              msg;
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildSariaMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    if (*textId == TEXT_SARIA_SFM) {
        msg = RAND_GET_HINT(RH_SARIA_HINT)->GetHintMessage(MF_AUTO_FORMAT, 0);
    } else {
        msg = RAND_GET_HINT(RH_SARIA_HINT)->GetHintMessage(MF_AUTO_FORMAT, 1);
    }
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMidoMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_MIDO_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildBiggoronHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_BIGGORON_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildBigPoesHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_BIG_POES_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildChickensHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_CHICKENS_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMalonHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    int id;
    if (*textId == TEXT_MALON_EVERYONE_TURNING_EVIL || *textId == TEXT_MALON_I_SING_THIS_SONG) {
        id = 0;
    } else if (*textId == TEXT_MALON_HOW_IS_EPONA_DOING) {
        id = 1;
    } else if (*textId == TEXT_MALON_OBSTICLE_COURSE) {
        id = 2;
    } else if (*textId == TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED) {
        id = 3;
    } else {
        assert(!"This should not be reachable");
        return;
    }
    msg = RAND_GET_HINT(RH_MALON_HINT)->GetHintMessage(MF_AUTO_FORMAT, id);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildHorsebackArcheryMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    int id;
    if (*textId == TEXT_GF_HBA_SIGN) {
        id = 0;
    } else if (*textId == TEXT_HBA_NOT_ON_HORSE) {
        id = 1;
    } else if (*textId == TEXT_HBA_INITIAL_EXPLAINATION) {
        id = 2;
    } else if (*textId == TEXT_HBA_ALREADY_HAVE_1000) {
        id = 3;
    } else {
        assert(!"This should not be reachable");
        return;
    }
    msg = RAND_GET_HINT(RH_HBA_HINT)->GetHintMessage(MF_AUTO_FORMAT, id);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildMaskShopSignMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = RAND_GET_HINT(RH_MASK_SHOP_HINT)->GetHintMessage(MF_AUTO_FORMAT);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildBossKeyHintMessage(uint16_t* textId, bool* loadFromMessageTable) {
    RandomizerHint rh = RH_NONE;
    switch (gPlayState->sceneNum) {
        case SCENE_FOREST_TEMPLE:
            rh = RH_FOREST_BOSS_KEY_HINT;
            break;
        case SCENE_FIRE_TEMPLE:
            rh = RH_FIRE_BOSS_KEY_HINT;
            break;
        case SCENE_WATER_TEMPLE:
            rh = RH_WATER_BOSS_KEY_HINT;
            break;
        case SCENE_SHADOW_TEMPLE:
            rh = RH_SHADOW_BOSS_KEY_HINT;
            break;
        case SCENE_SPIRIT_TEMPLE:
            rh = RH_SPIRIT_BOSS_KEY_HINT;
            break;
        case SCENE_GANONS_TOWER:
            rh = RH_GANONS_BOSS_KEY_HINT;
            break;
        default:
            break;
    }
    if (rh != RH_NONE) {
        CustomMessage msg = RAND_GET_HINT(rh)->GetHintMessage(MF_AUTO_FORMAT);
        msg.LoadIntoFont();
        *loadFromMessageTable = false;
    }
}

void RegisterStaticHints() {
    // Ganondorf
    COND_ID_HOOK(OnOpenText, TEXT_GANONDORF, RAND_GET_OPTION(RSK_GANONDORF_HINT), BuildGanondorfHint);
    // Sheik
    COND_ID_HOOK(OnOpenText, TEXT_SHEIK_NEED_HOOK, IS_RANDO, BuildSheikMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SHEIK_HAVE_HOOK, IS_RANDO, BuildSheikMessage);
    // Altar
    COND_ID_HOOK(OnOpenText, TEXT_ALTAR_CHILD, IS_RANDO, BuildChildAltarMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ALTAR_ADULT, IS_RANDO, BuildAdultAltarMessage);
    // Skulltula
    COND_ID_HOOK(OnOpenText, TEXT_SKULLTULA_PEOPLE_IM_CURSED, ANY_SKULLTULA_HINTS, BuildSkulltulaPeopleMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SKULLTULA_PEOPLE_MAKE_YOU_VERY_RICH, RAND_GET_OPTION(RSK_KAK_100_SKULLS_HINT),
                 Build100SkullsHintMessage);
    // Dampe's Diary
    COND_ID_HOOK(OnOpenText, TEXT_DAMPES_DIARY, RAND_GET_OPTION(RSK_DAMPES_DIARY_HINT), BuildDampesDiaryMessage);
    // Chest Game
    COND_ID_HOOK(OnOpenText, TEXT_CHEST_GAME_PROCEED, RAND_GET_OPTION(RSK_GREG_HINT), BuildGregHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_CHEST_GAME_REAL_GAMBLER, RAND_GET_OPTION(RSK_GREG_HINT), BuildGregHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_CHEST_GAME_THANKS_A_LOT, RAND_GET_OPTION(RSK_GREG_HINT), BuildGregHintMessage);
    // Warp
    COND_ID_HOOK(OnOpenText, TEXT_WARP_MINUET_OF_FOREST, RAND_GET_OPTION(RSK_WARP_SONG_HINTS), BuildMinuetWarpMessage);
    COND_ID_HOOK(OnOpenText, TEXT_WARP_BOLERO_OF_FIRE, RAND_GET_OPTION(RSK_WARP_SONG_HINTS), BuildBoleroWarpMessage);
    COND_ID_HOOK(OnOpenText, TEXT_WARP_SERENADE_OF_WATER, RAND_GET_OPTION(RSK_WARP_SONG_HINTS),
                 BuildSerenadeWarpMessage);
    COND_ID_HOOK(OnOpenText, TEXT_WARP_REQUIEM_OF_SPIRIT, RAND_GET_OPTION(RSK_WARP_SONG_HINTS),
                 BuildRequiemWarpMessage);
    COND_ID_HOOK(OnOpenText, TEXT_WARP_NOCTURNE_OF_SHADOW, RAND_GET_OPTION(RSK_WARP_SONG_HINTS),
                 BuildNocturneWarpMessage);
    COND_ID_HOOK(OnOpenText, TEXT_WARP_PRELUDE_OF_LIGHT, RAND_GET_OPTION(RSK_WARP_SONG_HINTS), BuildPreludeWarpMessage);
    // Frogs
    COND_ID_HOOK(OnOpenText, TEXT_FROGS_UNDERWATER, RAND_GET_OPTION(RSK_FROGS_HINT), BuildFrogsHintMessage);
    // Loach
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_CLOUDY, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_TRY_ANOTHER_LURE, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_SECRETS, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_GOOD_FISHERMAN, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_DIFFERENT_POND, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_SCRATCHING, RAND_GET_OPTION(RSK_LOACH_HINT), BuildLoachHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_TRY_ANOTHER_LURE_WITH_SINKING_LURE, RAND_GET_OPTION(RSK_LOACH_HINT),
                 BuildLoachHintMessage);
    // Fishing Pole
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_POND_START, RAND_GET_OPTION(RSK_SHUFFLE_FISHING_POLE),
                 BuildFishingPoleHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_FISHING_POND_START_MET, RAND_GET_OPTION(RSK_SHUFFLE_FISHING_POLE),
                 BuildFishingPoleHintMessage);
    // Saria
    COND_ID_HOOK(OnOpenText, TEXT_SARIA_SFM, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_FACE_TO_FACE, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_FOREST_SOUNDS, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_MR_DARUNIA, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_SPIRITUAL_WATER, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_SPIRITUAL_FIRE, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_DREAD_CASTLE, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_DIFFERENT_OCARINA, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_EYES_DARKNESS_STORM, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_DESERT_GODDESS, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_TEMPLES, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_FOREST_TEMPLE, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_GLAD_NOW, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_IMPRISON_GANONDORF, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SARIAS_SONG_CHANNELING_POWER, RAND_GET_OPTION(RSK_SARIA_HINT), BuildSariaMessage);
    // Mido
    COND_ID_HOOK(OnOpenText, TEXT_MIDO_SPEAK_TO_MIDO_FIRST_TIME, RAND_GET_OPTION(RSK_MIDO_HINT), BuildMidoMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MIDO_SPEAK_TO_MIDO_AGAIN, RAND_GET_OPTION(RSK_MIDO_HINT), BuildMidoMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MIDO_HOME_AFTER_ZELDAS_LETTER, RAND_GET_OPTION(RSK_MIDO_HINT), BuildMidoMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MIDO_HOME_BEFORE_ZELDAS_LETTER, RAND_GET_OPTION(RSK_MIDO_HINT), BuildMidoMessage);
    // Biggoron
    COND_ID_HOOK(OnOpenText, TEXT_BIGGORON_BETTER_AT_SMITHING, RAND_GET_OPTION(RSK_BIGGORON_HINT),
                 BuildBiggoronHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BIGGORON_WAITING_FOR_YOU, RAND_GET_OPTION(RSK_BIGGORON_HINT),
                 BuildBiggoronHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BIGGORON_RETURN_AFTER_A_FEW_DAYS, RAND_GET_OPTION(RSK_BIGGORON_HINT),
                 BuildBiggoronHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_BIGGORON_I_MAAAADE_THISSSS, RAND_GET_OPTION(RSK_BIGGORON_HINT),
                 BuildBiggoronHintMessage);
    // Big Poes
    COND_ID_HOOK(OnOpenText, TEXT_GHOST_SHOP_EXPLAINATION, RAND_GET_OPTION(RSK_BIG_POES_HINT), BuildBigPoesHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_GHOST_SHOP_CARD_HAS_POINTS, RAND_GET_OPTION(RSK_BIG_POES_HINT),
                 BuildBigPoesHintMessage);
    // Anju
    COND_ID_HOOK(OnOpenText, TEXT_ANJU_PLEASE_BRING_MY_CUCCOS_BACK, RAND_GET_OPTION(RSK_CHICKENS_HINT),
                 BuildChickensHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ANJU_PLEASE_BRING_4_CUCCOS, RAND_GET_OPTION(RSK_CHICKENS_HINT),
                 BuildChickensHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ANJU_PLEASE_BRING_3_CUCCOS, RAND_GET_OPTION(RSK_CHICKENS_HINT),
                 BuildChickensHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ANJU_PLEASE_BRING_2_CUCCOS, RAND_GET_OPTION(RSK_CHICKENS_HINT),
                 BuildChickensHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_ANJU_PLEASE_BRING_1_CUCCO, RAND_GET_OPTION(RSK_CHICKENS_HINT),
                 BuildChickensHintMessage);
    // Malon
    COND_ID_HOOK(OnOpenText, TEXT_MALON_EVERYONE_TURNING_EVIL, RAND_GET_OPTION(RSK_MALON_HINT), BuildMalonHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_I_SING_THIS_SONG, RAND_GET_OPTION(RSK_MALON_HINT), BuildMalonHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_HOW_IS_EPONA_DOING, RAND_GET_OPTION(RSK_MALON_HINT), BuildMalonHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_OBSTICLE_COURSE, RAND_GET_OPTION(RSK_MALON_HINT), BuildMalonHintMessage);
    COND_ID_HOOK(OnOpenText, TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED, RAND_GET_OPTION(RSK_MALON_HINT),
                 BuildMalonHintMessage);
    // Horseback Archery
    COND_ID_HOOK(OnOpenText, TEXT_GF_HBA_SIGN, RAND_GET_OPTION(RSK_HBA_HINT), BuildHorsebackArcheryMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HBA_NOT_ON_HORSE, RAND_GET_OPTION(RSK_HBA_HINT), BuildHorsebackArcheryMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HBA_INITIAL_EXPLAINATION, RAND_GET_OPTION(RSK_HBA_HINT),
                 BuildHorsebackArcheryMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HBA_ALREADY_HAVE_1000, RAND_GET_OPTION(RSK_HBA_HINT), BuildHorsebackArcheryMessage);
    // Mask Shop Sign
    COND_ID_HOOK(OnOpenText, TEXT_MASK_SHOP_SIGN, RAND_GET_OPTION(RSK_MASK_SHOP_HINT), BuildMaskShopSignMessage);
    // Boss Key Hints
    COND_ID_HOOK(OnOpenText, TEXT_NEED_SPECIAL_KEY, RAND_GET_OPTION(RSK_BOSS_KEY_HINT), BuildBossKeyHintMessage);
}

static RegisterShipInitFunc initFunc(RegisterStaticHints, { "IS_RANDO" });
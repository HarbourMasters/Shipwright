#include <soh/OTRGlobals.h>

extern "C" {
extern PlayState* gPlayState;
#include <macros.h>
#include <functions.h>
#include <variables.h>
}

#define RAND_GET_OPTION(rsk) OTRGlobals::Instance->gRandoContext->GetOption(rsk)
#define RAND_GET_HINT(rh) OTRGlobals::Instance->gRandoContext->GetHint(rh)
#define RAND_GET_ITEM_LOC(rc) OTRGlobals::Instance->gRandoContext->GetItemLocation(rc)

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
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildSheikMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg;
    switch (gPlayState->sceneNum) {
        case SCENE_TEMPLE_OF_TIME:
            if (RAND_GET_OPTION(RSK_OOT_HINT) && !RAND_GET_ITEM_LOC(RC_SONG_FROM_OCARINA_OF_TIME)->HasObtained()){
                msg = RAND_GET_HINT(RH_OOT_HINT)->GetHintMessage(MF_RAW);
            } else if (!CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_GANONS_TOWER)) {
                msg = CustomMessage(
                "@, meet me at %gGanon's Castle%w once you obtain the %rkey to his lair%w.",
                "@, wir treffen uns bei %gGanons Schloß%w, sobald Du den %rSchlüssel zu seinem Verlies%w hast.",
                "Retrouve-moi au %gChâteau de Ganon%w une fois que tu auras obtenu la %rclé de son repaire%w.");
            } else {
                msg = CustomMessage(
                "The time has come. Prepare yourself.",
                "Die Zeit ist gekommen.&Mach Dich bereit.",
                "Le moment est venu @.&Tu ferais bien de te préparer.");
            }
            break;
        case SCENE_INSIDE_GANONS_CASTLE:
            if (RAND_GET_OPTION(RSK_SHEIK_LA_HINT) && INV_CONTENT(ITEM_ARROW_LIGHT) != ITEM_ARROW_LIGHT) {
                msg = RAND_GET_HINT(RH_SHEIK_HINT)->GetHintMessage(MF_RAW);
            } else if (!(CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) && INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT &&
                       CUR_CAPACITY(UPG_QUIVER) >= 30 && gSaveContext.isMagicAcquired)) {
                msg = CustomMessage("You are still ill-equipped to face %rGanondorf%w."
                    "^Seek out the %cMaster Sword%w, %rsomething to hold your arrows%w, and %gmagic%w to summon the %ylight%w.",
                    "Du bist noch nicht gewappnet um Dich %rGanondorf%w stellen zu können.^"
                    "Begib Dich auf die Suche nach dem %cMaster-Schwert%w, %retwas um Deinen Pfeilen einen Sinn zu geben%w,^sowie %gdie Magie%w, um das %yLicht%w herauf beschwören zu können.",
                    "@, tu n'es toujours pas prêt à affronter %rGanondorf%w.^"
                    "Cherche l'%cÉpée de Légende%w, %rquelque chose pour ranger tes flèches%w et de la %gmagie%w pour invoquer la %ylumière%w.");
            } else if (!Flags_GetEventChkInf(EVENTCHKINF_DISPELLED_GANONS_TOWER_BARRIER) && !RAND_GET_OPTION(RSK_TRIAL_COUNT).Is(0)){
                msg = CustomMessage(
                    "You may have what you need to defeat %rthe Evil King%w, but the %cbarrier%w still stands.^Complete the remaining %gtrials%w to destroy it.",
                    "Du magst das haben, was Du brauchst um %rden bösen König%w zu besiegen, aber die %cBarriere%w steht noch.^Absolviere die verbleibenden %gPrüfungen%w um sie zu zerstören.",
		            "@, tu as peut-être ce qu'il te faut pour vaincre %rle Malin%w, mais les barrières sont toujours actives.^Termine les épreuves restantes pour les détruire.");
            } else {
                msg = CustomMessage(
                    "If you're ready, then proceed.^Good luck.",
                    "Wenn Du bereit bist, so schreite&voran.^Viel Glück.",
                    "Si tu es prêt, tu peux y aller.^Bonne chance.");
            }
            break;
    }
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void RegisterStaticHints() {
    COND_ID_HOOK(OnOpenText, TEXT_GANONDORF, RAND_GET_OPTION(RSK_GANONDORF_HINT), BuildGanondorfHint);
    COND_ID_HOOK(OnOpenText, TEXT_SHEIK_NEED_HOOK, IS_RANDO, BuildSheikMessage);
    COND_ID_HOOK(OnOpenText, TEXT_SHEIK_HAVE_HOOK, IS_RANDO, BuildSheikMessage);
}

RegisterShipInitFunc initFunc(RegisterStaticHints, { "IS_RANDO" });
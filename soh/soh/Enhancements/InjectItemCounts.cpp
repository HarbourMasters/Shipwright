#include <soh/OTRGlobals.h>

extern "C" {
#include "variables.h"
}

void BuildSkulltulaMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessage("You got a %rGold Skulltula Token%w!&You've collected %r[[gsCount]]%w tokens&in total!",
                      "Ein %rGoldenes Skulltula-Symbol%w!&Du hast nun insgesamt %r[[gsCount]]&%wGoldene "
                      "Skulltula-Symbole&gesammelt!",
                      "Vous obtenez un %rSymbole de&Skulltula d'or%w! Vous avez&collecté %r[[gsCount]]%w symboles en "
                      "tout!",
                      TEXTBOX_TYPE_BLUE);
    // The freeze text cannot be manually dismissed and must be auto-dismissed.
    // This is fine and even wanted when skull tokens are not shuffled, but when
    // when they are shuffled we don't want to be able to manually dismiss the box.
    // Otherwise if we get a token from a chest or an NPC we get stuck in the ItemGet
    // animation until the text box auto-dismisses.
    // RANDOTODO: Implement a way to determine if an item came from a skulltula and
    // inject the auto-dismiss control code if it did.
    if (CVarGetInteger(CVAR_ENHANCEMENT("SkulltulaFreeze"), 0) != 0 &&
        !(IS_RANDO && RAND_GET_OPTION(RSK_SHUFFLE_TOKENS))) {
        // Auto dismiss textbox after 0x3C (60) frames (about 3 seconds for OoT)
        msg = msg + "\x0E\x3C";
    }
    int16_t gsCount = gSaveContext.inventory.gsTokens + (IS_RANDO ? 1 : 0);
    msg.Replace("[[gsCount]]", std::to_string(gsCount));
    msg.AutoFormat(ITEM_SKULL_TOKEN);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildHeartContainerMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg = CustomMessage(
        "You got a %rHeart Container%w!&You've collected %r[[heartContainerCount]]%w containers&in total!",
        "Ein %rHerzcontainer%w!&Du hast nun insgesamt %r[[heartContainerCount]]%w&Herzcontainer gesammelt!",
        "Vous obtenez un %rCoeur&d'Energie%w! Vous en avez&collecté %r[[heartContainerCount]]%w en tout!");
    msg.Replace("[[heartContainerCount]]", std::to_string(gSaveContext.ship.stats.heartContainers + 1));
    msg.AutoFormat(ITEM_HEART_CONTAINER);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void BuildHeartPieceMessage(uint16_t* textId, bool* loadFromMessageTable) {
    CustomMessage msg =
        CustomMessage("You got a %rHeart Piece%w!&You've collected %r[[heartPieceCount]]%w pieces&in total!",
                      "Ein %rHerzteil%w!&Du hast nun insgesamt %r[[heartPieceCount]]%w&Herzteile gesammelt!",
                      "Vous obtenez un %rQuart de&Coeur%w! Vous en avez collecté&%r[[heartPieceCount]]%w en tout!",
                      TEXTBOX_TYPE_BLUE);
    msg.Replace("[[heartPieceCount]]", std::to_string(gSaveContext.ship.stats.heartPieces + 1));
    msg.AutoFormat(ITEM_HEART_PIECE);
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

void InjectSkulltulaCounts_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_GS_FREEZE, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 0),
                 BuildSkulltulaMessage);
    COND_ID_HOOK(OnOpenText, TEXT_GS_NO_FREEZE, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"), 0),
                 BuildSkulltulaMessage);
}

void InjectHeartContainerCounts_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_HEART_CONTAINER,
                 CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"), 0), BuildHeartContainerMessage);
}

void InjectHeartPieceCounts_Register() {
    // Heart Pieces don't have documented text IDs after the first one, but
    // there are 3 more in between TEXT_HEART_PIECE and TEXT_HEART_CONTAINER.
    COND_ID_HOOK(OnOpenText, TEXT_HEART_PIECE, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 0),
                 BuildHeartPieceMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HEART_PIECE + 1, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 0),
                 BuildHeartPieceMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HEART_PIECE + 2, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 0),
                 BuildHeartPieceMessage);
    COND_ID_HOOK(OnOpenText, TEXT_HEART_PIECE + 3, CVarGetInteger(CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"), 0),
                 BuildHeartPieceMessage);
}

static RegisterShipInitFunc initInjectSkulltulaCountFunc(InjectSkulltulaCounts_Register,
                                                         { CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula") });
static RegisterShipInitFunc initInjectHeartContainerCountFunc(InjectHeartContainerCounts_Register,
                                                              { CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer") });
static RegisterShipInitFunc initInjectHeartPieceCountFunc(InjectHeartPieceCounts_Register,
                                                          { CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece") });

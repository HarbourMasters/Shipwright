#include "randomizer.h"
#include "soh/Enhancements/custom_message/CustomMessage.h"
#include <message_data_static.h>

#define NOGDI
#define WIN32_LEAN_AND_MEAN

void Randomizer::CreateCustomMessages() {
    CustomMessage* customMessage = CustomMessage::Instance;
    customMessage->AddCustomMessageTable(Randomizer::customMessageTableID);
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rBig Poe in a Bottle%w!&Sell it to the Ghost Shop!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_FISH, ITEM_FISH,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rFish in a Bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_BUGS, ITEM_BUG,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, "You got a %rBug in a Bottle%w!&They love to burrow in&dirt holes!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_FAIRY, ITEM_FAIRY,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, "You got a %rFairy in a Bottle%w!&Use it wisely!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_RED_POTION, ITEM_POTION_RED,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, "You got a %rBottle of Red Potion%w!&Drink it to replenish your&%ghealth%w!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_GREEN_POTION, ITEM_POTION_GREEN,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rBottle of Green Potion%w!&Drink it to replenish your&%bmagic%w!", "", "" });
    customMessage->CreateGetItemMessage(
        Randomizer::customMessageTableID, GI_BOTTLE_WITH_POE, ITEM_POE,
        { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rPoe in a Bottle%w!&That creepy Ghost Shop might&be interested in this...", "", "" });
}

//std::string Randomizer::GetCustomGetItemMessage(GlobalContext* globalCtx, GetItemID giid) {
//    if (!gSaveContext.n64ddFlag) {
//        return "Not Randomized.";
//    }
//
//    return CustomMessage::Instance->RetrieveMessage(globalCtx, Randomizer::customMessageTableID, giid);
//}
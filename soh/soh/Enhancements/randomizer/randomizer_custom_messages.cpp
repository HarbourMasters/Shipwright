#include "randomizer.h"
#include "soh/Enhancements/custom_message/CustomMessage.h"

#define MESSAGES(eng, ger, fra) (new std::string[]{eng, ger, fra})

void Randomizer::CreateCustomMessages() {
    CustomMessage* customMessage = CustomMessage::Instance;
    customMessage->CreateGetItemMessage(
        GI_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE,
        MESSAGES("You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!", "", ""));
    customMessage->CreateGetItemMessage(
        GI_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
        MESSAGES("You got a %rBig Poe in a bottle%w!&Sell it to the Ghost Shop!", "", ""));
    customMessage->CreateGetItemMessage(
        GI_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
        MESSAGES("You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!", "", ""));
    customMessage->CreateGetItemMessage(
        GI_BOTTLE_WITH_FISH, ITEM_FISH,
        MESSAGES("You got a %rFish in a bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!", "",
                 ""));


}

std::string Randomizer::GetCustomGetItemMessage(GetItemID giid) {
    if (!gSaveContext.n64ddFlag) {
        return "Not Randomized.";
    }

    return CustomMessage::Instance->RetrieveGetItemMessage(giid);
}
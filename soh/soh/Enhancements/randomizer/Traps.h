#pragma once

#ifndef __cplusplus
#error This header should not be used in C files
#endif

#include "soh/Enhancements/custom-message/text.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"

class CustomMessage;

namespace Rando {
namespace Traps {
/// @brief A fake item name for an ice trap, with the article to use when it appears in a sentence
struct TrickName {
    Text name;
    Text article;
};
TrickName GetTrapName(RandomizerGet id, RandoIceTrapNames iceTrapNamesOption, uint64_t* state = nullptr);
RandomizerGet GetTrapTrickModel(uint64_t* state = nullptr);
bool ShouldJunkItemBeTrap();
void BuildIceTrapMessage(CustomMessage& msg, GetItemEntry getItemEntry);
} // namespace Traps
} // namespace Rando
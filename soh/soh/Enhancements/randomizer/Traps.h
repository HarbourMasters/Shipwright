#pragma once

#ifndef __cplusplus
#error This header should not be used in C files
#endif

#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/3drando/text.hpp"
#include "libultraship/libultra/types.h"

namespace Rando {
namespace Traps {
Text GetTrapName(uint16_t id);
RandomizerGet GetTrapTrickModel();
bool ShouldJunkItemBeTrap();
void BuildIceTrapMessage(CustomMessage& msg, GetItemEntry getItemEntry);
} // namespace Traps
} // namespace Rando
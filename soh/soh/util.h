#pragma once
#include <string>
#include <stdint.h>

namespace SohUtils {
    int32_t GetItemIdIconFromRandomizerGet(int32_t randomizerGet);

    const char* GetIconNameFromItemID(int32_t itemId);

    const std::string& GetSceneName(int32_t scene);

    const std::string& GetItemName(int32_t item);

    const std::string& GetRandomizerItemName(int32_t item);

    const std::string& GetQuestItemName(int32_t item);
} // namespace SohUtils

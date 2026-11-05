#include "MessageCatalog.h"
#include "UnicodeMessage.h"
#include "soh/Enhancements/Lang/GameLanguage.h"
#include "soh/Enhancements/Lang/MessageLanguage.h"
#include <algorithm>
#include <cstring>

extern "C" size_t GetEquipNowMessage(char* buffer, char* src, const size_t maxBufferSize) {
    if (maxBufferSize == 0)
        return 0;
    CustomMessage customMessage = CustomMessages::CreateMessage("enhancements.equip_now.prompt");
    customMessage.Format();

    std::string postfix = customMessage.GetForCurrentLanguage();
    const std::string locale = GameLanguage_GetMessageLocale();
    if (locale != "en_US" && locale != "de_DE" && locale != "fr_FR" && customMessage.HasLocale(locale)) {
        MessageLanguage_SetUnicode(GameLanguage_Get());
        const auto alignment = customMessage.GetAlignment(locale);
        MessageLanguage_SetAlignment(alignment == MESSAGE_ALIGNMENT_AUTO ? MESSAGE_ALIGNMENT_LEFT : alignment);
    }
    std::string str;
    std::string FixedBaseStr(src);
    size_t RemoveControlChar = FixedBaseStr.find_first_of("\x02");

    if (RemoveControlChar != std::string::npos) {
        FixedBaseStr = FixedBaseStr.substr(0, RemoveControlChar);
    }
    str = FixedBaseStr + postfix;
    if (MessageLanguage_UsesUnicode()) {
        str = UnicodeMessage::Format(str, locale == "ja_JP");
        if (str.size() >= maxBufferSize) {
            // Keep complete choice labels if an unusually long translation does not fit.
            MessageLanguage_Set(GameLanguage_GetLegacyText());
            str = FixedBaseStr + customMessage.GetForLanguage(GameLanguage_GetLegacyText());
        }
    }

    if (!str.empty()) {
        memset(buffer, 0, maxBufferSize);
        const size_t copiedCharLen = std::min<size_t>(maxBufferSize - 1, str.length());
        memcpy(buffer, str.c_str(), copiedCharLen);
        return copiedCharLen;
    }
    return 0;
}

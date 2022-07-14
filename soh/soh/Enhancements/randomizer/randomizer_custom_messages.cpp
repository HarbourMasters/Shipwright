#include "randomizer_custom_messages.h"
#include "randomizer.h"
#include <variables.h>

using namespace std::literals::string_literals;

std::string Randomizer::GetCustomGetItemMessage(GetItemID giid) {
    if (!gSaveContext.n64ddFlag) {
        return "Not Randomized.";
    }

    switch (giid) {
        default:
            switch (gSaveContext.language) { 
                case LANGUAGE_FRA:
                    return "Il n'y a pas de message personnalisé pour cet élément.";
                case LANGUAGE_GER:
                    return "Für diesen Artikel gibt es keine benutzerdefinierte Nachricht.";
                case LANGUAGE_ENG:
                default:
                    return "There is no custom message for this item.";
            }
    }
}

std::string MESSAGE_END() {
    return "\x02"s;
}

std::string ITEM_OBTAINED(uint8_t x) {
    return "\x13"s + char(x);
}

std::string NEWLINE() {
    return "\x01"s;
}

std::string COLOR(uint8_t x) {
    return "\x05"s + char(x);
}

std::string WAIT_FOR_INPUT() {
    return "\x04"s;
}
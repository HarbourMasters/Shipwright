#pragma once

#include <string>
#include <utility>
#include <vector>
#include <cstdint>

#include "text.hpp"

#define QM_WHITE  0x00
#define QM_RED    0x41
#define QM_GREEN  0x42
#define QM_BLUE   0x43
#define QM_LBLUE  0x44
#define QM_PINK   0x45
#define QM_YELLOW 0x46
#define QM_BLACK  0x47

namespace CustomMessages {
typedef struct {
    // In the true file format, offset is the offset into the QM file.
    // In randomizer, offset will be a pointer to the text in the game's address space.
    // Since these pointers will be much larger as u32 than the original script's offsets,
    // We will be able to distinguish between original and custom text using their numerical value.
    const char* offset;
    uint32_t length;
} MessageLanguageInfo;

typedef enum {
    /* 0x00 */ JAPANESE_J,
    /* 0x01 */ ENGLISH_U,
    /* 0x02 */ ENGLISH_E,
    /* 0x03 */ GERMAN_E,
    /* 0x04 */ FRENCH_E,
    /* 0x05 */ FRENCH_U,
    /* 0x06 */ SPANISH_E,
    /* 0x07 */ SPANISH_U,
    /* 0x08 */ ITALIAN_E,
    /* 0x09 */ DUTCH_E,
} MessageLanguage;

typedef struct {
    uint32_t id;
    uint32_t unk_04;
    uint32_t unk_08;
    uint32_t unk_0C;
    MessageLanguageInfo info[10];
} MessageEntry; // size = 0x60

typedef struct {
    char magic[4]; //"QM\0\0"
    uint32_t unk_04;
    uint32_t numEntries;
    uint32_t unk_0C;
} MessageFileHeader;

    void CreateMessage(uint32_t textId, uint32_t unk_04, uint32_t textBoxType, uint32_t textBoxPosition,
                       std::string englishText, std::string frenchText, std::string spanishText);
    void CreateMessageFromTextObject(uint32_t textId, uint32_t unk_04, uint32_t textBoxType, uint32_t textBoxPosition, const Text& text);

    uint32_t NumMessages();

    std::pair<const char*, uint32_t> RawMessageEntryData();
    std::pair<const char*, uint32_t> RawMessageData();

    void CreateAlwaysIncludedMessages();
    Text AddColorsAndFormat(Text text, const std::vector<uint8_t>& colors = {});
    void ClearMessages();

    std::string MESSAGE_END();
    std::string WAIT_FOR_INPUT();
    std::string HORIZONTAL_SPACE(uint8_t x);
    std::string GO_TO(uint16_t x);
    std::string INSTANT_TEXT_ON();
    std::string INSTANT_TEXT_OFF();
    std::string SHOP_MESSAGE_BOX();
    std::string EVENT_TRIGGER();
    std::string DELAY_FRAMES(uint8_t x);
    std::string CLOSE_AFTER(uint8_t x);
    std::string PLAYER_NAME();
    std::string PLAY_OCARINA();
    std::string ITEM_OBTAINED(uint8_t x);
    std::string SET_SPEED(uint8_t x);
    std::string SKULLTULAS_DESTROYED();
    std::string CURRENT_TIME();
    std::string UNSKIPPABLE();
    std::string TWO_WAY_CHOICE();
    std::string NEWLINE();
    std::string COLOR(uint8_t x);
    std::string CENTER_TEXT();
    std::string IF_NOT_MQ();
    std::string MQ_ELSE();
    std::string MQ_END();
}

#pragma once

typedef enum {
    TEXT_GS_NO_FREEZE = 0xB4,
    TEXT_GS_FREEZE = 0xB5,
    TEXT_RANDOMIZER_CUSTOM_ITEM = 0xF8,
    TEXT_SCRUB_POH = 0x10A2,
    TEXT_SCRUB_STICK_UPGRADE = 0x10DC,
    TEXT_SCRUB_NUT_UPGRADE = 0x10DD,
    TEXT_RANDOMIZER_GOSSIP_STONE_HINTS = 0x2053,
    TEXT_ALTAR_CHILD = 0x7040,
    TEXT_ALTAR_ADULT = 0x7088,
    TEXT_GANONDORF = 0x70CC,
    TEXT_GANONDORF_NOHINT = 0x70CD
} TextIDs;

#ifdef __cplusplus

typedef struct {
    GetItemID giid;
    ItemID iid;
    std::string english;
    std::string german;
    std::string french;
} GetItemMessage;

#define GIMESSAGE(giid, iid, english, german, french) \
    { giid, iid, english, german, french }

#define GIMESSAGE_UNTRANSLATED(giid, iid, message) \
    { giid, iid, message, message, message }

#endif

#pragma once

typedef enum {
    TEXT_BUY_BOMBCHU_10_PROMPT = 0x8C,
    TEXT_BUY_BOMBCHU_10_DESC = 0xBC,
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
    TEXT_GANONDORF_NOHINT = 0x70CD,
    TEXT_BLUE_RUPEE = 0xCC,
    TEXT_RED_RUPEE = 0xF0,
    TEXT_PURPLE_RUPEE = 0xF1,
    TEXT_HUGE_RUPEE = 0xF2,
    TEXT_BEAN_SALESMAN = 0x405E,
    TEXT_SCRUB_RANDOM = 0x9000,
    TEXT_SCRUB_RANDOM_FREE = 0x9001,
    TEXT_SHOP_ITEM_RANDOM = 0x9100,
    TEXT_SHOP_ITEM_RANDOM_CONFIRM = 0x9101,
} TextIDs;

#ifdef __cplusplus

typedef struct {
    u16 giid;
    ItemID iid;
    std::string english;
    std::string german;
    std::string french;
} GetItemMessage;

#define GIMESSAGE(giid, iid, english, german, french) \
    { giid, iid, english, german, french }

#define GIMESSAGE_UNTRANSLATED(giid, iid, message) \
    { giid, iid, message, message, message }

#define GIMESSAGE_NO_GERMAN(giid, iid, english, french) \
    { giid, iid, english, english, french  }

#endif

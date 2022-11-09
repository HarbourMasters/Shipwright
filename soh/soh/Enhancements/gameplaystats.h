#pragma once

void InitStatTracker();

typedef enum {
    // 0x00 to 0x9B (0 to 155) used for getting items,
    // see struct "ItemID" in z64item.h

    /* 0xA0 */ TIMESTAMP_DEFEAT_GOHMA = 0xA0,
    /* 0xA1 */ TIMESTAMP_DEFEAT_KING_DODONGO,
    /* 0xA2 */ TIMESTAMP_DEFEAT_BARINADE,
    /* 0xA3 */ TIMESTAMP_DEFEAT_PHANTOM_GANON,
    /* 0xA4 */ TIMESTAMP_DEFEAT_VOLVAGIA,
    /* 0xA5 */ TIMESTAMP_DEFEAT_MORPHA,
    /* 0xA6 */ TIMESTAMP_DEFEAT_BONGO_BONGO,
    /* 0xA7 */ TIMESTAMP_DEFEAT_TWINROVA,
    /* 0xA8 */ TIMESTAMP_DEFEAT_GANONDORF,
    /* 0xA9 */ TIMESTAMP_DEFEAT_GANON,
    /* 0xAA */ TIMESTAMP_MAX

}GameplayStatEvent;
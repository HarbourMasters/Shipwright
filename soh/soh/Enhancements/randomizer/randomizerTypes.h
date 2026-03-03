#pragma once

#include <stdint.h>
#include "z64item.h"
#include "randomizerEnums.h"

#define MAX_TRICK_NAME_SIZE 50

#define TWO_ACTOR_PARAMS(a, b) ((((a)&0xFFFF) << 16) | ((b)&0xFFFF))

typedef bool (*ConditionFn)();

typedef struct {
    char tex[512];
    uint16_t width;
    uint16_t height;
    uint8_t im_fmt;
    uint8_t im_siz;
    uint8_t id;
} Sprite;

typedef struct {
    RandomizerGet rgID;
    RandomizerGet fakeRgID;
    char trickName[MAX_TRICK_NAME_SIZE];
} RandomizerGetData;

typedef struct CheckIdentity {
    RandomizerInf randomizerInf;
    RandomizerCheck randomizerCheck;
} CheckIdentity;

typedef struct ScrubIdentity {
    CheckIdentity identity;
    GetItemID getItemId;
    int32_t itemPrice;
} ScrubIdentity;

typedef struct ShopItemIdentity {
    CheckIdentity identity;
    GetItemID ogItemId;
    int32_t enGirlAShopItem;
    int32_t itemPrice;
} ShopItemIdentity;

// RANDOTODO compare child long jumpslash range with adult short
#define ENTRANCE_GROTTO_LOAD_START 0x0700
#define ENTRANCE_GROTTO_EXIT_START 0x0800

#define ENTRANCE_GROTTO_LOAD(index) ENTRANCE_GROTTO_LOAD_START + index
#define ENTRANCE_GROTTO_EXIT(index) ENTRANCE_GROTTO_EXIT_START + index

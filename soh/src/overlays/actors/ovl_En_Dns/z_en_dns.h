#ifndef Z_EN_DNS_H
#define Z_EN_DNS_H

#include <libultraship/libultra.h>
#include "global.h"

#define DNS_GET_TYPE(thisx) ((thisx)->params)

typedef enum EnDnsType {
    /*  0 */ DNS_TYPE_DEKU_NUTS_5,
    /*  1 */ DNS_TYPE_DEKU_STICKS_1,
    /*  2 */ DNS_TYPE_HEART_PIECE,
    /*  3 */ DNS_TYPE_DEKU_SEEDS_30,
    /*  4 */ DNS_TYPE_DEKU_SHIELD,
    /*  5 */ DNS_TYPE_BOMBS_5,
    /*  6 */ DNS_TYPE_ARROWS_30,
    /*  7 */ DNS_TYPE_RED_POTION,
    /*  8 */ DNS_TYPE_GREEN_POTION,
    /*  9 */ DNS_TYPE_DEKU_STICK_UPGRADE,
    /* 10 */ DNS_TYPE_DEKU_NUT_UPGRADE
} EnDnsType;

typedef enum EnDnsCanBuyResult {
    /*  0 */ DNS_CANBUY_RESULT_NEED_RUPEES,
    /*  1 */ DNS_CANBUY_RESULT_CAPACITY_FULL,
    /*  2 */ DNS_CANBUY_RESULT_SUCCESS_NEW_ITEM,
    /*  3 */ DNS_CANBUY_RESULT_CANT_GET_NOW,
    /*  4 */ DNS_CANBUY_RESULT_SUCCESS
} EnDnsCanBuyResult;

typedef enum EnDnsAnimation {
    /*  0 */ DNS_ANIM_IDLE,
    /*  1 */ DNS_ANIM_BURROW,
    /*  2 */ DNS_ANIM_IDLE_TRANSITION
} EnDnsAnimation;

struct EnDns;

typedef void (*EnDnsActionFunc)(struct EnDns*, PlayState*);
typedef u32 (*EnDnsPurchaseableCheck)(struct EnDns*);
typedef void (*EnDnsSetRupeesAndFlags)(struct EnDns*);

typedef struct {
    /* 0x00 */ s16 itemPrice;
    /* 0x02 */ u16 itemAmount;
    /* 0x04 */ s32 getItemId;
    /* 0x08 */ EnDnsPurchaseableCheck purchaseableCheck;
    /* 0x0C */ EnDnsSetRupeesAndFlags setRupeesAndFlags;
} DnsItemEntry; // size = 0x10

typedef struct EnDns {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[18];
    /* 0x01FC */ Vec3s morphTable[18];
    /* 0x0268 */ EnDnsActionFunc actionFunc;
    /* 0x026C */ ColliderCylinder collider;
    /* 0x02B8 */ s16 dustTimer;
    /* 0x02BA */ u8 animIndex; // set but not read
    /* 0x02BB */ u8 maintainCollider;
    /* 0x02BC */ u8 standOnGround;
    /* 0x02BD */ u8 dropCollectible;
    /* 0x02C0 */ DnsItemEntry* dnsItemEntry;
    /* 0x02C4 */ f32 yInitPos;
    // #region SOH [Randomizer]
    /*        */ DnsItemEntry sohDnsItemEntry;
    /*        */ ScrubIdentity sohScrubIdentity;
    // #endregion
} EnDns; // size = 0x02C8

#endif

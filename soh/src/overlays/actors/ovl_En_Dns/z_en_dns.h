#ifndef Z_EN_DNS_H
#define Z_EN_DNS_H

#include "ultra64.h"
#include "global.h"

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
    /* 0x02BA */ u8 unk_2BA;
    /* 0x02BB */ u8 maintainCollider;
    /* 0x02BC */ u8 standOnGround;
    /* 0x02BD */ u8 dropCollectible;
    /* 0x02C0 */ DnsItemEntry* dnsItemEntry;
    /* 0x02C4 */ f32 yInitPos;
    /*        */ ScrubIdentity scrubIdentity;
} EnDns; // size = 0x02C8

#endif

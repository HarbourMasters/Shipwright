#ifndef Z_BG_HIDAN_FIREWALL_H
#define Z_BG_HIDAN_FIREWALL_H

#include "ultra64.h"
#include "global.h"

struct BgHidanFirewall;

typedef void (*BgHidanFirewallActionFunc)(struct BgHidanFirewall*, PlayState*);

typedef struct BgHidanFirewall {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BgHidanFirewallActionFunc actionFunc;
    /* 0x0150 */ s16 unk_150;
    /* 0x0154 */ ColliderCylinder collider;
} BgHidanFirewall; // size = 0x01A0

#endif

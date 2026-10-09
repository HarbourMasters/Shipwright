#ifndef Z_EN_GINKO_MAN_H
#define Z_EN_GINKO_MAN_H

#include "global.h"
#include "objects/object_boj/object_boj.h"

struct EnGinkoMan;

 typedef void (*EnGinkoManActionFunc)(struct EnGinkoMan*, PlayState*);

typedef struct EnGinkoMan {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnGinkoManActionFunc actionFunc;
    /* 0x18C */ Vec3s jointTable[OBJECT_BOJ_LIMB_MAX];
    /* 0x1EC */ Vec3s morphTable[OBJECT_BOJ_LIMB_MAX];
    /* 0x24C */ Vec3s headRot;
    /* 0x252 */ Vec3s torsoRot;
    /* 0x258 */ s16 curTextId;
    /* 0x25A */ s16 serviceFee;
    /* 0x25C */ s16 choiceDepositWithdrawl;
    /* 0x25E */ s16 isNewAccount;
    /* 0x260 */ s16 isStampChecked;
    /* 0x262 */ s16 previousBankValue;
    /* 0x264 */ s16 animTimer;
} EnGinkoMan; // size = 0x268

#define GINKOMAN_CHOICE_DEPOSIT   0
#define GINKOMAN_CHOICE_WITHDRAWL 1
#define GINKOMAN_CHOICE_CANCEL    2
#define GINKOMAN_CHOICE_RESET     0

#define GINKOMAN_CHOICE_YES  0
#define GINKOMAN_CHOICE_NO   1

#endif // Z_EN_GINKO_MAN_H

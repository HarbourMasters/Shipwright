#ifndef Z_EN_BOM_BOWL_MAN_H
#define Z_EN_BOM_BOWL_MAN_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"

struct EnBomBowlMan;

typedef void (*EnBomBowlManActionFunc)(struct EnBomBowlMan*, PlayState*);

typedef struct EnBomBowlMan {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[11];
    /* 0x01D2 */ Vec3s morphTable[11];
    /* 0x0214 */ EnBomBowlManActionFunc actionFunc;
    /* 0x0218 */ Vec3s unk_218;
    /* 0x021E */ char unk_21E[0x6];
    /* 0x0224 */ Vec3s unk_224;
    /* 0x022A */ s16 prizeRevealTimer;
    /* 0x022C */ s16 timer;
    /* 0x022E */ s16 dialogState;
    /* 0x0230 */ s16 prizeIndex;
    /* 0x0232 */ s16 startedPlaying; // set to true after starting the first round
    /* 0x0234 */ s16 eyeTextureIndex;
    /* 0x0236 */ s16 blinkTimer;
    /* 0x0238 */ s16 eyeMode;
    /* 0x023A */ s16 blinkCount;
    /* 0x023C */ s16 playingAgain; // whether player is playing again after a game
    /* 0x023E */ s16 wallStatus[2];
    /* 0x0242 */ s16 prizeSelect;
    /* 0x0244 */ s16 gameResult; // 0 = default, 1 = lost, 2 = won
    /* 0x0248 */ Vec3f posCopy; // Set and not used
    /* 0x0254 */ f32 frameCount;
    /* 0x0258 */ u8 minigamePlayStatus; // 0 = default, 1 = paid, 2 = playing
    /* 0x025C */ EnBomBowlPit* bowlPit;
    /* 0x0260 */ EnExItem* exItem;
} EnBomBowlMan; // size = 0x0264

#endif

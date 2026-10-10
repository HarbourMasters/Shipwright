/*
 * File: z_en_s_goro.c
 * Overlay: ovl_En_S_Goro
 * Description: Goron in Goron Shrine / Bomb Shop Goron
 */

/*
This actor appears to be used in three scenes.
-- Goron Shrine: Winter
       Three variants (one instance of each used in game).
       Main behavior determined by whether the Goron Elder's Son is asleep, and whether they themselves have been put to
sleep. One of them also has a dialogue tree that checks whether you have lit the chandelier (at least if the kid is not
crying)
-- Goron Shrine: Spring
       Six variants - one instance of each appearing in a circle around the elder on the upper level of the shrine if
you've beaten Goht during that cycle. Dialogue determined by whether you have talked to elder in that context as a goron
at least once during that cycle. They have a cheering animation used while the elder is talking to you, otherwise they
stand idle.
-- Bomb Shop
       Only one variant. Goron who sells you powder kegs in the bomb shop.
*/

/*
Week Event Flags:
        22 Bit 2	--	Goron Elder's Son has been pacified (Winter)
        77 Bit 7	--	You have spoken to the Goron Elder in goron form during spring

        36 All Bits
        37 Bits 0 - 2
                Track whether or not you have ever spoken to each of the three winter shrine gorons
                in various forms. ("Ever" being since last time reset, of course)
*/

#include "z_en_s_goro.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "overlays/actors/ovl_En_Gk/z_en_gk.h" // Goron Elder's Son
#include "overlays/actors/ovl_En_Jg/z_en_jg.h" // Goron Elder
#include "objects/object_taisou/object_taisou.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define EN_S_GORO_ROLLEDUP_YOFFSET 14.0f
#define EN_S_GORO_OFTYPE_WSHRINE (EN_S_GORO_GET_MAIN_TYPE(&this->actor) < 3)

#define EN_S_GORO_ACTIONFLAG_ROLLEDUP (1 << 0)
#define EN_S_GORO_ACTIONFLAG_FACEPLAYER (1 << 1)
#define EN_S_GORO_ACTIONFLAG_EYESOPEN (1 << 2)
#define EN_S_GORO_ACTIONFLAG_EARSCOVERED (1 << 3)
#define EN_S_GORO_ACTIONFLAG_ENGAGED (1 << 4)
#define EN_S_GORO_ACTIONFLAG_LASTMESSAGE (1 << 5)
#define EN_S_GORO_ACTIONFLAG_GKQUIET_ACKNOWLEDGED (1 << 6)
#define EN_S_GORO_ACTIONFLAG_SNOREPHASE (1 << 7)
#define EN_S_GORO_ACTIONFLAG_UNK0100 (1 << 8)
#define EN_S_GORO_ACTIONFLAG_HANDTAP (1 << 9)
#define EN_S_GORO_ACTIONFLAG_TIRED (1 << 10)
#define EN_S_GORO_ACTIONFLAG_SUPPRESS_SNORE (1 << 11)

#define EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN (1 << 0)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_GORONPK (1 << 1)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU (1 << 2)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA (1 << 3)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN_FINALNIGHT (1 << 4)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_GOROKPK_FINALNIGHT (1 << 5)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU_FINALNIGHT (1 << 6)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA_FINALNIGHT (1 << 7)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_GORON (1 << 8)
#define EN_S_GORO_BOMBBUYFLAG_TALKED_GORON_FINALNIGHT (1 << 9)
#define EN_S_GORO_BOMBBUYFLAG_YESBUY (1 << 10)

void EnSGoro_Init(Actor* thisx, PlayState* play);
void EnSGoro_Destroy(Actor* thisx, PlayState* play);
void EnSGoro_Update(Actor* thisx, PlayState* play);
void EnSGoro_Draw(Actor* thisx, PlayState* play);

ActorProfile En_S_Goro_Profile = {
    /**/ ACTOR_EN_S_GORO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnSGoro),
    /**/ EnSGoro_Init,
    /**/ EnSGoro_Destroy,
    /**/ EnSGoro_Update,
    /**/ EnSGoro_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

typedef enum EnSGoroAnimation {
    /*  0x0 */ EN_S_GORO_ANIM_IDLE_LIEDOWN_A,
    /*  0x1 */ EN_S_GORO_ANIM_IDLE_LIEDOWN_B,
    /*  0x2 */ EN_S_GORO_ANIM_UNROLL_A,
    /*  0x3 */ EN_S_GORO_ANIM_UNROLL_B,
    /*  0x4 */ EN_S_GORO_ANIM_ROLLUP,
    /*  0x5 */ EN_S_GORO_ANIM_SHIVER_A,
    /*  0x6 */ EN_S_GORO_ANIM_SHIVER_B,
    /*  0x7 */ EN_S_GORO_ANIM_7,
    /*  0x7 */ EN_S_GORO_ANIM_8,
    /*  0x7 */ EN_S_GORO_ANIM_9,
    /*  0x7 */ EN_S_GORO_ANIM_10,
    /*  0xB */ EN_S_GORO_ANIM_COVEREARS,
    /*  0xC */ EN_S_GORO_ANIM_TAISOU_CHEER,
    /*  0xD */ EN_S_GORO_ANIM_STAND_HANDTAP,
    /*  0xE */ EN_S_GORO_ANIM_SLEEPY,
    /*  0xF */ EN_S_GORO_ANIM_IDLE_STAND,
    /* 0x10 */ EN_S_GORO_ANIM_MAX
} EnSGoroAnimation;

static AnimationInfoS sAnimationInfo[EN_S_GORO_ANIM_MAX] = {
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                     // EN_S_GORO_ANIM_IDLE_LIEDOWN_A
    { &gGoronLyingDownIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                    // EN_S_GORO_ANIM_IDLE_LIEDOWN_B
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, 0 },                            // EN_S_GORO_ANIM_UNROLL_A
    { &gGoronUnrollAnim, 2.0f, 0, -1, ANIMMODE_ONCE, -4 },                           // EN_S_GORO_ANIM_UNROLL_B
    { &gGoronUnrollAnim, -2.0f, 0, -1, ANIMMODE_ONCE, -4 },                          // EN_S_GORO_ANIM_ROLLUP
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                            // EN_S_GORO_ANIM_SHIVER_A
    { &gGoronShiverAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                           // EN_S_GORO_ANIM_SHIVER_B
    { &gGoronAthleticsDoubleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // EN_S_GORO_ANIM_7
    { &gGoronAthleticsShakeLimbsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },               // EN_S_GORO_ANIM_8
    { &gGoronAthleticsSingleArmSideBendAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // EN_S_GORO_ANIM_9
    { &gGoronAthleticsHamstringStretchStandingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 }, // EN_S_GORO_ANIM_10
    { &gGoronCoverEarsAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },                         // EN_S_GORO_ANIM_COVEREARS
    { &gGoronAthleticsCheerAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },                   // EN_S_GORO_ANIM_TAISOU_CHEER
    { &gGoronStandingHandTappingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },              // EN_S_GORO_ANIM_STAND_HANDTAP
    { &gGoronSleepyAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -4 },                           // EN_S_GORO_ANIM_SLEEPY
    { &gGoronStandingIdleAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -8 },                     // EN_S_GORO_ANIM_IDLE_STAND
};

typedef enum EnSGoroEyeTexture {
    /* 0x0 */ EN_S_GORO_EYETEX_OPEN,
    /* 0x1 */ EN_S_GORO_EYETEX_HALF,
    /* 0x2 */ EN_S_GORO_EYETEX_CLOSED,
    /* 0x3 */ EN_S_GORO_EYETEX_CLOSED2,
    /* 0x4 */ EN_S_GORO_EYETEX_MAX
} EnSGoroEyeTexture;

static TexturePtr sEyeTextures[EN_S_GORO_EYETEX_MAX] = {
    gGoronEyeOpenTex,
    gGoronEyeHalfTex,
    gGoronEyeClosedTex,
    gGoronEyeClosed2Tex,
};

void EnSGoro_WinterShrineGoron_Idle(EnSGoro* this, PlayState* play);
void EnSGoro_WinterShrineGoron_Talk(EnSGoro* this, PlayState* play);
void EnSGoro_SpringShrineGoron_Idle(EnSGoro* this, PlayState* play);
void EnSGoro_SpringShrineGoron_Talk(EnSGoro* this, PlayState* play);
void EnSGoro_ShopGoron_Idle(EnSGoro* this, PlayState* play);
void EnSGoro_ShopGoron_FinishUnroll(EnSGoro* this, PlayState* play);
void EnSGoro_ShopGoron_Talk(EnSGoro* this, PlayState* play);
void EnSGoro_ShopGoron_TakePayment(EnSGoro* this, PlayState* play);
void EnSGoro_ShopGoron_FinishTransaction(EnSGoro* this, PlayState* play);
void EnSGoro_Sleep(EnSGoro* this, PlayState* play);
void EnSGoro_SleepTalk(EnSGoro* this, PlayState* play);

/**
 * Dialogue tree for EnSGoro when use in Goron Shrine context. Returns ID of next message to display.
 */
u16 EnSGoro_ShrineGoron_NextTextId(EnSGoro* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (EN_S_GORO_GET_MAIN_TYPE(&this->actor)) {
        case EN_S_GORO_TYPE_SHRINE_WINTER_A:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
                if (player->transformation == PLAYER_FORM_GORON) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_02)) {
                        switch (this->textId) {
                            case 0xCFB:
                                return 0xCFC;
                            case 0xCFC:
                                return 0xCFD;
                            case 0xCFD:
                                SET_WEEKEVENTREG(WEEKEVENTREG_36_02);
                                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                                return 0xCFE;
                            default:
                                return 0xCFB;
                        }
                    } else {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xCFE;
                    }
                } else {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_01)) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_36_01);
                        return 0xCF9;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xCFA;
                }
            } else {
                // Scene flag - Checks whether the chandelier has been lit
                if (!Flags_GetSwitch(play, EN_S_GORO_GET_SWITCH_FLAG(&this->actor))) {
                    if (player->transformation == PLAYER_FORM_GORON) {
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_08)) {
                            if (this->textId == 0xD02) {
                                SET_WEEKEVENTREG(WEEKEVENTREG_36_08);
                                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                                return 0xD03;
                            }
                            return 0xD02;
                        }
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xD04;
                    }
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_04)) {
                        if (this->textId == 0xCFF) {
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                            SET_WEEKEVENTREG(WEEKEVENTREG_36_04);
                            return 0xD00;
                        }
                        return 0xCFF;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD01;
                }
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_10)) {
                    if (this->textId == 0xD05) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_36_10);
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xD06;
                    }
                    return 0xD05;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xD07;
            }
            break;

        case EN_S_GORO_TYPE_SHRINE_WINTER_B:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
                if (player->transformation == PLAYER_FORM_GORON) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_40)) {
                        switch (this->textId) {
                            case 0xD15:
                                return 0xD16;
                            case 0xD16:
                                SET_WEEKEVENTREG(WEEKEVENTREG_36_40);
                                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                                return 0xD17;
                            default:
                                return 0xD15;
                        }
                    } else {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xD17;
                    }
                } else {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_20)) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_36_20);
                        return 0xD13;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD14;
                }
            } else if (player->transformation == PLAYER_FORM_GORON) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_37_01)) {
                    switch (this->textId) {
                        case 0xD1E:
                            return 0xD1F;
                        case 0xD1F:
                            SET_WEEKEVENTREG(WEEKEVENTREG_37_01);
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                            return 0xD20;
                        default:
                            return 0xD1E;
                    }
                } else {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD21;
                }
            } else {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_36_80)) {
                    switch (this->textId) {
                        case 0xD18:
                            return 0xD19;
                        case 0xD19:
                            return 0xD1A;
                        case 0xD1A:
                            return 0xD1B;
                        case 0xD1B:
                            SET_WEEKEVENTREG(WEEKEVENTREG_36_80);
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                            return 0xD1C;
                        default:
                            return 0xD18;
                    }
                } else {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD1D;
                }
            }
            break;

        case EN_S_GORO_TYPE_SHRINE_WINTER_C:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
                if (player->transformation == PLAYER_FORM_GORON) {
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_37_02)) {
                        switch (this->textId) {
                            case 0xD09:
                                return 0xD0A;
                            case 0xD0A:
                                return 0xD0B;
                            case 0xD0B:
                                SET_WEEKEVENTREG(WEEKEVENTREG_37_02);
                                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                                return 0xD0C;
                            default:
                                return 0xD09;
                        }
                    } else {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xD0D;
                    }
                } else {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD08;
                }
            } else if (player->transformation == PLAYER_FORM_GORON) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_37_04)) {
                    switch (this->textId) {
                        case 0xD0E:
                            return 0xD0F;
                        case 0xD0F:
                            SET_WEEKEVENTREG(WEEKEVENTREG_37_04);
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                            return 0xD10;
                        default:
                            return 0xD0E;
                    }
                } else {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xD11;
                }
            } else {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xD12;
            }
            break;

        case EN_S_GORO_TYPE_SHRINE_SPRING_A:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    if (this->textId == 0xDE3) {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xDE4;
                    }
                    return 0xDE3;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDE2;
            }
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0xDE1;

        case EN_S_GORO_TYPE_SHRINE_SPRING_B:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    if (this->textId == 0xDE7) {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xDE8;
                    }
                    return 0xDE7;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDE6;
            }
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0xDE5;

        case EN_S_GORO_TYPE_SHRINE_SPRING_C:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    if (this->textId == 0xDEB) {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xDEC;
                    }
                    return 0xDEB;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDEA;
            }
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0xDE9;

        case EN_S_GORO_TYPE_SHRINE_SPRING_D:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    if (this->textId == 0xDF1) {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xDF2;
                    }
                    return 0xDF1;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDF0;
            }
            switch (this->textId) {
                case 0xDED:
                    return 0xDEE;
                case 0xDEE:
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xDEF;
                default:
                    return 0xDED;
            }
            break;

        case EN_S_GORO_TYPE_SHRINE_SPRING_E:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    if (this->textId == 0xDF6) {
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0xDF7;
                    }
                    return 0xDF6;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDF5;
            }
            if (this->textId == 0xDF3) {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0xDF4;
            }
            return 0xDF3;

        case EN_S_GORO_TYPE_SHRINE_SPRING_F:
            if (player->transformation == PLAYER_FORM_GORON) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_80)) {
                    switch (this->textId) {
                        case 0xDFB:
                            return 0xDFC;
                        case 0xDFC:
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                            return 0xDFD;
                        default:
                            return 0xDFB;
                    }
                } else {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xDFA;
                }
            } else {
                if (this->textId == 0xDF8) {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0xDF9;
                }
                return 0xDF8;
            }
            break;

        default:
            return 0;
    }
}

/**
 * Dialogue tree for bomb shop goron. Returns ID of next message to display.
 */
u16 EnSGoro_BombshopGoron_NextTextId(EnSGoro* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->textId) {
        case 0x0:
            switch (player->transformation) {
                case PLAYER_FORM_GORON:
                    // Check if Powder Keg is in Powder Key slot.
                    if (INV_CONTENT(ITEM_POWDER_KEG) == ITEM_POWDER_KEG) {
                        if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                            if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_GOROKPK_FINALNIGHT)) {
                                this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_GOROKPK_FINALNIGHT;
                                this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
                                return 0x677;
                            }
                            return 0x67A;
                        }
                        if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_GORONPK)) {
                            this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_GORONPK;
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
                            return 0x66E;
                        }
                        return 0x679;
                    }

                    // No powder keg
                    if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                        if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_GORON_FINALNIGHT)) {
                            this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_GORON_FINALNIGHT;
                            this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
                            return 0x67E;
                        }
                        return 0x683;
                    }
                    if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_GORON)) {
                        this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_GORON;
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
                        return 0x67B;
                    }
                    return 0x681;

                case PLAYER_FORM_ZORA:
                    if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                        if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA_FINALNIGHT)) {
                            this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA_FINALNIGHT;
                            return 0x668;
                        }
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_TIRED;
                        return 0x66D;
                    }
                    if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA)) {
                        this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_ZORA;
                        return 0x664;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_TIRED;
                    return 0x66C;

                case PLAYER_FORM_DEKU:
                    if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                        if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU_FINALNIGHT)) {
                            this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU_FINALNIGHT;
                            return 0x668;
                        }
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0x66D;
                    }
                    if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU)) {
                        this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_DEKU;
                        return 0x664;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0x66C;

                case PLAYER_FORM_HUMAN:
                    if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                        if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN_FINALNIGHT)) {
                            this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN_FINALNIGHT;
                            return 0x668;
                        }
                        this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                        return 0x66D;
                    }
                    if (!(this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN)) {
                        this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_TALKED_HUMAN;
                        return 0x664;
                    }
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    return 0x66C;
            }

        case 0x664:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x665;

        case 0x665:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x666;

        case 0x666:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_TIRED;
            return 0x667;

        case 0x668:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x669;

        case 0x669:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x66A;

        case 0x66A:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_TIRED;
            return 0x66B;

        case 0x677:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x678;

        case 0x678:
            return 0x670;

        case 0x66E:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x66F;

        case 0x66F:
            return 0x670;

        case 0x679:
            return 0x670;

        case 0x67A:
            return 0x670;

        case 0x67E:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x67F;

        case 0x67F:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0x680;

        case 0x67B:
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_HANDTAP;
            return 0x67C;

        case 0x67C:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0x67D;

        case 0x681:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0x682;

        case 0x683:
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0x684;

        case 0x670:
            if (this->bombbuyFlags & EN_S_GORO_BOMBBUYFLAG_YESBUY) {
                if (GameInteractor_Should(VB_POWDER_KEG_CHECK_HAS, AMMO(ITEM_POWDER_KEG) != 0)) {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    return 0x673;
                }
                this->powderKegPrice = play->msgCtx.unk1206C;
                if (gSaveContext.save.saveInfo.playerData.rupees < this->powderKegPrice) {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_TIRED;
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    return 0x674;
                }
                if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                    this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                    Audio_PlaySfx_MessageDecide();
                    return 0x676;
                }
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                Audio_PlaySfx_MessageDecide();
                return 0x675;
            }
            if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                return 0x672;
            }
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            return 0x671;
    }
    return 0;
}

/**
 * Links to actor of type EN_JG (Goron Elder), returns whether operation successful.
 */
s32 EnSGoro_FindGoronElder(EnSGoro* this, PlayState* play) {
    this->otherGoron = SubS_FindActor(play, this->otherGoron, ACTORCAT_NPC, ACTOR_EN_JG);
    if (this->otherGoron != NULL) {
        return true;
    }
    return false;
}

/**
 * Links to actor of type EN_GK (Goron Elder's Son), returns whether operation successful.
 */
s32 EnSGoro_FindGoronChild(EnSGoro* this, PlayState* play) {
    this->otherGoron = NULL;
    this->otherGoron = SubS_FindActor(play, this->otherGoron, ACTORCAT_NPC, ACTOR_EN_GK);
    if (this->otherGoron != NULL) {
        return true;
    }
    return false;
}

/**
 * Looks like it manages the snoring cycle for when rolled up and asleep.
 */
void EnSGoro_UpdateSleeping(EnSGoro* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;

    this->snorePhase += 0x400;
    this->scaleFactor = Math_SinS(this->snorePhase) * 0.01f * 0.1f;
    this->actor.scale.y = 0.01f - this->scaleFactor;
    this->actor.scale.z = 0.01f - this->scaleFactor;
    this->actor.scale.x = 0.01f + this->scaleFactor;

    if (!(this->actionFlags & EN_S_GORO_ACTIONFLAG_SUPPRESS_SNORE)) {
        if (this->snorePhase == 0) {
            if (this->actionFlags & EN_S_GORO_ACTIONFLAG_SNOREPHASE) {
                this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_SNOREPHASE;
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SNORE2);
            } else {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_SNOREPHASE;
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOLON_SNORE1);
            }
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
        this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_SUPPRESS_SNORE;
    }

    if (!(this->actionFlags & EN_S_GORO_ACTIONFLAG_ROLLEDUP) && (curFrame == 0)) {
        this->actionFlags |= EN_S_GORO_ACTIONFLAG_ROLLEDUP;
        this->actor.shape.yOffset = EN_S_GORO_ROLLEDUP_YOFFSET;
    }

    this->actor.shape.yOffset = (this->actor.scale.y / 0.01f) * EN_S_GORO_ROLLEDUP_YOFFSET;
}

s32 EnSGoro_UpdateCheerAnimation(EnSGoro* this, PlayState* play) {
    if (this->animIndex == EN_S_GORO_ANIM_IDLE_STAND) {
        if (((EnJg*)this->otherGoron)->flags & 1) {
            this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_TAISOU);
            if (this->objectSlot > OBJECT_SLOT_NONE) {
                gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
                this->animIndex = EN_S_GORO_ANIM_TAISOU_CHEER;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
                return true;
            }
        }
    } else if ((this->animIndex == EN_S_GORO_ANIM_TAISOU_CHEER) && !(((EnJg*)this->otherGoron)->flags & 1)) {
        this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_OF1D_MAP);
        if (this->objectSlot > OBJECT_SLOT_NONE) {
            gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
            this->animIndex = EN_S_GORO_ANIM_IDLE_STAND;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->skelAnime.curFrame = this->skelAnime.endFrame;
            return true;
        }
    }
    return false;
}

s32 EnSGoro_CheckLullaby(EnSGoro* this, PlayState* play) {
    s32 actorType;
    Player* player = GET_PLAYER(play);

    if ((player->transformation == PLAYER_FORM_GORON) && (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT)) {
        if (play->msgCtx.lastPlayedSong == OCARINA_SONG_GORON_LULLABY) {
            this->animIndex = EN_S_GORO_ANIM_ROLLUP;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);

            actorType = EN_S_GORO_GET_MAIN_TYPE(&this->actor);
            this->snorePhase = 0x400 << (actorType + 1);

            /*
             * Should be:
             *	if ((actorType % 2) == 0)
             * However, the original code checks the negative case whereas the above does not.
             * So unfortunately, this is the only way to get it to match.
             */
            if (!(actorType % 2)) {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_SNOREPHASE;
            }
            this->actionFunc = EnSGoro_Sleep;
            return true;
        }
    }
    return false;
}

s32 EnSGoro_CheckGKBehavior(EnSGoro* this, PlayState* play) {
    s32 actorType;

    if (!EnSGoro_FindGoronChild(this, play)) {
        return false;
    }
    if ((!(this->actionFlags & EN_S_GORO_ACTIONFLAG_GKQUIET_ACKNOWLEDGED)) &&
        ((((EnGk*)this->otherGoron)->unk_1E4 & 0x80) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON))) {

        this->actionFlags |= EN_S_GORO_ACTIONFLAG_GKQUIET_ACKNOWLEDGED;
        this->animIndex = EN_S_GORO_ANIM_ROLLUP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);

        actorType = EN_S_GORO_GET_MAIN_TYPE(&this->actor);
        this->snorePhase = 0x400 << (actorType + 1);

        /*
         * Should be:
         *	if ((actorType % 2) == 0)
         * However, the original code checks the negative case whereas the above does not.
         * So unfortunately, this is the only way to get it to match.
         */
        if (!(actorType % 2)) {
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_SNOREPHASE;
        }
        this->actionFlags |= EN_S_GORO_ACTIONFLAG_SUPPRESS_SNORE;
        this->actionFunc = EnSGoro_Sleep;
        return true;
    }
    return false;
}

void EnSGoro_UpdateToHandtapAnimation(EnSGoro* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex != EN_S_GORO_ANIM_STAND_HANDTAP) && (curFrame == endFrame)) {
        this->animIndex = EN_S_GORO_ANIM_STAND_HANDTAP;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
}

void EnSGoro_UpdateSleepyAnimation(EnSGoro* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (this->animIndex != EN_S_GORO_ANIM_SLEEPY) {
        if (curFrame == endFrame) {
            this->animIndex = EN_S_GORO_ANIM_SLEEPY;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else if (curFrame == endFrame) {
        this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_TIRED;
        this->animIndex = EN_S_GORO_ANIM_IDLE_STAND;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
}

void EnSGoro_UpdateToIdleAnimation(EnSGoro* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex != EN_S_GORO_ANIM_IDLE_STAND) && (curFrame == endFrame)) {
        this->animIndex = EN_S_GORO_ANIM_IDLE_STAND;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
    }
}

void EnSGoro_UpdateCollider(EnSGoro* this, PlayState* play) {
    Vec3f worldPos = this->actor.world.pos;
    f32 radius = 24.0f;
    f32 height = 62.0f;

    this->collider.dim.pos.x = worldPos.x;
    this->collider.dim.pos.y = worldPos.y;
    this->collider.dim.pos.z = worldPos.z;
    this->collider.dim.radius = radius;
    this->collider.dim.height = height;

    //! @bug: The check is useless. If &this->collider somehow was NULL the above code would have already dereferenced
    //! it. Cast to `intptr_t` to suppress address comparision to NULL warning.
    if ((intptr_t)(&this->collider) != (intptr_t)NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnSGoro_UpdateEyes(EnSGoro* this) {
    if (this->actionFlags & EN_S_GORO_ACTIONFLAG_EYESOPEN) {
        if (DECR(this->eyeTimer) == 0) {
            this->eyeTexIndex++;
            if (this->eyeTexIndex > EN_S_GORO_EYETEX_CLOSED) {
                this->eyeTimer = Rand_S16Offset(30, 30);
                this->eyeTexIndex = EN_S_GORO_EYETEX_OPEN;
            }
        }
    }
}

void EnSGoro_UpdateActorFocus(EnSGoro* this) {
    f32 yDelta;

    if (this->actionFlags & EN_S_GORO_ACTIONFLAG_ROLLEDUP) {
        yDelta = this->actor.shape.yOffset;
    } else {
        yDelta = 58.0f;
    }

    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = this->actor.world.pos.y + yDelta;
    this->actor.focus.pos.z = this->actor.world.pos.z;
    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
}

s32 EnSGoro_UpdateRotationToPlayer(EnSGoro* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f playerPos;
    Vec3f thisPos;
    s16 target = (this->actor.yawTowardsPlayer - this->bodyRotY) - this->actor.shape.rot.y;

    Math_SmoothStepToS(&this->headRotY, target, 4, 0x2AA8, 1);
    this->headRotY = CLAMP(this->headRotY, -0x1FFE, 0x1FFE);

    target = (this->actor.yawTowardsPlayer - this->headRotY) - this->actor.shape.rot.y;
    Math_SmoothStepToS(&this->bodyRotY, target, 4, 0x2AA8, 1);
    this->bodyRotY = CLAMP(this->bodyRotY, -0x1C70, 0x1C70);

    playerPos = player->actor.world.pos;
    playerPos.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    thisPos = this->actor.world.pos;
    thisPos.y += 70.0f;

    target = Math_Vec3f_Pitch(&thisPos, &playerPos) - this->bodyRotZ;
    Math_SmoothStepToS(&this->headRotZ, target, 4, 0x2AA8, 1);
    this->headRotZ = CLAMP(this->headRotZ, -0x1C70, 0x1C70);

    target = Math_Vec3f_Pitch(&thisPos, &playerPos) - this->headRotZ;
    Math_SmoothStepToS(&this->bodyRotZ, target, 4, 0x2AA8, 1);
    this->bodyRotZ = CLAMP(this->bodyRotZ, -0x1C70, 0x1C70);

    return true;
}

s32 EnSGoro_UpdateAttentionTarget(EnSGoro* this, PlayState* play) {
    if (DECR(this->loseAttentionTimer) != 0) {
        // EnSGoro is in the process of returning to idle position from previously looking at the player.
        if ((this->actionFlags & EN_S_GORO_ACTIONFLAG_EARSCOVERED) &&
            (this->actionFlags & EN_S_GORO_ACTIONFLAG_EYESOPEN)) {
            // If ears are covered, but eyes are open, close eyes.
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_EYESOPEN;
            this->eyeTexIndex = EN_S_GORO_EYETEX_CLOSED2;
        }
        // Reset position to idle. EnSGoro cannot return attention to player until the timer is up.
        this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_FACEPLAYER;
        this->headRotZ = 0;
        this->headRotY = 0;
        this->bodyRotZ = 0;
        this->bodyRotY = 0;
        return true;
    }

    // Turn to face player if either player has engaged EnSGoro or if player is in range and EnSGoro isn't covering his
    // ears.
    if (this->actionFlags & EN_S_GORO_ACTIONFLAG_ENGAGED) {
        this->actionFlags |= EN_S_GORO_ACTIONFLAG_FACEPLAYER;
        EnSGoro_UpdateRotationToPlayer(this, play);
    } else if (!(this->actionFlags & EN_S_GORO_ACTIONFLAG_EARSCOVERED) &&
               Actor_IsFacingAndNearPlayer(&this->actor, 120.0f, 0x3FFC)) {
        this->actionFlags |= EN_S_GORO_ACTIONFLAG_FACEPLAYER;
        EnSGoro_UpdateRotationToPlayer(this, play);
    } else {
        // Unengaged or player out of range.
        if (this->actionFlags & EN_S_GORO_ACTIONFLAG_FACEPLAYER) {
            // Previously facing player, but now player out of range.
            // Start counter for frames/function calls to ignore player while in process of returning to idle.
            this->loseAttentionTimer = 20;
        }
        // Start animation to return to idle position.
        this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_FACEPLAYER;
        this->headRotZ = 0;
        this->headRotY = 0;
        this->bodyRotZ = 0;
        this->bodyRotY = 0;
    }
    return true;
}

void EnSGoro_SetupAction(EnSGoro* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actionFlags = 0;
        if (EN_S_GORO_OFTYPE_WSHRINE) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CALMED_GORON_ELDERS_SON)) {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_GKQUIET_ACKNOWLEDGED;
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_EYESOPEN;
                this->animIndex = EN_S_GORO_ANIM_SHIVER_A;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            } else {
                this->eyeTexIndex = EN_S_GORO_EYETEX_CLOSED2;
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_EARSCOVERED;
                this->animIndex = EN_S_GORO_ANIM_COVEREARS;
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            }
        } else {
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_EYESOPEN;
            this->animIndex = EN_S_GORO_ANIM_IDLE_STAND;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        }

        this->scaleFactor = 0.01f;
        Actor_SetScale(&this->actor, 0.01f);
        this->actor.gravity = -1.0f;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        this->actor.attentionRangeType = ATTENTION_RANGE_1;

        switch (EN_S_GORO_GET_MAIN_TYPE(&this->actor)) {
            case EN_S_GORO_TYPE_SHRINE_WINTER_A:
                this->actionFunc = EnSGoro_WinterShrineGoron_Idle;
                break;
            case EN_S_GORO_TYPE_SHRINE_WINTER_B:
                this->actionFunc = EnSGoro_WinterShrineGoron_Idle;
                break;
            case EN_S_GORO_TYPE_SHRINE_WINTER_C:
                this->actionFunc = EnSGoro_WinterShrineGoron_Idle;
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_A:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_B:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_C:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_D:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_E:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_SHRINE_SPRING_F:
                if (EnSGoro_FindGoronElder(this, play)) {
                    this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
                }
                break;
            case EN_S_GORO_TYPE_BOMBSHOP:
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_ROLLEDUP;
                this->actionFunc = EnSGoro_ShopGoron_Idle;
                this->actor.shape.yOffset = EN_S_GORO_ROLLEDUP_YOFFSET;
                break;
            default:
                Actor_Kill(&this->actor);
                break;
        }
    }
}

void EnSGoro_WinterShrineGoron_Idle(EnSGoro* this, PlayState* play) {
    if (!EnSGoro_CheckLullaby(this, play) && !EnSGoro_CheckGKBehavior(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_ENGAGED;
            if (this->actionFlags & EN_S_GORO_ACTIONFLAG_EARSCOVERED) {
                this->actionFlags |= EN_S_GORO_ACTIONFLAG_EYESOPEN;
                this->eyeTexIndex = EN_S_GORO_EYETEX_OPEN;
            }
            this->textId = EnSGoro_ShrineGoron_NextTextId(this, play);
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnSGoro_WinterShrineGoron_Talk;
        } else if ((this->actor.xzDistToPlayer < 250.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 5, 0x1000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnSGoro_WinterShrineGoron_Talk(EnSGoro* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        if (this->actionFlags & EN_S_GORO_ACTIONFLAG_LASTMESSAGE) {
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_ENGAGED;
            this->actionFunc = EnSGoro_WinterShrineGoron_Idle;
            return;
        }
        this->textId = EnSGoro_ShrineGoron_NextTextId(this, play);
        Message_StartTextbox(play, this->textId, &this->actor);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnSGoro_SpringShrineGoron_Idle(EnSGoro* this, PlayState* play) {
    if ((EN_S_GORO_GET_MAIN_TYPE(&this->actor) == EN_S_GORO_TYPE_SHRINE_SPRING_F) ||
        !EnSGoro_UpdateCheerAnimation(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actionFlags |= EN_S_GORO_ACTIONFLAG_ENGAGED;
            this->textId = EnSGoro_ShrineGoron_NextTextId(this, play);
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnSGoro_SpringShrineGoron_Talk;
        } else if ((this->actor.xzDistToPlayer < 250.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 5, 0x1000, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void EnSGoro_SpringShrineGoron_Talk(EnSGoro* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->actionFlags & EN_S_GORO_ACTIONFLAG_LASTMESSAGE) {
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_ENGAGED;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnSGoro_SpringShrineGoron_Idle;
            return;
        }
        this->textId = EnSGoro_ShrineGoron_NextTextId(this, play);
        Message_StartTextbox(play, this->textId, &this->actor);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnSGoro_ShopGoron_Idle(EnSGoro* this, PlayState* play) {
    if (!(this->actionFlags & EN_S_GORO_ACTIONFLAG_ROLLEDUP)) {
        EnSGoro_UpdateToIdleAnimation(this);
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFlags |= EN_S_GORO_ACTIONFLAG_ENGAGED;
        this->textId = EnSGoro_BombshopGoron_NextTextId(this, play);
        if (this->actionFlags & EN_S_GORO_ACTIONFLAG_ROLLEDUP) {
            this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_ROLLEDUP;
            this->animIndex = EN_S_GORO_ANIM_UNROLL_A;
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnSGoro_ShopGoron_FinishUnroll;
        } else {
            Message_StartTextbox(play, this->textId, &this->actor);
            this->actionFunc = EnSGoro_ShopGoron_Talk;
        }
    } else if ((this->actor.xzDistToPlayer < 250.0f) || this->actor.isLockedOn) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnSGoro_ShopGoron_FinishUnroll(EnSGoro* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex == EN_S_GORO_ANIM_UNROLL_A) && (curFrame == endFrame)) {
        this->animIndex = EN_S_GORO_ANIM_IDLE_STAND;
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, this->animIndex);
        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnSGoro_ShopGoron_Talk;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnSGoro_ShopGoron_Talk(EnSGoro* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (this->actionFlags & EN_S_GORO_ACTIONFLAG_HANDTAP) {
        EnSGoro_UpdateToHandtapAnimation(this);
    } else if (this->actionFlags & EN_S_GORO_ACTIONFLAG_TIRED) {
        EnSGoro_UpdateSleepyAnimation(this);
    } else {
        EnSGoro_UpdateToIdleAnimation(this);
    }

    if (talkState == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play)) {
            if (this->actionFlags & EN_S_GORO_ACTIONFLAG_LASTMESSAGE) {
                this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_LASTMESSAGE;
                this->actionFlags &= ~EN_S_GORO_ACTIONFLAG_ENGAGED;
                this->textId = 0;
                this->actionFunc = EnSGoro_ShopGoron_Idle;
                return;
            }
            this->textId = EnSGoro_BombshopGoron_NextTextId(this, play);
            Message_StartTextbox(play, this->textId, &this->actor);
        }
    } else if ((talkState == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                this->bombbuyFlags |= EN_S_GORO_BOMBBUYFLAG_YESBUY;
                break;
            case 1:
                Audio_PlaySfx_MessageCancel();
                this->bombbuyFlags &= ~EN_S_GORO_BOMBBUYFLAG_YESBUY;
                break;
        }

        this->textId = EnSGoro_BombshopGoron_NextTextId(this, play);
        if ((this->textId == 0x675) || (this->textId == 0x676)) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            Actor_OfferGetItem(&this->actor, play, GI_POWDER_KEG, 300.0f, 300.0f);
            this->actionFunc = EnSGoro_ShopGoron_TakePayment;
        } else {
            Message_StartTextbox(play, this->textId, &this->actor);
        }
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnSGoro_ShopGoron_TakePayment(EnSGoro* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        Rupees_ChangeBy(-this->powderKegPrice);
        this->actionFunc = EnSGoro_ShopGoron_FinishTransaction;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_POWDER_KEG, 300.0f, 300.0f);
    }
}

void EnSGoro_ShopGoron_FinishTransaction(EnSGoro* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnSGoro_ShopGoron_Talk;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnSGoro_Sleep(EnSGoro* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_StartTextbox(play, 0x23A, &this->actor);
        this->actionFunc = EnSGoro_SleepTalk;
    } else if (this->actor.isLockedOn) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
    EnSGoro_UpdateSleeping(this, play);
}

void EnSGoro_SleepTalk(EnSGoro* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play)) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnSGoro_Sleep;
        }
    }
    EnSGoro_UpdateSleeping(this, play);
}

void EnSGoro_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSGoro* this = (EnSGoro*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, &gGoronUnrollAnim, this->jointTable, this->morphTable,
                       GORON_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    if (this->actor.update != NULL) {
        s32 objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_OF1D_MAP);

        this->objectSlot = objectSlot;
        if (objectSlot <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
        }
    }
    this->actor.draw = EnSGoro_Draw;
    this->actionFunc = EnSGoro_SetupAction;
}

void EnSGoro_Destroy(Actor* thisx, PlayState* play) {
    EnSGoro* this = (EnSGoro*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnSGoro_Update(Actor* thisx, PlayState* play) {
    EnSGoro* this = (EnSGoro*)thisx;

    this->actionFunc(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);
    SkelAnime_Update(&this->skelAnime);
    if (this->animIndex != EN_S_GORO_ANIM_SLEEPY) {
        EnSGoro_UpdateAttentionTarget(this, play);
    }
    EnSGoro_UpdateEyes(this);
    EnSGoro_UpdateActorFocus(this);
    EnSGoro_UpdateCollider(this, play);
}

/**
 * This function appears to be identical to SubS_UpdateLimb. But it's here!
 */
s32 EnSGoro_UpdateLimb(s16 newRotZ, s16 newRotY, Vec3f* pos, Vec3s* rot, s32 stepRot, s32 overrideRot) {
    Vec3f newPos;
    Vec3f zeroVec = gZeroVec3f;
    Vec3s newRot;
    MtxF curState;

    Matrix_MultVec3f(&zeroVec, &newPos);
    Matrix_Get(&curState);
    Matrix_MtxFToYXZRot(&curState, &newRot, false);

    *pos = newPos;

    if (!stepRot && !overrideRot) {
        rot->x = newRot.x;
        rot->y = newRot.y;
        rot->z = newRot.z;
        return true;
    }

    if (overrideRot) {
        newRot.z = newRotZ;
        newRot.y = newRotY;
    }

    Math_SmoothStepToS(&rot->x, newRot.x, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&rot->y, newRot.y, 3, 0x2AA8, 0xB6);
    Math_SmoothStepToS(&rot->z, newRot.z, 3, 0x2AA8, 0xB6);
    return true;
}

s32 EnSGoro_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnSGoro_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    s32 stepRot;
    s32 overrideRot;
    EnSGoro* this = (EnSGoro*)thisx;

    switch (limbIndex) {
        case GORON_LIMB_HEAD:
            if (this->animIndex != EN_S_GORO_ANIM_SLEEPY) {
                if (this->actionFlags & EN_S_GORO_ACTIONFLAG_FACEPLAYER) {
                    overrideRot = true;
                } else {
                    overrideRot = false;
                }
                if (this->loseAttentionTimer != 0) {
                    stepRot = true;
                } else {
                    stepRot = false;
                }

                EnSGoro_UpdateLimb(this->headRotZ + this->bodyRotZ + 0x4000,
                                   this->headRotY + this->bodyRotY + this->actor.shape.rot.y + 0x4000,
                                   &this->headTranslate, &this->headRotate, stepRot, overrideRot);

                Matrix_Pop();
                Matrix_Translate(this->headTranslate.x, this->headTranslate.y, this->headTranslate.z, MTXMODE_NEW);
                Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
                Matrix_RotateYS(this->headRotate.y, MTXMODE_APPLY);
                Matrix_RotateXS(this->headRotate.x, MTXMODE_APPLY);
                Matrix_RotateZS(this->headRotate.z, MTXMODE_APPLY);
                Matrix_Translate(CVarGetFloat(CVAR_COSMETIC("Silly.GoronNeckLength"), 0.0f), 0.0f, 0.0f, MTXMODE_APPLY);
                Matrix_Push();
            }
            break;

        case GORON_LIMB_BODY:
            if (this->animIndex != EN_S_GORO_ANIM_SLEEPY) {
                if (this->actionFlags & EN_S_GORO_ACTIONFLAG_FACEPLAYER) {
                    overrideRot = true;
                } else {
                    overrideRot = false;
                }
                if (this->loseAttentionTimer != 0) {
                    stepRot = true;
                } else {
                    stepRot = false;
                }

                EnSGoro_UpdateLimb(this->bodyRotZ + 0x4000, this->bodyRotY + this->actor.shape.rot.y + 0x4000,
                                   &this->bodyTranslate, &this->bodyRotate, stepRot, overrideRot);

                Matrix_Pop();
                Matrix_Translate(this->bodyTranslate.x, this->bodyTranslate.y, this->bodyTranslate.z, MTXMODE_NEW);
                Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
                Matrix_RotateYS(this->bodyRotate.y, MTXMODE_APPLY);
                Matrix_RotateXS(this->bodyRotate.x, MTXMODE_APPLY);
                Matrix_RotateZS(this->bodyRotate.z, MTXMODE_APPLY);
                Matrix_Push();
            }
            break;

        default:
            break;
    }
}

void EnSGoro_DrawUnrolled(EnSGoro* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));
    gDPPipeSync(POLY_OPA_DISP++);

    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnSGoro_OverrideLimbDraw, NULL,
                                   EnSGoro_TransformLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSGoro_DrawRolledUp(EnSGoro* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + this->actor.shape.yOffset,
                     this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, this->actor.shape.yOffset, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGoronRolledUpDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnSGoro_Draw(Actor* thisx, PlayState* play) {
    EnSGoro* this = (EnSGoro*)thisx;

    if (this->actionFlags & EN_S_GORO_ACTIONFLAG_ROLLEDUP) {
        EnSGoro_DrawRolledUp(this, play);
    } else {
        EnSGoro_DrawUnrolled(this, play);
    }
}

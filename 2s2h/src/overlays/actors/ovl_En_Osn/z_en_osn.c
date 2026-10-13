/*
 * File: z_en_osn.c
 * Overlay: ovl_En_Osn
 * Description: Happy Mask Salesman
 */

#include "z_en_osn.h"
#include "objects/object_osn/object_osn.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnOsn_Init(Actor* thisx, PlayState* play);
void EnOsn_Destroy(Actor* thisx, PlayState* play);
void EnOsn_Update(Actor* thisx, PlayState* play);
void EnOsn_Draw(Actor* thisx, PlayState* play);

void EnOsn_Idle(EnOsn* this, PlayState* play);
void EnOsn_StartCutscene(EnOsn* this, PlayState* play);
void EnOsn_HandleCutscene(EnOsn* this, PlayState* play);
void EnOsn_Talk(EnOsn* this, PlayState* play);

#define OSN_STATE_SPECIAL_CONVERSTATION (1 << 0)
#define OSN_STATE_MET_HUMAN (1 << 1)
#define OSN_STATE_MET_DEKU (1 << 2)
#define OSN_STATE_MET_GORON (1 << 3)
#define OSN_STATE_MET_ZORA (1 << 4)
#define OSN_STATE_END_CONVERSATION (1 << 5)

#define OSN_MASK_TEXT_GREAT_FAIRY (1 << 0)
#define OSN_MASK_TEXT_GIBDO (1 << 1)
#define OSN_MASK_TEXT_TRUTH (1 << 2)
#define OSN_MASK_TEXT_GIANT (1 << 3)
#define OSN_MASK_TEXT_KAFEIS (1 << 4)
#define OSN_MASK_TEXT_DON_GERO (1 << 5)
#define OSN_MASK_TEXT_BLAST (1 << 6)
#define OSN_MASK_TEXT_COUPLE (1 << 7)
#define OSN_MASK_TEXT_SCENTS (1 << 8)
#define OSN_MASK_TEXT_KAMARO (1 << 9)
#define OSN_MASK_TEXT_STONE (1 << 10)
#define OSN_MASK_TEXT_POSTMAN (1 << 11)
#define OSN_MASK_TEXT_BUNNY (1 << 12)
#define OSN_MASK_TEXT_CAPTAIN (1 << 13)
#define OSN_MASK_TEXT_BREMEN (1 << 14)
#define OSN_MASK_TEXT_CIRCUS_LEADER (1 << 15)
#define OSN_MASK_TEXT_KEATON (1 << 16)
#define OSN_MASK_TEXT_GARO (1 << 17)
#define OSN_MASK_TEXT_ALL_NIGHT (1 << 18)
#define OSN_MASK_TEXT_ROMANI (1 << 19)

ActorProfile En_Osn_Profile = {
    /**/ ACTOR_EN_OSN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OSN,
    /**/ sizeof(EnOsn),
    /**/ EnOsn_Init,
    /**/ EnOsn_Destroy,
    /**/ EnOsn_Update,
    /**/ EnOsn_Draw,
};

typedef enum {
    /*  0 */ OSN_ANIM_IDLE,
    /*  1 */ OSN_ANIM_ARMS_OUT,
    /*  2 */ OSN_ANIM_BOWING,
    /*  3 */ OSN_ANIM_REMINISCE,
    /*  4 */ OSN_ANIM_HANDS_CLASPED,
    /*  5 */ OSN_ANIM_BELIEVE,
    /*  6 */ OSN_ANIM_THINK,
    /*  7 */ OSN_ANIM_SHAKE_HEAD,
    /*  8 */ OSN_ANIM_ORGAN_TALK,
    /*  9 */ OSN_ANIM_ORGAN_PLAY,
    /* 10 */ OSN_ANIM_SHAKE,
    /* 11 */ OSN_ANIM_CHOKE,
    /* 12 */ OSN_ANIM_DESPAIR,
    /* 13 */ OSN_ANIM_FAST_BOWS,
    /* 14 */ OSN_ANIM_HAND_OUT,
    /* 15 */ OSN_ANIM_LYING_DOWN_FACE_UP,
    /* 16 */ OSN_ANIM_LYING_DOWN_FACE_DOWN,
    /* 17 */ OSN_ANIM_MASK_LOOK_AT,
    /* 18 */ OSN_ANIM_TURN_AROUND_START,
    /* 19 */ OSN_ANIM_TURN_AROUND_LOOP,
    /* 20 */ OSN_ANIM_WALK_AWAY,
    /* 21 */ OSN_ANIM_MASK_LOOK_FROM_START,
    /* 22 */ OSN_ANIM_MASK_LOOK_FROM_LOOP,
    /* 23 */ OSN_ANIM_HAND_OUT_2,    // Exact same as OSN_ANIM_HAND_OUT
    /* 24 */ OSN_ANIM_WALK_AWAY_END, // Only the last frame of OSN_ANIM_WALK_AWAY
    /* 25 */ OSN_ANIM_MAX
} OsnAnimation;

static AnimationInfo sAnimationInfo[OSN_ANIM_MAX] = {
    { &gHappyMaskSalesmanIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },            // OSN_ANIM_IDLE
    { &gHappyMaskSalesmanArmsOutAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },         // OSN_ANIM_ARMS_OUT
    { &gHappyMaskSalesmanBowingAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },          // OSN_ANIM_BOWING
    { &gHappyMaskSalesmanReminisceAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },       // OSN_ANIM_REMINISCE
    { &gHappyMaskSalesmanHandsClaspedAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },    // OSN_ANIM_HANDS_CLASPED
    { &gHappyMaskSalesmanBelieveAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },         // OSN_ANIM_BELIEVE
    { &gHappyMaskSalesmanThinkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },           // OSN_ANIM_THINK
    { &gHappyMaskSalesmanShakeHeadAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },       // OSN_ANIM_SHAKE_HEAD
    { &gHappyMaskSalesmanOrganTalkAnim, 1.0f, 1.0f, 39.0f, ANIMMODE_LOOP, 0.0f },      // OSN_ANIM_ORGAN_TALK
    { &gHappyMaskSalesmanOrganPlayAnim, 1.0f, 1.0f, 70.0f, ANIMMODE_LOOP, 0.0f },      // OSN_ANIM_ORGAN_PLAY
    { &gHappyMaskSalesmanShakeAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },           // OSN_ANIM_SHAKE
    { &gHappyMaskSalesmanChokeAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },           // OSN_ANIM_CHOKE
    { &gHappyMaskSalesmanDespairAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },         // OSN_ANIM_DESPAIR
    { &gHappyMaskSalesmanFastBowsAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },        // OSN_ANIM_FAST_BOWS
    { &gHappyMaskSalesmanHandOutAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },         // OSN_ANIM_HAND_OUT
    { &gHappyMaskSalesmanLyingDownAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },       // OSN_ANIM_LYING_DOWN_FACE_UP
    { &gHappyMaskSalesmanLyingDownAnim, 0.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 0.0f },       // OSN_ANIM_LYING_DOWN_FACE_DOWN
    { &gHappyMaskSalesmanMaskLookAtAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },      // OSN_ANIM_MASK_LOOK_AT
    { &gHappyMaskSalesmanTurnAroundStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // OSN_ANIM_TURN_AROUND_START
    { &gHappyMaskSalesmanTurnAroundLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },  // OSN_ANIM_TURN_AROUND_LOOP
    { &gHappyMaskSalesmanWalkAwayAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },        // OSN_ANIM_WALK_AWAY
    // OSN_ANIM_MASK_LOOK_FROM_START
    { &gHappyMaskSalesmanMaskLookFromStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f },
    { &gHappyMaskSalesmanMaskLookFromLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // OSN_ANIM_MASK_LOOK_FROM_LOOP
    { &gHappyMaskSalesmanHandOutAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },          // OSN_ANIM_HAND_OUT_2
    { &gHappyMaskSalesmanWalkAwayAnim, 0.0f, 77.0f, 0.0f, ANIMMODE_ONCE, 0.0f },        // OSN_ANIM_WALK_AWAY_END
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_STOP),
};

void EnOsn_UpdateCollider(EnOsn* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnOsn_GetMaskText(PlayState* play) {
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_GREAT_FAIRY:
            return 0x1FD6;

        case PLAYER_MASK_GIBDO:
            return 0x1FD8;

        case PLAYER_MASK_TRUTH:
            return 0x1FDA;

        case PLAYER_MASK_GIANT:
            return 0x1FDC;

        case PLAYER_MASK_KAFEIS_MASK:
            return 0x1FDE;

        case PLAYER_MASK_DON_GERO:
            return 0x1FE0;

        case PLAYER_MASK_BLAST:
            return 0x1FE2;

        case PLAYER_MASK_COUPLE:
            return 0x1FE4;

        case PLAYER_MASK_SCENTS:
            return 0x1FE6;

        case PLAYER_MASK_KAMARO:
            return 0x1FE8;

        case PLAYER_MASK_STONE:
            return 0x1FEA;

        case PLAYER_MASK_POSTMAN:
            return 0x1FEC;

        case PLAYER_MASK_BUNNY:
            return 0x1FEE;

        case PLAYER_MASK_CAPTAIN:
            return 0x1FF0;

        case PLAYER_MASK_BREMEN:
            return 0x1FF2;

        case PLAYER_MASK_CIRCUS_LEADER:
            return 0x1FF4;

        case PLAYER_MASK_KEATON:
            return 0x1FF6;

        case PLAYER_MASK_GARO:
            return 0x1FF8;

        case PLAYER_MASK_ALL_NIGHT:
            return 0x1FFA;

        case PLAYER_MASK_ROMANI:
            return 0x1FFC;

        default:
            return 0;
    }
}

void EnOsn_TurnAround(EnOsn* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex == OSN_ANIM_TURN_AROUND_START) && (curFrame == endFrame)) {
        this->animIndex = OSN_ANIM_TURN_AROUND_LOOP;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, OSN_ANIM_TURN_AROUND_LOOP);
    }
}

void EnOsn_LookFromMask(EnOsn* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if ((this->animIndex == OSN_ANIM_MASK_LOOK_FROM_START) && (curFrame == endFrame)) {
        this->animIndex = OSN_ANIM_MASK_LOOK_FROM_LOOP;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, OSN_ANIM_MASK_LOOK_FROM_LOOP);
    }
}

void EnOsn_FadeOut(EnOsn* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    if (curFrame == endFrame) {
        this->alpha -= 8;
        if (this->alpha < 8) {
            this->alpha = 0;
            Actor_Kill(&this->actor);
        }
    }
}

s32 EnOsn_GetInitialMaskText(EnOsn* this, PlayState* play) {
    switch (Player_GetMask(play)) {
        case PLAYER_MASK_GREAT_FAIRY:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_GREAT_FAIRY)) {
                this->maskTextFlags |= OSN_MASK_TEXT_GREAT_FAIRY;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_GIBDO:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_GIBDO)) {
                this->maskTextFlags |= OSN_MASK_TEXT_GIBDO;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_TRUTH:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_TRUTH)) {
                this->maskTextFlags |= OSN_MASK_TEXT_TRUTH;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_GIANT:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_GIANT)) {
                this->maskTextFlags |= OSN_MASK_TEXT_GIANT;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_KAFEIS_MASK:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_KAFEIS)) {
                this->maskTextFlags |= OSN_MASK_TEXT_KAFEIS;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_DON_GERO:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_DON_GERO)) {
                this->maskTextFlags |= OSN_MASK_TEXT_DON_GERO;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_BLAST:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_BLAST)) {
                this->maskTextFlags |= OSN_MASK_TEXT_BLAST;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_COUPLE:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_COUPLE)) {
                this->maskTextFlags |= OSN_MASK_TEXT_COUPLE;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_SCENTS:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_SCENTS)) {
                this->maskTextFlags |= OSN_MASK_TEXT_SCENTS;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_KAMARO:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_KAMARO)) {
                this->maskTextFlags |= OSN_MASK_TEXT_KAMARO;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_STONE:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_STONE)) {
                this->maskTextFlags |= OSN_MASK_TEXT_STONE;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_POSTMAN:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_POSTMAN)) {
                this->maskTextFlags |= OSN_MASK_TEXT_POSTMAN;
                return 0x1FD2;
            }
            break;

        case PLAYER_MASK_BUNNY:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_BUNNY)) {
                this->maskTextFlags |= OSN_MASK_TEXT_BUNNY;
                return 0x1FD2;
            }
            break;

        case PLAYER_MASK_CAPTAIN:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_CAPTAIN)) {
                this->maskTextFlags |= OSN_MASK_TEXT_CAPTAIN;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_BREMEN:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_BREMEN)) {
                this->maskTextFlags |= OSN_MASK_TEXT_BREMEN;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_CIRCUS_LEADER:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_CIRCUS_LEADER)) {
                this->maskTextFlags |= OSN_MASK_TEXT_CIRCUS_LEADER;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_KEATON:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_KEATON)) {
                this->maskTextFlags |= OSN_MASK_TEXT_KEATON;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_GARO:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_GARO)) {
                this->maskTextFlags |= OSN_MASK_TEXT_GARO;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_ALL_NIGHT:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_ALL_NIGHT)) {
                this->maskTextFlags |= OSN_MASK_TEXT_ALL_NIGHT;
                return 0x1FD1;
            }
            break;

        case PLAYER_MASK_ROMANI:
            if (!(this->maskTextFlags & OSN_MASK_TEXT_ROMANI)) {
                this->maskTextFlags |= OSN_MASK_TEXT_ROMANI;
                return 0x1FD1;
            }
            break;

        default:
            break;
    }
    this->stateFlags |= OSN_STATE_END_CONVERSATION;
    if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        return 0x2006;
    }
    return 0x1FCD;
}

s32 EnOsn_GetInitialText(EnOsn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) &&
        CHECK_QUEST_ITEM(QUEST_SONG_HEALING)) {
        if (this->stateFlags & OSN_STATE_SPECIAL_CONVERSTATION) {
            this->stateFlags |= OSN_STATE_END_CONVERSATION;
            if ((gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) &&
                (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU)) {
                if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                    (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    return 0x2006;
                }
                return 0x1FCD;
            }
            return 0x1FAF;
        }

        if (player->transformation == PLAYER_FORM_DEKU) {
            if (this->stateFlags & OSN_STATE_MET_DEKU) {
                this->stateFlags |= OSN_STATE_END_CONVERSATION;
                if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                    (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    return 0x2006;
                }
                return 0x1FCD;
            }
            this->stateFlags |= OSN_STATE_MET_DEKU;
            return 0x1FC8;
        }

        if (player->transformation == PLAYER_FORM_GORON) {
            if (this->stateFlags & OSN_STATE_MET_GORON) {
                this->stateFlags |= OSN_STATE_END_CONVERSATION;
                if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                    (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    return 0x2006;
                } else {
                    return 0x1FCD;
                }
            }
            this->stateFlags |= OSN_STATE_MET_GORON;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_76_20)) {
                return 0x1FC8;
            } else {
                SET_WEEKEVENTREG(WEEKEVENTREG_76_20);
                return 0x1FCE;
            }
        }

        if (player->transformation == PLAYER_FORM_ZORA) {
            if (this->stateFlags & OSN_STATE_MET_ZORA) {
                this->stateFlags |= OSN_STATE_END_CONVERSATION;
                if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                    (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    return 0x2006;
                }
                return 0x1FCD;
            }

            this->stateFlags |= OSN_STATE_MET_ZORA;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_76_40)) {
                return 0x1FC8;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_76_40);
            return 0x1FD0;
        }

        if (Player_GetMask(play) == PLAYER_MASK_NONE) {
            if (this->stateFlags & OSN_STATE_MET_HUMAN) {
                this->stateFlags |= OSN_STATE_END_CONVERSATION;
                if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                    (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                    return 0x2006;
                }
                return 0x1FCD;
            }
            this->stateFlags |= OSN_STATE_MET_HUMAN;
            return 0x1FC8;
        }

        return EnOsn_GetInitialMaskText(this, play);
    }

    this->stateFlags |= OSN_STATE_END_CONVERSATION;
    if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) && (CURRENT_TIME < CLOCK_TIME(6, 0))) {
        return 0x2004;
    }

    return 0x1FAE;
}

void EnOsn_HandleConversation(EnOsn* this, PlayState* play) {
    switch (this->textId) {
        case 0x1FC8:
            this->textId = 0x1FC9;
            break;

        case 0x1FC9:
            this->textId = 0x1FCA;
            break;

        case 0x1FCA:
            if ((gSaveContext.save.day == 3) && (CURRENT_TIME >= CLOCK_TIME(5, 0)) &&
                (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                this->textId = 0x2007;
            } else {
                this->textId = 0x1FCB;
            }
            break;

        case 0x1FCB:
        case 0x2007:
            this->textId = 0x1FCC;
            this->stateFlags |= OSN_STATE_END_CONVERSATION;
            break;

        case 0x1FCE:
        case 0x1FD0:
            this->textId = 0x1FCF;
            break;

        case 0x1FCF:
            this->textId = 0x1FCA;
            break;

        case 0x1FD1:
        case 0x1FD2:
            this->textId = EnOsn_GetMaskText(play);
            break;

        case 0x1FD6:
            this->textId = 0x1FD7;
            break;

        case 0x1FD8:
            this->textId = 0x1FD9;
            break;

        case 0x1FDA:
            this->textId = 0x1FDB;
            break;

        case 0x1FDC:
            this->textId = 0x1FDD;
            break;

        case 0x1FDE:
            this->textId = 0x1FDF;
            break;

        case 0x1FE0:
            this->textId = 0x1FE1;
            break;

        case 0x1FE2:
            this->textId = 0x1FE3;
            break;

        case 0x1FE4:
            this->textId = 0x1FE5;
            break;

        case 0x1FE6:
            this->textId = 0x1FE7;
            break;

        case 0x1FE8:
            this->textId = 0x1FE9;
            break;

        case 0x1FEA:
            this->textId = 0x1FEB;
            break;

        case 0x1FEC:
            this->textId = 0x1FED;
            break;

        case 0x1FEE:
            this->textId = 0x1FEF;
            break;

        case 0x1FF0:
            this->textId = 0x1FF1;
            break;

        case 0x1FF2:
            this->textId = 0x1FF3;
            break;

        case 0x1FF4:
            this->textId = 0x1FF5;
            break;

        case 0x1FF6:
            this->textId = 0x1FF7;
            break;

        case 0x1FF8:
            this->textId = 0x1FF9;
            break;

        case 0x1FFA:
            this->textId = 0x1FFB;
            break;

        case 0x1FFC:
            this->textId = 0x1FFD;
            break;

        case 0x1FD7:
        case 0x1FDB:
        case 0x1FDD:
        case 0x1FDF:
        case 0x1FE1:
        case 0x1FE7:
        case 0x1FF1:
        case 0x1FF3:
        case 0x1FF7:
        case 0x1FF9:
        case 0x1FFB:
            this->textId = 0x1FD3;
            break;

        case 0x1FD9:
        case 0x1FE3:
        case 0x1FE9:
        case 0x1FEB:
        case 0x1FED:
        case 0x1FEF:
        case 0x1FF5:
        case 0x1FFD:
            this->textId = 0x1FD4;
            break;

        case 0x1FE5:
            this->textId = 0x1FFE;
            break;

        case 0x1FD3:
        case 0x1FD4:
        case 0x1FFE:
            this->textId = 0x1FD5;
            this->stateFlags |= OSN_STATE_END_CONVERSATION;
    }

    Message_StartTextbox(play, this->textId, &this->actor);
}

void EnOsn_InitCutscene(EnOsn* this) {
    this->csId = this->actor.csId;
    if ((gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) ||
        (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU)) {
        this->csId = CutsceneManager_GetAdditionalCsId(this->csId);

        if ((gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) ||
            (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU)) {
            this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        }
    }
}

void EnOsn_ChooseAction(EnOsn* this, PlayState* play) {
    u32 isSwitchFlagSet = Flags_GetSwitch(play, 0);

    this->csId = this->actor.csId;

    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, OSN_ANIM_IDLE);
    if (!isSwitchFlagSet) {
        this->actionFunc = EnOsn_HandleCutscene;
    } else {
        this->actionFunc = EnOsn_Idle;
    }
}

void EnOsn_Idle(EnOsn* this, PlayState* play) {
    s16 yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (GameInteractor_Should(VB_OSN_CONSIDER_ELIGIBLE_FOR_SONG_OF_HEALING,
                              (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] != ITEM_NONE) &&
                                  !CHECK_QUEST_ITEM(QUEST_SONG_HEALING),
                              this)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            if (GameInteractor_Should(VB_OSN_TEACH_SONG_OF_HEALING, true, this)) {
                this->actionFunc = EnOsn_StartCutscene;
            }
        } else if (((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) && (yaw < 0x4000) &&
                   (yaw > -0x4000)) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            this->actor.textId = 0xFFFF;
        }
    } else if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->textId = EnOsn_GetInitialText(this, play);
        Message_StartTextbox(play, this->textId, &this->actor);
        this->actionFunc = EnOsn_Talk;
    } else if (((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) && (yaw < 0x4000) && (yaw > -0x4000)) {
        Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
    }
}

void EnOsn_StartCutscene(EnOsn* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_Start(this->csId, &this->actor);
        this->actionFunc = EnOsn_HandleCutscene;
    } else {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        }
        CutsceneManager_Queue(this->csId);
    }
}

void EnOsn_HandleCutscene(EnOsn* this, PlayState* play) {
    u8 pad;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_130)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_130);
        this->shouldRotateHead = false;
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    this->animIndex = OSN_ANIM_BOWING;
                    break;

                case 2:
                    this->animIndex = OSN_ANIM_ARMS_OUT;
                    break;

                case 3:
                    this->animIndex = OSN_ANIM_SHAKE_HEAD;
                    break;

                case 4:
                    this->animIndex = OSN_ANIM_REMINISCE;
                    break;

                case 5:
                    this->animIndex = OSN_ANIM_THINK;
                    break;

                case 6:
                    this->animIndex = OSN_ANIM_BELIEVE;
                    break;

                case 7:
                    this->animIndex = OSN_ANIM_HANDS_CLASPED;
                    break;

                case 8:
                    this->animIndex = OSN_ANIM_IDLE;
                    break;

                case 10:
                    this->animIndex = OSN_ANIM_ORGAN_TALK;
                    break;

                case 11:
                    this->animIndex = OSN_ANIM_ORGAN_PLAY;
                    break;

                case 13:
                    this->animIndex = OSN_ANIM_SHAKE;
                    break;

                case 15:
                    this->animIndex = OSN_ANIM_CHOKE;
                    break;

                case 16:
                    this->animIndex = OSN_ANIM_DESPAIR;
                    break;

                case 17:
                    this->animIndex = OSN_ANIM_FAST_BOWS;
                    break;

                case 18:
                    this->animIndex = OSN_ANIM_HAND_OUT;
                    break;

                case 19:
                    this->animIndex = OSN_ANIM_MASK_LOOK_AT;
                    break;

                case 20:
                    this->animIndex = OSN_ANIM_TURN_AROUND_START;
                    break;

                case 21:
                    this->animIndex = OSN_ANIM_WALK_AWAY;
                    break;

                case 22:
                    this->animIndex = OSN_ANIM_MASK_LOOK_FROM_START;
                    break;

                case 23:
                    this->animIndex = OSN_ANIM_HAND_OUT_2;
                    break;

                case 24:
                    this->animIndex = OSN_ANIM_WALK_AWAY_END;
                    break;

                default:
                    this->animIndex = OSN_ANIM_IDLE;
                    break;
            }
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        }

        if ((this->animIndex == OSN_ANIM_BELIEVE) && (play->sceneId == SCENE_SPOT00) &&
            (gSaveContext.sceneLayer == 0xB) && (play->csCtx.curFrame == 400)) {
            Actor_PlaySfx(&this->actor, NA_SE_VO_OMVO00);
        }

        if (this->animIndex == OSN_ANIM_TURN_AROUND_START) {
            EnOsn_TurnAround(this);
        }

        if (this->animIndex == OSN_ANIM_MASK_LOOK_FROM_START) {
            EnOsn_LookFromMask(this);
        }

        if (this->animIndex == OSN_ANIM_WALK_AWAY_END) {
            EnOsn_FadeOut(this);
        }

        if ((this->animIndex == OSN_ANIM_WALK_AWAY) &&
            (Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 27.0f) ||
             Animation_OnFrame(&this->skelAnime, 37.0f) || Animation_OnFrame(&this->skelAnime, 47.0f) ||
             Animation_OnFrame(&this->skelAnime, 57.0f) || Animation_OnFrame(&this->skelAnime, 67.0f))) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_OMENYA_WALK);
        }
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    } else {
        this->shouldRotateHead = true;
        this->cueId = 99;
        EnOsn_ChooseAction(this, play);
    }
}

void EnOsn_Talk(EnOsn* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_DONE) || (talkState == TEXT_STATE_EVENT)) && Message_ShouldAdvance(play)) {
        if (this->stateFlags & OSN_STATE_END_CONVERSATION) {
            this->stateFlags &= ~OSN_STATE_END_CONVERSATION;
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
            this->actionFunc = EnOsn_Idle;
        } else {
            EnOsn_HandleConversation(this, play);
        }
    }
}

void EnOsn_DoNothing(EnOsn* this, PlayState* play) {
}

void EnOsn_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOsn* this = (EnOsn*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 20.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gHappyMaskSalesmanSkel, &gHappyMaskSalesmanIdleAnim, NULL, NULL, 0);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->alpha = 255;

    switch (ENOSN_GET_TYPE(&this->actor)) {
        case OSN_TYPE_CHOOSE:
            if ((gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 2)) ||
                (gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 3)) ||
                (gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 6))) {
                this->stateFlags |= OSN_STATE_SPECIAL_CONVERSTATION;
            }
            this->shouldRotateHead = true;
            if (play->sceneId == SCENE_INSIDETOWER) {
                if ((gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 2)) ||
                    (gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 6))) {
                    this->actionFunc = EnOsn_HandleCutscene;
                } else if (gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_INTERIOR, 3)) {
                    EnOsn_InitCutscene(this);
                    this->actionFunc = EnOsn_StartCutscene;
                } else {
                    EnOsn_ChooseAction(this, play);
                }
            } else {
                EnOsn_ChooseAction(this, play);
            }
            break;

        case OSN_TYPE_LIE_FACE_DOWN:
            this->animIndex = OSN_ANIM_LYING_DOWN_FACE_UP;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnOsn_DoNothing;
            break;

        case OSN_TYPE_LIE_FACE_UP:
            this->animIndex = OSN_ANIM_LYING_DOWN_FACE_DOWN;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnOsn_DoNothing;
            break;

        case OSN_TYPE_CUTSCENE:
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actionFunc = EnOsn_HandleCutscene;
            break;

        default:
            Actor_Kill(&this->actor);
            break;
    }
}

void EnOsn_Destroy(Actor* thisx, PlayState* play) {
    EnOsn* this = (EnOsn*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->collider);
}

void EnOsn_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOsn* this = (EnOsn*)thisx;
    u32 isSwitchFlagSet = Flags_GetSwitch(play, 0);

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    SkelAnime_Update(&this->skelAnime);

    if (ENOSN_GET_TYPE(&this->actor) == OSN_TYPE_CHOOSE) {
        if (isSwitchFlagSet) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            EnOsn_UpdateCollider(this, play);
            this->actor.draw = EnOsn_Draw;
        } else {
            this->actor.draw = NULL;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    }

    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
}

s32 EnOsn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                           Gfx** gfx) {
    EnOsn* this = (EnOsn*)thisx;

    if (this->shouldRotateHead && (limbIndex == HAPPY_MASK_SALESMAN_LIMB_HEAD)) {
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
    }
    if (((this->animIndex == OSN_ANIM_ORGAN_PLAY) || (this->animIndex == OSN_ANIM_ORGAN_TALK)) &&
        (limbIndex == HAPPY_MASK_SALESMAN_LIMB_BACKPACK)) {
        *dList = NULL;
    }
    return false;
}

void EnOsn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnOsn* this = (EnOsn*)thisx;
    Vec3f headOffset = { 0.0f, 0.0f, 0.0f };
    Vec3s leftHandRot = { 0x9920, -0x384, -0x320 };

    if (limbIndex == HAPPY_MASK_SALESMAN_LIMB_HEAD) {
        Matrix_MultVec3f(&headOffset, &this->actor.focus.pos);
    }

    if (((this->animIndex == OSN_ANIM_MASK_LOOK_AT) || (this->animIndex == OSN_ANIM_MASK_LOOK_FROM_START) ||
         (this->animIndex == OSN_ANIM_MASK_LOOK_FROM_LOOP)) &&
        (limbIndex == HAPPY_MASK_SALESMAN_LIMB_LEFT_HAND)) {
        Matrix_Push();
        Matrix_Translate(-400.0f, 1100.0f, -200.0f, MTXMODE_APPLY);
        Matrix_RotateXS(leftHandRot.x, MTXMODE_APPLY);
        Matrix_RotateYS(leftHandRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(leftHandRot.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx);
        gSPDisplayList((*gfx)++, &gHappyMaskSalesmanMajorasMaskDL);
        Matrix_Pop();
    }
}

void EnOsn_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeClosedHappyTex = gHappyMaskSalesmanEyeClosedHappyTex;
    static TexturePtr sEyeOpenTex = gHappyMaskSalesmanEyeOpenTex;
    static TexturePtr sEyeClosedAngryTex = gHappyMaskSalesmanEyeClosedAngryTex;
    static TexturePtr sSmileTex = gHappyMaskSalesmanSmileTex;
    static TexturePtr sFrownTex = gHappyMaskSalesmanFrownTex;
    s32 pad;
    EnOsn* this = (EnOsn*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->alpha == 255) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        if ((this->animIndex == OSN_ANIM_CHOKE) || (this->animIndex == OSN_ANIM_DESPAIR) ||
            (this->animIndex == OSN_ANIM_HAND_OUT_2) || (play->msgCtx.currentTextId == 0x1FCA)) {
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeOpenTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sSmileTex));
        } else if ((this->animIndex == OSN_ANIM_SHAKE_HEAD) || (this->animIndex == OSN_ANIM_REMINISCE) ||
                   (this->animIndex == OSN_ANIM_FAST_BOWS)) {
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeClosedAngryTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sFrownTex));
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeClosedHappyTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sSmileTex));
        }
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        Scene_SetRenderModeXlu(play, 0, 1);
        POLY_OPA_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnOsn_OverrideLimbDraw, EnOsn_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeClosedHappyTex));
        gSPSegment(POLY_XLU_DISP++, 0x09, Lib_SegmentedToVirtual(sSmileTex));
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->alpha);
        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP =
            SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               EnOsn_OverrideLimbDraw, EnOsn_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#include "soh/Enhancements/mm/forms/FormsCommon.h"

#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "soh/Enhancements/mm/MMPlayerAnims.h"
}

// Barehanded melee for the transformed forms: the Goron punch combo
// (PLAYER_MWA_GORON_PUNCH_LEFT/RIGHT/BUTT) and the Zora punch-punch-kick combo
// (PLAYER_MWA_ZORA_PUNCH_LEFT/COMBO/KICK), with animations and active frames taken from MM's
// sMeleeAttackAnimInfo. Damage is delivered through an AT cylinder placed on the striking
// hand; Goron punches are flagged as Megaton Hammer damage (romhack precedent, and MM treats
// them as hammer-strength blows), Zora fins count as master-sword-grade slashes.
//
// While transformed, sword/shield/projectile items are blocked via VB_PLAYER_USE_ITEM
// (MM gPlayerFormItemRestrictions equivalent), so B is free for the punch combo.

typedef struct FormPunch {
    const char* anim;
    f32 startFrame; // first AT-active frame (MM sMeleeAttackAnimInfo unk_C)
    f32 endFrame;   // last AT-active frame (MM sMeleeAttackAnimInfo unk_D)
    u8 bodyPart;    // PLAYER_BODYPART_* the hitbox follows
} FormPunch;

static const FormPunch sGoronPunches[3] = {
    { gPlayerAnim_pg_punchA, 6.0f, 8.0f, PLAYER_BODYPART_L_HAND },
    { gPlayerAnim_pg_punchB, 12.0f, 18.0f, PLAYER_BODYPART_R_HAND },
    { gPlayerAnim_pg_punchC, 8.0f, 14.0f, PLAYER_BODYPART_WAIST },
};

static const FormPunch sZoraPunches[3] = {
    { gPlayerAnim_pz_attackA, 2.0f, 5.0f, PLAYER_BODYPART_L_HAND },
    { gPlayerAnim_pz_attackB, 3.0f, 8.0f, PLAYER_BODYPART_R_HAND },
    { gPlayerAnim_pz_attackC, 3.0f, 10.0f, PLAYER_BODYPART_R_FOOT },
};

static ColliderCylinder sPunchCollider;
static Actor* sPunchColliderActor = nullptr;
static PlayerShipForm sPunchForm = PLAYER_SHIPFORM_HUMAN;

void FormMelee_PunchAction(Player* player, PlayState* play);

static const FormPunch* GetPunchTable() {
    return (sPunchForm == PLAYER_SHIPFORM_GORON) ? sGoronPunches : sZoraPunches;
}

static void SetPunchAttackCylinder(PlayState* play, Player* player, Vec3f* pos, u32 dmgFlags, u8 damage) {
    static ColliderCylinderInit sCylinderInit = {
        {
            COLTYPE_NONE,
            AT_ON | AT_TYPE_PLAYER,
            AC_NONE,
            OC1_NONE,
            OC2_NONE,
            COLSHAPE_CYLINDER,
        },
        {
            ELEMTYPE_UNK2,
            { 0x00000000, 0x00, 0x01 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 20, 30, -15, { 0, 0, 0 } },
    };

    if (sPunchColliderActor != &player->actor) {
        Collider_InitCylinder(play, &sPunchCollider);
        Collider_SetCylinder(play, &sPunchCollider, &player->actor, &sCylinderInit);
        sPunchColliderActor = &player->actor;
    }

    sPunchCollider.info.toucher.dmgFlags = dmgFlags;
    sPunchCollider.info.toucher.damage = damage;
    sPunchCollider.dim.pos.x = (s16)pos->x;
    sPunchCollider.dim.pos.y = (s16)pos->y;
    sPunchCollider.dim.pos.z = (s16)pos->z;
    CollisionCheck_SetAT(play, &play->colChkCtx, &sPunchCollider.base);
}

// av1.actionVar1 = combo index (0..2), av2.actionVar2 = next punch queued
void FormMelee_PunchAction(Player* player, PlayState* play) {
    const FormPunch* punch = &GetPunchTable()[player->av1.actionVar1];
    Input* input = &play->state.input[0];
    bool animDone = LinkAnimation_Update(play, &player->skelAnime);
    f32 frame = player->skelAnime.curFrame;

    player->stateFlags2 |= PLAYER_STATE2_DISABLE_ROTATION_Z_TARGET;
    Player_DecelerateToZero(player);

    if (CHECK_BTN_ALL(input->press.button, BTN_B) && (player->av1.actionVar1 < 2) && (frame >= punch->startFrame)) {
        player->av2.actionVar2 = 1;
    }

    if (LinkAnimation_OnFrame(&player->skelAnime, punch->startFrame)) {
        Player_PlaySfx(&player->actor, NA_SE_IT_SWORD_SWING_HARD); // MM NA_SE_IT_GORON_PUNCH_SWING
        Player_PlayVoiceSfx(player, NA_SE_VO_LI_SWORD_N);
    }

    if ((frame >= punch->startFrame) && (frame <= punch->endFrame)) {
        Vec3f fistPos = player->bodyPartsPos[punch->bodyPart];

        if (sPunchForm == PLAYER_SHIPFORM_GORON) {
            SetPunchAttackCylinder(play, player, &fistPos, DMG_HAMMER_SWING, 2);
        } else {
            SetPunchAttackCylinder(play, player, &fistPos, DMG_SLASH_MASTER, 2);
        }
    }

    if (animDone) {
        if ((player->av2.actionVar2 != 0) && (player->av1.actionVar1 < 2)) {
            player->av1.actionVar1++;
            player->av2.actionVar2 = 0;
            Player_AnimPlayOnceAdjusted(play, player,
                                        (LinkAnimationHeader*)GetPunchTable()[player->av1.actionVar1].anim);
        } else {
            func_80839F90(player, play);
        }
    }
}

static bool IsNeutralGroundAction(Player* player) {
    return player->actionFunc == Player_Action_Idle || player->actionFunc == Player_Action_80840450 ||
           player->actionFunc == Player_Action_808407CC || player->actionFunc == Player_Action_80840DE4 ||
           player->actionFunc == Player_Action_808414F8 || player->actionFunc == Player_Action_8084170C;
}

static void StartPunch(PlayState* play, Player* player) {
    sPunchForm = PlayerForm_GetApplied();
    Player_SetupAction(play, player, FormMelee_PunchAction, 0);
    Player_AnimPlayOnceAdjusted(play, player, (LinkAnimationHeader*)GetPunchTable()[0].anim);
}

// Item ids blocked while transformed: melee weapons, shields and held projectile weapons
// (MM gPlayerFormItemRestrictions GORON/ZORA rows in spirit). B punches instead.
static bool IsItemBlockedWhileTransformed(s32 item) {
    switch (item) {
        case ITEM_SWORD_KOKIRI:
        case ITEM_SWORD_MASTER:
        case ITEM_SWORD_BGS:
        case ITEM_SWORD_KNIFE:
        case ITEM_STICK:
        case ITEM_HAMMER:
        case ITEM_SHIELD_DEKU:
        case ITEM_SHIELD_HYLIAN:
        case ITEM_SHIELD_MIRROR:
        case ITEM_BOW:
        case ITEM_BOW_ARROW_FIRE:
        case ITEM_BOW_ARROW_ICE:
        case ITEM_BOW_ARROW_LIGHT:
        case ITEM_SLINGSHOT:
        case ITEM_HOOKSHOT:
        case ITEM_LONGSHOT:
        case ITEM_BOOMERANG:
            return true;
        default:
            return false;
    }
}

static void RegisterFormMelee() {
    // Start a punch on a fresh B press in a neutral grounded action. This runs before
    // Player_Update processes the frame's input, so the vanilla sword attack for the same
    // B press never starts.
    GameInteractor::Instance->RegisterGameHook<GameInteractor::ShouldActorUpdate>([](void* actorRef, bool*) {
        Actor* actor = (Actor*)actorRef;
        PlayState* play = gPlayState;

        if (play == nullptr || actor != &GET_PLAYER(play)->actor) {
            return;
        }
        Player* player = GET_PLAYER(play);

        if (PlayerForm_GetApplied() == PLAYER_SHIPFORM_HUMAN) {
            return;
        }

        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B) && (player->actor.bgCheckFlags & 1) &&
            IsNeutralGroundAction(player) && !Player_InBlockingCsMode(play, player) &&
            !(player->stateFlags1 &
              (PLAYER_STATE1_IN_WATER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_IN_CUTSCENE |
               PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_SHIELDING))) {
            StartPunch(play, player);
        }
    });

    REGISTER_VB_SHOULD(VB_PLAYER_USE_ITEM, {
        Player* player = va_arg(args, Player*);
        s32 item = va_arg(args, s32);

        (void)player;
        if ((PlayerForm_GetApplied() != PLAYER_SHIPFORM_HUMAN) && IsItemBlockedWhileTransformed(item)) {
            Sfx_PlaySfxCentered(NA_SE_SY_ERROR);
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFormMelee);

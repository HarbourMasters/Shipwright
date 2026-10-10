/*
 * File: z_en_kendo_js.c
 * Overlay: ovl_En_Kendo_Js
 * Description: Swordsman
 */

#include "z_en_kendo_js.h"
#include "overlays/actors/ovl_En_Maruta/z_en_maruta.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnKendoJs_Init(Actor* thisx, PlayState* play);
void EnKendoJs_Destroy(Actor* thisx, PlayState* play);
void EnKendoJs_Update(Actor* thisx, PlayState* play);
void EnKendoJs_Draw(Actor* thisx, PlayState* play);

void func_80B26538(EnKendoJs* this);
void func_80B2654C(EnKendoJs* this, PlayState* play);
void func_80B26AE8(EnKendoJs* this);
void func_80B26AFC(EnKendoJs* this, PlayState* play);
void func_80B2701C(EnKendoJs* this);
void func_80B27030(EnKendoJs* this, PlayState* play);
void func_80B2714C(EnKendoJs* this);
void func_80B27188(EnKendoJs* this, PlayState* play);
void func_80B273D0(EnKendoJs* this);
void func_80B2740C(EnKendoJs* this, PlayState* play);
void func_80B274BC(EnKendoJs* this, PlayState* play);
void func_80B276C4(EnKendoJs* this);
void func_80B276D8(EnKendoJs* this, PlayState* play);
void func_80B27760(EnKendoJs* this);
void func_80B27774(EnKendoJs* this, PlayState* play);
void func_80B2783C(EnKendoJs* this, PlayState* play);
s32 EnKendoJs_MovePlayerToPos(PlayState* play, Vec3f targetPos);
void func_80B279F0(EnKendoJs* this, PlayState* play, s32 arg2);
void func_80B27A90(EnKendoJs* this, PlayState* play);

ActorProfile En_Kendo_Js_Profile = {
    /**/ ACTOR_EN_KENDO_JS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_JS,
    /**/ sizeof(EnKendoJs),
    /**/ EnKendoJs_Init,
    /**/ EnKendoJs_Destroy,
    /**/ EnKendoJs_Update,
    /**/ EnKendoJs_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 18, 30, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnKendoJsAnimation {
    /* 0 */ ENKENDOJS_ANIM_0,
    /* 1 */ ENKENDOJS_ANIM_1,
    /* 2 */ ENKENDOJS_ANIM_2,
    /* 3 */ ENKENDOJS_ANIM_3,
    /* 4 */ ENKENDOJS_ANIM_4,
    /* 5 */ ENKENDOJS_ANIM_MAX
} EnKendoJsAnimation;

static AnimationInfo sAnimationInfo[ENKENDOJS_ANIM_MAX] = {
    { &object_js_Anim_000C7C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // ENKENDOJS_ANIM_0
    { &object_js_Anim_000F4C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // ENKENDOJS_ANIM_1
    { &object_js_Anim_00016C, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // ENKENDOJS_ANIM_2
    { &object_js_Anim_0003DC, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -8.0f }, // ENKENDOJS_ANIM_3
    { &object_js_Anim_000AD4, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f }, // ENKENDOJS_ANIM_4
};

s16 D_80B27CE0[][3] = {
    { 0x2731, 0x2732, 0x2733 },
    { 0x2734, 0x2735, 0x2736 },
    { 0x2737, 0x2738, 0x2739 },
};

s16 D_80B27CF4[][3] = {
    { 0x2710, 0x2712, 0x2714 },
    { 0x2711, 0x2713, 0x2715 },
};

s16 D_80B27D00[] = {
    0x271C, 0x271E, 0x2720, 0x2722, 0x2724, 0x2726, 0x2728,
};

s16 D_80B27D10[] = {
    0x271B, 0x271D, 0x271F, 0x2721, 0x2723, 0x2725, 0x2727,
};

void EnKendoJs_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKendoJs* this = (EnKendoJs*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);

    SkelAnime_InitFlex(play, &this->skelAnime, &object_js_Skel_006990, &object_js_Anim_000F4C, this->jointTable,
                       this->morphTable, OBJECT_JS_LIMB_MAX);

    if ((CURRENT_DAY == 3) && ((CURRENT_TIME > CLOCK_TIME(23, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
        if (ENKENDOJS_GET_FF(&this->actor) != ENKENDOJS_FF_1) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_KANBAN, this->actor.home.pos.x, this->actor.home.pos.y,
                        this->actor.home.pos.z - 10.0f, this->actor.home.rot.x, this->actor.home.rot.y,
                        this->actor.home.rot.z, 0x10);
            Actor_Kill(&this->actor);
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_4);
        }
    } else if (ENKENDOJS_GET_FF(&this->actor) == ENKENDOJS_FF_1) {
        Actor_Kill(&this->actor);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (ENKENDOJS_GET_FF(&this->actor) != ENKENDOJS_FF_1) {
        Path* path = &play->setupPathList[ENKENDOJS_GET_PATH_INDEX(&this->actor)];

        this->pathPoints = Lib_SegmentedToVirtual(path->points);
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 30.0f;
    this->actor.child = NULL;
    this->unk_28A = false;
    this->unk_28C = 0;
    this->unk_28E = 0;
    this->unk_286 = 0;
    func_80B26538(this);
}

void EnKendoJs_Destroy(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_82_08);
}

void func_80B26538(EnKendoJs* this) {
    this->actionFunc = func_80B2654C;
}

void func_80B2654C(EnKendoJs* this, PlayState* play) {
    s32 phi_v0;
    s32 sp30;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (CURRENT_DAY != 0) {
            sp30 = CURRENT_DAY - 1;
        } else {
            sp30 = 0;
        }

        if (ENKENDOJS_GET_FF(&this->actor) == ENKENDOJS_FF_1) {
            Message_StartTextbox(play, 0x273C, &this->actor);
            this->unk_288 = 0x273C;
        } else if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_DEKU:
                    phi_v0 = 0;
                    break;

                case PLAYER_FORM_GORON:
                    phi_v0 = 1;
                    break;

                case PLAYER_FORM_ZORA:
                    phi_v0 = 2;
                    break;

                default:
                    phi_v0 = 0;
                    break;
            }

            Message_StartTextbox(play, D_80B27CE0[phi_v0][sp30], &this->actor);
            this->unk_288 = D_80B27CE0[phi_v0][sp30];
        } else if ((Player_GetMask(play) != PLAYER_MASK_NONE) && (Player_GetMask(play) < PLAYER_MASK_GIANT)) {
            u16 sp2E = Player_GetMask(play) + 0x273C;

            //! FAKE:
            if (1) {}

            Message_StartTextbox(play, sp2E, &this->actor);
            this->unk_288 = sp2E;
        } else {
            if (!this->unk_28A) {
                this->unk_28A = true;
                phi_v0 = 0;
            } else {
                phi_v0 = 1;
            }
            Message_StartTextbox(play, D_80B27CF4[phi_v0][sp30], &this->actor);
            this->unk_288 = D_80B27CF4[phi_v0][sp30];
        }

        func_80B26AE8(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void func_80B26758(EnKendoJs* this, PlayState* play) {
    if (Message_ShouldAdvance(play) && (this->unk_288 == 0x2716)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x272C, &this->actor);
                    this->unk_288 = 0x272C;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_2);
                } else if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x2718, &this->actor);
                    this->unk_288 = 0x2718;
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-play->msgCtx.unk1206C);
                    Message_StartTextbox(play, 0x2719, &this->actor);
                    this->unk_288 = 0x2719;
                }
                break;

            case 1:
                if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_NONE) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x272C, &this->actor);
                    this->unk_288 = 0x272C;
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_2);
                } else if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk12070) {
                    Audio_PlaySfx(NA_SE_SY_ERROR);
                    Message_StartTextbox(play, 0x2718, &this->actor);
                    this->unk_288 = 0x2718;
                } else {
                    Audio_PlaySfx_MessageDecide();
                    Rupees_ChangeBy(-play->msgCtx.unk12070);
                    Message_StartTextbox(play, 0x273A, &this->actor);
                    this->unk_288 = 0x273A;
                }
                break;

            case 2:
                Audio_PlaySfx_MessageCancel();
                Message_StartTextbox(play, 0x2717, &this->actor);
                this->unk_288 = 0x2717;

            default:
                break;
        }
    }
}

void func_80B269A4(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!Message_ShouldAdvance(play)) {
        return;
    }

    switch (this->unk_288) {
        case 0x2710:
        case 0x2711:
        case 0x2712:
        case 0x2713:
        case 0x2714:
        case 0x2715:
            Message_StartTextbox(play, 0x2716, &this->actor);
            this->unk_288 = 0x2716;
            break;

        case 0x2719:
            Message_CloseTextbox(play);
            player->stateFlags1 |= PLAYER_STATE1_20;
            func_80B2701C(this);
            break;

        case 0x271A:
            Message_CloseTextbox(play);
            func_80B2714C(this);
            break;

        case 0x273A:
            Message_StartTextbox(play, 0x273B, &this->actor);
            this->unk_288 = 0x273B;
            break;

        case 0x273B:
            Message_CloseTextbox(play);
            Interface_InitMinigame(play);
            player->stateFlags1 |= PLAYER_STATE1_20;
            func_80B273D0(this);
            break;

        case 0x272D:
            Message_CloseTextbox(play);
            gSaveContext.minigameStatus = MINIGAME_STATUS_END;
            func_80B276C4(this);
            func_80B276D8(this, play);
            break;

        default:
            break;
    }
}

void func_80B26AE8(EnKendoJs* this) {
    this->actionFunc = func_80B26AFC;
}

void func_80B26AFC(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            func_80B26758(this, play);
            break;

        case TEXT_STATE_EVENT:
            func_80B269A4(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                if (this->unk_288 == 0x272C) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_3);
                }

                if ((this->unk_288 == 0x272E) || (this->unk_288 == 0x272F) || (this->unk_288 == 0x2730)) {
                    gSaveContext.minigameStatus = MINIGAME_STATUS_END;
                }

                player->stateFlags1 &= ~PLAYER_STATE1_20;
                func_80B26538(this);
            }

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        default:
            break;
    }
}

s32 func_80B26BF8(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_284) {
        case 0:
            if (func_80122FCC(play)) {
                return 0;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags3 & PLAYER_STATE3_8000000) || (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return 1;
            }
            break;

        case 1:
            if ((player->stateFlags3 & PLAYER_STATE3_8000000)) {
                return 0;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return 1;
            }
            break;

        case 2:
            if (func_80122F9C(play)) {
                return 0;
            }

            if ((player->meleeWeaponState != PLAYER_MELEE_WEAPON_STATE_0) ||
                (player->stateFlags3 & PLAYER_STATE3_8000000) || (player->stateFlags2 & PLAYER_STATE2_80000)) {
                return 1;
            }
            this->unk_28E = 0;
            break;

        case 3:
            if ((this->unk_28E == 1) && ((player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_SLASH_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_RIGHT_COMBO_1H))) {
                this->unk_28E = 0;
                return 0;
            }

            if ((this->unk_28E == 1) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->unk_28E = 0;
                return 1;
            }
            break;

        case 4:
            if ((this->unk_28E == 1) && ((player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_SLASH_1H) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_FORWARD_COMBO_1H))) {
                this->unk_28E = 0;
                return 0;
            }

            if ((this->unk_28E == 1) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->unk_28E = 0;
                return 1;
            }
            break;

        case 5:
            if ((this->unk_28E == 1) && (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H)) {
                this->unk_28E = 0;
                return 0;
            }

            if ((this->unk_28E == 1) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->unk_28E = 0;
                return 1;
            }
            break;

        case 6:
            if ((this->unk_28E == 1) && ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) ||
                                         (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH))) {
                this->unk_28E = 0;
                return 0;
            }

            if ((this->unk_28E == 1) || (player->stateFlags3 & PLAYER_STATE3_8000000) ||
                (player->stateFlags2 & PLAYER_STATE2_80000)) {
                this->unk_28E = 0;
                return 1;
            }
            break;

        default:
            break;
    }
    return 2;
}

void func_80B26EB4(EnKendoJs* this, PlayState* play) {
    Message_StartTextbox(play, D_80B27D00[this->unk_284], &this->actor);
    this->unk_288 = D_80B27D00[this->unk_284];
    this->unk_284++;
}

void func_80B26F14(EnKendoJs* this, PlayState* play) {
    Message_StartTextbox(play, D_80B27D10[this->unk_284], &this->actor);
    this->unk_288 = D_80B27D10[this->unk_284];
}

s32 func_80B26F6C(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->unk_288) {
        case 0x271D:
            if (Player_GetMeleeWeaponHeld(player) != PLAYER_MELEEWEAPON_NONE) {
                Message_StartTextbox(play, 0x272A, &this->actor);
                this->unk_288 = 0x272A;
                return true;
            }
            break;

        case 0x2721:
            if (this->unk_292 != 0) {
                Message_StartTextbox(play, 0x272B, &this->actor);
                this->unk_288 = 0x272B;
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}

void func_80B2701C(EnKendoJs* this) {
    this->actionFunc = func_80B27030;
}

void func_80B27030(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp20 = this->actor.world.pos;

    sp20.z += 200.0f;

    if (EnKendoJs_MovePlayerToPos(play, sp20)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            player->stateFlags1 &= ~PLAYER_STATE1_20;
            func_80B279F0(this, play, 0);
            Message_StartTextbox(play, 0x271A, &this->actor);
            this->unk_288 = 0x271A;
            func_80B26AE8(this);
        } else {
            Actor_OfferTalk(&this->actor, play, 800.0f);
        }
    }
}

void func_80B2714C(EnKendoJs* this) {
    SET_WEEKEVENTREG(WEEKEVENTREG_82_08);
    this->unk_28C = 1;
    this->unk_290 = 0;
    this->unk_284 = 0;
    this->unk_286 = 1;
    this->actionFunc = func_80B27188;
}

void func_80B27188(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_288 == 0x2729) {
            func_80B26F14(this, play);
        } else if (!func_80B26F6C(this, play)) {
            if (this->skelAnime.animation == &object_js_Anim_00016C) {
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_3);
            }
            this->unk_286 = 2;
            Message_CloseTextbox(play);
            player->stateFlags1 &= ~PLAYER_STATE1_20;
        }
    } else if (this->unk_286 == 2) {
        this->unk_286 = 1;
    }

    if (this->unk_286 == 1) {
        switch (func_80B26BF8(this, play)) {
            case 0:
                this->unk_286 = 0;
                Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
                player->stateFlags1 |= PLAYER_STATE1_20;
                func_80B26EB4(this, play);
                break;

            case 1:
                Actor_PlaySfx(&this->actor, NA_SE_SY_ERROR);
                this->unk_286 = 0;
                player->stateFlags1 |= PLAYER_STATE1_20;
                Message_StartTextbox(play, 0x2729, &this->actor);
                this->unk_288 = 0x2729;
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_2);
                break;

            default:
                if (this->unk_28C == 0) {
                    this->unk_290++;
                    if (this->unk_290 == 30) {
                        this->unk_290 = 0;
                        func_80B279F0(this, play, 0);
                    }
                }
                break;
        }

        if ((this->skelAnime.animation == &object_js_Anim_0003DC) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENKENDOJS_ANIM_1);
        }

        if (this->unk_284 == 7) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_82_08);
            func_80B26AE8(this);
        }
    }
}

void func_80B273D0(EnKendoJs* this) {
    SET_WEEKEVENTREG(WEEKEVENTREG_82_08);
    this->unk_290 = 120;
    this->unk_284 = 0;
    this->unk_286 = 1;
    this->actionFunc = func_80B2740C;
}

void func_80B2740C(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp18 = this->actor.world.pos;

    sp18.z += 300.0f;

    if (EnKendoJs_MovePlayerToPos(play, sp18)) {
        this->unk_28C = 0;
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        this->actionFunc = func_80B274BC;
    }
}

void func_80B274BC(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_290 >= 140) {
        if (this->unk_284 == 5) {
            if (gSaveContext.minigameScore == 30) {
                Message_StartTextbox(play, 0x272D, &this->actor);
                this->unk_288 = 0x272D;
            } else {
                Message_StartTextbox(play, 0x272E, &this->actor);
                this->unk_288 = 0x272E;
            }
            player->stateFlags1 |= PLAYER_STATE1_20;
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_82_08);
            func_80B26AE8(this);
            return;
        }

        Audio_PlaySfx(NA_SE_SY_FOUND);
        func_80B279F0(this, play, ((u8)Rand_Next() % 3) + 1);
        func_80B279F0(this, play, ((u8)Rand_Next() % 3) + 4);
        this->unk_290 = 0;
        this->unk_284++;
    } else if (this->unk_290 == 120) {
        func_80B27A90(this, play);
        this->unk_290++;
    } else {
        this->unk_290++;
    }

    if (this->unk_28E == 1) {
        if ((player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_START) ||
            (player->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {
            play->interfaceCtx.minigamePoints = 3;
            if (gSaveContext.minigameScore >= 27) {
                player->stateFlags1 |= PLAYER_STATE1_20;
            }
        } else if (player->meleeWeaponAnimation == PLAYER_MWA_STAB_1H) {
            play->interfaceCtx.minigamePoints = 2;
        } else {
            play->interfaceCtx.minigamePoints = 1;
        }
        Actor_PlaySfx(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
        this->unk_28E = 0;
    }
    func_80B2783C(this, play);
}

void func_80B276C4(EnKendoJs* this) {
    this->actionFunc = func_80B276D8;
}

void func_80B276D8(EnKendoJs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        func_80B27760(this);
    } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 800.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 800.0f, 100.0f);
    }
}

void func_80B27760(EnKendoJs* this) {
    this->actionFunc = func_80B27774;
}

void func_80B27774(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE);
            Message_StartTextbox(play, 0x272F, &this->actor);
            this->unk_288 = 0x272F;
        } else {
            Message_StartTextbox(play, 0x2730, &this->actor);
            this->unk_288 = 0x2730;
        }
        func_80B26AE8(this);
        player->stateFlags1 &= ~PLAYER_STATE1_20;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void func_80B2783C(EnKendoJs* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        Camera_ChangeActorCsCamIndex(play->cameraPtrs[CAM_ID_MAIN],
                                     CutsceneManager_GetCutsceneEntry(this->actor.csId)->csCamId);
    }
}

void func_80B27880(EnKendoJs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_f0 = this->actor.world.pos.z + 70.0f;

    if ((ENKENDOJS_GET_FF(&this->actor) != ENKENDOJS_FF_1) && (player->actor.world.pos.z < temp_f0)) {
        player->actor.world.pos.z = temp_f0;
    }
}

s32 EnKendoJs_MovePlayerToPos(PlayState* play, Vec3f targetPos) {
    Player* player = GET_PLAYER(play);
    f32 distXZ;
    f32 controlStickMagnitude;
    s16 controlStickAngle;

    controlStickAngle = Math_Vec3f_Yaw(&player->actor.world.pos, &targetPos);
    distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &targetPos);

    if (distXZ < 20.0f) {
        controlStickMagnitude = 10.0f;
    } else if (distXZ < 40.0f) {
        controlStickMagnitude = 40.0f;
    } else {
        controlStickMagnitude = 80.0f;
    }

    play->actorCtx.isOverrideInputOn = true;
    Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, controlStickAngle);

    if (distXZ < 20.0f) {
        return true;
    }
    return false;
}

void func_80B279AC(EnKendoJs* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80B279F0(EnKendoJs* this, PlayState* play, s32 arg2) {
    f32 x = this->pathPoints[arg2].x;
    f32 y = this->pathPoints[arg2].y;
    f32 z = this->pathPoints[arg2].z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_MARUTA, x, y, z, 0, 0, 0, 0);
    this->unk_28C++;
}

void func_80B27A90(EnKendoJs* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_MARUTA) {
            ((EnMaruta*)actor)->unk_220 = 1;
        }
        actor = actor->next;
    }

    this->unk_28C = 0;
}

void EnKendoJs_Update(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);
    Actor_TrackPlayer(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    func_80B279AC(this, play);
    func_80B27880(this, play);
}

s32 EnKendoJs_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    if (limbIndex == OBJECT_JS_LIMB_0C) {
        rot->y -= this->headRot.y;
    }
    return false;
}

void EnKendoJs_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
}

void EnKendoJs_Draw(Actor* thisx, PlayState* play) {
    EnKendoJs* this = (EnKendoJs*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnKendoJs_OverrideLimbDraw, EnKendoJs_PostLimbDraw, &this->actor);
}

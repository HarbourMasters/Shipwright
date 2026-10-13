/*
 * File: z_en_kbt.c
 * Overlay: ovl_En_Kbt
 * Description: Zubora
 */

#include "z_en_kbt.h"
#include "BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnKbt_Init(Actor* thisx, PlayState* play);
void EnKbt_Destroy(Actor* thisx, PlayState* play);
void EnKbt_Update(Actor* thisx, PlayState* play);
void EnKbt_Draw(Actor* thisx, PlayState* play);
void EnKbt_Reset(void);

s32 func_80B33E64(PlayState* play);
s32 func_80B33E8C(PlayState* play);
void EnKbt_ChangeAnim(EnKbt* this, s16 animIndex);
Actor* func_80B3403C(PlayState* play);
void func_80B34314(EnKbt* this, PlayState* play);
void func_80B34598(EnKbt* this, PlayState* play);

typedef enum EnKbtAnimation {
    /*  0 */ ENKBT_ANIM_0,
    /*  1 */ ENKBT_ANIM_1,
    /*  2 */ ENKBT_ANIM_2,
    /*  3 */ ENKBT_ANIM_3,
    /*  4 */ ENKBT_ANIM_4,
    /*  5 */ ENKBT_ANIM_5,
    /*  6 */ ENKBT_ANIM_6,
    /*  7 */ ENKBT_ANIM_7,
    /*  8 */ ENKBT_ANIM_8,
    /*  9 */ ENKBT_ANIM_9,
    /* 10 */ ENKBT_ANIM_10,
    /* 11 */ ENKBT_ANIM_11,
    /* 12 */ ENKBT_ANIM_12,
    /* 13 */ ENKBT_ANIM_MAX
} EnKbtAnimation;

ActorProfile En_Kbt_Profile = {
    /**/ ACTOR_EN_KBT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KBT,
    /**/ sizeof(EnKbt),
    /**/ EnKbt_Init,
    /**/ EnKbt_Destroy,
    /**/ EnKbt_Update,
    /**/ EnKbt_Draw,
    /**/ EnKbt_Reset,
};

static JointIndex* origIndicies_004274;

// #region 2S2H [Port] This data originally stopped at the the entry { 0x000A, 0x000B, 0x000C }.
// The next 3 entries are set to zero because using the data the original game would have read would also have caused an
// OOB read
static JointIndex object_kbt_Anim_004274JointIndicesFixed[] = {
    { 0x0000, 0x000D, 0x0002 }, { 0x0003, 0x002B, 0x0003 }, { 0x0049, 0x0067, 0x0085 }, { 0x0000, 0x0000, 0x00A3 },
    { 0x0000, 0x0000, 0x00C1 }, { 0x00DF, 0x00FD, 0x011B }, { 0x0000, 0x0000, 0x0139 }, { 0x0000, 0x0000, 0x0157 },
    { 0x0000, 0x0000, 0x0175 }, { 0x0000, 0x0193, 0x01B1 }, { 0x01CF, 0x01ED, 0x020B }, { 0x0000, 0x0000, 0x0229 },
    { 0x0004, 0x0005, 0x0006 }, { 0x0000, 0x0000, 0x0000 }, { 0x0007, 0x0008, 0x0009 }, { 0x0000, 0x0000, 0x0247 },
    { 0x000A, 0x000B, 0x000C }, { 0x0000, 0x0000, 0x0000 }, { 0x0060, 0x0000, 0x0000 }, { 0x0000, 0x0000, 0x0000 },
};
// #endregion

void EnKbt_Init(Actor* thisx, PlayState* play) {
    EnKbt* this = (EnKbt*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    // #region 2S2H [Port] The animation object_kbt_Anim_004274 has the wrong number of joint indicies. It should have
    // 20 but only has 17. We need to do the following steps to correct this. First we must tell the animation system to
    // not load the animation when it initializes the skeleton. Then we must manually load the animation and change the
    // joint index data see the static JointIndex data above for what we used for the data. Finally we manually set the
    // animation

    SkelAnime_InitFlex(play, &this->skelAnime, &object_kbt_Skel_00DEE8, NULL, this->jointTable, this->morphTable,
                       OBJECT_KBT_LIMB_MAX);

    AnimationHeader* anim = (AnimationHeader*)ResourceMgr_LoadAnimByName(object_kbt_Anim_004274);

    origIndicies_004274 = anim->jointIndices;
    anim->jointIndices = object_kbt_Anim_004274JointIndicesFixed;
    Animation_PlayLoop(&this->skelAnime, object_kbt_Anim_004274);
    // #endregion

    this->unk_27C = 0;
    this->actor.home.rot.z = 0;
    this->unk_27E = 4;
    this->unk_27F = 0;
    this->animIndex = ENKBT_ANIM_MAX;
    this->unk_282 = 13;
    this->unk_278 = func_80B3403C(play);
    this->unk_284 = 0;
    this->actor.textId = 0;
    if (func_80B33E64(play)) {
        EnKbt_ChangeAnim(this, ENKBT_ANIM_6);
        this->unk_282 = 11;
        if (func_80B33E8C(play)) {
            this->actor.textId = 0xC50;
        } else {
            this->actor.textId = 0xC4E;
            this->unk_27C |= 1;
        }
        this->actionFunc = func_80B34314;
    } else {
        EnKbt_ChangeAnim(this, ENKBT_ANIM_0);
        this->unk_282 = 0;
        this->actionFunc = func_80B34598;
    }
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnKbt_Destroy(Actor* thisx, PlayState* play) {
}

void EnKbt_Reset(void) {
    // #region 2S2H [Port] Reset the joint indicies back to their original value from the resource.
    AnimationHeader* anim = (AnimationHeader*)ResourceMgr_LoadAnimByName(object_kbt_Anim_004274);
    anim->jointIndices = origIndicies_004274;
    // #endregion
}

s32 func_80B33E64(PlayState* play) {
    return gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 1;
}

s32 func_80B33E8C(PlayState* play) {
    if ((CURRENT_DAY == 3) ||
        ((CURRENT_DAY == 2) && (gSaveContext.save.saveInfo.permanentSceneFlags[play->sceneId].unk_14 & 2))) {
        return true;
    }
    return false;
}

void EnKbt_ChangeAnim(EnKbt* this, s16 animIndex) {
    static AnimationHeader* sAnimations[ENKBT_ANIM_MAX] = {
        &object_kbt_Anim_000670, // ENKBT_ANIM_0
        &object_kbt_Anim_001674, // ENKBT_ANIM_1
        &object_kbt_Anim_002084, // ENKBT_ANIM_2
        &object_kbt_Anim_000FE8, // ENKBT_ANIM_3
        &object_kbt_Anim_001940, // ENKBT_ANIM_4
        &object_kbt_Anim_00E7BC, // ENKBT_ANIM_5
        &object_kbt_Anim_00F0C8, // ENKBT_ANIM_6
        &object_kbt_Anim_002710, // ENKBT_ANIM_7
        &object_kbt_Anim_002DE0, // ENKBT_ANIM_8
        &object_kbt_Anim_003414, // ENKBT_ANIM_9
        &object_kbt_Anim_003D24, // ENKBT_ANIM_10
        &object_kbt_Anim_001BF4, // ENKBT_ANIM_11
        &object_kbt_Anim_002084, // ENKBT_ANIM_12
    };
    u8 animationModes[ENKBT_ANIM_MAX] = {
        ANIMMODE_LOOP, // ENKBT_ANIM_0
        ANIMMODE_LOOP, // ENKBT_ANIM_1
        ANIMMODE_ONCE, // ENKBT_ANIM_2
        ANIMMODE_LOOP, // ENKBT_ANIM_3
        ANIMMODE_ONCE, // ENKBT_ANIM_4
        ANIMMODE_LOOP, // ENKBT_ANIM_5
        ANIMMODE_LOOP, // ENKBT_ANIM_6
        ANIMMODE_LOOP, // ENKBT_ANIM_7
        ANIMMODE_LOOP, // ENKBT_ANIM_8
        ANIMMODE_LOOP, // ENKBT_ANIM_9
        ANIMMODE_LOOP, // ENKBT_ANIM_10
        ANIMMODE_ONCE, // ENKBT_ANIM_11
        ANIMMODE_ONCE, // ENKBT_ANIM_12
    };

    if (this->animIndex != animIndex) {
        if (animIndex >= ENKBT_ANIM_12) {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], -1.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]) - 1.0f, 0.0f, animationModes[animIndex],
                             -5.0f);
        } else {
            Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                             Animation_GetLastFrame(sAnimations[animIndex]), animationModes[animIndex], -5.0f);
        }
        this->animIndex = animIndex;
    }
}

Actor* func_80B3403C(PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if (npc->id == ACTOR_EN_KGY) {
            return npc;
        }
        npc = npc->next;
    }

    return NULL;
}

void func_80B34078(EnKbt* this) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_284 > 0) {
            this->unk_284--;
        } else {
            switch (this->animIndex) {
                case ENKBT_ANIM_2:
                    switch (this->unk_282) {
                        case 0:
                            EnKbt_ChangeAnim(this, ENKBT_ANIM_12);
                            break;

                        case 5:
                            this->actor.home.rot.z = 0;
                            break;

                        default:
                            EnKbt_ChangeAnim(this, ENKBT_ANIM_5);
                            break;
                    }
                    break;

                case ENKBT_ANIM_12:
                    EnKbt_ChangeAnim(this, ENKBT_ANIM_0);
                    this->actor.home.rot.z = 0;
                    break;

                case ENKBT_ANIM_4:
                    EnKbt_ChangeAnim(this, ENKBT_ANIM_1);
                    this->unk_27E = 2;
                    this->actor.home.rot.z = 0;
                    break;

                default:
                    break;
            }
        }
    }
}

void func_80B3415C(EnKbt* this) {
    if (this->unk_27C & 2) {
        if (this->unk_284 > 0) {
            this->unk_284--;
        } else {
            this->unk_27F = 0;
            this->unk_282 = 3;
            this->unk_27C &= ~2;
        }
    }

    switch (this->unk_282) {
        case 0:
            if ((this->animIndex != ENKBT_ANIM_2) && (this->animIndex != ENKBT_ANIM_12)) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_0);
            }
            break;

        case 1:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_2);
            this->unk_282 = 0;
            break;

        case 2:
            if (this->animIndex != ENKBT_ANIM_12) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_3);
            }
            break;

        case 3:
            if (this->animIndex != ENKBT_ANIM_1) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_4);
            }
            break;

        case 10:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_11);
            break;

        case 4:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_2);
            this->unk_282 = 6;
            break;

        case 6:
            if (this->animIndex != ENKBT_ANIM_2) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_5);
            }
            break;

        case 11:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_6);
            break;

        case 7:
            if (this->animIndex != ENKBT_ANIM_2) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_7);
            }
            break;

        case 8:
            if (this->animIndex != ENKBT_ANIM_2) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_8);
            }
            break;

        case 9:
            if (this->animIndex != ENKBT_ANIM_2) {
                EnKbt_ChangeAnim(this, ENKBT_ANIM_9);
            }
            break;

        case 12:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_10);
            break;

        default:
            break;
    }
}

void func_80B34314(EnKbt* this, PlayState* play) {
    func_80B34078(this);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = func_80B34598;
        Actor_ChangeFocus(&this->actor, play, this->unk_278);
        this->unk_278->textId = this->actor.textId;
        this->unk_27C &= ~4;
        if (this->actor.textId == 0xC4E) {
            this->unk_27C |= 0x10;
        }
    } else if (this->actor.xzDistToPlayer < 250.0f) {
        if ((this->unk_278 != NULL) && (this->unk_278->xzDistToPlayer < 250.0f)) {
            if (this->unk_27C & 4) {
                if ((GET_PLAYER_FORM != PLAYER_FORM_HUMAN) ||
                    GameInteractor_Should(VB_SMITHY_CHECK_FOR_SWORD,
                                          ((CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_KOKIRI) &&
                                           (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_RAZOR) &&
                                           (CUR_FORM_EQUIP(EQUIP_SLOT_B) != ITEM_SWORD_GILDED)))) {
                    this->actor.textId = 0xC38;
                } else if (CURRENT_DAY == 3) {
                    this->actor.textId = 0xC39;
                } else if (this->unk_27C & 8) {
                    this->actor.textId = 0xC3E;
                } else {
                    this->actor.textId = 0xC3A;
                }
            }

            switch (this->actor.textId) {
                case 0xC4E:
                case 0xC4F:
                case 0xC50:
                    if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
                        this->actor.textId = 0xC37;
                    }
                    break;

                case 0xC37:
                    if (GET_PLAYER_FORM == PLAYER_FORM_HUMAN) {
                        if (func_80B33E8C(play)) {
                            this->actor.textId = 0xC50;
                        } else {
                            this->actor.textId = 0xC4E;
                        }
                    }
                    break;

                default:
                    break;
            }
            Actor_OfferTalk(&this->actor, play, 260.0f);
        }
    }
    func_80B3415C(this);
}

void func_80B34574(EnKbt* this) {
    this->unk_27E = 0;
    this->unk_27F = 0;
    this->unk_282 = 6;
    this->actionFunc = func_80B34314;
}

void func_80B34598(EnKbt* this, PlayState* play) {
    func_80B34078(this);

    switch (this->actor.textId) {
        case 0x1:
            this->unk_27E = 4;
            this->unk_27F = 0;
            this->unk_282 = 0;
            break;

        case 0x2:
            func_80B34574(this);
            this->actor.textId = 0xC3E;
            this->unk_27C |= 0x4;
            this->unk_27C |= 0x8;
            return;

        case 0x3:
            func_80B34574(this);
            this->actor.textId = 0xC58;
            return;

        case 0x4:
            func_80B34574(this);
            this->actor.textId = 0xC4B;
            return;

        case 0x5:
            this->unk_27E = 4;
            this->unk_27F = 0;
            this->unk_282 = 0;
            EnKbt_ChangeAnim(this, ENKBT_ANIM_12);
            break;

        case 0x6:
            func_80B34574(this);
            this->actor.textId = 0xC3A;
            this->unk_27C |= 4;
            return;

        case 0x7:
            this->unk_27E = 4;
            this->unk_27F = 0;
            this->unk_282 = 11;
            if (func_80B33E8C(play)) {
                this->actor.textId = 0xC50;
            } else if (this->unk_27C & 0x10) {
                this->actor.textId = 0xC4F;
            } else {
                this->actor.textId = 0xC4E;
            }
            this->actionFunc = func_80B34314;
            return;

        case 0x8:
            func_80B34574(this);
            this->actor.textId = 0xC56;
            return;

        case 0xC1E:
            this->unk_27E = 2;
            this->unk_27F = 1;
            break;

        case 0xC1F:
        case 0xC2E:
            this->unk_27E = 2;
            this->unk_27F = 1;
            this->unk_282 = 1;
            this->unk_284 = 20;
            break;

        case 0xC20:
        case 0xC23:
        case 0xC2F:
            this->unk_27E = 0;
            this->unk_282 = 2;
            break;

        case 0xC21:
        case 0xC29:
            this->unk_284 = 10;
            this->unk_27C |= 2;
            break;

        case 0xC22:
        case 0xC2A:
            this->unk_27F = 1;
            break;

        case 0xC2C:
            this->unk_27E = 0;
            this->unk_27F = 0;
            this->unk_282 = 2;
            break;

        case 0xC31:
            this->unk_27E = 2;
            this->unk_27F = 1;
            this->unk_282 = 4;
            break;

        case 0xC32:
            this->unk_27E = 1;
            this->unk_282 = 7;
            break;

        case 0xC34:
            this->unk_27E = 0;
            this->unk_282 = 8;
            break;

        case 0xC35:
            this->unk_27E = 4;
            this->unk_27F = 0;
            EnKbt_ChangeAnim(this, ENKBT_ANIM_2);
            this->unk_282 = 5;
            break;

        case 0xC36:
            this->unk_27E = 1;
            this->unk_27F = 1;
            EnKbt_ChangeAnim(this, ENKBT_ANIM_7);
            this->unk_282 = 7;
            break;

        case 0xC37:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_10);
            this->unk_27E = 0;
            this->unk_27F = 1;
            break;

        case 0xC38:
        case 0xC39:
            EnKbt_ChangeAnim(this, ENKBT_ANIM_7);
            this->unk_27E = 0;
            this->unk_27F = 1;
            break;

        case 0xC3A:
        case 0xC4D:
        case 0xC58:
            this->unk_27E = 0;
            this->unk_27F = 1;
            this->unk_282 = 8;
            break;

        case 0xC3C:
        case 0xC57:
            this->unk_27E = 1;
            this->unk_282 = 7;
            break;

        case 0xC3D:
            this->unk_27E = 0;
            break;

        case 0xC3E:
            this->unk_27F = 1;
            this->unk_282 = 8;
            break;

        case 0xC3F:
        case 0xC47:
        case 0xC4C:
            this->unk_27E = 1;
            this->unk_27F = 1;
            this->unk_282 = 7;
            break;

        case 0xC40:
            this->unk_27E = 3;
            this->unk_282 = 6;
            break;

        case 0xC45:
        case 0xC4B:
        case 0xC56:
            this->unk_27E = 0;
            this->unk_27F = 1;
            this->unk_282 = 7;
            break;

        case 0xC46:
            this->unk_27E = 1;
            this->unk_27F = 1;
            this->unk_282 = 9;
            break;

        case 0xC48:
            this->unk_27E = 2;
            this->unk_27F = 0;
            this->unk_282 = 10;
            break;

        case 0xC49:
            this->unk_27F = 1;
            break;

        case 0xC4A:
            this->unk_27E = 0;
            this->unk_282 = 7;
            break;

        case 0xC4E:
            this->unk_27E = 1;
            this->unk_27F = 1;
            this->unk_282 = 12;
            break;

        case 0xC4F:
            this->unk_27E = 2;
            this->unk_27F = 1;
            this->unk_282 = 12;
            break;

        case 0xC50:
            this->unk_27E = 0;
            this->unk_27F = 1;
            this->unk_282 = 12;
            break;

        default:
            break;
    }

    this->actor.textId = 0;
    func_80B3415C(this);
}

void EnKbt_Update(Actor* thisx, PlayState* play) {
    EnKbt* this = (EnKbt*)thisx;

    this->actionFunc(this, play);
}

s32 EnKbt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKbt* this = (EnKbt*)thisx;

    if (!(this->unk_27C & 1) && (limbIndex == OBJECT_KBT_LIMB_0E)) {
        *dList = NULL;
    }
    return false;
}

Vec3f D_80B34B84 = { 500.0f, 500.0f, 0.0f };

void EnKbt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnKbt* this = (EnKbt*)thisx;

    if (limbIndex == OBJECT_KBT_LIMB_09) {
        Matrix_MultVec3f(&D_80B34B84, &this->actor.focus.pos);
    }
}

TexturePtr D_80B34B90[] = {
    object_kbt_Tex_00A5B8,
    object_kbt_Tex_00ADB8,
};

TexturePtr D_80B34B98[] = {
    object_kbt_Tex_00B5B8, object_kbt_Tex_00BDB8, object_kbt_Tex_00C5B8, object_kbt_Tex_00CDB8, object_kbt_Tex_00D5B8,
};

void EnKbt_Draw(Actor* thisx, PlayState* play) {
    EnKbt* this = (EnKbt*)thisx;
    Gfx* gfx;
    TexturePtr tex;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;

    tex = Lib_SegmentedToVirtual(D_80B34B98[this->unk_27E]);
    gSPSegment(&gfx[0], 0x08, tex);

    tex = Lib_SegmentedToVirtual(D_80B34B90[this->unk_27F]);
    gSPSegment(&gfx[1], 0x09, tex);

    POLY_OPA_DISP = &gfx[2];

    CLOSE_DISPS(play->state.gfxCtx);

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnKbt_OverrideLimbDraw, EnKbt_PostLimbDraw, &this->actor);
}

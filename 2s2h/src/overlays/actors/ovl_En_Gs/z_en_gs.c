/*
 * File: z_en_gs.c
 * Overlay: ovl_En_Gs
 * Description: Gossip stone
 */

#include "z_en_gs.h"
#include "z64voice.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_gs/object_gs.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnGs_Init(Actor* thisx, PlayState* play);
void EnGs_Destroy(Actor* thisx, PlayState* play);
void EnGs_Update(Actor* thisx, PlayState* play);
void EnGs_Draw(Actor* thisx, PlayState* play);

void func_80997D14(EnGs* this, PlayState* play);
void func_80997D38(EnGs* this, PlayState* play);
void func_80997E4C(EnGs* this, PlayState* play);
void func_80998040(EnGs* this, PlayState* play);
void func_8099807C(EnGs* this, PlayState* play);
void func_80998300(EnGs* this, PlayState* play);
void func_809984F4(EnGs* this, PlayState* play);
void func_809985B8(EnGs* this, PlayState* play);
void func_80998704(EnGs* this, PlayState* play);
void func_8099874C(EnGs* this, PlayState* play);
void func_809989B4(EnGs* this, PlayState* play);
void func_809989F4(EnGs* this, PlayState* play);
s32 func_80998A48(EnGs* this, PlayState* play);
s32 func_80998BBC(EnGs* this, PlayState* play);
s32 func_80998D44(EnGs* this, PlayState* play);
s32 func_80998F9C(EnGs* this, PlayState* play);
s32 func_809995A4(EnGs* this, PlayState* play);
void func_80999A8C(EnGs* this, PlayState* play);
void func_80999AC0(EnGs* this);

ActorProfile En_Gs_Profile = {
    /**/ ACTOR_EN_GS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GS,
    /**/ sizeof(EnGs),
    /**/ EnGs_Init,
    /**/ EnGs_Destroy,
    /**/ EnGs_Update,
    /**/ EnGs_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 21, 48, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0xE),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0xC),
    /* Zora boomerang */ DMG_ENTRY(0, 0xE),
    /* Normal arrow   */ DMG_ENTRY(0, 0xE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xE),
    /* Goron punch    */ DMG_ENTRY(0, 0xD),
    /* Sword          */ DMG_ENTRY(0, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xD),
    /* Fire arrow     */ DMG_ENTRY(0, 0xB),
    /* Ice arrow      */ DMG_ENTRY(0, 0xB),
    /* Light arrow    */ DMG_ENTRY(0, 0xB),
    /* Goron spikes   */ DMG_ENTRY(0, 0xE),
    /* Deku spin      */ DMG_ENTRY(0, 0xE),
    /* Deku bubble    */ DMG_ENTRY(0, 0xD),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xE),
    /* Zora punch     */ DMG_ENTRY(0, 0xE),
    /* Spin attack    */ DMG_ENTRY(0, 0xE),
    /* Sword beam     */ DMG_ENTRY(0, 0xE),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0xC),
};

s8 func_80997A90(s16 arg0, s16 arg1) {
    s8 phi_v1;

    if ((arg0 == 0) || ((arg0 != 1) && (arg0 != 2) && (arg0 == 3))) {
        phi_v1 = 0;
    } else {
        phi_v1 = (gSaveContext.save.saveInfo.unk_EA0 >> (arg1 * 3)) & 7;
    }
    return phi_v1;
}

void func_80997AFC(s32 idx, Color_RGB8* arg1) {
    static Color_RGB8 D_8099A3F8[] = {
        { 255, 255, 255 },
        { 150, 255, 100 },
        { 255, 80, 40 },
        { 100, 150, 255 },
    };

    *arg1 = D_8099A3F8[idx];
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void EnGs_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGs* this = (EnGs*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->unk_208 = -1;
    this->unk_204 = 1;
    this->unk_198 = this->actor.world.rot.z;
    this->unk_195 = ENGS_GET_1F(thisx);
    this->switchFlag = ENGS_GET_SWITCH_FLAG(thisx);
    this->actor.params = ENGS_GET_F000(thisx);
    this->actor.world.rot.z = 0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->unk_216 = 0;
    this->unk_218 = 0;
    this->unk_200 = 1.0f;
    this->unk_194 = func_80997A90(this->actor.params, this->unk_198);

    func_80997AFC(this->unk_194, &this->unk_1FA);
    this->unk_1F4 = this->unk_1FA;
    Math_Vec3f_Copy(&this->unk_1B0[0], &gOneVec3f);
    Math_Vec3f_Copy(&this->unk_1B0[1], &gOneVec3f);
    SubS_FillCutscenesList(&this->actor, this->csIdList, ARRAY_COUNT(this->csIdList));
    AudioVoice_InitWord(VOICE_WORD_ID_HOURS);
    if (this->actor.params == ENGS_1) {
        Actor_SetScale(&this->actor, 0.15f);
        this->collider.dim.radius *= 1.5f;
        this->collider.dim.height *= 1.5f;
    }
    func_80997D14(this, play);
}

void EnGs_Destroy(Actor* thisx, PlayState* play) {
    EnGs* this = (EnGs*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    Play_DisableMotionBlur();
}

void func_80997D14(EnGs* this, PlayState* play) {
    GameInteractor_Should(VB_EN_GS_FINISH_OCARINA_ON_RESET, true, this, play);
    this->unk_19A &= ~0x200;
    this->actionFunc = func_80997D38;
}

void func_80997D38(EnGs* this, PlayState* play) {
    static f32 D_8099A408[] = { 40.0f, 60.0f, 40.0f, 40.0f };

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
        if (this->actor.xzDistToPlayer <= D_8099A408[this->actor.params]) {
            SubS_OfferTalkExchangeFacing(&this->actor, play, D_8099A408[this->actor.params],
                                         D_8099A408[this->actor.params], PLAYER_IA_NONE, 0x2000, 0x2000);
        }
    }

    if (this->actor.params != ENGS_2) {
        Actor_OfferOcarinaInteraction(&this->actor, play, 100.0f, 100.0f);
    }
}

void func_80997DEC(EnGs* this, PlayState* play) {
    if (GameInteractor_Should(VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED, Player_GetMask(play) == PLAYER_MASK_TRUTH, this)) {
        this->unk_210 = 0x20D1;
    } else {
        this->unk_210 = 0x20D0;
    }
    this->unk_19A |= 0x200;
    this->actionFunc = func_80997E4C;
}

void func_80997E4C(EnGs* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
            Message_StartTextbox(play, this->unk_210, &this->actor);
            break;

        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x20D0:
                        func_80997D14(this, play);
                        break;

                    case 0x20D1:
                        switch (this->actor.params) {
                            case ENGS_0:
                                this->unk_210 = this->unk_195 + 0x20D3;
                                if (this->unk_210 >= 0x20D4) {
                                    s32 temp_v1 = this->unk_210 - 0x20D4;

                                    if ((this->unk_210 < 0x20E8) &&
                                        ((temp_v1 + ITEM_MASK_TRUTH) == INV_CONTENT(temp_v1 + ITEM_MASK_TRUTH))) {
                                        this->unk_210 = temp_v1 + 0x2103;
                                    }
                                }
                                break;

                            case ENGS_3:
                                this->unk_210 = this->unk_195 + 0x20B0;
                                break;

                            case ENGS_1:
                                this->unk_210 = this->unk_195 + 0x20F3;
                                break;

                            case ENGS_2:
                                this->unk_210 = this->unk_195 + 0x20F7;
                                break;

                            default:
                                break;
                        }
                        if (GameInteractor_Should(VB_GS_CONTINUE_TEXTBOX, true, this)) {
                            Message_ContinueTextbox(play, this->unk_210);
                        }
                        break;

                    default:
                        func_80997D14(this, play);
                        break;
                }
            }
            break;

        default:
            break;
    }
}

void func_80997FF0(EnGs* this, PlayState* play) {
    if (SubS_StartCutscene(&this->actor, play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA], CS_ID_NONE,
                           SUBS_CUTSCENE_NORMAL)) {
        func_80998040(this, play);
    }
}

void func_80998040(EnGs* this, PlayState* play) {
    Message_DisplayOcarinaStaff(play, OCARINA_ACTION_FREE_PLAY);
    this->actionFunc = func_8099807C;
}

void func_8099807C(EnGs* this, PlayState* play) {
    switch (play->msgCtx.ocarinaMode) {
        case OCARINA_MODE_EVENT:
            switch (play->msgCtx.lastPlayedSong) {
                case OCARINA_SONG_HEALING:
                case OCARINA_SONG_EPONAS:
                    if (!Flags_GetSwitch(play, this->switchFlag)) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                    this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0,
                                    FAIRY_PARAMS(FAIRY_TYPE_2, false, 0));
                        Actor_PlaySfx(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                        Flags_SetSwitch(play, this->switchFlag);
                    }
                    break;

                case OCARINA_SONG_STORMS:
                    if (!Flags_GetSwitch(play, this->switchFlag)) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                                    this->actor.world.pos.y + 40.0f, this->actor.world.pos.z, 0, 0, 0,
                                    FAIRY_PARAMS(FAIRY_TYPE_7, false, 0));
                        Actor_PlaySfx(&this->actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                        Flags_SetSwitch(play, this->switchFlag);
                    }
                    break;

                case OCARINA_SONG_SONATA:
                    if ((this->actor.params == ENGS_1) && (GET_PLAYER_FORM == PLAYER_FORM_DEKU)) {
                        this->unk_194 = 1;
                        this->unk_19C = 5;
                        this->unk_19A |= 1;
                        func_80999AC0(this);
                        func_809984F4(this, play);
                    }
                    break;

                case OCARINA_SONG_NEW_WAVE:
                    if ((this->actor.params == ENGS_1) && (GET_PLAYER_FORM == PLAYER_FORM_ZORA)) {
                        this->unk_194 = 3;
                        this->unk_19C = 5;
                        this->unk_19A |= 1;
                        func_80999AC0(this);
                        func_809984F4(this, play);
                    }
                    break;

                case OCARINA_SONG_GORON_LULLABY:
                    if ((this->actor.params == ENGS_1) && (GET_PLAYER_FORM == PLAYER_FORM_GORON)) {
                        this->unk_194 = 2;
                        this->unk_19C = 5;
                        this->unk_19A |= 1;
                        func_80999AC0(this);
                        func_809984F4(this, play);
                    }
                    break;

                default:
                    break;
            }
            break;

        case OCARINA_MODE_NONE:
        case OCARINA_MODE_END:
            func_80998300(this, play);

        case OCARINA_MODE_APPLY_DOUBLE_SOT:
            func_80997D14(this, play);
            break;

        default:
            break;
    }
}

void func_80998300(EnGs* this, PlayState* play) {
    if (this->actor.csId != CS_ID_NONE) {
        CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_ITEM_OCARINA]);
    }
}

f32 func_80998334(EnGs* this, PlayState* play, f32* arg2, f32* arg3, s16* arg4, f32 arg5, f32 arg6, f32 arg7, s32 arg8,
                  s32 arg9) {
    f32 sp2C = Math_SmoothStepToF(arg2, *arg3, arg5, arg6, arg7);

    if (arg9 == 0) {
        sp2C = Math_SmoothStepToF(arg2, *arg3, arg5, arg6, arg7);
        this->unk_1B0[0].x = (sinf(DEG_TO_RAD((*arg4 % arg8) * (1.0f / arg8) * 360.0f)) * *arg2) + 1.0f;
        this->unk_1B0[0].y = 1.0f - (sinf(DEG_TO_RAD((*arg4 % arg8) * (1.0f / arg8) * 360.0f)) * *arg2);
        (*arg4)++;
    }
    return sp2C;
}

void func_809984F4(EnGs* this, PlayState* play) {
    GameInteractor_Should(VB_EN_GS_BEFORE_GOSSIP_GROTTO_SEQUENCE, true, this, play);
    EnGs* gossipStone = NULL;

    do {
        gossipStone = (EnGs*)SubS_FindActor(play, &gossipStone->actor, ACTORCAT_PROP, ACTOR_EN_GS);
        if (gossipStone != NULL) {
            if ((this != gossipStone) && (this->unk_194 == gossipStone->unk_194)) {
                gossipStone->unk_19A |= 1;
                func_80999AC0(gossipStone);
                gossipStone->unk_19C = 3;
                func_80998704(gossipStone, play);
            }
            gossipStone = (EnGs*)gossipStone->actor.next;
        }
    } while (gossipStone != NULL);

    Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
    this->actionFunc = func_809985B8;
}

void func_809985B8(EnGs* this, PlayState* play) {
    EnGs* gossipStone;
    Vec3f sp38;

    if (SubS_StartCutscene(&this->actor, this->csIdList[0], CS_ID_NONE, SUBS_CUTSCENE_WITH_PLAYER)) {
        Player* player = GET_PLAYER(play);

        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
        Matrix_MultVecZ(160.0f, &sp38);
        Math_Vec3f_Sum(&player->actor.world.pos, &sp38, &player->actor.world.pos);
        Math_Vec3f_Copy(&player->actor.prevPos, &player->actor.world.pos);
        this->unk_200 = 0.0f;
        gSaveContext.save.saveInfo.unk_EA0 = ((u32)gSaveContext.save.saveInfo.unk_EA0 & ~(7 << (this->unk_198 * 3))) |
                                             ((this->unk_194 & 7) << (this->unk_198 * 3));
        gossipStone = NULL;

        do {
            gossipStone = (EnGs*)SubS_FindActor(play, &gossipStone->actor, ACTORCAT_PROP, ACTOR_EN_GS);
            if (gossipStone != NULL) {
                if ((gossipStone != this) && (gossipStone->actor.params == ENGS_2) &&
                    (gossipStone->unk_198 == this->unk_198)) {
                    gossipStone->unk_194 = this->unk_194;
                    gossipStone->unk_200 = 0.0f;
                }
                gossipStone = (EnGs*)gossipStone->actor.next;
            }
        } while (gossipStone != NULL);

        func_80998704(this, play);
    }
}

void func_80998704(EnGs* this, PlayState* play) {
    this->unk_19D = 0;
    this->unk_19A &= ~(0x100 | 0x4);
    Actor_PlaySfx(&this->actor, NA_SE_EV_G_STONE_CHANGE_COLOR);
    this->actionFunc = func_8099874C;
}

void func_8099874C(EnGs* this, PlayState* play) {
    s32 phi_v0 = 0;

    switch (this->unk_19C) {
        case 0:
            phi_v0 = func_80998A48(this, play);
            break;

        case 1:
            phi_v0 = func_80998BBC(this, play);
            break;

        case 2:
            phi_v0 = func_80998D44(this, play);
            break;

        case 3:
        case 5:
            phi_v0 = func_80998F9C(this, play);
            break;

        case 4:
            phi_v0 = func_809995A4(this, play);
            break;

        default:
            break;
    }

    if (phi_v0 == 0) {
        this->unk_216 = 0;
        if ((this->unk_19C == 5) && (this->unk_194 != 0)) {
            s32 i;

            CutsceneManager_Stop(this->csIdList[0]);
            phi_v0 = 1;

            for (i = 0; i < 4; i++) {
                if (((gSaveContext.save.saveInfo.unk_EA0 >> (i * 3)) & 7) != (u32)this->unk_194) {
                    phi_v0 = 0;
                }
            }

            if (phi_v0 != 0) {
                this->getItemId = -1;
                switch (this->unk_194) {
                    case 1:
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_77_08)) {
                            this->getItemId = GI_RUPEE_SILVER;
                            SET_WEEKEVENTREG(WEEKEVENTREG_77_08);
                        }
                        break;

                    case 3:
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_77_10)) {
                            this->getItemId = GI_RUPEE_SILVER;
                            SET_WEEKEVENTREG(WEEKEVENTREG_77_10);
                        }
                        break;

                    case 2:
                        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_77_20)) {
                            this->getItemId = GI_RUPEE_SILVER;
                            SET_WEEKEVENTREG(WEEKEVENTREG_77_20);
                        }
                        break;

                    default:
                        break;
                }

                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GOSSIP_STONE_GROTTO_HEART_PIECE)) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GOSSIP_STONE_GROTTO_HEART_PIECE);
                    this->getItemId = GI_HEART_PIECE;
                }

                if (this->getItemId > GI_NONE) {
                    func_809989B4(this, play);
                } else {
                    func_80997D14(this, play);
                }
            } else {
                func_80997D14(this, play);
            }
        } else {
            func_80997D14(this, play);
        }
    }
}

void func_809989B4(EnGs* this, PlayState* play) {
    Actor_OfferGetItem(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer, this->actor.playerHeightRel);
    this->actionFunc = func_809989F4;
}

void func_809989F4(EnGs* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        func_80997D14(this, play);
    } else {
        Actor_OfferGetItem(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer,
                           this->actor.playerHeightRel);
    }
}

s32 func_80998A48(EnGs* this, PlayState* play) {
    s32 sp3C = -1;

    if (this->unk_19D == 0) {
        this->unk_216 = 200;
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);

        this->unk_1D4 = 0;
        this->unk_19A |= 1;
        this->quakeY = 5;
        this->quakeDuration = 40;
        this->unk_197++;
        this->unk_197 &= 0xF;
        this->unk_19D = 1;

        this->unk_1DC = 0.5f;
        this->unk_1E0 = 0.0f;
    } else if (this->unk_19D == 1) {
        if (func_80998334(this, play, &this->unk_1DC, &this->unk_1E0, &this->unk_1D4, 0.8f, 0.007f, 0.001f, 7, 0) ==
            0.0f) {
            if ((this->actor.params != ENGS_0) && !Play_InCsMode(play) &&
                (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->unk_216 = 0;
                Actor_PlaySfx(&this->actor, NA_SE_EV_FAIVE_LUPY_COUNT);
                Message_StartTextbox(play, 0x20D2, NULL);
            }
            this->unk_19A &= ~1;
            sp3C = 0;
        }
    }
    return sp3C;
}

s32 func_80998BBC(EnGs* this, PlayState* play) {
    s32 sp34 = -1;

    if (this->unk_19D == 0) {
        this->unk_216 = 200;
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->unk_1DC = 0.3f;
        this->unk_1E0 = 0.0f;

        this->unk_1D4 = 0;
        this->unk_19A |= 1;
        this->quakeY = 5;
        this->quakeDuration = 40;
        this->unk_197--;
        this->unk_197 &= 0xF;
        this->unk_19D = 1;
    } else if (this->unk_19D == 1) {
        this->unk_19E[0].z = DEG_TO_BINANG((this->unk_1D4 % 8) * 0.125f * 360.0f);
        this->unk_19E[1].z = -this->unk_19E[0].z;
        if (func_80998334(this, play, &this->unk_1DC, &this->unk_1E0, &this->unk_1D4, 0.8f, 0.005f, 0.001f, 7, 0) ==
            0.0f) {
            this->unk_19A &= ~1;
            sp34 = 0;
        }
    }
    return sp34;
}

s32 func_80998D44(EnGs* this, PlayState* play) {
    s32 sp3C = -1;
    f32 step;

    if (this->unk_19D == 0) {
        this->unk_216 = 200;
        this->unk_1DC = this->unk_1B0[0].y - 1.0f;
        this->unk_1E0 = -0.8f;
        Actor_PlaySfx(&this->actor, NA_SE_EV_G_STONE_CRUSH);
        this->unk_19A |= 1;
        this->quakeY = 40;
        this->quakeDuration = 11;
        this->unk_19D++;
    } else if (this->unk_19D == 1) {
        step = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 1.0f, 0.4f, 0.001f);
        this->unk_1B0[0].y = this->unk_1DC + 1.0f;
        if (step == 0.0f) {
            this->unk_216 = 0;
            this->unk_1D4 = 0;
            this->unk_19D++;
        }
    } else if (this->unk_19D == 2) {
        this->unk_1D4++;
        if (this->unk_1D4 >= 100) {
            this->unk_216 = 200;
            this->unk_19D++;
            this->unk_1DC = this->unk_1B0[0].y - 1.0f;
            this->unk_1E0 = 0.0f;
        }
    } else if (this->unk_19D == 3) {
        step = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 1.0f, 0.5f, 0.001f);
        this->unk_1B0[0].y = this->unk_1DC + 1.0f;
        if (step == 0.0f) {
            this->unk_216 = 0;
            this->unk_1E0 = 0.0f;
            this->unk_1D4 = 0;
            this->quakeY = 10;
            this->quakeDuration = 10;
            this->unk_1DC = 0.5f;
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->unk_19D += 1;
        }
    } else if ((this->unk_19D == 4) && (func_80998334(this, play, &this->unk_1DC, &this->unk_1E0, &this->unk_1D4, 1.0f,
                                                      0.03f, 0.001f, 5, 0) == 0.0f)) {
        this->unk_19A &= ~1;
        sp3C = 0;
    }
    return sp3C;
}

s32 func_80998F9C(EnGs* this, PlayState* play) {
    s32 sp4C = -1;
    f32 sp48;
    f32 sp44;
    f32 sp40;
    s32 phi_v0_2;
    u16 sp3A = this->unk_19E[0].y;

    if (this->unk_19D == 0) {
        this->unk_1DC = 0;
        this->unk_1E0 = 90.0f;
        this->unk_1E4 = 0.1f;
        this->unk_1E8 = 2.0f;
        this->unk_1EC = 0.0f;

        this->unk_19D = 1;
        this->quakeY = 2;
        this->quakeDuration = 40;
        this->unk_216 = 200;
    }

    if (this->unk_19D == 1) {
        Math_SmoothStepToF(&this->unk_1E4, this->unk_1E8, 1.0f, 0.1f, 0.001f);
        sp48 = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 1.0f, this->unk_1E4, 0.001f);
        this->unk_19E[0].y += (s32)DEG_TO_BINANG_ALT3(this->unk_1DC);
        if (sp48 == 0.0f) {
            this->unk_1D4 = 0;
            this->unk_19D = 2;
        }
    }

    if (this->unk_19D == 2) {
        this->unk_19E[0].y += (s32)DEG_TO_BINANG_ALT3(this->unk_1DC);
        if (this->unk_1D4++ > 40) {
            this->unk_1DC = this->unk_1B0[0].y - 1.0f;
            this->unk_1E0 = 1.5f;
            this->unk_1E4 = this->unk_1B0[1].y - 1.0f;
            this->unk_1E8 = -0.3f;
            Actor_PlaySfx(&this->actor, NA_SE_EV_STONE_GROW_UP);
            this->unk_19D = 3;
        }
    }

    if (this->unk_19D == 3) {
        this->unk_19E[0].y += 0x4000;
        sp48 = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 0.8f, 0.2f, 0.001f);
        Math_SmoothStepToF(&this->unk_1E4, this->unk_1E8, 0.8f, 0.2f, 0.001f);
        this->unk_1B0[0].x = this->unk_1E4 + 1.0f;
        this->unk_1B0[0].y = this->unk_1DC + 1.0f;
        if (sp48 == 0.0f) {
            this->unk_1DC = 2.0f * M_PIf / 9.0000002;
            this->unk_1E0 = M_PIf / 9.0000002;
            this->unk_19D = 4;
        }
    }

    if (this->unk_19D == 4) {
        sp48 = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 0.8f, 16384.0f, 3640.0f);
        this->unk_19E[0].y += TRUNCF_BINANG(this->unk_1DC);
        if (sp48 == 0.0f) {
            phi_v0_2 = this->unk_19E[0].y;
            if (phi_v0_2 > 0) {
                phi_v0_2 += 0xFFFF0000;
            }
            this->unk_1DC = phi_v0_2;
            this->unk_1E0 = 0;
            this->unk_19D = 5;
        }
    }

    if (this->unk_19D == 5) {
        phi_v0_2 = this->unk_19E[0].y;
        if (phi_v0_2 > 0) {
            phi_v0_2 += 0xFFFF0001;
        }
        this->unk_1DC = phi_v0_2;

        sp48 = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 0.8f, 3640.0f, 0.001f);
        this->unk_19E[0].y = (s32)this->unk_1DC;
        if (sp48 == 0.0f) {
            this->unk_19E[0].y = 0;
            this->unk_1D4 = 0;
            this->unk_1DC = this->unk_1B0[0].y - 1.0f;
            this->unk_1E0 = 0.0f;
            this->unk_1E4 = this->unk_1B0[0].x - 1.0f;
            this->unk_1E8 = 0.0f;
            this->unk_1F0 = 0.0f;
            this->unk_1EC = 0.5f;
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKID_ATTACK);
            this->quakeY = 20;
            this->quakeDuration = 2;
            this->unk_19D = 6;
        }
    }

    if (this->unk_19D == 6) {
        sp48 = Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 0.8f, 0.1f, 0.001f);
        sp44 = Math_SmoothStepToF(&this->unk_1E4, this->unk_1E8, 0.8f, 0.1f, 0.001f);
        sp40 = Math_SmoothStepToF(&this->unk_1EC, this->unk_1F0, 0.8f, 0.02f, 0.001f);
        this->unk_1B0[0].x = this->unk_1E4 + 1.0f;
        this->unk_1B0[0].y = this->unk_1DC + 1.0f;
        this->unk_1B0[0].x += sinf(DEG_TO_RAD((this->unk_1D4 % 10) * 0.1f * 360.0f)) * this->unk_1EC;
        this->unk_1B0[0].y += sinf(DEG_TO_RAD((this->unk_1D4 % 10) * 0.1f * 360.0f)) * this->unk_1EC;
        this->unk_1D4++;
        if ((sp48 == 0.0f) && (sp44 == 0.0f) && (sp40 == 0.0f)) {
            this->unk_216 = 0;
            sp4C = 0;
        }
    }

    if ((u16)this->unk_19E[0].y < sp3A) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_STONE_ROLLING);
    }

    return sp4C;
}

void func_80999584(Color_RGB8* arg0, Color_RGB8* arg1) {
    arg0->r = arg1->r;
    arg0->g = arg1->g;
    arg0->b = arg1->b;
}

s32 func_809995A4(EnGs* this, PlayState* play) {
    static Color_RGB8 sFlashColours[] = {
        { 255, 50, 50 },
        { 50, 50, 255 },
        { 255, 255, 255 },
    };
    static Vec3f sDustAccel = { 0.0f, -0.3f, 0.0f };
    static Color_RGBA8 sDustPrimColor = { 200, 200, 200, 128 };
    static Color_RGBA8 sDustEnvColor = { 100, 100, 100, 0 };
    static Vec3f sBomb2Velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sBomb2Accel = { 0.0f, 0.0f, 0.0f };
    s32 sp7C = -1;

    if (this->unk_19D == 0) {
        this->unk_19A |= 0x100;
        this->unk_1D4 = 40;
        this->unk_19D++;
    }

    if (this->unk_19D == 1) {
        if (this->unk_1D4-- == 0) {
            this->unk_1D4 = 80;
            this->unk_19A |= 4;
            this->unk_19D++;
        }
    }

    if (this->unk_19D == 2) {
        u8 pad;

        this->unk_1D4--;
        func_80999584(&this->unk_1FA, &sFlashColours[2]);
        if (this->unk_1D4 < 80) {
            if ((this->unk_1D4 % 20) < 8) {
                if (this->unk_1D4 < 20) {
                    if ((this->unk_1D4 % 20) == 7) {
                        func_80999584(&this->unk_1FA, &sFlashColours[0]);
                        this->unk_1F4 = this->unk_1FA;
                        Audio_PlaySfx(NA_SE_SY_WARNING_COUNT_E);
                        this->unk_200 = 0.0f;
                    }
                } else if ((this->unk_1D4 % 20) == 7) {
                    func_80999584(&this->unk_1FA, &sFlashColours[1]);
                    this->unk_1F4 = this->unk_1FA;
                    Audio_PlaySfx(NA_SE_SY_WARNING_COUNT_N);
                    this->unk_200 = 0.0f;
                }
            }
        }

        if (this->unk_1D4 <= 0) {
            this->unk_19A &= ~4;
            this->quakeY = 3;
            this->quakeDuration = 40;
            this->unk_1D4 = 0;
            this->unk_19D++;
        }
    }

    if (this->unk_19D == 3) {
        u8 i;
        Vec3f sp6C;
        Vec3f sp60;

        for (i = 0; i < 3; i++) {
            sp60.x = Rand_CenteredFloat(15.0f);
            sp60.y = Rand_ZeroFloat(-1.0f);
            sp60.z = Rand_CenteredFloat(15.0f);

            sp6C.x = this->actor.world.pos.x + (2.0f * sp60.x);
            sp6C.y = this->actor.world.pos.y + 7.0f;
            sp6C.z = this->actor.world.pos.z + (2.0f * sp60.z);

            func_800B0EB0(play, &sp6C, &sp60, &sDustAccel, &sDustPrimColor, &sDustEnvColor,
                          Rand_ZeroFloat(50.0f) + 200.0f, 40, 15);
        }

        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);

        if (this->unk_1D4++ >= 40) {
            this->unk_19A |= 0x10;

            this->actor.cullingVolumeDistance = 12000.0f;
            this->actor.gravity = 0.3f;
            this->unk_1DC = 0.0f;

            this->unk_19D++;
            this->quakeY = 5;
            this->quakeDuration = 20;
            this->unk_19A |= 1;
            this->unk_216 = 200;
            this->unk_1E0 = (this->unk_197 >> 2) * 0x444;
        }
    }

    if (this->unk_19D == 4) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 60.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
        if (this->actor.bgCheckFlags & (BGCHECKFLAG_WALL | BGCHECKFLAG_CEILING)) {
            Vec3f sp54;

            sp54.x = this->actor.world.pos.x;
            sp54.y = this->actor.world.pos.y;
            sp54.z = this->actor.world.pos.z;
            Actor_PlaySfx(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
            EffectSsBomb2_SpawnLayered(play, &sp54, &sBomb2Velocity, &sBomb2Accel, 100, 20);
            this->unk_1D4 = 10;
            this->unk_19A |= 8;
            this->unk_216 = 0;
            this->actionFunc = func_80999A8C;
        } else {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_STONE_LAUNCH - SFX_FLAG);
        }

        Actor_MoveWithGravity(&this->actor);
        Math_SmoothStepToF(&this->unk_1DC, this->unk_1E0, 0.5f, 364.0f, 0.0f);

        this->unk_19E[1].y += TRUNCF_BINANG(this->unk_1DC);

        if ((this->actor.world.pos.y - this->actor.home.pos.y) >= 4000.0f) {
            this->unk_216 = 0;
        }

        if (this->actor.playerHeightRel < -12000.0f) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, BOMB_EXPLOSIVE_TYPE_BOMB, this->actor.world.rot.y, 0, BOMB_TYPE_BODY);
            Actor_Kill(&this->actor);
            sp7C = 0;
        }
    }

    return sp7C;
}

void func_80999A8C(EnGs* this, PlayState* play) {
    if (this->unk_1D4-- <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80999AC0(EnGs* this) {
    static Vec3s D_8099A450 = { 0, 0, 0 };
    static Vec3f D_8099A458 = { 1.0f, 1.0f, 1.0f };
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_19E); i++) {
        this->unk_19E[i] = D_8099A450;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_1B0); i++) {
        this->unk_1B0[i] = D_8099A458;
    }
}

void func_80999B34(EnGs* this) {
    if (this->unk_216 > 0) {
        if (this->unk_218 == 0) {
            Play_EnableMotionBlur(this->unk_216);
            this->unk_218 = this->unk_216;
        } else {
            Play_SetMotionBlurAlpha(this->unk_218);
        }
    } else if (this->unk_218 > 0) {
        Math_StepToS(&this->unk_218, this->unk_216, 20);
        Play_SetMotionBlurAlpha(this->unk_218);
        if (this->unk_218 <= 0) {
            Play_DisableMotionBlur();
        }
    }
}

void func_80999BC8(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGs* this = (EnGs*)thisx;
    s32 pad;

    if (this->actor.isLockedOn && (AudioVoice_GetWord() == VOICE_WORD_ID_HOURS)) {
        this->unk_19D = 0;
        this->unk_19A |= 1;
        func_80999AC0(this);
        this->unk_19C = 0;
        func_80998704(this, play);
        this->unk_208 = 0;
    }

    if (this->actor.params == ENGS_1) {
        Actor_SetFocus(&this->actor, 34.5f);
    } else {
        Actor_SetFocus(&this->actor, 23.0f);
    }

    if (this->unk_21A > 0) {
        this->unk_21A--;
        if (this->unk_21A <= 0) {
            this->unk_21A = 0;
        }
    }

    if (!(this->unk_19A & (0x200 | 0x10)) && (this->unk_21A == 0)) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->unk_19D = 0;
            this->collider.base.acFlags &= ~AC_HIT;

            switch (this->actor.colChkInfo.damageEffect) {
                case 15:
                    this->unk_19A |= 1;
                    func_80999AC0(this);
                    this->unk_19C = 0;
                    func_80998704(this, play);
                    this->unk_21A = 5;
                    break;

                case 14:
                    this->unk_19A |= 1;
                    func_80999AC0(this);
                    this->unk_19C = 1;
                    func_80998704(this, play);
                    this->unk_21A = 5;
                    break;

                case 13:
                    func_80999AC0(this);
                    this->unk_19C = 2;
                    func_80998704(this, play);
                    this->unk_21A = 5;
                    break;

                case 12:
                    this->unk_19A |= 2;
                    func_80999AC0(this);
                    this->unk_19C = 4;
                    func_80998704(this, play);
                    this->unk_21A = 5;
                    break;

                case 11:
                    this->unk_19A |= 1;
                    func_80999AC0(this);
                    this->unk_19C = 3;
                    func_80998704(this, play);
                    this->unk_21A = 5;
                    break;
            }
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    this->actionFunc(this, play);
}

void EnGs_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGs* this = (EnGs*)thisx;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        play->msgCtx.msgMode = MSGMODE_NONE;
        play->msgCtx.msgLength = 0;
        this->collider.base.acFlags &= ~AC_HIT;
        func_80997DEC(this, play);
    } else if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
        this->unk_19A |= 0x200;
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->actor.csId != CS_ID_NONE) {
            this->actionFunc = func_80997FF0;
        } else {
            func_80998040(this, play);
        }
    } else {
        s16 screenPosX;
        s16 screenPosY;

        if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) || (this->unk_19A & 0x100) ||
            (this->unk_19A & 0x200)) {
            func_80999BC8(&this->actor, play);
            Actor_GetScreenPos(play, &this->actor, &screenPosX, &screenPosY);
            if ((this->actor.xyzDistToPlayerSq > SQ(400.0f)) || (screenPosX < 0) || (screenPosX > SCREEN_WIDTH) ||
                (screenPosY < 0) || (screenPosY > SCREEN_HEIGHT)) {
                this->unk_216 = 0;
            } else if (this->quakeY > 0) {
                Actor_RequestQuakeAndRumble(&this->actor, play, this->quakeY, this->quakeDuration);
            }
        } else {
            this->unk_216 = 0;
        }

        if (this->unk_200 < 1.0f) {
            if (this->unk_19A & 4) {
                Math_StepToF(&this->unk_200, 1.0f, 0.06f);
            } else {
                Math_StepToF(&this->unk_200, 1.0f, 0.02f);
            }
            func_80997AFC(this->unk_194, &this->unk_1F7);
            Color_RGB8_Lerp(&this->unk_1F4, &this->unk_1F7, this->unk_200, &this->unk_1FA);
            if (this->unk_200 >= 1.0f) {
                this->unk_1F4 = this->unk_1FA;
            }
        }

        if (this->quakeY > 0) {
            this->quakeY = 0;
        }

        func_80999B34(this);
    }
}

void EnGs_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGs* this = (EnGs*)thisx;
    u32 frames;

    if (this->unk_19A & 8) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    frames = play->gameplayFrames;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Push();

    if (this->unk_19A & 1) {
        Matrix_RotateYS(this->unk_19E[0].y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_19E[0].x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_19E[0].z, MTXMODE_APPLY);
        Matrix_Scale(this->unk_1B0[0].x, this->unk_1B0[0].y, this->unk_1B0[0].z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_19E[1].y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_19E[1].x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_19E[1].z, MTXMODE_APPLY);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gGossipStoneMaterialDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->unk_1FA.r, this->unk_1FA.g, this->unk_1FA.b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gGossipStoneDL);
    gSPDisplayList(POLY_OPA_DISP++, gGossipStoneBottomModelDL);

    Matrix_Pop();

    if (this->unk_19A & 2) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(0.05f, -0.05f, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPSegment(
            POLY_XLU_DISP++, 0x08,
            Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, -frames * 20, 0x20, 0x80, 0, 0, 0, -20));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

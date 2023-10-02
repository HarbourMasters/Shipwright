/*
 * File: z_en_heishi2.c
 * Overlay: ovl_En_Heishi2
 * Description: Hyrulian Guards
 */

#include "vt.h"
#include "z_en_heishi2.h"
#include "objects/object_sd/object_sd.h"
#include "objects/object_link_child/object_link_child.h"
#include "overlays/actors/ovl_Bg_Gate_Shutter/z_bg_gate_shutter.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY)

void EnHeishi2_Init(Actor* thisx, PlayState* play);
void EnHeishi2_Destroy(Actor* thisx, PlayState* play);
void EnHeishi2_Update(Actor* thisx, PlayState* play);
void EnHeishi2_Draw(Actor* thisx, PlayState* play);

void EnHeishi2_DrawKingGuard(Actor* thisx, PlayState* play);
void EnHeishi2_DoNothing1(EnHeishi2* this, PlayState* play);
void EnHeishi_DoNothing2(EnHeishi2* this, PlayState* play);
void func_80A531E4(EnHeishi2* this, PlayState* play);
void func_80A53278(EnHeishi2* this, PlayState* play);
void func_80A5344C(EnHeishi2* this, PlayState* play);
void func_80A54954(EnHeishi2* this, PlayState* play);
void func_80A53538(EnHeishi2* this, PlayState* play);
void func_80A535BC(EnHeishi2* this, PlayState* play);
void func_80A5399C(EnHeishi2* this, PlayState* play);
void func_80A53638(EnHeishi2* this, PlayState* play);
void func_80A5372C(EnHeishi2* this, PlayState* play);
void func_80A5475C(EnHeishi2* this, PlayState* play);
void func_80A53AD4(EnHeishi2* this, PlayState* play);
void func_80A53C0C(EnHeishi2* this, PlayState* play);
void func_80A53C90(EnHeishi2* this, PlayState* play);
void func_80A53D0C(EnHeishi2* this, PlayState* play);
void func_80A544AC(EnHeishi2* this, PlayState* play);
void func_80A53908(EnHeishi2* this, PlayState* play);
void func_80A53F30(EnHeishi2* this, PlayState* play);
void func_80A54038(EnHeishi2* this, PlayState* play);
void func_80A5427C(EnHeishi2* this, PlayState* play);
void func_80A549E8(EnHeishi2* this, PlayState* play);
void func_80A53850(EnHeishi2* this, PlayState* play);
void func_80A54320(EnHeishi2* this, PlayState* play);
void func_80A543A0(EnHeishi2* this, PlayState* play);
void func_80A5455C(EnHeishi2* this, PlayState* play);
void func_80A546DC(EnHeishi2* this, PlayState* play);
void func_80A541FC(EnHeishi2* this, PlayState* play);
void func_80A53DF8(EnHeishi2* this, PlayState* play);

const ActorInit En_Heishi2_InitVars = {
    ACTOR_EN_HEISHI2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SD,
    sizeof(EnHeishi2),
    (ActorFunc)EnHeishi2_Init,
    (ActorFunc)EnHeishi2_Destroy,
    (ActorFunc)EnHeishi2_Update,
    (ActorFunc)EnHeishi2_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 33, 40, 0, { 0, 0, 0 } },
};

void EnHeishi2_Init(Actor* thisx, PlayState* play) {
    ColliderCylinder* collider;
    EnHeishi2* this = (EnHeishi2*)thisx;

    Actor_SetScale(&this->actor, 0.01f);
    this->type = this->actor.params & 0xFF;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    if ((this->type == 6) || (this->type == 9)) {
        this->actor.draw = EnHeishi2_DrawKingGuard;
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, 6);
        if (this->type == 6) {
            this->actionFunc = EnHeishi2_DoNothing1;

        } else {
            osSyncPrintf("\n\n");
            // "No, I'm completely disappointed" (message for when shooting guard window in courtyard)
            osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ いやー ついうっかり ☆☆☆☆☆ \n" VT_RST);

            Actor_SetScale(&this->actor, 0.02f);

            this->unk_274 = this->actor.world.pos;
            this->actor.world.rot.y = 0x7918;
            this->actor.world.pos.x += 90.0f;
            this->actor.world.pos.y -= 60.0f;
            this->actor.world.pos.z += 90.0f;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            Collider_DestroyCylinder(play, &this->collider);
            func_8002DF54(play, 0, 8);
            this->actor.flags |= ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_UPDATE_WHILE_CULLED;
            this->actionFunc = func_80A544AC;
        }
    } else {
        this->unk_2E0 = 60.0f;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        SkelAnime_Init(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, this->jointTable,
                       this->morphTable, 17);
        collider = &this->collider;
        Collider_InitCylinder(play, collider);
        Collider_SetCylinder(play, collider, &this->actor, &sCylinderInit);
        this->collider.dim.yShift = 0;
        this->collider.dim.radius = 15;
        this->collider.dim.height = 70;
        this->actor.targetMode = 6;

        switch (this->type) {

            case 2:
                this->actionFunc = func_80A531E4;
                this->actor.gravity = -1.0f;
                break;
            case 5:
                this->actionFunc = func_80A53908;
                this->actor.gravity = -1.0f;
                break;
            case 6:
                osSyncPrintf("\n\n");
                // "Peep hole soldier!"
                osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 覗き穴奥兵士ふぃ〜 ☆☆☆☆☆ \n" VT_RST);
                Collider_DestroyCylinder(play, collider);
                this->actor.flags &= ~(ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY);
                this->actionFunc = EnHeishi_DoNothing2;
                break;
        }

        this->unk_2F0 = (this->actor.params >> 8) & 0xFF;
        osSyncPrintf("\n\n");
        // "Soldier Set 2 Completed!"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 兵士２セット完了！ ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        // "Identification Completed!"
        osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 識別完了！         ☆☆☆☆☆ %d\n" VT_RST, this->type);
        // "Message completed!"
        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ メッセージ完了！   ☆☆☆☆☆ %x\n\n" VT_RST, (this->actor.params >> 8) & 0xF);
    }
}

void EnHeishi2_Destroy(Actor* thisx, PlayState* play) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    if ((this->collider.dim.radius != 0) || (this->collider.dim.height != 0)) {
        Collider_DestroyCylinder(play, &this->collider);
    }

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnHeishi2_DoNothing1(EnHeishi2* this, PlayState* play) {
}

void EnHeishi_DoNothing2(EnHeishi2* this, PlayState* play) {
}

void func_80A531E4(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = func_80A53278;
}

void func_80A53278(EnHeishi2* this, PlayState* play) {
    this->unk_30B = 0;
    this->unk_30E = 0;
    if (Text_GetFaceReaction(play, 5) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, 5);
        this->unk_30B = 1;
        this->unk_300 = TEXT_STATE_DONE;
        this->actionFunc = func_80A5475C;
    } else if ((Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) && (Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) &&
               (Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP))) {
        // "Get all spiritual stones!"
        osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 全部の精霊石GET！ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7006;
        this->actionFunc = func_80A5475C;
    } else if (!IS_DAY) {
        // "Sleep early for children!"
        osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 子供ははやくネロ！ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7002;
        this->actionFunc = func_80A5475C;

    } else if (this->unk_30C != 0) {
        // "Anything passes"
        osSyncPrintf(VT_FGCOL(BLUE) " ☆☆☆☆☆ なんでも通るよ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7099;
        this->actionFunc = func_80A5475C;
    } else if (Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_POCKET_EGG)) {
        if (this->unk_30E == 0) {
            // "Start under the first sleeve!"
            osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ １回目袖の下開始！ ☆☆☆☆☆ \n" VT_RST);
            this->actor.textId = 0x7071;
            this->unk_30E = 1;
        } else {
            // "Start under the second sleeve!"
            osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ ２回目袖の下開始！ ☆☆☆☆☆ \n" VT_RST);
            this->actor.textId = 0x7072;
        }
        this->unk_300 = TEXT_STATE_CHOICE;
        this->actionFunc = func_80A5475C;

    } else {
        // "That's okay"
        osSyncPrintf(VT_FGCOL(CYAN) " ☆☆☆☆☆ それはとおらんよぉ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7029;
        this->actionFunc = func_80A5475C;
    }
}

void func_80A5344C(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        this->unk_300 = TEXT_STATE_EVENT;
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (gSaveContext.rupees >= 10) {
                    Rupees_ChangeBy(-10);
                    this->actor.textId = 0x7098;
                    this->actionFunc = func_80A53538;
                    break;
                } else {
                    this->actor.textId = 0x7097;
                    this->actionFunc = func_80A54954;
                    break;
                }
            case 1:
                this->actor.textId = 0x7096;
                this->actionFunc = func_80A54954;
                break;

            default:
                break;
        }
        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void func_80A53538(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_300 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        func_8002DF54(play, NULL, 8);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = func_80A535BC;
    }
}

void func_80A535BC(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Animation_Change(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = func_80A53638;
}

void func_80A53638(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount = this->skelAnime.curFrame;

    BgSpot15Saku* actor = (BgSpot15Saku*)play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    SkelAnime_Update(&this->skelAnime);
    if ((frameCount >= 12.0f) && (!this->audioFlag)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_SPEAR_HIT);
        this->audioFlag = 1;
    }
    if (this->unk_2EC <= frameCount) {
        while (actor != NULL) {
            if (actor->dyna.actor.id != ACTOR_BG_SPOT15_SAKU) {
                actor = (BgSpot15Saku*)(actor->dyna.actor.next);
            } else {
                this->gate = &actor->dyna.actor;
                actor->unk_168 = 1;
                break;
            }
        }
        // "I've come!"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆ きたきたきたぁ！ ☆☆☆ %x\n" VT_RST, actor->dyna.actor.next);
        this->actionFunc = func_80A5372C;
    }
}

void func_80A5372C(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->unk_2F2[0] = 200;
    this->cameraId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->cameraId, CAM_STAT_ACTIVE);
    this->unk_280.x = 947.0f;
    this->unk_280.y = 1195.0f;
    this->unk_280.z = 2682.0f;

    this->unk_28C.x = 1164.0f;
    this->unk_28C.y = 1145.0f;
    this->unk_28C.z = 3014.0f;

    Play_CameraSetAtEye(play, this->cameraId, &this->unk_280, &this->unk_28C);
    this->actionFunc = func_80A53850;
}

void func_80A53850(EnHeishi2* this, PlayState* play) {
    BgSpot15Saku* gate;

    SkelAnime_Update(&this->skelAnime);
    Play_CameraSetAtEye(play, this->cameraId, &this->unk_280, &this->unk_28C);
    gate = (BgSpot15Saku*)this->gate;
    if ((this->unk_2F2[0] == 0) || (gate->unk_168 == 0)) {
        Play_ClearCamera(play, this->cameraId);
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        Message_CloseTextbox(play);
        this->unk_30C = 1;
        func_8002DF54(play, NULL, 7);
        this->actionFunc = func_80A531E4;
    }
}

void func_80A53908(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = func_80A5399C;
}

void func_80A5399C(EnHeishi2* this, PlayState* play) {
    s16 var;

    this->unk_30B = 0;
    var = 0;
    if (Flags_GetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD)) {
        if (!Flags_GetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK)) {
            if (Player_GetMask(play) == PLAYER_MASK_KEATON) {
                if (this->unk_309 == 0) {
                    this->actor.textId = 0x200A;
                } else {
                    this->actor.textId = 0x200B;
                }
                this->unk_300 = TEXT_STATE_CHOICE;
                this->unk_30B = 1;
                var = 1;
            } else {
                this->actor.textId = 0x2016;
                this->unk_300 = TEXT_STATE_DONE;
                var = 1;
            }
        } else {
            this->actor.textId = 0x2020;
            this->unk_300 = TEXT_STATE_EVENT;
            this->unk_30E = 0;
        }
        if (Text_GetFaceReaction(play, 5) != 0) {
            if (var == 0) {
                this->actor.textId = Text_GetFaceReaction(play, 5);
                this->unk_30B = 1;
                this->unk_300 = TEXT_STATE_DONE;
                this->unk_30E = 0;
            }
        }
        this->actionFunc = func_80A5475C;
    } else {
        // "I don't know"
        osSyncPrintf(VT_FGCOL(PURPLE) " ☆☆☆☆☆ とおしゃしねぇちゅーの ☆☆☆☆☆ \n" VT_RST);
        this->actionFunc = func_80A53AD4;
    }
}

void func_80A53AD4(EnHeishi2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 exchangeItemId;
    s16 yawDiffTemp;
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);
    if (Text_GetFaceReaction(play, 5) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, 5);
    } else {
        this->actor.textId = 0x200E;
    }
    this->unk_300 = TEXT_STATE_DONE;
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        exchangeItemId = func_8002F368(play);
        if (exchangeItemId == EXCH_ITEM_LETTER_ZELDA) {
            func_80078884(NA_SE_SY_CORRECT_CHIME);
            player->actor.textId = 0x2010;
            this->unk_300 = TEXT_STATE_EVENT;
            this->actionFunc = func_80A53C0C;
        } else if (exchangeItemId != EXCH_ITEM_NONE) {
            player->actor.textId = 0x200F;
        }
    } else {
        yawDiffTemp = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        yawDiff = ABS(yawDiffTemp);
        if (!(120.0f < this->actor.xzDistToPlayer) && (yawDiff < 0x4300)) {
            func_8002F298(&this->actor, play, 100.0f, EXCH_ITEM_LETTER_ZELDA);
        }
    }
}

void func_80A53C0C(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((this->unk_300 == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        func_8002DF54(play, 0, 8);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = func_80A53C90;
    }
}

void func_80A53C90(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Animation_Change(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = func_80A53D0C;
}

void func_80A53D0C(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount;
    BgGateShutter* gate;

    frameCount = this->skelAnime.curFrame;
    gate = (BgGateShutter*)play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    SkelAnime_Update(&this->skelAnime);
    if (12.0f <= frameCount) {
        if (this->audioFlag == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EV_SPEAR_HIT);
            this->audioFlag = 1;
        }
    }
    if (this->unk_2EC <= frameCount) {
        while (gate != NULL) {
            if (gate->dyna.actor.id != ACTOR_BG_GATE_SHUTTER) {
                gate = (BgGateShutter*)gate->dyna.actor.next;
            } else {
                this->gate = &gate->dyna.actor;
                gate->openingState = 1;
                break;
            }
        }
        // "I've come!"
        osSyncPrintf(VT_FGCOL(PURPLE) "☆☆☆ きたきたきたぁ！ ☆☆☆ %x\n" VT_RST, gate->dyna.actor.next);
        this->actionFunc = func_80A53DF8;
    }
}

void func_80A53DF8(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->unk_2F2[0] = 200;
    this->cameraId = Play_CreateSubCamera(play);
    Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, this->cameraId, CAM_STAT_ACTIVE);
    this->unk_2BC.x = -71.0f;
    this->unk_280.x = -71.0f;
    this->unk_2BC.y = 571.0f;
    this->unk_280.y = 571.0f;
    this->unk_2BC.z = -1487.0f;
    this->unk_280.z = -1487.0f;
    this->unk_298.x = 181.0f;
    this->unk_28C.x = 181.0f;
    this->unk_298.y = 417.0f;
    this->unk_28C.y = 417.0f;
    this->unk_298.z = -1079.0f;
    this->unk_28C.z = -1079.0f;
    Play_CameraSetAtEye(play, this->cameraId, &this->unk_280, &this->unk_28C);
    this->actionFunc = func_80A53F30;
}

void func_80A53F30(EnHeishi2* this, PlayState* play) {
    BgGateShutter* gate;

    SkelAnime_Update(&this->skelAnime);
    Play_CameraSetAtEye(play, this->cameraId, &this->unk_280, &this->unk_28C);
    gate = (BgGateShutter*)this->gate;
    if ((this->unk_2F2[0] == 0) || (gate->openingState == 0)) {
        Play_ClearCamera(play, this->cameraId);
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        if ((this->unk_30A != 2)) {
            if (this->unk_30A == 0) {
                this->actor.textId = 0x2015;
                Message_ContinueTextbox(play, this->actor.textId);
                this->actionFunc = func_80A54038;
            } else {
                Message_CloseTextbox(play);
                func_8002DF54(play, NULL, 7);
                this->actionFunc = func_80A53908;
            }
        } else {
            this->unk_30E = 0;
            this->actor.textId = 0x2021;
            Rupees_ChangeBy(15);
            Message_ContinueTextbox(play, this->actor.textId);
            this->actionFunc = func_80A5427C;
        }
    }
}

void func_80A54038(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Flags_SetInfTable(INFTABLE_SHOWED_ZELDAS_LETTER_TO_GATE_GUARD);
        Message_CloseTextbox(play);
        func_8002DF54(play, 0, 7);
        this->actionFunc = func_80A53908;
    }
}

void func_80A540C0(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                this->actor.textId = 0x2020;
                Message_ContinueTextbox(play, this->actor.textId);
                Player_UnsetMask(play);
                Flags_SetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK);
                Flags_SetItemGetInf(ITEMGETINF_38);
                Item_Give(play, ITEM_SOLD_OUT);
                if (this->unk_30A != 0) {
                    this->unk_30A = 2;
                    this->unk_30E = 1;
                    this->actionFunc = func_80A5427C;
                } else {
                    this->unk_30E = 0;
                    this->actionFunc = func_80A541FC;
                }
                break;
            case 1:
                this->unk_30E = 1;
                this->actor.textId = 0x200C;
                Message_ContinueTextbox(play, this->actor.textId);
                this->unk_300 = TEXT_STATE_EVENT;
                if (this->unk_30A == 0) {
                    this->actionFunc = func_80A5427C;
                } else {
                    this->actionFunc = func_80A54954;
                }
        }
    }
}

void func_80A541FC(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actor.textId = 0x2021;
        Rupees_ChangeBy(15);
        Message_ContinueTextbox(play, this->actor.textId);
        this->actionFunc = func_80A5427C;
    }
}

void func_80A5427C(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->unk_30E == 0) {
            this->unk_30E = 0;
            this->unk_30A = this->unk_30E;
            Message_CloseTextbox(play);
            this->actionFunc = func_80A53908;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            this->actionFunc = func_80A54320;
        }
    }
}

void func_80A54320(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Animation_Change(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->audioFlag = 0;
    this->actionFunc = func_80A543A0;
}

void func_80A543A0(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount = this->skelAnime.curFrame;
    BgGateShutter* gate = (BgGateShutter*)(play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head);
    SkelAnime_Update(&this->skelAnime);

    if ((frameCount >= 12.0f) && (!this->audioFlag)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_SPEAR_HIT);
        this->audioFlag = 1;
    }

    if (this->unk_2EC <= frameCount) {
        while (gate != NULL) {
            if (ACTOR_BG_GATE_SHUTTER != gate->dyna.actor.id) {
                gate = (BgGateShutter*)(gate->dyna.actor.next);
            } else {
                this->gate = &gate->dyna.actor;
                if (this->unk_30A != 2) {
                    gate->openingState = -1;
                    break;
                } else {
                    gate->openingState = 2;
                    break;
                }
            }
        }
        if (this->unk_30A == 0) {
            this->unk_30A = 1;
        }
        this->actionFunc = func_80A53DF8;
    }
}

void func_80A544AC(EnHeishi2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, -6100, 5, this->unk_2E4, 0);
    Math_ApproachF(&this->unk_2E4, 3000.0f, 1.0f, 500.0f);
    this->actor.world.rot.z = this->actor.shape.rot.z;
    if (this->actor.shape.rot.z < -6000) {
        Message_StartTextbox(play, 0x708F, NULL);
        this->actor.flags |= ACTOR_FLAG_WILL_TALK;
        this->actionFunc = func_80A5455C;
        this->unk_2E4 = 0.0f;
    }
}

void func_80A5455C(EnHeishi2* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    f32 rotY;
    EnBom* bomb;

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        func_8002DF54(play, NULL, 7);
        Message_CloseTextbox(play);

        pos.x = Rand_CenteredFloat(20.0f) + this->unk_274.x;
        pos.y = Rand_CenteredFloat(20.0f) + (this->unk_274.y - 40.0f);
        pos.z = Rand_CenteredFloat(20.0f) + (this->unk_274.z - 20.0f);
        rotY = Rand_CenteredFloat(7000.0f) + this->actor.yawTowardsPlayer;
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, pos.x, pos.y, pos.z, 0, rotY, 0, 0, true);
        if (bomb != NULL) {
            bomb->actor.speedXZ = Rand_CenteredFloat(5.0f) + 10.0f;
            bomb->actor.velocity.y = Rand_CenteredFloat(5.0f) + 10.0f;
        }

        // "This is down!"
        osSyncPrintf(VT_FGCOL(YELLOW) " ☆☆☆☆☆ これでダウンだ！ ☆☆☆☆☆ \n" VT_RST);
        this->actionFunc = func_80A546DC;
    }
}

void func_80A546DC(EnHeishi2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.z, 200, 5, this->unk_2E4, 0);
    Math_ApproachF(&this->unk_2E4, 3000.0f, 1.0f, 500.0f);
    this->actor.world.rot.z = this->actor.shape.rot.z;
    if (this->actor.shape.rot.z > 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80A5475C(EnHeishi2* this, PlayState* play) {
    s16 yawDiff;

    SkelAnime_Update(&this->skelAnime);

    if (Text_GetFaceReaction(play, 5) != 0) {
        if (this->unk_30B == 0) {
            if (this->type == 2) {
                this->actionFunc = func_80A53278;
                return;
            }
            if (this->type == 5) {
                this->actionFunc = func_80A5399C;
                return;
            }
        }
    } else if (this->unk_30B != 0) {
        if (this->type == 2) {
            this->actionFunc = func_80A53278;
            return;
        }
        if (this->type == 5) {
            this->actionFunc = func_80A5399C;
            return;
        }
    }

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        if (this->type == 2) {
            if (this->unk_30E == 1) {
                this->actionFunc = func_80A5344C;
                return;
            } else {
                this->actionFunc = func_80A53278;
                return;
            }
        } else if (this->type == 5) {
            if (this->unk_300 == TEXT_STATE_DONE) {
                this->actionFunc = func_80A5399C;
            }

            if (this->unk_300 == TEXT_STATE_EVENT) {
                this->actionFunc = func_80A54954;
            }

            if (this->unk_300 == TEXT_STATE_CHOICE) {
                this->unk_309 = 1;
                func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
                this->actionFunc = func_80A540C0;
            }
            return;
        }
    }

    if (((this->type != 2) && (this->type != 5)) ||
        ((yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)),
          !(this->actor.xzDistToPlayer > 120.0f)) &&
         (yawDiff < 0x4300))) {
        func_8002F2F4(&this->actor, play);
    }
}

void func_80A54954(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gEnHeishiIdleAnim);

    Animation_Change(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = func_80A549E8;
}

void func_80A549E8(EnHeishi2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_300 == Message_GetState(&play->msgCtx) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (this->type == 2) {
            this->actionFunc = func_80A531E4;
        }
        if (this->type == 5) {
            this->actionFunc = func_80A53908;
        }
    }
}

void EnHeishi2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHeishi2* this = (EnHeishi2*)thisx;
    s32 i;

    Actor_SetFocus(&this->actor, this->unk_2E0);
    if ((this->type == 2) || (this->type == 5)) {
        this->actor.focus.pos.y = 70.0f;
        Actor_SetFocus(&this->actor, 70.0f);
        func_80038290(play, &this->actor, &this->unk_260, &this->unk_26C, this->actor.focus.pos);
    }

    this->unk_2FC++;

    for (i = 0; i != 5; i++) {
        if (this->unk_2F2[i] != 0) {
            this->unk_2F2[i]--;
        }
    }
    this->actionFunc(this, play);
    Actor_MoveForward(&this->actor);
    switch (this->type) {
        case 6:
            break;
        case 9:
            break;
        default:
            Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 30.0f, 0x1D);
            Collider_UpdateCylinder(&this->actor, &this->collider);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            break;
    }
}

s32 EnHeishi2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                               void* thisx) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    switch (this->type) {
        case 1:
            break;
        case 7:
            break;
        default:
            if (limbIndex == 9) {
                rot->x = rot->x + this->unk_26C.y;
            }
            if (limbIndex == 16) {
                rot->x = rot->x + this->unk_260.y;
                rot->z = rot->z + this->unk_260.z;
            }
    }

    return false;
}

void EnHeishi2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    if (limbIndex == 16) {
        Matrix_Get(&this->mtxf_330);
    }
}

void EnHeishi2_DrawKingGuard(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gHeishiKingGuardDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHeishi2_Draw(Actor* thisx, PlayState* play) {
    EnHeishi2* this = (EnHeishi2*)thisx;
    Mtx* mtx;
    s32 linkObjBankIndex;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, EnHeishi2_OverrideLimbDraw,
                      EnHeishi2_PostLimbDraw, this);
    if ((this->type == 5) && (Flags_GetInfTable(INFTABLE_GATE_GUARD_PUT_ON_KEATON_MASK))) {
        linkObjBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_LINK_CHILD);
        if (linkObjBankIndex >= 0) {
            Matrix_Put(&this->mtxf_330);
            Matrix_Translate(-570.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_RotateZ(DEGTORAD(70.0), MTXMODE_APPLY);
            mtx = MATRIX_NEWMTX(play->state.gfxCtx) - 7;

            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[linkObjBankIndex].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx);
            gSPDisplayList(POLY_OPA_DISP++, gLinkChildKeatonMaskDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[this->actor.objBankIndex].segment);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

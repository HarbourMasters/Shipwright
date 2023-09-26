/*
 * File: z_en_gm.c
 * Overlay: ovl_En_Gm
 * Description: Medi-Goron
 */

#include "z_en_gm.h"
#include "objects/object_oF1d_map/object_oF1d_map.h"
#include "objects/object_gm/object_gm.h"
#include "vt.h"
#include <assert.h>

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_WHILE_CULLED)

void EnGm_Init(Actor* thisx, PlayState* play);
void EnGm_Destroy(Actor* thisx, PlayState* play);
void EnGm_Update(Actor* thisx, PlayState* play);
void EnGm_Draw(Actor* thisx, PlayState* play);

void func_80A3D838(EnGm* this, PlayState* play);
void func_80A3DFBC(EnGm* this, PlayState* play);
void func_80A3DB04(EnGm* this, PlayState* play);
void func_80A3DC44(EnGm* this, PlayState* play);
void func_80A3DBF4(EnGm* this, PlayState* play);
void func_80A3DD7C(EnGm* this, PlayState* play);
void EnGm_ProcessChoiceIndex(EnGm* this, PlayState* play);
void func_80A3DF00(EnGm* this, PlayState* play);
void func_80A3DF60(EnGm* this, PlayState* play);

const ActorInit En_Gm_InitVars = {
    ACTOR_EN_GM,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_OF1D_MAP,
    sizeof(EnGm),
    (ActorFunc)EnGm_Init,
    (ActorFunc)EnGm_Destroy,
    (ActorFunc)EnGm_Update,
    NULL,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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
    { 100, 120, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

void EnGm_Init(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    // "Medi Goron"
    osSyncPrintf(VT_FGCOL(GREEN) "%s[%d] : 中ゴロン[%d]" VT_RST "\n", __FILE__, __LINE__, this->actor.params);

    this->objGmBankIndex = Object_GetIndex(&play->objectCtx, OBJECT_GM);

    if (this->objGmBankIndex < 0) {
        osSyncPrintf(VT_COL(RED, WHITE));
        // "There is no model bank! !! (Medi Goron)"
        osSyncPrintf("モデル バンクが無いよ！！（中ゴロン）\n");
        osSyncPrintf(VT_RST);
        assert(this->objGmBankIndex < 0);
    }

    this->updateFunc = func_80A3D838;
}

void EnGm_Destroy(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

s32 func_80A3D7C8(void) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        return 0;
    } else if ((IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF) &&
               !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
        return 1;
    } else if (!(gBitFlags[2] & gSaveContext.inventory.equipment)) { // Don't have giant's knife
        return 1;
    } else if (gBitFlags[3] & gSaveContext.inventory.equipment) { // Have broken giant's knife
        return 2;
    } else {
        return 3;
    }
}

void func_80A3D838(EnGm* this, PlayState* play) {
    if (Object_IsLoaded(&play->objectCtx, this->objGmBankIndex)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_WHILE_CULLED;
        SkelAnime_InitFlex(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable, 18);
        gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objGmBankIndex].segment);
        Animation_Change(&this->skelAnime, &object_gm_Anim_0002B8, 1.0f, 0.0f,
                         Animation_GetLastFrame(&object_gm_Anim_0002B8), ANIMMODE_LOOP, 0.0f);
        this->actor.draw = EnGm_Draw;
        Collider_InitCylinder(play, &this->collider);
        Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
        Actor_SetScale(&this->actor, 0.05f);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->eyeTexIndex = 0;
        this->blinkTimer = 20;
        this->actor.textId = 0x3049;
        this->updateFunc = func_80A3DFBC;
        this->actionFunc = func_80A3DB04;
        this->actor.speedXZ = 0.0f;
        this->actor.gravity = -1.0f;
        this->actor.velocity.y = 0.0f;
    }
}

void EnGm_UpdateEye(EnGm* this) {
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    } else {
        this->eyeTexIndex++;

        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
            this->blinkTimer = Rand_ZeroFloat(60.0f) + 20.0f;
        }
    }
}

void EnGm_SetTextID(EnGm* this) {
    switch (func_80A3D7C8()) {
        case 0:
            if (Flags_GetInfTable(INFTABLE_B0)) {
                this->actor.textId = 0x304B;
            } else {
                this->actor.textId = 0x304A;
            }
            break;
        case 1:
            if (Flags_GetInfTable(INFTABLE_B1)) {
                this->actor.textId = 0x304F;
            } else {
                this->actor.textId = 0x304C;
            }
            break;
        case 2:
            this->actor.textId = 0x304E;
            break;
        case 3:
            this->actor.textId = 0x304D;
            break;
    }
}

void func_80A3DB04(EnGm* this, PlayState* play) {
    f32 dx;
    f32 dz;
    Player* player = GET_PLAYER(play);

    dx = this->talkPos.x - player->actor.world.pos.x;
    dz = this->talkPos.z - player->actor.world.pos.z;

    if (Flags_GetSwitch(play, this->actor.params)) {
        EnGm_SetTextID(this);
        this->actionFunc = func_80A3DC44;
    } else if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = func_80A3DBF4;
    } else if ((this->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        this->collider.base.acFlags &= ~AC_HIT;
        func_8002F2CC(&this->actor, play, 415.0f);
    }
}

void func_80A3DBF4(EnGm* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        this->actionFunc = func_80A3DB04;
    }
}

void func_80A3DC44(EnGm* this, PlayState* play) {
    f32 dx;
    f32 dz;
    s32 pad;
    Player* player = GET_PLAYER(play);

    EnGm_SetTextID(this);

    dx = this->talkPos.x - player->actor.world.pos.x;
    dz = this->talkPos.z - player->actor.world.pos.z;

    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        switch (func_80A3D7C8()) {
            case 0:
                Flags_SetInfTable(INFTABLE_B0);
            case 3:
                this->actionFunc = func_80A3DD7C;
                return;
            case 1:
                Flags_SetInfTable(INFTABLE_B1);
                if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
                    !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
                        //Resets "Talked to Medigoron" flag in infTable to restore initial conversation state
                        Flags_UnsetInfTable(INFTABLE_B1);
                    }
            case 2:
                this->actionFunc = EnGm_ProcessChoiceIndex;
            default:
                return;
        }

        this->actionFunc = EnGm_ProcessChoiceIndex;
    }
    if ((this->collider.base.ocFlags1 & OC1_HIT) || (SQ(dx) + SQ(dz)) < SQ(100.0f)) {
        this->collider.base.acFlags &= ~AC_HIT;
        func_8002F2CC(&this->actor, play, 415.0f);
    }
}

void func_80A3DD7C(EnGm* this, PlayState* play) {
    u8 dialogState = Message_GetState(&play->msgCtx);

    if ((dialogState == TEXT_STATE_DONE || dialogState == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = func_80A3DC44;
        if (dialogState == TEXT_STATE_EVENT) {
            play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
            play->msgCtx.stateTimer = 4;
        }
    }
}

void EnGm_ProcessChoiceIndex(EnGm* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (gSaveContext.rupees < 200) {
                    Message_ContinueTextbox(play, 0xC8);
                    this->actionFunc = func_80A3DD7C;
                } else {
                    GetItemEntry itemEntry;

                    if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
                        !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
                        itemEntry = Randomizer_GetItemFromKnownCheck(RC_GC_MEDIGORON, GI_SWORD_KNIFE);
                        GiveItemEntryFromActor(&this->actor, play, itemEntry, 415.0f, 10.0f);
                        Flags_SetInfTable(INFTABLE_B1);
                    } else {
                        itemEntry = ItemTable_Retrieve(GI_SWORD_KNIFE);
                        func_8002F434(&this->actor, play, GI_SWORD_KNIFE, 415.0f, 10.0f);
                    }

                    gSaveContext.pendingSale = itemEntry.itemId;
                    gSaveContext.pendingSaleMod = itemEntry.modIndex;
                    this->actionFunc = func_80A3DF00;
                }
                break;
            case 1: // no
                Message_ContinueTextbox(play, 0x3050);
                this->actionFunc = func_80A3DD7C;
                break;
        }
    }
}

void func_80A3DF00(EnGm* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
            !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
            Flags_SetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON);
        }

        this->actor.parent = NULL;
        this->actionFunc = func_80A3DF60;
    } else {
        if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
            !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
            GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(RC_GC_MEDIGORON, GI_SWORD_KNIFE);
            GiveItemEntryFromActor(&this->actor, play, itemEntry, 415.0f, 10.0f);
            Flags_SetInfTable(INFTABLE_B1);
        }
        else {
            func_8002F434(&this->actor, play, GI_SWORD_KNIFE, 415.0f, 10.0f);
        }
    }
}

void func_80A3DF60(EnGm* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        Rupees_ChangeBy(-200);
        this->actionFunc = func_80A3DC44;
    }
}

void func_80A3DFBC(EnGm* this, PlayState* play) {
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[this->objGmBankIndex].segment);
    this->timer++;
    this->actionFunc(this, play);
    this->actor.focus.rot.x = this->actor.world.rot.x;
    this->actor.focus.rot.y = this->actor.world.rot.y;
    this->actor.focus.rot.z = this->actor.world.rot.z;
    EnGm_UpdateEye(this);
    SkelAnime_Update(&this->skelAnime);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnGm_Update(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    this->updateFunc(this, play);
}

void func_80A3E090(EnGm* this) {
    Vec3f vec1;
    Vec3f vec2;

    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 2600.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &vec2);
    this->collider.dim.pos.x = vec2.x;
    this->collider.dim.pos.y = vec2.y;
    this->collider.dim.pos.z = vec2.z;
    Matrix_Pop();
    Matrix_Push();
    Matrix_Translate(0.0f, 0.0f, 4300.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &this->talkPos);
    Matrix_Pop();
    Matrix_Translate(0.0f, 0.0f, 3800.0f, MTXMODE_APPLY);
    Matrix_RotateZYX(this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = vec1.z = 0.0f;
    Matrix_MultVec3f(&vec1, &this->actor.focus.pos);
    this->actor.focus.pos.y += 100.0f;
}

void EnGm_Draw(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    EnGm* this = (EnGm*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));
    SkelAnime_DrawSkeletonOpa(play, &this->skelAnime, NULL, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);

    func_80A3E090(this);
}

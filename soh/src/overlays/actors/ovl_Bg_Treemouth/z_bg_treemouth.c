/*
 * File: z_bg_treemouth.c
 * Overlay: ovl_Bg_Treemouth
 * Description: Great Deku Tree's Mouth
 */

#include "z_bg_treemouth.h"
#include "objects/object_spot04_objects/object_spot04_objects.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgTreemouth_Init(Actor* thisx, PlayState* play);
void BgTreemouth_Destroy(Actor* thisx, PlayState* play);
void BgTreemouth_Update(Actor* thisx, PlayState* play);
void BgTreemouth_Draw(Actor* thisx, PlayState* play);

void func_808BC65C(BgTreemouth* this, PlayState* play);
void func_808BC6F8(BgTreemouth* this, PlayState* play);
void func_808BC80C(BgTreemouth* this, PlayState* play);
void func_808BC864(BgTreemouth* this, PlayState* play);
void BgTreemouth_DoNothing(BgTreemouth* this, PlayState* play);
void func_808BC8B8(BgTreemouth* this, PlayState* play);
void func_808BC9EC(BgTreemouth* this, PlayState* play);
void func_808BCAF0(BgTreemouth* this, PlayState* play);

extern CutsceneData D_808BCE20[];
extern CutsceneData D_808BD2A0[];
extern CutsceneData D_808BD520[];
extern CutsceneData D_808BD790[];

const ActorInit Bg_Treemouth_InitVars = {
    ACTOR_BG_TREEMOUTH,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT04_OBJECTS,
    sizeof(BgTreemouth),
    (ActorFunc)BgTreemouth_Init,
    (ActorFunc)BgTreemouth_Destroy,
    (ActorFunc)BgTreemouth_Update,
    (ActorFunc)BgTreemouth_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 8000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 300, ICHAIN_STOP),
};

// unused
static f32 D_808BD9C4[] = {
    -2746.0f, 545.0f, 4694.0f, -2654.0f, 146.0f, 4534.0f,
};

void BgTreemouth_SetupAction(BgTreemouth* this, BgTreemouthActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgTreemouth_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgTreemouth* this = (BgTreemouth*)thisx;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(thisx, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gDekuTreeMouthCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);
    Actor_SetFocus(thisx, 50.0f);

    if ((gSaveContext.sceneSetupIndex < 4) && !LINK_IS_ADULT) {
        BgTreemouth_SetupAction(this, func_808BC8B8);
    } else if (LINK_IS_ADULT || (gSaveContext.sceneSetupIndex == 7)) {
        this->unk_168 = 0.0f;
        BgTreemouth_SetupAction(this, BgTreemouth_DoNothing);
    } else {
        this->unk_168 = 1.0f;
        BgTreemouth_SetupAction(this, func_808BC6F8);
    }

    thisx->textId = 0x905;
}

void BgTreemouth_Destroy(Actor* thisx, PlayState* play) {
    BgTreemouth* this = (BgTreemouth*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808BC65C(BgTreemouth* this, PlayState* play) {
    CsCmdActorAction* npcAction;

    if ((play->csCtx.state != CS_STATE_IDLE)) {
        npcAction = play->csCtx.npcActions[0];
        if (npcAction != NULL) {
            if (npcAction->action == 2) {
                BgTreemouth_SetupAction(this, func_808BC80C);
            } else if (npcAction->action == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_WOODDOOR_OPEN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                BgTreemouth_SetupAction(this, func_808BC6F8);
            }
        }
    }
}

void func_808BC6F8(BgTreemouth* this, PlayState* play) {
    Vec3f sp34;

    if (this->unk_168 < 1.0f) {
        this->unk_168 += 0.01f;
    } else {
        this->unk_168 = 1.0f;
    }

    if ((gSaveContext.sceneSetupIndex == 6) && (play->csCtx.frames >= 0x2BD) &&
        (play->state.frames % 8 == 0)) {
        sp34.x = (Rand_ZeroOne() * 1158.0f) + 3407.0f;
        sp34.y = 970.0f;
        sp34.z = (Rand_ZeroOne() * 2026.0f) + -2163.0f;
        EffectSsHahen_SpawnBurst(play, &sp34, 0.8f, 0, 50, 30, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
    }
}

void func_808BC80C(BgTreemouth* this, PlayState* play) {
    this->unk_168 += 0.05f;
    if (this->unk_168 >= 0.8f) {
        BgTreemouth_SetupAction(this, func_808BC864);
    }
}

void func_808BC864(BgTreemouth* this, PlayState* play) {
    this->unk_168 -= 0.03f;
    if (this->unk_168 <= 0.0f) {
        BgTreemouth_SetupAction(this, func_808BC65C);
    }
}

void func_808BC8B8(BgTreemouth* this, PlayState* play) {
    if ((!(Flags_GetEventChkInf(5))) || LINK_IS_ADULT) {
        if (!LINK_IS_ADULT) {
            if (Flags_GetEventChkInf(0xC)) {
                if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 1658.0f, 0x7530)) {
                    this->dyna.actor.flags |= ACTOR_FLAG_0;
                    if (this->dyna.actor.isTargeted) {
                        this->dyna.actor.flags &= ~ACTOR_FLAG_0;
                        play->csCtx.segment = D_808BD2A0;
                        gSaveContext.cutsceneTrigger = 1;
                        BgTreemouth_SetupAction(this, func_808BC9EC);
                    }
                }
            } else if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 1658.0f, 0x4E20)) {
                Flags_SetEventChkInf(0xC);
                play->csCtx.segment = D_808BCE20;
                gSaveContext.cutsceneTrigger = 1;
                BgTreemouth_SetupAction(this, func_808BC9EC);
            }
        }
    } else {
        this->unk_168 = 1.0f;
    }
}

void func_808BC9EC(BgTreemouth* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->csCtx.state == CS_STATE_UNSKIPPABLE_INIT) {
        if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 350.0f, 0x7530)) {
            player->actor.world.pos.x = 3827.0f;
            player->actor.world.pos.y = -161.0f;
            player->actor.world.pos.z = -1142.0f;
        }

        play->csCtx.frames = 0;
        play->csCtx.unk_18 = 0xFFFF;
        D_8015FCC0 = 0xFFFF;
        D_8015FCC2 = 0xFFFF;
        D_8015FCC4 = 0xFFFF;
        play->csCtx.unk_1A = 0;
        play->csCtx.unk_1B = 0;
        play->csCtx.state = CS_STATE_SKIPPABLE_EXEC;

        if (play->msgCtx.choiceIndex == 0) {
            play->csCtx.segment = D_808BD520;
            Flags_SetEventChkInf(5);
            BgTreemouth_SetupAction(this, func_808BCAF0);
        } else {
            play->csCtx.segment = D_808BD790;
            play->csCtx.frames = 0;
            BgTreemouth_SetupAction(this, func_808BC8B8);
        }
    }
}

void func_808BCAF0(BgTreemouth* this, PlayState* play) {
    CsCmdActorAction* npcAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        npcAction = play->csCtx.npcActions[0];
        if (npcAction != NULL) {
            if (npcAction->action == 2) {
                BgTreemouth_SetupAction(this, func_808BC80C);
            } else if (npcAction->action == 3) {
                Audio_PlaySoundGeneral(NA_SE_EV_WOODDOOR_OPEN, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                BgTreemouth_SetupAction(this, func_808BC6F8);
            }
        }
    }
}

void BgTreemouth_DoNothing(BgTreemouth* this, PlayState* play) {
}

void BgTreemouth_Update(Actor* thisx, PlayState* play) {
    BgTreemouth* this = (BgTreemouth*)thisx;
    f32 unk_168;

    this->actionFunc(this, play);
    unk_168 = this->unk_168;
    thisx->world.pos.x = (unk_168 * -160.0f) + 4029.0f;
    thisx->world.pos.y = (unk_168 * -399.0f) + 136.0f;
    thisx->world.pos.z = (unk_168 * 92.0f) + -1255.0f;
}

void BgTreemouth_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    u16 alpha = 500;

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);

    if ((gSaveContext.sceneSetupIndex < 4) || LINK_IS_ADULT) {
        if (gSaveContext.eventChkInf[0] & 0x80) {
            alpha = 2150;
        }
    } else { // neeeded to match
    }

    if (gSaveContext.sceneSetupIndex == 6) {
        alpha = (play->roomCtx.unk_74[0] + 0x1F4);
    }

    gDPSetEnvColor(POLY_OPA_DISP++, 128, 128, 128, alpha * 0.1f);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gDekuTreeMouthDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

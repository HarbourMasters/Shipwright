/*
 * File: z_bg_bom_guard.c
 * Overlay: Bg_Bom_Guard
 * Description: Bombchu Bowling Alley Walls
 */

#include "z_bg_bom_guard.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
#include "objects/object_bowl/object_bowl.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgBomGuard_Init(Actor* thisx, PlayState* play);
void BgBomGuard_Destroy(Actor* thisx, PlayState* play);
void BgBomGuard_Update(Actor* thisx, PlayState* play);

void func_8086E638(BgBomGuard* this, PlayState* play);

const ActorInit Bg_Bom_Guard_InitVars = {
    ACTOR_BG_BOM_GUARD,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_BOWL,
    sizeof(BgBomGuard),
    (ActorFunc)BgBomGuard_Init,
    (ActorFunc)BgBomGuard_Destroy,
    (ActorFunc)BgBomGuard_Update,
    NULL,
    NULL,
};

void BgBomGuard_SetupAction(BgBomGuard* this, BgBomGuardActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgBomGuard_Init(Actor* thisx, PlayState* play) {
    BgBomGuard* this = (BgBomGuard*)thisx;
    s32 pad[2];
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gBowlingDefaultCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);

    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 透明ガード出現 ☆☆☆☆☆ \n" VT_RST);

    thisx->scale.x = 1.0f;
    thisx->scale.y = 1.0f;
    thisx->scale.z = 1.0f;
    this->unk_16C = thisx->world.pos;
    BgBomGuard_SetupAction(this, func_8086E638);
}

void BgBomGuard_Destroy(Actor* thisx, PlayState* play) {
    BgBomGuard* this = (BgBomGuard*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8086E638(BgBomGuard* this, PlayState* play) {
    Actor* it = play->actorCtx.actorLists[ACTORCAT_NPC].head;
    Actor* thisx = &this->dyna.actor;

    this->unk_168 = 0;

    while (it != 0) {
        if (it->id == ACTOR_EN_BOM_BOWL_MAN) {
            if ((((EnBomBowlMan*)it)->minigamePlayStatus != 0) && (fabsf(play->view.eye.x) > -20.0f) &&
                (fabsf(play->view.eye.y) > 110.0f)) {
                this->unk_168 = 1;
            }
            break;
        }
        it = it->next;
    }

    if (this->unk_168 == 0) {
        thisx->world.pos.y = sREG(64) + -200.0f;
    } else {
        thisx->world.pos.y = 0.0f;
    }
}

void BgBomGuard_Update(Actor* thisx, PlayState* play) {
    BgBomGuard* this = (BgBomGuard*)thisx;

    this->actionFunc(this, play);
}

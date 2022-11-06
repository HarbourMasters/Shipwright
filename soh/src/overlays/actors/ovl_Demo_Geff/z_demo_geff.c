/*
 * File: z_demo_geff.c
 * Overlay: Demo_Geff
 * Description: Ganon's Lair Rubble Fragment
 */

#include "z_demo_geff.h"
#include "objects/object_geff/object_geff.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void DemoGeff_Init(Actor* thisx, PlayState* play);
void DemoGeff_Destroy(Actor* thisx, PlayState* play);
void DemoGeff_Update(Actor* thisx, PlayState* play);
void DemoGeff_Draw(Actor* thisx, PlayState* play);

void func_80978030(DemoGeff* this, PlayState* play);

void func_809783D4(DemoGeff* this, PlayState* play);
void func_80978308(DemoGeff* this, PlayState* play);

void func_809784D4(DemoGeff* this, PlayState* play);
void func_80978344(DemoGeff* this, PlayState* play);

static s16 sObjectIDs[] = {
    OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF,
};

static DemoGeffInitFunc sInitFuncs[] = {
    func_80978030, func_80978030, func_80978030, func_80978030, func_80978030,
    func_80978030, func_80978030, func_80978030, func_80978030,
};

static DemoGeffActionFunc sActionFuncs[] = {
    func_809783D4,
    func_80978308,
};

static DemoGeffDrawFunc sDrawFuncs[] = {
    func_809784D4,
    func_80978344,
};

const ActorInit Demo_Geff_InitVars = {
    ACTOR_DEMO_GEFF,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GEFF,
    sizeof(DemoGeff),
    (ActorFunc)DemoGeff_Init,
    (ActorFunc)DemoGeff_Destroy,
    (ActorFunc)DemoGeff_Update,
    (ActorFunc)DemoGeff_Draw,
    NULL,
};

void DemoGeff_Destroy(Actor* thisx, PlayState* play) {
}

void DemoGeff_Init(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;

    if (this->actor.params < 0 || this->actor.params >= 9) {
        osSyncPrintf(VT_FGCOL(RED) "Demo_Geff_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_Kill(&this->actor);
        return;
    }
    this->action = 0;
    this->drawConfig = 0;
}

void func_80977EA8(PlayState* play, Gfx* dlist) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    func_80093D18(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx);
}

void func_80977F80(DemoGeff* this, PlayState* play) {
    s32 pad[2];
    s32 objBankIndex = this->objBankIndex;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.status[objBankIndex].segment);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[objBankIndex].segment);

    CLOSE_DISPS(gfxCtx);
}

void func_80978030(DemoGeff* this, PlayState* play) {
    Vec3f* thisScale = &this->actor.scale;

    this->action = 1;
    this->drawConfig = 1;

    switch (this->actor.params) {
        case 0:
        case 3:
        case 6:
            thisScale->x = (kREG(7) * 0.01f) + 0.3f;
            thisScale->y = (kREG(8) * 0.01f) + 0.3f;
            thisScale->z = (kREG(9) * 0.01f) + 0.3f;
            break;
        case 1:
        case 4:
        case 7:
            thisScale->x = (kREG(10) * 0.01f) + 0.15f;
            thisScale->y = (kREG(11) * 0.01f) + 0.29f;
            thisScale->z = (kREG(12) * 0.01f) + 0.12f;
            break;
        default:
            thisScale->x = (kREG(13) * 0.01f) + 0.1f;
            thisScale->y = (kREG(14) * 0.01f) + 0.15f;
            thisScale->z = (kREG(15) * 0.01f) + 0.2f;
            break;
    }
}

void func_809781FC(DemoGeff* this, PlayState* play) {
    s32 targetParams = 2;
    Actor* propIt;

    if (this->demoGt == NULL) {
        propIt = play->actorCtx.actorLists[ACTORCAT_PROP].head;
        if ((this->actor.params != 0) && (this->actor.params != 1) && (this->actor.params != 2)) {
            targetParams = 2;
        } else {
            targetParams = 1;
        }
        while (propIt != NULL) {
            if (propIt->id == ACTOR_DEMO_GT && propIt->params == targetParams) {
                this->deltaPosX = this->actor.world.pos.x - propIt->world.pos.x;
                this->deltaPosY = this->actor.world.pos.y - propIt->world.pos.y;
                this->deltaPosZ = this->actor.world.pos.z - propIt->world.pos.z;
                this->demoGt = (DemoGt*)propIt;
            }
            propIt = propIt->next;
        }
    }
}

void func_809782A0(DemoGeff* this, PlayState* play) {
    DemoGt* demoGt = this->demoGt;
    s16 params = this->actor.params;

    if (demoGt != NULL && (params != 6) && (params != 7) && (params != 8)) {
        this->actor.world.pos.x = demoGt->dyna.actor.world.pos.x + this->deltaPosX;
        this->actor.world.pos.y = demoGt->dyna.actor.world.pos.y + this->deltaPosY;
        this->actor.world.pos.z = demoGt->dyna.actor.world.pos.z + this->deltaPosZ;
    }
}

void func_80978308(DemoGeff* this, PlayState* play) {
    func_809781FC(this, play);
    func_809782A0(this, play);
    func_80978030(this, play);
}

void func_80978344(DemoGeff* this, PlayState* play) {
    func_80977EA8(play, gGanonRubbleDL);
}

void func_80978370(DemoGeff* this, PlayState* play) {
    s16 params = this->actor.params;
    DemoGeffInitFunc initFunc = sInitFuncs[params];
    if (initFunc == NULL) {
        osSyncPrintf(VT_FGCOL(RED) " Demo_Geff_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, params);
        Actor_Kill(&this->actor);
        return;
    }
    initFunc(this, play);
}

void func_809783D4(DemoGeff* this, PlayState* play) {
    ObjectContext* objCtx = &play->objectCtx;
    Actor* thisx = &this->actor;
    s32 params = thisx->params;
    s16 objectId = sObjectIDs[params];
    s32 objBankIndex = Object_GetIndex(objCtx, objectId);
    s32 pad;

    if (objBankIndex < 0) {
        osSyncPrintf(VT_FGCOL(RED) "Demo_Geff_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, params);
        Actor_Kill(thisx);
        return;
    }
    if (Object_IsLoaded(objCtx, objBankIndex)) {
        this->objBankIndex = objBankIndex;
        func_80978370(this, play);
    }
}

void DemoGeff_Update(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;

    if (this->action < 0 || this->action >= 2 || sActionFuncs[this->action] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void func_809784D4(DemoGeff* this, PlayState* play) {
}

void DemoGeff_Draw(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;
    s32 drawConfig = this->drawConfig;

    if (drawConfig < 0 || drawConfig >= 2 || sDrawFuncs[drawConfig] == NULL) {
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    if (drawConfig != 0) {
        func_80977F80(this, play);
    }
    sDrawFuncs[drawConfig](this, play);
}

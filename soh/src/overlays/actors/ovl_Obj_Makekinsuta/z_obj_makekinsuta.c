/*
 * File: z_obj_makekinsuta.c
 * Overlay: ovl_Obj_Makekinsuta
 * Description: Skulltula Sprouting from Bean Spot
 */

#include "z_obj_makekinsuta.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void ObjMakekinsuta_Init(Actor* thisx, GlobalContext* globalCtx);
void ObjMakekinsuta_Update(Actor* thisx, GlobalContext* globalCtx);

void func_80B98320(ObjMakekinsuta* this, GlobalContext* globalCtx);
void ObjMakekinsuta_DoNothing(ObjMakekinsuta* this, GlobalContext* globalCtx);

const ActorInit Obj_Makekinsuta_InitVars = {
    ACTOR_OBJ_MAKEKINSUTA,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMakekinsuta),
    (ActorFunc)ObjMakekinsuta_Init,
    (ActorFunc)Actor_Noop,
    (ActorFunc)ObjMakekinsuta_Update,
    NULL,
    NULL,
};

void ObjMakekinsuta_Init(Actor* thisx, GlobalContext* globalCtx) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    if ((this->actor.params & 0x6000) == 0x4000) {
        osSyncPrintf(VT_FGCOL(BLUE));
        // "Gold Star Enemy(arg_data %x)"
        osSyncPrintf("金スタ発生敵(arg_data %x)\n", this->actor.params);
        osSyncPrintf(VT_RST);
    } else {
        osSyncPrintf(VT_COL(YELLOW, BLACK));
        // "Invalid Argument (arg_data %x)(%s %d)"
        osSyncPrintf("引数不正 (arg_data %x)(%s %d)\n", this->actor.params, __FILE__, __LINE__);
        osSyncPrintf(VT_RST);
    }
    this->actionFunc = func_80B98320;
}

void func_80B98320(ObjMakekinsuta* this, GlobalContext* globalCtx) {
    if (this->unk_152 != 0) {
        if (this->timer >= 60 && !func_8002DEEC(GET_PLAYER(globalCtx))) {
            Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_SW, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, (this->actor.params | 0x8000));
            this->actionFunc = ObjMakekinsuta_DoNothing;
        } else {
            this->timer++;
        }
    } else {
        this->timer = 0;
    }
}

void ObjMakekinsuta_DoNothing(ObjMakekinsuta* this, GlobalContext* globalCtx) {
}

void ObjMakekinsuta_Update(Actor* thisx, GlobalContext* globalCtx) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    this->actionFunc(this, globalCtx);
}

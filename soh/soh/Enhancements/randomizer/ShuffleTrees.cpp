#include "ShuffleTrees.h"
#include "soh_assets.h"
#include "assets/objects/object_wood02/object_wood02.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_en_Wood02/z_en_wood02.h"
extern PlayState* gPlayState;
}

static Gfx* D_80B3BF54[] = {
    (Gfx*)object_wood02_DL_0078D0, (Gfx*)object_wood02_DL_007CA0, (Gfx*)object_wood02_DL_0080D0,
    (Gfx*)object_wood02_DL_000090, (Gfx*)object_wood02_DL_000340, (Gfx*)object_wood02_DL_000340,
    (Gfx*)object_wood02_DL_000700,
};

static Gfx* D_80B3BF70[] = {
    (Gfx*)object_wood02_DL_007968,
    (Gfx*)object_wood02_DL_007D38,
    (Gfx*)object_wood02_DL_0081A8,
    NULL,
    NULL,
    NULL,
    (Gfx*)object_wood02_DL_007AD0,
    (Gfx*)object_wood02_DL_007E20,
    (Gfx*)object_wood02_DL_008350,
    (Gfx*)object_wood02_DL_000160,
    (Gfx*)object_wood02_DL_000440,
    (Gfx*)object_wood02_DL_000700,
};

extern void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play);

extern "C" void EnWood02_RandomizerDraw(Actor* thisx, PlayState* play) {
    EnWood02* thisy = (EnWood02*)thisx;
    float treeSize = 1.0f;

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if ((thisy->actor.params == WOOD_LEAF_GREEN) || (thisy->actor.params == WOOD_LEAF_YELLOW)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 127);
        Gfx_DrawDListOpa(play, (Gfx*)gRandoTreeDL);
    } else if (D_80B3BF70[thisy->drawType & 0xF] != NULL) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 0);
        Gfx_DrawDListOpa(play, (Gfx*)gRandoTreeDL);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, D_80B3BF70[thisy->drawType & 0xF]);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

uint8_t EnWood02_RandomizerHoldsItem(EnWood02* treeActor, PlayState* play) {
    uint8_t treeSetting = Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TREES).Get();

    // Don't pull randomized item if tree isn't randomized or is already checked
    if (!IS_RANDO ||
        Flags_GetRandomizerInf(treeActor->treeId.randomizerInf) ||
        treeActor->treeId.randomizerCheck == RC_UNKNOWN_CHECK) {
        return false;
    }
    else {
        return true;
    }
}

void EnWood02_RandomizerSpawnCollectible(EnWood02* treeActor, PlayState* play) {
    EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &treeActor->actor.world.pos, ITEM00_SOH_DUMMY);
    item00->randoInf = treeActor->treeId.randomizerInf;
    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(treeActor->treeId.randomizerCheck, true, GI_NONE);
    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
    item00->actor.velocity.y = 8.0f;
    item00->actor.speedXZ = 2.0f;
    item00->actor.world.rot.y = Rand_CenteredFloat(65536.0f);
}

void EnWood02_RandomizerInit(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    // 0x00 - Large trees
    // 0x01 - Medium trees
    // 0x02 - Small trees
    if (actor->id == ACTOR_EN_WOOD02 && actor->params >= 0x03 && gPlayState->sceneNum == SCENE_HYRULE_CASTLE)
        return;
    if (actor->id != ACTOR_EN_WOOD02 || (gPlayState->sceneNum == SCENE_HYRULE_FIELD && gSaveContext.cutsceneIndex == 0xFFF3))
        return;
    if (actor->id == ACTOR_EN_WOOD02 && ((actor->params >= WOOD_BUSH_GREEN_SMALL) || (actor->params == WOOD_TREE_OVAL_GREEN_SPAWNED || actor->params == WOOD_TREE_OVAL_YELLOW_SPAWNED || actor->params == WOOD_TREE_CONICAL_SPAWNED)))
        return;

    EnWood02* treeActor = static_cast<EnWood02*>(actorRef);

    treeActor->treeId = OTRGlobals::Instance->gRandomizer->IdentifyTree(gPlayState->sceneNum, (s16)actor->world.pos.x, (s16)actor->world.pos.z);
}

void ShuffleTrees_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    // Hook drawing function, draw randomized tree.
    if (id == VB_TREE_SETUP_DRAW) {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            treeActor->actor.draw = (ActorFunc)EnWood02_RandomizerDraw;
            *should = false;
        }
    }

    // Hook dropping function, spawn randomized item.
    if (id == VB_TREE_DROP_ITEM) {
        EnWood02* treeActor = va_arg(args, EnWood02*);
        if (EnWood02_RandomizerHoldsItem(treeActor, gPlayState)) {
            EnWood02_RandomizerSpawnCollectible(treeActor, gPlayState);
            *should = false;
        }
    }

    va_end(args);
}

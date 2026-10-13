/*
 * File: z_en_sekihi.c
 * Overlay: ovl_En_Sekihi
 * Description: Mikau's Grave & Song Pedestals
 */

#include "z_en_sekihi.h"
#include "objects/object_sekihil/object_sekihil.h"
#include "objects/object_sekihig/object_sekihig.h"
#include "objects/object_sekihin/object_sekihin.h"
#include "objects/object_sekihiz/object_sekihiz.h"
#include "objects/object_zog/object_zog.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnSekihi_Init(Actor* thisx, PlayState* play);
void EnSekihi_Destroy(Actor* thisx, PlayState* play);
void EnSekihi_Update(Actor* thisx, PlayState* play);
void EnSekihi_Draw(Actor* thisx, PlayState* play);

void func_80A44DE8(EnSekihi* this, PlayState* play);
void func_80A450B0(EnSekihi* this, PlayState* play);
void EnSekihi_DoNothing(EnSekihi* this, PlayState* play);

ActorProfile En_Sekihi_Profile = {
    /**/ ACTOR_EN_SEKIHI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnSekihi),
    /**/ EnSekihi_Init,
    /**/ EnSekihi_Destroy,
    /**/ EnSekihi_Update,
    /**/ NULL,
};

static s16 sObjectIds[] = { OBJECT_SEKIHIL, OBJECT_SEKIHIG, OBJECT_SEKIHIN, OBJECT_SEKIHIZ, OBJECT_ZOG };

static Gfx* sOpaDLists[] = {
    gSunsSongGraveTriforceDL, gSunsSongGraveGoronDL, gSunsSongGraveKokiriDL, gSongOfSoaringPedestalDL, gMikauGraveDL,
};

static Gfx* sXluDLists[] = {
    gSunsSongGraveTriforceSymbolsDL,
    gSunsSongGraveGoronSymbolsDL,
    gSunsSongGraveKokiriSymbolsDL,
    gSongOfSoaringPedestalEmptyDL,
    gMikauGraveDirtDL,
};

static u16 sTextIds[] = { 0, 0, 0, 0, 0x1018 };

void EnSekihi_Init(Actor* thisx, PlayState* play) {
    EnSekihi* this = (EnSekihi*)thisx;
    s32 type = ENSIKIHI_GET_TYPE(thisx);
    s32 objectSlot;
    s32 pad;

    if (((type < SEKIHI_TYPE_0) || (type >= SEKIHI_TYPE_MAX)) || (sOpaDLists[type] == NULL)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if ((type == SEKIHI_TYPE_4) &&
        ((gSaveContext.save.saveInfo.skullTokenCount & 0xFFFF) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
        // For some reason the mikau grave sets the flag instead of something in the spider house on exit.
        SET_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_BUYER_MOVED_IN);
    }

    objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[type]);
    if (objectSlot > OBJECT_SLOT_NONE) {
        this->objectSlot = objectSlot;
    }
    this->actionFunc = func_80A44DE8;
    this->opaDList = sOpaDLists[type];
    this->xluDList = sXluDLists[type];
    this->dyna.actor.textId = sTextIds[type];
    this->dyna.actor.focus.pos.y = this->dyna.actor.world.pos.y + 60.0f;
    Actor_SetScale(&this->dyna.actor, 0.1f);
}

void EnSekihi_Destroy(Actor* thisx, PlayState* play) {
    EnSekihi* this = (EnSekihi*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80A44DE8(EnSekihi* this, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    s32 type;
    CollisionHeader* colHeaders[] = {
        &gSunsSongGraveTriforceCol, &gSunsSongGraveGoronCol, &gSunsSongGraveKokiriCol,
        &gSongOfSoaringPedestalCol, &gMikauGraveCol,
    };

    type = ENSIKIHI_GET_TYPE(&this->dyna.actor);
    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->dyna.actor.objectSlot = this->objectSlot;
        this->dyna.actor.draw = EnSekihi_Draw;
        if (type == SEKIHI_TYPE_4) {
            this->actionFunc = func_80A450B0;
        } else {
            this->actionFunc = EnSekihi_DoNothing;
        }

        Actor_SetObjectDependency(play, &this->dyna.actor);
        DynaPolyActor_Init(&this->dyna, 0);
        if (colHeaders[type] != NULL) {
            CollisionHeader_GetVirtual(colHeaders[type], &colHeader);
        }

        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        if ((type == SEKIHI_TYPE_4) &&
            GameInteractor_Should(VB_CONSIDER_MIKAU_HEALED, INV_CONTENT(ITEM_MASK_ZORA) != ITEM_MASK_ZORA, false)) {
            Actor_Kill(&this->dyna.actor);
        }
    }
}

void func_80A44F40(EnSekihi* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(play) && (play->msgCtx.currentTextId == 0x1019)) {
                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        Audio_PlaySfx_MessageDecide();
                        Message_ContinueTextbox(play, 0x101A);
                        break;
                    case 1:
                        Audio_PlaySfx_MessageDecide();
                        Message_ContinueTextbox(play, 0x101B);
                        break;
                    case 2:
                        Audio_PlaySfx_MessageCancel();
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80A450B0;
                        break;
                }
            } else {
                break;
            }
            break;
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x1018:
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;
                    case 0x101A:
                    case 0x101B:
                        Message_CloseTextbox(play);
                        this->actionFunc = func_80A450B0;
                        break;
                }
            }
            break;
    }
}

void func_80A450B0(EnSekihi* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->dyna.actor, &play->state)) {
        this->actionFunc = func_80A44F40;
    } else if ((this->dyna.actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->dyna.actor, 0x2600, play)) {
        Actor_OfferTalk(&this->dyna.actor, play, 120.0f);
    }
}

void EnSekihi_DoNothing(EnSekihi* this, PlayState* play) {
}

void EnSekihi_Update(Actor* thisx, PlayState* play) {
    EnSekihi* this = (EnSekihi*)thisx;

    this->actionFunc(this, play);
}

void EnSekihi_Draw(Actor* thisx, PlayState* play) {
    EnSekihi* this = (EnSekihi*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->xluDList != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, this->xluDList);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->opaDList);

    CLOSE_DISPS(play->state.gfxCtx);
}

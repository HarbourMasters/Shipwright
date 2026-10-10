#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnAObj_Init(Actor* thisx, PlayState* play);
void EnAObj_Destroy(Actor* thisx, PlayState* play);
void EnAObj_Update(Actor* thisx, PlayState* play);
void EnAObj_Draw(Actor* thisx, PlayState* play);

void EnAObj_Idle(EnAObj* this, PlayState* play);
void EnAObj_Talk(EnAObj* this, PlayState* play);

ActorProfile En_A_Obj_Profile = {
    /**/ ACTOR_EN_A_OBJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnAObj),
    /**/ EnAObj_Init,
    /**/ EnAObj_Destroy,
    /**/ EnAObj_Update,
    /**/ EnAObj_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 25, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_STOP),
};

void EnAObj_Init(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    this->actor.textId = AOBJ_GET_TEXTID(&this->actor);
    this->actor.params = AOBJ_GET_TYPE(&this->actor);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0, ActorShadow_DrawCircle, 12);
    Collider_InitAndSetCylinder(play, &this->collision, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collision);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actionFunc = EnAObj_Idle;
}

void EnAObj_Destroy(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    Collider_DestroyCylinder(play, &this->collision);
}

void EnAObj_Idle(EnAObj* this, PlayState* play) {
    s32 yawDiff;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnAObj_Talk;
    } else {
        yawDiff = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

        if ((yawDiff < 0x2800) || ((this->actor.params == AOBJ_SIGNPOST_ARROW) && (yawDiff > 0x5800))) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
    }
}

void EnAObj_Talk(EnAObj* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = EnAObj_Idle;
    }
}

void EnAObj_Update(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 45.0f);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collision.base);
}

static Gfx* sDLists[] = {
    gSignRectangularDL, // AOBJ_SIGNPOST_OBLONG
    gSignDirectionalDL, // AOBJ_SIGNPOST_ARROW
};

void EnAObj_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, sDLists[thisx->params]);
}

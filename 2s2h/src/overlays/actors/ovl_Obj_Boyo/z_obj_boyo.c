#include "z_obj_boyo.h"
#include "overlays/actors/ovl_En_Kaizoku/z_en_kaizoku.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_boyo/object_boyo.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjBoyo_Init(Actor* thisx, PlayState* play);
void ObjBoyo_Destroy(Actor* thisx, PlayState* play2);
void ObjBoyo_Update(Actor* thisx, PlayState* play2);
void ObjBoyo_Draw(Actor* thisx, PlayState* play);

void ObjBoyo_PushPlayer(ObjBoyo* this, Actor* actor);
void ObjBoyo_PushPirate(ObjBoyo* this, Actor* actor);
void ObjBoyo_ExplodeBomb(ObjBoyo* this, Actor* actor);
Actor* ObjBoyo_FindCollidedActor(ObjBoyo* this, PlayState* play, s32* index);

ActorProfile Obj_Boyo_Profile = {
    /**/ ACTOR_OBJ_BOYO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOYO,
    /**/ sizeof(ObjBoyo),
    /**/ ObjBoyo_Init,
    /**/ ObjBoyo_Destroy,
    /**/ ObjBoyo_Update,
    /**/ ObjBoyo_Draw,
};

typedef void (*ObjBoyoCollisionHandler)(ObjBoyo* this, Actor* actor);

typedef struct ObjBoyoUnkStruct {
    /* 0x0 */ s16 id;
    /* 0x4 */ ObjBoyoCollisionHandler colHandler;
} ObjBoyoUnkStruct; // size = 0x8

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_TYPE_PLAYER | AC_HARD | AC_ON,
        OC1_TYPE_2 | OC1_TYPE_1 | OC1_TYPE_PLAYER | OC1_ON,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01CBFFBE, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    {
        60,
        140,
        0,
        { 0, 0, 0 },
    },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static ObjBoyoUnkStruct sCollisionHandlers[] = {
    { ACTOR_PLAYER, ObjBoyo_PushPlayer },
    { ACTOR_EN_KAIZOKU, ObjBoyo_PushPirate },
    { ACTOR_EN_BOM, ObjBoyo_ExplodeBomb },
};

void ObjBoyo_Init(Actor* thisx, PlayState* play) {
    ObjBoyo* this = (ObjBoyo*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->animatedMaterial = Lib_SegmentedToVirtual(&object_boyo_Matanimheader_000E88);
}

void ObjBoyo_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBoyo* this = (ObjBoyo*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjBoyo_PushPlayer(ObjBoyo* this, Actor* actor) {
    Player* player = (Player*)actor;

    player->pushedSpeed = 30.0f;
    player->pushedYaw = this->actor.yawTowardsPlayer;
}

void ObjBoyo_PushPirate(ObjBoyo* this, Actor* actor) {
    EnKaizoku* kaizoku = (EnKaizoku*)actor;

    kaizoku->boyoBounceVelocity = 30.0f;
    kaizoku->boyoBounceAngle = Actor_WorldYawTowardActor(&this->actor, &kaizoku->picto.actor);
}

void ObjBoyo_ExplodeBomb(ObjBoyo* this, Actor* actor) {
    EnBom* bom = (EnBom*)actor;

    bom->timer = 0;
}

/*
 * Find the Actor we collided with.
 */
Actor* ObjBoyo_FindCollidedActor(ObjBoyo* this, PlayState* play, s32* index) {
    Actor* collidedActor;
    ObjBoyoUnkStruct* iter;
    s32 i;

    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        *index = 0;
        return &GET_PLAYER(play)->actor;
    }
    iter = sCollisionHandlers + 1;
    if (this->collider.base.ocFlags1 & OC1_HIT) {
        collidedActor = this->collider.base.oc;
        for (i = 1; i < 3; i++) {
            if (iter->id == collidedActor->id) {
                *index = i;
                return collidedActor;
            }
            iter++;
        }
    }
    return NULL;
}

void ObjBoyo_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBoyo* this = (ObjBoyo*)thisx;
    Actor* collidedActor;
    s32 index;

    collidedActor = ObjBoyo_FindCollidedActor(this, play, &index);

    if (collidedActor != NULL) {
        sCollisionHandlers[index].colHandler(this, collidedActor);
        // TODO: find out what all of these are.

        this->unk194 = 0x64;
        this->unk196 = 3;
        this->unk198 = 0.01f;
        this->unk19C = 0.03f;
        this->unk1A4 = 0x3F40;
        this->unk1A6 = 0x7D0;
        this->unk1A8 = 0;
        this->unk1A0 = 0.03f;
        this->unk1AA = 0x2DF7;
        this->unk1AC = 0x258;
    }

    if (this->unk194 > 0) {
        // compute new scaling
        // this computation might help finding what those values are.

        this->unk194 -= this->unk196;
        this->unk1AA += this->unk1AC;
        this->unk1A8 += this->unk1AA;

        this->actor.scale.x = this->actor.scale.z =
            (Math_CosS(this->unk1A8 + this->unk1A4) * (f32)this->unk194 * this->unk19C * this->unk198) + 0.1f;

        this->actor.scale.y =
            (Math_CosS(this->unk1A8 + this->unk1A6) * (f32)this->unk194 * this->unk1A0 * this->unk198) + 0.1f;
    } else {
        Actor_SetScale(&this->actor, 0.1f);
        if (this->collider.base.acFlags & AC_HIT) {
            // TODO: find out what all of these are.

            this->unk194 = 0x1E;
            this->unk196 = 2;
            this->unk198 = 0.033333335f;
            this->unk19C = 0.012f;
            this->unk1A0 = 0.006f;
            this->unk1A4 = 0x3F40;
            this->unk1A6 = 0x7D0;
            this->unk1A8 = 0;
            this->unk1AA = 0x3A98;
            this->unk1AC = 0x640;
        }
    }
    this->collider.base.acFlags &= ~AC_HIT;
    this->collider.base.ocFlags1 &= ~OC1_HIT;
    this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (this->actor.xzDistToPlayer < 2000.0f) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void ObjBoyo_Draw(Actor* thisx, PlayState* play) {
    ObjBoyo* this = (ObjBoyo*)thisx;

    AnimatedMat_Draw(play, this->animatedMaterial);
    Gfx_DrawDListOpa(play, object_boyo_DL_000300);
}

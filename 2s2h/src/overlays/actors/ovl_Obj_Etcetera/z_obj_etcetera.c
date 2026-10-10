/*
 * File: z_obj_etcetera.c
 * Overlay: ovl_Obj_Etcetera
 * Description: Deku Flower
 */

#include "z_obj_etcetera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjEtcetera_Init(Actor* thisx, PlayState* play);
void ObjEtcetera_Destroy(Actor* thisx, PlayState* play);
void ObjEtcetera_Update(Actor* thisx, PlayState* play);

void ObjEtcetera_PlayRustleAnimation(ObjEtcetera* this, PlayState* play);
void ObjEtcetera_DoBounceOscillation(ObjEtcetera* this, PlayState* play);
void ObjEtcetera_Setup(ObjEtcetera* this, PlayState* play);
void ObjEtcetera_DrawIdle(Actor* thisx, PlayState* play);
void ObjEtcetera_DrawAnimated(Actor* thisx, PlayState* play);

ActorProfile Obj_Etcetera_Profile = {
    ACTOR_OBJ_ETCETERA,
    ACTORCAT_BG,
    FLAGS,
    GAMEPLAY_KEEP,
    sizeof(ObjEtcetera),
    (ActorFunc)ObjEtcetera_Init,
    (ActorFunc)ObjEtcetera_Destroy,
    (ActorFunc)ObjEtcetera_Update,
    (ActorFunc)NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x01000202, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 20, 14, 0, { 0, 0, 0 } },
};

static s16 sObjectIds[] = {
    GAMEPLAY_KEEP,
    GAMEPLAY_KEEP,
    GAMEPLAY_KEEP,
    GAMEPLAY_KEEP,
};

/**
 * Most interactions with a Deku Flower cause it to slightly oscillate on the X and Z axes.
 * When these small oscillations happen, the game determines how to scale the appropriate
 * axes by using this table.
 */
static f32 sOscillationTable[] = {
    -1.0f, -1.0f, -1.0f, -0.7f, 0.0f, 0.7f, 1.0f, 0.7f, 0.0f, -0.7f, -1.0f, -0.7f, 0.0f, 0.7f, 1.0f, 0.7f, 0.0f, -0.7f,
};

void ObjEtcetera_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjEtcetera* this = (ObjEtcetera*)thisx;
    s32 objectSlot;
    s32 type = DEKU_FLOWER_TYPE(&this->dyna.actor);
    s32 floorBgId;
    Vec3f pos;

    if ((type < DEKU_FLOWER_TYPE_PINK) || (type >= DEKU_FLOWER_TYPE_MAX)) {
        type = DEKU_FLOWER_TYPE_PINK;
    }

    objectSlot = Object_GetSlot(&play->objectCtx, sObjectIds[type]);
    if (objectSlot > OBJECT_SLOT_NONE) {
        this->objectSlot = objectSlot;
    }

    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y + 10.0f;
    pos.z = this->dyna.actor.world.pos.z;
    BgCheck_EntityRaycastFloor5(&play->colCtx, &this->dyna.actor.floorPoly, &floorBgId, &this->dyna.actor, &pos);
    this->dyna.actor.floorBgId = floorBgId;
    Collider_InitAndSetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    this->actionFunc = ObjEtcetera_Setup;
    Actor_SetScale(&this->dyna.actor, 0.01f);
    this->dyna.actor.scale.y = 0.02f;
    this->burrowFlag = 0;
}

void ObjEtcetera_Destroy(Actor* thisx, PlayState* play) {
    ObjEtcetera* this = (ObjEtcetera*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * This function will make the flower oscillate on the X and Z axes in most situations
 * where something interacts with it. When the player launches out of the flower, the
 * oscillation is handled by ObjEtcetera_DoBounceOscillation instead.
 */
void ObjEtcetera_DoNormalOscillation(ObjEtcetera* this, PlayState* play) {
    if (this->oscillationTimer > 0) {
        s32 requiredScopeTemp;

        Actor_SetScale(&this->dyna.actor,
                       (sOscillationTable[play->gameplayFrames % 18] * (0.0001f * this->oscillationTimer)) + 0.01f);
        this->dyna.actor.scale.y = 0.02f;
        this->oscillationTimer--;
    } else {
        Actor_SetScale(&this->dyna.actor, 0.01f);
        this->dyna.actor.scale.y = 0.02f;
    }
}

void ObjEtcetera_StartRustleAnimation(ObjEtcetera* this) {
    Animation_Change(&this->skelAnime, &gDekuFlowerRustleAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuFlowerRustleAnim), ANIMMODE_ONCE, 0.0f);
    this->dyna.actor.draw = ObjEtcetera_DrawAnimated;
    this->actionFunc = ObjEtcetera_PlayRustleAnimation;
}

void ObjEtcetera_Idle(ObjEtcetera* this, PlayState* play) {
    s16 minOscillationTimer;
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags3 & PLAYER_STATE3_200) && (this->dyna.actor.xzDistToPlayer < 20.0f)) {
        // Player is launching out of the Deku Flower
        Animation_Change(&this->skelAnime, &gDekuFlowerBounceAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuFlowerBounceAnim), ANIMMODE_ONCE, 0.0f);
        this->dyna.actor.draw = ObjEtcetera_DrawAnimated;
        this->actionFunc = ObjEtcetera_DoBounceOscillation;
        Actor_SetScale(&this->dyna.actor, 0.01f);
        this->dyna.actor.scale.y = 0.02f;
        this->bounceOscillationScale = 0.003f;
        this->oscillationTimer = 30;
        this->burrowFlag &= ~1;
    } else if ((player->stateFlags3 & PLAYER_STATE3_2000) && (this->dyna.actor.xzDistToPlayer < 30.0f) &&
               (this->dyna.actor.playerHeightRel > 0.0f)) {
        // Player is hovering above the Deku Flower
        minOscillationTimer = 10 - (s32)(this->dyna.actor.playerHeightRel * 0.05f);
        if (this->oscillationTimer < minOscillationTimer) {
            this->oscillationTimer = minOscillationTimer;
        }
    } else {
        if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
            if (!(this->burrowFlag & 1)) {
                // Player is walking onto the Deku Flower, or falling on it from a height
                this->oscillationTimer = 10;
                ObjEtcetera_StartRustleAnimation(this);
            } else if ((player->actor.speed > 0.1f) ||
                       ((player->unk_ABC < 0.0f) && !(player->stateFlags3 & PLAYER_STATE3_100))) {
                // Player is walking on top of the Deku Flower, is at the very start of burrowing, or is at the very
                // start of launching
                this->oscillationTimer = 10;
            }

            this->burrowFlag |= 1;
        } else {
            if (this->burrowFlag & 1) {
                // Player is walking off the Deku Flower
                this->oscillationTimer = 10;
                ObjEtcetera_StartRustleAnimation(this);
            }

            this->burrowFlag &= ~1;
        }
    }

    if ((this->collider.base.acFlags & AC_HIT)) {
        this->oscillationTimer = 10;
        ObjEtcetera_StartRustleAnimation(this);
    }

    ObjEtcetera_DoNormalOscillation(this, play);
}

void ObjEtcetera_PlayRustleAnimation(ObjEtcetera* this, PlayState* play) {
    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        this->burrowFlag |= 1;
    } else {
        this->burrowFlag &= ~1;
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->dyna.actor.draw = ObjEtcetera_DrawIdle;
        this->actionFunc = ObjEtcetera_Idle;
    }

    ObjEtcetera_DoNormalOscillation(this, play);
}

/**
 * When the bounce animation plays (either because the player launched out of the flower,
 * or because the flower spawned after killing a Mad Scrub), this function makes the
 * flower oscillate stronger than it normally does, including an oscillation on the Y-axis.
 */
void ObjEtcetera_DoBounceOscillation(ObjEtcetera* this, PlayState* play) {
    // In order to match, we are seemingly required to access scale.x at one point
    // without using this. We can create a thisx or dyna pointer to achieve that, but
    // it's more likely they used dyna given that DynaPolyActor_IsPlayerOnTop takes a DynaPolyActor.
    DynaPolyActor* dyna = &this->dyna;
    f32 scaleTemp;

    if (DynaPolyActor_IsPlayerOnTop(dyna)) {
        this->burrowFlag |= 1;
    } else {
        this->burrowFlag &= ~1;
    }

    SkelAnime_Update(&this->skelAnime);

    if (this->oscillationTimer > 0) {
        this->oscillationTimer--;
    } else {
        this->dyna.actor.draw = ObjEtcetera_DrawIdle;
        this->actionFunc = ObjEtcetera_Idle;
        Actor_SetScale(&this->dyna.actor, 0.01f);
        this->dyna.actor.scale.y = 0.02f;
        this->oscillationTimer = 0;
        this->bounceOscillationScale = 0.0f;
        return;
    }

    this->bounceOscillationScale *= 0.8f;
    this->bounceOscillationScale -= (this->dyna.actor.scale.x - 0.01f) * 0.4f;
    scaleTemp = dyna->actor.scale.x + this->bounceOscillationScale;
    Actor_SetScale(&this->dyna.actor, scaleTemp);
    this->dyna.actor.scale.y = 2.0f * scaleTemp;
}

void ObjEtcetera_Setup(ObjEtcetera* this, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    s32 type;
    CollisionHeader* collisionHeaders[] = {
        &gPinkDekuFlowerCol,
        &gPinkDekuFlowerCol,
        &gGoldDekuFlowerCol,
        &gGoldDekuFlowerCol,
    };
    s32 pad;
    CollisionHeader* thisCollisionHeader;

    type = DEKU_FLOWER_TYPE(&this->dyna.actor);
    if ((type < DEKU_FLOWER_TYPE_PINK) || (type >= DEKU_FLOWER_TYPE_MAX)) {
        type = DEKU_FLOWER_TYPE_PINK;
    }

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->dyna.actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->dyna.actor);
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
        thisCollisionHeader = collisionHeaders[type];
        if (thisCollisionHeader != NULL) {
            CollisionHeader_GetVirtual(thisCollisionHeader, &colHeader);
        }

        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

        type = DEKU_FLOWER_TYPE(&this->dyna.actor);
        switch (type) {
            case DEKU_FLOWER_TYPE_PINK:
            case DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE:
                SkelAnime_Init(play, &this->skelAnime, &gPinkDekuFlowerSkel, &gDekuFlowerBounceAnim, this->jointTable,
                               this->morphTable, PINK_DEKU_FLOWER_LIMB_MAX);
                this->dList = gPinkDekuFlowerIdleDL;
                break;

            case DEKU_FLOWER_TYPE_GOLD:
            case DEKU_FLOWER_TYPE_GOLD_WITH_INITIAL_BOUNCE:
                this->dList = gGoldDekuFlowerIdleDL;
                SkelAnime_Init(play, &this->skelAnime, &gGoldDekuFlowerSkel, &gDekuFlowerBounceAnim, this->jointTable,
                               this->morphTable, GOLD_DEKU_FLOWER_LIMB_MAX);
                this->collider.dim.height = 20;
                break;

            default:
                break;
        }

        type = DEKU_FLOWER_TYPE(&this->dyna.actor);
        switch (type) {
            case DEKU_FLOWER_TYPE_PINK:
            case DEKU_FLOWER_TYPE_GOLD:
                this->dyna.actor.draw = ObjEtcetera_DrawIdle;
                this->actionFunc = ObjEtcetera_Idle;
                Actor_SetScale(&this->dyna.actor, 0.01f);
                this->dyna.actor.scale.y = 0.02f;
                this->dyna.actor.focus.pos.y = this->dyna.actor.home.pos.y + 10.0f;
                this->dyna.actor.attentionRangeType = ATTENTION_RANGE_3;
                break;

            case DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE:
            case DEKU_FLOWER_TYPE_GOLD_WITH_INITIAL_BOUNCE:
                Animation_Change(&this->skelAnime, &gDekuFlowerBounceAnim, 1.0f, 0.0f,
                                 Animation_GetLastFrame(&gDekuFlowerBounceAnim), ANIMMODE_ONCE, 0.0f);
                this->dyna.actor.draw = ObjEtcetera_DrawAnimated;
                this->actionFunc = ObjEtcetera_DoBounceOscillation;
                Actor_SetScale(&this->dyna.actor, 0.0f);
                this->oscillationTimer = 30;
                this->bounceOscillationScale = 0.0f;
                this->dyna.actor.focus.pos.y = this->dyna.actor.home.pos.y + 10.0f;
                this->dyna.actor.attentionRangeType = ATTENTION_RANGE_3;
                break;

            default:
                break;
        }
    }
}

void ObjEtcetera_Update(Actor* thisx, PlayState* play) {
    ObjEtcetera* this = (ObjEtcetera*)thisx;
    CollisionPoly* floorPoly;
    u8 floorBgId = this->dyna.actor.floorBgId;

    if (floorBgId == BGCHECK_SCENE) {
        floorPoly = this->dyna.actor.floorPoly;
        if ((floorPoly != NULL) && (this->burrowFlag & 1)) {
            Environment_ChangeLightSetting(play, SurfaceType_GetLightSettingIndex(&play->colCtx, floorPoly, floorBgId));
        }
    }

    this->actionFunc(this, play);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * When the flower isn't animating, this function is used to draw it.
 * It draws the flower as a single, non-moving display list that encompasses the whole flower.
 * When an animation is finished, functions are expected to set the actor's draw function to this.
 */
void ObjEtcetera_DrawIdle(Actor* thisx, PlayState* play) {
    ObjEtcetera* this = (ObjEtcetera*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, this->dList);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * When the flower is animating, this function is used to draw it.
 * It draws the flower as an animated bunch of limbs using the SkelAnime system.
 * When a function wants to play an animation, it is expected to set the actor's draw function to this.
 */
void ObjEtcetera_DrawAnimated(Actor* thisx, PlayState* play) {
    ObjEtcetera* this = (ObjEtcetera*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->dyna.actor);
}

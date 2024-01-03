/*
 * File: z_custom_projectiles.c
 * Overlay: ovl_Custom_Projectile
 * Description: Arrow, Deku Seed, and Deku Nut Projectile
 */

#include "z_custom_projectile.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void CustomProjectile_Init(Actor* thisx, PlayState* play);
void CustomProjectile_Destroy(Actor* thisx, PlayState* play);
void CustomProjectile_Update(Actor* thisx, PlayState* play);
void CustomProjectile_Draw(Actor* thisx, PlayState* play);

void CustomProjectile_ReadyToFire(CustomProjectile* this, PlayState* play);
void CustomProjectile_Fly(CustomProjectile* this, PlayState* play);

void UltrahandProjectile_Fly(CustomProjectile* this, PlayState* play);
void ArmcannonProjectile_Fly(CustomProjectile* this, PlayState* play);

static Input* sControlInput;

int armcannonFullCharge = 40;


const ActorInit Custom_Projectile_InitVars = {
    ACTOR_CUSTOM_PROJECTILE, 
    ACTORCAT_ITEMACTION, //ACTORCAT_EXPLOSIVE
    FLAGS,
    OBJECT_GAMEPLAY_FIELD_KEEP,
    sizeof(CustomProjectile),
    (ActorFunc)CustomProjectile_Init,
    (ActorFunc)CustomProjectile_Destroy,
    (ActorFunc)CustomProjectile_Update,
    (ActorFunc)CustomProjectile_Draw,
    NULL,
};

static ColliderQuadInit quadColliderUltrahand = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK0,
        { 0x000001B7, 0x00, 0x00 },
        { 0xFFFFFFFF, 0xFF, 0xFF },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NONE,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit quadColliderArmcannon = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEMTYPE_UNK2,
        { 0x000001B7, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderCylinderInit cylinderColliderReflector = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x000001B7, 0x00, 0x01 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,// | BUMP_NO_AT_INFO,
        OCELEM_NONE,
    },
    { 35, 45, 0, { 0, 0, 0 } },
};
static InitChainEntry sInitChain[] = {
    ICHAIN_F32(minVelocityY, -150, ICHAIN_STOP),
};

void CustomProjectile_SetupAction(CustomProjectile* this, CustomProjectileActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void CustomProjectile_Init(Actor* thisx, PlayState* play) {
    thisx->room = -1;
    CustomProjectile* this = (CustomProjectile*)thisx;
    Actor_ProcessInitChain(&this->actor, sInitChain);

    if (this->actor.params == PROJECTILE_ULTRAHAND) {
        Collider_InitQuad(play, &this->collider[0]);
        Collider_SetQuad(play, &this->collider[0], &this->actor, &quadColliderUltrahand);
        //this->collider[0].info.toucherFlags &= ~0x18;
        //this->collider[0].info.toucherFlags |= 0;
        //this->collider[0].info.toucher.dmgFlags = dmgFlags[this->actor.params];
    }
    else if (this->actor.params == PROJECTILE_ARMCANNON) {
        Collider_InitQuad(play, &this->collider[0]);
        Collider_SetQuad(play, &this->collider[0], &this->actor, &quadColliderArmcannon);
        Collider_InitQuad(play, &this->collider[1]);
        Collider_SetQuad(play, &this->collider[1], &this->actor, &quadColliderArmcannon);
        Collider_InitQuad(play, &this->collider[2]);
        Collider_SetQuad(play, &this->collider[2], &this->actor, &quadColliderArmcannon);
    }
    else if (this->actor.params == PROJECTILE_REFLECTOR) {
        Collider_InitCylinder(play, &this->colliderCylinder);
        Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &cylinderColliderReflector);
    }

    CustomProjectile_SetupAction(this, CustomProjectile_ReadyToFire);
}

void CustomProjectile_Destroy(Actor* thisx, PlayState* play) {
    CustomProjectile* this = (CustomProjectile*)thisx;
    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyQuad(play, &this->collider[0]);
    Collider_DestroyQuad(play, &this->collider[0]);
}

void CustomProjectile_ReadyToFire(CustomProjectile* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    if (this->actor.params == PROJECTILE_REFLECTOR) {
        this->relectorFrame++;
        if (this->relectorFrame == 1) {
            //first frame = do damage
            if (gSaveContext.linkAge == LINK_AGE_ADULT) {
                this->colliderCylinder.dim.radius = 40;
                this->colliderCylinder.dim.height = 50;
            }
            //this->actor.world.pos.y -= 20.0f;
            Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderCylinder.base);
            //this->actor.world.pos.y += 20.0f;
        }
        if (this->relectorFrame >= 4)this->relectorFrame = 2; //only draw frame 1 on start, no frame 0
        this->actor.world.pos = player->actor.world.pos;
        this->actor.world.pos.y += 20.0f;
        if (!(player->stateFlags1 & PLAYER_STATE1_SHIELDING)) {
            player->hasReflector = false;
            Actor_Kill(&this->actor);
        }
        return;
    }


    if (this->actor.parent == NULL) {

        switch (this->actor.params) {
        case PROJECTILE_ULTRAHAND:
            Player_PlaySfx(&player->actor, NA_SE_IT_ARROW_SHOT);
            this->timer = 6;
            func_8002D9A4(&this->actor, 150.0f); //set speed at launch
            break;

        case PROJECTILE_ARMCANNON:
            Actor_SetScale(&this->actor, 0.0030f + (this->armcannonCharge * 0.00020f));
            this->timer = 20;
            func_8002D9A4(&this->actor, 50.0f); //set speed at launch
            if (this->armcannonCharge >= armcannonFullCharge) {
                this->bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM,
                    this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0,
                    this->actor.shape.rot.y, 0, 2, 0);
                this->bomb->timer = 20;
                Actor_SetScale(&this->bomb->actor, 0.0f);
                this->bomb->actor.parent = this;

                this->freqscale = 1.0f;
                this->volumeShoot = 1.0f;
                this->reverb = 0;
                Audio_PlaySoundGeneral(NA_SE_IT_EXPLOSION_LIGHT, &this->actor.world.pos, 4, &this->freqscale, &this->volumeShoot, &this->reverb);
            }
            else {
                this->freqscale = 1.2f;
                this->volumeShoot = 0.80f;
                this->reverb = 0;
                Audio_PlaySoundGeneral(NA_SE_IT_EXPLOSION_LIGHT, &this->actor.world.pos, 4, &this->freqscale, &this->volumeShoot, &this->reverb);
            }
            break;
        }

        CustomProjectile_SetupAction(this, CustomProjectile_Fly);
        Math_Vec3f_Copy(&this->unk_210, &this->actor.world.pos);

    }
    else {


        if (this->armcannonCharge == 0) {
            this->freqscale = 0.75f;
            this->volume = 2.5f;
            this->reverb = 0;
        }

        if (this->actor.params == PROJECTILE_ARMCANNON) {
            if (this->armcannonCharge < armcannonFullCharge) {
                this->armcannonCharge++;
                if (this->armcannonCharge <= 4) {
                    Actor_SetScale(&this->actor, 0.0f);
                }
                else {
                    Actor_SetScale(&this->actor, 0.0030f + (this->armcannonCharge * 0.00020f));
                }
                if ((((this->armcannonCharge) % 10) == 0) & ((this->armcannonCharge != armcannonFullCharge))) {
                    this->freqscale += 0.05f;
                    Audio_PlaySoundGeneral(NA_SE_PL_SWORD_CHARGE, &this->actor.world.pos, 4, &this->freqscale, &this->volume, &this->reverb);
                }
            }
            else {
                if (this->armcannonCharge == armcannonFullCharge) {
                    this->freqscale += 0.2f;
                    this->volume = 3.0f;
                    this->collider[0].info.toucher.dmgFlags = 0xFFFF;
                    this->collider[1].info.toucher.dmgFlags = 0xFFFF;
                    this->collider[2].info.toucher.dmgFlags = 0xFFFF;
                    Audio_PlaySoundGeneral(NA_SE_PL_SWORD_CHARGE, &this->actor.world.pos, 4, &this->freqscale, &this->volume, &this->reverb);
                    this->armcannonCharge++;
                }
            }
        }
    }
}

void CustomProjectile_Fly(CustomProjectile* this, PlayState* play) {
    if (this->actor.params == PROJECTILE_ULTRAHAND) UltrahandProjectile_Fly(this, play);
    else if (this->actor.params == PROJECTILE_ARMCANNON) ArmcannonProjectile_Fly(this, play);
}


void UltrahandProjectile_Fly(CustomProjectile* this, PlayState* play) {
    sControlInput = &play->state.input[0];
    CollisionPoly* hitPoly;
    s32 bgId;
    Vec3f hitPoint;
    Vec3f posCopy;
    s32 atTouched;
    bool dynaTouched = false;
    u16 sfxId;
    Actor* hitActor;
    Vec3f sp60;
    Vec3f sp54;
    Player* player = GET_PLAYER(play);

    DynaPolyActor* dynaPolyActor;

    if (this->ultrahandedActor == NULL) {
        if (DECR(this->timer) == 0) {
            if (player->customProjectile == &this->actor) player->customProjectile = NULL;
            Actor_Kill(&this->actor);
            return;
        }

        atTouched = (this->collider[0].base.atFlags & AT_HIT);

        Math_Vec3f_Copy(&this->unk_210, &this->actor.world.pos);
        Actor_MoveForward(&this->actor);
        this->touchedPoly = BgCheck_ProjectileLineTest(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &hitPoint, &this->actor.wallPoly, true, true, true, true, &bgId);
        if (this->touchedPoly != 0) {

            dynaPolyActor = DynaPoly_GetActor(&play->colCtx, bgId);
            if (dynaPolyActor != NULL) {
                this->ultrahandedActor = &dynaPolyActor->actor;
                dynaTouched = true;
            }


            func_8002F9EC(play, &this->actor, this->actor.wallPoly, bgId, &hitPoint);
            Math_Vec3f_Copy(&posCopy, &this->actor.world.pos);
            Math_Vec3f_Copy(&this->actor.world.pos, &hitPoint);
        }
        this->actor.shape.rot.x = Math_Atan2S(this->actor.speedXZ, -this->actor.velocity.y); //point arrow in the direction it is moving

        if (this->hitActor != NULL) {
            if (this->hitActor->update != NULL) {
                Math_Vec3f_Sum(&this->unk_210, &this->unk_250, &sp60);
                Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_250, &sp54);

                if (BgCheck_EntityLineTest1(&play->colCtx, &sp60, &sp54, &hitPoint, &hitPoly, true, true, true, true,
                    &bgId)) {
                    this->hitActor->world.pos.x = hitPoint.x + ((sp54.x <= hitPoint.x) ? 1.0f : -1.0f);
                    this->hitActor->world.pos.y = hitPoint.y + ((sp54.y <= hitPoint.y) ? 1.0f : -1.0f);
                    this->hitActor->world.pos.z = hitPoint.z + ((sp54.z <= hitPoint.z) ? 1.0f : -1.0f);
                    Math_Vec3f_Diff(&this->hitActor->world.pos, &this->actor.world.pos, &this->unk_250);
                    this->hitActor->flags &= ~ACTOR_FLAG_DRAGGED_BY_ARROW;
                    this->hitActor = NULL;
                }
                else {
                    Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_250, &this->hitActor->world.pos);
                }

                if (this->touchedPoly && (this->hitActor != NULL)) {
                    this->hitActor->flags &= ~ACTOR_FLAG_DRAGGED_BY_ARROW;
                    this->hitActor = NULL;
                }
            }
            else {
                this->hitActor = NULL;
            }
        }

        if (atTouched | dynaTouched) {
            if (this->madeFirstContact == false) {
                if (atTouched) {
                    hitActor = this->collider[0].base.at; //AC or OC???
                    this->ultrahandedActor = hitActor;
                }
                this->madeFirstContact = true;
                this->ultrahandedOriginalGravity = this->ultrahandedActor->gravity;
                float x = this->ultrahandedActor->world.pos.x - player->actor.world.pos.x;
                float y = this->ultrahandedActor->world.pos.y - player->actor.world.pos.y;
                float z = this->ultrahandedActor->world.pos.z - player->actor.world.pos.z;
                this->ultrahandedDistance = sqrtf((x * x) + (y * y) + (z * z));
                this->ultrahandedDistance = CLAMP(this->ultrahandedDistance, 150.0f, 500.0f);
                this->ultrahandedActor->room = -1;
            }
        }
    }
    else { //if (this->ultrahandedActor != NULL)
        if ((this->ultrahandedActor->update != NULL) & (true)) { //this->ultrahandedActor->isDrawn

            if ((player->heldItemAction == PLAYER_IA_ULTRAHAND) & !(this->actor.flags & 1)) {
                if (this->ultrahandReleased == false) {
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DDOWN)) this->ultrahandedDistance -= 25.0f;
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DUP)) this->ultrahandedDistance += 25.0f;
                    this->ultrahandedDistance = CLAMP(this->ultrahandedDistance, 150.0f, 500.0f);
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DLEFT)) {
                        this->ultrahandedActor->world.rot.y += 1000;
                        this->ultrahandedActor->shape.rot.y += 1000;
                    }
                    if (CHECK_BTN_ALL(sControlInput->cur.button, BTN_DRIGHT)) {
                        this->ultrahandedActor->world.rot.y -= 1000;
                        this->ultrahandedActor->shape.rot.y -= 1000;
                    }


                    float distXZ = Math_CosS(player->actor.focus.rot.x) * this->ultrahandedDistance;
                    float distY = Math_SinS(player->actor.focus.rot.x) * this->ultrahandedDistance;
                    f32 posX = (Math_SinS(player->actor.focus.rot.y) * distXZ) + player->actor.world.pos.x;
                    f32 posZ = (Math_CosS(player->actor.focus.rot.y) * distXZ) + player->actor.world.pos.z;
                    f32 posY = -distY + player->actor.world.pos.y;
                    Vec3f old = this->ultrahandedActor->world.pos;
                    Vec3f new = (Vec3f){ posX, posY, posZ };

                    float oldWeight = 0.80f;
                    float newWeight = 1.0f - oldWeight;
                    this->ultrahandedActor->world.pos = (Vec3f){ (old.x * oldWeight) + (new.x * newWeight) - this->ultrahandedActor->colChkInfo.displacement.x,
                                                            (old.y * oldWeight) + (new.y * newWeight) - this->ultrahandedActor->colChkInfo.displacement.y,
                                                            (old.z * oldWeight) + (new.z * newWeight) - this->ultrahandedActor->colChkInfo.displacement.z };
                    this->ultrahandedActor->velocity = (Vec3f){ 0.0f, 0.0f, 0.0f };
                    Actor_UpdateBgCheckInfo(play, this->ultrahandedActor, 0.0f, 0.0f, 0.0f, 4);
                    if (this->ultrahandedActor->bgCheckFlags & 1) this->ultrahandedActor->world.pos.y += 1.0f;
                    this->ultrahandedActor->colorFilterParams = 0x0001;
                    this->ultrahandedActor->colorFilterTimer = 10;
                }
            }
            else {
                if (this->ultrahandReleased == false) {
                    this->ultrahandReleased = true;
                    this->timer = 80;
                    this->ultrahandedActor->colorFilterParams = 0x0000;
                }
            }
            if ((this->ultrahandReleased) & !(this->ultrahandedGrounded)) {
                this->timer--;
                if (this->timer <= 0) this->ultrahandedGrounded = true;
                if (this->ultrahandedActor->colorFilterParams == 0x0001) this->ultrahandedGrounded = true; //another ultrahand is grabbing it
                this->ultrahandedActor->gravity = -0.5f;
                Actor_MoveForward(this->ultrahandedActor);

                Actor_UpdateBgCheckInfo(play, this->ultrahandedActor, 0.0f, 0.0f, 0.0f, 4);
                if (this->ultrahandedActor->bgCheckFlags & 1) this->ultrahandedGrounded = true;
                if (player->customProjectile == &this->actor) player->customProjectile = NULL;
            }
            if ((this->ultrahandedGrounded) | (player->actor.world.pos.y - this->ultrahandedActor->world.pos.y >= 250.0f)) {
                this->ultrahandedActor->velocity.y = 0.0f;
                this->ultrahandedActor->gravity = this->ultrahandedOriginalGravity;
                this->ultrahandedActor == NULL;
                this->hitActor == NULL;
                if (player->customProjectile == &this->actor) player->customProjectile = NULL;
                Actor_Kill(&this->actor);
                return;
            }
        }
        else {
            this->ultrahandedActor == NULL;
            this->hitActor == NULL;
            if (player->customProjectile == &this->actor) player->customProjectile = NULL;
            Actor_Kill(&this->actor);
            return;
        }
    }
}

void ArmcannonProjectile_Fly(CustomProjectile* this, PlayState* play) {
    CollisionPoly* hitPoly;
    s32 bgId;
    Vec3f hitPoint;
    Vec3f posCopy;
    s32 atHit;
    s32 atBounced;
    s32 ocHit;
    s32 acHit;
    s32 dynaHit;
    u16 sfxId;
    DynaPolyActor* hitActor;
    s32 hitDynaID;
    Vec3f sp60;
    Vec3f sp54;
    Player* player = GET_PLAYER(play);
    if (this->volumeShoot >= 0.05f)this->volumeShoot -= .05f;


    if (DECR(this->timer) == 0) {
        if (this->bomb != NULL)Actor_Kill(&this->bomb->actor);
        Actor_Kill(&this->actor);
        return;
    }

    atHit = ((this->collider[0].base.atFlags & AT_HIT) | (this->collider[1].base.atFlags & AT_HIT) | (this->collider[2].base.atFlags & AT_HIT));
    acHit = ((this->collider[0].base.acFlags & AC_HIT) | (this->collider[1].base.acFlags & AC_HIT) | (this->collider[2].base.acFlags & AC_HIT));
    ocHit = ((this->collider[0].base.ocFlags1 & OC1_HIT) | (this->collider[1].base.ocFlags1 & OC1_HIT) | (this->collider[2].base.ocFlags1 & OC1_HIT));
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
    Math_Vec3f_Copy(&this->unk_210, &this->actor.world.pos);
    Actor_MoveForward(&this->actor);
    if (this->bomb != NULL) this->bomb->actor.world.pos = this->actor.world.pos;

    

    this->touchedPoly = BgCheck_ProjectileLineTest(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos, &hitPoint, &this->actor.wallPoly, true, true, true, true, &bgId);
    if (this->touchedPoly != 0) {
        func_8002F9EC(play, &this->actor, this->actor.wallPoly, bgId, &hitPoint);
        Math_Vec3f_Copy(&posCopy, &this->actor.world.pos);
        Math_Vec3f_Copy(&this->actor.world.pos, &hitPoint);
    }
    this->actor.shape.rot.x = Math_Atan2S(this->actor.speedXZ, -this->actor.velocity.y); //point arrow in the direction it is moving

    if (!atHit) {
        dynaHit = BgCheck_EntityLineTest1(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos,
            &hitPoint, &this->actor.wallPoly, true, true, true, true, &hitDynaID);
        if (dynaHit) {
            // If the boomerang collides with something and it's is a Jabu Object actor with params equal to 0, then
            // set collided to 0 so that the boomerang will go through the wall.
            // Otherwise play a clank sound and keep collided set to bounce back.
            if (func_8002F9EC(play, &this->actor, this->actor.wallPoly, hitDynaID, &hitPoint) != 0 ||
                (hitDynaID != BGCHECK_SCENE &&
                    ((hitActor = DynaPoly_GetActor(&play->colCtx, hitDynaID)) != NULL) &&
                    hitActor->actor.id == ACTOR_BG_BDAN_OBJECTS && hitActor->actor.params == 0)) {
                atHit = false;
            }
            else {
                CollisionCheck_SpawnShieldParticlesMetal(play, &hitPoint);
            }
        }
    }



    if ((atHit) | (this->touchedPoly != 0) | (acHit) | (ocHit) | (dynaHit)) {
        if (this->armcannonCharge >= armcannonFullCharge) {
            this->bomb->actor.world.pos = hitPoint;
            this->bomb->actor.params = 0; //be a normal bomb
            this->bomb->timer = 0;
        }
        else {
            //be like sling shot pellet
            EffectSsStone1_Spawn(play, &this->actor.world.pos, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, sfxId);
        }
        Actor_Kill(&this->actor);
    }
}

void CustomProjectile_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    CustomProjectile* this = (CustomProjectile*)thisx;
    Player* player = GET_PLAYER(play);

    this->actionFunc(this, play);
}

void CustomProjectile_func_809B4800(CustomProjectile* this, PlayState* play) { //this one detects collisions after drawing?
    static Vec3f tip = { 2000.0f, 000.0f, -000.0f };
    static Vec3f base = { -2000.0f, -000.0f, 000.0f };
    Vec3f newTipPos;
    Vec3f newBasePos;

    static Vec3f tip2 = { 000.0f, 2000.0f, 000 };
    static Vec3f base2 = { -000.0f, -2000.0f, -000.0f };
    Vec3f newTipPos2;
    Vec3f newBasePos2;

    static Vec3f tip3 = { 000.0f, 000.0f, 2000 };
    static Vec3f base3 = { -000.0f, -000.0f, -2000.0f };
    Vec3f newTipPos3;
    Vec3f newBasePos3;



    static Vec3f D_809B4EA0 = { 0.0f, 0.0f, -2000.0f };
    Matrix_MultVec3f(&D_809B4EA0, &this->unk_21C);

    s32 addBlureVertex;

    if (CustomProjectile_Fly == this->actionFunc) {
        Matrix_MultVec3f(&tip, &newTipPos);
        Matrix_MultVec3f(&base, &newBasePos);
        Matrix_MultVec3f(&tip2, &newTipPos2);
        Matrix_MultVec3f(&base2, &newBasePos2);
        Matrix_MultVec3f(&tip3, &newTipPos3);
        Matrix_MultVec3f(&base3, &newBasePos3);

        if (this->hitActor == NULL) {
            addBlureVertex &= func_80090480(play, &this->collider[0], &this->weaponInfo, &newTipPos, &newBasePos);
            if (this->actor.params == PROJECTILE_ARMCANNON) {
                addBlureVertex &= func_80090480(play, &this->collider[1], &this->weaponInfo, &newTipPos2, &newBasePos2);
                addBlureVertex &= func_80090480(play, &this->collider[2], &this->weaponInfo, &newTipPos3, &newBasePos3);

            }
        }

        if (addBlureVertex) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->effectIndex), &newTipPos, &newBasePos);
        }
    }
}

void DrawHexSegment(Actor* thisx, PlayState* play, float scale) {
    if (gSaveContext.linkAge == LINK_AGE_ADULT) scale *= 1.8f;
    CustomProjectile* this = (CustomProjectile*)thisx;
    this->actor.shape.rot = (Vec3s){ 0, 0, 0 };
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu2(play->state.gfxCtx);
    //gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 100, 255, 255);
    //gDPSetEnvColor(POLY_XLU_DISP++, 122, 160, 255, 244);

    Matrix_Push();
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    //Matrix_RotateZ((0) * (M_PI / 0x8000), MTXMODE_APPLY);
    Matrix_Translate(0, 0, 2500.0f, MTXMODE_APPLY);
    Matrix_Scale(scale * 3.0f, scale, scale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
        G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gReflectorsDL);
    Matrix_Pop();
    CLOSE_DISPS(play->state.gfxCtx);
}


void CustomProjectile_Draw(Actor* thisx, PlayState* play) {
    CustomProjectile* this = (CustomProjectile*)thisx;
    if (this->actor.params == PROJECTILE_ULTRAHAND) {
        if ((this->actor.parent == NULL) & (this->ultrahandedActor == NULL)) {//either flying in air or hit a wall
            //draw it
            Gfx_DrawDListOpa(play, gUltrahandProjectileDL);
        }
        else { //either not yet fired or hit an actor
            //dont draw it
        }
    }
    else if (this->actor.params == PROJECTILE_ARMCANNON) {
        this->actor.shape.rot =(Vec3s){ 0, 0, 0 };
        OPEN_DISPS(play->state.gfxCtx);
        Gfx_SetupDL_25Xlu2(play->state.gfxCtx);
        if (this->armcannonCharge < armcannonFullCharge) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 122, 160, 122, 244);
        }
        else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 50, 255, 255, 244);
        }
        float scale = 100.0f;

        Matrix_Push();
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        // redundant check because this is contained in an if block for non-zero speed
        Matrix_RotateZ(((play->gameplayFrames & 0xFF) * 8000) * (M_PI / 0x8000),MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
            G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);
        Matrix_Pop();
       // Matrix_RotateY(this->actor.world.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        CLOSE_DISPS(play->state.gfxCtx);

    }
    else if (this->actor.params == PROJECTILE_REFLECTOR) {
        if ((this->relectorFrame == 1)) DrawHexSegment(this, play, 10.0f);
        if ((this->relectorFrame == 2)) DrawHexSegment(this, play, 4.0f);
        if ((this->relectorFrame == 3)) DrawHexSegment(this, play, 6.0f);
        if ((this->relectorFrame == 4)) DrawHexSegment(this, play, 8.0f);


    }

    CustomProjectile_func_809B4800(this, play); //WHY IS THIS PART OF DRAW?
}


/*
 * File: z_en_encount2.c
 * Overlay: ovl_En_Encount2
 * Description: Astral Observatory - Majora's Mask Balloon
 */

#include "z_en_encount2.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/object_fusen/object_fusen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnEncount2_Init(Actor* thisx, PlayState* play);
void EnEncount2_Destroy(Actor* thisx, PlayState* play);
void EnEncount2_Update(Actor* thisx, PlayState* play);
void EnEncount2_Draw(Actor* thisx, PlayState* play);

void EnEncount2_Idle(EnEncount2* this, PlayState* play);
void EnEncount2_Popped(EnEncount2* this, PlayState* play);
void EnEncount2_Die(EnEncount2* this, PlayState* play);
void EnEncount2_SetupIdle(EnEncount2* this);
void EnEncount2_InitEffects(EnEncount2* this, Vec3f* pos, s16 fadeDelay);
void EnEncount2_UpdateEffects(EnEncount2* this, PlayState* play);
void EnEncount2_DrawEffects(EnEncount2* this, PlayState* play);

ActorProfile En_Encount2_Profile = {
    /**/ ACTOR_EN_ENCOUNT2,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_FUSEN,
    /**/ sizeof(EnEncount2),
    /**/ EnEncount2_Init,
    /**/ EnEncount2_Destroy,
    /**/ EnEncount2_Update,
    /**/ EnEncount2_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 0 }, 1 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0xF),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xE),
    /* Normal arrow   */ DMG_ENTRY(1, 0xE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0xE),
    /* Goron punch    */ DMG_ENTRY(0, 0xF),
    /* Sword          */ DMG_ENTRY(0, 0xF),
    /* Goron pound    */ DMG_ENTRY(0, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0xE),
    /* Ice arrow      */ DMG_ENTRY(1, 0xE),
    /* Light arrow    */ DMG_ENTRY(1, 0xE),
    /* Goron spikes   */ DMG_ENTRY(0, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0xF),
    /* Deku bubble    */ DMG_ENTRY(1, 0xE),
    /* Deku launch    */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0xF),
    /* Zora barrier   */ DMG_ENTRY(0, 0xF),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0xE),
    /* Zora punch     */ DMG_ENTRY(0, 0xF),
    /* Spin attack    */ DMG_ENTRY(0, 0xF),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0xF),
};

void EnEncount2_Init(Actor* thisx, PlayState* play) {
    EnEncount2* this = (EnEncount2*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&object_fusen_Colheader_002420, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ActorShape_Init(&this->dyna.actor.shape, 0.0f, ActorShadow_DrawCircle, 25.0f);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Collider_InitAndSetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit, &this->colElement);

    this->dyna.actor.attentionRangeType = ATTENTION_RANGE_6;
    this->dyna.actor.colChkInfo.health = 1;
    this->scale = 0.1f;
    this->switchFlag = ENCOUNT2_GET_SWITCH_FLAG(&this->dyna.actor);

    if (this->switchFlag == ENCOUNT2_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    this->collider.elements[0].dim.modelSphere.radius = 57;
    this->collider.elements[0].dim.scale = 1.0f;
    this->collider.elements[0].dim.modelSphere.center.x = 0;
    this->collider.elements[0].dim.modelSphere.center.y = -4;
    this->collider.elements[0].dim.modelSphere.center.z = 0;

    this->dyna.actor.colChkInfo.damageTable = &sDamageTable;
    EnEncount2_SetupIdle(this);
}

void EnEncount2_Destroy(Actor* thisx, PlayState* play) {
    EnEncount2* this = (EnEncount2*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyJntSph(play, &this->collider);
}

void EnEncount2_SetupIdle(EnEncount2* this) {
    this->isPopped = false;
    this->actionFunc = EnEncount2_Idle;
}

void EnEncount2_Idle(EnEncount2* this, PlayState* play) {
    this->oscillationAngle += 1500.0f;
    this->dyna.actor.velocity.y = Math_SinS(this->oscillationAngle);
    Math_ApproachF(&this->scale, 0.1f, 0.3f, 0.01f);

    if ((this->collider.base.acFlags & AC_HIT) && (this->dyna.actor.colChkInfo.damageEffect == 0xE)) {
        this->dyna.actor.colChkInfo.health = 0;
        this->isPopped = true;
        this->actionFunc = EnEncount2_Popped;
    }
}

void EnEncount2_Popped(EnEncount2* this, PlayState* play) {
    s32 i;
    Vec3f effPos;

    Math_Vec3f_Copy(&effPos, &this->dyna.actor.world.pos);
    effPos.y += 60.0f;
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, effPos.x, effPos.y, effPos.z, 255, 255, 200,
                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_EXPLOSION));

    for (i = 0; i < ARRAY_COUNT(this->effects) / 2; ++i) {
        EnEncount2_InitEffects(this, &effPos, 10);
    }

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_MUJURA_BALLOON_BROKEN);
    this->deathTimer = 30;
    this->actionFunc = EnEncount2_Die;
}

void EnEncount2_Die(EnEncount2* this, PlayState* play) {
    if (this->deathTimer == 0) {
        if (this->switchFlag > SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        Actor_Kill(&this->dyna.actor);
    }
}

void EnEncount2_Update(Actor* thisx, PlayState* play) {
    EnEncount2* this = (EnEncount2*)thisx;
    s32 pad;

    DECR(this->deathTimer);

    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    Actor_SetFocus(&this->dyna.actor, 30.0f);
    Actor_SetScale(&this->dyna.actor, this->scale);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->dyna.actor);
    EnEncount2_UpdateEffects(this, play);

    if (!this->isPopped) {
        Collider_UpdateSpheresElement(&this->collider, 0, &this->dyna.actor);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnEncount2_Draw(Actor* thisx, PlayState* play) {
    EnEncount2* this = (EnEncount2*)thisx;
    if (this->isPopped != true) {
        Gfx_DrawDListOpa(play, gMajoraBalloonDL);
        Gfx_DrawDListOpa(play, gMajoraBalloonKnotDL);
    }
    EnEncount2_DrawEffects(this, play);
}

void EnEncount2_InitEffects(EnEncount2* this, Vec3f* pos, s16 fadeDelay) {
    s16 i;
    EnEncount2Effect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->pos = *pos;
            effect->alphaFadeDelay = fadeDelay;
            effect->alpha = 255;

            effect->accel.x = (Rand_ZeroOne() - 0.5f) * 10.0f;
            effect->accel.y = (Rand_ZeroOne() - 0.5f) * 10.0f;
            effect->accel.z = (Rand_ZeroOne() - 0.5f) * 10.0f;

            effect->velocity.x = Rand_ZeroOne() - 0.5f;
            effect->velocity.y = Rand_ZeroOne() - 0.5f;
            effect->velocity.z = Rand_ZeroOne() - 0.5f;

            effect->scale = (Rand_ZeroFloat(1.0f) * 0.5f) + 2.0f;
            return;
        }
    }
}

void EnEncount2_UpdateEffects(EnEncount2* this, PlayState* play) {
    s32 i;
    EnEncount2Effect* effect = this->effects;

    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;

            if (effect->alphaFadeDelay != 0) {
                effect->alphaFadeDelay--;
            } else {
                effect->alpha -= 10;
                if (effect->alpha < 10) {
                    effect->isEnabled = 0;
                }
            }
        }
    }
}

void EnEncount2_DrawEffects(EnEncount2* this, PlayState* play) {
    s16 i;
    EnEncount2Effect* effect;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    effect = this->effects;
    Gfx_SetupDL25_Opa(gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    for (i = 0; i < ARRAY_COUNT(this->effects); i++, effect++) {
        if (effect->isEnabled) {
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
            gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gSun1Tex));
            gSPDisplayList(POLY_XLU_DISP++, gSunSparkleMaterialDL);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 180, 255, effect->alpha);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            Matrix_RotateZF(DEG_TO_RAD(play->state.frames * 20.0f), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gSunSparkleModelDL);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

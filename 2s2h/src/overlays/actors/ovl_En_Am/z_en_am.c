/*
 * File: z_en_am.c
 * Overlay: ovl_En_Am
 * Description: Armos
 */

#include "z_en_am.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnAm_Init(Actor* thisx, PlayState* play);
void EnAm_Destroy(Actor* thisx, PlayState* play);
void EnAm_Update(Actor* thisx, PlayState* play);
void EnAm_Draw(Actor* thisx, PlayState* play);

void func_808AFF9C(EnAm* this);
void EnAm_RemoveEnemyTexture(EnAm* this, PlayState* play);
void EnAm_WakeUp(EnAm* this);
void EnAm_ApplyEnemyTexture(EnAm* this, PlayState* play);
void func_808B0358(EnAm* this);
void func_808B03C0(EnAm* this, PlayState* play);
void func_808B0460(EnAm* this);
void func_808B04A8(EnAm* this, PlayState* play);
void func_808B0508(EnAm* this, PlayState* play);
void func_808B057C(EnAm* this);
void func_808B05C8(EnAm* this, PlayState* play);
void func_808B0640(EnAm* this);
void func_808B066C(EnAm* this, PlayState* play);
void func_808B07A8(EnAm* this, PlayState* play);
void func_808B0820(EnAm* this);
void func_808B0894(EnAm* this, PlayState* play);
void func_808B0B4C(EnAm* this, PlayState* play);

ActorProfile En_Am_Profile = {
    /**/ ACTOR_EN_AM,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_AM,
    /**/ sizeof(EnAm),
    /**/ EnAm_Init,
    /**/ EnAm_Destroy,
    /**/ EnAm_Update,
    /**/ EnAm_Draw,
};

static ColliderCylinderInit sEnemyCylinderInit = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x08 },
        { 0x81C2C788, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 23, 98, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x760D3877, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 23, 98, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0xF),
    /* Deku Stick     */ DMG_ENTRY(0, 0xF),
    /* Horse trample  */ DMG_ENTRY(0, 0xF),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(0, 0xF),
    /* Normal arrow   */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xD),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0xF),
    /* Ice arrow      */ DMG_ENTRY(0, 0xF),
    /* Light arrow    */ DMG_ENTRY(0, 0xF),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0xF),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0xF),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 23, 98, MASS_HEAVY };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 14, ICHAIN_CONTINUE),
    ICHAIN_S8(hintId, TATL_HINT_ID_ARMOS, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

void EnAm_Init(Actor* thisx, PlayState* play) {
    EnAm* this = (EnAm*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 300.0f / 7.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_am_Skel_005948, &gArmosPushedBackAnim, this->jointTable,
                   this->morphTable, OBJECT_AM_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->enemyCollider, &this->actor, &sEnemyCylinderInit);
    Collider_InitAndSetCylinder(play, &this->interactCollider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->actor.home.pos.x -= 9.0f * Math_SinS(this->actor.shape.rot.y);
    this->actor.home.pos.z -= 9.0f * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    func_808AFF9C(this);
}

void EnAm_Destroy(Actor* thisx, PlayState* play) {
    EnAm* this = (EnAm*)thisx;

    Collider_DestroyCylinder(play, &this->enemyCollider);
    Collider_DestroyCylinder(play, &this->interactCollider);
}

void EnAm_SpawnEffects(EnAm* this, PlayState* play) {
    static Vec3f sVelocity = { 0.0f, -1.5f, 0.0f };
    static Vec3f sAccel = { 0.0f, -(1.0f / 5.0f), 0.0f };
    static Color_RGBA8 sPrimColor = { 100, 100, 100, 0 };
    static Color_RGBA8 sEnvColor = { 40, 40, 40, 0 };
    s32 i;
    Vec3f effectPos;
    s32 pad;

    // Dust Clouds that spawn from the hop
    for (i = 4; i > 0; i--) {
        effectPos.x = Rand_CenteredFloat(65.0f) + this->actor.world.pos.x;
        effectPos.y = Rand_CenteredFloat(10.0f) + (this->actor.world.pos.y + 40.0f);
        effectPos.z = Rand_CenteredFloat(65.0f) + this->actor.world.pos.z;
        EffectSsKirakira_SpawnSmall(play, &effectPos, &sVelocity, &sAccel, &sPrimColor, &sEnvColor);
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_AMOS_WALK);
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 4.0f, 3, 8.0f, 300, 15, false);
}

void func_808AFF9C(EnAm* this) {
    f32 endFrame = Animation_GetLastFrame(&gArmosPushedBackAnim);

    Animation_Change(&this->skelAnime, &gArmosPushedBackAnim, 0.0f, endFrame, endFrame, ANIMMODE_LOOP, 0.0f);
    this->enemyCollider.elem.acDmgInfo.dmgFlags = 0x80000088;
    this->interactCollider.elem.acDmgInfo.dmgFlags = 0x77CFFF77;
    if (this->actor.colChkInfo.health != 0) {
        this->enemyCollider.base.atFlags &= ~AT_ON;
    }
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actionFunc = EnAm_RemoveEnemyTexture;
}

void EnAm_RemoveEnemyTexture(EnAm* this, PlayState* play) {
    if (((this->enemyCollider.base.ocFlags1 & OC1_HIT) && (this->enemyCollider.base.ocFlags2 & OC2_HIT_PLAYER)) ||
        (this->interactCollider.base.acFlags & AC_HIT)) {
        if (this->textureBlend == 0) {
            EnAm_WakeUp(this);
        }
    } else if (this->textureBlend > 10) {
        this->textureBlend -= 10;
    } else {
        this->textureBlend = 0;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->unkFlag = 0;
    }
}

void EnAm_WakeUp(EnAm* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_AMOS_WAVE);
    Actor_PlaySfx(&this->actor, NA_SE_EN_AMOS_VOICE);
    this->returnHomeTimer = 300;
    this->actionFunc = EnAm_ApplyEnemyTexture;
}

void EnAm_ApplyEnemyTexture(EnAm* this, PlayState* play) {
    s32 tempTextureBlend;
    f32 cos;
    u8 pad;
    f32 rand;
    f32 sin;

    if (this->textureBlend + 20 >= 255) {
        this->textureBlend = 255;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->enemyCollider.elem.acDmgInfo.dmgFlags = 0x81C2C788;
        this->interactCollider.elem.acDmgInfo.dmgFlags = 0x760D3877;
        this->enemyCollider.base.atFlags |= AT_ON;
        this->actor.shape.yOffset = 0.0f;
        func_808B0358(this);
    } else {
        tempTextureBlend = this->textureBlend + 20;
        rand = Rand_CenteredFloat(10.0f);
        cos = Math_CosS(this->actor.shape.rot.y) * rand;
        sin = Math_SinS(this->actor.shape.rot.y) * rand;
        this->actor.world.pos.x = this->actor.home.pos.x + cos;
        this->actor.world.pos.z = this->actor.home.pos.z + sin;
        this->textureBlend = tempTextureBlend;
    }
}

void func_808B0208(EnAm* this, PlayState* play) {
    // If the armos is against a wall, rotate and turn away from it
    if ((this->actor.speed > 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        this->actor.world.rot.y = (this->actor.wallYaw * 2) - this->actor.world.rot.y;
        this->actor.world.pos.x += this->actor.speed * Math_SinS(this->actor.world.rot.y);
        this->actor.world.pos.z += this->actor.speed * Math_CosS(this->actor.world.rot.y);
    }
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
        this->actor.speed = this->speed;
        this->actor.velocity.y = 12.0f;
    } else if (this->skelAnime.curFrame > 11.0f) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->skelAnime.curFrame = 11.0f;
        } else {
            Math_ScaledStepToS(&this->actor.world.rot.y, this->armosYaw, 0x1F40);
            this->actor.speed = 0.0f;
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
                EnAm_SpawnEffects(this, play);
            }
        }
    }
    if (this->actionFunc != func_808B0894) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

void func_808B0358(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->explodeTimer = 3;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->speed = 6.0f;
    this->actionFunc = func_808B03C0;
}

void func_808B03C0(EnAm* this, PlayState* play) {
    this->armosYaw = this->actor.yawTowardsPlayer;
    func_808B0208(this, play);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->explodeTimer--;
    }
    if (this->explodeTimer == 0) {
        func_808B0640(this);
    } else if ((this->returnHomeTimer == 0) || Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 240.0f) {
        func_808B0460(this);
    }
}

void func_808B0460(EnAm* this) {
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->speed = 0.0f;
    this->armosYaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->actionFunc = func_808B04A8;
}

void func_808B04A8(EnAm* this, PlayState* play) {
    func_808B0208(this, play);
    if (this->armosYaw == this->actor.world.rot.y) {
        func_808B057C(this);
    }
}

void func_808B04E4(EnAm* this) {
    this->speed = 0.0f;
    this->armosYaw = this->actor.home.rot.y;
    this->actionFunc = func_808B0508;
}

void func_808B0508(EnAm* this, PlayState* play) {
    func_808B0208(this, play);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 2.0f);
        Math_StepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 2.0f);
    }
    if (this->actor.home.rot.y == this->actor.world.rot.y) {
        func_808AFF9C(this);
    }
}

void func_808B057C(EnAm* this) {
    this->speed = 6.0f;
    this->armosYaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->explodeTimer = 1;
    this->actionFunc = func_808B05C8;
}

void func_808B05C8(EnAm* this, PlayState* play) {
    this->armosYaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    func_808B0208(this, play);
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 8.0f) {
        func_808B04E4(this);
    }
}

void func_808B0640(EnAm* this) {
    this->explodeTimer = 40;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->speed = 0.0f;
    this->actionFunc = func_808B066C;
}

void func_808B066C(EnAm* this, PlayState* play) {
    if (this->explodeTimer != 0) {
        this->explodeTimer--;
    } else {
        this->armosYaw = this->actor.yawTowardsPlayer;
        func_808B0208(this, play);
        if (this->armosYaw == this->actor.shape.rot.y) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_AMOS_VOICE);
            func_808B0358(this);
        }
    }
}

void EnAm_TakeDamage(EnAm* this, PlayState* play) {
    f32 endFrame = Animation_GetLastFrame(&gArmosTakeDamageAnim) - 6;

    Animation_Change(&this->skelAnime, &gArmosTakeDamageAnim, 1.0f, 4.0f, endFrame, ANIMMODE_ONCE, 0.0f);
    func_800BE504(&this->actor, &this->enemyCollider);
    this->actor.speed = 6.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Animation_GetLastFrame(&gArmosTakeDamageAnim) - 10);
    Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DAMAGE);
    this->enemyCollider.base.acFlags &= ~AC_ON;
    this->textureBlend = 255;
    this->actionFunc = func_808B07A8;
}

void func_808B07A8(EnAm* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            func_808B0820(this);
        } else {
            this->enemyCollider.base.acFlags |= AC_ON;
            func_808B0358(this);
        }
    }
}

void func_808B0820(EnAm* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gArmosHopAnim, 4.0f);
    this->explodeTimer = 64;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.speed = 0.0f;
    this->speed = 6.0f;
    this->actionFunc = func_808B0894;
}

void func_808B0894(EnAm* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 150, 150, 150, 255 };
    static Color_RGBA8 sEnvColor = { 100, 100, 100, 150 };
    s32 i;
    Vec3f dustPos;
    s32 pad;

    this->explodeTimer--;
    this->armosYaw = this->actor.yawTowardsPlayer;
    func_808B0208(this, play);
    if (this->explodeTimer == 1) {
        EnBom* bomb =
            (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, BOMB_EXPLOSIVE_TYPE_BOMB, 0, 2, BOMB_TYPE_BODY);
        if (bomb != NULL) {
            bomb->timer = 0;
        }
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_DEAD);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xB0);

        for (i = 0; i < 8; i++) {
            dustPos.x = (Math_SinS(0) * 7.0f) + this->actor.world.pos.x;
            dustPos.y = (Rand_CenteredFloat(10.0f) * 6.0f) + (this->actor.world.pos.y + 40.0f);
            dustPos.z = (Math_CosS(0) * 7.0f) + this->actor.world.pos.z;

            func_800B0EB0(play, &dustPos, &gZeroVec3f, &gZeroVec3f, &sPrimColor, &sEnvColor, 200, 45, 12);
        }
    } else if (this->explodeTimer == 0) {
        Actor_Kill(&this->actor);
        return;
    } else if (!(this->explodeTimer & 3)) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 4);
    }
    if (this->actor.world.rot.z < 0x1F40) {
        this->actor.world.rot.z += 0x320;
    }
    this->actor.shape.rot.y += this->actor.world.rot.z;
}

void func_808B0AD0(EnAm* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gArmosPushedBackAnim, 1.0f, 0.0f, 8.0f, ANIMMODE_ONCE, 0.0f);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.speed = -6.0f;
    this->actionFunc = func_808B0B4C;
}

void func_808B0B4C(EnAm* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808B0358(this);
    }
}

s32 EnAm_UpdateDamage(EnAm* this, PlayState* play) {
    if (this->enemyCollider.base.acFlags & AC_HIT) {
        this->enemyCollider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->enemyCollider.elem);
        if (!Actor_ApplyDamage(&this->actor)) {
            Enemy_StartFinishingBlow(play, &this->actor);
        }
        if (this->actor.colChkInfo.damageEffect == 0xD) {
            return true;
        }
        if (this->actor.colChkInfo.health != 0) {
            this->enemyCollider.base.atFlags &= ~AT_ON;
        }
        this->enemyCollider.base.atFlags &= ~AT_HIT;
        if (this->actor.colChkInfo.damageEffect == 0x4) {
            this->drawDmgEffScale = 0.7f;
            this->drawDmgEffAlpha = 4.0f;
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->enemyCollider.elem.acDmgInfo.hitPos.x,
                        this->enemyCollider.elem.acDmgInfo.hitPos.y, this->enemyCollider.elem.acDmgInfo.hitPos.z, 0, 0,
                        0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
        EnAm_TakeDamage(this, play);
        return true;
    }
    return false;
}

void EnAm_Update(Actor* thisx, PlayState* play) {
    EnAm* this = (EnAm*)thisx;
    s32 pad;

    if (EnAm_UpdateDamage(this, play) == false) {
        if (this->enemyCollider.base.atFlags & AT_BOUNCED) {
            this->enemyCollider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
            if (this->actor.colChkInfo.health == 0) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
            } else {
                func_808B0AD0(this, play);
            }
        }
    }
    if (this->returnHomeTimer != 0) {
        this->returnHomeTimer--;
    }
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 30.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Actor_SetFocus(&this->actor, 64.0f);
    Collider_UpdateCylinder(&this->actor, &this->enemyCollider);
    Collider_UpdateCylinder(&this->actor, &this->interactCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->enemyCollider.base);
    if (this->enemyCollider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->enemyCollider.base);
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->interactCollider.base);
    if (this->enemyCollider.base.atFlags & AT_ON) {
        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->enemyCollider.base);
    }
    Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
    this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.35f;
    this->drawDmgEffScale = (this->drawDmgEffScale > 0.7f) ? 0.7f : this->drawDmgEffScale;
}

static Vec3f D_808B1128[] = {
    { 4700.0f, -500.0f, 1800.0f },  // ENAM_BODYPART_0
    { 4700.0f, -500.0f, -1800.0f }, // ENAM_BODYPART_1
    { 2000.0f, -1500.0f, 0.0f },    // ENAM_BODYPART_2
    { 2000.0f, 0.0f, -1500.0f },    // ENAM_BODYPART_3
    { 2000.0f, 0.0f, 1500.0f },     // ENAM_BODYPART_4
};

static Vec3f D_808B1164[] = {
    { 0.0f, -3000.0f, 0.0f },  // ENAM_BODYPART_5, ENAM_BODYPART_7
    { 700.0f, -800.0f, 0.0f }, // ENAM_BODYPART_6, ENAM_BODYPART_8
};

static Vec3f D_808B117C[] = {
    { 800.0f, 1000.0f, -1000.0f },  // ENAM_BODYPART_9
    { 800.0f, 1000.0f, 1000.0f },   // ENAM_BODYPART_10
    { 800.0f, -1000.0f, 1000.0f },  // ENAM_BODYPART_11
    { 800.0f, -1000.0f, -1000.0f }, // ENAM_BODYPART_12
};

void EnAm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 i;
    s32 phi_s3;
    Vec3f* phi_s1;
    Vec3f* phi_s2;
    EnAm* this = (EnAm*)thisx;

    phi_s2 = 0;
    phi_s1 = 0;
    if (limbIndex == OBJECT_AM_LIMB_04) {
        phi_s2 = &this->bodyPartsPos[ENAM_BODYPART_0];
        phi_s1 = D_808B1128;
        phi_s3 = ARRAY_COUNT(D_808B1128);
    } else if (limbIndex == OBJECT_AM_LIMB_0D) {
        phi_s2 = &this->bodyPartsPos[ENAM_BODYPART_9];
        phi_s1 = D_808B117C;
        phi_s3 = ARRAY_COUNT(D_808B117C);
    } else if ((limbIndex == OBJECT_AM_LIMB_07) || (limbIndex == OBJECT_AM_LIMB_0A)) {
        phi_s2 = (limbIndex == OBJECT_AM_LIMB_07) ? &this->bodyPartsPos[ENAM_BODYPART_5]
                                                  : &this->bodyPartsPos[ENAM_BODYPART_7];
        phi_s1 = D_808B1164;
        phi_s3 = ARRAY_COUNT(D_808B1164);
    } else {
        phi_s3 = 0;
    }
    for (i = 0; i < phi_s3; i++, phi_s2++, phi_s1++) {
        Matrix_MultVec3f(phi_s1, phi_s2);
    }
}

void EnAm_Draw(Actor* thisx, PlayState* play) {
    Gfx* gfx;
    EnAm* this = (EnAm*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;
    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
    gDPSetEnvColor(&gfx[1], 0, 0, 0, this->textureBlend);
    POLY_OPA_DISP = &gfx[2];
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, EnAm_PostLimbDraw,
                      &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENAM_BODYPART_MAX, this->drawDmgEffScale, 0.0f,
                            this->drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);

    CLOSE_DISPS(play->state.gfxCtx);
}

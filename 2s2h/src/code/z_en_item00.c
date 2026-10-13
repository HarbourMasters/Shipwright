#include "global.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_gi_hearts/object_gi_hearts.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Elforg/z_en_elforg.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS 0x00000000

void EnItem00_Init(Actor* thisx, PlayState* play);
void EnItem00_Destroy(Actor* thisx, PlayState* play);
void EnItem00_Update(Actor* thisx, PlayState* play);
void EnItem00_Draw(Actor* thisx, PlayState* play);

void EnItem00_WaitForHeartObject(EnItem00* this, PlayState* play);
void func_800A640C(EnItem00* this, PlayState* play);
void func_800A6650(EnItem00* this, PlayState* play);
void func_800A6780(EnItem00* this, PlayState* play);
void func_800A6A40(EnItem00* this, PlayState* play);

void EnItem00_DrawRupee(EnItem00* this, PlayState* play);
void EnItem00_DrawSprite(EnItem00* this, PlayState* play);
void EnItem00_DrawHeartContainer(EnItem00* this, PlayState* play);
void EnItem00_DrawHeartPiece(EnItem00* this, PlayState* play);

ActorProfile En_Item00_Profile = {
    /**/ ACTOR_EN_ITEM00,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnItem00),
    /**/ EnItem00_Init,
    /**/ EnItem00_Destroy,
    /**/ EnItem00_Update,
    /**/ EnItem00_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AT_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000010, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

static s32 sBssPad;

void EnItem00_SetObject(EnItem00* this, PlayState* play, f32* shadowOffset, f32* shadowScale) {
    Actor_SetObjectDependency(play, &this->actor);
    Actor_SetScale(&this->actor, 0.5f);
    this->unk154 = 0.5f;
    *shadowOffset = 0.0f;
    *shadowScale = 0.6f;
    this->actor.world.rot.x = 0x4000;
}

void EnItem00_Init(Actor* thisx, PlayState* play) {
    EnItem00* this = (EnItem00*)thisx;
    s32 pad;
    f32 shadowOffset = 980.0f;
    f32 shadowScale = 6.0f;
    s32 getItemId = GI_NONE;
    s32 sp30 = ENITEM00_GET_8000(thisx) ? 1 : 0;

    this->collectibleFlag = ENITEM00_GET_7F00(thisx);

    thisx->params &= 0xFF;

    if (Flags_GetCollectible(play, this->collectibleFlag)) {
        if (thisx->params == ITEM00_HEART_PIECE) {
            sp30 = 0;
            this->collectibleFlag = 0;
            thisx->params = ITEM00_RECOVERY_HEART;
        } else {
            Actor_Kill(thisx);
            return;
        }
    }
    if (thisx->params == ITEM00_3_HEARTS) {
        thisx->params = ITEM00_RECOVERY_HEART;
    }

    Actor_ProcessInitChain(thisx, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, thisx, &sCylinderInit);

    this->unk150 = 1;

    switch (thisx->params) {
        case ITEM00_RUPEE_GREEN:
        case ITEM00_RUPEE_BLUE:
        case ITEM00_RUPEE_RED:
            Actor_SetScale(thisx, 0.015f);
            this->unk154 = 0.015f;
            shadowOffset = 750.0f;
            break;

        case ITEM00_SMALL_KEY:
            this->unk150 = 0;
            Actor_SetScale(thisx, 0.03f);
            this->unk154 = 0.03f;
            shadowOffset = 350.0f;
            break;

        case ITEM00_HEART_PIECE:
        case ITEM00_HEART_CONTAINER:
            this->unk150 = 0;
            Actor_SetScale(thisx, 0.02f);
            this->unk154 = 0.02f;
            shadowOffset = 650.0f;
            if (thisx->params == ITEM00_HEART_CONTAINER) {
                sp30 = -1;
            }
            break;

        case ITEM00_RECOVERY_HEART:
            thisx->home.rot.z = Rand_CenteredFloat(0xFFFF);
            shadowOffset = 430.0f;
            Actor_SetScale(thisx, 0.02f);
            this->unk154 = 0.02f;
            break;

        case ITEM00_ARROWS_10:
        case ITEM00_ARROWS_30:
        case ITEM00_ARROWS_40:
        case ITEM00_ARROWS_50:
            Actor_SetScale(thisx, 0.035f);
            this->unk154 = 0.035f;
            shadowOffset = 250.0f;
            break;

        case ITEM00_BOMBS_A:
        case ITEM00_BOMBS_B:
        case ITEM00_DEKU_NUTS_1:
        case ITEM00_DEKU_STICK:
        case ITEM00_MAGIC_JAR_SMALL:
        case ITEM00_DEKU_NUTS_10:
        case ITEM00_BOMBS_0:
            Actor_SetScale(thisx, 0.03f);
            this->unk154 = 0.03f;
            shadowOffset = 320.0f;
            break;

        case ITEM00_MAGIC_JAR_BIG:
            Actor_SetScale(thisx, 4.5f * 0.01f);
            this->unk154 = 4.5f * 0.01f;
            shadowOffset = 320.0f;
            break;

        case ITEM00_RUPEE_HUGE:
            Actor_SetScale(thisx, 4.5f * 0.01f);
            this->unk154 = 4.5f * 0.01f;
            shadowOffset = 750.0f;
            break;

        case ITEM00_RUPEE_PURPLE:
            Actor_SetScale(thisx, 0.03f);
            this->unk154 = 0.03f;
            shadowOffset = 750.0f;
            break;

        case ITEM00_FLEXIBLE:
        case ITEM00_BIG_FAIRY:
            shadowOffset = 500.0f;
            Actor_SetScale(thisx, 0.01f);
            this->unk154 = 0.01f;
            break;

        case ITEM00_SHIELD_HERO:
            thisx->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_SHIELD_2);
            EnItem00_SetObject(this, play, &shadowOffset, &shadowScale);
            break;

        case ITEM00_MAP:
            thisx->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_MAP);
            EnItem00_SetObject(this, play, &shadowOffset, &shadowScale);
            break;

        case ITEM00_COMPASS:
            thisx->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_COMPASS);
            EnItem00_SetObject(this, play, &shadowOffset, &shadowScale);
            break;

        default:
            break;
    }

    this->unk14E = 0;
    ActorShape_Init(&thisx->shape, shadowOffset, ActorShadow_DrawCircle, shadowScale);
    thisx->shape.shadowAlpha = 180;
    thisx->focus.pos = thisx->world.pos;
    this->getItemId = GI_NONE;

    if (sp30 < 0) {
        this->actionFunc = EnItem00_WaitForHeartObject;
        this->unk152 = -1;
        return;
    }

    if (sp30 == 0) {
        this->actionFunc = func_800A640C;
        this->unk152 = -1;
        return;
    }

    this->unk152 = 15;
    this->unk14C = 35;

    thisx->speed = 0.0f;
    thisx->velocity.y = 0.0f;
    thisx->gravity = 0.0f;

    switch (thisx->params) {
        case ITEM00_RUPEE_GREEN:
            Item_Give(play, ITEM_RUPEE_GREEN);
            break;

        case ITEM00_RUPEE_BLUE:
            Item_Give(play, ITEM_RUPEE_BLUE);
            break;

        case ITEM00_RUPEE_RED:
            Item_Give(play, ITEM_RUPEE_RED);
            break;

        case ITEM00_RUPEE_PURPLE:
            Item_Give(play, ITEM_RUPEE_PURPLE);
            break;

        case ITEM00_RUPEE_HUGE:
            Item_Give(play, ITEM_RUPEE_HUGE);
            break;

        case ITEM00_RECOVERY_HEART:
            Item_Give(play, ITEM_RECOVERY_HEART);
            break;

        case ITEM00_FLEXIBLE:
        case ITEM00_BIG_FAIRY:
            Health_ChangeBy(play, 0x70);
            break;

        case ITEM00_BOMBS_A:
        case ITEM00_BOMBS_B:
            Item_Give(play, ITEM_BOMBS_5);
            break;

        case ITEM00_ARROWS_10:
            Item_Give(play, ITEM_ARROWS_10);
            break;

        case ITEM00_ARROWS_30:
            Item_Give(play, ITEM_ARROWS_30);
            break;

        case ITEM00_ARROWS_40:
            Item_Give(play, ITEM_ARROWS_40);
            break;

        case ITEM00_ARROWS_50:
            Item_Give(play, ITEM_ARROWS_50);
            break;

        case ITEM00_MAGIC_JAR_BIG:
            Item_Give(play, ITEM_MAGIC_JAR_BIG);
            break;

        case ITEM00_MAGIC_JAR_SMALL:
            Item_Give(play, ITEM_MAGIC_JAR_SMALL);
            break;

        case ITEM00_SMALL_KEY:
            Item_Give(play, ITEM_KEY_SMALL);
            break;

        case ITEM00_DEKU_NUTS_1:
            getItemId = GI_DEKU_NUTS_1;
            break;

        case ITEM00_DEKU_NUTS_10:
            getItemId = GI_DEKU_NUTS_10;
            break;

        default:
            break;
    }

    if ((getItemId != GI_NONE) && !Actor_HasParent(thisx, play)) {
        Actor_OfferGetItem(thisx, play, getItemId, 50.0f, 20.0f);
    }

    this->actionFunc = func_800A6A40;
    this->actionFunc(this, play);
}

void EnItem00_Destroy(Actor* thisx, PlayState* play) {
    EnItem00* this = (EnItem00*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnItem00_WaitForHeartObject(EnItem00* this, PlayState* play) {
    s32 objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_HEARTS);

    if (Object_IsLoaded(&play->objectCtx, objectSlot)) {
        this->actor.objectSlot = objectSlot;
        this->actionFunc = func_800A640C;
    }
}

void func_800A640C(EnItem00* this, PlayState* play) {
    if ((this->actor.params <= ITEM00_RUPEE_RED) ||
        ((this->actor.params == ITEM00_RECOVERY_HEART) && (this->unk152 < 0)) ||
        (this->actor.params == ITEM00_HEART_PIECE) || (this->actor.params == ITEM00_HEART_CONTAINER)) {
        this->actor.shape.rot.y += 0x3C0;
    } else if ((this->actor.params >= ITEM00_SHIELD_HERO) && (this->actor.params != ITEM00_DEKU_NUTS_10) &&
               (this->actor.params < ITEM00_BOMBS_0)) {
        if (this->unk152 == -1) {
            if (!Math_SmoothStepToS(&this->actor.shape.rot.x, this->actor.world.rot.x - 0x4000, 2, 0xBB8, 0x5DC)) {
                this->unk152 = -2;
            }
        } else if (!Math_SmoothStepToS(&this->actor.shape.rot.x, -0x4000 - this->actor.world.rot.x, 2, 0xBB8, 0x5DC)) {
            this->unk152 = -1;
        }

        Math_SmoothStepToS(&this->actor.world.rot.x, 0, 2, 0x9C4, 0x1F4);
    } else if ((this->actor.params == ITEM00_MAP) || (this->actor.params == ITEM00_COMPASS)) {
        this->unk152 = -1;
        this->actor.shape.rot.y += 0x3C0;
    }

    if ((this->actor.params == ITEM00_HEART_PIECE) || (this->actor.params == ITEM00_HEART_CONTAINER)) {
        this->actor.shape.yOffset = (Math_SinS(this->actor.shape.rot.y) * 150.0f) + 850.0f;
    }

    Math_SmoothStepToF(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);

    if (this->unk14C == 0) {
        if ((this->actor.params != ITEM00_SMALL_KEY) && (this->actor.params != ITEM00_HEART_PIECE) &&
            (this->actor.params != ITEM00_HEART_CONTAINER)) {
            this->unk14C = -1;
        }
    }

    if (this->unk152 == 0) {
        if ((this->actor.params != ITEM00_SMALL_KEY) && (this->actor.params != ITEM00_HEART_PIECE) &&
            (this->actor.params != ITEM00_HEART_CONTAINER)) {
            Actor_Kill(&this->actor);
        }
    }

    if ((this->actor.gravity != 0.0f) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actionFunc = func_800A6650;
    }
}

static Color_RGBA8 sEffectPrimColor = { 255, 255, 127, 0 };
static Color_RGBA8 sEffectEnvColor = { 255, 255, 255, 0 };
static Vec3f sEffectVelocity = { 0.0f, 0.1f, 0.0f };
static Vec3f sEffectAccel = { 0.0f, 0.01f, 0.0f };

void func_800A6650(EnItem00* this, PlayState* play) {
    s32 pad;
    Vec3f pos;

    if (this->actor.params <= ITEM00_RUPEE_RED) {
        this->actor.shape.rot.y += 0x3C0;
    }

    if ((play->gameplayFrames & 1) != 0) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(10.0f);
        pos.y = this->actor.world.pos.y + Rand_CenteredFloat(10.0f);
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(10.0f);
        EffectSsKirakira_SpawnSmall(play, &pos, &sEffectVelocity, &sEffectAccel, &sEffectPrimColor, &sEffectEnvColor);
    }

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        if (this->actor.velocity.y > -2.0f) {
            this->actionFunc = func_800A640C;
        } else {
            this->actor.velocity.y *= -0.8f;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        }
    }
}

void func_800A6780(EnItem00* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    s32 var1;

    this->unk152++;

    if (this->actor.params == ITEM00_RECOVERY_HEART) {
        if (this->actor.velocity.y < 0.0f) {
            this->actor.speed = 0.0f;
            this->actor.gravity = -0.4f;
            if (this->actor.velocity.y < -1.5f) {
                this->actor.velocity.y = -1.5f;
            }
            this->actor.home.rot.z += TRUNCF_BINANG((this->actor.velocity.y + 3.0f) * 1000.0f);
            this->actor.world.pos.x +=
                (Math_CosS(this->actor.yawTowardsPlayer) * (-3.0f * Math_CosS(this->actor.home.rot.z)));
            this->actor.world.pos.z +=
                (Math_SinS(this->actor.yawTowardsPlayer) * (-3.0f * Math_CosS(this->actor.home.rot.z)));
        }
    }

    if (this->actor.params <= ITEM00_RUPEE_RED) {
        this->actor.shape.rot.y += 0x3C0;
    } else if ((this->actor.params >= ITEM00_SHIELD_HERO) && (this->actor.params != ITEM00_DEKU_NUTS_10) &&
               (this->actor.params != ITEM00_BOMBS_0)) {
        this->actor.world.rot.x -= 0x2BC;
        this->actor.shape.rot.y += 0x190;
        this->actor.shape.rot.x = this->actor.world.rot.x - 0x4000;
    }

    if (this->actor.velocity.y <= 2.0f) {
        var1 = (u16)this->actor.shape.rot.z + 0x2710;
        if (var1 < 0xFFFF) {
            this->actor.shape.rot.z += 0x2710;
        } else {
            this->actor.shape.rot.z = -1;
        }
    }

    if ((play->gameplayFrames & 1) == 0) {
        pos.x = this->actor.world.pos.x + ((Rand_ZeroOne() - 0.5f) * 10.0f);
        pos.y = this->actor.world.pos.y + ((Rand_ZeroOne() - 0.5f) * 10.0f);
        pos.z = this->actor.world.pos.z + ((Rand_ZeroOne() - 0.5f) * 10.0f);
        EffectSsKirakira_SpawnSmall(play, &pos, &sEffectVelocity, &sEffectAccel, &sEffectPrimColor, &sEffectEnvColor);
    }

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        this->actionFunc = func_800A640C;
        this->actor.shape.rot.z = 0;
        this->actor.speed = 0.0f;
    }
}

void func_800A6A40(EnItem00* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->getItemId != GI_NONE) {
        if (!Actor_HasParent(&this->actor, play)) {
            Actor_OfferGetItem(&this->actor, play, this->getItemId, 50.0f, 80.0f);
            this->unk152++;
        } else {
            this->getItemId = GI_NONE;
        }
    }

    if (this->unk152 == 0) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.world.pos = player->actor.world.pos;

    if (this->actor.params <= ITEM00_RUPEE_RED) {
        this->actor.shape.rot.y += 0x3C0;
    } else if (this->actor.params == ITEM00_RECOVERY_HEART) {
        this->actor.shape.rot.y = 0;
    }

    this->actor.world.pos.y += (40.0f + (Math_SinS(this->unk152 * 15000) * (this->unk152 * 0.3f)));

    if (LINK_IS_ADULT) {
        this->actor.world.pos.y += 20.0f;
    }
}

void EnItem00_Update(Actor* thisx, PlayState* play) {
    EnItem00* this = (EnItem00*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 sp38 = player->stateFlags3 & PLAYER_STATE3_1000;
    s32 getItemId = GI_NONE;
    s32 params;

    if (this->unk152 > 0) {
        this->unk152--;
    }

    if ((this->unk152 > 0) && (this->unk152 <= 40) && (this->unk14C <= 0)) {
        this->unk14E = this->unk152;
    }

    this->actionFunc(this, play);

    Math_SmoothStepToF(&this->actor.scale.x, this->unk154, 0.1f, this->unk154 * 0.1f, 0.0f);
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = this->actor.scale.x;

    if (this->actor.gravity != 0.0f) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 15.0f, 15.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);

        if (this->actor.floorHeight <= BGCHECK_Y_MIN) {
            Actor_Kill(&this->actor);
            return;
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    params = this->actor.params;
    if ((params == ITEM00_SHIELD_HERO) || (params == ITEM00_MAP) || (params == ITEM00_COMPASS)) {
        this->actor.shape.yOffset = fabsf(Math_CosS(this->actor.shape.rot.x) * 37.0f);
    }

    if (this->unk14C > 0) {
        return;
    }

    if (!((sp38 != 0) && (this->actor.xzDistToPlayer <= 60.0f) && (this->actor.playerHeightRel >= -100.0f) &&
          (this->actor.playerHeightRel <= 100.0f)) &&
        !((sp38 == 0) && (this->actor.xzDistToPlayer <= 30.0f) && (this->actor.playerHeightRel >= -50.0f) &&
          (this->actor.playerHeightRel <= 50.0f))) {
        if (!Actor_HasParent(&this->actor, play)) {
            return;
        }
    }

    if (!GameInteractor_Should(VB_GIVE_ITEM_FROM_ITEM00, true, this)) {
        return;
    }

    if (play->gameOverCtx.state != GAMEOVER_INACTIVE) {
        return;
    }

    switch (this->actor.params) {
        case ITEM00_RUPEE_GREEN:
            this->unk1A4 = 1;
            Item_Give(play, ITEM_RUPEE_GREEN);
            break;

        case ITEM00_RUPEE_BLUE:
            this->unk1A4 = 1;
            Item_Give(play, ITEM_RUPEE_BLUE);
            break;

        case ITEM00_RUPEE_RED:
            this->unk1A4 = 1;
            Item_Give(play, ITEM_RUPEE_RED);
            break;

        case ITEM00_RUPEE_PURPLE:
            this->unk1A4 = 1;
            Item_Give(play, ITEM_RUPEE_PURPLE);
            break;

        case ITEM00_RUPEE_HUGE:
            this->unk1A4 = 1;
            Item_Give(play, ITEM_RUPEE_HUGE);
            break;

        case ITEM00_DEKU_STICK:
            getItemId = GI_DEKU_STICKS_1;
            break;

        case ITEM00_DEKU_NUTS_1:
            getItemId = GI_DEKU_NUTS_1;
            break;

        case ITEM00_DEKU_NUTS_10:
            getItemId = GI_DEKU_NUTS_10;
            break;

        case ITEM00_RECOVERY_HEART:
            Item_Give(play, ITEM_RECOVERY_HEART);
            break;

        case ITEM00_FLEXIBLE:
        case ITEM00_BIG_FAIRY:
            Health_ChangeBy(play, 0x70);
            break;

        case ITEM00_BOMBS_A:
        case ITEM00_BOMBS_B:
            Item_Give(play, ITEM_BOMBS_5);
            break;

        case ITEM00_ARROWS_10:
            Item_Give(play, ITEM_ARROWS_10);
            break;

        case ITEM00_ARROWS_30:
            Item_Give(play, ITEM_ARROWS_30);
            break;

        case ITEM00_ARROWS_40:
            Item_Give(play, ITEM_ARROWS_40);
            break;

        case ITEM00_ARROWS_50:
            Item_Give(play, ITEM_ARROWS_50);
            break;

        case ITEM00_SMALL_KEY:
            getItemId = GI_KEY_SMALL;
            break;

        case ITEM00_HEART_PIECE:
            getItemId = GI_HEART_PIECE;
            break;

        case ITEM00_HEART_CONTAINER:
            getItemId = GI_HEART_CONTAINER;
            break;

        case ITEM00_MAGIC_JAR_BIG:
            Item_Give(play, ITEM_MAGIC_JAR_BIG);
            break;

        case ITEM00_MAGIC_JAR_SMALL:
            Item_Give(play, ITEM_MAGIC_JAR_SMALL);
            break;

        case ITEM00_SHIELD_HERO:
            getItemId = GI_SHIELD_HERO;
            break;

        case ITEM00_MAP:
            getItemId = GI_MAP;
            break;

        case ITEM00_COMPASS:
            getItemId = GI_COMPASS;
            break;

        default:
            break;
    }

    if (getItemId != GI_NONE) {
        if (!Actor_HasParent(&this->actor, play)) {
            Actor_OfferGetItem(&this->actor, play, getItemId, 50.0f, 20.0f);
        }
    }

    switch (this->actor.params) {
        case ITEM00_HEART_PIECE:
        case ITEM00_HEART_CONTAINER:
        case ITEM00_SMALL_KEY:
        case ITEM00_SHIELD_HERO:
        case ITEM00_MAP:
        case ITEM00_COMPASS:
            if (Actor_HasParent(&this->actor, play)) {
                Flags_SetCollectible(play, this->collectibleFlag);
                Actor_Kill(&this->actor);
            }
            return;

        default:
            break;
    }

    if ((this->actor.params <= ITEM00_RUPEE_RED) || (this->actor.params == ITEM00_RUPEE_HUGE)) {
        Audio_PlaySfx(NA_SE_SY_GET_RUPY);
    } else if (getItemId != GI_NONE) {
        if (Actor_HasParent(&this->actor, play)) {
            Flags_SetCollectible(play, this->collectibleFlag);
            Actor_Kill(&this->actor);
        }
        return;
    } else {
        Audio_PlaySfx(NA_SE_SY_GET_ITEM);
    }

    Flags_SetCollectible(play, this->collectibleFlag);

    this->unk152 = 15;
    this->unk14C = 35;
    this->actor.shape.rot.z = 0;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;

    Actor_SetScale(&this->actor, this->unk154);

    this->getItemId = GI_NONE;
    this->actionFunc = func_800A6A40;
}

void EnItem00_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnItem00* this = (EnItem00*)thisx;

    if (!(this->unk14E & this->unk150)) {
        // 2S2H [Interpolation] Skip interpolation when the item moves from the ground to over the player head
        Player* player = GET_PLAYER(play);
        bool itemOnPlayer =
            player->actor.home.pos.x == this->actor.world.pos.x && player->actor.home.pos.z == this->actor.world.pos.z;
        FrameInterpolation_RecordOpenChild(this, itemOnPlayer ? 1 : 0);
        FrameInterpolation_IgnoreActorMtx();

        switch (this->actor.params) {
            case ITEM00_RUPEE_GREEN:
            case ITEM00_RUPEE_BLUE:
            case ITEM00_RUPEE_RED:
            case ITEM00_RUPEE_HUGE:
            case ITEM00_RUPEE_PURPLE:
                EnItem00_DrawRupee(this, play);
                break;

            case ITEM00_HEART_PIECE:
                EnItem00_DrawHeartPiece(this, play);
                break;

            case ITEM00_HEART_CONTAINER:
                EnItem00_DrawHeartContainer(this, play);
                break;

            case ITEM00_RECOVERY_HEART:
                if (this->unk152 < 0) {
                    if (this->unk152 == -1) {
                        s8 objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_GI_HEART);

                        if (Object_IsLoaded(&play->objectCtx, objectSlot)) {
                            this->actor.objectSlot = objectSlot;
                            Actor_SetObjectDependency(play, &this->actor);
                            this->unk152 = -2;
                        }
                    } else {
                        Matrix_Scale(16.0f, 16.0f, 16.0f, MTXMODE_APPLY);
                        GetItem_Draw(play, GID_RECOVERY_HEART);
                    }
                    break;
                }
                // fallthrough
            case ITEM00_BOMBS_A:
            case ITEM00_ARROWS_10:
            case ITEM00_ARROWS_30:
            case ITEM00_ARROWS_40:
            case ITEM00_ARROWS_50:
            case ITEM00_BOMBS_B:
            case ITEM00_DEKU_NUTS_1:
            case ITEM00_DEKU_STICK:
            case ITEM00_MAGIC_JAR_BIG:
            case ITEM00_MAGIC_JAR_SMALL:
            case ITEM00_SMALL_KEY:
            case ITEM00_DEKU_NUTS_10:
            case ITEM00_BOMBS_0:
                EnItem00_DrawSprite(this, play);
                break;

            case ITEM00_SHIELD_HERO:
                GetItem_Draw(play, GID_SHIELD_HERO);
                break;

            case ITEM00_MAP:
                GetItem_Draw(play, GID_DUNGEON_MAP);
                break;

            case ITEM00_COMPASS:
                GetItem_Draw(play, GID_COMPASS);
                break;

            case ITEM00_MASK:
            case ITEM00_FLEXIBLE:
            case ITEM00_3_HEARTS:
            case ITEM00_NOTHING:
            case ITEM00_BIG_FAIRY:
                break;
        }

        FrameInterpolation_RecordCloseChild();
    }
}

static TexturePtr sRupeeTextures[] = {
    gRupeeGreenTex, gRupeeBlueTex, gRupeeRedTex, gRupeeOrangeTex, gRupeePurpleTex,
};

void EnItem00_DrawRupee(EnItem00* this, PlayState* play) {
    s32 pad;
    s32 texIndex;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    func_800B8050(&this->actor, play, 0);

    if (this->actor.params <= ITEM00_RUPEE_RED) {
        texIndex = this->actor.params;
    } else {
        texIndex = this->actor.params - 0x10;
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sRupeeTextures[texIndex]));

    gSPDisplayList(POLY_OPA_DISP++, gRupeeDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

TexturePtr sItemDropTextures[] = {
    gDropRecoveryHeartTex, // Heart (Not used)
    gDropBombTex,          // Bombs (A), Bombs (0)
    gDropArrows1Tex,       // Arrows (10)
    gDropArrows2Tex,       // Arrows (30)
    gDropArrows3Tex,       // Arrows (40), Arrows (50)
    gDropBombTex,          // Bombs (B)
    gDropDekuNutTex,       // Nuts (1), Nuts (10)
    gDropDekuStickTex,     // Sticks (1)
    gDropMagicLargeTex,    // Magic (Large)
    gDropMagicSmallTex,    // Magic (Small)
    NULL,
    gDropKeySmallTex // Small Key
};

void EnItem00_DrawSprite(EnItem00* this, PlayState* play) {
    s32 texIndex = this->actor.params - 3;

    OPEN_DISPS(play->state.gfxCtx);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    if (this->actor.params == ITEM00_DEKU_NUTS_10) {
        texIndex = 6;
    } else if (this->actor.params == ITEM00_BOMBS_0) {
        texIndex = 1;
    } else if (this->actor.params >= ITEM00_ARROWS_30) {
        texIndex -= 3;
        if (this->actor.params < ITEM00_ARROWS_50) {
            texIndex++;
        }
    }

    POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sItemDropTextures[texIndex]));

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnItem00_DrawHeartContainer(EnItem00* this, PlayState* play) {
    s32 pad[2];

    if (Object_GetSlot(&play->objectCtx, OBJECT_GI_HEARTS) == this->actor.objectSlot) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Scale(20.0f, 20.0f, 20.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, gGiHeartBorderDL);
        gSPDisplayList(POLY_XLU_DISP++, gGiHeartContainerDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnItem00_DrawHeartPiece(EnItem00* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gHeartPieceInteriorDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s16 func_800A7650(s16 dropId) {
    if ((((dropId == ITEM00_BOMBS_A) || (dropId == ITEM00_BOMBS_0) || (dropId == ITEM00_BOMBS_B)) &&
         (INV_CONTENT(ITEM_BOMB) == ITEM_NONE)) ||
        (((dropId == ITEM00_ARROWS_10) || (dropId == ITEM00_ARROWS_30) || (dropId == ITEM00_ARROWS_40) ||
          (dropId == ITEM00_ARROWS_50)) &&
         (INV_CONTENT(ITEM_BOW) == ITEM_NONE)) ||
        (((dropId == ITEM00_MAGIC_JAR_BIG) || (dropId == ITEM00_MAGIC_JAR_SMALL)) &&
         (gSaveContext.save.saveInfo.playerData.magicLevel == 0))) {
        return ITEM00_NO_DROP;
    }

    if (dropId == ITEM00_RECOVERY_HEART) {
        if (((void)0, gSaveContext.save.saveInfo.playerData.healthCapacity) ==
            ((void)0, gSaveContext.save.saveInfo.playerData.health)) {
            return ITEM00_RUPEE_GREEN;
        }
    }

    return dropId;
}

Actor* Item_DropCollectible(PlayState* play, Vec3f* spawnPos, u32 params) {

    s32 pad;
    Actor* spawnedActor = NULL;
    s32 newParamFF;
    s32 param10000 = params & 0x10000;
    s16 param8000 = params & 0x8000;
    s16 param7F00 = params & 0x7F00;
    s32 param20000 = params & 0x20000;
    s32 paramFF = params & 0xFF;
    s32 i;

    if ((GameInteractor_Should(VB_DROP_COLLECTIBLE, true, *spawnPos, params))) {
        params &= 0x7FFF;
        newParamFF = params & 0xFF;

        if (paramFF == ITEM00_3_HEARTS) {
            for (i = 0; i < 3; i++) {
                spawnedActor = Item_DropCollectible(play, spawnPos, param7F00 | ITEM00_RECOVERY_HEART | param8000);
            }
        } else if (paramFF == ITEM00_MUSHROOM_CLOUD) {
            param7F00 >>= 8;
            if (!Flags_GetCollectible(play, param7F00)) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_KINOKO, spawnPos->x, spawnPos->y, spawnPos->z, 0, 0, 0,
                            param7F00);
            }
        } else if (((paramFF == ITEM00_FLEXIBLE) || (newParamFF == ITEM00_BIG_FAIRY)) && (param10000 == 0)) {
            newParamFF = params & 0xFF;
            if (newParamFF == ITEM00_FLEXIBLE) {
                spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, spawnPos->x, spawnPos->y + 40.0f,
                                           spawnPos->z, 0, 0, 0, FAIRY_PARAMS(FAIRY_TYPE_2, true, param7F00 >> 8));
                if (!Flags_GetCollectible(play, (param7F00 >> 8) & 0x7F)) {
                    SoundSource_PlaySfxAtFixedWorldPos(play, spawnPos, 40, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                }
            } else {
                spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELFORG, spawnPos->x, spawnPos->y + 40.0f,
                                           spawnPos->z, 0, 0, 0,
                                           STRAY_FAIRY_PARAMS((param7F00 >> 8) & 0x7F, STRAY_FAIRY_AREA_CLOCK_TOWN,
                                                              STRAY_FAIRY_TYPE_COLLECTIBLE));
                if (param20000 == 0) {
                    if (!Flags_GetCollectible(play, (param7F00 >> 8) & 0x7F)) {
                        SoundSource_PlaySfxAtFixedWorldPos(play, spawnPos, 40, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                    }
                }
            }
        } else {
            if (param8000 == 0) {
                params = func_800A7650(newParamFF);
            }
            if ((s32)params != ITEM00_NO_DROP) {
                spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, spawnPos->x, spawnPos->y,
                                           spawnPos->z, 0, 0, 0, (s32)params | param8000 | param7F00);
                if ((spawnedActor != NULL) && (param8000 == 0)) {
                    if (param10000 == 0) {
                        spawnedActor->velocity.y = 8.0f;
                    } else {
                        spawnedActor->velocity.y = -2.0f;
                    }
                    spawnedActor->speed = 2.0f;
                    spawnedActor->gravity = -0.9f;
                    spawnedActor->world.rot.y = Rand_CenteredFloat(0x10000);
                    Actor_SetScale(spawnedActor, 0.0f);
                    ((EnItem00*)spawnedActor)->actionFunc = func_800A6780;
                    ((EnItem00*)spawnedActor)->unk152 = 0xDC;
                    if ((spawnedActor->params != ITEM00_SMALL_KEY) && (spawnedActor->params != ITEM00_HEART_PIECE) &&
                        (spawnedActor->params != ITEM00_HEART_CONTAINER)) {
                        spawnedActor->room = -1;
                    }
                    spawnedActor->flags |= 0x0010;
                }
            }
        }
    }

    return spawnedActor;
}

Actor* Item_DropCollectible2(PlayState* play, Vec3f* spawnPos, s32 params) {
    Actor* spawnedActor = NULL;
    s32 pad;
    s32 param10000 = params & 0x10000;
    s16 param8000 = params & 0x8000;
    s16 param7F00 = params & 0x7F00;

    params &= 0xFF;

    if ((params & 0xFF) == ITEM00_3_HEARTS) {
        return NULL;
    }

    if ((((params & 0xFF) == ITEM00_FLEXIBLE) || ((params & 0xFF) == ITEM00_BIG_FAIRY)) && (param10000 == 0)) {
        if ((params & 0xFF) == ITEM00_FLEXIBLE) {
            spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, spawnPos->x, spawnPos->y + 40.0f,
                                       spawnPos->z, 0, 0, 0, FAIRY_PARAMS(FAIRY_TYPE_2, true, param7F00 >> 8));
        } else {
            spawnedActor = Actor_Spawn(
                &play->actorCtx, play, ACTOR_EN_ELFORG, spawnPos->x, spawnPos->y + 40.0f, spawnPos->z, 0, 0, 0,
                STRAY_FAIRY_PARAMS((param7F00 >> 8) & 0x7F, STRAY_FAIRY_AREA_CLOCK_TOWN, STRAY_FAIRY_TYPE_COLLECTIBLE));
        }
        if (Flags_GetCollectible(play, (param7F00 >> 8) & 0x7F) == 0) {
            SoundSource_PlaySfxAtFixedWorldPos(play, spawnPos, 40, NA_SE_EV_BUTTERFRY_TO_FAIRY);
        }
    } else {
        params = func_800A7650(params & 0xFF);
        if (params != ITEM00_NO_DROP) {
            spawnedActor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, spawnPos->x, spawnPos->y, spawnPos->z, 0,
                                       0, 0, (s32)params | param8000 | param7F00);
            if (spawnedActor != NULL) {
                if (param8000 == 0) {
                    spawnedActor->velocity.y = 0.0f;
                    spawnedActor->speed = 0.0f;
                    if (param10000 != 0) {
                        spawnedActor->gravity = 0.0f;
                    } else {
                        spawnedActor->gravity = -0.9f;
                    }
                    spawnedActor->world.rot.y = Rand_CenteredFloat(0x10000);
                    spawnedActor->flags |= 0x10;
                }
            }
        }
    }

    return spawnedActor;
}

u8 sDropTable[DROP_TABLE_SIZE * DROP_TABLE_NUMBER] = {
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RUPEE_GREEN,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RUPEE_GREEN,
    ITEM00_NO_DROP,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_NO_DROP,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_RUPEE_RED,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_30,
    ITEM00_ARROWS_30,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_BOMBS_A,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_DEKU_NUTS_1,
    ITEM00_DEKU_NUTS_1,
    ITEM00_NO_DROP,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_DEKU_STICK,
    ITEM00_DEKU_STICK,
    ITEM00_NO_DROP,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RUPEE_GREEN,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_BLUE,
    ITEM00_RUPEE_RED,
    ITEM00_NO_DROP,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_10,
    ITEM00_ARROWS_30,
    ITEM00_BOMBS_A,
    ITEM00_NO_DROP,
    ITEM00_DEKU_STICK,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_FLEXIBLE,
    ITEM00_RUPEE_GREEN,
    ITEM00_RECOVERY_HEART,
    ITEM00_RECOVERY_HEART,
    ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MASK,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
    ITEM00_NO_DROP,
};

u8 sDropTableAmounts[DROP_TABLE_SIZE * DROP_TABLE_NUMBER] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x01, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

void Item_DropCollectibleRandom(PlayState* play, Actor* fromActor, Vec3f* spawnPos, s16 params) {

    EnItem00* spawnedActor;
    u8 dropId;
    s32 dropQuantity;
    s16 dropTableIndex = Rand_ZeroOne() * 16.0f;
    s16 param8000 = params & 0x8000;
    u8 dropFlag;

    params &= 0x1F0;

    if (params < 0x101) {
        dropId = sDropTable[params + dropTableIndex];
        dropQuantity = sDropTableAmounts[params + dropTableIndex];
        if (!(GameInteractor_Should(VB_DROP_COLLECTIBLE, true, *spawnPos, dropId))) {
            return;
        }

        if (dropId == ITEM00_MASK) {
            switch (GET_PLAYER_FORM) {
                case PLAYER_FORM_HUMAN:
                    dropId = ITEM00_ARROWS_10;
                    break;

                case PLAYER_FORM_ZORA:
                    dropId = ITEM00_RECOVERY_HEART;
                    break;

                case PLAYER_FORM_GORON:
                    dropId = ITEM00_MAGIC_JAR_SMALL;
                    break;

                default:
                    dropId = ITEM00_RUPEE_GREEN;
                    break;
            }
            dropQuantity = 1;
        }

        if (fromActor != NULL) {
            dropFlag = fromActor->dropFlag;

            if (dropFlag != DROPFLAG_NONE) {
                if (fromActor->dropFlag & DROPFLAG_1) {
                    params = 0x10;
                    dropId = ITEM00_ARROWS_30;
                    dropQuantity = 1;
                } else if (fromActor->dropFlag & DROPFLAG_2) {
                    params = 0x10;
                    dropId = ITEM00_RECOVERY_HEART;
                    dropQuantity = 1;
                } else if (fromActor->dropFlag & DROPFLAG_20) {
                    dropId = ITEM00_RUPEE_PURPLE;
                    dropQuantity = 1;
                }
            }
        }

        if (dropId == ITEM00_FLEXIBLE) {
            if (gSaveContext.save.saveInfo.playerData.health <= 0x10) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ELF, spawnPos->x, spawnPos->y + 40.0f, spawnPos->z, 0, 0, 0,
                            FAIRY_PARAMS(FAIRY_TYPE_2, false, 0));
                SoundSource_PlaySfxAtFixedWorldPos(play, spawnPos, 40, NA_SE_EV_BUTTERFRY_TO_FAIRY);
                return;
            }

            if (gSaveContext.save.saveInfo.playerData.health <= 0x30) {
                params = 0x10;
                dropId = ITEM00_RECOVERY_HEART;
                dropQuantity = 3;
            } else if (gSaveContext.save.saveInfo.playerData.health <= 0x50) {
                params = 0x10;
                dropId = ITEM00_RECOVERY_HEART;
                dropQuantity = 1;
            } else if ((gSaveContext.save.saveInfo.playerData.magicLevel != 0) &&
                       (gSaveContext.save.saveInfo.playerData.magic == 0)) {
                params = 0xD0;
                dropId = ITEM00_MAGIC_JAR_BIG;
                dropQuantity = 1;
            } else if ((gSaveContext.save.saveInfo.playerData.magicLevel != 0) &&
                       ((gSaveContext.save.saveInfo.playerData.magicLevel >> 1) >=
                        gSaveContext.save.saveInfo.playerData.magic)) {
                params = 0xD0;
                dropId = ITEM00_MAGIC_JAR_BIG;
                dropQuantity = 1;
            } else if (AMMO(ITEM_BOW) < 6) {
                params = 0xA0;
                dropId = ITEM00_ARROWS_30;
                dropQuantity = 1;
            } else if (AMMO(ITEM_BOMB) < 6) {
                params = 0xB0;
                dropId = ITEM00_BOMBS_A;
                dropQuantity = 1;
            } else if (gSaveContext.save.saveInfo.playerData.rupees < 11) {
                params = 0xA0;
                dropId = ITEM00_RUPEE_RED;
                dropQuantity = 1;
            } else {
                return;
            }
        }

        if (dropId != (u8)ITEM00_NO_DROP) {
            while (dropQuantity > 0) {
                if (param8000 == 0) {
                    dropId = func_800A7650(dropId);
                    if (dropId != (u8)ITEM00_NO_DROP) {
                        spawnedActor = (EnItem00*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, spawnPos->x,
                                                              spawnPos->y, spawnPos->z, 0, 0, 0, dropId);
                        if ((spawnedActor != 0) && (dropId != (u8)ITEM00_NO_DROP)) {
                            spawnedActor->actor.velocity.y = 8.0f;
                            spawnedActor->actor.speed = 2.0f;
                            spawnedActor->actor.gravity = -0.9f;
                            spawnedActor->actor.world.rot.y = Rand_ZeroOne() * 40000.0f;
                            Actor_SetScale(&spawnedActor->actor, 0.0f);
                            spawnedActor->actionFunc = func_800A6780;
                            spawnedActor->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                            if ((spawnedActor->actor.params != ITEM00_SMALL_KEY) &&
                                (spawnedActor->actor.params != ITEM00_HEART_PIECE) &&
                                (spawnedActor->actor.params != ITEM00_HEART_CONTAINER)) {
                                spawnedActor->actor.room = -1;
                            }
                            spawnedActor->unk152 = 220;
                        }
                    }
                } else {
                    Item_DropCollectible(play, spawnPos, params | 0x8000);
                }

                dropQuantity--;
            }
        }
    }
}

s32 D_801AE194[32] = {
    ITEM00_NO_DROP,        ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE,  ITEM00_NO_DROP,         ITEM00_RUPEE_RED,
    ITEM00_RUPEE_PURPLE,   ITEM00_NO_DROP,     ITEM00_RUPEE_HUGE,  ITEM00_COMPASS,         ITEM00_MUSHROOM_CLOUD,
    ITEM00_RECOVERY_HEART, ITEM00_3_HEARTS,    ITEM00_HEART_PIECE, ITEM00_HEART_CONTAINER, ITEM00_MAGIC_JAR_SMALL,
    ITEM00_MAGIC_JAR_BIG,  ITEM00_FLEXIBLE,    ITEM00_BIG_FAIRY,   ITEM00_NO_DROP,         ITEM00_DEKU_NUTS_10,
    ITEM00_NO_DROP,        ITEM00_BOMBS_A,     ITEM00_NO_DROP,     ITEM00_NO_DROP,         ITEM00_NO_DROP,
    ITEM00_DEKU_STICK,     ITEM00_NO_DROP,     ITEM00_NO_DROP,     ITEM00_NO_DROP,         ITEM00_NO_DROP,
    ITEM00_ARROWS_10,      ITEM00_ARROWS_30,
};

s32 func_800A8150(s32 index) {
    if ((index < 0) || (index >= ARRAY_COUNT(D_801AE194))) {
        return ITEM00_NO_DROP;
    }

    return D_801AE194[index];
}

u8 D_801AE214[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

s32 func_800A817C(s32 index) {
    if ((index < 0) || (index >= ARRAY_COUNT(D_801AE214))) {
        return 0;
    }

    return D_801AE214[index];
}

bool Item_CanDropBigFairy(PlayState* play, s32 index, s32 collectibleFlag) {
    return (func_800A8150(index) == ITEM00_BIG_FAIRY) && (!Flags_GetCollectible(play, collectibleFlag));
}

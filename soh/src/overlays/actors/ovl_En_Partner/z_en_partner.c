/*
 * File: z_en_partner.c
 * Overlay: ovl_En_Partner
 * Description: Navi Coop Partner
 */

#include "z_en_partner.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include <overlays/actors/ovl_En_Arrow/z_en_arrow.h>
#include "overlays/actors/ovl_En_Si/z_en_si.h"
#include <objects/object_link_child/object_link_child.h>
#include <overlays/actors/ovl_En_Bom/z_en_bom.h>
#include <overlays/actors/ovl_Obj_Switch/z_obj_switch.h>

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED | ACTOR_FLAG_DRAGGED_BY_HOOKSHOT | ACTOR_FLAG_CAN_PRESS_SWITCH)

void EnPartner_Init(Actor* thisx, PlayState* play);
void EnPartner_Destroy(Actor* thisx, PlayState* play);
void EnPartner_Update(Actor* thisx, PlayState* play);
void EnPartner_Draw(Actor* thisx, PlayState* play);
void EnPartner_SpawnSparkles(EnPartner* this, PlayState* play, s32 sparkleLife);

void func_808328EC(Player* this, u16 sfxId);
void func_808429B4(PlayState* play, s32 speed, s32 y, s32 countdown);
s32 spawn_boomerang_ivan(EnPartner* this, PlayState* play);

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 8, ICHAIN_STOP),
};

static Color_RGBAf sInnerColors[] = {
    { 255.0f, 255.0f, 255.0f, 255.0f },
};

static Color_RGBAf sOuterColors[] = {
    { 0.0f, 255.0f, 0.0f, 255.0f },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00020002, 0x00, 0x01 },
        { 0x4FC00748, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 27, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sCCInfoInit = { 0, 12, 60, MASS_HEAVY };

void EnPartner_Init(Actor* thisx, PlayState* play) {
    EnPartner* this = (EnPartner*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 i;

    this->usedItem = 0xFF;
    this->canMove = 1;
    this->shouldDraw = 1;
    this->hookshotTarget = NULL;
    GET_PLAYER(play)->ivanFloating = 0;

    this->innerColor.r = 255.0f;
    this->innerColor.g = 255.0f;
    this->innerColor.b = 255.0f;
    this->innerColor.a = 255.0f;

    this->outerColor.r = 0.0f;
    this->outerColor.g = 255.0f;
    this->outerColor.b = 0.0f;
    this->outerColor.a = 255.0f;

    this->usedItemButton = 0xFF;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sCCInfoInit);
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
    this->collider.info.toucher.damage = 1;
    GET_PLAYER(play)->ivanDamageMultiplier = 1;

    Actor_ProcessInitChain(thisx, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &gFairySkel, &gFairyAnim, this->jointTable, this->morphTable, 15);
    ActorShape_Init(&thisx->shape, 1000.0f, ActorShadow_DrawCircle, 15.0f);
    thisx->shape.shadowAlpha = 0xFF;

    Lights_PointGlowSetInfo(&this->lightInfoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 200, 255,
                            200, 0);
    this->lightNodeGlow = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfoGlow);

    Lights_PointNoGlowSetInfo(&this->lightInfoNoGlow, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, 200,
                              255, 200, 0);
    this->lightNodeNoGlow = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfoNoGlow);

	thisx->room = -1;
}

void EnPartner_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPartner* this = (EnPartner*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNodeGlow);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNodeNoGlow);

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnPartner_UpdateLights(EnPartner* this, PlayState* play) {
    s16 glowLightRadius = 100;
    s16 lightRadius = 200;

    if (this->shouldDraw == 0) {
        glowLightRadius = 0;
        lightRadius = 0;
    }

    Player* player;

    player = GET_PLAYER(play);
    Lights_PointNoGlowSetInfo(&this->lightInfoNoGlow, player->actor.world.pos.x, (s16)(player->actor.world.pos.y) + 69,
                              player->actor.world.pos.z, 200, 255, 200, lightRadius);

    Lights_PointGlowSetInfo(&this->lightInfoGlow, this->actor.world.pos.x, this->actor.world.pos.y + 9,
                            this->actor.world.pos.z, 200, 255, 200, glowLightRadius);

    Actor_SetScale(&this->actor, this->actor.scale.x);
}

void EnPartner_SpawnSparkles(EnPartner* this, PlayState* play, s32 sparkleLife) {
    static Vec3f sparkleVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f sparkleAccel = { 0.0f, -0.025f, 0.0f };
    s32 pad;
    Vec3f sparklePos;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;

    sparklePos.x = Rand_CenteredFloat(6.0f) + this->actor.world.pos.x;
    sparklePos.y = (Rand_ZeroOne() * 6.0f) + this->actor.world.pos.y + 5;
    sparklePos.z = Rand_CenteredFloat(6.0f) + this->actor.world.pos.z;

    primColor.r = this->innerColor.r;
    primColor.g = this->innerColor.g;
    primColor.b = this->innerColor.b;

    envColor.r = this->outerColor.r;
    envColor.g = this->outerColor.g;
    envColor.b = this->outerColor.b;

    EffectSsKiraKira_SpawnDispersed(play, &sparklePos, &sparkleVelocity, &sparkleAccel, &primColor, &envColor,
                                    1500, sparkleLife);
}

Vec3f Vec3fNormalize(Vec3f vec) {
    f32 norm = sqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

    if (norm != 0.0f) {
        vec.x /= norm;
        vec.y /= norm;
        vec.z /= norm;
    } else {
        vec.x = vec.y = vec.z = 0.0f;
    }

    return vec;
}

void CenterIvanOnLink(Actor* thisx, PlayState* play) {
    EnPartner* this = (EnPartner*)thisx;
    this->actor.world.pos = GET_PLAYER(play)->actor.world.pos;
    this->actor.world.pos.y += Player_GetHeight(GET_PLAYER(play)) + 5.0f;
}

static u8 magicArrowCosts[] = { 0, 4, 4, 8 };

void UseBow(Actor* thisx, PlayState* play, u8 started, u8 arrowType) {
    EnPartner* this = (EnPartner*)thisx;

    if (started == 1) {
        func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
        this->canMove = 0;
    } else if (started == 0) {
        if (this->itemTimer <= 0) {
            if (AMMO(ITEM_BOW) > 0) {
                if (arrowType >= 1 && !func_80087708(play, magicArrowCosts[arrowType], 0)) {
                    func_80078884(NA_SE_SY_ERROR);
                    this->canMove = 1;
                    return;
                }

                this->itemTimer = 10;

                Actor* newarrow = Actor_SpawnAsChild(
                    &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                    this->actor.world.pos.y + 7, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, ARROW_NORMAL);

                switch (arrowType) {
                    case 1:
                        newarrow->params = ARROW_FIRE;
                        break;
                    case 2:
                        newarrow->params = ARROW_ICE;
                        break;
                    case 3:
                        newarrow->params = ARROW_LIGHT;
                        break;
                }

                GET_PLAYER(play)->unk_A73 = 4;
                newarrow->parent = NULL;
                Inventory_ChangeAmmo(ITEM_BOW, -1);
            }
        }
    }
}

void UseSlingshot(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (started == 1) {
        func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
        this->canMove = 0;
    } else if (started == 0) {
        if (this->itemTimer <= 0) {
            if (AMMO(ITEM_SLINGSHOT) > 0) {
                this->itemTimer = 10;
                Actor* newarrow = Actor_SpawnAsChild(
                    &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                    this->actor.world.pos.y + 7, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, ARROW_SEED);
                GET_PLAYER(play)->unk_A73 = 4;
                newarrow->parent = NULL;
                Inventory_ChangeAmmo(ITEM_SLINGSHOT, -1);
            } else {
                func_80078884(NA_SE_SY_ERROR);
            }
        }
    }
}

void UseBombs(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            if (AMMO(ITEM_BOMB) > 0 && play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length < 3) {
                this->itemTimer = 10;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y + 7,
                            this->actor.world.pos.z, 0, 0, 0, 0, false);
                Inventory_ChangeAmmo(ITEM_BOMB, -1);
            } else {
                func_80078884(NA_SE_SY_ERROR);
            }
        }
    }
}

static Vec3f D_80854A40 = { 0.0f, 40.0f, 45.0f };

void UseHammer(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            this->itemTimer = 10;
            static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
            Vec3f shockwavePos = this->actor.world.pos;

            func_808429B4(play, 27767, 7, 20);
            Player_PlaySfx(&this->actor, NA_SE_IT_HAMMER_HIT);

            EffectSsBlast_SpawnWhiteShockwave(play, &shockwavePos, &zeroVec, &zeroVec);

            if (this->actor.xzDistToPlayer < 100.0f && this->actor.yDistToPlayer < 35.0f) {
                func_8002F71C(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            }
        }
    }
}

void UseBombchus(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            if (AMMO(ITEM_BOMBCHU) > 0) {
                this->itemTimer = 10;
                EnBom* bomb = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y + 7,
                            this->actor.world.pos.z, 0, 0, 0, 0, false);
                bomb->timer = 0;
                Inventory_ChangeAmmo(ITEM_BOMBCHU, -1);
            } else {
                func_80078884(NA_SE_SY_ERROR);
            }
        }
    }
}

static Vec3f D_808547A4 = { 0.0f, 0.5f, 0.0f };
static Vec3f D_808547B0 = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 D_808547BC = { 255, 255, 100, 255 };
static Color_RGBA8 D_808547C0 = { 255, 50, 0, 0 };

void UseDekuStick(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            if (AMMO(ITEM_STICK) > 0) {
                func_808328EC(this, NA_SE_EV_FLAME_IGNITION);
            } else {
                func_80078884(NA_SE_SY_ERROR);
            }
        }

        if (started == 2) {
            if (AMMO(ITEM_STICK) > 0) {
                this->stickWeaponInfo.tip = this->actor.world.pos;
                this->stickWeaponInfo.tip.y += 7.0f;

                func_8002836C(play, &this->stickWeaponInfo.tip, &D_808547A4, &D_808547B0, &D_808547BC, &D_808547C0,
                              200.0f, 0, 8);

                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);

                if (this->damageTimer <= 0) {
                    Inventory_ChangeAmmo(ITEM_STICK, -1);
                    this->damageTimer = 20;
                } else {
                    this->damageTimer--;
                }
            }
        }
    }
}

void UseNuts(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            if (AMMO(ITEM_NUT) > 0) {
                this->itemTimer = 10;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ARROW, this->actor.world.pos.x, this->actor.world.pos.y + 7,
                            this->actor.world.pos.z, 0x1000, this->actor.world.rot.y, 0, ARROW_NUT, false);
                Inventory_ChangeAmmo(ITEM_NUT, -1);
            } else {
                func_80078884(NA_SE_SY_ERROR);
            }
        }
    }
}

void UseHookshot(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
            this->canMove = 0;
            this->hookshotTarget =
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_OBJ_HSBLOCK, this->actor.world.pos.x,
                                   this->actor.world.pos.y + 7.5f, this->actor.world.pos.z, this->actor.world.rot.x,
                                   this->actor.world.rot.y, this->actor.world.rot.z, 2);
            this->hookshotTarget->scale.x = 0.05f;
            this->hookshotTarget->scale.y = 0.05f;
            this->hookshotTarget->scale.z = 0.05f;
        } else if (started == 0) {
            Actor_Kill(this->hookshotTarget);
            this->hookshotTarget = NULL;
            func_808328EC(this, NA_SE_PL_CHANGE_ARMS);
            this->canMove = 1;
        } else if (started == 2) {
            this->hookshotTarget->shape.rot.y = this->actor.world.rot.y;
        }
    }
}

void UseOcarina(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            Audio_PlaySoundTransposed(&this->actor.projectedPos, NA_SE_VO_NA_HELLO_2, -6);
        }
    }
}

void UseBoomerang(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            this->itemTimer = 20;
            spawn_boomerang_ivan(&this->actor, play);
        }
    }
}

void UseLens(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            func_80078884(NA_SE_SY_GLASSMODE_ON);
            this->shouldDraw = 0;
        }

        if (started == 0) {
            func_80078884(NA_SE_SY_GLASSMODE_OFF);
            this->shouldDraw = 1;
        }
    }
}

void UseBeans(Actor* thisx, PlayState* play, u8 started) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->itemTimer <= 0) {
        if (started == 1) {
            this->entry = ItemTable_Retrieve(GI_BEAN);
            if (play->actorCtx.titleCtx.alpha <= 0) {
                if (gSaveContext.rupees >= 100 && GiveItemEntryWithoutActor(play, this->entry)) {
                    Rupees_ChangeBy(-100);
                } else {
                    func_80078884(NA_SE_SY_ERROR);
                }
            }
        }
    }
}

void UseSpell(Actor* thisx, PlayState* play, u8 started, u8 spellType) {
    EnPartner* this = (EnPartner*)thisx;

    if (gSaveContext.magic > 0) {
        if (this->itemTimer <= 0 && this->usedSpell == 0) {
            if (started == 1) {
                this->usedSpell = spellType;
            }
        }

        if (started == 0 && this->usedSpell != 0) {
            this->itemTimer = 10;
            gSaveContext.magicState = 5;

            switch (this->usedSpell) {
                case 1:
                    GET_PLAYER(play)->ivanDamageMultiplier = 1;
                    break;
                case 3:
                    GET_PLAYER(play)->ivanFloating = 0;
                    break;
            }
            
            this->usedSpell = 0;
        }

        if (started == 2) {
            if (this->usedSpell != 0) {
                Vec3f spE4[3];
                Vec3f newBasePos[3];

                switch (this->usedSpell) {
                    case 1: //Din's
                        GET_PLAYER(play)->ivanDamageMultiplier = 2;
                        break;
                    case 2: //Nayru's
                        GET_PLAYER(play)->invincibilityTimer = -10;
                        break;
                    case 3: //Farore's
                        GET_PLAYER(play)->hoverBootsTimer = 10;
                        GET_PLAYER(play)->ivanFloating = 1;
                        break;
                }

                gSaveContext.magicState = 3;
                this->magicTimer--;
                if (this->magicTimer <= 0) {
                    gSaveContext.magic--;
                    this->magicTimer = 20;
                    if (gSaveContext.magic <= 0) {
                        gSaveContext.magic = 0;

                        this->itemTimer = 10;
                        this->usedSpell = 0;
                        gSaveContext.magicState = 5;
                    }
                }
            }
        }
    }
}

void UseItem(uint8_t usedItem, u8 started, Actor* thisx, PlayState* play) {
    EnPartner* this = (EnPartner*)thisx;

    if (this->usedItem != 0xFF && this->itemTimer <= 0) {
        switch (usedItem) {
            case SLOT_STICK:
                UseDekuStick(this, play, started);
                break;
            case SLOT_BOMB:
                UseBombs(this, play, started);
                break;
            case SLOT_BOMBCHU:
                UseBombchus(this, play, started);
                break;
            case SLOT_NUT:
                UseNuts(this, play, started);
                break;
            case SLOT_BOW:
                UseBow(this, play, started, 0);
                break;
            case SLOT_ARROW_FIRE:
                UseBow(this, play, started, 1);
                break;
            case SLOT_ARROW_ICE:
                UseBow(this, play, started, 2);
                break;
            case SLOT_ARROW_LIGHT:
                UseBow(this, play, started, 3);
                break;
            case SLOT_SLINGSHOT:
                UseSlingshot(this, play, started);
                break;
            case SLOT_OCARINA:
                UseOcarina(this, play, started);
                break;
            case SLOT_HOOKSHOT:
                UseHookshot(this, play, started);
                break;
            case SLOT_DINS_FIRE:
                UseSpell(this, play, started, 1);
                break;
            case SLOT_NAYRUS_LOVE:
                UseSpell(this, play, started, 2);
                break;
            case SLOT_FARORES_WIND:
                UseSpell(this, play, started, 3);
                break;
            case SLOT_HAMMER:
                UseHammer(this, play, started);
                break;
            case SLOT_BOOMERANG:
                UseBoomerang(this, play, started);
                break;
            case SLOT_LENS:
                UseLens(this, play, started);
                break;
            case SLOT_BEAN:
                UseBeans(this, play, started);
                break;
        }
    }

    if (started == 0) {
        this->usedItem = 0xFF;
    }
}

void EnPartner_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPartner* this = (EnPartner*)thisx;

    Input sControlInput = play->state.input[this->actor.params];

    f32 relX = sControlInput.cur.stick_x / 10.0f * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1);
    f32 relY = sControlInput.cur.stick_y / 10.0f;

    Vec3f camForward = { GET_ACTIVE_CAM(play)->at.x - GET_ACTIVE_CAM(play)->eye.x, 0.0f,
                         GET_ACTIVE_CAM(play)->at.z - GET_ACTIVE_CAM(play)->eye.z };
    camForward = Vec3fNormalize(camForward);

    Vec3f camRight = { -camForward.z, 0.0f, camForward.x };

    this->actor.velocity.x = 0;
    this->actor.velocity.y = 0;
    this->actor.velocity.z = 0;

    this->actor.velocity.x += camRight.x * relX;
    this->actor.velocity.z += camRight.z * relX;
    this->actor.velocity.x += camForward.x * relY;
    this->actor.velocity.z += camForward.z * relY;

    if (this->actor.velocity.x != 0 || this->actor.velocity.z != 0) {
        int16_t finalDir = Math_Atan2S(-this->actor.velocity.x, this->actor.velocity.z) - 0x4000;
        Math_SmoothStepToS(&this->actor.world.rot.y, finalDir, 2, 10000, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.y, finalDir, 2, 10000, 0);
    }

    if (!this->canMove) {
        relX = 0;
        relY = 0;
    }

    Math_SmoothStepToF(&this->actor.speedXZ, sqrtf(SQ(relX) + SQ(relY)), 1.0f, 1.3f, 0.0f);

    if (this->shouldDraw == 1) {
        thisx->shape.shadowAlpha = 0xFF;
        EnPartner_SpawnSparkles(this, play, 12);
    } else {
        thisx->shape.shadowAlpha = 0;
    }

    if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_A) && this->canMove) {
        Math_SmoothStepToF(&this->yVelocity, 6.0f, 1.0f, 1.5f, 0.0f);
    } else if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_B) && this->canMove) {
        Math_SmoothStepToF(&this->yVelocity, -6.0f, 1.0f, 1.5f, 0.0f);
    } else {
        Math_SmoothStepToF(&this->yVelocity, 0.0f, 1.0f, 1.5f, 0.0f);
    }

    this->actor.gravity = this->yVelocity;

    if (this->canMove == 1) {
        Actor_MoveForward(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 19.0f, 20.0f, 0.0f, 5);
    }

    if (this->usedSpell != 0) {
        func_8002F974(thisx, NA_SE_PL_MAGIC_SOUL_NORMAL - SFX_FLAG);
    }

    if (!Player_InCsMode(play)) {
        // Collect drops & rupees
        Actor* itemActor = play->actorCtx.actorLists[ACTORCAT_MISC].head;
        while (itemActor != NULL) {
            if (itemActor->id == ACTOR_EN_ITEM00) {
                if (itemActor->params == ITEM00_RUPEE_GREEN || itemActor->params == ITEM00_RUPEE_BLUE ||
                    itemActor->params == ITEM00_RUPEE_RED || itemActor->params == ITEM00_RUPEE_PURPLE ||
                    itemActor->params == ITEM00_RUPEE_ORANGE || itemActor->params == ITEM00_HEART ||
                    itemActor->params == ITEM00_BOMBS_A || itemActor->params == ITEM00_BOMBS_B ||
                    itemActor->params == ITEM00_ARROWS_SINGLE || itemActor->params == ITEM00_ARROWS_SMALL ||
                    itemActor->params == ITEM00_ARROWS_MEDIUM || itemActor->params == ITEM00_ARROWS_LARGE ||
                    itemActor->params == ITEM00_BOMBCHU || itemActor->params == ITEM00_MAGIC_SMALL ||
                    itemActor->params == ITEM00_MAGIC_LARGE || itemActor->params == ITEM00_NUTS ||
                    itemActor->params == ITEM00_STICK) {
                    f32 distanceToObject = Actor_WorldDistXYZToActor(&this->actor, itemActor);
                    if (distanceToObject <= 20.0f) {
                        itemActor->world.pos = GET_PLAYER(play)->actor.world.pos;
                        break;
                    }
                }
            }
            itemActor = itemActor->next;
        }

        itemActor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
        while (itemActor != NULL) {
            if (itemActor->id == ACTOR_EN_SI) {
                f32 distanceToObject = Actor_WorldDistXYZToActor(&this->actor, itemActor);
                if (distanceToObject <= 20.0f) {
                    EnSi* ensi = (EnSi*)itemActor;
                    ensi->collider.base.ocFlags2 = OC2_HIT_PLAYER;
                    break;
                }
            }
            itemActor = itemActor->next;
        }
    }

    if (this->itemTimer > 0) {
        this->itemTimer--;
        if (this->itemTimer <= 0) {
            this->canMove = 1;
        }
    }

    if (!Player_InCsMode(play)) {
        uint8_t pressed = 0;
        uint8_t released = 0;
        uint8_t current = 0;

        if (this->usedItem == 0xFF && this->itemTimer <= 0) {
            if (CHECK_BTN_ALL(sControlInput.press.button, BTN_CLEFT)) {
                this->usedItem = gSaveContext.equips.cButtonSlots[0];
                this->usedItemButton = 0;
                pressed = 1;
            } else if (CHECK_BTN_ALL(sControlInput.press.button, BTN_CDOWN)) {
                this->usedItem = gSaveContext.equips.cButtonSlots[1];
                this->usedItemButton = 1;
                pressed = 1;
            } else if (CHECK_BTN_ALL(sControlInput.press.button, BTN_CRIGHT)) {
                this->usedItem = gSaveContext.equips.cButtonSlots[2];
                this->usedItemButton = 2;
                pressed = 1;
            }
        }

        if (this->usedItem != 0xFF) {
            if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_CLEFT) && this->usedItemButton == 0) {
                current = 1;
            } else if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_CDOWN) && this->usedItemButton == 1) {
                current = 1;
            } else if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_CRIGHT) && this->usedItemButton == 2) {
                current = 1;
            }
        }

        if (this->usedItem != 0xFF) {
            if (CHECK_BTN_ALL(sControlInput.rel.button, BTN_CLEFT) && this->usedItemButton == 0) {
                released = 1;
            } else if (CHECK_BTN_ALL(sControlInput.rel.button, BTN_CDOWN) && this->usedItemButton == 1) {
                released = 1;
            } else if (CHECK_BTN_ALL(sControlInput.rel.button, BTN_CRIGHT) && this->usedItemButton == 2) {
                released = 1;
            }
        }

        if (pressed == 1) {
            UseItem(this->usedItem, 1, this, play);
        } else if (released == 1) {
            UseItem(this->usedItem, 0, this, play);
            this->usedItemButton = 0xFF;
        } else if (current == 1) {
            UseItem(this->usedItem, 2, this, play);
        }
    } else {
        UseItem(this->usedItem, 0, this, play);
        this->usedItem = 0xFF;
        this->itemTimer = 10;
    }

    if (CHECK_BTN_ALL(sControlInput.press.button, BTN_Z) && this->canMove) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_FAIRY_DASH);
    }

    if (CHECK_BTN_ALL(sControlInput.cur.button, BTN_Z) && this->canMove) {
        CenterIvanOnLink(this, play);
    } else if (this->canMove == 1 && this->hookshotTarget == NULL) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    SkelAnime_Update(&this->skelAnime);

    EnPartner_UpdateLights(this, play);
}

s32 EnPartner_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    f32 scale;
    Vec3f mtxMult;
    EnPartner* this = (EnPartner*)thisx;

    if (limbIndex == 8) {
        scale = ((Math_SinS(4096) * 0.1f) + 1.0f) * 0.012f;
        scale *= (this->actor.scale.x * 124.99999f);
        Matrix_MultVec3f(&zeroVec, &mtxMult);
        Matrix_Translate(mtxMult.x, mtxMult.y, mtxMult.z, MTXMODE_NEW);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    }

    return false;
}

void DrawOrb(Actor* thisx, PlayState* play, u8 color) {
    EnPartner* this = (EnPartner*)thisx;
    Vec3f pos;
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 sp6C = play->state.frames & 0x1F;

    pos = this->actor.world.pos;
    pos.y += 5.0f;

    pos.x -= (this->actor.scale.x * 300.0f * Math_SinS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) *
              Math_CosS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));
    pos.y -= (this->actor.scale.x * 300.0f * Math_SinS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));
    pos.z -= (this->actor.scale.x * 300.0f * Math_CosS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play))) *
              Math_CosS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);

    switch (color) {
        case 1:
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
            break;
        case 2:
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);
            break;
        case 3:
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 0, 255);
            break;
    }

    Matrix_Translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x * 3.0f, this->actor.scale.y * 3.0f, this->actor.scale.z * 3.0f, MTXMODE_APPLY);
    Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_Push();
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    Matrix_RotateZ(sp6C * (M_PI / 32), MTXMODE_APPLY);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    Matrix_Pop();
    Matrix_RotateZ(-sp6C * (M_PI / 32), MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}


void EnPartner_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    f32 alphaScale;
    s32 envAlpha;
    EnPartner* this = (EnPartner*)thisx;
    s32 pad1;
    Gfx* dListHead;
    Player* player = GET_PLAYER(play);

    if (play->pauseCtx.state != 0 && this->usedItem != 0xFF) {
        UseItem(this->usedItem, 0, this, play);
        this->usedItem = 0xFF;
    }

    if (this->shouldDraw == 0) {
        return;
    }

    dListHead = Graph_Alloc(play->state.gfxCtx, sizeof(Gfx) * 4);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_27Xlu(play->state.gfxCtx);

    envAlpha = (50) & 0x1FF;
    envAlpha = (envAlpha > 255) ? 511 - envAlpha : envAlpha;

    alphaScale = 1.0f;

    gSPSegment(POLY_XLU_DISP++, 0x08, dListHead);
    gDPPipeSync(dListHead++);
    gDPSetPrimColor(dListHead++, 0, 0x01, (u8)this->innerColor.r, (u8)this->innerColor.g, (u8)this->innerColor.b,
                    (u8)(this->innerColor.a * alphaScale));

    gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_ZB_CLD_SURF2);

    gSPEndDisplayList(dListHead++);
    gDPSetEnvColor(POLY_XLU_DISP++, (u8)this->outerColor.r, (u8)this->outerColor.g, (u8)this->outerColor.b,
                   (u8)(envAlpha * alphaScale));
    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   EnPartner_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->usedSpell > 0) {
        DrawOrb(this, play, this->usedSpell);
    }
}

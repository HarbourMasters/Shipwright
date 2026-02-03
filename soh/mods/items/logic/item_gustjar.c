/**
 * item_gustjar.c - Gust Jar from Minish Cap
 *
 * Controls:
 *   Hold C Button:  Suction mode - pulls in items, enemies, projectiles
 *   Release C:      Fire sucked object as projectile
 *   Tap C Button:   Quick wind gust push
 *
 * Features:
 *   - First-person aiming mode (toggles with Z-targeting)
 *   - Suction pulls collectibles and small enemies
 *   - Captured objects become ammo for projectile attack
 *   - Visual feedback: jar decoration changes color when active
 */

#include "z64.h"
#include "item_gustjar.h"
#include "../custom_items.h"
#include "../helpers/camera_helper.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"
void Player_InitGustJarIA(PlayState* play, Player* this) {
    Collider_InitCylinder(play, &gCustomItemState.gustJarCollider);
    Collider_SetCylinder(play, &gCustomItemState.gustJarCollider, &this->actor, &sGustJarColliderInit);
}
extern void Item_DropCollectibleRandom(PlayState* play, Actor* fromActor, Vec3f* spawnPos, s16 params);
extern Actor* Actor_SpawnAsChild(ActorContext* actorCtx, Actor* parent, PlayState* play, s16 actorId, f32 x, f32 y,
                                 f32 z, s16 rotX, s16 rotY, s16 rotZ, s16 params);
static Color_RGBA8 sGustDustColor = { 170, 170, 170, 255 };
static Color_RGBA8 sShockColor = { 255, 255, 100, 255 };
static Color_RGBA8 sShockEnvColor = { 255, 255, 255, 255 };

// Note: sGustJarTable is defined in item_gustjar.h for easy editing

static void GustJar_ClearScaleCache(void);
static void GustJar_Equip(PlayState* play, Player* player) {
    if (gCustomItemState.gustJarEquipped)
        return;
    gCustomItemState.gustJarEquipped = 1;
    gCustomItemState.gustJarMode = 1;
    if (Player_IsZTargeting(player)) {
        gCustomItemState.gustJarAimMode = 1;
        gCustomItemState.gustJarFirstPersonActive = 0;
    } else {
        gCustomItemState.gustJarAimMode = 0;
        FirstPerson_Init(player, play);
        gCustomItemState.gustJarFirstPersonActive = 1;
    }
    ItemEquip_PlayEquipSFX(play, player);
}
static void GustJar_Unequip(PlayState* play, Player* player) {
    if (!gCustomItemState.gustJarEquipped)
        return;
    if (gCustomItemState.gustJarFirstPersonActive) {
        FirstPerson_Exit(player, play);
        gCustomItemState.gustJarFirstPersonActive = 0;
    }
    GustJar_ClearScaleCache();
    gCustomItemState.gustJarEquipped = 0;
    gCustomItemState.gustJarMode = 0;
    gCustomItemState.gustJarAimMode = 0;
    gCustomItemState.gustJarProjectileActive = 0;
    gCustomItemState.gustJarAmmoType = 0;
    gCustomItemState.gustJarButtonMask = 0;
    ItemEquip_PlayUnequipSFX(play, player);
}
static s16 GustJar_GetAimAngle(PlayState* play, Player* player) {
    switch (gCustomItemState.gustJarAimMode) {
        case 0:
            if (gCustomItemState.gustJarFirstPersonActive) {
                return FirstPerson_GetAimYaw(player);
            }
            return player->actor.shape.rot.y;
        case 1:
            if (Player_IsZTargeting(player) && player->focusActor != NULL) {
                return Math_Vec3f_Yaw(&player->actor.world.pos, &player->focusActor->focus.pos);
            }
            return player->actor.shape.rot.y;
        case 2:
        default:
            return player->actor.shape.rot.y;
    }
}
static void GustJar_SaveOriginalScale(Actor* actor) {
    for (u8 i = 0; i < gCustomItemState.gustJarScaleCacheCount; i++) {
        if (gCustomItemState.gustJarScaleCache[i].actor == actor) {
            return;
        }
    }
    if (gCustomItemState.gustJarScaleCacheCount < 16) {
        u8 idx = gCustomItemState.gustJarScaleCacheCount;
        gCustomItemState.gustJarScaleCache[idx].actor = actor;
        gCustomItemState.gustJarScaleCache[idx].originalScale = actor->scale;
        gCustomItemState.gustJarScaleCacheCount++;
    }
}
static Vec3f* GustJar_GetOriginalScale(Actor* actor) {
    for (u8 i = 0; i < gCustomItemState.gustJarScaleCacheCount; i++) {
        if (gCustomItemState.gustJarScaleCache[i].actor == actor) {
            return &gCustomItemState.gustJarScaleCache[i].originalScale;
        }
    }
    return NULL;
}
static void GustJar_RestoreAndRemoveScale(Actor* actor) {
    for (u8 i = 0; i < gCustomItemState.gustJarScaleCacheCount; i++) {
        if (gCustomItemState.gustJarScaleCache[i].actor == actor) {
            actor->scale = gCustomItemState.gustJarScaleCache[i].originalScale;
            for (u8 j = i; j < gCustomItemState.gustJarScaleCacheCount - 1; j++) {
                gCustomItemState.gustJarScaleCache[j] = gCustomItemState.gustJarScaleCache[j + 1];
            }
            gCustomItemState.gustJarScaleCacheCount--;
            return;
        }
    }
}
static void GustJar_ClearScaleCache(void) {
    for (u8 i = 0; i < gCustomItemState.gustJarScaleCacheCount; i++) {
        Actor* actor = gCustomItemState.gustJarScaleCache[i].actor;
        if (actor != NULL && actor->update != NULL) {
            actor->scale = gCustomItemState.gustJarScaleCache[i].originalScale;
        }
    }
    gCustomItemState.gustJarScaleCacheCount = 0;
}
static u8 GustJar_GetAmmoType(Actor* actor) {
    for (size_t i = 0; i < GUSTJAR_TABLE_COUNT; i++) {
        if (actor->id == sGustJarTable[i].actorId) {
            if (sGustJarTable[i].params == -1 || sGustJarTable[i].params == actor->params) {
                return sGustJarTable[i].ammoType;
            }
        }
    }
    // Fallback: small enemies can be sucked as physical ammo
    if (actor->category == ACTORCAT_ENEMY && actor->colChkInfo.health > 0 && actor->colChkInfo.health <= 4) {
        return GUST_AMMO_PHYSICAL;
    }
    return GUST_AMMO_NONE;
}
static void GustJar_HandleCaptureDrop(PlayState* play, Player* link, Actor* enemy) {
    Vec3f* originalScale = GustJar_GetOriginalScale(enemy);
    if (originalScale != NULL) {
        enemy->scale = *originalScale;
    }
    Vec3f dropPos = enemy->world.pos;
    dropPos.y = enemy->floorHeight + 10.0f;
    if (enemy->id == ACTOR_EN_SW) {
        Actor_SpawnAsChild(&play->actorCtx, enemy, play, ACTOR_EN_SI, dropPos.x, dropPos.y, dropPos.z, 0, 0, 0,
                           enemy->params);
        Audio_PlaySoundGeneral(NA_SE_SY_KINSTA_MARK_APPEAR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                               &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    } else if (enemy->category == ACTORCAT_ENEMY || enemy->category == ACTORCAT_PROP) {
        s16 dropTable = 0x40;
        switch (enemy->id) {
            case 0x0002:
                dropTable = 0x00;
                break;
            case 0x001B:
                dropTable = 0x40;
                break;
            case 0x0027:
                dropTable = 0x10;
                break;
            case 0x0043:
                dropTable = 0x40;
                break;
            case 0x0095:
                break;
            case 0x0003:
                dropTable = 0x20;
                break;
            default:
                dropTable = 0x40;
                break;
        }
        Item_DropCollectibleRandom(play, enemy, &dropPos, dropTable);
    }
    GustJar_RestoreAndRemoveScale(enemy);
}
static void GustJar_SpawnSuctionFX(PlayState* play, Vec3f* origin, s16 yaw, s16 pitch) {
    if (play->gameplayFrames % 2 != 0) return;
    FX_SpawnSuction(play, origin, yaw, pitch);
}
void CustomItems_DrawGustJar(Player* this, PlayState* play) {
    GustJarPot_Draw(this, play);
    if (!gCustomItemState.gustJarProjectileActive)
        return;
    u8 type = gCustomItemState.gustJarAmmoType;
    Vec3f* pos = &gCustomItemState.gustJarProjPos;
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    if (type == GUST_AMMO_FIRE) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 50, 50, 230);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 200);
    } else if (type == GUST_AMMO_ICE) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 80, 180, 255, 230);
        gDPSetEnvColor(POLY_XLU_DISP++, 150, 220, 255, 200);
    } else if (type == GUST_AMMO_SHOCK) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 100, 230);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 150, 200);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 160, 160, 160, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 160);
    }
    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);
    Matrix_Scale(1.5f, 1.5f, 1.5f, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(play->state.gfxCtx, __FILE__, __LINE__),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffBubbleDL);
    CLOSE_DISPS(play->state.gfxCtx);
}
void Handle_GustJar(Player* this, PlayState* play) {
    if (gCustomItemState.gustJarCollider.base.shape != COLSHAPE_CYLINDER) {
        Player_InitGustJarIA(play, this);
    }
    ItemInputState input;
    static s8 prevInvincibility = 0;
    ItemInput_Update(&input, ITEM_GUST_JAR, this, play);
    if (!input.wasEquipped) {
        if (gCustomItemState.gustJarEquipped)
            GustJar_Unequip(play, this);
        return;
    }
    gCustomItemState.gustJarButtonMask = input.equippedButton;
    if (ItemInput_CheckDamage(this, &prevInvincibility)) {
        GustJar_Unequip(play, this);
        return;
    }
    u8 btnPressed = input.isPressed;
    u8 btnHeld = input.isHeld;
    if (!gCustomItemState.gustJarEquipped) {
        if (btnPressed) {
            GustJar_Equip(play, this);
        }
        return;
    }
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        if (gCustomItemState.gustJarAimMode == 2) {
            if (Player_IsZTargeting(this)) {
                gCustomItemState.gustJarAimMode = 1;
            } else {
                gCustomItemState.gustJarAimMode = 0;
                FirstPerson_Init(this, play);
                gCustomItemState.gustJarFirstPersonActive = 1;
            }
        } else {
            if (gCustomItemState.gustJarFirstPersonActive) {
                FirstPerson_Exit(this, play);
                gCustomItemState.gustJarFirstPersonActive = 0;
            }
            gCustomItemState.gustJarAimMode = 2;
        }
        ItemEquip_PlayEquipSFX(play, this);
        return;
    }
    if (input.otherButtonPressed) {
        GustJar_Unequip(play, this);
        return;
    }
    u8 isZTargeting = Player_IsZTargeting(this);
    if (gCustomItemState.gustJarAimMode == 0 && isZTargeting) {
        FirstPerson_Exit(this, play);
        gCustomItemState.gustJarFirstPersonActive = 0;
        gCustomItemState.gustJarAimMode = 1;
    } else if (gCustomItemState.gustJarAimMode == 1 && !isZTargeting) {
        FirstPerson_Init(this, play);
        gCustomItemState.gustJarFirstPersonActive = 1;
        gCustomItemState.gustJarAimMode = 0;
    }
    if (gCustomItemState.gustJarAimMode == 0 && gCustomItemState.gustJarFirstPersonActive) {
        FirstPerson_Update(this, play);
    }
    s16 aimYaw = GustJar_GetAimAngle(play, this);
    s16 aimPitch = gCustomItemState.gustJarFirstPersonActive ? FirstPerson_GetAimPitch(this) : 0;
    Vec3f nozzle = this->actor.world.pos;
    nozzle.y += 25.0f;
    f32 horizontalDist = 35.0f * Math_CosS(aimPitch);
    nozzle.x += Math_SinS(aimYaw) * horizontalDist;
    nozzle.z += Math_CosS(aimYaw) * horizontalDist;
    nozzle.y -= Math_SinS(aimPitch) * 35.0f;
    if (!gCustomItemState.gustJarProjectileActive) {
        gCustomItemState.gustJarProjPos = nozzle;
    }
    if (gCustomItemState.gustJarProjectileActive) {
        gCustomItemState.gustJarTimer--;
        f32 speed = 30.0f;

        // Homing: if Z-targeting an actor, track it dynamically
        if (Player_IsZTargeting(this) && this->focusActor != NULL && this->focusActor->update != NULL) {
            Vec3f* targetPos = &this->focusActor->focus.pos;
            s16 targetYaw = Math_Vec3f_Yaw(&gCustomItemState.gustJarProjPos, targetPos);
            f32 dx = targetPos->x - gCustomItemState.gustJarProjPos.x;
            f32 dy = targetPos->y - gCustomItemState.gustJarProjPos.y;
            f32 dz = targetPos->z - gCustomItemState.gustJarProjPos.z;
            f32 distXZ = sqrtf(SQ(dx) + SQ(dz));
            s16 targetPitch = Math_Atan2S(dy, distXZ);

            // Smooth homing - turn towards target
            Math_SmoothStepToS(&gCustomItemState.gustJarProjYaw, targetYaw, 3, 0x1500, 0x100);
            Math_SmoothStepToS(&gCustomItemState.gustJarProjPitch, targetPitch, 3, 0x800, 0x100);
        }

        f32 speedXZ = Math_CosS(gCustomItemState.gustJarProjPitch) * speed;
        gCustomItemState.gustJarProjPos.x += Math_SinS(gCustomItemState.gustJarProjYaw) * speedXZ;
        gCustomItemState.gustJarProjPos.z += Math_CosS(gCustomItemState.gustJarProjYaw) * speedXZ;
        gCustomItemState.gustJarProjPos.y -= Math_SinS(gCustomItemState.gustJarProjPitch) * speed;
        u8 type = gCustomItemState.gustJarAmmoType;
        // Spawn trail effects using FX helper
        FX_Type fxType = FX_DUST;
        if (type == GUST_AMMO_FIRE) fxType = FX_FIRE;
        else if (type == GUST_AMMO_ICE) fxType = FX_ICE;
        else if (type == GUST_AMMO_SHOCK) fxType = FX_SHOCK;
        FX_SpawnProjectileTrail(play, &gCustomItemState.gustJarProjPos, fxType);
        ColliderCylinder* collider = &gCustomItemState.gustJarCollider;
        collider->dim.pos.x = (s16)gCustomItemState.gustJarProjPos.x;
        collider->dim.pos.y = (s16)gCustomItemState.gustJarProjPos.y;
        collider->dim.pos.z = (s16)gCustomItemState.gustJarProjPos.z;
        u32 dmgFlags = 0;
        u8 damage = 2;
        u8 effect = 0;
        if (type == GUST_AMMO_FIRE) {
            dmgFlags = DMG_GU_FIRE;
            effect = 1;
        } else if (type == GUST_AMMO_ICE) {
            dmgFlags = DMG_GU_ICE;
            effect = 3;
        } else if (type == GUST_AMMO_SHOCK) {
            dmgFlags = DMG_GU_SHOCK;
            effect = 2;
        } else {
            dmgFlags = DMG_GU_PHYSICAL;
            effect = 0;
        }
        collider->info.toucher.dmgFlags = dmgFlags;
        collider->info.toucher.damage = damage;
        collider->info.toucher.effect = effect;
        collider->base.atFlags |= AT_ON | AT_TYPE_PLAYER;
        CollisionCheck_SetAT(play, &play->colChkCtx, &collider->base);
        if (collider->base.atFlags & AT_HIT) {
            Audio_PlaySoundGeneral(NA_SE_IT_EXPLOSION_FRAME, &gCustomItemState.gustJarProjPos, 4,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            if (type == GUST_AMMO_SHOCK && collider->base.at != NULL && collider->base.at->update != NULL) {
                Actor* hitActor = collider->base.at;
                if (hitActor->category == ACTORCAT_ENEMY) {
                    hitActor->freezeTimer = 40;
                    Actor_SetColorFilter(hitActor, 0, 255, 0x2000, 40);
                    Audio_PlayActorSound2(hitActor, NA_SE_EN_GOMA_JR_FREEZE);
                }
            }
            gCustomItemState.gustJarProjectileActive = 0;
            gCustomItemState.gustJarAmmoType = 0;
            gCustomItemState.gustJarMode = 0;
        }
        if (gCustomItemState.gustJarTimer <= 0) {
            gCustomItemState.gustJarProjectileActive = 0;
            gCustomItemState.gustJarAmmoType = 0;
            gCustomItemState.gustJarMode = 0;
        }
        return;
    }
    if (gCustomItemState.gustJarMode == 3 && gCustomItemState.gustJarAmmoType != GUST_AMMO_NONE) {
        if (play->gameplayFrames % 5 == 0) {
            Vec3f sPos = nozzle;
            sPos.x += Rand_CenteredFloat(6.0f);
            sPos.z += Rand_CenteredFloat(6.0f);
            Vec3f sVel = { 0, 1.5f, 0 };
            Vec3f sAcc = { 0, 0.1f, 0 };
            Color_RGBA8 col = { 150, 150, 150, 255 };
            if (gCustomItemState.gustJarAmmoType == GUST_AMMO_FIRE) {
                col.r = 255;
                col.g = 50;
                col.b = 0;
            } else if (gCustomItemState.gustJarAmmoType == GUST_AMMO_ICE) {
                col.r = 0;
                col.g = 200;
                col.b = 255;
            } else if (gCustomItemState.gustJarAmmoType == GUST_AMMO_SHOCK) {
                col.r = 255;
                col.g = 255;
                col.b = 0;
            }
            func_8002836C(play, &sPos, &sVel, &sAcc, &col, &col, 40, 10, 8);
        }
        if (btnPressed) {
            gCustomItemState.gustJarProjectileActive = 1;
            gCustomItemState.gustJarProjPos = nozzle;
            gCustomItemState.gustJarProjYaw = aimYaw;
            gCustomItemState.gustJarProjPitch = aimPitch;
            gCustomItemState.gustJarTimer = 50;
            gCustomItemState.gustJarMode = 1;
            Player_PlaySfx(this, NA_SE_PL_MAGIC_WIND_NORMAL);
            if (!gCustomItemState.gustJarFirstPersonActive) {
                FirstPerson_Init(this, play);
                gCustomItemState.gustJarFirstPersonActive = 1;
            }
        }
        return;
    }
    static u8 wasSuctionActive = 0;
    u8 isSuctionActive = btnHeld && !btnPressed;
    if (wasSuctionActive && !btnHeld) {
        GustJar_ClearScaleCache();
        if (gCustomItemState.gustJarMode == 2) {
            gCustomItemState.gustJarMode = 1;
        }
        wasSuctionActive = 0;
        return;
    }
    if (isSuctionActive) {
        if (gCustomItemState.gustJarMode == 1) {
            gCustomItemState.gustJarMode = 2;
        }
        if (gSaveContext.magic <= 0) {
            gCustomItemState.gustJarMode = 1;
            wasSuctionActive = 0;
            return;
        }
        if (play->gameplayFrames % 10 == 0) {
            ItemMagic_Consume(play, 1);
        }
        func_8002F974(&this->actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);
        GustJar_SpawnSuctionFX(play, &nozzle, aimYaw, aimPitch);
        wasSuctionActive = 1;
        // Use squared distance for initial check to avoid sqrtf
        f32 rangeSq = SQ(GUST_RANGE_MAX);
        ActorCategory categories[] = { ACTORCAT_ENEMY, ACTORCAT_PROP };
        for (int c = 0; c < 2; c++) {
            Actor* actor = play->actorCtx.actorLists[categories[c]].head;
            while (actor != NULL) {
                if (actor->update != NULL) {
                    // Quick squared distance check first
                    f32 dx = nozzle.x - actor->world.pos.x;
                    f32 dz = nozzle.z - actor->world.pos.z;
                    f32 distXZSq = SQ(dx) + SQ(dz);
                    if (distXZSq > rangeSq) {
                        actor = actor->next;
                        continue;
                    }
                    // Calculate full distance only if passed initial check
                    f32 dist = sqrtf(distXZSq + SQ(nozzle.y - actor->world.pos.y));
                    f32 dy = fabsf(nozzle.y - actor->world.pos.y);
                    if (dist < GUST_RANGE_MAX && dy < LINK_HEIGHT_HITBOX) {
                        u8 ammo = GustJar_GetAmmoType(actor);
                        if (ammo != GUST_AMMO_NONE) {
                            if (ammo == GUST_AMMO_BREAK && dist < GUST_RANGE_SHRINK) {
                                Vec3f breakPos = actor->world.pos;
                                Vec3f dustVel = { 0, 2.0f, 0 };
                                Vec3f dustAccel = { 0, -0.5f, 0 };
                                Color_RGBA8 dustCol = { 180, 150, 120, 255 };
                                // Spawn 4 dust particles
                                for (int i = 0; i < 4; i++) {
                                    Vec3f particlePos = breakPos;
                                    particlePos.x += Rand_CenteredFloat(15.0f);
                                    particlePos.y += Rand_CenteredFloat(10.0f);
                                    particlePos.z += Rand_CenteredFloat(15.0f);
                                    Vec3f particleVel = dustVel;
                                    particleVel.x += Rand_CenteredFloat(8.0f);
                                    particleVel.y += Rand_ZeroOne() * 4.0f;
                                    particleVel.z += Rand_CenteredFloat(8.0f);
                                    func_8002836C(play, &particlePos, &particleVel, &dustAccel, &dustCol, &dustCol, 200,
                                                  40, 15);
                                }
                                Audio_PlaySoundGeneral(NA_SE_EV_POT_BROKEN, &breakPos, 4, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                                if (actor->id == ACTOR_OBJ_TSUBO) {
                                    if (Rand_ZeroOne() > 0.5f) {
                                        Vec3f dropPos = breakPos;
                                        dropPos.y = actor->floorHeight + 10.0f;
                                        s16 dropType = (Rand_ZeroOne() > 0.5f) ? ITEM00_RUPEE_GREEN : ITEM00_HEART;
                                        Item_DropCollectible(play, &dropPos, dropType);
                                    }
                                } else if (actor->id == ACTOR_EN_KUSA) {
                                    if (Rand_ZeroOne() > 0.7f) {
                                        Vec3f dropPos = breakPos;
                                        dropPos.y = actor->floorHeight + 10.0f;
                                        Item_DropCollectible(play, &dropPos, ITEM00_RUPEE_GREEN);
                                    }
                                }
                                Actor_Kill(actor);
                                actor = actor->next;
                                continue;
                            }
                            if (dist < GUST_RANGE_CAPTURE) {
                                gCustomItemState.gustJarMode = 3;
                                gCustomItemState.gustJarAmmoType = ammo;
                                Audio_PlaySoundGeneral(NA_SE_SY_METRONOME, &nozzle, 4, &gSfxDefaultFreqAndVolScale,
                                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                                GustJar_HandleCaptureDrop(play, this, actor);
                                Actor_Kill(actor);
                                if (!gCustomItemState.gustJarFirstPersonActive) {
                                    FirstPerson_Init(this, play);
                                    gCustomItemState.gustJarFirstPersonActive = 1;
                                }
                                return;
                            }
                            // Reuse dx/dz already calculated for suction force
                            f32 strength = 8.0f;
                            f32 norm = sqrtf(distXZSq);
                            if (norm > 0.1f) {
                                f32 invNorm = strength / norm;
                                actor->world.pos.x += dx * invNorm;
                                actor->world.pos.z += dz * invNorm;
                                // Lift actor slightly if on ground
                                if (actor->bgCheckFlags & BGCHECKFLAG_GROUND)
                                    actor->world.pos.y += 2.0f;
                            }
                            if (dist < GUST_RANGE_SHRINK) {
                                GustJar_SaveOriginalScale(actor);
                                Vec3f* origScale = GustJar_GetOriginalScale(actor);
                                if (origScale != NULL) {
                                    f32 scaleFactor =
                                        (dist - GUST_RANGE_CAPTURE) / (GUST_RANGE_SHRINK - GUST_RANGE_CAPTURE);
                                    if (scaleFactor < 0.2f)
                                        scaleFactor = 0.2f;
                                    if (scaleFactor > 1.0f)
                                        scaleFactor = 1.0f;
                                    actor->scale.x = origScale->x * scaleFactor;
                                    actor->scale.y = origScale->y * scaleFactor;
                                    actor->scale.z = origScale->z * scaleFactor;
                                }
                            }
                        }
                    }
                }
                actor = actor->next;
            }
        }
    }
}

#include "2s2h/ObjectExtension/ObjectExtension.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "overlays/actors/ovl_En_Fz/z_en_fz.h"
#include "overlays/actors/ovl_En_M_Thunder/z_en_m_thunder.h"
#include "overlays/actors/ovl_En_Neo_Reeba/z_en_neo_reeba.h"
}

#define CVAR_NAME "gEnhancements.Masks.FierceDeitysAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

struct SwordBeamCollision {
    bool hitBySwordBeam = false;
};
static ObjectExtension::Register<SwordBeamCollision> SwordBeamCollisionRegister;

static bool GetActorHitBySwordBeam(Actor* actor) {
    const SwordBeamCollision* collision = ObjectExtension::GetInstance().Get<SwordBeamCollision>(actor);
    return collision != nullptr ? collision->hitBySwordBeam : SwordBeamCollision{}.hitBySwordBeam;
}

static void SetActorHitBySwordBeam(const Actor* actor, bool hitBySwordBeam) {
    ObjectExtension::GetInstance().Set<SwordBeamCollision>(actor, SwordBeamCollision{ hitBySwordBeam });
}

static void AllowTransformationMask(ItemId* itemId, bool* should) {
    Player* player = GET_PLAYER(gPlayState);
    if ((player == NULL) || (player->transformation != PLAYER_FORM_FIERCE_DEITY)) {
        return;
    }

    if (*itemId >= ITEM_MASK_DEKU && *itemId <= ITEM_MASK_ZORA) {
        *should = false;
    }
}

static void RegisterFierceDeityAnywhere() {
    COND_VB_SHOULD(VB_DISABLE_FD_MASK, CVAR, { *should = false; });

    COND_VB_SHOULD(VB_DAMAGE_MULTIPLIER, CVAR, {
        int index = va_arg(args, int);
        DamageTable* damageTable = va_arg(args, DamageTable*);
        f32* damage = va_arg(args, f32*);
        f32* damageMultipliers = va_arg(args, f32*);

        /*
         * 25 is the index of the sword beam damage effect.
         */
        if (index == 25) {
            *should = false;
            /*
             * If the existing damage multiplier for sword beams is 0, use a damage multiplier of 1.0 instead.
             */
            u8 defaultMultiplier = damageTable->attack[index] & 0xF;
            *damage *= damageMultipliers[defaultMultiplier == 0 ? 1 : defaultMultiplier];
        }
    });

    COND_VB_SHOULD(VB_DAMAGE_EFFECT, CVAR, {
        int index = va_arg(args, int);
        DamageTable* damageTable = va_arg(args, DamageTable*);
        u32* effect = va_arg(args, u32*);
        Actor* actor = va_arg(args, Actor*);
        /*
         * 25 is the index of the sword beam damage effect.
         */
        if (index == 25) {
            *should = false;
            /*
             * If the sword beam effect is 0, use the light arrow effect instead.
             */
            u8 defaultEffect = (damageTable->attack[index] >> 4) & 0xF;
            if (defaultEffect == 0) {
                // 13 is the index of the light arrow damage effect
                *effect = (damageTable->attack[13] >> 4) & 0xF;
            } else {
                *effect = defaultEffect;
            }
            SetActorHitBySwordBeam(actor, true);
        } else if (index != 9) {
            /*
             * 9 is the index of the sword damage effect. With how FD plays, it is possible for the sword to connect
             * after sword beams have dealt damage. Without this check, the damage effect would revert back to the
             * light arrows effect upon sword collision.
             */
            SetActorHitBySwordBeam(actor, false);
        }
    });

    /*
     * Keese, Freezards, and Leevers are unique in that they call Actor_DrawDamageEffects() with NULL for the actor.
     * Normally, that method only uses the actor for playing positional sound effects. For sword beams only, we
     * overwrite these calls and pass in the actor so that the sword beam draws can be handled properly in the
     * VB_DRAW_DAMAGE_EFFECT hook.
     */
    COND_VB_SHOULD(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        // Only change the call if there is a sword beam collision
        if (GetActorHitBySwordBeam(actor)) {
            *should = false;
            Vec3f* bodyPartsPos = va_arg(args, Vec3f*);
            int bodyPartsCount = va_arg(args, int);
            if (actor->id == ACTOR_EN_FIREFLY) { // Keese
                EnFirefly* enFireFly = (EnFirefly*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount,
                                        enFireFly->drawDmgEffScale * actor->scale.y * 200.0f,
                                        enFireFly->drawDmgEffFrozenSteamScale, enFireFly->drawDmgEffAlpha,
                                        enFireFly->drawDmgEffType);
            } else if (actor->id == ACTOR_EN_FZ) { // Freezard
                EnFz* enFz = (EnFz*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount, enFz->drawDmgEffScale * 4.0f,
                                        0.5f, enFz->drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);
            } else if (actor->id == ACTOR_EN_NEO_REEBA) { // Leever
                EnNeoReeba* enNeoReeba = (EnNeoReeba*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount, enNeoReeba->drawEffectScale,
                                        0.5f, enNeoReeba->drawEffectAlpha, enNeoReeba->drawEffectType);
            }
        }
    });

    /*
     * If we're drawing the light arrow damage effect, but we know it's from a sword beam, then quietly change the type
     * to the blue lights effect.
     */
    COND_VB_SHOULD(VB_DRAW_DAMAGE_EFFECT, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        if (actor != nullptr && GetActorHitBySwordBeam(actor)) {
            u8* type = va_arg(args, u8*);
            if (*type == ACTOR_DRAW_DMGEFF_LIGHT_ORBS) {
                *type = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            }
        }
    });

    /*
     * If this is a sword beam collision, just hand wave it as a valid collision. This allows for sword beams to hit
     * enemies in a damaging way, such as Skulltulas and Big Octos.
     */
    COND_VB_SHOULD(VB_CHECK_BUMPER_COLLISION, CVAR, {
        ColliderElement* toucher = va_arg(args, ColliderElement*);
        if (toucher->atDmgInfo.dmgFlags & DMG_SWORD_BEAM) {
            *should = false;
        }
    });

    /*
     * If this is a sword beam collision, only handle it if the collided actor is an enemy. We don't want the sword
     * beams to collide with other objects and trigger their collision behavior, such as burning spiderwebs.
     */
    COND_VB_SHOULD(VB_PERFORM_AC_COLLISION, CVAR, {
        Collider* at = va_arg(args, Collider*);
        Collider* ac = va_arg(args, Collider*);
        ColliderElement* atInfo = va_arg(args, ColliderElement*);
        ColliderElement* acInfo = va_arg(args, ColliderElement*);
        /*
         * If the AT actor is EnMThunder with a subtype > ENMTHUNDER_SUBTYPE_SPIN_REGULAR, it is a sword beam. If the AC
         * actor is not an enemy/boss and does not normally collide with sword beams, then do not handle the sword beam
         * collision.
         */
        if (at->actor->id == ACTOR_EN_M_THUNDER && ((EnMThunder*)at->actor)->subtype > 1 &&
            ac->actor->category != ACTORCAT_ENEMY && ac->actor->category != ACTORCAT_BOSS &&
            !(acInfo->acDmgInfo.dmgFlags & DMG_SWORD_BEAM)) {
            *should = false;
        }
    });

    /*
     * Define a custom damage effect for sword beams for the Big Octo, which handles drawing damage effects differently
     * from most enemies. We cannot easily piggyback off of the light arrows effect like we do for everybody else.
     */
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_BIGOKUTA, CVAR, [](Actor* actor, bool* result) {
        EnBigokuta* enBigOkuta = (EnBigokuta*)actor;
        if (enBigOkuta->bodyCollider.base.acFlags & AC_HIT &&
            enBigOkuta->bodyCollider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_SWORD_BEAM) {
            enBigOkuta->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            enBigOkuta->drawDmgEffScale = 1.2f;
            enBigOkuta->drawDmgEffAlpha = 4.0f;
            Actor_Spawn(
                &gPlayState->actorCtx, gPlayState, ACTOR_EN_CLEAR_TAG, enBigOkuta->bodyCollider.elem.acDmgInfo.hitPos.x,
                enBigOkuta->bodyCollider.elem.acDmgInfo.hitPos.y, enBigOkuta->bodyCollider.elem.acDmgInfo.hitPos.z, 0,
                0, 3, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
    });

    /*
     * If it's a light ray effect type with yellow color, and the player is in the Fierce Deity form, assume it's
     * actually supposed to be the blue light ray effect.
     */
    COND_ID_HOOK(ShouldActorInit, ACTOR_EN_CLEAR_TAG, CVAR, [](Actor* actor, bool* result) {
        if ((actor->params == CLEAR_TAG_SMALL_LIGHT_RAYS || actor->params == CLEAR_TAG_LARGE_LIGHT_RAYS) &&
            actor->world.rot.z == 0 && GET_PLAYER(gPlayState)->transformation == PLAYER_FORM_FIERCE_DEITY) {
            actor->world.rot.z = 3;
        }
    });

    // Allow FD to open doors
    COND_VB_SHOULD(VB_BE_NEAR_DOOR, CVAR, {
        f32 playerZPosRelToDoor = *va_arg(args, f32*);
        // Vanilla proximity is 50.0f, but FD cannot get that close to some doors
        if (GET_PLAYER_FORM == PLAYER_FORM_FIERCE_DEITY && fabsf(playerZPosRelToDoor) < 60.0f) {
            *should = true;
        }
    });

    /*
     * Allow the FD Mask to be usable in water. This requires two pieces: using VB_DISABLE_ITEM_UNDERWATER to make the
     * assigned button itself enabled and using VB_USE_ITEM_CONSIDER_ITEM_ACTION to ensure the item action can be
     * completed in water. This is how, normally, Zora Mask is the only usable item in water.
     */
    COND_VB_SHOULD(VB_USE_ITEM_CONSIDER_ITEM_ACTION, CVAR, {
        PlayerItemAction itemAction = *va_arg(args, PlayerItemAction*);
        if (itemAction == PLAYER_IA_MASK_FIERCE_DEITY) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_DISABLE_ITEM_UNDERWATER, CVAR, {
        s32 item = va_arg(args, s32);
        if (item == ITEM_MASK_FIERCE_DEITY &&
            Player_GetEnvironmentalHazard(gPlayState) > PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        AllowTransformationMask(itemId, should);
    });

    /*
     * Needed to allow FD Link to use transformation masks while in water.
     */
    COND_VB_SHOULD(VB_FD_ALWAYS_WIELD_SWORD, CVAR, {
        if (Player_GetEnvironmentalHazard(gPlayState) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFierceDeityAnywhere, { CVAR_NAME });

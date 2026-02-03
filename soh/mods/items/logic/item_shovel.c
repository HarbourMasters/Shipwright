/**
 * item_shovel.c - Shovel from Link's Awakening
 *
 * Controls:
 *   C Button: Dig at current position
 *
 * Features:
 *   - Uncovers buried items (rupees, hearts, secret items)
 *   - Cannot use while swimming, attacking, or shielding
 *   - Uses dig animation with dust particles
 */

#include "z64.h"
#include "item_shovel.h"
#include "../custom_items.h"
#include "../helpers/equip_helper.h"
#include "../helpers/fx_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Obj_Makekinsuta/z_obj_makekinsuta.h"
#include "overlays/actors/ovl_Door_Ana/z_door_ana.h"
#include "overlays/actors/ovl_En_Tk/z_en_tk.h"
#include "../objects/shovel_hole_DL/model.inc.c"

extern EnItem00* Item_DropCollectible(PlayState* play, Vec3f* spawnPos, s16 params);
extern void DoorAna_WaitOpen(DoorAna* this, PlayState* play);

static s8 sShovelPrevInvinc = 0;

// Shovel Hole Actor
typedef struct { Actor actor; s16 lifetime; f32 scale; } ShovelHole;

void ShovelHole_Init(Actor* thisx, PlayState* play) {
    ShovelHole* this = (ShovelHole*)thisx;
    this->lifetime = SHOVEL_HOLE_LIFETIME;
    this->scale = 0.0f;
    this->actor.world.rot.x = 0x4000;
}

void ShovelHole_Update(Actor* thisx, PlayState* play) {
    ShovelHole* this = (ShovelHole*)thisx;
    if (this->scale < 1.0f) this->scale += 0.1f;

    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;
    while (actor != NULL) {
        if (actor->update != NULL) {
            f32 dist = Math_Vec3f_DistXYZ(&this->actor.world.pos, &actor->world.pos);
            s32 canDamage = 0;
            switch (actor->id) {
                case ACTOR_EN_REEBA:
                case ACTOR_EN_DEKUNUTS:
                case ACTOR_EN_DNS:
                case ACTOR_EN_HINTNUTS:
                    canDamage = 1;
                    break;
            }
            if (canDamage && dist < SHOVEL_HOLE_RADIUS) {
                if (actor->colChkInfo.health > 0) {
                    actor->colChkInfo.health -= 16;
                    if (actor->colChkInfo.health <= 0) {
                        actor->colChkInfo.health = 0;
                        Enemy_StartFinishingBlow(play, actor);
                    }
                    Audio_PlayActorSound2(actor, NA_SE_EN_NUTS_DAMAGE);
                }
            }
        }
        actor = actor->next;
    }

    this->lifetime--;
    if (this->lifetime <= 0) Actor_Kill(&this->actor);
}

void ShovelHole_Draw(Actor* thisx, PlayState* play) {
    ShovelHole* this = (ShovelHole*)thisx;
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + 0.5f, this->actor.world.pos.z, MTXMODE_NEW);
    f32 finalScale = this->scale * 0.01f;
    Matrix_Scale(finalScale, 1.0f, finalScale, MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    u8 alpha = (this->lifetime > 60) ? 180 : (this->lifetime * 3);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 50, 35, 15, alpha);
    gSPDisplayList(POLY_XLU_DISP++, g_shovelhole_dl);
    CLOSE_DISPS(play->state.gfxCtx);
}

static void DropRandomItem(PlayState* play, Vec3f* pos) {
    // Graveyard heart piece
    if (play->sceneNum == SCENE_GRAVEYARD && !Flags_GetItemGetInf(ITEMGETINF_1C)) {
        s16 itemType = ((COLLECTFLAG_GRAVEDIGGING_HEART_PIECE & 0x3F) << 8) | ITEM00_HEART_PIECE;
        Vec3f dropPos = *pos; dropPos.y += 10.0f;
        Item_DropCollectible(play, &dropPos, itemType);
        Flags_SetItemGetInf(ITEMGETINF_1C);
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &dropPos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        return;
    }

    if (Rand_ZeroOne() * 100.0f > (f32)SHOVEL_ITEM_DROP_CHANCE) return;

    f32 roll = Rand_ZeroOne() * 100.0f;
    s16 itemType;

    if (roll < (f32)SHOVEL_FAIRY_CHANCE) {
        itemType = ITEM00_FAIRY;
    } else if (roll < 50.0f) {
        f32 rupeeRoll = Rand_ZeroOne();
        if (rupeeRoll < 0.70f) itemType = ITEM00_RUPEE_GREEN;
        else if (rupeeRoll < 0.95f) itemType = ITEM00_RUPEE_BLUE;
        else itemType = ITEM00_RUPEE_RED;
    } else if (roll < 75.0f) {
        f32 recoveryRoll = Rand_ZeroOne();
        if (recoveryRoll < 0.50f) itemType = ITEM00_HEART;
        else if (recoveryRoll < 0.75f) itemType = ITEM00_MAGIC_SMALL;
        else itemType = ITEM00_MAGIC_LARGE;
    } else {
        f32 consumableRoll = Rand_ZeroOne();
        if (consumableRoll < 0.6f) itemType = ITEM00_BOMBS_A;
        else itemType = ITEM00_NUTS;
    }

    Vec3f dropPos = *pos; dropPos.y += 10.0f;
    Item_DropCollectible(play, &dropPos, itemType);
    Audio_PlaySoundGeneral(NA_SE_SY_GET_ITEM, &dropPos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

static void ActivateSoftSoil(PlayState* play, Actor* soilActor) {
    ObjMakekinsuta* soil = (ObjMakekinsuta*)soilActor;
    soil->unk_152 = 1;
    Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &soilActor->world.pos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    FX_SpawnParticles(play, &soilActor->world.pos, FX_DUST, 6);
}

static void RevealGrotto(PlayState* play, Actor* grottoActor) {
    DoorAna* grotto = (DoorAna*)grottoActor;
    if ((grotto->actor.params & 0x300) != 0) {
        if ((grotto->actor.params & 0x200) != 0) {
            Collider_DestroyCylinder(play, &grotto->collider);
        }
        grotto->actor.params &= ~0x0300;
        grotto->actionFunc = DoorAna_WaitOpen;
        grotto->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &gSfxDefaultPos, 4,
            &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        FX_SpawnParticles(play, &grotto->actor.world.pos, FX_DUST, 12);
    }
}

static void CreateDamageHole(PlayState* play, Vec3f* pos) {
    shHoleActor = (Actor*)pos;
}

static void PerformDig(Player* p, PlayState* play) {
    Vec3f digPos = p->actor.world.pos;
    digPos.y = p->actor.floorHeight + 5.0f;
    s16 facingYaw = p->actor.shape.rot.y;
    digPos.x += Math_SinS(facingYaw) * 30.0f;
    digPos.z += Math_CosS(facingYaw) * 30.0f;

    FX_SpawnParticles(play, &digPos, FX_DUST, 8);
    Audio_PlaySoundGeneral(NA_SE_PL_WALK_SAND, &digPos, 4,
        &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    s32 foundSpecial = 0;
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    while (actor != NULL) {
        if (actor->update != NULL) {
            f32 dist = Math_Vec3f_DistXYZ(&digPos, &actor->world.pos);
            switch (actor->id) {
                case ACTOR_OBJ_MAKEKINSUTA:
                    if (dist < SHOVEL_BEAN_RADIUS) { ActivateSoftSoil(play, actor); foundSpecial = 1; }
                    break;
                case ACTOR_DOOR_ANA:
                    if (dist < SHOVEL_DOOR_ANA_RADIUS) { RevealGrotto(play, actor); foundSpecial = 1; }
                    break;
            }
        }
        actor = actor->next;
    }

    DropRandomItem(play, &digPos);
    if (!foundSpecial) CreateDamageHole(play, &digPos);
}

static void Shovel_Stop(Player* p, PlayState* play) {
    if (!shActive) return;
    shActive = 0; shAnimating = 0; shAnimTimer = 0;
    p->upperLimbRot.x = p->upperLimbRot.y = p->upperLimbRot.z = 0;
    p->jointTable[PLAYER_LIMB_R_SHOULDER].x = 0;
    p->jointTable[PLAYER_LIMB_R_FOREARM].x = 0;
    p->jointTable[PLAYER_LIMB_L_SHOULDER].x = 0;
    p->jointTable[PLAYER_LIMB_L_FOREARM].x = 0;
    p->jointTable[PLAYER_LIMB_R_SHOULDER].y = 0;
    p->jointTable[PLAYER_LIMB_L_SHOULDER].y = 0;
    p->jointTable[PLAYER_LIMB_R_THIGH].x = 0;
    p->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;
    ItemEquip_PlayUnequipSFX(play, p);
}

static void Shovel_Start(Player* p, PlayState* play) {
    if (shActive) return;
    shActive = 1; shAnimating = 1; shAnimTimer = 0;
    ItemEquip_PlayEquipSFX(play, p);
}

static void Shovel_UpdateAnimation(Player* p, PlayState* play) {
    p->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    p->actor.speedXZ = 0.0f;

    if (shAnimTimer < 10) {
        f32 prog = shAnimTimer / 10.0f;
        p->upperLimbRot.x = (s16)(prog * 800.0f);
        p->jointTable[PLAYER_LIMB_R_SHOULDER].x = (s16)(prog * -4000.0f);
        p->jointTable[PLAYER_LIMB_R_FOREARM].x = (s16)(prog * -2000.0f);
        p->jointTable[PLAYER_LIMB_L_SHOULDER].x = (s16)(prog * -4000.0f);
        p->jointTable[PLAYER_LIMB_L_FOREARM].x = (s16)(prog * -2000.0f);
        p->jointTable[PLAYER_LIMB_R_SHOULDER].y = (s16)(prog * 1500.0f);
        p->jointTable[PLAYER_LIMB_L_SHOULDER].y = (s16)(prog * -1500.0f);
    } else if (shAnimTimer < 20) {
        f32 prog = (shAnimTimer - 10) / 10.0f;
        p->upperLimbRot.x = (s16)(800.0f - prog * 3300.0f);
        p->jointTable[PLAYER_LIMB_R_SHOULDER].x = (s16)(-4000.0f + prog * 6000.0f);
        p->jointTable[PLAYER_LIMB_R_FOREARM].x = (s16)(-2000.0f + prog * 1000.0f);
        p->jointTable[PLAYER_LIMB_L_SHOULDER].x = (s16)(-4000.0f + prog * 6000.0f);
        p->jointTable[PLAYER_LIMB_L_FOREARM].x = (s16)(-2000.0f + prog * 1000.0f);
        p->jointTable[PLAYER_LIMB_R_SHOULDER].y = (s16)(1500.0f + prog * 2000.0f);
        p->jointTable[PLAYER_LIMB_L_SHOULDER].y = (s16)(-1500.0f - prog * 2000.0f);
        p->jointTable[PLAYER_LIMB_R_THIGH].x = (s16)(prog * -2000.0f);
        if (shAnimTimer % 4 < 2) p->upperLimbRot.x -= 400;
        if (shAnimTimer == 10) PerformDig(p, play);
    } else {
        f32 prog = (shAnimTimer - 20) / 10.0f;
        p->upperLimbRot.x = (s16)(-2500.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_R_SHOULDER].x = (s16)(2000.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_R_FOREARM].x = (s16)(-1000.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_L_SHOULDER].x = (s16)(2000.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_L_FOREARM].x = (s16)(-1000.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_R_SHOULDER].y = (s16)(3500.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_L_SHOULDER].y = (s16)(-3500.0f * (1.0f - prog));
        p->jointTable[PLAYER_LIMB_R_THIGH].x = (s16)(-2000.0f * (1.0f - prog));
    }

    shAnimTimer++;
    if (shAnimTimer >= SHOVEL_ANIM_DURATION) Shovel_Stop(p, play);
}

void Handle_Shovel(Player* p, PlayState* play) {
    ItemInputState in;
    ItemInput_Update(&in, ITEM_SHOVEL, p, play);

    if (!in.wasEquipped) { if (shActive) Shovel_Stop(p, play); return; }
    if (ItemInput_IsBlocked(p, play)) { if (shActive) Shovel_Stop(p, play); return; }
    if (ItemInput_CheckDamage(p, &sShovelPrevInvinc)) { Shovel_Stop(p, play); return; }
    if (in.otherButtonPressed) { Shovel_Stop(p, play); return; }

    if (!shActive) {
        // All restrictions: no water, must be on ground
        if (p->stateFlags1 & PLAYER_STATE1_IN_WATER) return;
        if (p->meleeWeaponState != 0) return;
        if (in.isPressed && (p->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Shovel_Start(p, play);
        }
        return;
    }

    if (shAnimating) Shovel_UpdateAnimation(p, play);
}

s32 Player_UpperAction_Shovel(Player* p, PlayState* play) { return 0; }

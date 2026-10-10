/*
 * File: z_obj_ice_poly.c
 * Overlay: ovl_Obj_Ice_Poly
 * Description: Large Block of Meltable Ice
 */

#include "z_obj_ice_poly.h"
#include "overlays/actors/ovl_Obj_Aqua/z_obj_aqua.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjIcePoly_Init(Actor* thisx, PlayState* play);
void ObjIcePoly_Destroy(Actor* thisx, PlayState* play);
void ObjIcePoly_Update(Actor* thisx, PlayState* play);
void ObjIcePoly_Draw(Actor* thisx, PlayState* play);

void func_80931828(ObjIcePoly* this, PlayState* play);
void func_80931A38(ObjIcePoly* this, PlayState* play);
void func_80931E58(ObjIcePoly* this, PlayState* play);
void func_80931EEC(ObjIcePoly* this, PlayState* play);

ActorProfile Obj_Ice_Poly_Profile = {
    /**/ ACTOR_OBJ_ICE_POLY,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjIcePoly),
    /**/ ObjIcePoly_Init,
    /**/ ObjIcePoly_Destroy,
    /**/ ObjIcePoly_Update,
    /**/ ObjIcePoly_Draw,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x02, 0x00 },
        { 0xF7CFF7FF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 50, 105, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK5,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x00000800, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 65, 105, 0, { 0, 0, 0 } },
};

static Color_RGBA8 D_80932378 = { 250, 250, 250, 255 };
static Color_RGBA8 D_8093237C = { 180, 180, 180, 255 };

void ObjIcePoly_Init(Actor* thisx, PlayState* play) {
    ObjIcePoly* this = (ObjIcePoly*)thisx;
    s32 i;

    this->switchFlag = OBJICEPOLY_GET_SWITCH_FLAG(thisx);
    thisx->params = OBJICEPOLY_GET_SCALE(thisx);
    thisx->cullingVolumeDistance = 5600.0f;

    Actor_SetScale(thisx, thisx->params * 0.01f);

    for (i = 0; i < ARRAY_COUNT(this->colliders1); i++) {
        Collider_InitAndSetCylinder(play, &this->colliders1[i], thisx, &sCylinderInit1);
        Collider_InitAndSetCylinder(play, &this->colliders2[i], thisx, &sCylinderInit2);
        Collider_UpdateCylinder(thisx, &this->colliders1[i]);
        Collider_UpdateCylinder(thisx, &this->colliders2[i]);
    }

    this->colliders1[0].dim.radius = thisx->scale.x * 48.0f;
    this->colliders1[0].dim.height = thisx->scale.y * 58.0f;
    this->colliders2[0].dim.height = this->colliders1[0].dim.height;
    this->colliders2[0].dim.radius = this->colliders1[0].dim.radius + 17;

    this->colliders1[1].dim.radius = thisx->scale.x * 23.0f;
    this->colliders1[1].dim.height = thisx->scale.y * 50.0f;
    this->colliders2[1].dim.radius = this->colliders1[1].dim.radius;
    this->colliders2[1].dim.height = this->colliders1[1].dim.height;

    this->colliders1[1].dim.yShift = this->colliders1[0].dim.height;
    this->colliders2[1].dim.yShift = this->colliders1[1].dim.yShift;

    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_148 = 255;

    Actor_SetFocus(thisx, thisx->scale.y * 30.0f);

    thisx->shape.rot.x = 0x500;
    thisx->shape.rot.z = -0x500;

    if (((this->switchFlag != OBJICEPOLY_SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) ||
        ((play->sceneId == SCENE_KAJIYA) && CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE))) {
        Actor_Kill(thisx);
        return;
    }

    this->actionFunc = func_80931A38;
}

void ObjIcePoly_Destroy(Actor* thisx, PlayState* play) {
    ObjIcePoly* this = (ObjIcePoly*)thisx;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->colliders1); i++) {
        Collider_DestroyCylinder(play, &this->colliders1[i]);
        Collider_DestroyCylinder(play, &this->colliders2[i]);
    }
}

void func_80931828(ObjIcePoly* this, PlayState* play) {
    static Color_RGBA8 D_80932380 = { 170, 255, 255, 255 };
    static Color_RGBA8 D_80932384 = { 0, 50, 100, 255 };
    static Vec3f D_80932388 = { 0.0f, -1.0f, 0.0f };
    f32 temp;
    Vec3f spA0;
    Vec3f sp94;
    f32 sp90;
    s32 i;
    CollisionPoly* sp88;

    spA0.x = this->actor.world.pos.x;
    spA0.y = (this->colliders1[0].dim.height * 2) + this->actor.world.pos.y;
    spA0.z = this->actor.world.pos.z;

    sp90 = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp88, &spA0);
    if (sp90 < this->actor.world.pos.y) {
        sp90 = this->actor.world.pos.y;
    }
    temp = spA0.y - sp90;

    for (i = 0; i < 30; i++) {
        sp94.x = Rand_CenteredFloat(12.0f);
        sp94.y = Rand_ZeroFloat(15.0f);
        sp94.z = Rand_CenteredFloat(12.0f);

        spA0.x = (this->colliders1[0].dim.radius * (sp94.x * (1.0f / 12))) + this->actor.world.pos.x;
        spA0.z = (this->colliders1[0].dim.radius * (sp94.z * (1.0f / 12))) + this->actor.world.pos.z;
        spA0.y = Rand_ZeroFloat(temp) + sp90;
        EffectSsEnIce_Spawn(play, &spA0, (Rand_ZeroFloat(0.4f) + 0.3f) * this->actor.scale.x, &sp94, &D_80932388,
                            &D_80932380, &D_80932384, 40);
    }
}

void func_80931A38(ObjIcePoly* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f sp6C;
    s32 i;
    s32 pad2;
    s32 phi_v0;
    s32 pad3;
    f32 sp58;

    if (!(player->stateFlags2 & PLAYER_STATE2_4000) && (this->unk_14A != 0)) {
        this->unk_14A--;
    }

    if ((this->colliders1[0].base.atFlags & AT_HIT) || (this->colliders1[1].base.atFlags & AT_HIT)) {
        this->unk_14A = 40;
        this->colliders1[0].base.atFlags &= ~AT_HIT;
        this->colliders1[1].base.atFlags &= ~AT_HIT;
    }

    if (((this->colliders2[0].base.acFlags & AC_HIT) &&
         ((this->colliders2[0].base.ac == NULL) ||
          ((this->colliders2[0].base.ac->id != ACTOR_OBJ_AQUA) &&
           (this->colliders2[0].elem.acHitElem->atDmgInfo.dmgFlags == 0x800)) ||
          ((this->colliders2[0].base.ac->id == ACTOR_OBJ_AQUA) &&
           (this->colliders2[0].base.ac->params == AQUA_TYPE_HOT)))) ||
        ((this->colliders2[1].base.acFlags & AC_HIT) &&
         ((this->colliders2[1].base.ac == NULL) ||
          ((this->colliders2[1].base.ac->id != ACTOR_OBJ_AQUA) &&
           (this->colliders2[1].elem.acHitElem->atDmgInfo.dmgFlags == 0x800)) ||
          ((this->colliders2[1].base.ac->id == ACTOR_OBJ_AQUA) &&
           (this->colliders2[1].base.ac->params == AQUA_TYPE_HOT))))) {
        CutsceneManager_Queue(this->actor.csId);
        this->unk_14A = 0;
        this->actionFunc = func_80931E58;
        this->actor.focus.rot.y = this->actor.yawTowardsPlayer;

        if (play->sceneId == SCENE_00KEIKOKU) {
            Actor* actor = NULL;

            do {
                actor = SubS_FindActor(play, actor, ACTORCAT_ITEMACTION, ACTOR_OBJ_ICE_POLY);
                if (actor != NULL) {
                    if ((&this->actor != actor) && (&this->actor != actor)) {
                        ((ObjIcePoly*)actor)->unk_14A = 0;
                        ((ObjIcePoly*)actor)->actionFunc = func_80931E58;
                    }
                    actor = actor->next;
                }
            } while (actor != NULL);
        }
    } else if ((this->switchFlag != OBJICEPOLY_SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        CutsceneManager_Queue(this->actor.csId);
        this->unk_14A = 1;
        this->actionFunc = func_80931E58;
    } else {
        if ((this->actor.parent != NULL) && (this->actor.parent->update != NULL)) {
            this->actor.parent->freezeTimer = 40;
        } else {
            this->actor.parent = NULL;
        }

        if (this->unk_14A == 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliders1[0].base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliders1[1].base);
        }

        for (i = 0; i < ARRAY_COUNT(this->colliders1); i++) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliders1[i].base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliders1[i].base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliders2[i].base);
        }
    }

    if (Rand_ZeroOne() < 0.05f) {
        sp58 = Rand_ZeroOne();
        if (Rand_ZeroOne() < 0.5f) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        sp6C.x = (phi_v0 * (15.0f + (sp58 * 15.0f)) * this->actor.scale.x) + this->actor.world.pos.x;
        sp6C.y = (((Rand_ZeroOne() * 90.0f) + 10.0f) * this->actor.scale.y) + this->actor.world.pos.y;

        sp58 = Rand_ZeroOne();
        if (Rand_ZeroOne() < 0.5f) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        sp6C.z = (phi_v0 * (15.0f + (sp58 * 15.0f)) * this->actor.scale.z) + this->actor.world.pos.z;

        EffectSsKirakira_SpawnDispersed(play, &sp6C, &gZeroVec3f, &gZeroVec3f, &D_80932378, &D_8093237C, 2000, 5);
    }
}

void func_80931E58(ObjIcePoly* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        if (this->unk_14A == 1) {
            func_80931828(this, play);
            Actor_Kill(&this->actor);
            return;
        }

        this->unk_14A = 40;
        Actor_PlaySfx(&this->actor, NA_SE_EV_ICE_MELT);
        this->actionFunc = func_80931EEC;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80931EEC(ObjIcePoly* this, PlayState* play) {
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    f32 temp_f20;
    s32 i;
    s32 phi_v0;

    spAC.x = 0.0f;
    spAC.y = this->actor.scale.y;
    spAC.z = 0.0f;

    spA0.x = 0.0f;
    spA0.y = this->actor.scale.y;
    spA0.z = 0.0f;

    for (i = 0; i < 2; i++) {
        temp_f20 = Rand_ZeroOne();
        if (Rand_ZeroOne() < 0.5f) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        sp94.x = (phi_v0 * (20.0f + (20.0f * temp_f20)) * this->actor.scale.x) + this->actor.world.pos.x;
        sp94.y = (Rand_ZeroOne() * this->actor.scale.y * 50.0f) + this->actor.world.pos.y;

        temp_f20 = Rand_ZeroOne();
        if (Rand_ZeroOne() < 0.5f) {
            phi_v0 = -1;
        } else {
            phi_v0 = 1;
        }
        sp94.z = (phi_v0 * (20.0f + (20.0f * temp_f20)) * this->actor.scale.x) + this->actor.world.pos.z;

        func_800B0DE0(play, &sp94, &spA0, &spAC, &D_80932378, &D_8093237C,
                      ((Rand_ZeroOne() * 100.0f) + 350.0f) * this->actor.scale.x, this->actor.scale.x * 20.0f);
    }

    if (this->unk_14A != 0) {
        this->unk_14A--;
    }

    this->actor.scale.y = this->actor.params * 0.01f * (0.5f + (this->unk_14A * 0.0125f));
    this->unk_148 -= 6;

    if (this->unk_14A == 0) {
        CutsceneManager_Stop(this->actor.csId);
        if (this->switchFlag != OBJICEPOLY_SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        Actor_Kill(&this->actor);
    }
}

void ObjIcePoly_Update(Actor* thisx, PlayState* play) {
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    this->actionFunc(this, play);
}

void ObjIcePoly_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjIcePoly* this = (ObjIcePoly*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, play->gameplayFrames % 256, 0x20, 0x10, 1, 0,
                                  (play->gameplayFrames * 2) % 256, 0x40, 0x20, 0, 1, 0, 2));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->unk_148);
    gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

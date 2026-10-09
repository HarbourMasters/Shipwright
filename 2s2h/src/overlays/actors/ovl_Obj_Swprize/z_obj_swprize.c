/*
 * File: z_obj_swprize.c
 * Overlay: ovl_Obj_Swprize
 * Description: Spawns item drop from soft soil
 */

#include "z_obj_swprize.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjSwprize_Init(Actor* thisx, PlayState* play);
void ObjSwprize_Destroy(Actor* thisx, PlayState* play);
void ObjSwprize_Update(Actor* thisx, PlayState* play);

void ObjSwprize_DoNothing(ObjSwprize* this, PlayState* play);
void func_80C25654(ObjSwprize* this, PlayState* play);
void func_80C25640(ObjSwprize* this);
void func_80C25698(ObjSwprize* this);
void func_80C256AC(ObjSwprize* this, PlayState* play);
void func_80C25710(ObjSwprize* this);
void func_80C2572C(ObjSwprize* this, PlayState* play);
void ObjSwprize_SetupDoNothing(ObjSwprize* this);

ActorProfile Obj_Swprize_Profile = {
    /**/ ACTOR_OBJ_SWPRIZE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjSwprize),
    /**/ ObjSwprize_Init,
    /**/ ObjSwprize_Destroy,
    /**/ ObjSwprize_Update,
    /**/ NULL,
};

s16 D_80C257F0[] = { 2, 0x14, 1, 8 };

s16 D_80C257F8[] = { -0x888, 0, 0x888 };

void func_80C25360(ObjSwprize* this, Vec3f* vec) {
    Matrix_Push();
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_MultVecY(1.0f, vec);
    Matrix_Pop();
}

void func_80C253D0(ObjSwprize* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s32 i;
    Actor* collectible;
    Vec3f sp78;
    s32 type = OBJ_SWPRIZE_GET_TYPE(thisx);
    s32 temp_s0 = D_80C257F0[type];

    func_80C25360(this, &sp78);

    if (type == 2) {
        for (i = 0; i < 3; i++) {
            collectible = Item_DropCollectible(play, &thisx->world.pos, temp_s0);
            if (collectible != NULL) {
                if (sp78.y < 0.98f) {
                    collectible->velocity.y = (sp78.y + 1.0f) * 4.0f;
                    collectible->speed = (2.0f * (1.0f - fabsf(sp78.y))) + 2.0f;
                    collectible->world.rot.y = Math_Atan2S_XY(sp78.z, sp78.x) + D_80C257F8[i];
                } else {
                    collectible->world.rot.y = i * (0x10000 / 3);
                }
            }
        }
    } else {
        collectible = Item_DropCollectible(play, &thisx->world.pos, temp_s0);
        if ((collectible != NULL) && (sp78.y < 0.98f)) {
            collectible->velocity.y = (sp78.y + 1.0f) * 4.0f;
            collectible->speed = (2.0f * (1.0f - fabsf(sp78.y))) + 2.0f;
            collectible->world.rot.y = Math_Atan2S_XY(sp78.z, sp78.x);
        }
    }
}

void ObjSwprize_Init(Actor* thisx, PlayState* play) {
    ObjSwprize* this = (ObjSwprize*)thisx;

    if (Flags_GetSwitch(play, OBJ_SWPRIZE_GET_SWITCH_FLAG(&this->actor))) {
        ObjSwprize_SetupDoNothing(this);
    } else {
        func_80C25640(this);
    }
}

void ObjSwprize_Destroy(Actor* thisx, PlayState* play) {
}

void func_80C25640(ObjSwprize* this) {
    this->actionFunc = func_80C25654;
}

void func_80C25654(ObjSwprize* this, PlayState* play) {
    if (Flags_GetSwitch(play, OBJ_SWPRIZE_GET_SWITCH_FLAG(&this->actor))) {
        func_80C25698(this);
    }
}

void func_80C25698(ObjSwprize* this) {
    this->actionFunc = func_80C256AC;
}

void func_80C256AC(ObjSwprize* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        func_80C253D0(this, play);
        func_80C25710(this);
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80C25710(ObjSwprize* this) {
    this->timer = 40;
    this->actionFunc = func_80C2572C;
}

void func_80C2572C(ObjSwprize* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        if (this->timer == 0) {
            CutsceneManager_Stop(this->actor.csId);
            ObjSwprize_SetupDoNothing(this);
        }
    }
}

void ObjSwprize_SetupDoNothing(ObjSwprize* this) {
    this->actionFunc = ObjSwprize_DoNothing;
}

void ObjSwprize_DoNothing(ObjSwprize* this, PlayState* play) {
}

void ObjSwprize_Update(Actor* thisx, PlayState* play) {
    ObjSwprize* this = (ObjSwprize*)thisx;

    this->actionFunc(this, play);
}

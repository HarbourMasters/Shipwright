/*
 * File: z_obj_makekinsuta.c
 * Overlay: ovl_Obj_Makekinsuta
 * Description: Swamp Spider House - Soft soil with Skulltula hiding in it
 */

#include "z_obj_makekinsuta.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjMakekinsuta_Init(Actor* thisx, PlayState* play);
void ObjMakekinsuta_Destroy(Actor* thisx, PlayState* play);
void ObjMakekinsuta_Update(Actor* thisx, PlayState* play);

void func_8099FB64(Actor* thisx, PlayState* play);
void func_8099FD7C(Actor* thisx, PlayState* play);

ActorProfile Obj_Makekinsuta_Profile = {
    /**/ ACTOR_OBJ_MAKEKINSUTA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjMakekinsuta),
    /**/ ObjMakekinsuta_Init,
    /**/ ObjMakekinsuta_Destroy,
    /**/ ObjMakekinsuta_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1, ICHAIN_STOP),
};

bool func_8099FA40(ObjMakekinsuta* this, PlayState* play) {
    s32 chestFlag = -1;
    s32 skulltulaParams = (OBJMAKEKINSUTA_GET_1F(&this->actor) << 2) | 0xFF01;

    if (OBJMAKEKINSUTA_GETS_3(skulltulaParams)) {
        chestFlag = OBJMAKEKINSUTA_GETS_3FC(skulltulaParams);
    }
    return (chestFlag < 0) == true || !Flags_GetTreasure(play, chestFlag);
}

void ObjMakekinsuta_Init(Actor* thisx, PlayState* play) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (!func_8099FA40(this, play)) {
        this->unk144 = -1;
    }
    if (Flags_GetSwitch(play, OBJMAKEKINSUTA_GET_SWITCH_FLAG(thisx))) {
        Actor_Kill(&this->actor);
    }
}

void ObjMakekinsuta_Destroy(Actor* thisx, PlayState* play) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    if (func_8099FA40(this, play)) {
        Flags_UnsetSwitch(play, OBJMAKEKINSUTA_GET_SWITCH_FLAG(thisx));
    }
}

void func_8099FB64(Actor* thisx, PlayState* play) {
    s32 pad[0x2];
    Actor* actor;
    Vec3f destVec;
    s16 rotY;
    f32 speedXZ;
    s32 rotCheck;
    static Vec3f D_8099FE3C = { 0.0f, 1.0f, 0.0f };

    rotCheck = thisx->shape.rot.x == 0;
    if (rotCheck) {
        rotCheck = thisx->shape.rot.z == 0;
    }

    if (rotCheck) {
        rotY = thisx->shape.rot.y;
    } else {
        Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_NEW);
        Matrix_RotateXS(thisx->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(thisx->shape.rot.z, MTXMODE_APPLY);
        Matrix_MultVec3f(&D_8099FE3C, &destVec);
        rotY = Math_Atan2S_XY(destVec.z, destVec.x);
    }
    actor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SW, thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z,
                        0, rotY, 0, (OBJMAKEKINSUTA_GET_1F(thisx) << 2) | 0xFF01);
    if (actor != NULL) {
        actor->parent = thisx;
        if (rotCheck) {
            actor->velocity.y = 10.0f;
            actor->speed = 3.0f;
        } else {
            speedXZ = sqrtf((destVec.x * destVec.x) + (destVec.z * destVec.z));
            actor->velocity.y = (4 * destVec.y) + 4.0f;
            actor->speed = (2 * speedXZ) + 2.0f;
        }
    }
}

void ObjMakekinsuta_Update(Actor* thisx, PlayState* play) {
    ObjMakekinsuta* this = (ObjMakekinsuta*)thisx;

    if (Flags_GetSwitch(play, OBJMAKEKINSUTA_GET_SWITCH_FLAG(thisx))) {
        this->actor.update = func_8099FD7C;
        CutsceneManager_Queue(this->actor.csId);
    } else {
        if (this->unk144 >= 0) {
            if (this->unk144 == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
                if (Rand_ZeroOne() < 0.1f) {
                    this->unk144 = Rand_S16Offset(0x28, 0x50);
                } else {
                    this->unk144 = 8;
                }
            } else {
                this->unk144--;
            }
        }
    }
}

void func_8099FD7C(Actor* thisx, PlayState* play) {
    if (CutsceneManager_IsNext(thisx->csId)) {
        CutsceneManager_StartWithPlayerCs(thisx->csId, thisx);
        if (thisx->csId > CS_ID_NONE) {
            Player_SetCsActionWithHaltedActors(play, thisx, PLAYER_CSACTION_4);
        }
        func_8099FB64(thisx, play);
        thisx->update = Actor_Noop;
        thisx->flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else {
        CutsceneManager_Queue(thisx->csId);
    }
}

/*
 * File: z_obj_mure3.c
 * Overlay: ovl_Obj_Mure3
 * Description: Group Rupee spawner
 */

#include "z_obj_mure3.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

void ObjMure3_Init(Actor* thisx, PlayState* play);
void ObjMure3_Destroy(Actor* thisx, PlayState* play);
void ObjMure3_Update(Actor* thisx, PlayState* play);

void func_8098F598(ObjMure3* this);
void func_8098F5AC(ObjMure3* this, PlayState* play);
void func_8098F5D0(ObjMure3* this);
void func_8098F5E4(ObjMure3* this, PlayState* play);
void func_8098F66C(ObjMure3* this);
void func_8098F680(ObjMure3* this, PlayState* play);

ActorProfile Obj_Mure3_Profile = {
    /**/ ACTOR_OBJ_MURE3,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjMure3),
    /**/ ObjMure3_Init,
    /**/ ObjMure3_Destroy,
    /**/ ObjMure3_Update,
    /**/ NULL,
};

static s16 sRupeeCounts[] = { 5, 5, 7, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void func_8098F040(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;

    Math_Vec3f_Copy(&spawnPos, &this->actor.world.pos);
    for (i = 0; i < 5; i++, spawnPos.y += 20.0f) {
        if (!((this->unk164 >> i) & 1)) {
            this->unk148[i] = (EnItem00*)Item_DropCollectible2(play, &spawnPos, 0x10001);
            if (this->unk148[i] != NULL) {
                this->unk148[i]->actor.room = this->actor.room;
            }
        }
    }
}

void func_8098F110(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;
    f32 sin = Math_SinS(this->actor.world.rot.y);
    f32 cos = Math_CosS(this->actor.world.rot.y);
    f32 dist;

    spawnPos.y = this->actor.world.pos.y;

    for (i = 0, dist = -40.0f; i < 5; i++, dist += 20.0f) {
        if (!((this->unk164 >> i) & 1)) {
            spawnPos.x = this->actor.world.pos.x + (sin * dist);
            spawnPos.z = this->actor.world.pos.z + (cos * dist);
            this->unk148[i] = (EnItem00*)Item_DropCollectible2(play, &spawnPos, 0x4000);
            if (this->unk148[i] != NULL) {
                this->unk148[i]->actor.room = this->actor.room;
            }
        }
    }
}

void func_8098F220(ObjMure3* this, PlayState* play) {
    s16 yRot;
    Vec3f spawnPos;
    s32 i;

    spawnPos.y = this->actor.world.pos.y;
    yRot = this->actor.world.rot.y;

    for (i = 0; i < 6; i++) {
        if (GameInteractor_Should(VB_OBJ_MURE3_DROP_COLLECTIBLE, !((this->unk164 >> i) & 1), this, i)) {
            spawnPos.x = (Math_SinS(yRot) * 40.0f) + this->actor.world.pos.x;
            spawnPos.z = (Math_CosS(yRot) * 40.0f) + this->actor.world.pos.z;
            this->unk148[i] = (EnItem00*)Item_DropCollectible2(play, &spawnPos, 0x4000);
            if (this->unk148[i] != NULL) {
                this->unk148[i]->actor.room = this->actor.room;
            }
        }
        yRot += 0x2AAA;
    }

    if (GameInteractor_Should(VB_OBJ_MURE3_DROP_COLLECTIBLE, !((this->unk164 >> 6) & 1), this, 6)) {
        spawnPos.x = this->actor.world.pos.x;
        spawnPos.z = this->actor.world.pos.z;
        this->unk160 = Item_DropCollectible2(play, &spawnPos, 0x4002);
        if (this->unk160 != NULL) {
            this->unk160->room = this->actor.room;
        }
    }
}

void func_8098F364(ObjMure3* this, PlayState* play) {
    s16 count = sRupeeCounts[OBJMURE3_PARAM_RUPEEINDEX(&this->actor)];
    s32 i;
    EnItem00** collectible;

    for (i = 0; i < count; i++) {
        collectible = &this->unk148[i];
        if ((!((this->unk164 >> i) & 1)) && (*collectible != NULL)) {
            if (((*collectible)->unk1A4 != 0) || ((*collectible)->actor.update == NULL)) {
                this->unk164 |= (1 << i);
            } else {
                Actor_Kill(&(*collectible)->actor);
            }
        }
        *collectible = NULL;
    }
}

void func_8098F438(ObjMure3* this, PlayState* play) {
    s16 count = sRupeeCounts[OBJMURE3_PARAM_RUPEEINDEX(&this->actor)];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &this->unk148[i];

        if ((*collectible != NULL) && !((this->unk164 >> i) & 1)) {
            if ((*collectible)->unk1A4 != 0) {
                Flags_SetSwitch(play, OBJMURE3_GET_SWITCH_FLAG(&this->actor));
            }
            if ((*collectible)->actor.update == NULL) {
                this->unk164 |= (1 << i);
                this->unk148[i] = NULL;
            }
        }
    }
}

void ObjMure3_Init(Actor* thisx, PlayState* play) {
    ObjMure3* this = (ObjMure3*)thisx;

    if (Flags_GetSwitch(play, OBJMURE3_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    func_8098F598(this);
}

void ObjMure3_Destroy(Actor* thisx, PlayState* play) {
}

void func_8098F598(ObjMure3* this) {
    this->actionFunc = func_8098F5AC;
}

void func_8098F5AC(ObjMure3* this, PlayState* play) {
    func_8098F5D0(this);
}

void func_8098F5D0(ObjMure3* this) {
    this->actionFunc = func_8098F5E4;
}

void func_8098F5E4(ObjMure3* this, PlayState* play) {
    static ObjMure3SpawnFunc sSpawnFuncs[] = { func_8098F040, func_8098F110, func_8098F220 };

    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) < SQ(1150.0f)) {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        sSpawnFuncs[OBJMURE3_PARAM_RUPEEINDEX(&this->actor)](this, play);
        func_8098F66C(this);
    }
}

void func_8098F66C(ObjMure3* this) {
    this->actionFunc = func_8098F680;
}

void func_8098F680(ObjMure3* this, PlayState* play) {
    func_8098F438(this, play);
    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) >= SQ(1450.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        func_8098F364(this, play);
        func_8098F5D0(this);
    }
}

void ObjMure3_Update(Actor* thisx, PlayState* play) {
    ObjMure3* this = (ObjMure3*)thisx;

    this->actionFunc(this, play);
}

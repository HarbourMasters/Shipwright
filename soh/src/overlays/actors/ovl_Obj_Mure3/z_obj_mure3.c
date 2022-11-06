/*
 * File: z_obj_mure3.c
 * Overlay: ovl_Obj_Mure3
 * Description: Tower of Rupees
 */

#include "z_obj_mure3.h"

#define FLAGS 0

void ObjMure3_Init(Actor* thisx, PlayState* play);
void ObjMure3_Destroy(Actor* thisx, PlayState* play);
void ObjMure3_Update(Actor* thisx, PlayState* play);

void func_80B9AF24(ObjMure3* this);
void func_80B9AF34(ObjMure3* this, PlayState* play);
void func_80B9AF54(ObjMure3* this);
void func_80B9AF64(ObjMure3* this, PlayState* play);
void func_80B9AFEC(ObjMure3* this);
void func_80B9AFFC(ObjMure3* this, PlayState* play);

const ActorInit Obj_Mure3_InitVars = {
    ACTOR_OBJ_MURE3,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(ObjMure3),
    (ActorFunc)ObjMure3_Init,
    (ActorFunc)ObjMure3_Destroy,
    (ActorFunc)ObjMure3_Update,
    NULL,
    NULL,
};

static s16 sRupeeCounts[] = { 5, 5, 7, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 100, ICHAIN_STOP),
};

void func_80B9A9D0(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;

    Math_Vec3f_Copy(&spawnPos, &this->actor.world.pos);
    for (i = 0; i < 5; i++, spawnPos.y += 20.0f) {
        if (!((this->unk_16C >> i) & 1)) {
            this->unk_150[i] = Item_DropCollectible2(play, &spawnPos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
    }
}

void func_80B9AA90(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;
    f32 sn = Math_SinS(this->actor.world.rot.y);
    f32 cos = Math_CosS(this->actor.world.rot.y);
    f32 radius;

    spawnPos.y = this->actor.world.pos.y;

    for (i = 0, radius = -40.0f; i < 5; i++, radius += 20.0f) {
        if (!((this->unk_16C >> i) & 1)) {
            spawnPos.x = this->actor.world.pos.x + (sn * radius);
            spawnPos.z = this->actor.world.pos.z + (cos * radius);
            this->unk_150[i] = Item_DropCollectible2(play, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
    }
}

void func_80B9ABA0(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;
    s16 yRot;

    spawnPos.y = this->actor.world.pos.y;
    yRot = this->actor.world.rot.y;
    for (i = 0; i < 6; i++) {
        if (!((this->unk_16C >> i) & 1)) {
            spawnPos.x = (Math_SinS(yRot) * 40.0f) + this->actor.world.pos.x;
            spawnPos.z = (Math_CosS(yRot) * 40.0f) + this->actor.world.pos.z;
            this->unk_150[i] = Item_DropCollectible2(play, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
        yRot += 0x2AAA;
    }
    if (!((this->unk_16C >> 6) & 1)) {
        spawnPos.x = this->actor.world.pos.x;
        spawnPos.z = this->actor.world.pos.z;
        this->unk_150[6] = Item_DropCollectible2(play, &spawnPos, 0x4000 | ITEM00_RUPEE_RED);
        if (this->unk_150[6] != NULL) {
            this->unk_150[6]->actor.room = this->actor.room;
        }
    }
}

void func_80B9ACE4(ObjMure3* this, PlayState* play) {
    s16 count = sRupeeCounts[(this->actor.params >> 13) & 7];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &this->unk_150[i];

        if (!((this->unk_16C >> i) & 1) && (*collectible != NULL)) {
            if (Actor_HasParent(&(*collectible)->actor, play) || ((*collectible)->actor.update == NULL)) {
                this->unk_16C |= (1 << i);
            } else {
                Actor_Kill(&(*collectible)->actor);
            }
        }
        *collectible = NULL;
    }
}

void func_80B9ADCC(ObjMure3* this, PlayState* play) {
    s16 count = sRupeeCounts[(this->actor.params >> 13) & 7];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &this->unk_150[i];

        if ((*collectible != NULL) && !((this->unk_16C >> i) & 1)) {
            if (Actor_HasParent(&(*collectible)->actor, play)) {
                Flags_SetSwitch(play, this->actor.params & 0x3F);
            }
            if ((*collectible)->actor.update == NULL) {
                this->unk_16C |= (1 << i);
                this->unk_150[i] = NULL;
            }
        }
    }
}

void ObjMure3_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjMure3* this = (ObjMure3*)thisx;

    if (Flags_GetSwitch(play, this->actor.params & 0x3F)) {
        Actor_Kill(&this->actor);
        return;
    }
    Actor_ProcessInitChain(&this->actor, sInitChain);
    func_80B9AF24(this);
}

void ObjMure3_Destroy(Actor* thisx, PlayState* play) {
}

void func_80B9AF24(ObjMure3* this) {
    this->actionFunc = func_80B9AF34;
}

void func_80B9AF34(ObjMure3* this, PlayState* play) {
    func_80B9AF54(this);
}

void func_80B9AF54(ObjMure3* this) {
    this->actionFunc = func_80B9AF64;
}

void func_80B9AF64(ObjMure3* this, PlayState* play) {
    static ObjMure3SpawnFunc spawnFuncs[] = { func_80B9A9D0, func_80B9AA90, func_80B9ABA0 };

    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) < SQ(1150.0f)) {
        this->actor.flags |= ACTOR_FLAG_4;
        spawnFuncs[(this->actor.params >> 13) & 7](this, play);
        func_80B9AFEC(this);
    }
}

void func_80B9AFEC(ObjMure3* this) {
    this->actionFunc = func_80B9AFFC;
}

void func_80B9AFFC(ObjMure3* this, PlayState* play) {
    func_80B9ADCC(this, play);
    if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) >= SQ(1450.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_4;
        func_80B9ACE4(this, play);
        func_80B9AF54(this);
    }
}

void ObjMure3_Update(Actor* thisx, PlayState* play) {
    ObjMure3* this = (ObjMure3*)thisx;

    this->actionFunc(this, play);
}

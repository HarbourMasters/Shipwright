/*
 * File: z_en_encount4.c
 * Overlay: ovl_En_Encount4
 * Description: Spawner for Stalchild and Fire Wall in Keeta chase
 */

#include "z_en_encount4.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnEncount4_Init(Actor* thisx, PlayState* play);
void EnEncount4_Destroy(Actor* thisx, PlayState* play);
void EnEncount4_Update(Actor* thisx, PlayState* play);

void func_809C3FD8(EnEncount4* this, PlayState* play);
void func_809C4078(EnEncount4* this, PlayState* play);
void func_809C42A8(EnEncount4* this, PlayState* play);
void func_809C4598(EnEncount4* this, PlayState* play);
void func_809C464C(EnEncount4* this, PlayState* play);

ActorProfile En_Encount4_Profile = {
    /**/ ACTOR_EN_ENCOUNT4,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnEncount4),
    /**/ EnEncount4_Init,
    /**/ EnEncount4_Destroy,
    /**/ EnEncount4_Update,
    /**/ NULL,
};

s16 D_809C46D0[] = {
    0x4000, 0xC000, 0x4000, 0, 0xC000,
};
f32 D_809C46DC[] = {
    200.0f, 200.0f, 100.0f, 0.0f, 100.0f,
};

void EnEncount4_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnEncount4* this = (EnEncount4*)thisx;

    this->unk_148 = ENCOUNT4_GET_F000(thisx);
    this->switchFlag = ENCOUNT4_GET_SWITCH_FLAG(thisx);
    if (this->switchFlag == ENCOUNT4_SWITCH_FLAG_NONE) {
        this->switchFlag = SWITCH_FLAG_NONE;
    }
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = func_809C3FD8;
}

void EnEncount4_Destroy(Actor* thisx, PlayState* play) {
}

void func_809C3FD8(EnEncount4* this, PlayState* play) {
    Actor* actor;

    this->unk_14E = 0;
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        this->timer = 100;
        this->actionFunc = func_809C464C;
    } else {
        actor = play->actorCtx.actorLists[ACTORCAT_BOSS].first;
        while (actor != NULL) {
            if (actor->id != ACTOR_EN_BSB) {
                actor = actor->next;
                continue;
            }

            this->captainKeeta = (EnBsb*)actor;
            this->actionFunc = func_809C4078;
            break;
        }
    }
}

void func_809C4078(EnEncount4* this, PlayState* play) {
    s16 rotY;
    Vec3f pos;
    s32 fireWallParams;
    s32 i;
    EnBsb* captainKeeta = this->captainKeeta;

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        this->timer = 100;
        this->actionFunc = func_809C464C;
    } else if (BREG(1) == 0) {
        if ((this->captainKeeta->actor.id != ACTOR_EN_BSB) || (captainKeeta->actor.update == NULL)) {
            Actor_Kill(&this->actor);
            return;
        }

        if ((this->unk_148 != 0) || (this->actor.xzDistToPlayer < 240.0f)) {
            if ((this->unk_148 == 0) && captainKeeta->unk_02DC) {
                Actor_Kill(&this->actor);
                return;
            }

            fireWallParams = BGFIREWALL_PARAM_0;
            if ((this->unk_148 == 0) || captainKeeta->unk_02DC) {
                i = 0;
                if (this->unk_148 != 0) {
                    fireWallParams = BGFIREWALL_PARAM_1;
                    i = 2;
                }
                while (i < ARRAY_COUNT(D_809C46DC)) {
                    rotY = D_809C46D0[i] + this->actor.world.rot.y;
                    pos.x = Math_SinS(rotY) * D_809C46DC[i] + this->actor.world.pos.x;
                    pos.y = this->actor.world.pos.y;
                    pos.z = Math_CosS(rotY) * D_809C46DC[i] + this->actor.world.pos.z;
                    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_BG_FIRE_WALL, pos.x, pos.y, pos.z, 0,
                                       this->actor.world.rot.y, 0, fireWallParams);
                    i++;
                }
                this->actionFunc = func_809C42A8;
            }
        }
    }
}

void func_809C42A8(EnEncount4* this, PlayState* play) {
    Player* actor = GET_PLAYER(play);
    EnBsb* captainKeeta = this->captainKeeta;
    Vec3f pos;
    f32 yIntersect;
    s16 yRot;
    CollisionPoly* colPoly;
    s32 bgId;

    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        this->timer = 100;
        this->actionFunc = func_809C464C;

        return;
    }

    if (this->unk_148 == 1) {
        if ((this->captainKeeta->actor.id != ACTOR_EN_BSB) || (captainKeeta->actor.update == NULL)) {
            Actor_Kill(&this->actor);
        }
        return;
    }

    if (this->unk_14E >= 2) {
        this->timer = 100;
        this->actionFunc = func_809C464C;
        return;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_85_40) || (this->unk_14C >= 2) || (this->actor.xzDistToPlayer > 240.0f)) {
        return;
    }

    pos.x = (Math_SinS(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.x;
    pos.y = actor->actor.floorHeight + 120.0f;
    pos.z = (Math_CosS(this->actor.world.rot.y) * 30.0f) + this->actor.world.pos.z;
    yIntersect = BgCheck_EntityRaycastFloor5(&play->colCtx, &colPoly, &bgId, &this->actor, &pos);
    if ((yIntersect <= BGCHECK_Y_MIN) || ((actor->actor.depthInWater != BGCHECK_Y_MIN) &&
                                          (yIntersect < (actor->actor.world.pos.y - actor->actor.depthInWater)))) {
        return;
    }

    pos.y = yIntersect;
    yRot = (s32)Rand_ZeroFloat(0x200) + this->actor.world.rot.y + 0x3800;
    if (this->unk_14C != 0) {
        yRot += 0x8000;
    }
    pos.x += Math_SinS(yRot) * (40.0f + Rand_CenteredFloat(40.0f));
    pos.z += Math_CosS(yRot) * (40.0f + Rand_CenteredFloat(40.0f));
    if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_SKB, pos.x, pos.y, pos.z, 0, 0, 0,
                           ENSKB_PARAM_0) != NULL) {
        this->unk_14C++;
        if (this->unk_14C >= 2) {
            this->actionFunc = func_809C4598;
        }
    }
}

void func_809C4598(EnEncount4* this, PlayState* play) {
    if ((this->switchFlag > SWITCH_FLAG_NONE) && Flags_GetSwitch(play, this->switchFlag)) {
        this->timer = 100;
        this->actionFunc = func_809C464C;
    } else if (this->unk_14E >= 2) {
        this->timer = 100;
        if (this->switchFlag > SWITCH_FLAG_NONE) {
            Flags_SetSwitch(play, this->switchFlag);
        }
        this->actionFunc = func_809C464C;
    } else if (this->unk_14C == 0) {
        this->actionFunc = func_809C42A8;
    }
}

void func_809C464C(EnEncount4* this, PlayState* play) {
    if (this->timer == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnEncount4_Update(Actor* thisx, PlayState* play) {
    EnEncount4* this = (EnEncount4*)thisx;

    DECR(this->timer);
    this->actionFunc(this, play);
}

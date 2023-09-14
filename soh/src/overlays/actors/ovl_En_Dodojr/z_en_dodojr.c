/*
 * File: z_en_dodojr.c
 * Overlay: ovl_En_Dodojr
 * Description: Baby Dodongo
 */

#include "z_en_dodojr.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "objects/object_dodojr/object_dodojr.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE)

void EnDodojr_Init(Actor* thisx, PlayState* play);
void EnDodojr_Destroy(Actor* thisx, PlayState* play);
void EnDodojr_Update(Actor* thisx, PlayState* play);
void EnDodojr_Draw(Actor* thisx, PlayState* play);

void func_809F73AC(EnDodojr* this, PlayState* play);
void func_809F7BE4(EnDodojr* this, PlayState* play);
void func_809F74C4(EnDodojr* this, PlayState* play);
void func_809F758C(EnDodojr* this, PlayState* play);
void func_809F786C(EnDodojr* this, PlayState* play);
void func_809F799C(EnDodojr* this, PlayState* play);
void func_809F78EC(EnDodojr* this, PlayState* play);
void func_809F773C(EnDodojr* this, PlayState* play);
void func_809F77AC(EnDodojr* this, PlayState* play);
void func_809F784C(EnDodojr* this, PlayState* play);
void func_809F7AB8(EnDodojr* this, PlayState* play);
void func_809F7A00(EnDodojr* this, PlayState* play);
void func_809F7B3C(EnDodojr* this, PlayState* play);
void func_809F7C48(EnDodojr* this, PlayState* play);
void func_809F768C(EnDodojr* this, PlayState* play);

const ActorInit En_Dodojr_InitVars = {
    ACTOR_EN_DODOJR,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_DODOJR,
    sizeof(EnDodojr),
    (ActorFunc)EnDodojr_Init,
    (ActorFunc)EnDodojr_Destroy,
    (ActorFunc)EnDodojr_Update,
    (ActorFunc)EnDodojr_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFC5FFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 20, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 1, 2, 25, 25, 0xFF };

void EnDodojr_Init(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 18.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_dodojr_Skel_0020E0, &object_dodojr_Anim_0009D4,
                   this->jointTable, this->morphTable, 15);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(4), &sColChkInit);

    this->actor.naviEnemyId = 0xE;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;

    Actor_SetScale(&this->actor, 0.02f);

    this->actionFunc = func_809F73AC;
}

void EnDodojr_Destroy(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void func_809F64D0(EnDodojr* this) {
    Audio_PlayActorSound2(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
    Actor_SetColorFilter(&this->actor, 0x4000, 200, 0, 8);
}

void func_809F6510(EnDodojr* this, PlayState* play, s32 count) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((Rand_ZeroOne() - 0.5f) * 65536.0f);
    s32 i;

    pos.y = this->dustPos.y;

    for (i = count; i >= 0; i--, angle += (s16)(0x10000 / count)) {
        accel.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
        accel.z = (Rand_ZeroOne() - 0.5f) * 4.0f;

        pos.x = (Math_SinS(angle) * 22.0f) + this->dustPos.x;
        pos.z = (Math_CosS(angle) * 22.0f) + this->dustPos.z;

        func_8002836C(play, &pos, &velocity, &accel, &prim, &env, 120, 40, 10);
    }
}

void func_809F6730(EnDodojr* this, PlayState* play, Vec3f* arg2) {
    Color_RGBA8 prim = { 170, 130, 90, 255 };
    Color_RGBA8 env = { 100, 60, 20, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = ((Rand_ZeroOne() - 0.5f) * 65536.0f);

    pos.y = this->actor.floorHeight;

    accel.x = (Rand_ZeroOne() - 0.5f) * 2;
    accel.z = (Rand_ZeroOne() - 0.5f) * 2;

    pos.x = (Math_SinS(angle) * 11.0f) + arg2->x;
    pos.z = (Math_CosS(angle) * 11.0f) + arg2->z;

    func_8002836C(play, &pos, &velocity, &accel, &prim, &env, 100, 60, 8);
}

s32 func_809F68B0(EnDodojr* this, PlayState* play) {
    if (this->actor.velocity.y >= 0.0f) {
        return 0;
    }

    if (this->unk_1FC == 0) {
        return 0;
    }

    if (this->actor.bgCheckFlags & 1) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
        this->dustPos = this->actor.world.pos;
        func_809F6510(this, play, 10);
        this->actor.velocity.y = 10.0f / (4 - this->unk_1FC);
        this->unk_1FC--;

        if (this->unk_1FC == 0) {
            this->actor.velocity.y = 0.0f;
            return 1;
        }
    }

    return 0;
}

void func_809F6994(EnDodojr* this) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000860);

    Animation_Change(&this->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP, -10.0f);
    this->actor.velocity.y = 0.0f;
    this->actor.speedXZ = 2.6f;
    this->actor.gravity = -0.8f;
}

void func_809F6A20(EnDodojr* this) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_0004A0);

    Animation_Change(&this->skelAnime, &object_dodojr_Anim_0004A0, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -10.0f);
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.gravity = -0.8f;

    if (this->unk_1FC == 0) {
        this->unk_1FC = 3;
        this->actor.velocity.y = 10.0f;
    }
    GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
}

void func_809F6AC4(EnDodojr* this) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_0005F0);

    Animation_Change(&this->skelAnime, &object_dodojr_Anim_0005F0, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, 0.0f);
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -0.8f;
}

void func_809F6B38(EnDodojr* this) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000724);

    Animation_Change(&this->skelAnime, &object_dodojr_Anim_000724, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.gravity = -0.8f;
    this->unk_1FC = 3;
    this->actor.velocity.y = 10.0f;
}

void func_809F6BBC(EnDodojr* this) {
    this->actor.shape.shadowDraw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    this->actor.home.pos = this->actor.world.pos;
    this->actor.speedXZ = 0.0f;
    this->actor.gravity = -0.8f;
    this->timer3 = 30;
    this->dustPos = this->actor.world.pos;
}

void func_809F6C24(EnDodojr* this) {
    Animation_Change(&this->skelAnime, &object_dodojr_Anim_000724, 1.0f, 8.0f, 12.0f, ANIMMODE_ONCE, 0.0f);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_EAT);
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.gravity = -0.8f;
}

s32 func_809F6CA4(EnDodojr* this, PlayState* play) {
    Actor* bomb;
    Vec3f unkVec = { 99999.0f, 99999.0f, 99999.0f };
    s32 retVar = 0;
    f32 xDist;
    f32 yDist;
    f32 zDist;

    bomb = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    this->bomb = NULL;

    while (bomb != NULL) {
        if ((bomb->params != 0) || (bomb->parent != NULL) || (bomb->update == NULL)) {
            bomb = bomb->next;
            continue;
        }

        if (bomb->id != ACTOR_EN_BOM) {
            bomb = bomb->next;
            continue;
        }

        xDist = bomb->world.pos.x - this->actor.world.pos.x;
        yDist = bomb->world.pos.y - this->actor.world.pos.y;
        zDist = bomb->world.pos.z - this->actor.world.pos.z;

        if ((fabsf(xDist) >= fabsf(unkVec.x)) || (fabsf(yDist) >= fabsf(unkVec.y)) ||
            (fabsf(zDist) >= fabsf(unkVec.z))) {
            bomb = bomb->next;
            continue;
        }

        this->bomb = bomb;
        unkVec = bomb->world.pos;
        retVar = 1;
        bomb = bomb->next;
    }

    return retVar;
}

s32 func_809F6DD0(EnDodojr* this) {
    if (this->bomb == NULL) {
        return 0;
    } else if (this->bomb->parent != NULL) {
        return 0;
    } else if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->bomb->world.pos) > 30.0f) {
        return 0;
    } else {
        this->bomb->parent = &this->actor;
        return 1;
    }
}

void func_809F6E54(EnDodojr* this, PlayState* play) {
    f32 angles[] = { 0.0f, 210.0f, 60.0f, 270.0f, 120.0f, 330.0f, 180.0f, 30.0f, 240.0f, 90.0f, 300.0f, 150.0f };
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f pos;
    s16 angleIndex;

    if ((this->bomb == NULL) || (this->bomb->update == NULL) ||
        ((this->bomb != NULL) && (this->bomb->parent != NULL))) {
        func_809F6CA4(this, play);
    }

    if (this->bomb != NULL) {
        pos = this->bomb->world.pos;
    } else {
        pos = player->actor.world.pos;
    }

    if (Math_Vec3f_DistXYZ(&this->actor.world.pos, &pos) > 80.0f) {
        angleIndex = (s16)(this->actor.home.pos.x + this->actor.home.pos.y + this->actor.home.pos.z +
                           play->state.frames / 30) %
                     12;
        angleIndex = ABS(angleIndex);
        pos.x += 80.0f * sinf(angles[angleIndex]);
        pos.z += 80.0f * cosf(angles[angleIndex]);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &pos), 10, 1000, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

s32 func_809F706C(EnDodojr* this) {
    if (this->actor.xzDistToPlayer > 40.0f) {
        return 0;
    } else {
        return 1;
    }
}

void func_809F709C(EnDodojr* this) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
    func_809F6A20(this);
    this->actionFunc = func_809F7AB8;
}

s32 func_809F70E8(EnDodojr* this, PlayState* play) {
    if ((this->actionFunc == func_809F773C) || (this->actionFunc == func_809F77AC) ||
        (this->actionFunc == func_809F784C) || (this->actionFunc == func_809F7A00) ||
        (this->actionFunc == func_809F7AB8) || (this->actionFunc == func_809F7B3C) ||
        (this->actionFunc == func_809F7BE4)) {
        return 0;
    }

    if (play->actorCtx.unk_02 != 0) {
        if (this->actionFunc != func_809F73AC) {
            if (this->actionFunc == func_809F74C4) {
                this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
            }

            func_809F709C(this);
        }
        return 0;
    }

    if (!(this->collider.base.acFlags & 2)) {
        return 0;
    } else {
        this->collider.base.acFlags &= ~2;

        if ((this->actionFunc == func_809F73AC) || (this->actionFunc == func_809F74C4)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        }

        if ((this->actor.colChkInfo.damageEffect == 0) && (this->actor.colChkInfo.damage != 0)) {
            Enemy_StartFinishingBlow(play, &this->actor);
            this->timer2 = 2;
            this->actionFunc = func_809F7C48;
            return 1;
        }

        if ((this->actor.colChkInfo.damageEffect == 1) && (this->actionFunc != func_809F78EC) &&
            (this->actionFunc != func_809F786C)) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            this->timer1 = 120;
            Actor_SetColorFilter(&this->actor, 0, 200, 0, 120);
            func_809F6A20(this);
            this->actionFunc = func_809F786C;
        }

        return 0;
    }
}

void func_809F72A4(EnDodojr* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);

    if ((this->actionFunc != func_809F73AC) && (this->actionFunc != func_809F7BE4)) {
        if ((this->actionFunc == func_809F74C4) || (this->actionFunc == func_809F758C) ||
            (this->actionFunc == func_809F799C)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((this->actionFunc == func_809F74C4) || (this->actionFunc == func_809F758C) ||
            (this->actionFunc == func_809F786C) || (this->actionFunc == func_809F78EC) ||
            (this->actionFunc == func_809F799C)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_809F73AC(EnDodojr* this, PlayState* play) {
    f32 lastFrame = Animation_GetLastFrame(&object_dodojr_Anim_000860);
    Player* player = GET_PLAYER(play);
    f32 dist;

    if (!(this->actor.xzDistToPlayer >= 320.0f)) {
        dist = this->actor.world.pos.y - player->actor.world.pos.y;

        if (!(dist >= 40.0f)) {
            Animation_Change(&this->skelAnime, &object_dodojr_Anim_000860, 1.8f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                             -10.0f);
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_UP);
            this->actor.world.pos.y -= 60.0f;
            this->actor.flags |= ACTOR_FLAG_TARGETABLE;
            this->actor.world.rot.x -= 0x4000;
            this->actor.shape.rot.x = this->actor.world.rot.x;
            this->dustPos = this->actor.world.pos;
            this->dustPos.y = this->actor.floorHeight;
            this->actionFunc = func_809F74C4;
        }
    }
}

void func_809F74C4(EnDodojr* this, PlayState* play) {
    f32 sp2C;

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 4, 0x3E8, 0x64);
    sp2C = this->actor.shape.rot.x;
    sp2C /= 16384.0f;
    this->actor.world.pos.y = this->actor.home.pos.y + (60.0f * sp2C);
    func_809F6510(this, play, 3);

    if (sp2C == 0.0f) {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.world.rot.x = this->actor.shape.rot.x;
        this->actor.speedXZ = 2.6f;
        this->actionFunc = func_809F758C;
    }
}

void func_809F758C(EnDodojr* this, PlayState* play) {
    func_8002D868(&this->actor);
    func_809F6730(this, play, &this->actor.world.pos);

    if (DECR(this->timer4) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_MOVE);
        this->timer4 = 5;
    }

    if (func_809F6DD0(this) != 0) {
        func_809F6C24(this);
        this->actionFunc = func_809F768C;
        return;
    }

    func_809F6E54(this, play);

    if (func_809F706C(this) != 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_CRY);
        func_809F6B38(this);
        this->actionFunc = func_809F799C;
    }

    if (this->actor.bgCheckFlags & 8) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_DOWN);
        func_809F6BBC(this);
        this->actionFunc = func_809F7A00;
    }
}

void func_809F768C(EnDodojr* this, PlayState* play) {
    EnBom* bomb;

    if (((s16)this->skelAnime.curFrame - 8) < 4) {
        bomb = (EnBom*)this->bomb;
        bomb->timer++;
        this->bomb->world.pos = this->headPos;
    } else {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_K_DRINK);
        Actor_Kill(this->bomb);
        this->timer3 = 24;
        this->unk_1FC = 0;
        this->actionFunc = func_809F773C;
    }
}

void func_809F773C(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer3) == 0) {
        func_809F64D0(this);
        this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
        func_809F6A20(this);
        this->actionFunc = func_809F77AC;
    }
}

void func_809F77AC(EnDodojr* this, PlayState* play) {
    this->rootScale = 1.2f;
    this->rootScale *= ((f32)this->actor.colorFilterTimer / 8);
    func_8002D868(&this->actor);

    if (func_809F68B0(this, play) != 0) {
        this->timer3 = 60;
        func_809F6AC4(this);
        this->unk_1FC = 7;
        this->actionFunc = func_809F784C;
    }
}

void func_809F784C(EnDodojr* this, PlayState* play) {
    func_809F7B3C(this, play);
}

void func_809F786C(EnDodojr* this, PlayState* play) {
    func_8002D868(&this->actor);

    if (func_809F68B0(this, play) != 0) {
        func_809F6AC4(this);
        this->actionFunc = func_809F78EC;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, 0, 4, 1000, 10);
    this->actor.world.rot.x = this->actor.shape.rot.x;
    this->actor.colorFilterTimer = this->timer1;
}

void func_809F78EC(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer1) != 0) {
        if (this->timer1 < 30) {
            if ((this->timer1 & 1) != 0) {
                this->actor.world.pos.x += 1.5f;
                this->actor.world.pos.z += 1.5f;
            } else {
                this->actor.world.pos.x -= 1.5f;
                this->actor.world.pos.z -= 1.5f;
            }

            return;
        }
    } else {
        func_809F6994(this);
        this->actionFunc = func_809F758C;
    }
}

void func_809F799C(EnDodojr* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_PLAY_HIT_SFX;
    func_8002D868(&this->actor);

    if (func_809F68B0(this, play) != 0) {
        func_809F6994(this);
        this->actionFunc = func_809F758C;
    }
}

void func_809F7A00(EnDodojr* this, PlayState* play) {
    f32 tmp;

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0x4000, 4, 1000, 100);

    if (DECR(this->timer3) != 0) {
        tmp = (30 - this->timer3) / 30.0f;
        this->actor.world.pos.y = this->actor.home.pos.y - (60.0f * tmp);
    } else {
        Actor_Kill(&this->actor);
    }

    func_809F6510(this, play, 3);
}

void func_809F7AB8(EnDodojr* this, PlayState* play) {
    func_8002D868(&this->actor);
    Math_SmoothStepToS(&this->actor.shape.rot.y, 0, 4, 1000, 10);
    this->actor.world.rot.x = this->actor.shape.rot.x;

    if (func_809F68B0(this, play) != 0) {
        this->timer3 = 60;
        func_809F6AC4(this);
        this->unk_1FC = 7;
        this->actionFunc = func_809F7B3C;
    }
}

void func_809F7B3C(EnDodojr* this, PlayState* play) {
    EnBom* bomb;

    if (this->unk_1FC != 0) {
        if (this->actor.colorFilterTimer == 0) {
            Actor_SetColorFilter(&this->actor, 0x4000, 200, 0, this->unk_1FC);
            this->unk_1FC--;
        }
    } else {
        bomb = (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, BOMB_BODY, true);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        this->timer3 = 8;
        this->actionFunc = func_809F7BE4;
    }
}

void func_809F7BE4(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer3) == 0) {
        Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x40);
        Actor_Kill(&this->actor);
    }
}

void func_809F7C48(EnDodojr* this, PlayState* play) {
    if (DECR(this->timer2) == 0) {
        func_809F709C(this);
    }
}

void EnDodojr_Update(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    SkelAnime_Update(&this->skelAnime);
    Actor_MoveForward(&this->actor);
    func_809F70E8(this, play);

    if (this->actionFunc != func_809F73AC) {
        Actor_UpdateBgCheckInfo(play, &this->actor, this->collider.dim.radius, this->collider.dim.height, 0.0f, 5);
    }

    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 10.0f);
    func_809F72A4(this, play);
}

s32 func_809F7D50(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDodojr* this = (EnDodojr*)thisx;
    Vec3f D_809F7F64 = { 480.0f, 620.0f, 0.0f };

    if (limbIndex == 1) {
        Matrix_Scale((this->rootScale * 0.5f) + 1.0f, this->rootScale + 1.0f, (this->rootScale * 0.5f) + 1.0f,
                     MTXMODE_APPLY);
    }

    if (limbIndex == 4) {
        Matrix_MultVec3f(&D_809F7F64, &this->headPos);
    }

    return false;
}

void func_809F7DFC(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void EnDodojr_Draw(Actor* thisx, PlayState* play) {
    EnDodojr* this = (EnDodojr*)thisx;

    if ((this->actionFunc != func_809F73AC) && (this->actionFunc != func_809F7BE4)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, func_809F7D50, func_809F7DFC,
                          &this->actor);
    }
}

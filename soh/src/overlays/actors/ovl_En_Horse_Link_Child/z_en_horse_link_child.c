/*
 * File: z_en_horse_link_child.c
 * Overlay: ovl_En_Horse_Link_Child
 * Description: Young Epona
 */

#include "z_en_horse_link_child.h"
#include "objects/object_horse_link_child/object_horse_link_child.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void EnHorseLinkChild_Init(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Destroy(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Update(Actor* thisx, PlayState* play);
void EnHorseLinkChild_Draw(Actor* thisx, PlayState* play);

void func_80A69B7C(EnHorseLinkChild* this);
void func_80A69EC0(EnHorseLinkChild* this);
void func_80A6A4DC(EnHorseLinkChild* this);
void func_80A6A724(EnHorseLinkChild* this);

const ActorInit En_Horse_Link_Child_InitVars = {
    ACTOR_EN_HORSE_LINK_CHILD,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HORSE_LINK_CHILD,
    sizeof(EnHorseLinkChild),
    (ActorFunc)EnHorseLinkChild_Init,
    (ActorFunc)EnHorseLinkChild_Destroy,
    (ActorFunc)EnHorseLinkChild_Update,
    (ActorFunc)EnHorseLinkChild_Draw,
    NULL,
};

static AnimationHeader* sAnimations[] = {
    &gChildEponaIdleAnim,     &gChildEponaWhinnyAnim,    &gChildEponaWalkingAnim,
    &gChildEponaTrottingAnim, &gChildEponaGallopingAnim,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementInit,
};

static CollisionCheckInfoInit sColCheckInfoInit = { 10, 35, 100, MASS_HEAVY };

void func_80A693D0(EnHorseLinkChild* this) {
    static s32 D_80A6AF5C[] = { 1, 19 };

    if ((this->skin.skelAnime.curFrame > D_80A6AF5C[this->unk_1F0]) &&
        !((this->unk_1F0 == 0) && (this->skin.skelAnime.curFrame > D_80A6AF5C[1]))) {
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_WALK, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        this->unk_1F0++;
        if (this->unk_1F0 >= ARRAY_COUNT(D_80A6AF5C)) {
            this->unk_1F0 = 0;
        }
    }
}

void func_80A6948C(EnHorseLinkChild* this) {
    if (this->animationIdx == 2) {
        func_80A693D0(this);
    } else if (this->skin.skelAnime.curFrame == 0.0f) {
        if ((this->animationIdx == 3) || (this->animationIdx == 4)) {
            Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_RUN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                   &D_801333E8);
        } else if (this->animationIdx == 1) {
            if (Rand_ZeroOne() > 0.5f) {
                Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_GROAN, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            } else {
                Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }
        }
    }
}

static f32 D_80A6AF64[] = { 1.0f, 1.0f, 1.5f, 1.5f, 1.5f };

f32 func_80A695A4(EnHorseLinkChild* this) {
    f32 result;

    if (this->animationIdx == 2) {
        result = D_80A6AF64[this->animationIdx] * this->actor.speedXZ * (1.0f / 2.0f);
    } else if (this->animationIdx == 3) {
        result = D_80A6AF64[this->animationIdx] * this->actor.speedXZ * (1.0f / 3.0f);
    } else if (this->animationIdx == 4) {
        result = D_80A6AF64[this->animationIdx] * this->actor.speedXZ * (1.0f / 5.0f);
    } else {
        result = D_80A6AF64[this->animationIdx];
    }

    return result;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 1200, ICHAIN_STOP),
};

void EnHorseLinkChild_Init(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Actor_SetScale(&this->actor, 0.005f);
    this->actor.gravity = -3.5f;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawHorse, 20.0f);
    this->actor.speedXZ = 0.0f;
    this->action = 1;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    Skin_Init(play, &this->skin, &gChildEponaSkel, &gChildEponaGallopingAnim);
    this->animationIdx = 0;
    Animation_PlayOnce(&this->skin.skelAnime, sAnimations[0]);
    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinderType1(play, &this->bodyCollider, &this->actor, &sCylinderInit);
    Collider_InitJntSph(play, &this->headCollider);
    Collider_SetJntSph(play, &this->headCollider, &this->actor, &sJntSphInit, this->headElements);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColCheckInfoInit);
    this->unk_1F0 = 0;
    this->eyeTexIndex = 0;

    if (gSaveContext.sceneSetupIndex > 3) {
        func_80A69EC0(this);
    } else if (play->sceneNum == SCENE_LON_LON_RANCH) {
        if (!Flags_GetEventChkInf(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
            Actor_Kill(&this->actor);
            return;
        }
        this->unk_2A0 = Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
        func_80A69EC0(this);
    } else {
        func_80A69EC0(this);
    }

    this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
}

void EnHorseLinkChild_Destroy(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Skin_Free(play, &this->skin);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyJntSph(play, &this->headCollider);
}

void func_80A6988C(EnHorseLinkChild* this) {
    this->action = 0;
    this->animationIdx++;
    if (this->animationIdx >= ARRAY_COUNT(sAnimations)) {
        this->animationIdx = 0;
    }

    Animation_PlayOnce(&this->skin.skelAnime, sAnimations[this->animationIdx]);
    this->skin.skelAnime.playSpeed = func_80A695A4(this);
}

void func_80A698F4(EnHorseLinkChild* this, PlayState* play) {
    this->actor.speedXZ = 0.0f;
    if (SkelAnime_Update(&this->skin.skelAnime)) {
        func_80A6988C(this);
    }
}

void func_80A6993C(EnHorseLinkChild* this, s32 newAnimationIdx) {
    this->action = 2;
    this->actor.speedXZ = 0.0f;

    if (!((newAnimationIdx == 0) || (newAnimationIdx == 1))) {
        newAnimationIdx = 0;
    }

    if (this->animationIdx != newAnimationIdx) {
        this->animationIdx = newAnimationIdx;
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
    }
}

void func_80A699FC(EnHorseLinkChild* this, PlayState* play) {
    f32 distFromLink;
    s32 newAnimationIdx;

    distFromLink = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if ((distFromLink < 1000.0f) && (distFromLink > 70.0f)) {
            func_80A69B7C(this);
        } else {
            newAnimationIdx = this->animationIdx == 1 ? 0 : 1;
            if (this->animationIdx != newAnimationIdx) {
                this->animationIdx = newAnimationIdx;
                Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                                 Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                                 Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0);
            }
        }
    }
}

void func_80A69B7C(EnHorseLinkChild* this) {
    this->action = 1;
    this->animationIdx = 0;
    this->actor.speedXZ = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A69C18(EnHorseLinkChild* this, PlayState* play) {
    s16 yawDiff;
    f32 distFromLink;
    s32 newAnimationIdx;

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        yawDiff = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

        if (yawDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            this->actor.world.rot.y -= 0x12C;
        } else {
            this->actor.world.rot.y += yawDiff;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        distFromLink = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);

        if (distFromLink > 1000.0f) {
            func_80A6993C(this, 0);
            return;
        } else if ((distFromLink < 1000.0f) && (distFromLink >= 300.0f)) {
            newAnimationIdx = 4;
            this->actor.speedXZ = 6.0f;
        } else if ((distFromLink < 300.0f) && (distFromLink >= 150.0f)) {
            newAnimationIdx = 3;
            this->actor.speedXZ = 4.0f;
        } else if ((distFromLink < 150.0f) && (distFromLink >= 70.0f)) {
            newAnimationIdx = 2;
            this->actor.speedXZ = 2.0f;
            this->unk_1F0 = 0;
        } else {
            func_80A6993C(this, 1);
            return;
        }

        if (this->animationIdx != newAnimationIdx) {
            this->animationIdx = newAnimationIdx;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                             Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                             Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void func_80A69EC0(EnHorseLinkChild* this) {
    this->action = 3;
    this->animationIdx = 0;
    this->actor.speedXZ = 0.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A69F5C(EnHorseLinkChild* this, PlayState* play) {
    Player* player;
    s16 yawDiff;
    s32 yawSign;
    s32 yawOffset;

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        player = GET_PLAYER(play);

        if (Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos) < 250.0f) {
            yawDiff = player->actor.shape.rot.y;
            yawSign = Actor_WorldYawTowardActor(&this->actor, &player->actor) > 0 ? 1 : -1;
            yawOffset = yawSign << 0xE;
            yawDiff += yawOffset;
        } else {
            yawDiff = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (yawDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            this->actor.world.rot.y -= 0x12C;
        } else {
            this->actor.world.rot.y += yawDiff;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

void func_80A6A068(EnHorseLinkChild* this, PlayState* play) {
    Player* player;
    f32 distFromLink;
    s32 animationEnded;
    s32 newAnimationIdx;
    f32 distFromHome;
    f32 distLinkFromHome;

    func_80A69F5C(this, play);
    player = GET_PLAYER(play);
    distFromLink = Actor_WorldDistXZToActor(&this->actor, &player->actor);

    if (gSaveContext.entranceIndex == 0x2AE) {
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_80A6A724(this);
        return;
    }

    if (((Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON)) && (DREG(53) != 0)) ||
        ((play->sceneNum == SCENE_LON_LON_RANCH) && (gSaveContext.cutsceneIndex == 0xFFF1))) {
        func_80A6A4DC(this);
    } else {
        this->unk_2A0 = Flags_GetEventChkInf(EVENTCHKINF_INVITED_TO_SING_WITH_CHILD_MALON);
    }

    newAnimationIdx = this->animationIdx;
    animationEnded = SkelAnime_Update(&this->skin.skelAnime);
    if (animationEnded || (this->animationIdx == 1) || (this->animationIdx == 0)) {
        if (Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_CHILD_MALON_AT_RANCH)) {
            distFromHome = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos);
            distLinkFromHome = Math3D_Vec3f_DistXYZ(&player->actor.world.pos, &this->actor.home.pos);
            if (distLinkFromHome > 250.0f) {
                if (distFromHome >= 300.0f) {
                    newAnimationIdx = 4;
                    this->actor.speedXZ = 6.0f;
                } else if ((distFromHome < 300.0f) && (distFromHome >= 150.0f)) {
                    newAnimationIdx = 3;
                    this->actor.speedXZ = 4.0f;
                } else if ((distFromHome < 150.0f) && (distFromHome >= 70.0f)) {
                    newAnimationIdx = 2;
                    this->actor.speedXZ = 2.0f;
                    this->unk_1F0 = 0;
                } else {
                    this->actor.speedXZ = 0.0f;
                    if (this->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            } else {
                if (distFromLink < 200.0f) {
                    newAnimationIdx = 4;
                    this->actor.speedXZ = 6.0f;
                } else if (distFromLink < 300.0f) {
                    newAnimationIdx = 3;
                    this->actor.speedXZ = 4.0f;
                } else if (distFromLink < 400.0f) {
                    newAnimationIdx = 2;
                    this->actor.speedXZ = 2.0f;
                    this->unk_1F0 = 0;
                } else {
                    this->actor.speedXZ = 0.0f;
                    if (this->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            }
        } else {
            this->actor.speedXZ = 0.0f;
            if (this->animationIdx == 0) {
                newAnimationIdx = animationEnded == true ? 1 : 0;
            } else {
                newAnimationIdx = animationEnded == true ? 0 : 1;
            }
        }
    }

    if ((this->animationIdx != newAnimationIdx) || (animationEnded == true)) {
        this->animationIdx = newAnimationIdx;
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                         Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
    } else {
        Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this),
                         this->skin.skelAnime.curFrame, Animation_GetLastFrame(sAnimations[this->animationIdx]), 2,
                         0.0f);
    }
}

void func_80A6A4DC(EnHorseLinkChild* this) {
    this->action = 5;
    this->animationIdx = Rand_ZeroOne() > 0.5f ? 0 : 1;
    DREG(53) = 0;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void func_80A6A5A4(EnHorseLinkChild* this, PlayState* play) {
    s16 yawDiff;

    if (DREG(53) != 0) {
        DREG(53) = 0;
        Audio_PlaySoundGeneral(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                               &D_801333E8);
        func_80A6A724(this);
    } else {
        this->actor.speedXZ = 0.0f;
        yawDiff = Actor_WorldYawTowardActor(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
        // 0.7071 = cos(pi/4)
        if ((Math_CosS(yawDiff) < 0.7071f) && (this->animationIdx == 2)) {
            func_8006DD9C(&this->actor, &GET_PLAYER(play)->actor.world.pos, 300);
        }

        if (SkelAnime_Update(&this->skin.skelAnime)) {
            if (Math_CosS(yawDiff) < 0.0f) {
                this->animationIdx = 2;
                Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], D_80A6AF64[this->animationIdx],
                                 0.0f, Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                func_80A6A4DC(this);
            }
        }
    }
}

void func_80A6A724(EnHorseLinkChild* this) {
    this->timer = 0;
    this->action = 4;
    this->animationIdx = 2;
    this->unk_1E8 = false;
    this->actor.speedXZ = 2.0f;
    Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                     Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void func_80A6A7D0(EnHorseLinkChild* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;
    s32 newAnimationIdx;

    this->timer++;
    if (this->timer > 300) {
        this->unk_1E8 = true;
    }

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        if (!this->unk_1E8) {
            func_8006DD9C(&this->actor, &player->actor.world.pos, 300);
        } else {
            func_8006DD9C(&this->actor, &this->actor.home.pos, 300);
        }
    }

    if (SkelAnime_Update(&this->skin.skelAnime)) {
        if (!this->unk_1E8) {
            dist = Actor_WorldDistXZToActor(&this->actor, &GET_PLAYER(play)->actor);
        } else {
            dist = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.home.pos);
        }

        if (!this->unk_1E8) {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                this->actor.speedXZ = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                this->actor.speedXZ = 4.0f;
            } else {
                newAnimationIdx = 2;
                this->actor.speedXZ = 2.0f;
                this->unk_1F0 = 0;
            }
        } else {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                this->actor.speedXZ = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                this->actor.speedXZ = 4.0f;
            } else if (dist >= 70.0f) {
                newAnimationIdx = 2;
                this->actor.speedXZ = 2.0f;
                this->unk_1F0 = 0;
            } else {
                func_80A6A4DC(this);
                return;
            }
        }

        if (this->animationIdx != newAnimationIdx) {
            this->animationIdx = newAnimationIdx;
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                             Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Animation_Change(&this->skin.skelAnime, sAnimations[this->animationIdx], func_80A695A4(this), 0.0f,
                             Animation_GetLastFrame(sAnimations[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

static EnHorseLinkChildActionFunc sActionFuncs[] = {
    func_80A698F4, func_80A69C18, func_80A699FC, func_80A6A068, func_80A6A7D0, func_80A6A5A4,
};

static void* sEyeTextures[] = { gChildEponaEyeOpenTex, gChildEponaEyeHalfTex, gChildEponaEyeCloseTex };
static u8 sEyeIndexOrder[] = { 0, 1, 2, 1 };

void EnHorseLinkChild_Update(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 pad;

    sActionFuncs[this->action](this, play);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 55.0f, 100.0f, 0x1D);

    if ((play->sceneNum == SCENE_LON_LON_RANCH) && (this->actor.world.pos.z < -2400.0f)) {
        this->actor.world.pos.z = -2400.0f;
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;

    if ((Rand_ZeroOne() < 0.025f) && (this->eyeTexIndex == 0)) {
        this->eyeTexIndex++;
    } else if (this->eyeTexIndex > 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= ARRAY_COUNT(sEyeIndexOrder)) {
            this->eyeTexIndex = 0;
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
    func_80A6948C(this);
}

void EnHorseLinkChild_PostDraw(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f center;
    Vec3f newCenter;
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 i;

    for (i = 0; i < this->headCollider.count; i++) {
        center.x = this->headCollider.elements[i].dim.modelSphere.center.x;
        center.y = this->headCollider.elements[i].dim.modelSphere.center.y;
        center.z = this->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_GetLimbPos(skin, this->headCollider.elements[i].dim.limb, &center, &newCenter);
        this->headCollider.elements[i].dim.worldSphere.center.x = newCenter.x;
        this->headCollider.elements[i].dim.worldSphere.center.y = newCenter.y;
        this->headCollider.elements[i].dim.worldSphere.center.z = newCenter.z;
        this->headCollider.elements[i].dim.worldSphere.radius =
            this->headCollider.elements[i].dim.modelSphere.radius * this->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->headCollider.base);
}

s32 EnHorseLinkChild_OverrideLimbDraw(Actor* thisx, PlayState* play, s32 arg2, Skin* skin) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (arg2 == 0xD) {
        u8 index = sEyeIndexOrder[this->eyeTexIndex];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[index]));
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return 1;
}

void EnHorseLinkChild_Draw(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    func_800A6360(&this->actor, play, &this->skin, EnHorseLinkChild_PostDraw, EnHorseLinkChild_OverrideLimbDraw,
                  true);
}

/*
 * File: z_en_tanron1.c
 * Overlay: ovl_En_Tanron1
 * Description: Swarm of Moths in Woodfall Temple
 */

#include "z_en_tanron1.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnTanron1_Init(Actor* thisx, PlayState* play);
void EnTanron1_Destroy(Actor* thisx, PlayState* play);
void EnTanron1_Update(Actor* thisx, PlayState* play);
void EnTanron1_Draw(Actor* thisx, PlayState* play);

void func_80BB5318(EnTanron1* this, PlayState* play);
void func_80BB5AAC(EnTanron1* this, PlayState* play);

ActorProfile En_Tanron1_Profile = {
    /**/ ACTOR_EN_TANRON1,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTanron1),
    /**/ EnTanron1_Init,
    /**/ EnTanron1_Destroy,
    /**/ EnTanron1_Update,
    /**/ EnTanron1_Draw,
};

static s32 sPad = 0;

#include "overlays/ovl_En_Tanron1/ovl_En_Tanron1.h"

void EnTanron1_Init(Actor* thisx, PlayState* play) {
    EnTanron1* this = (EnTanron1*)thisx;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (!ENTANRON1_GET_100(&this->actor)) {
        this->unk_144 = 0;
    } else {
        this->actor.params = 200;
        this->unk_144 = 1;
    }
}

void EnTanron1_Destroy(Actor* thisx, PlayState* play) {
}

void func_80BB4E50(EnTanron1Struct* arg0, Vec3f* arg1, s16 arg2) {
    s16 i;

    for (i = 0; i < 200; i++, arg0++) {
        if (arg0->unk_24 == 0) {
            arg0->unk_24 = 1;
            arg0->unk_2C = 0.0f;
            arg0->unk_00 = *arg1;
            arg0->unk_18 = arg0->unk_1C = 0;
            arg0->unk_1A = arg2;
            arg0->unk_26 = Rand_ZeroFloat(100.0f);
            arg0->unk_28 = 0;
            if (Rand_ZeroOne() < 0.5f) {
                arg0->unk_2A = 0xC00;
            } else {
                arg0->unk_2A = -0xC00;
            }
            break;
        }
    }
}

void EnTanron1_Update(Actor* thisx, PlayState* play) {
    EnTanron1* this = (EnTanron1*)thisx;
    Actor* temp_a0;
    Player* player = GET_PLAYER(play);
    s16 i;
    EnTanron1Struct* ptr = &this->unk_160[0];
    Vec3f temp;
    f32 phi_f18;
    s32 pad;

    if (this->unk_148 != 0) {
        this->unk_148--;
    }

    switch (this->unk_144) {
        case 0:
            for (i = 0; i < this->actor.params; i++) {
                func_80BB4E50(this->unk_160, &this->actor.world.pos, Rand_ZeroFloat(0x10000));
            }
            this->unk_144 = 200;
            break;

        case 1:
            this->unk_14C.x = player->actor.world.pos.x;
            this->unk_14C.y = player->actor.world.pos.y + 60.0f;
            this->unk_14C.z = player->actor.world.pos.z;
            if (this->unk_148 != 0) {
                func_80BB4E50(this->unk_160, &this->actor.world.pos, this->actor.world.rot.y);
            }
            this->unk_158 = 0x1000;
            this->unk_15C = 150.0f;
            break;

        case 100:
            phi_f18 = 10.0f;

            while (phi_f18 < 1000.0f) {
                temp_a0 = play->actorCtx.actorLists[ACTORCAT_PROP].first;
                while (temp_a0 != NULL) {
                    if (temp_a0->id != ACTOR_OBJ_SYOKUDAI) {
                        temp_a0 = temp_a0->next;
                        continue;
                    }

                    temp.x = this->unk_14C.x - temp_a0->world.pos.x;
                    temp.y = this->unk_14C.y - temp_a0->world.pos.y + 70.0f;
                    temp.z = this->unk_14C.z - temp_a0->world.pos.z;

                    if (sqrtf(SQXYZ(temp)) < phi_f18) {
                        this->unk_14C.x = temp_a0->world.pos.x;
                        this->unk_14C.y = temp_a0->world.pos.y + 70.0f;
                        this->unk_14C.z = temp_a0->world.pos.z;
                        goto end;
                    }
                    temp_a0 = temp_a0->next;
                }
                phi_f18 += 20.0f;
            }
        end:
            break;

        case 200:
            this->unk_14C = this->actor.world.pos;
            this->unk_158 = 0x5000;
            this->unk_15C = 50.0f;
            break;

        case 250:
            for (i = 0; i < ARRAY_COUNT(this->unk_160); i++, ptr++) {
                if ((ptr->unk_24 != 0) && (ptr->unk_28 < 8)) {
                    ptr->unk_28 = 8;
                    ptr->unk_24 = 2;
                }
            }
            this->unk_144 = 1;
            break;

        default:
            break;
    }

    if ((player->heldItemAction == PLAYER_IA_DEKU_STICK) && (player->unk_B28 != 0)) {
        this->unk_14C.x = player->meleeWeaponInfo[0].tip.x;
        this->unk_14C.y = player->meleeWeaponInfo[0].tip.y;
        this->unk_14C.z = player->meleeWeaponInfo[0].tip.z;

        this->unk_158 = 0x5000;
        this->unk_15C = 50.0f;
        if (this->unk_144 != 1) {
            this->unk_144 = 100;
        }
    } else {
        temp_a0 = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
        while (temp_a0 != NULL) {
            if (temp_a0->params == 1) {
                temp_a0 = temp_a0->next;
                continue;
            }

            this->unk_14C.x = temp_a0->world.pos.x;
            this->unk_14C.y = temp_a0->world.pos.y + 50.0f;
            this->unk_14C.z = temp_a0->world.pos.z;

            this->unk_15C = 150.0f;
            this->unk_158 = 0x1000;
            if (this->unk_144 != 1) {
                this->unk_144 = 100;
            }
            break;
        }
    }
    func_80BB5318(this, play);
}

void EnTanron1_Draw(Actor* thisx, PlayState* play) {
    EnTanron1* this = (EnTanron1*)thisx;

    func_80BB5AAC(this, play);
}

void func_80BB5318(EnTanron1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnTanron1Struct* ptr = NULL;
    f32 phi_f28 = 0.0f;
    Vec3f* phi_s2 = NULL;
    Vec3f temp;
    s16 spBA = 0;
    s16 spB8 = 0;
    Vec3f* spB4 = NULL;
    f32 spB0;
    Vec3f spA4;
    WaterBox* waterBox;
    f32 sp9C;
    CollisionPoly* sp98;
    Actor* temp_v0;
    s16 i;
    f32 temp_f30 = this->unk_15C;

    if (player->unk_ADC != 0) {
        phi_s2 = &player->meleeWeaponInfo[0].tip;
        if (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            phi_f28 = 2500.0f;
        } else {
            phi_f28 = 400.0f;
        }
    }

    temp_v0 = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
    while (temp_v0 != NULL) {
        if (temp_v0->params != 1) {
            temp_v0 = temp_v0->next;
            continue;
        }
        phi_s2 = &temp_v0->world.pos;
        phi_f28 = 40000.0f;
        break;
    }

    ptr = &this->unk_160[0];
    for (i = 0; i < this->actor.params; i++, ptr++) {
        if (ptr->unk_24 != 0) {
            ptr->unk_26++;
            ptr->unk_00.x += ptr->unk_0C.x;
            ptr->unk_00.y += ptr->unk_0C.y;
            ptr->unk_00.z += ptr->unk_0C.z;

            if (ptr->unk_24 == 0) {
            } else {
                spB4 = &ptr->unk_00;
                if (ptr->unk_28 == 0) {
                    spBA++;
                    ptr->unk_2C = Math_SinS(ptr->unk_26 * 0x5000) * 1.2f;
                    if ((ptr->unk_26 & 3) == 0) {
                        temp.x = ptr->unk_30 + (this->unk_14C.x - ptr->unk_00.x);
                        temp.y = ptr->unk_34 + (this->unk_14C.y - ptr->unk_00.y);
                        temp.z = ptr->unk_38 + (this->unk_14C.z - ptr->unk_00.z);

                        ptr->unk_20 = Math_Atan2S(temp.x, temp.z);
                        ptr->unk_1E = Math_Atan2S(temp.y, sqrtf(SQXZ(temp)));
                        if ((ptr->unk_26 & 0xF) == 0) {
                            ptr->unk_30 = Rand_CenteredFloat(temp_f30);
                            ptr->unk_34 = Rand_CenteredFloat(temp_f30 * 0.5f);
                            ptr->unk_38 = Rand_CenteredFloat(temp_f30);
                        }

                        temp.x = player->actor.world.pos.x - ptr->unk_00.x;
                        temp.y = (player->actor.world.pos.y + 40.0f) - ptr->unk_00.y;
                        temp.z = player->actor.world.pos.z - ptr->unk_00.z;

                        if ((SQXYZ(temp) < 400.0f) && (player->transformation != PLAYER_FORM_DEKU)) {
                            func_800B8D10(play, &this->actor, 0.0f, 0, 0.0f, 1, 1);
                        }
                    }

                    Math_ApproachS(&ptr->unk_1A, ptr->unk_20, 2, this->unk_158);
                    Math_ApproachS(&ptr->unk_18, ptr->unk_1E, 2, this->unk_158);
                    Matrix_RotateYS(ptr->unk_1A, MTXMODE_NEW);
                    Matrix_RotateXS(-ptr->unk_18, MTXMODE_APPLY);
                    Matrix_MultVecZ(6.0f, &ptr->unk_0C);

                    if (phi_s2 != NULL) {
                        temp.x = phi_s2->x - ptr->unk_00.x;
                        temp.y = phi_s2->y - ptr->unk_00.y;
                        temp.z = phi_s2->z - ptr->unk_00.z;

                        if (SQXYZ(temp) < phi_f28) {
                            ptr->unk_20 = Math_Atan2S(temp.x, temp.z);
                            ptr->unk_1E = Math_Atan2S(temp.y, sqrtf(SQXZ(temp)));

                            Matrix_RotateYS(ptr->unk_20, MTXMODE_NEW);
                            Matrix_RotateXS(-ptr->unk_1E, MTXMODE_APPLY);
                            Matrix_MultVecZ(-20.0f, &ptr->unk_0C);

                            if (phi_f28 >= 100000.0f) {
                                ptr->unk_28 = 1;
                            } else {
                                ptr->unk_28 = 6;
                            }
                            ptr->unk_24 = 2;
                            spB8++;
                        }
                    }
                } else if (ptr->unk_28 < 9) {
                    ptr->unk_18 += 0x3000;
                    ptr->unk_1A += 0x5000;
                    ptr->unk_30 = 0.0f;
                    ptr->unk_34 = 0.0f;
                    ptr->unk_28++;
                } else {
                    ptr->unk_1A += ptr->unk_2A;
                    Math_ApproachS(&ptr->unk_18, 0, 0xA, 0x1000);
                    Matrix_RotateYS(ptr->unk_1A, MTXMODE_NEW);
                    Matrix_MultVecZ(ptr->unk_30, &spA4);

                    ptr->unk_0C.x = spA4.x;
                    ptr->unk_0C.z = spA4.z;
                    ptr->unk_0C.y = -2.0f;

                    if (phi_s2 != NULL) {
                        temp.x = phi_s2->x - ptr->unk_00.x;
                        temp.y = phi_s2->y - ptr->unk_00.y;
                        temp.z = phi_s2->z - ptr->unk_00.z;

                        if (SQXYZ(temp) < phi_f28) {
                            ptr->unk_20 = Math_Atan2S(temp.x, temp.z);
                            ptr->unk_1E = Math_Atan2S(temp.y, sqrtf(SQXZ(temp)));

                            Matrix_RotateYS(ptr->unk_20, MTXMODE_NEW);
                            Matrix_RotateXS(-ptr->unk_1E, MTXMODE_APPLY);
                            Matrix_MultVecZ(-20.0f, &ptr->unk_0C);

                            ptr->unk_3C = ptr->unk_00.y - 1000.0f;
                            ptr->unk_30 = 5.0f;
                        }
                    }

                    if (ptr->unk_00.y <= (ptr->unk_3C + 5.0f)) {
                        ptr->unk_00.y = (ptr->unk_3C + 5.0f);
                        Math_ApproachZeroF(&ptr->unk_30, 1.0f, 0.3f);
                        Math_ApproachS(&ptr->unk_2A, 0, 1, 0x100);
                        ptr->unk_28++;
                        if (ptr->unk_28 > 50) {
                            ptr->unk_24 = 0;
                        }
                    } else {
                        Math_ApproachF(&ptr->unk_30, ptr->unk_34, 1.0f, 0.5f);
                        if ((ptr->unk_26 & 0xF) == 0) {
                            if (Rand_ZeroOne() < 0.5f) {
                                ptr->unk_34 = Rand_CenteredFloat(12.0f);
                            }
                            ptr->unk_3C = BgCheck_EntityRaycastFloor1(&play->colCtx, &sp98, &ptr->unk_00);
                            sp9C = ptr->unk_00.y;
                            WaterBox_GetSurface1(play, &play->colCtx, ptr->unk_00.x, ptr->unk_00.z, &sp9C, &waterBox);
                            if ((sp9C < ptr->unk_00.y) && (ptr->unk_3C < sp9C)) {
                                ptr->unk_3C = sp9C;
                            }
                        }
                    }
                }
            }
        }
    }

    if (spB4 != NULL) {
        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, spB4, &this->unk_3360, &spB0);
        if (spB8 >= (s16)(KREG(39) + 20)) {
            Audio_PlaySfx_AtPos(&this->unk_3360, NA_SE_EN_MB_MOTH_DEAD);
        } else if (spBA >= 20) {
            Audio_PlaySfx_AtPos(&this->unk_3360, NA_SE_EN_MB_MOTH_FLY - SFX_FLAG);
        }
    }
}

void func_80BB5AAC(EnTanron1* this, PlayState* play) {
    EnTanron1Struct* ptrBase = &this->unk_160[0];
    s16 i;
    u8 flag = 0;
    EnTanron1Struct* ptr = ptrBase;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < this->actor.params; i++, ptr++) {
        if (ptr->unk_24 == 1) {
            FrameInterpolation_RecordOpenChild(ptr, ptr->unk_24);
            if (!flag) {
                gSPDisplayList(POLY_OPA_DISP++, ovl_En_Tanron1_DL_001888);
                flag++;
            }
            Matrix_Translate(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, MTXMODE_NEW);
            Matrix_RotateYS(ptr->unk_1A, MTXMODE_APPLY);
            Matrix_RotateXS(ptr->unk_18 * -1, MTXMODE_APPLY);
            Matrix_Scale(1.2f, ptr->unk_2C, 1.2f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, ovl_En_Tanron1_DL_001900);
            FrameInterpolation_RecordCloseChild();
        }
    }

    flag = 0;
    ptr = ptrBase;
    for (i = 0; i < this->actor.params; i++, ptr++) {
        if (ptr->unk_24 == 2) {
            FrameInterpolation_RecordOpenChild(ptr, ptr->unk_24);
            if (!flag) {
                gSPDisplayList(POLY_OPA_DISP++, ovl_En_Tanron1_DL_001888);
                gDPLoadTextureBlock(POLY_OPA_DISP++, ovl_En_Tanron1_DL_001428, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 32, 0,
                                    G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 4, 5, G_TX_NOLOD,
                                    G_TX_NOLOD);
                flag++;
            }

            Matrix_Translate(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, MTXMODE_NEW);
            Matrix_RotateYS(ptr->unk_1A, MTXMODE_APPLY);
            Matrix_RotateXS(ptr->unk_18 * -1, MTXMODE_APPLY);
            Matrix_Scale(1.0f, ptr->unk_2C, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, ovl_En_Tanron1_DL_001900);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

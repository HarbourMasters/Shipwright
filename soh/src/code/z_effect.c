#include "global.h"

EffectContext sEffectContext;

EffectInfo sEffectInfoTable[] = {
    {
        sizeof(EffectSpark),
        EffectSpark_Init,
        EffectSpark_Destroy,
        EffectSpark_Update,
        EffectSpark_Draw,
    },
    {
        sizeof(EffectBlure),
        EffectBlure_Init1,
        EffectBlure_Destroy,
        EffectBlure_Update,
        EffectBlure_Draw,
    },
    {
        sizeof(EffectBlure),
        EffectBlure_Init2,
        EffectBlure_Destroy,
        EffectBlure_Update,
        EffectBlure_Draw,
    },
    {
        sizeof(EffectShieldParticle),
        EffectShieldParticle_Init,
        EffectShieldParticle_Destroy,
        EffectShieldParticle_Update,
        EffectShieldParticle_Draw,
    },
};

PlayState* Effect_GetPlayState(void) {
    return sEffectContext.play;
}

void* Effect_GetByIndex(s32 index) {
    if (index == TOTAL_EFFECT_COUNT) {
        return NULL;
    }

    if (index < SPARK_COUNT) {
        if (sEffectContext.sparks[index].status.active == true) {
            return &sEffectContext.sparks[index].effect;
        } else {
            return NULL;
        }
    }

    index -= SPARK_COUNT;
    if (index < BLURE_COUNT) {
        if (sEffectContext.blures[index].status.active == true) {
            return &sEffectContext.blures[index].effect;
        } else {
            return NULL;
        }
    }

    index -= BLURE_COUNT;
    if (index < SHIELD_PARTICLE_COUNT) {
        if (sEffectContext.shieldParticles[index].status.active == true) {
            return &sEffectContext.shieldParticles[index].effect;
        } else {
            return NULL;
        }
    }

    return NULL;
}

void Effect_InitStatus(EffectStatus* status) {
    status->active = false;
    status->unk_01 = 0;
    status->unk_02 = 0;
}

void Effect_InitContext(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        Effect_InitStatus(&sEffectContext.sparks[i].status);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        Effect_InitStatus(&sEffectContext.blures[i].status);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        //! @bug This is supposed to initialize shieldParticles, not blures again
        Effect_InitStatus(&sEffectContext.blures[i].status);
    }

    sEffectContext.play = play;
}

void Effect_Add(PlayState* play, s32* pIndex, s32 type, u8 arg3, u8 arg4, void* initParams) {
    s32 i;
    u32 slotFound;
    void* effect = NULL;
    EffectStatus* status = NULL;

    *pIndex = TOTAL_EFFECT_COUNT;

    if (FrameAdvance_IsEnabled(play) != true) {
        slotFound = false;
        switch (type) {
            case EFFECT_SPARK:
                for (i = 0; i < SPARK_COUNT; i++) {
                    if (sEffectContext.sparks[i].status.active == false) {
                        slotFound = true;
                        *pIndex = i;
                        effect = &sEffectContext.sparks[i].effect;
                        status = &sEffectContext.sparks[i].status;
                        break;
                    }
                }
                break;
            case EFFECT_BLURE1:
            case EFFECT_BLURE2:
                for (i = 0; i < BLURE_COUNT; i++) {
                    if (sEffectContext.blures[i].status.active == false) {
                        slotFound = true;
                        *pIndex = i + SPARK_COUNT;
                        effect = &sEffectContext.blures[i].effect;
                        status = &sEffectContext.blures[i].status;
                        break;
                    }
                }
                break;
            case EFFECT_SHIELD_PARTICLE:
                for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
                    if (sEffectContext.shieldParticles[i].status.active == false) {
                        slotFound = true;
                        *pIndex = i + SPARK_COUNT + BLURE_COUNT;
                        effect = &sEffectContext.shieldParticles[i].effect;
                        status = &sEffectContext.shieldParticles[i].status;
                        break;
                    }
                }
                break;
        }

        if (!slotFound) {
            // "EffectAdd(): I cannot secure it. Be careful. Type %d"
            osSyncPrintf("EffectAdd():確保できません。注意してください。Type%d\n", type);
            osSyncPrintf("エフェクト追加せずに終了します。\n"); // "Exit without adding the effect."
        } else {
            sEffectInfoTable[type].init(effect, initParams);
            status->unk_02 = arg3;
            status->unk_01 = arg4;
            status->active = true;
        }
    }
}

void Effect_DrawAll(GraphicsContext* gfxCtx) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (sEffectContext.sparks[i].status.active) {
            sEffectInfoTable[EFFECT_SPARK].draw(&sEffectContext.sparks[i].effect, gfxCtx);
        }
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (sEffectContext.blures[i].status.active) {
            sEffectInfoTable[EFFECT_BLURE1].draw(&sEffectContext.blures[i].effect, gfxCtx);
        }
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (sEffectContext.shieldParticles[i].status.active) {
            sEffectInfoTable[EFFECT_SHIELD_PARTICLE].draw(&sEffectContext.shieldParticles[i].effect, gfxCtx);
        }
    }
}

void Effect_UpdateAll(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (sEffectContext.sparks[i].status.active) {
            if (sEffectInfoTable[EFFECT_SPARK].update(&sEffectContext.sparks[i].effect) == 1) {
                Effect_Delete(play, i);
            }
        }
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (sEffectContext.blures[i].status.active) {
            if (sEffectInfoTable[EFFECT_BLURE1].update(&sEffectContext.blures[i].effect) == 1) {
                Effect_Delete(play, i + SPARK_COUNT);
            }
        }
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (sEffectContext.shieldParticles[i].status.active) {
            if (sEffectInfoTable[EFFECT_SHIELD_PARTICLE].update(&sEffectContext.shieldParticles[i].effect) == 1) {
                Effect_Delete(play, i + SPARK_COUNT + BLURE_COUNT);
            }
        }
    }
}

void Effect_Delete(PlayState* play, s32 index) {
    if (index == TOTAL_EFFECT_COUNT) {
        return;
    }

    if (index < SPARK_COUNT) {
        sEffectContext.sparks[index].status.active = false;
        sEffectInfoTable[EFFECT_SPARK].destroy(&sEffectContext.sparks[index].effect);
        return;
    }

    index -= SPARK_COUNT;
    if (index < BLURE_COUNT) {
        sEffectContext.blures[index].status.active = false;
        sEffectInfoTable[EFFECT_BLURE1].destroy(&sEffectContext.blures[index].effect);
        return;
    }

    index -= BLURE_COUNT;
    if (index < SHIELD_PARTICLE_COUNT) {
        sEffectContext.shieldParticles[index].status.active = false;
        sEffectInfoTable[EFFECT_SHIELD_PARTICLE].destroy(&sEffectContext.shieldParticles[index].effect);
        return;
    }
}

void Effect_DeleteAll(PlayState* play) {
    s32 i;

    osSyncPrintf("エフェクト総て解放\n"); // "All effect release"

    for (i = 0; i < SPARK_COUNT; i++) {
        sEffectContext.sparks[i].status.active = false;
        sEffectInfoTable[EFFECT_SPARK].destroy(&sEffectContext.sparks[i].effect);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        sEffectContext.blures[i].status.active = false;
        sEffectInfoTable[EFFECT_BLURE1].destroy(&sEffectContext.blures[i].effect);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        sEffectContext.shieldParticles[i].status.active = false;
        sEffectInfoTable[EFFECT_SHIELD_PARTICLE].destroy(&sEffectContext.shieldParticles[i].effect);
    }

    osSyncPrintf("エフェクト総て解放 終了\n"); // "All effects release End"
}

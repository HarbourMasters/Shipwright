#include "z64effect.h"
#include "global.h"

#define SPARK_COUNT 3
#define BLURE_COUNT 25
#define SHIELD_PARTICLE_COUNT 3
#define TIRE_MARK_COUNT 15

#define TOTAL_EFFECT_COUNT SPARK_COUNT + BLURE_COUNT + SHIELD_PARTICLE_COUNT + TIRE_MARK_COUNT

typedef struct EffectStatus {
    /* 0x0 */ u8 active;
    /* 0x1 */ u8 unk1;
    /* 0x2 */ u8 unk2;
} EffectStatus; // size = 0x3

typedef struct EffectContext {
    /* 0x0000 */ struct PlayState* play;
    struct {
        EffectStatus status;
        EffectSpark effect;
    } /* 0x0004 */ sparks[SPARK_COUNT];
    struct {
        EffectStatus status;
        EffectBlure effect;
    } /* 0x0E5C */ blures[BLURE_COUNT];
    struct {
        EffectStatus status;
        EffectShieldParticle effect;
    } /* 0x388C */ shieldParticles[SHIELD_PARTICLE_COUNT];
    struct {
        EffectStatus status;
        EffectTireMark effect;
    } /* 0x3DF0 */ tireMarks[TIRE_MARK_COUNT];
} EffectContext; // size = 0x98E0

EffectContext sEffectContext;

typedef struct EffectInfo {
    /* 0x00 */ u32 size;
    /* 0x04 */ void (*init)(void* effect, void* initParams);
    /* 0x08 */ void (*destroy)(void* effect);
    /* 0x0C */ s32 (*update)(void* effect);
    /* 0x10 */ void (*draw)(void* effect, struct GraphicsContext* gfxCtx);
} EffectInfo; // size = 0x14

EffectInfo sEffectInfoTable[EFFECT_MAX] = {
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
    {
        sizeof(EffectTireMark),
        EffectTireMark_Init,
        EffectTireMark_Destroy,
        EffectTireMark_Update,
        EffectTireMark_Draw,
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

    index -= SHIELD_PARTICLE_COUNT;
    if (index < TIRE_MARK_COUNT) {
        if (sEffectContext.tireMarks[index].status.active == true) {
            return &sEffectContext.tireMarks[index].effect;
        } else {
            return NULL;
        }
    }

    return NULL;
}

void Effect_InitStatus(EffectStatus* status) {
    status->active = false;
    status->unk1 = 0;
    status->unk2 = 0;
}

void Effect_Init(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        Effect_InitStatus(&sEffectContext.sparks[i].status);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        Effect_InitStatus(&sEffectContext.blures[i].status);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        //! @bug This is probably supposed to initialize shieldParticles, not blures again
        Effect_InitStatus(&sEffectContext.blures[i].status);
    }

    for (i = 0; i < TIRE_MARK_COUNT; i++) {
        Effect_InitStatus(&sEffectContext.tireMarks[i].status);
    }

    sEffectContext.play = play;
}

void Effect_Add(PlayState* play, s32* pIndex, EffectType type, u8 arg3, u8 arg4, void* initParams) {
    u32 slotFound;
    s32 i;
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

            case EFFECT_TIRE_MARK:
                for (i = 0; i < TIRE_MARK_COUNT; i++) {
                    if (sEffectContext.tireMarks[i].status.active == false) {
                        slotFound = true;
                        *pIndex = i + SPARK_COUNT + BLURE_COUNT + SHIELD_PARTICLE_COUNT;
                        effect = &sEffectContext.tireMarks[i].effect;
                        status = &sEffectContext.tireMarks[i].status;
                        break;
                    }
                }
                break;

            default:
                break;
        }

        if (slotFound) {
            sEffectInfoTable[type].init(effect, initParams);
            status->unk2 = arg3;
            status->unk1 = arg4;
            status->active = true;
        }
    }
}

void Effect_DrawAll(GraphicsContext* gfxCtx) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (!sEffectContext.sparks[i].status.active) {
            continue;
        }
        sEffectInfoTable[EFFECT_SPARK].draw(&sEffectContext.sparks[i].effect, gfxCtx);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (!sEffectContext.blures[i].status.active) {
            continue;
        }
        sEffectInfoTable[EFFECT_BLURE1].draw(&sEffectContext.blures[i].effect, gfxCtx);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (!sEffectContext.shieldParticles[i].status.active) {
            continue;
        }
        sEffectInfoTable[EFFECT_SHIELD_PARTICLE].draw(&sEffectContext.shieldParticles[i].effect, gfxCtx);
    }

    for (i = 0; i < TIRE_MARK_COUNT; i++) {
        if (!sEffectContext.tireMarks[i].status.active) {
            continue;
        }
        sEffectInfoTable[EFFECT_TIRE_MARK].draw(&sEffectContext.tireMarks[i].effect, gfxCtx);
    }
}

void Effect_UpdateAll(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (!sEffectContext.sparks[i].status.active) {
            continue;
        }
        if (sEffectInfoTable[EFFECT_SPARK].update(&sEffectContext.sparks[i].effect) == 1) {
            Effect_Destroy(play, i);
        }
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (!sEffectContext.blures[i].status.active) {
            continue;
        }
        if (sEffectInfoTable[EFFECT_BLURE1].update(&sEffectContext.blures[i].effect) == 1) {
            Effect_Destroy(play, i + SPARK_COUNT);
        }
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (!sEffectContext.shieldParticles[i].status.active) {
            continue;
        }
        if (sEffectInfoTable[EFFECT_SHIELD_PARTICLE].update(&sEffectContext.shieldParticles[i].effect) == 1) {
            Effect_Destroy(play, i + SPARK_COUNT + BLURE_COUNT);
        }
    }

    for (i = 0; i < TIRE_MARK_COUNT; i++) {
        if (!sEffectContext.tireMarks[i].status.active) {
            continue;
        }
        if (sEffectInfoTable[EFFECT_TIRE_MARK].update(&sEffectContext.tireMarks[i].effect) == 1) {
            Effect_Destroy(play, i + SPARK_COUNT + BLURE_COUNT + SHIELD_PARTICLE_COUNT);
        }
    }
}

void Effect_Destroy(PlayState* play, s32 index) {
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

    index -= SHIELD_PARTICLE_COUNT;
    if (index < TIRE_MARK_COUNT) {
        sEffectContext.tireMarks[index].status.active = false;
        sEffectInfoTable[EFFECT_TIRE_MARK].destroy(&sEffectContext.tireMarks[index].effect);
        return;
    }
}

void Effect_DestroyAll(PlayState* play) {
    s32 i;

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

    for (i = 0; i < TIRE_MARK_COUNT; i++) {
        sEffectContext.tireMarks[i].status.active = false;
        sEffectInfoTable[EFFECT_TIRE_MARK].destroy(&sEffectContext.tireMarks[i].effect);
    }
}

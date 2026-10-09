#include "z64effect_ss.h"

#include "tha.h"
#include "loadfragment.h"
#include "z64malloc.h"
#include "global.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

void EffectSs_Reset(EffectSs* effectSs);

EffectSsInfo sEffectSsInfo = { 0 };

void EffectSs_InitInfo(PlayState* play, s32 tableSize) {
    u32 i;
    EffectSs* effectSs;
    EffectSsOverlay* overlay;

    sEffectSsInfo.table = (EffectSs*)THA_AllocTailAlign16(&play->state.tha, tableSize * sizeof(EffectSs));
    sEffectSsInfo.searchStartIndex = 0;
    sEffectSsInfo.tableSize = tableSize;

    for (effectSs = &sEffectSsInfo.table[0]; effectSs < &sEffectSsInfo.table[sEffectSsInfo.tableSize]; effectSs++) {
        EffectSs_Reset(effectSs);
        effectSs->epoch = 0;
    }

    overlay = &gEffectSsOverlayTable[0];
    for (i = 0; i < EFFECT_SS_TYPE_MAX; i++) {
        overlay->loadedRamAddr = NULL;
        overlay++;
    }
}

void EffectSs_ClearAll(PlayState* play) {
    u32 i;
    EffectSs* effectSs;
    EffectSsOverlay* overlay;
    void* addr;

    sEffectSsInfo.table = NULL;
    sEffectSsInfo.searchStartIndex = 0;
    sEffectSsInfo.tableSize = 0;

    //! @bug: Effects left in the table are not properly deleted, as dataTable was just set to NULL and size to 0
    for (effectSs = &sEffectSsInfo.table[0]; effectSs < &sEffectSsInfo.table[sEffectSsInfo.tableSize]; effectSs++) {
        EffectSs_Delete(effectSs);
    }

    // Free memory from loaded effectSs overlays
    overlay = &gEffectSsOverlayTable[0];
    for (i = 0; i < EFFECT_SS_TYPE_MAX; i++) {
        addr = overlay->loadedRamAddr;
        if (addr != NULL) {
            ZeldaArena_Free(addr);
        }

        overlay->loadedRamAddr = NULL;
        overlay++;
    }
}

EffectSs* EffectSs_GetTable(void) {
    return sEffectSsInfo.table;
}

void EffectSs_Delete(EffectSs* effectSs) {
    if (effectSs->flags & 2) {
        AudioSfx_StopByPos(&effectSs->pos);
    }

    if (effectSs->flags & 4) {
        AudioSfx_StopByPos(&effectSs->vec);
    }

    EffectSs_Reset(effectSs);
}

void EffectSs_Reset(EffectSs* effectSs) {
    u32 i;

    effectSs->type = EFFECT_SS_TYPE_MAX;
    effectSs->accel.x = effectSs->accel.y = effectSs->accel.z = 0;
    effectSs->velocity.x = effectSs->velocity.y = effectSs->velocity.z = 0;
    effectSs->vec.x = effectSs->vec.y = effectSs->vec.z = 0;
    effectSs->pos.x = effectSs->pos.y = effectSs->pos.z = 0;
    effectSs->life = -1;
    effectSs->flags = 0;
    effectSs->priority = 128;
    effectSs->draw = NULL;
    effectSs->update = NULL;
    effectSs->gfx = NULL;
    effectSs->actor = NULL;

    for (i = 0; i < ARRAY_COUNT(effectSs->regs); i++) {
        effectSs->regs[i] = 0;
    }
}

s32 EffectSs_FindSlot(s32 priority, s32* index) {
    s32 foundFree;
    s32 i;

    if (sEffectSsInfo.searchStartIndex >= sEffectSsInfo.tableSize) {
        sEffectSsInfo.searchStartIndex = 0;
    }

    // Search for a unused entry
    i = sEffectSsInfo.searchStartIndex;
    foundFree = false;
    while (true) {
        if (sEffectSsInfo.table[i].life == -1) {
            foundFree = true;
            break;
        }

        i++;

        if (i >= sEffectSsInfo.tableSize) {
            i = 0; // Loop around the whole table
        }

        // After a full loop, break out
        if (i == sEffectSsInfo.searchStartIndex) {
            break;
        }
    }

    if (foundFree == true) {
        *index = i;
        return 0;
    }

    // If all slots are in use, search for a slot with a lower priority
    // Note that a lower priority is representend by a higher value
    i = sEffectSsInfo.searchStartIndex;
    while (true) {
        // Equal priority should only be considered "lower" if flag 0 is set
        if ((priority <= sEffectSsInfo.table[i].priority) &&
            !((priority == sEffectSsInfo.table[i].priority) && (sEffectSsInfo.table[i].flags & 1))) {
            break;
        }

        i++;

        if (i >= sEffectSsInfo.tableSize) {
            i = 0; // Loop around the whole table
        }

        // After a full loop, return 1 to indicate that we failed to find a suitable slot
        if (i == sEffectSsInfo.searchStartIndex) {
            return 1;
        }
    }

    *index = i;
    return 0;
}

void EffectSs_Insert(PlayState* play, EffectSs* effectSs) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) != true) {
        if (EffectSs_FindSlot(effectSs->priority, &index) == 0) {
            u32 epoch = sEffectSsInfo.table[index].epoch + 1;

            sEffectSsInfo.searchStartIndex = index + 1;
            sEffectSsInfo.table[index] = *effectSs;
            sEffectSsInfo.table[index].epoch = epoch;
        }
    }
}

void EffectSs_Spawn(PlayState* play, s32 type, s32 priority, void* initData) {
    s32 index;
    u32 overlaySize;
    EffectSsOverlay* overlayEntry = &gEffectSsOverlayTable[type];
    EffectSsProfile* profile;

    if (EffectSs_FindSlot(priority, &index) != 0) {
        // Abort because we couldn't find a suitable slot to add this effect in
        return;
    }

    sEffectSsInfo.searchStartIndex = index + 1;
    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    if (overlayEntry->vramStart == NULL) {
        profile = overlayEntry->profile;
    } else {
        if (overlayEntry->loadedRamAddr == NULL) {
            overlayEntry->loadedRamAddr = ZeldaArena_MallocR(overlaySize);

            if (overlayEntry->loadedRamAddr == NULL) {
                return;
            }

            Overlay_Load(overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                         overlayEntry->loadedRamAddr);
        }

        profile = (void*)(uintptr_t)((overlayEntry->profile != NULL)
                                         ? (void*)((uintptr_t)overlayEntry->profile -
                                                   (intptr_t)((uintptr_t)overlayEntry->vramStart -
                                                              (uintptr_t)overlayEntry->loadedRamAddr))
                                         : NULL);
    }

    if (profile->init == NULL) {
        return;
    }

    // Delete the previous effect in the slot, in case the slot wasn't free
    EffectSs_Delete(&sEffectSsInfo.table[index]);

    sEffectSsInfo.table[index].type = type;
    sEffectSsInfo.table[index].priority = priority;
    sEffectSsInfo.table[index].epoch++;

    if (profile->init(play, index, &sEffectSsInfo.table[index], initData) == 0) {
        EffectSs_Reset(&sEffectSsInfo.table[index]);
    }
}

void EffectSs_Update(PlayState* play, s32 index) {
    EffectSs* effectSs = &sEffectSsInfo.table[index];

    if (effectSs->update != NULL) {
        effectSs->velocity.x += effectSs->accel.x;
        effectSs->velocity.y += effectSs->accel.y;
        effectSs->velocity.z += effectSs->accel.z;

        effectSs->pos.x += effectSs->velocity.x;
        effectSs->pos.y += effectSs->velocity.y;
        effectSs->pos.z += effectSs->velocity.z;

        effectSs->update(play, index, effectSs);
    }
}

void EffectSs_UpdateAll(PlayState* play) {
    s32 i;

    for (i = 0; i < sEffectSsInfo.tableSize; i++) {
        if (sEffectSsInfo.table[i].life > -1) {
            sEffectSsInfo.table[i].life--;

            if (sEffectSsInfo.table[i].life < 0) {
                EffectSs_Delete(&sEffectSsInfo.table[i]);
            }
        }

        if (sEffectSsInfo.table[i].life > -1) {
            EffectSs_Update(play, i);
        }
    }
}

void EffectSs_Draw(PlayState* play, s32 index) {
    EffectSs* effectSs = &sEffectSsInfo.table[index];

    if (effectSs->draw != NULL) {
        FrameInterpolation_RecordOpenChild(effectSs, effectSs->epoch);
        effectSs->draw(play, index, effectSs);
        FrameInterpolation_RecordCloseChild();
    }
}

void EffectSs_DrawAll(PlayState* play) {
    Lights* lights = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);
    s32 i;

    Lights_BindAll(lights, play->lightCtx.listHead, NULL, play);
    Lights_Draw(lights, play->state.gfxCtx);

    for (i = 0; i < sEffectSsInfo.tableSize; i++) {
        if (sEffectSsInfo.table[i].life > -1) {
            if ((sEffectSsInfo.table[i].pos.x > BGCHECK_Y_MAX) || (sEffectSsInfo.table[i].pos.x < BGCHECK_Y_MIN) ||
                (sEffectSsInfo.table[i].pos.y > BGCHECK_Y_MAX) || (sEffectSsInfo.table[i].pos.y < BGCHECK_Y_MIN) ||
                (sEffectSsInfo.table[i].pos.z > BGCHECK_Y_MAX) || (sEffectSsInfo.table[i].pos.z < BGCHECK_Y_MIN)) {
                EffectSs_Delete(&sEffectSsInfo.table[i]);
            } else {
                EffectSs_Draw(play, i);
            }
        }
    }
}

/**
 * Lerp from `a` (weightInv == inf) to `b` (weightInv == 1 or 0).
 */
s16 EffectSs_LerpInv(s16 a, s16 b, s32 weightInv) {
    s16 ret = (weightInv == 0) ? b : (a + (s32)((b - a) / (f32)weightInv));

    return ret;
}

/**
 * Lerp from `a` (weight == 0) to `b` (weight == 1).
 */
s16 EffectSs_LerpS16(s16 a, s16 b, f32 weight) {
    return (b - a) * weight + a;
}

/**
 * Lerp from `a` (weight == 0) to `b` (weight == 1).
 */
u8 EffectSs_LerpU8(u8 a, u8 b, f32 weight) {
    return weight * ((f32)b - (f32)a) + a;
}

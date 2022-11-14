#include "global.h"
#include "vt.h"

#include "soh/frame_interpolation.h"

EffectSsInfo sEffectSsInfo = { 0 }; // "EffectSS2Info"

void EffectSs_InitInfo(PlayState* play, s32 tableSize) {
    u32 i;
    EffectSs* effectSs;
    EffectSsOverlay* overlay;

    for (i = 0; i < ARRAY_COUNT(gEffectSsOverlayTable); i++) {
        overlay = &gEffectSsOverlayTable[i];
        osSyncPrintf("effect index %3d:size=%6dbyte romsize=%6dbyte\n", i,
                     (uintptr_t)overlay->vramEnd - (uintptr_t)overlay->vramStart, overlay->vromEnd - overlay->vromStart);
    }

    sEffectSsInfo.table =
        GAMESTATE_ALLOC_MC(&play->state, tableSize * sizeof(EffectSs));
    ASSERT(sEffectSsInfo.table != NULL);

    sEffectSsInfo.searchStartIndex = 0;
    sEffectSsInfo.tableSize = tableSize;

    for (effectSs = &sEffectSsInfo.table[0]; effectSs < &sEffectSsInfo.table[sEffectSsInfo.tableSize]; effectSs++) {
        EffectSs_Reset(effectSs);
    }

    overlay = &gEffectSsOverlayTable[0];
    for (i = 0; i < ARRAY_COUNT(gEffectSsOverlayTable); i++) {
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

    // This code doesn't actually work, since table was just set to NULL and tableSize to 0
    for (effectSs = &sEffectSsInfo.table[0]; effectSs < &sEffectSsInfo.table[sEffectSsInfo.tableSize]; effectSs++) {
        EffectSs_Delete(effectSs);
    }

    overlay = &gEffectSsOverlayTable[0];
    for (i = 0; i < ARRAY_COUNT(gEffectSsOverlayTable); i++) {
        addr = overlay->loadedRamAddr;

        if (addr != NULL) {
            ZELDA_ARENA_FREE_DEBUG(addr);
        }

        overlay->loadedRamAddr = NULL;
        overlay++;
    }
}

void EffectSs_Delete(EffectSs* effectSs) {
    if (effectSs->flags & 2) {
        Audio_StopSfxByPos(&effectSs->pos);
    }

    if (effectSs->flags & 4) {
        Audio_StopSfxByPos(&effectSs->vec);
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

s32 EffectSs_FindSlot(s32 priority, s32* pIndex) {
    s32 foundFree;
    s32 i;

    if (sEffectSsInfo.searchStartIndex >= sEffectSsInfo.tableSize) {
        sEffectSsInfo.searchStartIndex = 0;
    }

    // Search for a free slot
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
        *pIndex = i;
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

    *pIndex = i;
    return 0;
}

void EffectSs_Insert(PlayState* play, EffectSs* effectSs) {
    s32 index;

    if (FrameAdvance_IsEnabled(play) != true) {
        if (EffectSs_FindSlot(effectSs->priority, &index) == 0) {
            sEffectSsInfo.searchStartIndex = index + 1;
            sEffectSsInfo.table[index] = *effectSs;
        }
    }
}

// original name: "EffectSoftSprite2_makeEffect"
void EffectSs_Spawn(PlayState* play, s32 type, s32 priority, void* initParams) {
    s32 index;
    u32 overlaySize;
    EffectSsOverlay* overlayEntry;
    EffectSsInit* initInfo;

    overlayEntry = &gEffectSsOverlayTable[type];

    ASSERT(type < EFFECT_SS_TYPE_MAX);

    if (EffectSs_FindSlot(priority, &index) != 0) {
        // Abort because we couldn't find a suitable slot to add this effect in
        return;
    }

    sEffectSsInfo.searchStartIndex = index + 1;
    overlaySize = (uintptr_t)overlayEntry->vramEnd - (uintptr_t)overlayEntry->vramStart;

    if (overlayEntry->vramStart == NULL) {
        // "Not an overlay"
        osSyncPrintf("EffectSoftSprite2_makeEffect():オーバーレイではありません。\n");
        initInfo = overlayEntry->initInfo;
    } else {
        if (overlayEntry->loadedRamAddr == NULL) {
            overlayEntry->loadedRamAddr = ZELDA_ARENA_MALLOC_RDEBUG(overlaySize);

            if (overlayEntry->loadedRamAddr == NULL) {
                osSyncPrintf(VT_FGCOL(RED));
                // "The memory of %d byte cannot be secured. Therefore, the program cannot be loaded.
                // What a dangerous situation! Naturally, effects will not produced either."
                osSyncPrintf("EffectSoftSprite2_makeEffect():zelda_malloc_r()により,%"
                             "dbyteのメモリ確保ができま\nせん。そのため、プログラムのロードも\n出来ません。ただいま危険"
                             "な状態です！\nもちろん,エフェクトも出ません。\n",
                             overlaySize);
                osSyncPrintf(VT_RST);
                return;
            }

            Overlay_Load(overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                         overlayEntry->loadedRamAddr);

            osSyncPrintf(VT_FGCOL(GREEN));
            osSyncPrintf("EFFECT SS OVL:SegRom %08x %08x, Seg %08x %08x, RamStart %08x, type: %d\n",
                         overlayEntry->vromStart, overlayEntry->vromEnd, overlayEntry->vramStart, overlayEntry->vramEnd,
                         overlayEntry->loadedRamAddr, type);
            osSyncPrintf(VT_RST);
        }

        initInfo = (void*)(uintptr_t)((overlayEntry->initInfo != NULL)
                                    ? (void*)((uintptr_t)overlayEntry->initInfo -
                                              ((intptr_t)overlayEntry->vramStart - (intptr_t)overlayEntry->loadedRamAddr))
                                    : NULL);
    }

    if (initInfo->init == NULL) {
        // "Effects have already been loaded, but the constructor is NULL so the addition will not occur.
        // Please fix this. (Waste of memory) %08x %d"
        osSyncPrintf("EffectSoftSprite2_makeEffect():すでにエフェクトはロード済みで\nすが,"
                     "コンストラクターがNULLなので追加をやめます。\n直してください。（メモリーの無駄) %08x %d\n",
                     initInfo, type);
        return;
    }

    // Delete the previous effect in the slot, in case the slot wasn't free
    EffectSs_Delete(&sEffectSsInfo.table[index]);

    sEffectSsInfo.table[index].type = type;
    sEffectSsInfo.table[index].priority = priority;
    sEffectSsInfo.table[index].epoch++;

    if (initInfo->init(play, index, &sEffectSsInfo.table[index], initParams) == 0) {
        osSyncPrintf(VT_FGCOL(GREEN));
        // "Construction failed for some reason. The constructor returned an error.
        // Ceasing effect addition."
        osSyncPrintf("EffectSoftSprite2_makeEffect():"
                     "何らかの理由でコンストラクト失敗。コンストラクターがエラーを返しました。エフェクトの追加を中"
                     "止します。\n");
        osSyncPrintf(VT_RST);
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

// original name: "EffectSoftSprite2_disp"
void EffectSs_DrawAll(PlayState* play) {
    Lights* lights = LightContext_NewLights(&play->lightCtx, play->state.gfxCtx);
    s32 i;

    Lights_BindAll(lights, play->lightCtx.listHead, NULL);
    Lights_Draw(lights, play->state.gfxCtx);

    for (i = 0; i < sEffectSsInfo.tableSize; i++) {
        if (sEffectSsInfo.table[i].life > -1) {
            if ((sEffectSsInfo.table[i].pos.x > 32000.0f) || (sEffectSsInfo.table[i].pos.x < -32000.0f) ||
                (sEffectSsInfo.table[i].pos.y > 32000.0f) || (sEffectSsInfo.table[i].pos.y < -32000.0f) ||
                (sEffectSsInfo.table[i].pos.z > 32000.0f) || (sEffectSsInfo.table[i].pos.z < -32000.0f)) {
                osSyncPrintf(VT_FGCOL(RED));
                // "Since the position is outside the area, delete it.
                // Effect label No. %d: Please respond by the program.
                // Here is ==> pos (%f, %f, %f) and the label is in z_effect_soft_sprite_dlftbls.decl."
                osSyncPrintf("EffectSoftSprite2_disp():位置が領域外のため "
                             "削除します。エフェクトラベルNo.%d:プログラムの方で対応をお願いします。ここです ==> "
                             "pos(%f, %f, %f)で、ラベルはz_effect_soft_sprite_dlftbls.declにあります。\n",
                             sEffectSsInfo.table[i].type, sEffectSsInfo.table[i].pos.x, sEffectSsInfo.table[i].pos.y,
                             sEffectSsInfo.table[i].pos.z);
                osSyncPrintf(VT_FGCOL(GREEN));
                // "If you are using pos for something else, consult me."
                osSyncPrintf("もし、posを別のことに使っている場合相談に応じます。\n");
                osSyncPrintf(VT_RST);

                EffectSs_Delete(&sEffectSsInfo.table[i]);
            } else {
                EffectSs_Draw(play, i);
            }
        }
    }
}

s16 func_80027DD4(s16 arg0, s16 arg1, s32 arg2) {
    s16 ret = (arg2 == 0) ? arg1 : (arg0 + (s32)((arg1 - arg0) / (f32)arg2));

    return ret;
}

s16 func_80027E34(s16 arg0, s16 arg1, f32 arg2) {
    return (arg1 - arg0) * arg2 + arg0;
}

u8 func_80027E84(u8 arg0, u8 arg1, f32 arg2) {
    return arg2 * ((f32)arg1 - (f32)arg0) + arg0;
}

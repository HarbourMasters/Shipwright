#include "global.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"

typedef struct {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ u8 token;
    /* 0x04 */ s8* reverbAdd;
    /* 0x08 */ Vec3f* pos;
    /* 0x0C */ f32* freqScale;
    /* 0x10 */ f32* volume;
} SfxRequest; // size = 0x14

typedef struct {
    /* 0x0 */ f32 value;
    /* 0x4 */ f32 target;
    /* 0x8 */ f32 step;
    /* 0xC */ u16 remainingFrames;
} SfxBankLerp; // size = 0x10

typedef enum {
    /* 0 */ SFX_RM_REQ_BY_BANK,
    /* 1 */ SFX_RM_REQ_BY_POS_AND_BANK,
    /* 2 */ SFX_RM_REQ_BY_POS,
    /* 3 */ SFX_RM_REQ_BY_POS_AND_ID,
    /* 4 */ SFX_RM_REQ_BY_TOKEN_AND_ID,
    /* 5 */ SFX_RM_REQ_BY_ID
} SfxRemoveRequest;

SfxBankEntry sSfxPlayerBank[9];
SfxBankEntry sSfxItemBank[12];
SfxBankEntry sSfxEnvironmentBank[32];
SfxBankEntry sSfxEnemyBank[20];
SfxBankEntry sSfxSystemBank[8];
SfxBankEntry sSfxOcarinaBank[3];
SfxBankEntry sSfxVoiceBank[5];
SfxRequest sSfxRequests[0x100];
u8 sSfxBankListEnd[7];
u8 sSfxBankFreeListStart[7];
u8 sSfxBankUnused[7];
ActiveSfx gActiveSfx[7][3];
u8 sCurSfxPlayerChannelIndex;
u8 gSfxBankMuted[7];
SfxBankLerp sSfxBankLerp[7];

// sSfxRequests ring buffer endpoints. read index <= write index, wrapping around mod 256.
u8 sSfxRequestWriteIndex = 0;
u8 sSfxRequestReadIndex = 0;

/**
 * Array of pointers to arrays of SfxBankEntry of sizes: 9, 12, 32, 20, 8, 3, 5
 *
 * 0 : Player Bank          size 9
 * 1 : Item Bank            size 12
 * 2 : Environment Bank     size 32
 * 3 : Enemy Bank           size 20
 * 4 : System Bank          size 8
 * 5 : Ocarina Bank         size 3
 * 6 : Voice Bank           size 5
 */
SfxBankEntry* gSfxBanks[7] = {
    sSfxPlayerBank, sSfxItemBank, sSfxEnvironmentBank, sSfxEnemyBank, sSfxSystemBank, sSfxOcarinaBank, sSfxVoiceBank,
};

u8 sSfxBankSizes[ARRAY_COUNT(gSfxBanks)] = {
    ARRAY_COUNT(sSfxPlayerBank), ARRAY_COUNT(sSfxItemBank),   ARRAY_COUNT(sSfxEnvironmentBank),
    ARRAY_COUNT(sSfxEnemyBank),  ARRAY_COUNT(sSfxSystemBank), ARRAY_COUNT(sSfxOcarinaBank),
    ARRAY_COUNT(sSfxVoiceBank),
};

u8 gSfxChannelLayout = 0;
u16 sSfxChannelLowVolumeFlag = 0;

// The center of the screen in projected coordinates.
// Gives the impression that the sfx has no specific location
Vec3f gSfxDefaultPos = { 0.0f, 0.0f, 0.0f };

// Reused as either frequency or volume multiplicative scaling factor
// Does not alter or change frequency or volume
f32 gSfxDefaultFreqAndVolScale = 1.0f;
s32 D_801DB4B4 = 0; // unused

// Adds no reverb to the existing reverb
s8 gSfxDefaultReverb = 0;

void AudioSfx_MuteBanks(u16 muteMask) {
    u8 bankId;

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        if (muteMask & 1) {
            gSfxBankMuted[bankId] = true;
        } else {
            gSfxBankMuted[bankId] = false;
        }
        muteMask = muteMask >> 1;
    }
}

/**
 * Lowers volumes of both bgm players so that sfx can be more pronounced.
 * Each sfx channel stores its own bit to lower the volume.
 * Only a single channel needs to request this to lower the volume.
 */
void AudioSfx_LowerBgmVolume(u8 channelIndex) {
    sSfxChannelLowVolumeFlag |= (1 << channelIndex);

    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_SFX, 0x40, 0xF);
    AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_SFX, 0x40, 0xF);
}

/**
 * Clears the flag for the specific channel to lower volume.
 * If all flags are cleared, then both bgm players return to full volume
 */
void AudioSfx_RestoreBgmVolume(u8 channelIndex) {
    sSfxChannelLowVolumeFlag &= ((1 << channelIndex) ^ 0xFFFF);

    if (sSfxChannelLowVolumeFlag == 0) {
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_SFX, 0x7F, 0xF);
        AudioSeq_SetVolumeScale(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_SFX, 0x7F, 0xF);
    }
}

/**
 * The main function to request a sfx. All sfx requests begin here.
 */
void AudioSfx_PlaySfx(u16 sfxId, Vec3f* pos, u8 token, f32* freqScale, f32* volume, s8* reverbAdd) {
    u8 i;
    SfxRequest* reqWrite;
    SfxRequest* reqRead;

    if (!gSfxBankMuted[SFX_BANK_SHIFT(sfxId)]) {
        reqWrite = &sSfxRequests[sSfxRequestWriteIndex];

        for (i = sSfxRequestReadIndex; sSfxRequestWriteIndex != i; i++) {
            reqRead = &sSfxRequests[i];
            if ((reqRead->pos == pos) && (reqRead->sfxId == sfxId)) {
                return;
            }
        }

        reqWrite->sfxId = sfxId;
        reqWrite->pos = pos;
        reqWrite->token = token;
        reqWrite->freqScale = freqScale;
        reqWrite->volume = volume;
        reqWrite->reverbAdd = reverbAdd;
        sSfxRequestWriteIndex++;
    }
}

void AudioSfx_RemoveMatchingRequests(u8 aspect, SfxBankEntry* entry) {
    SfxRequest* req;
    s32 remove;
    u8 i = sSfxRequestReadIndex;

    for (; i != sSfxRequestWriteIndex; i++) {
        remove = false;
        req = &sSfxRequests[i];

        switch (aspect) {
            case SFX_RM_REQ_BY_BANK:
                if (SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(entry->sfxId)) {
                    remove = true;
                }
                break;

            case SFX_RM_REQ_BY_POS_AND_BANK:
                if ((SFX_BANK_MASK(req->sfxId) == SFX_BANK_MASK(entry->sfxId)) && (&req->pos->x == entry->posX)) {
                    remove = true;
                }
                break;

            case SFX_RM_REQ_BY_POS:
                if (&req->pos->x == entry->posX) {
                    remove = true;
                }
                break;

            case SFX_RM_REQ_BY_POS_AND_ID:
                if ((&req->pos->x == entry->posX) && (req->sfxId == entry->sfxId)) {
                    remove = true;
                }
                break;

            case SFX_RM_REQ_BY_TOKEN_AND_ID:
                if ((req->token == entry->token) && (req->sfxId == entry->sfxId)) {
                    remove = true;
                }
                break;

            case SFX_RM_REQ_BY_ID:
                if (req->sfxId == entry->sfxId) {
                    remove = true;
                }
                break;

            default:
                break;
        }

        if (remove) {
            req->sfxId = NA_SE_NONE;
        }
    }
}

void AudioSfx_ProcessRequest(void) {
    u16 sfxId;
    u8 channelCount;
    u8 index;
    SfxRequest* req = &sSfxRequests[sSfxRequestReadIndex];
    SfxBankEntry* entry;
    SfxParams* sfxParams;
    s32 bankId;
    u8 evictImportance;
    u8 evictIndex = 0x80;

    if (req->sfxId == NA_SE_NONE) {
        return;
    }
    // #region 2S2H [Port] Custom sequences
    evictIndex = 0x80;
    if (req->sfxId == 0) {
        return;
    }
    // All custom sequences use the same collection, which can result in collisions when different types (e.g. SFX and
    // BGM) share the same IDs. As we do not yet shuffle SFX, simply do not get the replacement sequence for now.
    // u16 newSfxId = AudioEditor_GetReplacementSeq(req->sfxId);
    // if (req->sfxId != newSfxId) {
    //     gAudioCtx.seqReplaced[SEQ_PLAYER_SFX] = 1;
    //     req->sfxId = newSfxId;
    // }
    // #end region
    bankId = SFX_BANK(req->sfxId);
    channelCount = 0;
    index = gSfxBanks[bankId][0].next;

    // Compare the sfx request to all sfx bank entries
    while ((index != 0xFF) && (index != 0)) {

        // If an existing sfx from the same source exists in the bank, then we should either:
        // renew if it is the same sfx, check if any sfx channels are available for that bank,
        // interrupt that sfx and replace it with the new sfx request, or drop the new sfx request.
        if (gSfxBanks[bankId][index].posX == &req->pos->x) {

            // If the new sfx has equal importance to the existing sfx,
            // drop the request if the existing sfx has the "SFX_FLAG_BLOCK_EQUAL_IMPORTANCE" flag.
            // Otherwise, keep processing the new sfx request
            if ((gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].params &
                 SFX_FLAG_BLOCK_EQUAL_IMPORTANCE) &&
                (gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance ==
                 gSfxBanks[bankId][index].sfxImportance)) {
                // Drop the new request
                return;
            }

            if (gSfxBanks[bankId][index].sfxId == req->sfxId) {
                // The new sfx is the same as the existing sfx
                // Set channelCount to max value to reprocess the sfx
                channelCount = gUsedChannelsPerBank[gSfxChannelLayout][bankId];
            } else {
                // Determine which sfx to evict based on importance if there are no more channels available
                if (channelCount == 0) {
                    // If this is the first sfx at the same source, than store the existing sfx info into evict temps
                    evictIndex = index;
                    sfxId = gSfxBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSfxParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                } else if (gSfxBanks[bankId][index].sfxImportance < evictImportance) {
                    // If a different existing sfx is at the same source, than update the evict temps if the next
                    // existing src has lower importance than the previous existing source
                    evictIndex = index;
                    sfxId = gSfxBanks[bankId][index].sfxId & 0xFFFF;
                    evictImportance = gSfxParams[SFX_BANK_SHIFT(sfxId)][SFX_INDEX(sfxId)].importance;
                }

                channelCount++;

                // There are no channels available, evict the lowest importance
                if (channelCount == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                    if (gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)].importance >= evictImportance) {
                        index = evictIndex;
                    } else {
                        index = 0;
                    }
                }
            }

            // Process this sfx by refreshing or replacing and existing sfx
            if (channelCount == gUsedChannelsPerBank[gSfxChannelLayout][bankId]) {
                sfxParams = &gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];

                // Interrupt existing sfx and play the new sfx instead.
                if ((req->sfxId & SFX_FLAG_MASK) || (sfxParams->flags & SFX_FLAG2_FORCE_RESET) ||
                    (index == evictIndex)) {

                    // Restore bgm if the sfx about to be replaced has the right flag
                    if ((gSfxBanks[bankId][index].sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) &&
                        (gSfxBanks[bankId][index].state != SFX_STATE_QUEUED)) {
                        AudioSfx_RestoreBgmVolume(gSfxBanks[bankId][index].channelIndex);
                    }

                    gSfxBanks[bankId][index].token = req->token;
                    gSfxBanks[bankId][index].sfxId = req->sfxId;
                    gSfxBanks[bankId][index].state = SFX_STATE_QUEUED;
                    gSfxBanks[bankId][index].freshness = 2;
                    gSfxBanks[bankId][index].freqScale = req->freqScale;
                    gSfxBanks[bankId][index].volume = req->volume;
                    gSfxBanks[bankId][index].reverbAdd = req->reverbAdd;
                    gSfxBanks[bankId][index].sfxParams = sfxParams->params;
                    gSfxBanks[bankId][index].sfxFlags = sfxParams->flags;
                    gSfxBanks[bankId][index].sfxImportance = sfxParams->importance;
                } else if (gSfxBanks[bankId][index].state == SFX_STATE_PLAYING_ONE_FRAME) {
                    // The new sfx is "one-frame" and equals the existing sfx,
                    // continue playing the "one-frame" sfx by setting "SFX_STATE_PLAYING".
                    // This will avoid triggering deletion and get "SFX_STATE_REFRESH"
                    // The state will be set back to "SFX_STATE_PLAYING_ONE_FRAME" at the end of the cycle
                    gSfxBanks[bankId][index].state = SFX_STATE_PLAYING;
                }
                // Terminate loop, function is finished (nothing more is processed)
                index = 0;
            }
        }

        if (index != 0) {
            index = gSfxBanks[bankId][index].next;
        }
    }

    if ((gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].next != 0xFF) && (index != 0)) {
        // Allocate from free list
        index = sSfxBankFreeListStart[bankId];

        entry = &gSfxBanks[bankId][index];
        entry->posX = &req->pos->x;
        entry->posY = &req->pos->y;
        entry->posZ = &req->pos->z;
        entry->token = req->token;
        entry->freqScale = req->freqScale;
        entry->volume = req->volume;
        entry->reverbAdd = req->reverbAdd;

        sfxParams = &gSfxParams[SFX_BANK_SHIFT(req->sfxId)][SFX_INDEX(req->sfxId)];

        entry->sfxParams = sfxParams->params;
        entry->sfxFlags = sfxParams->flags;
        entry->sfxImportance = sfxParams->importance;
        entry->sfxId = req->sfxId;
        entry->state = SFX_STATE_QUEUED;
        entry->freshness = 2;
        entry->prev = sSfxBankListEnd[bankId];

        gSfxBanks[bankId][sSfxBankListEnd[bankId]].next = sSfxBankFreeListStart[bankId];
        sSfxBankListEnd[bankId] = sSfxBankFreeListStart[bankId];
        sSfxBankFreeListStart[bankId] = gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].next;
        gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].prev = 0xFF;

        entry->next = 0xFF;
    }
}

void AudioSfx_RemoveBankEntry(u8 bankId, u8 entryIndex) {
    SfxBankEntry* entry = &gSfxBanks[bankId][entryIndex];
    u8 i;

    if (entry->sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) {
        AudioSfx_RestoreBgmVolume(entry->channelIndex);
    }

    if (entryIndex == sSfxBankListEnd[bankId]) {
        sSfxBankListEnd[bankId] = entry->prev;
    } else {
        gSfxBanks[bankId][entry->next].prev = entry->prev;
    }

    gSfxBanks[bankId][entry->prev].next = entry->next;
    entry->next = sSfxBankFreeListStart[bankId];
    entry->prev = 0xFF;
    gSfxBanks[bankId][sSfxBankFreeListStart[bankId]].prev = entryIndex;
    sSfxBankFreeListStart[bankId] = entryIndex;
    entry->state = SFX_STATE_EMPTY;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        if (gActiveSfx[bankId][i].entryIndex == entryIndex) {
            gActiveSfx[bankId][i].entryIndex = 0xFF;
            i = gChannelsPerBank[gSfxChannelLayout][bankId];
        }
    }
}

void AudioSfx_ChooseActiveSfx(u8 bankId) {
    u8 numChosenSfx = 0;
    u8 numChannels;
    u8 entryIndex;
    u8 i;
    u8 j;
    u8 k;
    u8 sfxImportance;
    u8 needNewSfx;
    u8 chosenEntryIndex;
    s32 pad;
    SfxBankEntry* entry;
    ActiveSfx chosenSfx[MAX_CHANNELS_PER_BANK];
    ActiveSfx* activeSfx;
    f32 entryPosY;
    f32 entryPosX;

    for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
        chosenSfx[i].priority = 0x7FFFFFFF;
        chosenSfx[i].entryIndex = 0xFF;
    }

    entryIndex = gSfxBanks[bankId][0].next;
    k = 0;

    // Delete stale sfx and prioritize remaining sfx into the gActiveSfx arrays
    while (entryIndex != 0xFF) {

        // Update the freshness for an "all-frame" sfx if it is still queued
        if ((gSfxBanks[bankId][entryIndex].state == SFX_STATE_QUEUED) &&
            (gSfxBanks[bankId][entryIndex].sfxId & SFX_FLAG_MASK)) {
            gSfxBanks[bankId][entryIndex].freshness--;

            // If a "one-frame" sfx is still in "SFX_STATE_PLAYING_ONE_FRAME", then remove the sfx
        } else if (!(gSfxBanks[bankId][entryIndex].sfxId & SFX_FLAG_MASK) &&
                   (gSfxBanks[bankId][entryIndex].state == SFX_STATE_PLAYING_ONE_FRAME)) {
            // ioPort 0, force stop sfx in seq 0
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, gSfxBanks[bankId][entryIndex].channelIndex, 0, 0);
            AudioSfx_RemoveBankEntry(bankId, entryIndex);
        }

        // If a "all-frame" sfx goes 2 frames in the "queued" state
        // (because it is too low priority), then remove the sfx
        if (gSfxBanks[bankId][entryIndex].freshness == 0) {
            AudioSfx_RemoveBankEntry(bankId, entryIndex);
        } else if (gSfxBanks[bankId][entryIndex].state != SFX_STATE_EMPTY) {
            // process the entry
            entry = &gSfxBanks[bankId][entryIndex];

            // Recompute distSq each frame since the sound's position may have changed
            // (later converted into dist)
            if (&gSfxDefaultPos.x == entry[0].posX) {
                entry->dist = 0.0f;
            } else {
                entryPosY = *entry->posY * 1;
                entryPosX = *entry->posX * 0.5f;
                entry->dist = (SQ(entryPosX) + SQ(entryPosY) + SQ(*entry->posZ)) / 10.0f;
            }

            // Recompute priority, possibly based on sfx position relative to the camera.
            // (Note that the priority is the opposite of importance; lower is more preserved)
            sfxImportance = entry->sfxImportance;

            if (entry->sfxParams & SFX_FLAG_PRIORITY_NO_DIST) {
                // Priority is independent of distance
                entry->priority = SQ(0xFF - sfxImportance) * SQ(76);
            } else {
                // Priority value increases with distance (more likely to eject)
                if (entry->dist > 0x7FFFFFD0) {
                    // max distance
                    entry->dist = 0x70000008;
                }

                entry->priority = (u32)entry->dist + (SQ(0xFF - sfxImportance) * SQ(76));
                if (*entry->posZ < 0.0f) {
                    entry->priority += (s32)(-*entry->posZ * 6.0f);
                }
            }

            // Sfx is too far away, do not consider for chosen sfx
            if (entry->dist > SQ(1e5f)) {

                // If too far away and also playing, stop playing
                if (entry->state == SFX_STATE_PLAYING) {
                    // ioPort 0, force stop sfx in seq 0
                    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
                    if (entry->sfxId & SFX_FLAG_MASK) {
                        AudioSfx_RemoveBankEntry(bankId, entryIndex);
                        entryIndex = k;
                    }
                }
            } else {
                // Loop through all channels allocated to a specific bank
                // Choose which sfx from gSfxBanks to play
                // This includes all sfx requests and sfx already playing
                // Sort all current sfx entries in gSfxBanks by priority
                // This is where the "active" sfx are chosen
                numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];

                for (i = 0; i < numChannels; i++) {
                    // Sort all sfx entries by priority
                    if (chosenSfx[i].priority >= entry->priority) {
                        // Update the number of sfx to attampt to play
                        if (numChosenSfx < gChannelsPerBank[gSfxChannelLayout][bankId]) {
                            numChosenSfx++;
                        }

                        for (j = numChannels - 1; j > i; j--) {
                            chosenSfx[j].priority = chosenSfx[j - 1].priority;
                            chosenSfx[j].entryIndex = chosenSfx[j - 1].entryIndex;
                        }

                        chosenSfx[i].priority = entry->priority;
                        chosenSfx[i].entryIndex = entryIndex;
                        i = numChannels; // make this the last pass through the loop, here equivalent to "break"
                    }
                }
            }

            k = entryIndex;
        }

        entryIndex = gSfxBanks[bankId][k].next;
    }

    // If a sfx is chosen, update its state
    for (i = 0; i < numChosenSfx; i++) {
        entry = &gSfxBanks[bankId][chosenSfx[i].entryIndex];

        if (entry->state == SFX_STATE_QUEUED) {
            // sfx is a new entry
            entry->state = SFX_STATE_READY;
        } else if (entry->state == SFX_STATE_PLAYING) {
            // sfx is already playing
            entry->state = SFX_STATE_PLAYING_REFRESH;
        }
    }

    // Apply the chosenSfx to the activeSfx in each channel
    numChannels = gChannelsPerBank[gSfxChannelLayout][bankId];
    for (i = 0; i < numChannels; i++) {
        needNewSfx = false;
        activeSfx = &gActiveSfx[bankId][i];

        // Check if a sfx is already in the channel
        if (activeSfx->entryIndex == 0xFF) {
            // No sfx playing in the channel
            // Available for use
            needNewSfx = true;
        } else {
            // The channel is already playing a sfx
            // Get the entry of the sfx playing
            entry = &gSfxBanks[bankId][activeSfx[0].entryIndex];

            // Check the state of the sfx playing
            if (entry->state == SFX_STATE_PLAYING) {
                if (entry->sfxId & SFX_FLAG_MASK) {
                    // For "all-frames" sfx, remove the entry
                    AudioSfx_RemoveBankEntry(bankId, activeSfx->entryIndex);
                } else {
                    // For "one-frame" sfx, reset the state to queued
                    entry->state = SFX_STATE_QUEUED;
                    entry->freshness = 0x80;
                }
                needNewSfx = true;
            } else if (entry->state == SFX_STATE_EMPTY) {
                // The sfx already in the channel is empty
                // Can replace the sfx
                activeSfx->entryIndex = 0xFF;
                needNewSfx = true;
            } else {
                // Sfx is already playing as it should, nothing to do.
                for (j = 0; j < numChannels; j++) {
                    if (activeSfx->entryIndex == chosenSfx[j].entryIndex) {
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels;
                    }
                }

                numChosenSfx--;
            }
        }

        // enter the new sfx into the activeSfx
        if (needNewSfx == true) {
            for (j = 0; j < numChannels; j++) {
                chosenEntryIndex = chosenSfx[j].entryIndex;
                if ((chosenEntryIndex != 0xFF) &&
                    (gSfxBanks[bankId][chosenEntryIndex].state != SFX_STATE_PLAYING_REFRESH)) {
                    for (k = 0; k < numChannels; k++) {
                        if (chosenEntryIndex == gActiveSfx[bankId][k].entryIndex) {
                            needNewSfx = false;
                            k = numChannels; // "break; i.e. force for-loop to end"
                        }
                    }

                    if (needNewSfx == true) {
                        activeSfx->entryIndex = chosenEntryIndex;
                        chosenSfx[j].entryIndex = 0xFF;
                        j = numChannels + 1;
                        numChosenSfx--;
                    }
                }
            }
            if (j == numChannels) {
                // nothing found
                activeSfx->entryIndex = 0xFF;
            }
        }
    }
}

void AudioSfx_PlayActiveSfx(u8 bankId) {
    u8 entryIndex;
    SequenceChannel* channel;
    SfxBankEntry* entry;
    u8 i;
    u8 ioPort5Data;

    for (i = 0; i < gChannelsPerBank[gSfxChannelLayout][bankId]; i++) {
        entryIndex = gActiveSfx[bankId][i].entryIndex;
        // If entry is not empty
        if (entryIndex != 0xFF) {
            entry = &gSfxBanks[bankId][entryIndex];
            channel = gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].channels[sCurSfxPlayerChannelIndex];

            if (entry->state == SFX_STATE_READY) {
                // Initialize a sfx (new sfx request)
                entry->channelIndex = sCurSfxPlayerChannelIndex;
                if (entry->sfxParams & SFX_FLAG_LOWER_VOLUME_BGM) {
                    AudioSfx_LowerBgmVolume(sCurSfxPlayerChannelIndex);
                }

                // Add noise that will offset the frequency of the sfx
                if ((entry->sfxParams & SFX_PARAM_RAND_FREQ_RAISE_MASK) != (0 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT)) {
                    switch (entry->sfxParams & SFX_PARAM_RAND_FREQ_RAISE_MASK) {
                        case (1 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT):
                            entry->randFreq = AudioThread_NextRandom() & 0xF;
                            break;

                        case (2 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT):
                            entry->randFreq = AudioThread_NextRandom() & 0x1F;
                            break;

                        case (3 << SFX_PARAM_RAND_FREQ_RAISE_SHIFT):
                            entry->randFreq = AudioThread_NextRandom() & 0x3F;
                            break;

                        default:
                            entry->randFreq = 0;
                            break;
                    }
                }

                // Calculate all the properties of sfx
                AudioSfx_SetProperties(bankId, entryIndex, sCurSfxPlayerChannelIndex);

                // ioPort 0, enable the sfx to play in `NA_BGM_GENERAL_SFX`
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, sCurSfxPlayerChannelIndex, 0, 1);

                // ioPort 4, write the lower bits sfx index for `NA_BGM_GENERAL_SFX` to find the right code to execute
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, sCurSfxPlayerChannelIndex, 4, entry->sfxId & 0xFF);

                // If the sfx bank has more than 255 entries (greater than a u8 can store),
                // then store the Id in upper and lower bits
                if (gIsLargeSfxBank[bankId]) {
                    // Store upper bits
                    ioPort5Data = ((u8)((entry->sfxId & 0x300) >> 7) + (u8)((entry->sfxId & 0xFF) >> 7));
                } else {
                    ioPort5Data = 0;
                }

                if ((entry->sfxParams & SFX_FLAG_8) && (entry->freshness == 0x80)) {
                    ioPort5Data += 0x80;
                }

                if (D_801D6608[bankId]) {
                    // ioPort 5, write the upper bits sfx index and a flag for `NA_BGM_GENERAL_SFX`,
                    // for banks with > 0xFF entries
                    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, sCurSfxPlayerChannelIndex, 5, ioPort5Data);
                }

                // Update playing state
                if (entry->sfxId & SFX_FLAG_MASK) {
                    // "all-frames" sfx
                    entry->state = SFX_STATE_PLAYING;
                } else {
                    // "one-frame" sfx
                    entry->state = SFX_STATE_PLAYING_ONE_FRAME;
                }
            } else if ((u8)channel->seqScriptIO[1] == (u8)SEQ_IO_VAL_NONE) {
                // Signal from seq 0 that the sfx is finished playing. Remove entry
                AudioSfx_RemoveBankEntry(bankId, entryIndex);
            } else if (entry->state == SFX_STATE_PLAYING_REFRESH) {
                // Sfx is playing but a refresh is requested
                AudioSfx_SetProperties(bankId, entryIndex, sCurSfxPlayerChannelIndex);

                // Update playing state
                if (entry->sfxId & SFX_FLAG_MASK) {
                    // "all-frames" sfx
                    entry->state = SFX_STATE_PLAYING;
                } else {
                    // "one-frame" sfx
                    entry->state = SFX_STATE_PLAYING_ONE_FRAME;
                }
            }
        }

        sCurSfxPlayerChannelIndex++;
    }
}

void AudioSfx_StopByBank(u8 bankId) {
    SfxBankEntry* entry;
    s32 pad;
    SfxBankEntry entryToRemove;
    u8 entryIndex = gSfxBanks[bankId][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[bankId][entryIndex];
        if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
        }

        if (entry->state != SFX_STATE_EMPTY) {
            AudioSfx_RemoveBankEntry(bankId, entryIndex);
        }
        entryIndex = gSfxBanks[bankId][0].next;
    }

    entryToRemove.sfxId = bankId << 12;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_BANK, &entryToRemove);
}

void AudioSfx_StopByPosAndBankImpl(u8 bankId, Vec3f* pos) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[bankId][0].next;
    u8 prevEntryIndex = 0;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[bankId][entryIndex];
        if (entry->posX == &pos->x) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }

            if (entry->state != SFX_STATE_EMPTY) {
                AudioSfx_RemoveBankEntry(bankId, entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }

        entryIndex = gSfxBanks[bankId][prevEntryIndex].next;
    }
}

void AudioSfx_StopByPosAndBank(u8 bankId, Vec3f* pos) {
    SfxBankEntry entryToRemove;

    AudioSfx_StopByPosAndBankImpl(bankId, pos);
    entryToRemove.sfxId = bankId << 12;
    entryToRemove.posX = &pos->x;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_POS_AND_BANK, &entryToRemove);
}

void AudioSfx_StopByPos(Vec3f* pos) {
    u8 bankId;
    SfxBankEntry entryToRemove;

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        AudioSfx_StopByPosAndBankImpl(bankId, pos);
    }

    entryToRemove.posX = &pos->x;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_POS, &entryToRemove);
}

void AudioSfx_StopByPosAndId(Vec3f* pos, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry entryToRemove;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if ((entry->posX == &pos->x) && (entry->sfxId == sfxId)) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }

            if (entry->state != SFX_STATE_EMPTY) {
                AudioSfx_RemoveBankEntry(SFX_BANK(sfxId), entryIndex);
            }
            entryIndex = 0xFF;
        } else {
            prevEntryIndex = entryIndex;
        }

        if (entryIndex != 0xFF) {
            entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }

    entryToRemove.posX = &pos->x;
    entryToRemove.sfxId = sfxId;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_POS_AND_ID, &entryToRemove);
}

void AudioSfx_StopByTokenAndId(u8 token, u16 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry entryToRemove;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if ((entry->token == token) && (entry->sfxId == sfxId)) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }

            if (entry->state != SFX_STATE_EMPTY) {
                AudioSfx_RemoveBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }

        if (entryIndex != 0xFF) {
            entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
        }
    }

    entryToRemove.token = token;
    entryToRemove.sfxId = sfxId;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_TOKEN_AND_ID, &entryToRemove);
}

void AudioSfx_StopById(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;
    u8 prevEntryIndex = 0;
    SfxBankEntry entryToRemove;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            if (entry->state >= SFX_STATE_PLAYING_REFRESH) {
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, entry->channelIndex, 0, 0);
            }
            if (entry->state != SFX_STATE_EMPTY) {
                AudioSfx_RemoveBankEntry(SFX_BANK(sfxId), entryIndex);
            }
        } else {
            prevEntryIndex = entryIndex;
        }

        entryIndex = gSfxBanks[SFX_BANK(sfxId)][prevEntryIndex].next;
    }

    entryToRemove.sfxId = sfxId;
    AudioSfx_RemoveMatchingRequests(SFX_RM_REQ_BY_ID, &entryToRemove);
}

void AudioSfx_ProcessRequests(void) {
    if (gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].enabled) {
        while (sSfxRequestWriteIndex != sSfxRequestReadIndex) {
            AudioSfx_ProcessRequest();
            sSfxRequestReadIndex++;
        }
    }
}

/**
 * Unused
 */
void AudioSfx_SetBankLerp(u8 bankId, u8 target, u16 delay) {
    if (delay == 0) {
        delay++;
    }

    sSfxBankLerp[bankId].target = target / 127.0f;
    sSfxBankLerp[bankId].remainingFrames = delay;
    sSfxBankLerp[bankId].step = ((sSfxBankLerp[bankId].value - sSfxBankLerp[bankId].target) / delay);
}

void AudioSfx_StepBankLerp(u8 bankId) {
    if (sSfxBankLerp[bankId].remainingFrames != 0) {
        sSfxBankLerp[bankId].remainingFrames--;

        if (sSfxBankLerp[bankId].remainingFrames != 0) {
            sSfxBankLerp[bankId].value -= sSfxBankLerp[bankId].step;
        } else {
            sSfxBankLerp[bankId].value = sSfxBankLerp[bankId].target;
        }
    }
}

void AudioSfx_ProcessActiveSfx(void) {
    u8 bankId;

    if (gAudioCtx.seqPlayers[SEQ_PLAYER_SFX].enabled) {
        sCurSfxPlayerChannelIndex = 0;

        for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
            AudioSfx_ChooseActiveSfx(bankId);
            AudioSfx_PlayActiveSfx(bankId);
            AudioSfx_StepBankLerp(bankId);
        }
    }
}

u8 AudioSfx_IsPlaying(u32 sfxId) {
    SfxBankEntry* entry;
    u8 entryIndex = gSfxBanks[SFX_BANK(sfxId)][0].next;

    while (entryIndex != 0xFF) {
        entry = &gSfxBanks[SFX_BANK(sfxId)][entryIndex];
        if (entry->sfxId == sfxId) {
            return true;
        }
        entryIndex = entry->next;
    }

    return false;
}

void AudioSfx_Reset(void) {
    u8 bankId;
    u8 i;
    u8 entryIndex;

    sSfxRequestWriteIndex = 0;
    sSfxRequestReadIndex = 0;
    sSfxChannelLowVolumeFlag = 0;

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        sSfxBankListEnd[bankId] = 0;
        sSfxBankFreeListStart[bankId] = 1;
        sSfxBankUnused[bankId] = 0;
        gSfxBankMuted[bankId] = false;
        sSfxBankLerp[bankId].value = 1.0f;
        sSfxBankLerp[bankId].remainingFrames = 0;
    }

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        for (i = 0; i < MAX_CHANNELS_PER_BANK; i++) {
            gActiveSfx[bankId][i].entryIndex = 0xFF;
        }
    }

    for (bankId = 0; bankId < ARRAY_COUNT(gSfxBanks); bankId++) {
        gSfxBanks[bankId][0].prev = 0xFF;
        gSfxBanks[bankId][0].next = 0xFF;

        for (i = 1; i < sSfxBankSizes[bankId] - 1; i++) {
            gSfxBanks[bankId][i].prev = i - 1;
            gSfxBanks[bankId][i].next = i + 1;
        }

        gSfxBanks[bankId][i].prev = i - 1;
        gSfxBanks[bankId][i].next = 0xFF;
    }
}

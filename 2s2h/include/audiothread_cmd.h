#ifndef AUDIOTHREAD_CMD_H
#define AUDIOTHREAD_CMD_H

/**
 * Audio thread commands to safely transfer information/requests/data
 * from the external graph thread to the internal audio thread
 */

typedef enum {
    // Channel Commands
    /* 0x00 */ AUDIOCMD_OP_NOOP,
    /* 0x01 */ AUDIOCMD_OP_CHANNEL_SET_VOL_SCALE,
    /* 0x02 */ AUDIOCMD_OP_CHANNEL_SET_VOL,
    /* 0x03 */ AUDIOCMD_OP_CHANNEL_SET_PAN,
    /* 0x04 */ AUDIOCMD_OP_CHANNEL_SET_FREQ_SCALE,
    /* 0x05 */ AUDIOCMD_OP_CHANNEL_SET_REVERB_VOLUME,
    /* 0x06 */ AUDIOCMD_OP_CHANNEL_SET_IO,
    /* 0x07 */ AUDIOCMD_OP_CHANNEL_SET_PAN_WEIGHT,
    /* 0x08 */ AUDIOCMD_OP_CHANNEL_SET_MUTE,
    /* 0x09 */ AUDIOCMD_OP_CHANNEL_SET_MUTE_FLAGS,
    /* 0x0A */ AUDIOCMD_OP_CHANNEL_SET_VIBRATO_DEPTH,
    /* 0x0B */ AUDIOCMD_OP_CHANNEL_SET_VIBRATO_RATE,
    /* 0x0C */ AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_SIZE,
    /* 0x0D */ AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_GAIN,
    /* 0x0E */ AUDIOCMD_OP_CHANNEL_SET_STEREO,
    /* 0x0F */ AUDIOCMD_OP_CHANNEL_SET_SET_START_POS,
    /* 0x10 */ AUDIOCMD_OP_CHANNEL_SET_SFX_STATE,
    /* 0x11 */ AUDIOCMD_OP_CHANNEL_SET_REVERB_INDEX,
    /* 0x12 */ AUDIOCMD_OP_CHANNEL_SET_SURROUND_EFFECT_INDEX,
    /* 0x13 */ AUDIOCMD_OP_CHANNEL_SET_FILTER,
    /* 0x14 */ AUDIOCMD_OP_CHANNEL_SET_GAIN,
    // SeqPlayer Commands
    /* 0x41 */ AUDIOCMD_OP_SEQPLAYER_FADE_VOLUME_SCALE = 0x41,
    /* 0x46 */ AUDIOCMD_OP_SEQPLAYER_SET_IO = 0x46,
    /* 0x47 */ AUDIOCMD_OP_SEQPLAYER_SET_TEMPO,
    /* 0x48 */ AUDIOCMD_OP_SEQPLAYER_SET_TRANSPOSITION,
    /* 0x49 */ AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO,
    /* 0x4A */ AUDIOCMD_OP_SEQPLAYER_FADE_TO_SET_VOLUME,
    /* 0x4B */ AUDIOCMD_OP_SEQPLAYER_FADE_TO_SCALED_VOLUME,
    /* 0x4C */ AUDIOCMD_OP_SEQPLAYER_RESET_VOLUME,
    /* 0x4D */ AUDIOCMD_OP_SEQPLAYER_SET_BEND,
    /* 0x4E */ AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO_TICKS,
    // Global Commands
    /* 0x81 */ AUDIOCMD_OP_GLOBAL_SYNC_LOAD_SEQ_PARTS = 0x81,
    /* 0x82 */ AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER,
    /* 0x83 */ AUDIOCMD_OP_GLOBAL_DISABLE_SEQPLAYER,
    /* 0x85 */ AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS = 0x85,
    /* 0x90 */ AUDIOCMD_OP_GLOBAL_SET_CHANNEL_MASK = 0x90,
    /* 0xE0 */ AUDIOCMD_OP_GLOBAL_SET_DRUM_FONT = 0xE0,
    /* 0xE1 */ AUDIOCMD_OP_GLOBAL_SET_SFX_FONT,
    /* 0xE2 */ AUDIOCMD_OP_GLOBAL_SET_INSTRUMENT_FONT,
    /* 0xE3 */ AUDIOCMD_OP_GLOBAL_POP_PERSISTENT_CACHE,
    /* 0xE4 */ AUDIOCMD_OP_GLOBAL_SET_CUSTOM_FUNCTION, // TODO: check
    /* 0xE5 */ AUDIOCMD_OP_GLOBAL_E5,                  // TODO: check
    /* 0xE6 */ AUDIOCMD_OP_GLOBAL_SET_REVERB_DATA,
    /* 0xF0 */ AUDIOCMD_OP_GLOBAL_SET_SOUND_MODE = 0xF0,
    /* 0xF1 */ AUDIOCMD_OP_GLOBAL_MUTE,
    /* 0xF2 */ AUDIOCMD_OP_GLOBAL_UNMUTE,
    /* 0xF3 */ AUDIOCMD_OP_GLOBAL_SYNC_LOAD_INSTRUMENT,
    /* 0xF4 */ AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SAMPLE_BANK,
    /* 0xF5 */ AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_FONT,
    /* 0xF6 */ AUDIOCMD_OP_GLOBAL_DISCARD_SEQ_FONTS,
    /* 0xF8 */ AUDIOCMD_OP_GLOBAL_STOP_AUDIOCMDS = 0xF8,
    /* 0xF9 */ AUDIOCMD_OP_GLOBAL_RESET_AUDIO_HEAP,
    /* 0xFA */ AUDIOCMD_OP_GLOBAL_NOOP_1, // used but no code exists for it
    /* 0xFB */ AUDIOCMD_OP_GLOBAL_SET_CUSTOM_UPDATE_FUNCTION,
    /* 0xFC */ AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SEQ,
    /* 0xFD */ AUDIOCMD_OP_GLOBAL_NOOP_2, // used but no code exists for it
    /* 0xFE */ AUDIOCMD_OP_GLOBAL_DISABLE_ALL_SEQPLAYERS
} AudioThreadCmdOp;

#define AUDIO_MK_CMD(b0, b1, b2, b3) (_SHIFTL(b0, 24, 8) | _SHIFTL(b1, 16, 8) | _SHIFTL(b2, 8, 8) | _SHIFTL(b3, 0, 8))

#define AUDIOCMD_ALL_SEQPLAYERS 0xFF
// Pass to a AUDIOCMD_CHANNEL_ command in place of a channelIndex to try and apply to all channels.
// Then uses `threadCmdChannelMask` to determine which channels to apply the command to.
#define AUDIOCMD_ALL_CHANNELS 0xFF

// ==== Audio Thread Channel Commands ====

/**
 * Set the volumeScale on a given channel
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param volumeScale (f32) the volume scale for the sequence. No change in volume is 1.0f
 */
#define AUDIOCMD_CHANNEL_SET_VOL_SCALE(seqPlayerIndex, channelIndex, volumeScale)                             \
    AudioThread_QueueCmdF32(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_VOL_SCALE, seqPlayerIndex, channelIndex, 0), \
                            volumeScale)

/**
 * Set the volume on a given channel
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param volume (f32) the target volume for the sequence. Default volume is 1.0f
 */
#define AUDIOCMD_CHANNEL_SET_VOL(seqPlayerIndex, channelIndex, volume) \
    AudioThread_QueueCmdF32(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_VOL, seqPlayerIndex, channelIndex, 0), volume)

/**
 * Set the pan
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param pan (s8) the pan applied to the channel
 */
#define AUDIOCMD_CHANNEL_SET_PAN(seqPlayerIndex, channelIndex, pan) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_PAN, seqPlayerIndex, channelIndex, 0), pan)

/**
 * Set frequency scale
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param freqScale (f32) the scaling factor to shift the pitch.
 */
#define AUDIOCMD_CHANNEL_SET_FREQ_SCALE(seqPlayerIndex, channelIndex, freqScale)                               \
    AudioThread_QueueCmdF32(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_FREQ_SCALE, seqPlayerIndex, channelIndex, 0), \
                            freqScale)

/**
 * Set reverb volume
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param reverbVolume (s8) volume to set the reverb to (dry/wet mix)
 */
#define AUDIOCMD_CHANNEL_SET_REVERB_VOLUME(seqPlayerIndex, channelIndex, reverbVolume)                           \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_REVERB_VOLUME, seqPlayerIndex, channelIndex, 0), \
                           reverbVolume)

/**
 * Write a value that can be read as input directly by the sequence itself. This will be set to the channel
 * ioPort, which will only affect a single channel
 *
 * @param seqPlayerIndex the index of the seqPlayer to write the input to
 * @param channelIndex the index of the channel to write the input to
 * @param ioPort the index of the array to store the input-output value
 * @param ioData (s8) the value that's written to the input-output array
 */
#define AUDIOCMD_CHANNEL_SET_IO(seqPlayerIndex, channelIndex, ioPort, ioData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_IO, seqPlayerIndex, channelIndex, ioPort), ioData)

/**
 * Set the proportion of pan that comes from the channel
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param panChannelWeight (u8) proportion of pan that comes from the channel.
 *                         Set to 0 for layer-only, and 128 for channel-only.
 */
#define AUDIOCMD_CHANNEL_SET_PAN_WEIGHT(seqPlayerIndex, channelIndex, panChannelWeight)                       \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_PAN_WEIGHT, seqPlayerIndex, channelIndex, 0), \
                           panChannelWeight)

/**
 * Mute a specified channel. How a muted channel behaves will depend on channel mute flags
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param muted (s8) set true to mute
 */
#define AUDIOCMD_CHANNEL_SET_MUTE(seqPlayerIndex, channelIndex, muted) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_MUTE, seqPlayerIndex, channelIndex, 0), muted)

/**
 * Set the muteFlags for a specified channel
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param muteFlags (s8) Affected how a muted channel behaves. See `MUTE_BEHAVIOR_` macros
 */
#define AUDIOCMD_CHANNEL_SET_MUTE_FLAGS(seqPlayerIndex, channelIndex, muteFlags) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_MUTE_FLAGS, seqPlayerIndex, channelIndex, 0), muteFlags)

/**
 * Set the vibrato depth (magnitude/amplitude/extent)
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param vibratoDepthTarget (s8) the vibrato depth scaled down by 1/8th
 */
#define AUDIOCMD_CHANNEL_SET_VIBRATO_DEPTH(seqPlayerIndex, channelIndex, vibratoDepthTarget)                     \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_VIBRATO_DEPTH, seqPlayerIndex, channelIndex, 0), \
                           vibratoDepthTarget)

/**
 * Set the vibrato rate (freq/pitch)
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param vibratoRateTarget (s8) the vibrato rate scaled down by 1/32nd
 */
#define AUDIOCMD_CHANNEL_SET_VIBRATO_RATE(seqPlayerIndex, channelIndex, vibratoRateTarget)                      \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_VIBRATO_RATE, seqPlayerIndex, channelIndex, 0), \
                           vibratoRateTarget)

/**
 * Set the comb filter size
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param combFilterSize (s8) delay number of sample bytes to offset and add back to itself
 */
#define AUDIOCMD_CHANNEL_SET_COMB_FILTER_SIZE(seqPlayerIndex, channelIndex, combFilterSize)                         \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_SIZE, seqPlayerIndex, channelIndex, 0), \
                           combFilterSize)

/**
 * Set the comb filter gain
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param combFilterGain (u16) strength of the signal added back to itself
 */
#define AUDIOCMD_CHANNEL_SET_COMB_FILTER_GAIN(seqPlayerIndex, channelIndex, combFilterGain)                          \
    AudioThread_QueueCmdU16(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_COMB_FILTER_GAIN, seqPlayerIndex, channelIndex, 0), \
                            combFilterGain)

/**
 * Set the stereo bits
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param stereoData (s8) bit-packed stereo information. See `StereoData`
 */
#define AUDIOCMD_CHANNEL_SET_STEREO(seqPlayerIndex, channelIndex, stereoData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_STEREO, seqPlayerIndex, channelIndex, 0), stereoData)

/**
 * Set the start position of a sample.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param startSamplePos
 */
#define AUDIOCMD_CHANNEL_SET_SET_START_POS(seqPlayerIndex, channelIndex, startSamplePos)                          \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_SET_START_POS, seqPlayerIndex, channelIndex, 0), \
                            startSamplePos)

/**
 * Set the address of the sfxState to read from.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param sfxState
 */
#define AUDIOCMD_CHANNEL_SET_SFX_STATE(seqPlayerIndex, channelIndex, sfxState) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_SFX_STATE, seqPlayerIndex, channelIndex, 0), (void*)sfxState)

/**
 * Set the reverb index.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param reverbIndex
 */
#define AUDIOCMD_CHANNEL_SET_REVERB_INDEX(seqPlayerIndex, channelIndex, reverbIndex)                            \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_REVERB_INDEX, seqPlayerIndex, channelIndex, 0), \
                           reverbIndex)

/**
 * Set the surround effect index.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param surroundEffectIndex
 */
#define AUDIOCMD_CHANNEL_SET_SURROUND_EFFECT_INDEX(seqPlayerIndex, channelIndex, surroundEffectIndex) \
    AudioThread_QueueCmdS8(                                                                           \
        AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_SURROUND_EFFECT_INDEX, seqPlayerIndex, channelIndex, 0), \
        surroundEffectIndex)

/**
 * Set a filter.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param filterCutoff
 * @param filter
 */
#define AUDIOCMD_CHANNEL_SET_FILTER(seqPlayerIndex, channelIndex, filterCutoff, filter)                               \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_FILTER, seqPlayerIndex, channelIndex, filterCutoff), \
                            filter)

/**
 * Set the gain.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the seqPlayer to modify
 * @param gain
 */
#define AUDIOCMD_CHANNEL_SET_GAIN(seqPlayerIndex, channelIndex, gain) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_CHANNEL_SET_GAIN, seqPlayerIndex, channelIndex, 0), gain)

// ==== Audio Thread SeqPlayer Commands ====

/**
 * Set the fade volume scale
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param fadeVolumeScale (f32) multiplicative scaling factor to apply to volume
 */
#define AUDIOCMD_SEQPLAYER_FADE_VOLUME_SCALE(seqPlayerIndex, fadeVolumeScale)                            \
    AudioThread_QueueCmdF32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_FADE_VOLUME_SCALE, seqPlayerIndex, 0, 0), \
                            fadeVolumeScale)

/**
 * Write a value that can be read as input directly by the sequence itself. This will be set to the global
 * ioPort, which can affect the entire sequence
 *
 * @param seqPlayerIndex the index of the seqPlayer to write the input to
 * @param ioPort the index of the array to store the input-output value
 * @param ioData (s8) the value that's written to the input-output array
 */
#define AUDIOCMD_SEQPLAYER_SET_IO(seqPlayerIndex, ioPort, ioData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_SET_IO, seqPlayerIndex, 0, ioPort), ioData)

/**
 * Set the tempo (bpm) of a sequence on a given seqPlayer
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param tempo (s32) the tempo for the sequence, in bpm
 */
#define AUDIOCMD_SEQPLAYER_SET_TEMPO(seqPlayerIndex, tempo) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_SET_TEMPO, seqPlayerIndex, 0, 0), tempo)

/**
 * Set the transposition
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param transposition (s8) the number of semitones to increase or decrease by for all notes on the seqPlayer
 */
#define AUDIOCMD_SEQPLAYER_SET_TRANSPOSITION(seqPlayerIndex, transposition) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_SET_TRANSPOSITION, seqPlayerIndex, 0, 0), transposition)

/**
 * Set tempoChange in bpm
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param tempoChange (s32) difference in tempo to change, in bpm
 */
#define AUDIOCMD_SEQPLAYER_CHANGE_TEMPO(seqPlayerIndex, tempoChange) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO, seqPlayerIndex, 0, 0), tempoChange)

/**
 * Set tempoChange in ticks per minute
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param tempoChange (s32) difference in tempo to change, in ticks per minute
 */
#define AUDIOCMD_SEQPLAYER_CHANGE_TEMPO_TICKS(seqPlayerIndex, tempoChange) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_CHANGE_TEMPO_TICKS, seqPlayerIndex, 0, 0), tempoChange)

/**
 * Fade the volume to the target volume requested in the command
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param fadeVolume target volume to fade to
 * @param fadeTimer (s32) number of ticks to fade to `fadeVolume`
 */
#define AUDIOCMD_SEQPLAYER_FADE_TO_SET_VOLUME(seqPlayerIndex, fadeVolume, fadeTimer)                               \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_FADE_TO_SET_VOLUME, seqPlayerIndex, fadeVolume, 0), \
                            fadeTimer)

/**
 * Fade the volume to the current volume scaled by a scale factor
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param fadeVolumeScale scaling factor to apply to volume to get the targetVolume
 * @param fadeTimer (s32) number of ticks to fade to `targetVolume`
 */
#define AUDIOCMD_SEQPLAYER_FADE_TO_SCALED_VOLUME(seqPlayerIndex, fadeVolumeScale, fadeTimer) \
    AudioThread_QueueCmdS32(                                                                 \
        AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_FADE_TO_SCALED_VOLUME, seqPlayerIndex, fadeVolumeScale, 0), fadeTimer)

/**
 * Reset to the default volume of the seqPlayer
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param fadeTimer (s32) number of ticks to fade the sequence back to its default volume
 */
#define AUDIOCMD_SEQPLAYER_RESET_VOLUME(seqPlayerIndex, fadeTimer) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_RESET_VOLUME, seqPlayerIndex, 0, 0), fadeTimer)

/**
 * Set the bend
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param bend (f32) ratio relative to 1.0f to scale channel frequencies by
 */
#define AUDIOCMD_SEQPLAYER_SET_BEND(seqPlayerIndex, bend) \
    AudioThread_QueueCmdF32(AUDIO_MK_CMD(AUDIOCMD_OP_SEQPLAYER_SET_BEND, seqPlayerIndex, 0, 0), bend)

// ==== Audio Thread Global Commands ====

/**
 * Synchronously load a sequence in parts
 *
 * @param seqId the id of the sequence to load, see `SeqId`
 * @param flags set `& 1` to load the sequence, set `& 2` to load the soundfonts
 * @param data
 */
// 2S2H [Custom Audio] encode the seqId into the second part of the command to use 16 bits for the id
#define AUDIOCMD_GLOBAL_SYNC_LOAD_SEQ_PARTS(seqId, flags, data) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SYNC_LOAD_SEQ_PARTS, 0, 0, flags), (seqId << 16) | data)

/**
 * Synchronously initialize a sequence player
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param seqId the id of the sequence to play, see `SeqId`
 * @param fadeInTimer (s32) number of ticks to fade in the sequence to the requested volume
 */
// 2S2H [Custom Audio] encode the seqId into the second part of the command to use 16 bits for the id
#define AUDIOCMD_GLOBAL_INIT_SEQPLAYER(seqPlayerIndex, seqId, fadeInTimer) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER, seqPlayerIndex, 0, 0), (seqId << 16) | fadeInTimer)

/**
 * Disable a sequence player
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param fadeOutTimer (s32) number of ticks to fade out the sequence
 */
#define AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(seqPlayerIndex, fadeOutTimer) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_DISABLE_SEQPLAYER, seqPlayerIndex, 0, 0), fadeOutTimer)

/**
 * Synchronously initialize a sequence player and skip ticks,
 * allowing the sequence to start somewhere other than the beginning of the sequences
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param seqId the id of the sequence to play, see `SeqId`
 * @param skipTicks (s32) number of ticks to skip before starting the sequence
 */
// 2S2H [Custom Audio] encode the seqId into the second part of the command to use 16 bits for the id
#define AUDIOCMD_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS(seqPlayerIndex, seqId, skipTicks)                               \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_INIT_SEQPLAYER_SKIP_TICKS, seqPlayerIndex, 0, 0), \
                            (seqId << 16) | skipTicks)

/**
 * When processing an audio thread channel command on all channels, set which channels to process
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param threadCmdChannelMask (u16) bitfield for 16 channels. Turn bit on to allow audio thread commands of type
 * "Channel" to process that channel with `AUDIOCMD_ALL_CHANNELS` set.
 */
#define AUDIOCMD_GLOBAL_SET_CHANNEL_MASK(seqPlayerIndex, threadCmdChannelMask)                       \
    AudioThread_QueueCmdU16(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_CHANNEL_MASK, seqPlayerIndex, 0, 0), \
                            threadCmdChannelMask)

/**
 * Set a drum ptr within a soundfont
 *
 * @param fontId the id of the soundfont to set the drum in
 * @param drumId the id of the drum to set
 * @param drumPtr (s32) the ptr to the `Drum` struct
 */
#define AUDIOCMD_GLOBAL_SET_DRUM_FONT(fontId, drumId, drumPtr) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_DRUM_FONT, 0, fontId, drumId), drumPtr)

/**
 * Set a soundeffect ptr within a soundfont
 *
 * @param fontId the id of the soundfont to set the sound effect in
 * @param soundEffectId the id of the sound effect to set
 * @param soundEffectPtr (s32) the ptr to the `SoundEffect` struct
 */
#define AUDIOCMD_GLOBAL_SET_SFX_FONT(fontId, soundEffectId, soundEffectPtr) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_SFX_FONT, 0, fontId, soundEffectId), soundEffectPtr)

/**
 * Set an instrument ptr within a soundfont
 *
 * @param fontId the id of the soundfont to set the instrument in
 * @param instId the id of the instrument to set
 * @param instPtr (s32) the ptr to the `Instrument` struct
 */
#define AUDIOCMD_GLOBAL_SET_INSTRUMENT_FONT(fontId, instId, instPtr) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_INSTRUMENT_FONT, 0, fontId, instId), instPtr)

/**
 * Pop the persistent cache of the specified table
 *
 * @param tableType (s32) see the `SampleBankTableType` enum
 */
#define AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(tableType) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_POP_PERSISTENT_CACHE, 0, 0, 0), tableType)

/**
 * Set a custom function.
 *
 * @param functionType
 * @param functionPtr
 */
#define AUDIOCMD_GLOBAL_SET_CUSTOM_FUNCTION(functionType, functionPtr) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_CUSTOM_FUNCTION, 0, 0, functionType), functionPtr)

/**
 * Do something related to the unloaded-type audio data in the heap.
 *
 * @param tableType
 * @param id
 * @param type
 * @param data
 */
#define AUDIOCMD_GLOBAL_E5(tableType, id, type, data) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_E5, tableType, id, type), data)

/**
 * Set reverb data.
 *
 * @param reverbIndex
 * @param dataType
 * @param data
 */
#define AUDIOCMD_GLOBAL_SET_REVERB_DATA(reverbIndex, dataType, data) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_REVERB_DATA, dataType, reverbIndex, 0), (uintptr_t)data)

/**
 * Change the sound mode of audio
 *
 * @param soundMode (s32) see the `SoundMode` enum
 */
#define AUDIOCMD_GLOBAL_SET_SOUND_MODE(soundMode) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_SOUND_MODE, 0, 0, 0), soundMode)

/**
 * Mute a sequence player
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 */
#define AUDIOCMD_GLOBAL_MUTE(seqPlayerIndex) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_MUTE, seqPlayerIndex, 0, 0), 0)

/**
 * Unmute a sequence player
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param restart (s32) if set to 1, then notes with the `MUTE_FLAGS_STOP_SAMPLES` flag set are marked as finished
 */
#define AUDIOCMD_GLOBAL_UNMUTE(seqPlayerIndex, restart) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_UNMUTE, seqPlayerIndex, 0, 0), restart)

/**
 * Synchronously load an instrument
 *
 * @param fontId the id of the soundfont to load
 * @param instId If below 0x7F, the id of the instrument to use. If equal to 0x7F, load the drum using the drumId
 * @param drumId the id of the drum to use
 */
#define AUDIOCMD_GLOBAL_SYNC_LOAD_INSTRUMENT(fontId, instId, drumId) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SYNC_LOAD_INSTRUMENT, fontId, instId, drumId), 0)

/**
 * Asynchronously load a sample bank
 *
 * @param sampleBankId the id of the samplebank to load
 * @param retData return data from `externalLoadQueue`
 */
#define AUDIOCMD_GLOBAL_ASYNC_LOAD_SAMPLE_BANK(sampleBankId, retData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SAMPLE_BANK, sampleBankId, 0, retData), 0)

/**
 * Asynchronously load a font
 *
 * @param fontId the id of the soundfont to load
 * @param retData return data from `externalLoadQueue`
 */
#define AUDIOCMD_GLOBAL_ASYNC_LOAD_FONT(fontId, retData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_FONT, fontId, 20, retData), 0)

/**
 * Discard sequence fonts
 *
 * @param seqId the id of the sequence to discard, see `SeqId`
 */
// 2S2H [Custom Audio] encode the seqId into the second part of the command to use 16 bits for the id
#define AUDIOCMD_GLOBAL_DISCARD_SEQ_FONTS(seqId) \
    AudioThread_QueueCmdU16(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_DISCARD_SEQ_FONTS, 0, 0, 0), seqId)

/**
 * Stop processing all audio thread commands
 */
#define AUDIOCMD_GLOBAL_STOP_AUDIOCMDS() \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_STOP_AUDIOCMDS, 0, 0, 0), 0)

/**
 * Reset Audio Heap
 *
 * @param specId (s32) index for the audio specifications to set high-level audio parameters
 */
#define AUDIOCMD_GLOBAL_RESET_AUDIO_HEAP(specId) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_RESET_AUDIO_HEAP, 0, 0, 0), specId)

/**
 * No Operation. No code exists for this OP
 *
 * @param arg0 No info
 * @param arg1 No info
 * @param arg2 No info
 * @param data (s32) No info
 */
#define AUDIOCMD_GLOBAL_NOOP_1(arg0, arg1, arg2, data) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_NOOP_1, arg0, arg1, arg2), data)

/**
 * Set a custom function that runs every audio thread update, see `AudioCustomUpdateFunction`
 *
 * @param functionPtr (s32) address of the function to run once every audio frame
 */
#define AUDIOCMD_GLOBAL_SET_CUSTOM_UPDATE_FUNCTION(functionPtr) \
    AudioThread_QueueCmdPtr(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_SET_CUSTOM_UPDATE_FUNCTION, 0, 0, 0), functionPtr)

/**
 * Asynchronously load a sequence
 *
 * @param seqId the id of the sequence to load, see `SeqId`
 * @param retData return data from `externalLoadQueue`
 */
// 2S2H [Custom Audio] encode the seqId into the second part of the command to use 16 bits for the id
#define AUDIOCMD_GLOBAL_ASYNC_LOAD_SEQ(seqId, retData) \
    AudioThread_QueueCmdS8(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_ASYNC_LOAD_SEQ, 0, 0, retData), seqId)

/**
 * No Operation. No code exists for this OP
 *
 * @param arg0 No info
 * @param arg1 No info
 * @param arg2 No info
 * @param data (s32) No info
 */
#define AUDIOCMD_GLOBAL_NOOP_2(arg0, arg1, arg2, data) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_NOOP_2, arg0, arg1, arg2), data)

/**
 * Disable all sequence players
 *
 * @param flags (s32) Set `AUDIO_NOTE_RELEASE` to discard all sequences.
 *
 * @note Also set `AUDIO_NOTE_SAMPLE_NOTES` will also only discard sampled notes, but the sequences are disabled anyway.
 *       Not setting `AUDIO_NOTE_RELEASE` should make this command useless
 */
#define AUDIOCMD_GLOBAL_DISABLE_ALL_SEQPLAYERS(flags) \
    AudioThread_QueueCmdS32(AUDIO_MK_CMD(AUDIOCMD_OP_GLOBAL_DISABLE_ALL_SEQPLAYERS, 0, 0, 0), flags)

#endif

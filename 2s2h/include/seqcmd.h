#ifndef SEQCMD_H
#define SEQCMD_H

// ==== Primary commands ====

#define SEQCMD_OP_MASK 0xF0000000
#define SEQCMD_ASYNC_ACTIVE 0x8000000
#define SEQCMD_SEQPLAYER_MASK 0x7000000
#define SEQCMD_SEQID_MASK 0xFF // Only applies to certain seqcmds
// 2S2H [Custom Audio] Allow for 16 bit sequence IDs. Not 0xFFFF because the top bits are for the various flags
#define SEQCMD_SEQID_MASK_16 0x7FF // Only applies to certain seqcmds

typedef enum {
    /* 0x0 */ SEQCMD_OP_PLAY_SEQUENCE,
    /* 0x1 */ SEQCMD_OP_STOP_SEQUENCE,
    /* 0x2 */ SEQCMD_OP_QUEUE_SEQUENCE,
    /* 0x3 */ SEQCMD_OP_UNQUEUE_SEQUENCE,
    /* 0x4 */ SEQCMD_OP_SET_SEQPLAYER_VOLUME,
    /* 0x5 */ SEQCMD_OP_SET_SEQPLAYER_FREQ,
    /* 0x6 */ SEQCMD_OP_SET_CHANNEL_VOLUME,
    /* 0x7 */ SEQCMD_OP_SET_SEQPLAYER_IO,
    /* 0x8 */ SEQCMD_OP_SET_CHANNEL_IO,
    /* 0x9 */ SEQCMD_OP_SET_CHANNEL_IO_DISABLE_MASK,
    /* 0xA */ SEQCMD_OP_SET_CHANNEL_DISABLE_MASK,
    /* 0xB */ SEQCMD_OP_TEMPO_CMD,
    /* 0xC */ SEQCMD_OP_SETUP_CMD,
    /* 0xD */ SEQCMD_OP_SET_CHANNEL_FREQ,
    /* 0xE */ SEQCMD_OP_GLOBAL_CMD,
    /* 0xF */ SEQCMD_OP_RESET_AUDIO_HEAP
} SeqCmdOp;

// ==== Secondary commands ====

// Subset of `SEQCMD_OP_TEMPO_CMD`
typedef enum {
    /* 0x0 */ SEQCMD_SUB_OP_TEMPO_SET,
    /* 0x1 */ SEQCMD_SUB_OP_TEMPO_SPEED_UP,
    /* 0x2 */ SEQCMD_SUB_OP_TEMPO_SLOW_DOWN,
    /* 0x3 */ SEQCMD_SUB_OP_TEMPO_SCALE,
    /* 0x4 */ SEQCMD_SUB_OP_TEMPO_RESET
} SeqCmdTempoCmdOp;

// Subset of `SEQCMD_OP_SETUP_CMD`
typedef enum {
    /* 0x0 */ SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME,
    /* 0x1 */ SEQCMD_SUB_OP_SETUP_SEQ_UNQUEUE,
    /* 0x2 */ SEQCMD_SUB_OP_SETUP_RESTART_SEQ,
    /* 0x3 */ SEQCMD_SUB_OP_SETUP_TEMPO_SCALE,
    /* 0x4 */ SEQCMD_SUB_OP_SETUP_TEMPO_RESET,
    /* 0x5 */ SEQCMD_SUB_OP_SETUP_PLAY_SEQ,
    /* 0x6 */ SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER,
    /* 0x7 */ SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_IF_QUEUED,
    /* 0x8 */ SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX,
    /* 0x9 */ SEQCMD_SUB_OP_SETUP_SET_CHANNEL_DISABLE_MASK,
    /* 0xA */ SEQCMD_SUB_OP_SETUP_SET_SEQPLAYER_FREQ,
    /* 0xE */ SEQCMD_SUB_OP_SETUP_POP_PERSISTENT_CACHE = 0xE,
    /* 0xF */ SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS
} SeqCmdSetupCmdOp;

// Subset of `SEQCMD_OP_GLOBAL_CMD`
typedef enum {
    /* 0x0 */ SEQCMD_SUB_OP_GLOBAL_SET_SOUND_MODE,
    /* 0x1 */ SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES,
    /* 0x2 */ SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES_2
} SeqCmdSubCmdOp;

// ==== Audio Sequence Primary Commands ====

/**
 * Play a sequence on a given seqPlayer
 *
 * @param seqPlayerIndex the index of the seqPlayer to play the sequence
 * @param fadeInDuration effect will depend on seqArg. See below
 * @param seqArg no effect: < 0x7F, skip ticks: = 0x7F, play after loading asynchronously: >= 0x80 (see note)
 * @param seqId the id of the sequence to play, see `SeqId`
 *
 * @note seqArg will also be stored in gActiveSeqs.seqId, any check against that seqId must also include seqArg.
 * seqArg = 0x7F  will interpret the duration as the number of ticks to skip.
 * seqArg >= 0x80 load the soundFont asynchronously, will only start playing once loaded.
 */
#define SEQCMD_PLAY_SEQUENCE(seqPlayerIndex, fadeInDuration, seqId)                                                   \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_PLAY_SEQUENCE << 28) | ((seqPlayerIndex) << 24) | ((u32)(fadeInDuration) << 16) | \
                         (u32)(seqId))

/**
 * Stop a sequence on a given seqPlayer
 *
 * @param seqPlayerIndex the index of the seqPlayer to stop the sequence
 * @param fadeOutDuration duration the sequence will fade out over
 *
 * @note the 0xFF in the command is not read from at all, but is common in all Stop SeqPlayer Commands
 */
#define SEQCMD_STOP_SEQUENCE(seqPlayerIndex, fadeOutDuration)                                    \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_STOP_SEQUENCE << 28) | 0xFF | ((u8)(seqPlayerIndex) << 24) | \
                         ((fadeOutDuration) << 16))

/**
 * Add a sequence to a queue of sequences associated with a given seqPlayer.
 * If the sequence is first in queue, play the sequence
 *
 * @param seqPlayerIndex the index of the seqPlayer to queue the sequence
 * @param fadeInDuration if the sequence is played, duration the sequence will fade in over
 * @param priority priority in the queue. The highest valued priority will be queued to play first. Also used as `seqArg`.
 * @param seqId the id of the sequence to play, see `SeqId`
 *
 * @note for the next sequence in the queue to play, the current sequence must be unqueued with the unqueue command
 * @note for a priority >= 0x7F, similar effects happen as `seqArg` in the play sequence command
 */
#define SEQCMD_QUEUE_SEQUENCE(seqPlayerIndex, fadeInDuration, priority, seqId)             \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_QUEUE_SEQUENCE << 28) | ((u8)(seqPlayerIndex) << 24) | \
                         ((u8)(fadeInDuration) << 16) | ((u8)(priority) << 8) | (u8)(seqId))

/**
 * Remove a sequence from a queue of sequences associated with a given seqPlayer.
 * If the sequence is first in queue, stop the sequence, and play the next one in queue if any
 *
 * @param seqPlayerIndex the index of the seqPlayer to queue the sequence
 * @param fadeInDuration if the sequence is stopped, duration the sequence will fade out over, and duration the next seq will fade in over
 * @param seqId the id of the sequence to remove, see `SeqId`
 *
 * @note for the next sequence in the queue to play, the current sequence must be unqueued with this command
 */
#define SEQCMD_UNQUEUE_SEQUENCE(seqPlayerIndex, fadeOutInDuration, seqId)                     \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_UNQUEUE_SEQUENCE << 28) | ((u32)(seqPlayerIndex) << 24) | \
                         ((u8)(fadeOutInDuration) << 16) | (u32)(seqId))

/**
 * Set the volume of an entire sequence on a given seqPlayer. Fade to the volume over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the volume
 * @param volume the target volume for the sequence. Ranged from 0-0xFF, with 0x7F mapping to 1.0f
 */
#define SEQCMD_SET_SEQPLAYER_VOLUME(seqPlayerIndex, duration, volume)                             \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_SEQPLAYER_VOLUME << 28) | ((u32)(seqPlayerIndex) << 24) | \
                         ((u32)(duration) << 16) | (volume))

/**
 * Scale the frequency of every channel on a given seqPlayer over a specified duration.
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the frequency
 * @param freqScale the scaling factor to shift the pitch. Ranged from 0-0xFFFF, with 1000 mapping to 1.0f
 *
 * @note 2000 will double the frequency (raise an octave), 500 will halve the frequency (lower an octave).
 *       Cannot be used with `SEQCMD_SET_CHANNEL_FREQ` as they will overwrite one another.
 */
#define SEQCMD_SET_SEQPLAYER_FREQ(seqPlayerIndex, duration, freqScale)                          \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_SEQPLAYER_FREQ << 28) | (u32)((seqPlayerIndex) << 24) | \
                         ((u32)((duration) << 16)) | (u16)(freqScale))

/**
 * Scale the frequency of a specific channel on a given seqPlayer over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param duration duration to transition to the frequency
 * @param freqScale the scaling factor to shift the pitch. Ranged from 0-0xFFF, with 1000 mapping to 1.0f
 *
 * @note a frequency of 2000 will double the frequency (raise an octave), 500 will halve the frequency (lower an
 * octave). Cannot be used with `SEQCMD_SET_SEQPLAYER_FREQ` as they will overwrite one another.
 */
#define SEQCMD_SET_CHANNEL_FREQ(seqPlayerIndex, channelIndex, duration, freqScale)                                \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_CHANNEL_FREQ << 28) | ((u8)(seqPlayerIndex) << 24) | ((duration) << 16) | \
                         ((channelIndex) << 12) | (freqScale))

/**
 * Set the volume of a specific channel on a given seqPlayer. Fade to the volume over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelIndex the index of the channel to modify
 * @param duration duration to transition to the volume
 * @param volume the target volume for the sequence. Ranged from 0-0xFF, with 0x7F mapping to 1.0f
 */
#define SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, channelIndex, duration, volume)               \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_CHANNEL_VOLUME << 28) | ((u32)(seqPlayerIndex) << 24) | \
                         ((u32)(duration) << 16) | ((u32)(channelIndex) << 8) | (volume))

/**
 * Write a value that can be read as input directly by the sequence itself. This will be set to the global
 * ioPort, which can affect the entire sequence.
 *
 * @param seqPlayerIndex the index of the seqPlayer to write the input to
 * @param ioPort the index of the array to store the input-output value
 * @param ioData the value s8 that's written to the input-output array
 *
 * @note Each seqPlayer has 8 global ioPorts indexed 0-7.
 * ioPort 0 and 1 are read-only-once, and will reset after being read by the sequence.
 * ioPort 2-7 can be read multiple times.
 */
#define SEQCMD_SET_SEQPLAYER_IO(seqPlayerIndex, ioPort, ioData)                                                       \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_SEQPLAYER_IO << 28) | ((u32)(seqPlayerIndex) << 24) | ((u32)(ioPort) << 16) | \
                         (u32)(ioData))

/**
 * Write a value that can be read as input directly by the sequence itself. This will be set to the channel
 * ioPort, which will only affect a single channel.
 *
 * @param seqPlayerIndex the index of the seqPlayer to write the input to
 * @param channelIndex the index of the channel to write the input to
 * @param ioPort the index of the array to store the input-output value
 * @param ioData the value s8 that's written to the input-output array
 *
 * @note Each channel has 8 channel ioPorts indexed 0-7.
 * ioPort 0 and 1 are read-only-once, and will reset after being read by the sequence.
 * ioPort 2-7 can be read multiple times.
 */
#define SEQCMD_SET_CHANNEL_IO(seqPlayerIndex, channelIndex, ioPort, ioData)                                         \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_CHANNEL_IO << 28) | ((u32)(seqPlayerIndex) << 24) | ((u32)(ioPort) << 16) | \
                         ((u32)(channelIndex) << 8) | (u32)(ioData))

/**
 * Disable (or reenable) specific channels from receiving input set by `SEQCMD_SET_CHANNEL_IO`
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelMask a 16 bit mask where each bit maps to a channel. Bitflag on to disable
 *
 * @note using Audio_QueueCmdS8 0x06 will bypass this channelMask
 */
#define SEQCMD_SET_CHANNEL_IO_DISABLE_MASK(seqPlayerIndex, channelMask)                                 \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_CHANNEL_IO_DISABLE_MASK << 28) | ((u8)(seqPlayerIndex) << 24) | \
                         (u16)(channelMask))

/**
 * Disable (or reenable) specific channels
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param channelMask a 16 bit mask where each bit maps to a channel. Bitflag on to disable
 */
#define SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMask)                                  \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SET_CHANNEL_DISABLE_MASK << 28) | ((u32)(seqPlayerIndex) << 24) | \
                         (u32)(channelMask))

// ==== Audio Sequence Tempo Commands ====

/**
 * Set the absolute tempo of a sequence on a given seqPlayer, fading over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 * @param tempoTarget the target tempo for the sequence
 *
 * @note the absolute tempo is constrained to a maximum of 300
 */
#define SEQCMD_SET_TEMPO(seqPlayerIndex, duration, tempoTarget)                          \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_TEMPO_CMD << 28) | (SEQCMD_SUB_OP_TEMPO_SET << 12) | \
                         ((u8)(seqPlayerIndex) << 24) | ((u8)(duration) << 16) | (u8)(tempoTarget))

/**
 * Increase the tempo of a sequence by a relative value on a given seqPlayer over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 * @param tempoIncrease the change to add to the current tempo
 *
 * @note the absolute tempo is constrained to a maximum of 300
 */
#define SEQCMD_SPEED_UP_TEMPO(seqPlayerIndex, duration, tempoIncrease)                        \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_TEMPO_CMD << 28) | (SEQCMD_SUB_OP_TEMPO_SPEED_UP << 12) | \
                         ((u8)(seqPlayerIndex) << 24) | ((u8)(duration) << 16) | (u16)(tempoIncrease))

/**
 * Decrease the tempo of a sequence by a relative value on a given seqPlayer over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 * @param tempoDecrease the change to subtract to the current tempo
 *
 * @note the absolute tempo is constrained to a maximum of 300
 */
#define SEQCMD_SLOW_DOWN_TEMPO(seqPlayerIndex, duration, tempoDecrease)                        \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_TEMPO_CMD << 28) | (SEQCMD_SUB_OP_TEMPO_SLOW_DOWN << 12) | \
                         ((u8)(seqPlayerIndex) << 24) | ((u8)(duration) << 16) | (u16)(tempoDecrease))

/**
 * Scale the tempo of a sequence by a multiplicative value on a given seqPlayer over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 * @param tempoScale the scaling factor of the tempo, relative to 100
 *
 * @note a tempoScale of 200 will go double-time, 50 will go half-time.
 * the absolute tempo is constrained to a maximum of 300
 */
#define SEQCMD_SCALE_TEMPO(seqPlayerIndex, duration, tempoScale)                                         \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_TEMPO_CMD << 28) | (SEQCMD_SUB_OP_TEMPO_SCALE << 12) | ((u8)(seqPlayerIndex) << 24) | \
                         ((u8)(duration) << 16) | (u16)(tempoScale))

/**
 * Reset the tempo of a sequence to the original tempo on a given seqPlayer over a specified duration
 *
 * @param seqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 */
#define SEQCMD_RESET_TEMPO(seqPlayerIndex, duration)                                                     \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_TEMPO_CMD << 28) | (SEQCMD_SUB_OP_TEMPO_RESET << 12) | ((u8)(seqPlayerIndex) << 24) | \
                         ((u8)(duration) << 16))

/**
 * ==== Audio Sequence Setup Commands ====
 *
 * The setup commands are designed to wait to be executed.
 * Up to 8 commands can be queued per `seqPlayerIndex`.
 * These commands will only execute once that `seqPlayerIndex` is finished playing and is no longer enabled.
 * They will often target a different player (`targetSeqPlayerIndex`) but not always.
 */

/**
 * Setup a request to restore a volume on target seqPlayer once a setup seqPlayer is finished playing and disabled
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the volume
 */
#define SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME(setupSeqPlayerIndex, targetSeqPlayerIndex, duration)            \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) | (u8)(duration))

/**
 * Setup a request to unqueue a seqPlayer once that same setup seqPlayer is finished playing and disabled
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to unqueue once the same seqPlayer is disabled
 *
 * @bug this command was misimplemented and fails to unqueue. The command relies on `gActiveSeqs.seqId` for
 * unqueueing, but seqId is reset before being used to unqueue. A simple fix is to unqueue based on
 * `gActiveSeqs.prevSeqId` instead.
 */
#define SEQCMD_SETUP_UNQUEUE_SEQUENCE(setupSeqPlayerIndex)                                       \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_SEQ_UNQUEUE << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24))

/**
 * Setup a request to restart and play an active sequence currently playing on a target seqPlayer
 * once a setup seqPlayer is finished playing and disabled
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to restart its active sequence
 */
#define SEQCMD_SETUP_RESTART_SEQUENCE(setupSeqPlayerIndex, targetSeqPlayerIndex)                 \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_RESTART_SEQ << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16))

/**
 * Setup a request to scale the tempo of a sequence by a multiplicative value on a target seqPlayer
 * once a setup seqPlayer is finished playing and disabled
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 * @param tempoScale the scaling factor of the tempo, relative to 100
 *
 * @note a tempoScale of 200 will go double-time, 50 will go half-time.
 * the absolute tempo is constrained to a maximum of 300
 */
#define SEQCMD_SETUP_SCALE_TEMPO(setupSeqPlayerIndex, targetSeqPlayerIndex, duration, tempoScale) \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_TEMPO_SCALE << 20) |  \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) | \
                         ((u8)(duration) << 8) | (u8)(tempoScale))

/**
 * Setup a request to reset the tempo of a sequence to the original tempo on a target seqPlayer
 * once a setup seqPlayer is finished playing and disabled
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the tempo
 */
#define SEQCMD_SETUP_RESET_TEMPO(setupSeqPlayerIndex, targetSeqPlayerIndex, duration)            \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_TEMPO_RESET << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) | (u8)(duration))

/**
 * Setup a request to play a sequence on a target seqPlayer once a setup seqPlayer is finished playing and disabled.
 * This command is optionally paired with `SEQCMD_SETUP_SET_FADE_IN_TIMER` to set the fade in duration
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to play the sequence
 * @param seqId the id of the sequence to play, see `SeqId`
 */
#define SEQCMD_SETUP_PLAY_SEQUENCE(setupSeqPlayerIndex, targetSeqPlayerIndex, seqId)          \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_PLAY_SEQ << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) | (u16)(seqId))

/**
 * This command is an optional command before `SEQCMD_SETUP_PLAY_SEQUENCE` to set the fade in duration
 * (see cmd above)
 *
 * @param targetSeqPlayerIndex the index of the seqPlayer to play the sequence
 * @param fadeInDuration duration the sequence will fade in over
 */
#define SEQCMD_SETUP_SET_FADE_IN_TIMER(targetSeqPlayerIndex, fadeInDuration)                        \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_SET_FADE_TIMER << 20) | \
                         ((u8)(targetSeqPlayerIndex) << 24) | ((u8)(fadeInDuration) << 8))

/**
 * Setup a request to restore a volume on target seqPlayer once a setup seqPlayer is finished playing and disabled.
 * Specifically, it will only restore volume if the number of queued requests on the setup seqPlayer matches
 * the number of sequences queued
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the volume
 * @param numSeqRequests the number of sequence requests queued that must match the actual number of sequence requests
 */
#define SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_IF_QUEUED(setupSeqPlayerIndex, targetSeqPlayerIndex, duration, \
                                                        numSeqRequests)                                      \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) |                                                       \
                         (SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_IF_QUEUED << 20) |                    \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) |            \
                         ((u8)(duration) << 8) | (u8)(numSeqRequests))

/**
 * Setup a request to restore a volume on target seqPlayer once a setup seqPlayer is finished playing and disabled.
 * Allows `scaleIndex` to be specified.
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param scaleIndex the scale index of a seqPlayer
 * @param duration duration to transition to the volume
 */
#define SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(setupSeqPlayerIndex, targetSeqPlayerIndex, scaleIndex, \
                                                               duration)                                              \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) |                                                                \
                         (SEQCMD_SUB_OP_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX << 20) |                      \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) |                     \
                         ((u8)(scaleIndex) << 8) | (u8)(duration))

/**
 * Setup a request to disable (or reenable) specific channels on a target seqPlayer
 * once a setup seqPlayer is finished playing and disabled.
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param channelMask a 16 bit mask where each bit maps to a channel. Bitflag on to disable
 */
#define SEQCMD_SETUP_SET_CHANNEL_DISABLE_MASK(setupSeqPlayerIndex, targetSeqPlayerIndex, channelMask)         \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_SET_CHANNEL_DISABLE_MASK << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) | (u16)(channelMask))

/**
 * Queue a request to scale the frequency of an entire sequence on a target seqPlayer
 * once a setup seqPlayer is finished playing and disabled.
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param targetSeqPlayerIndex the index of the seqPlayer to modify
 * @param duration duration to transition to the frequency
 * @param freqScale the scaling factor to shift the pitch. Ranged from 0-0xFF, with 100 mapping to 1.0f
 *
 * @note The base value for frequency, 100, is 10 times smaller than other frequency commands.
 * 200 will double the frequency (raise an octave), 50 will halve the frequency (lower an octave).
 */
#define SEQCMD_SETUP_SET_SEQPLAYER_FREQ(setupSeqPlayerIndex, targetSeqPlayerIndex, duration, freqScale) \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_SET_SEQPLAYER_FREQ << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)(targetSeqPlayerIndex) << 16) |       \
                         ((u8)(duration) << 8) | (u8)(freqScale))

/**
 * Queue a request to discard audio data by popping an entry from the persistent caches on the audio heap,
 * once a setup seqPlayer is finished playing and disabled.
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to wait for to be disabled
 * @param tableTypeFlag All tables with the flag `(tableTypeFlag & (1 << tableType))` will be discarded. Specifically:
 *   `(tableTypeFlag & 1)` will discard the `SEQUENCE_TABLE`
 *   `(tableTypeFlag & 2)` will discard the `FONT_TABLE`
 *   `(tableTypeFlag & 4)` will discard the `SAMPLE_TABLE`
 */
#define SEQCMD_SETUP_POP_PERSISTENT_CACHE(setupSeqPlayerIndex, tableTypeFlag)                             \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_POP_PERSISTENT_CACHE << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24) | ((u8)tableTypeFlag))

/**
 * Discard all setup command requests on a seqPlayerIndex by resetting the setup command queue
 *
 * @param setupSeqPlayerIndex the index of the seqPlayer to disable all setup commands
 */
#define SEQCMD_RESET_SETUP_CMDS(setupSeqPlayerIndex)                                                  \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_SETUP_CMD << 28) | (SEQCMD_SUB_OP_SETUP_RESET_SETUP_CMDS << 20) | \
                         ((u8)(setupSeqPlayerIndex) << 24))

/**
 * ==== Audio Sequence Global Commands ====
 *
 * The global commands will apply to the entire audio system and all 4 sequence players
 */

/**
 * Change the sound mode of audio
 *
 * @param soundMode see the `SoundMode` enum
 */
#define SEQCMD_SET_SOUND_MODE(soundMode) \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_GLOBAL_CMD << 28) | (SEQCMD_SUB_OP_GLOBAL_SET_SOUND_MODE << 8) | (soundMode))

/**
 * Disable (or reenable) new sequences from starting
 *
 * @param isDisabled true to disable, false to enable
 *
 * @note this does not disable the sfx player
 */
#define SEQCMD_DISABLE_PLAY_SEQUENCES(isDisabled)                                                           \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_GLOBAL_CMD << 28) | (SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES << 8) | \
                         (u16)(isDisabled))

#define SEQCMD_DISABLE_PLAY_SEQUENCES_2(isDisabled)                                                           \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_GLOBAL_CMD << 28) | (SEQCMD_SUB_OP_GLOBAL_DISABLE_NEW_SEQUENCES_2 << 8) | \
                         (u16)(isDisabled))

/**
 * Restart the audio heap with the specified settings
 *
 * @param fadeReverb bitmask for reverb indices indicating which reverbs to fade leading into audio heap reset
 * @param sfxChannelLayout index for different mappings between the 7 banks and the 16 channels
 * @param specId index for the audio specifications to set high-level audio parameters
 *
 * @note: For sfxChannelLayout, There are 4 possible layouts indexed by 0-3.
 * However, only index 0 is properly implemented. Other indices lead to bugs and softlocks.
 */
#define SEQCMD_RESET_AUDIO_HEAP(fadeReverb, sfxChannelLayout, specId)                    \
    AudioSeq_QueueSeqCmd((SEQCMD_OP_RESET_AUDIO_HEAP << 28) | ((u8)(fadeReverb) << 16) | \
                         ((u8)(sfxChannelLayout) << 8) | (u8)(specId))

#endif

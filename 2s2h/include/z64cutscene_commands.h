#ifndef Z64CUTSCENE_COMMANDS_H
#define Z64CUTSCENE_COMMANDS_H

#include "command_macros_base.h"
#include "z64cutscene.h"

/**
 * Cutscene scripts are arrays of `CutsceneData` words, including bit-packed integers and floats.
 */

/**
 * Marks the beginning of a cutscene script.
 * 
 * ARGS
 *   s32 totalEntries (e), s32 frameCount (n)
 * FORMAT
 *   eeeeeeee nnnnnnnn
 *   size = 0x8
 */
#define CS_BEGIN_CUTSCENE(totalEntries, frameCount) { CMD_W(totalEntries) }, { CMD_W(frameCount) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000000A eeeeeeee
 *   size = 0x8
 */
#define CS_TEXT_LIST(entries) { CS_CMD_TEXT }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 textId (i), s16 startFrame (s), s16 endFrame (e), s16 type (o),
 *   s16 topOptionBranch (y), s16 bottomOptionBranch (n)
 * FORMAT
 *   iiiissss eeeeoooo yyyynnnn
 *   size = 0xC
 */
#define CS_TEXT(textId, startFrame, endFrame, type, topOptionBranch, bottomOptionBranch) \
    { CMD_HH(textId, startFrame) }, { CMD_HH(endFrame, type) }, { CMD_HH(topOptionBranch, bottomOptionBranch) }

/**
 * ARGS
 *   s16 textId (i), s16 startFrame (s), s16 endFrame (e),
 *   s16 topOptionBranch (y), s16 bottomOptionBranch (n)
 * FORMAT
 *   iiiissss eeee0000 yyyynnnn
 *   size = 0xC
 */
#define CS_TEXT_DEFAULT(textId, startFrame, endFrame, topOptionBranch, bottomOptionBranch) \
    CS_TEXT(textId, startFrame, endFrame, CS_TEXT_TYPE_DEFAULT, topOptionBranch, bottomOptionBranch)

/**
 * ARGS
 *   s16 textId (i), s16 startFrame (s), s16 endFrame (e),
 *   s16 topOptionBranch (y), s16 bottomOptionBranch (n)
 * FORMAT
 *   iiiissss eeee0001 yyyynnnn
 *   size = 0xC
 */
#define CS_TEXT_TYPE_1(textId, startFrame, endFrame, topOptionBranch, bottomOptionBranch) \
    CS_TEXT(textId, startFrame, endFrame, CS_TEXT_TYPE_1, topOptionBranch, bottomOptionBranch)

/**
 * ARGS
 *   s16 ocarinaSongAction (o), s16 startFrame (s), s16 endFrame (e), s16 textId (i)
 * FORMAT
 *   oooossss eeee0002 iiiiFFFF
 *   size = 0xC
 */
#define CS_TEXT_OCARINA_ACTION(ocarinaSongAction, startFrame, endFrame, textId) \
    CS_TEXT(ocarinaSongAction, startFrame, endFrame, CS_TEXT_OCARINA_ACTION, textId, 0xFFFF)

/**
 * ARGS
 *   s16 textId (i), s16 startFrame (s), s16 endFrame (e),
 *   s16 topOptionBranch (y), s16 bottomOptionBranch (n)
 * FORMAT
 *   iiiissss eeee0003 yyyynnnn
 *   size = 0xC
 */
#define CS_TEXT_TYPE_3(textId, startFrame, endFrame, topOptionBranch, bottomOptionBranch) \
    CS_TEXT(textId, startFrame, endFrame, CS_TEXT_TYPE_3, topOptionBranch, bottomOptionBranch)

/**
 * If Player has all 4 bosses' remains then alternativeTextId is used, otherwise defaultTextId is used
 * ARGS
 *   s16 defaultTextId (d), s16 startFrame (s), s16 endFrame (e), s16 alternativeTextId (a)
 * FORMAT
 *   ddddssss eeee0004 aaaaFFFF
 *   size = 0xC
 */
#define CS_TEXT_BOSSES_REMAINS(defaultTextId, startFrame, endFrame, alternativeTextId) \
    CS_TEXT(defaultTextId, startFrame, endFrame, CS_TEXT_TYPE_BOSSES_REMAINS, alternativeTextId, 0xFFFF)

/**
 * If Player has every non-transformation mask then alternativeTextId is used, otherwise defaultTextId is used
 * ARGS
 *   s16 defaultTextId (d), s16 startFrame (s), s16 endFrame (e), s16 alternativeTextId (a)
 * FORMAT
 *   ddddssss eeee0005 aaaaFFFF
 *   size = 0xC
 */
#define CS_TEXT_ALL_NORMAL_MASKS(defaultTextId, startFrame, endFrame, alternativeTextId) \
    CS_TEXT(defaultTextId, startFrame, endFrame, CS_TEXT_TYPE_ALL_NORMAL_MASKS, alternativeTextId, 0xFFFF)

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   FFFFssss eeeeFFFF FFFFFFFF
 *   size = 0xC
 */
#define CS_TEXT_NONE(startFrame, endFrame) \
    CS_TEXT(0xFFFF, startFrame, endFrame, CS_TEXT_TYPE_NONE, 0xFFFF, 0xFFFF)


/**
 * ARGS
 *   s32 numBytes (b)
 * FORMAT
 *   0000005A bbbbbbbb
 *   size = 0x8
 */
#define CS_CAM_SPLINE_LIST(numBytes) { CS_CMD_CAMERA_SPLINE }, { CMD_W(numBytes) }

/**
 * ARGS
 *   s16 numEntries (e), s16 duration (d)
 * FORMAT
 *   Capital U is Unused
 *   eeeeUUUU UUUUdddd
 *   size = 0x8
 */
#define CS_CAM_SPLINE(numEntries, unused0, unused1, duration) \
    { CMD_HH(numEntries, unused0) }, { CMD_HH(unused1, duration) }

/**
 * ARGS
 *   u8 interpType (i), u8 weight (2), s16 duration (d), Vec3s pos (x/y/z), s16 relativeTo (r)
 * FORMAT
 *   ii22dddd xxxxyyyy zzzzrrrr
 *   size = 0xC
 */
#define CS_CAM_POINT(interpType, weight, duration, posX, posY, posZ, relativeTo) \
    { CMD_BBH(interpType, weight, duration) }, { CMD_HH(posX, posY) }, { CMD_HH(posZ, relativeTo) }

/**
 * ARGS
 *   s16 roll (r), s16 fov (f)
 * FORMAT
 *   Capital U is Unused
 *   UUUUrrrr ffffUUUU
 *   size = 0x8
 */
#define CS_CAM_MISC(unused0, roll, fov, unused1) \
    { CMD_HH(unused0, roll) }, { CMD_HH(fov, unused1) }

// First half-word is read from as `numEntries` in `CS_CAM_SPLINE()`
#define CS_CAM_END() { CMD_HH(0xFFFF, 4) }


/**
 * Declares a list of `CS_MISC` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000096 eeeeeeee
 *   size = 0x8
 */
#define CS_MISC_LIST(entries) { CS_CMD_MISC }, { CMD_W(entries) }

/**
 * Various miscellaneous commands.
 * @see `CutsceneMiscType` enum for the different types of commands.
 * @note setting `endFrame` to same value as `startFrame` will not behave as expected.
 * For commands that only need to last one frame, set `endFrame` to `startFrame + 1`.
 * 
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_MISC(type, startFrame, endFrame, unused0) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, unused0) }


/**
 * Declares a list of `CS_LIGHT_SETTING` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000097 eeeeeeee
 *   size = 0x8
 */
#define CS_LIGHT_SETTING_LIST(entries) { CS_CMD_LIGHT_SETTING }, { CMD_W(entries) }

/**
 * Changes the environment lights to the specified setting.
 * The lighting change will take place immediately with no blending.
 * 
 * ARGS
 *   s16 setting (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_LIGHT_SETTING(lightSetting, startFrame, endFrame) \
    { CMD_BBH(0, (lightSetting + 1), startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000098 eeeeeeee
 *   size = 0x8
 */
#define CS_TRANSITION_LIST(entries) { CS_CMD_TRANSITION }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_TRANSITION(type, startFrame, endFrame) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000099 eeeeeeee
 *   size = 0x8
 */
#define CS_MOTION_BLUR_LIST(entries) { CS_CMD_MOTION_BLUR }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_MOTION_BLUR(type, startFrame, endFrame) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000009A eeeeeeee
 *   size = 0x8
 */
#define CS_GIVE_TATL_LIST(entries) { CS_CMD_GIVE_TATL }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 giveTatl (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_GIVE_TATL(giveTatl, startFrame, endFrame) \
    { CMD_HH(giveTatl, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000009B eeeeeeee
 *   size = 0x8
 */
#define CS_TRANSITION_GENERAL_LIST(entries) { CS_CMD_TRANSITION_GENERAL }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e),
 *   u8 red (r), u8 green (g), blue (b)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeerrgg bbUUUUUU
 *   size = 0xC
 */
#define CS_TRANSITION_GENERAL(type, startFrame, endFrame, red, green, blue) \
    { CMD_HH(type, startFrame) }, { CMD_HBB(endFrame, red, green) }, { CMD_BBBB(blue, 0, 0, 0) }


/**
 * Declares a list of `CS_TIME` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000009D eeeeeeee
 *   size = 0x8
 */
#define CS_TIME_LIST(entries) { CS_CMD_TIME }, { CMD_W(entries) }

/**
 * Sets the time of day.
 * Both the day time and skybox time are set by this command.
 * @note `endFrame` is not used in the implementation of the command, so its value does not matter
 * 
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e), s8 hour (h), s8 min (m)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeehhmm UUUUUUUU
 *   size = 0xC
 */
#define CS_TIME(unused0, startFrame, endFrame, hour, min) \
    { CMD_HH(unused0, startFrame) }, { CMD_HBB(endFrame, hour, min) }, { CMD_W(0) }


/**
 * Declares a list of `CS_ACTOR_CUE` entries.
 * 
 * ARGS
 *   s32 cmdType (c), s32 entries (e)
 * FORMAT
 *   cccccccc eeeeeeee
 *   size = 0x8
 */
#define CS_ACTOR_CUE_LIST(cmdType, entries) { CMD_W(cmdType) }, { CMD_W(entries) }

/**
 * Defines a cue that an actor can listen for. 
 * The actor can choose whether or not to use the position and rotation data supplied to it.
 * The cue `id` is a number that has an actor-specific meaning.
 * 
 * ARGS
 *   s16 id (a),        s16 startFrame (s), s16 endFrame (e),
 *   s16 rotX (u),      s16 rotY (v),       s16 rotZ (w),
 *   s32 startX (i),    s32 startY (j),     s32 startZ (k),
 *   s32 endX (l),      s32 endY (m),       s32 endZ (n),
 *   f32 normX (x),     f32 normY (y),      f32 normZ (z),
 * FORMAT
 *   aaaassss eeeeuuuu vvvvwwww iiiiiiii jjjjjjjj kkkkkkkk llllllll mmmmmmmm nnnnnnnn xxxxxxxx yyyyyyyy zzzzzzzz
 *   size = 0x30
 */
#define CS_ACTOR_CUE(id, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ) \
    { CMD_HH(id, startFrame) }, { CMD_HH(endFrame, rotX) }, { CMD_HH(rotY, rotZ) }, \
    { CMD_W(startX) }, { CMD_W(startY) }, { CMD_W(startZ) }, \
    { CMD_W(endX) }, { CMD_W(endY) }, { CMD_W(endZ) }, \
    { CMD_F(normX) }, { CMD_F(normY) }, { CMD_F(normZ) }


/**
 * Declares a list of `CS_PLAYER_CUE` entries.
 * 
 * ARGS
 *   s32 cmdType (c), s32 entries (e)
 * FORMAT
 *   000000C8 eeeeeeee
 *   size = 0x8
 */
#define CS_PLAYER_CUE_LIST(entries) { CS_CMD_PLAYER_CUE }, { CMD_W(entries) }

/**
 * A player cue is the same as `CS_ACTOR_CUE` but is specifically for player. 
 * 
 * ARGS
 *   s16 id (a),         s16 startFrame (s), s16 endFrame (e),
 *   s16 rotX (u),       s16 rotY (v),       s16 rotZ (w),
 *   s32 startX (i),     s32 startY (j),     s32 startZ (k),
 *   s32 endX (l),       s32 endY (m),       s32 endZ (n),
 *   f32 normX (x),      f32 normY (y),      f32 normZ (z),
 * FORMAT
 *   aaaassss eeeeuuuu vvvvwwww iiiiiiii jjjjjjjj kkkkkkkk llllllll mmmmmmmm nnnnnnnn xxxxxxxx yyyyyyyy zzzzzzzz
 *   size = 0x30
 */
#define CS_PLAYER_CUE(id, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ) \
    CS_ACTOR_CUE(id, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ)


/**
 * Declares a list of `CS_START_SEQ` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000012C eeeeeeee
 *   size = 0x8
 */
#define CS_START_SEQ_LIST(entries) \
    { CS_CMD_START_SEQ }, { CMD_W(entries) }

/**
 * Starts a sequence at the specified time.
 * @note `endFrame` is not used in the implementation of the command, so its value does not matter
 * 
 * ARGS
 *   s16 seqId (i), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   iiiissss eeeeUUUU
 *   size = 0x8
 */
#define CS_START_SEQ(seqId, startFrame, endFrame) \
    { CMD_HH((seqId + 1), startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * Declares a list of `CS_STOP_SEQ` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000133 eeeeeeee
 *   size = 0x8
 */
#define CS_STOP_SEQ_LIST(entries) { CS_CMD_STOP_SEQ }, { CMD_W(entries) }

/**
 * Stops a sequence at the specified time.
 * 
 * ARGS
 *   s16 seqId (i), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   iiiissss eeeeUUUU
 *   size = 0x8
 */
#define CS_STOP_SEQ(seqId, startFrame, endFrame, unk_06) \
    { CMD_HH((seqId + 1), startFrame) }, { CMD_HH(endFrame, unk_06) }


/**
 *  Stops a sequence at the specified time.
 * @note `endFrame` is not used in the implementation of the command, so its value does not matter
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000009C eeeeeeee
 *   size = 0x8
 */
#define CS_FADE_OUT_SEQ_LIST(entries) { CS_CMD_FADE_OUT_SEQ }, { CMD_W(entries) }

/**
 *  Fade out the sequence that is playing on the specified sequence player, over the specified frame range.
 * @see `CutsceneFadeOutSeqPlayer`
 * 
 * ARGS
 *   s16 seqPlayer (p), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ppppssss eeeeUUUU UUUUUUUU
 *   size = 0xC
 */
#define CS_FADE_OUT_SEQ(seqPlayer, startFrame, endFrame) \
    { CMD_HH(seqPlayer, startFrame) }, { CMD_HH(endFrame, 0) }, { CMD_W(0) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000134 eeeeeeee
 *   size = 0x8
 */
#define CS_START_AMBIENCE_LIST(entries) { CS_CMD_START_AMBIENCE }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeeUUUU
 *   size = 0x8
 */
#define CS_START_AMBIENCE(unused0, startFrame, endFrame) \
    { CMD_HH(unused0, startFrame) }, { CMD_HH(endFrame, 0) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000135 eeeeeeee
 *   size = 0x8
 */
#define CS_FADE_OUT_AMBIENCE_LIST(entries) { CS_CMD_FADE_OUT_AMBIENCE }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeeUUUU
 *   size = 0x8
 */
#define CS_FADE_OUT_AMBIENCE(unused0, startFrame, endFrame) \
    { CMD_HH(unused0, startFrame) }, { CMD_HH(endFrame, 0) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000132 eeeeeeee
 *   size = 0x8
 */
#define CS_MODIFY_SEQ_LIST(entries) { CS_CMD_MODIFY_SEQ }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_MODIFY_SEQ(type, startFrame, endFrame) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000130 eeeeeeee
 *   size = 0x8
 */
#define CS_SFX_REVERB_INDEX_2_LIST(entries) { CS_CMD_SFX_REVERB_INDEX_2 }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeeUUUU
 *   size = 0x30
 */
#define CS_SFX_REVERB_INDEX_2(unused0, startFrame, endFrame) \
    { CMD_HH(unused0, startFrame) }, { CMD_HH(endFrame, 0) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000131 eeeeeeee
 *   size = 0x8
 */
#define CS_SFX_REVERB_INDEX_1_LIST(entries) { CS_CMD_SFX_REVERB_INDEX_1 }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeeUUUU
 *   size = 0x8
 */
#define CS_SFX_REVERB_INDEX_1(unused0, startFrame, endFrame) \
    { CMD_HH(unused0, startFrame) }, { CMD_HH(endFrame, 0) }


/**
 * Declares a list of `CS_DESTINATION` entries.
 * 
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000015E eeeeeeee
 *   size = 0x8
 */
#define CS_DESTINATION_LIST(entries) { CS_CMD_DESTINATION }, { CMD_W(entries) }

/**
 *  Sends the player to a new destination using the entry defined in `CutsceneScriptEntry`.
 * 
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_DESTINATION(type, startFrame, endFrame) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000015F eeeeeeee
 *   size = 0x8
 */
#define CS_CHOOSE_CREDITS_SCENES_LIST(entries) { CS_CMD_CHOOSE_CREDITS_SCENES }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_CHOOSE_CREDITS_SCENES(type, startFrame, endFrame) \
    { CMD_HH(type, startFrame) }, { CMD_HH(endFrame, endFrame) }


/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000004 eeeeeeee
 *   size = 0x8
 */
#define CS_RUMBLE_LIST(entries) { CS_CMD_RUMBLE }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 type (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU
 *   size = 0x8
 */
#define CS_RUMBLE(type, startFrame, endFrame, intensity, decayTimer, decayStep) \
    { CMD_HH(type, startFrame) }, { CMD_HBB(endFrame, intensity, decayTimer) }, { CMD_BBBB(decayStep, 0, 0, 0) }


/**
 * ARGS
 *   s32 cmd (c), s32 entries (e)
 * FORMAT
 *   cccccccc eeeeeeee
 *   size = 0x8
 */
#define CS_UNK_DATA_LIST(cmd, entries) { CMD_W(cmd) }, { CMD_W(entries) }

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   UUUUssss eeeeUUUU
 *   size = 0x8
 */
#define CS_UNK_DATA(unused0, startFrame, endFrame, unk_06) \
    { CMD_HH(unused0, startFrame) }, { CMD_HH(endFrame, unk_06) }


/**
 * Marks the end of a cutscene
 */
#define CS_END() { CMD_W(0xFFFFFFFF) }

// TODO: Fix ZAPD and delete these
#define CS_ACTOR_ACTION_LIST           CS_ACTOR_CUE_LIST
#define CS_ACTOR_ACTION                CS_ACTOR_CUE
#define CS_PLAYER_ACTION_LIST          CS_PLAYER_CUE_LIST
#define CS_PLAYER_ACTION               CS_PLAYER_CUE
#define CS_LIGHTING_LIST               CS_LIGHT_SETTING_LIST
#define CS_CAMERA_LIST                 CS_CAM_SPLINE_LIST
#define CS_TEXT_DISPLAY_TEXTBOX        CS_TEXT
#define CS_TEXT_LEARN_SONG             CS_TEXT_OCARINA_ACTION
#define CS_SCENE_TRANS_FX_LIST         CS_TRANSITION_LIST
#define CS_SCENE_TRANS_FX              CS_TRANSITION
#define CS_GIVETATL_LIST               CS_GIVE_TATL_LIST
#define CS_GIVETATL                    CS_GIVE_TATL
#define CS_PLAYSEQ_LIST                CS_START_SEQ_LIST
#define CS_STOPSEQ_LIST                CS_STOP_SEQ_LIST
#define CS_FADESEQ_LIST                CS_FADE_OUT_SEQ_LIST
#define CS_FADESEQ                     CS_FADE_OUT_SEQ
#define CS_PLAYAMBIENCE_LIST           CS_START_AMBIENCE_LIST
#define CS_PLAYAMBIENCE                CS_START_AMBIENCE
#define CS_FADEAMBIENCE_LIST           CS_FADE_OUT_AMBIENCE_LIST
#define CS_FADEAMBIENCE                CS_FADE_OUT_AMBIENCE
#define CS_SCENE_UNK_130_LIST          CS_SFX_REVERB_INDEX_2_LIST
#define CS_SCENE_UNK_130               CS_SFX_REVERB_INDEX_2
#define CS_SCENE_UNK_131_LIST          CS_SFX_REVERB_INDEX_1_LIST
#define CS_SCENE_UNK_131               CS_SFX_REVERB_INDEX_1
#define CS_SCENE_UNK_132_LIST          CS_MODIFY_SEQ_LIST
#define CS_SCENE_UNK_132               CS_MODIFY_SEQ
#define CS_MOTIONBLUR_LIST             CS_MOTION_BLUR_LIST
#define CS_MOTIONBLUR                  CS_MOTION_BLUR
#define CS_FADESCREEN_LIST             CS_TRANSITION_GENERAL_LIST
#define CS_FADESCREEN                  CS_TRANSITION_GENERAL
#define CS_TERMINATOR_LIST             CS_DESTINATION_LIST
#define CS_TERMINATOR                  CS_DESTINATION

#define CS_PLAYSEQ(seqId, startFrame, endFrame) \
CS_START_SEQ((seqId)-1, startFrame, endFrame)

#define CS_STOPSEQ(seqId, startFrame, endFrame, unk_06) \
CS_STOP_SEQ((seqId)-1, startFrame, endFrame, unk_06)

#define CS_LIGHTING(lightSetting, startFrame, endFrame) \
CS_LIGHT_SETTING((lightSetting)-1, startFrame, endFrame)

#endif

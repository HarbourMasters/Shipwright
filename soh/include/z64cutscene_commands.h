#ifndef Z64CUTSCENE_COMMANDS_H
#define Z64CUTSCENE_COMMANDS_H

#include "command_macros_base.h"
#include "z64cutscene.h"

/**
 * ARGS
 *   s32 totalEntries (e), s32 endFrame (n)
 * FORMAT
 *   eeeeeeee nnnnnnnn
 *   size = 0x8
 */
#define CS_BEGIN_CUTSCENE(totalEntries, endFrame) CMD_W(totalEntries), CMD_W(endFrame)

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   00000001 0001ssss eeee0000
 *   size = 0xC
 */
#define CS_CAM_POS_LIST CS_CAM_EYE_LIST
#define CS_CAM_EYE_LIST(startFrame, endFrame) \
    CS_CMD_CAM_EYE, CMD_HH(0x0001, startFrame), CMD_HH(endFrame, 0x0000)

/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CAM_POS CS_CAM_EYE
#define CS_CAM_EYE(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   00000002 0001ssss eeee0000
 *   size = 0xC
 */
#define CS_CAM_FOCUS_POINT_LIST CS_CAM_AT_LIST
#define CS_CAM_AT_LIST(startFrame, endFrame) \
    CS_CMD_CAM_AT, CMD_HH(0x0001, startFrame), CMD_HH(endFrame, 0x0000)

/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CAM_FOCUS_POINT CS_CAM_AT
#define CS_CAM_AT(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000003 eeeeeeee
 *   size = 0x8
 */
#define CS_MISC_LIST(entries) CS_CMD_MISC, CMD_W(entries)

/**
 * ARGS
 *   s16 unk (u), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   uuuussss eeeeUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU
 *   size = 0x30
 */
#define CS_MISC(unk, startFrame, endFrame, unused0, unused1, unused2, unused3, unused4, unused5, unused6, unused7, unused8, unused9, unused10) \
    CMD_HH(unk, startFrame), CMD_HH(endFrame, unused0), \
    CMD_W(unused1), CMD_W(unused2), CMD_W(unused3), CMD_W(unused4), CMD_W(unused5), \
    CMD_W(unused6), CMD_W(unused7), CMD_W(unused8), CMD_W(unused9), CMD_W(unused10)

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000004 eeeeeeee
 *   size = 0x8
 */
#define CS_LIGHTING_LIST(entries) CS_CMD_SET_LIGHTING, CMD_W(entries)

/**
 * ARGS
 *   s16 setting (m), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   mmmmssss eeeeUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU 00000000 00000000 00000000 
 *   size = 0x30
 */
#define CS_LIGHTING(setting, startFrame, endFrame, unused0, unused1, unused2, unused3, unused4, unused5, unused6, unused7) \
    CMD_HH(setting, startFrame), CMD_HH(endFrame, unused0), \
    CMD_W(unused1), CMD_W(unused2), CMD_W(unused3), CMD_W(unused4), CMD_W(unused5), \
    CMD_W(unused6), CMD_W(unused7), 0x00000000, 0x00000000, 0x00000000

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused , may be consistently zero
 *   00000005 0001ssss eeee0000
 *   size = 0xC
 */
#define CS_CAM_POS_PLAYER_LIST CS_CAM_EYE_REL_TO_PLAYER_LIST
#define CS_CAM_EYE_REL_TO_PLAYER_LIST(startFrame, endFrame) \
    CS_CMD_CAM_EYE_REL_TO_PLAYER, CMD_HH(0x0001, startFrame), CMD_HH(endFrame, 0x0000)

/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CAM_POS_PLAYER CS_CAM_EYE_REL_TO_PLAYER
#define CS_CAM_EYE_REL_TO_PLAYER(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused , may be consistently zero
 *   00000006 0001ssss eeee0000
 *   size = 0xC
 */
#define CS_CAM_FOCUS_POINT_PLAYER_LIST CS_CAM_AT_REL_TO_PLAYER_LIST
#define CS_CAM_AT_REL_TO_PLAYER_LIST(startFrame, endFrame) \
    CS_CMD_CAM_AT_REL_TO_PLAYER, CMD_HH(0x0001, startFrame), CMD_HH(endFrame, 0x0000)
/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CAM_FOCUS_POINT_PLAYER CS_CAM_AT_REL_TO_PLAYER
#define CS_CAM_AT_REL_TO_PLAYER(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s16 unk (u), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   00000007 uuuussss eeeeUUUU
 *   size = 0xC
 */
#define CS_CMD_07_LIST(unk, startFrame, endFrame, unused) \
    CS_CMD_07, CMD_HH(unk, startFrame), CMD_HH(endFrame, unused)

/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CMD_07(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s16 unk (u), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   00000008 uuuussss eeeeUUUU
 *   size = 0xC
 */
#define CS_CMD_08_LIST(unk, startFrame, endFrame, unused) \
    CS_CMD_08, CMD_HH(unk, startFrame), CMD_HH(endFrame, unused)

/**
 * ARGS
 *   s8 continueFlag (c), s8 roll (r), s16 frame (f), f32 viewAngle (a), 
 *   s16 xPos (x), s16 yPos (y), s16 zPos (z)
 * FORMAT
 *   Capital U is Unused
 *   ccrrffff aaaaaaaa xxxxyyyy zzzzUUUU
 *   size = 0x10
 */
#define CS_CMD_08(continueFlag, roll, frame, viewAngle, xPos, yPos, zPos, unused) \
    CMD_BBH(continueFlag, roll, frame), CMD_F(viewAngle), CMD_HH(xPos, yPos), CMD_HH(zPos, unused)

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000009 eeeeeeee
 *   size = 0x8
 */
#define CS_CMD_09_LIST(entries) CS_CMD_09, CMD_W(entries)

/**
 * ARGS
 *   s16 unk (u), s16 startFrame (s), s16 endFrame (e), s16 unk2 (v), s16 unk3 (w), s16 unk4 (x)
 * FORMAT
 *   Capital U is Unused
 *   uuuussss eeeevvww xxUUUUUU
 *   size = 0xC
 */
#define CS_CMD_09(unk, startFrame, endFrame, unk2, unk3, unk4, unused0, unused1) \
    CMD_HH(unk, startFrame), CMD_HBB(endFrame, unk2, unk3), CMD_BBH(unk4, unused0, unused1)

/**
 * ARGS
 *   s32 cmdType (c), s32 entries (e)
 * FORMAT
 *   cccccccc eeeeeeee
 *   size = 0x8
 */
#define CS_UNK_DATA_LIST(cmdType, entries) CMD_W(cmdType), CMD_W(entries)

/**
 * ARGS
 *   s32 unk1 (a), s32 unk2 (b), s32 unk3 (c), s32 unk4 (d), s32 unk5 (e), s32 unk6 (f), 
 *   s32 unk7 (g), s32 unk8 (h), s32 unk9 (i), s32 unk10 (j), s32 unk11 (k), s32 unk12 (l)
 * FORMAT
 *   aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee ffffffff gggggggg hhhhhhhh iiiiiiii jjjjjjjj kkkkkkkk llllllll
 *   size = 0x30
 */
#define CS_UNK_DATA(unk1, unk2, unk3, unk4, unk5, unk6, unk7, unk8, unk9, unk10, unk11, unk12) \
    CMD_W(unk1), CMD_W(unk2), CMD_W(unk3), CMD_W(unk4), CMD_W(unk5), CMD_W(unk6), \
    CMD_W(unk7), CMD_W(unk8), CMD_W(unk9), CMD_W(unk10), CMD_W(unk11), CMD_W(unk12)

/**
 * ARGS
 *   s32 cmdType (c), s32 entries (e)
 * FORMAT
 *   cccccccc eeeeeeee
 *   size = 0x8
 */
#define CS_NPC_ACTION_LIST(cmdType, entries) CMD_W(cmdType), CMD_W(entries)

/**
 * ARGS
 *   s16 npcAction (a), s16 startFrame (s), s16 endFrame (e),
 *   s16 rotX (u),      s16 rotY (v),       s16 rotZ (w),
 *   s32 startX (i),    s32 startY (j),     s32 startZ (k),
 *   s32 endX (l),      s32 endY (m),       s32 endZ (n),
 *   f32 normX (x),     f32 normY (y),      f32 normZ (z),
 * FORMAT
 *   aaaassss eeeeuuuu vvvvwwww iiiiiiii jjjjjjjj kkkkkkkk llllllll mmmmmmmm nnnnnnnn xxxxxxxx yyyyyyyy zzzzzzzz
 *   size = 0x30
 */
#define CS_NPC_ACTION(npcAction, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ) \
    CMD_HH(npcAction, startFrame), CMD_HH(endFrame, rotX), CMD_HH(rotY, rotZ), \
    CMD_W(startX), CMD_W(startY), CMD_W(startZ), \
    CMD_W(endX), CMD_W(endY), CMD_W(endZ), \
    CMD_F(normX), CMD_F(normY), CMD_F(normZ)

/**
 * ARGS
 *   s32 cmdType (c), s32 entries (e)
 * FORMAT
 *   cccccccc eeeeeeee
 *   size = 0x8
 */
#define CS_PLAYER_ACTION_LIST(entries) CS_CMD_SET_PLAYER_ACTION, CMD_W(entries)

/**
 * ARGS
 *   s16 linkAction (a), s16 startFrame (s), s16 endFrame (e),
 *   s16 rotX (u),       s16 rotY (v),       s16 rotZ (w),
 *   s32 startX (i),     s32 startY (j),     s32 startZ (k),
 *   s32 endX (l),       s32 endY (m),       s32 endZ (n),
 *   f32 normX (x),      f32 normY (y),      f32 normZ (z),
 * FORMAT
 *   aaaassss eeeeuuuu vvvvwwww iiiiiiii jjjjjjjj kkkkkkkk llllllll mmmmmmmm nnnnnnnn xxxxxxxx yyyyyyyy zzzzzzzz
 *   size = 0x30
 */
#define CS_PLAYER_ACTION(linkAction, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ) \
    CS_NPC_ACTION(linkAction, startFrame, endFrame, rotX, rotY, rotZ, startX, startY, startZ, endX, endY, endZ, normX, normY, normZ)

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000013 eeeeeeee
 *   size = 0x8
 */
#define CS_TEXT_LIST(entries) CS_CMD_TEXTBOX, CMD_W(entries)

/**
 * ARGS
 *   s16 messageId (i), s16 startFrame (s), s16 endFrame (e), s16 type (o), 
 *   s16 topOptionBranch (y), s16 bottomOptionBranch (n)
 * FORMAT
 *   iiiissss eeeeoooo yyyynnnn
 *   size = 0xC
 */
#define CS_TEXT_DISPLAY_TEXTBOX(messageId, startFrame, endFrame, type, topOptionBranch, bottomOptionBranch) \
    CMD_HH(messageId, startFrame), CMD_HH(endFrame, type), CMD_HH(topOptionBranch, bottomOptionBranch)

/**
 * ARGS
 *   s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   FFFFssss eeeeFFFF FFFFFFFF
 *   size = 0xC
 */
#define CS_TEXT_NONE(startFrame, endFrame) \
    CS_TEXT_DISPLAY_TEXTBOX(0xFFFF, startFrame, endFrame, 0xFFFF, 0xFFFF, 0xFFFF)

/**
 * ARGS
 *   s16 ocarinaSongAction (o), s16 startFrame (s), s16 endFrame (e), s16 topOptionBranch (i)
 * FORMAT
 *   oooossss eeee0002 iiiiFFFF
 *   size = 0xC
 */
#define CS_TEXT_LEARN_SONG(ocarinaSongAction, startFrame, endFrame, messageId) \
    CS_TEXT_DISPLAY_TEXTBOX(ocarinaSongAction, startFrame, endFrame, 0x0002, messageId, 0xFFFF)

/**
 * ARGS
 *   s16 transitionType (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused , endFrame duplicate
 *   0000002D 00000001 ttttssss eeeeUUUU
 *   size = 0x10
 */
#define CS_SCENE_TRANS_FX(transitionType, startFrame, endFrame) \
    CS_CMD_SCENE_TRANS_FX, 0x00000001, CMD_HH(transitionType, startFrame), CMD_HH(endFrame, endFrame)

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000056 eeeeeeee
 *   size = 0x8
 */
#define CS_PLAY_BGM_LIST(entries) CS_CMD_PLAYBGM, CMD_W(entries)

/**
 * ARGS
 *   s16 sequence (q), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   qqqqssss eeeeUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU 00000000 00000000 00000000
 *   size = 0x30
 */
#define CS_PLAY_BGM(sequence, startFrame, endFrame, unused0, unused1, unused2, unused3, unused4, unused5, unused6, unused7) \
    CMD_HH(sequence, startFrame), CMD_HH(endFrame, unused0), \
    CMD_W(unused1), CMD_W(unused2), CMD_W(unused3), CMD_W(unused4), CMD_W(unused5), \
    CMD_W(unused6), CMD_W(unused7), 0x00000000, 0x00000000, 0x00000000

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   00000057 eeeeeeee
 *   size = 0x8
 */
#define CS_STOP_BGM_LIST(entries) CS_CMD_STOPBGM, CMD_W(entries)

/**
 * ARGS
 *   s16 sequence (q), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   uuqqssss eeeeUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU 00000000 00000000 00000000
 *   size = 0x30
 */
#define CS_STOP_BGM(sequence, startFrame, endFrame, unused0, unused1, unused2, unused3, unused4, unused5, unused6, unused7) \
    CMD_HH(sequence, startFrame), CMD_HH(endFrame, unused0), \
    CMD_W(unused1), CMD_W(unused2), CMD_W(unused3), CMD_W(unused4), CMD_W(unused5), \
    CMD_W(unused6), CMD_W(unused7), 0x00000000, 0x00000000, 0x00000000

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000007C eeeeeeee
 *   size = 0x8
 */
#define CS_FADE_BGM_LIST(entries) CS_CMD_FADEBGM, CMD_W(entries)

/**
 * ARGS
 *   s16 fadeType (t), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused
 *   ttttssss eeeeUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU UUUUUUUU 00000000 00000000 00000000
 *   size = 0x30
 */
#define CS_FADE_BGM(fadeType, startFrame, endFrame, unused0, unused1, unused2, unused3, unused4, unused5, unused6, unused7) \
    CMD_HH(fadeType, startFrame), CMD_HH(endFrame, unused0), \
    CMD_W(unused1), CMD_W(unused2), CMD_W(unused3), CMD_W(unused4), CMD_W(unused5), \
    CMD_W(unused6), CMD_W(unused7), 0x00000000, 0x00000000, 0x00000000

/**
 * ARGS
 *   s32 entries (e)
 * FORMAT
 *   0000008C eeeeeeee
 *   size = 0x8
 */
#define CS_TIME_LIST(entries) CS_CMD_SETTIME, CMD_W(entries)

/**
 * ARGS
 *   s16 unk (u), s16 startFrame (s), s16 endFrame (e), s8 hour (h), s8 min (m)
 * FORMAT
 *   Capital U is Unused
 *   uuuussss eeeehhmm UUUUUUUU
 *   size = 0xC
 */
#define CS_TIME(unk, startFrame, endFrame, hour, min, unused) \
    CMD_HH(unk, startFrame), \
    CMD_HBB(endFrame, hour, min), \
    CMD_W(unused)

/**
 * ARGS
 *   CutsceneTerminatorDestination dest (d), s16 startFrame (s), s16 endFrame (e)
 * FORMAT
 *   Capital U is Unused , endFrame duplicate
 *   000003E8 00000001 ddddssss eeeeUUUU
 *   size = 0x10
 */
#define CS_TERMINATOR(dest, startFrame, endFrame) \
    CS_CMD_TERMINATOR, 0x00000001, CMD_HH(dest, startFrame), CMD_HH(endFrame, endFrame)

/**
 * Marks the end of a cutscene
 */
#define CS_END() 0xFFFFFFFF, 0x00000000

#endif

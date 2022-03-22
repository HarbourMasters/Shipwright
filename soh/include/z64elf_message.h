#ifndef Z64ELF_MESSAGE_H
#define Z64ELF_MESSAGE_H

#include "ultra64.h"

// Checks the condition and exits the script if the check passes
#define ELF_MSG_TYPE_CHECK  0
// ? (unused)
#define ELF_MSG_TYPE_UNK_1  1
// ? (unused)
#define ELF_MSG_TYPE_UNK_2  2
// Checks the condition and skips forward by some number of commands if the check passes
#define ELF_MSG_TYPE_SKIP   3
// Always ends the script, returning the text id for this command
#define ELF_MSG_TYPE_END    7

// Check an eventChkInf flag
#define ELF_MSG_CONDITION_FLAG          0
// Check a dungeon item (map, compass, boss key)
#define ELF_MSG_CONDITION_DUNGEON_ITEM  1
// Check if an item is in an item slot
#define ELF_MSG_CONDITION_ITEM          2
// "Other" conditions described below
#define ELF_MSG_CONDITION_OTHER         3

// Check what strength upgrade has been obtained so far
#define ELF_MSG_CONDITION_STRENGTH_UPG  0
// Check if specific boots have been obtained so far
#define ELF_MSG_CONDITION_BOOTS         1
// Check if a particular song has been obtained
#define ELF_MSG_CONDITION_SONG          2
// Check if a particular medallion has been obtained
#define ELF_MSG_CONDITION_MEDALLION     3
// Check if the magic meter has been obtained
#define ELF_MSG_CONDITION_MAGIC         4

/*
 * Bitpack byte 0
 */
#define ELF_MSG_B0(type, cond_type, tf)            \
    _SHIFTL(ELF_MSG_TYPE_##type, 5, 3) |           \
    _SHIFTL(ELF_MSG_CONDITION_##cond_type, 1, 4) | \
    _SHIFTL(tf, 0, 1)

/*
 * Bitpack byte 1
 */
#define ELF_MSG_B1(cond_type, data)                \
    _SHIFTL(ELF_MSG_CONDITION_##cond_type, 4, 4) | \
    _SHIFTL(data, 0, 4)

/*
 * Other bytes
 */
#define ELF_MSG_B(data) \
    _SHIFTL(data, 0, 8)

/*
 * Command macros
 */

#define ELF_MSG_FLAG(type, textId, tf, flag) \
    {                                        \
        ELF_MSG_B0(type, FLAG, tf),          \
        ELF_MSG_B(flag),                     \
        ELF_MSG_B(textId),                   \
        ELF_MSG_B(0),                        \
    }

#define ELF_MSG_END(textId)            \
    ELF_MSG_FLAG(END, textId, false, 0)

#define ELF_MSG_DUNGEON_ITEM(type, textId, tf, itemId) \
    {                                                  \
        ELF_MSG_B0(type, DUNGEON_ITEM, tf),            \
        ELF_MSG_B(itemId),                             \
        ELF_MSG_B(textId),                             \
        ELF_MSG_B(0),                                  \
    }

#define ELF_MSG_ITEM(type, textId, tf, slotItemId, itemId) \
    {                                                      \
        ELF_MSG_B0(type, ITEM, tf),                        \
        ELF_MSG_B(slotItemId),                             \
        ELF_MSG_B(textId),                                 \
        ELF_MSG_B(itemId),                                 \
    }

#define ELF_MSG_STRENGTH_UPG(type, textId, tf, strUpg) \
    {                                                  \
        ELF_MSG_B0(type, OTHER, tf),                   \
        ELF_MSG_B1(STRENGTH_UPG, strUpg),              \
        ELF_MSG_B(textId),                             \
        ELF_MSG_B(0),                                  \
    }

#define ELF_MSG_BOOTS(type, textId, tf, itemId) \
    {                                           \
        ELF_MSG_B0(type, OTHER, tf),            \
        ELF_MSG_B1(BOOTS, 0),                   \
        ELF_MSG_B(textId),                      \
        ELF_MSG_B(itemId),                      \
    }

#define ELF_MSG_SONG(type, textId, tf, itemId) \
    {                                           \
        ELF_MSG_B0(type, OTHER, tf),           \
        ELF_MSG_B1(SONG, 0),                   \
        ELF_MSG_B(textId),                     \
        ELF_MSG_B(itemId),                     \
    }

#define ELF_MSG_MEDALLION(type, textId, tf, itemId) \
    {                                               \
        ELF_MSG_B0(type, OTHER, tf),                \
        ELF_MSG_B1(MEDALLION, 0),                   \
        ELF_MSG_B(textId),                          \
        ELF_MSG_B(itemId),                          \
    }

#define ELF_MSG_MAGIC(type, textId, tf) \
    {                                   \
        ELF_MSG_B0(type, OTHER, tf),    \
        ELF_MSG_B1(MAGIC, 0),           \
        ELF_MSG_B(textId),              \
        ELF_MSG_B(0),                   \
    }

#endif

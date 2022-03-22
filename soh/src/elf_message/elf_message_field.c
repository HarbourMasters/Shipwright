#include "global.h"
#include "z64elf_message.h"

ElfMessage gOverworldNaviMsgs[] = {
    ELF_MSG_FLAG(CHECK, 0x40, false, 0x05), /* eventChkInf[0] & 0x20 */
    ELF_MSG_FLAG(CHECK, 0x41, false, 0x09), /* eventChkInf[0] & 0x200 */
    ELF_MSG_FLAG(CHECK, 0x42, false, 0x12), /* eventChkInf[1] & 0x4 */
    ELF_MSG_FLAG(CHECK, 0x43, false, 0x14), /* eventChkInf[1] & 0x10 */
    ELF_MSG_FLAG(CHECK, 0x44, false, 0x40), /* eventChkInf[4] & 0x1 */
    ELF_MSG_SONG(CHECK, 0x45, false, ITEM_SONG_SARIA),
    ELF_MSG_STRENGTH_UPG(CHECK, 0x46, true, 0),
    ELF_MSG_FLAG(CHECK, 0x47, false, 0x25), /* eventChkInf[2] & 0x20 */
    ELF_MSG_MAGIC(CHECK, 0x48, false),
    ELF_MSG_FLAG(CHECK, 0x49, false, 0x33), /* eventChkInf[3] & 0x8 */
    ELF_MSG_FLAG(CHECK, 0x4A, false, 0x37), /* eventChkInf[3] & 0x80 */
    ELF_MSG_FLAG(CHECK, 0x4B, false, 0x80), /* eventChkInf[8] & 0x1 */
    ELF_MSG_FLAG(CHECK, 0x4C, false, 0x43), /* eventChkInf[4] & 0x8 */
    ELF_MSG_FLAG(CHECK, 0x4D, false, 0x45), /* eventChkInf[4] & 0x20 */
    ELF_MSG_ITEM(CHECK, 0x4E, true, ITEM_HOOKSHOT, ITEM_NONE),
    ELF_MSG_MEDALLION(CHECK, 0x50, false, ITEM_MEDALLION_FOREST),
    ELF_MSG_MEDALLION(CHECK, 0x51, false, ITEM_MEDALLION_FIRE),
    ELF_MSG_BOOTS(CHECK, 0x52, false, ITEM_BOOTS_IRON),
    ELF_MSG_MEDALLION(CHECK, 0x53, false, ITEM_MEDALLION_WATER),
    ELF_MSG_FLAG(CHECK, 0x54, false, 0xAA), /* eventChkInf[10] & 0x400 */
    ELF_MSG_ITEM(CHECK, 0x55, true, ITEM_LENS, ITEM_NONE),
    ELF_MSG_MEDALLION(CHECK, 0x57, false, ITEM_MEDALLION_SHADOW),
    ELF_MSG_SONG(CHECK, 0x58, false, ITEM_SONG_REQUIEM),
    ELF_MSG_STRENGTH_UPG(CHECK, 0x56, true, 1),
    ELF_MSG_MEDALLION(CHECK, 0x5A, false, ITEM_MEDALLION_SPIRIT),
    ELF_MSG_ITEM(CHECK, 0x5B, true, ITEM_ARROW_LIGHT, ITEM_NONE),
    ELF_MSG_FLAG(CHECK, 0x5C, false, 0xC3), /* eventChkInf[12] & 0x8 */
    ELF_MSG_END(0x5F),
};

#include "global.h"
#include "z64elf_message.h"
#include <assert.h>

ElfMessage sChildSariaMsgs[] = {
    ELF_MSG_STRENGTH_UPG(SKIP, 3, false, 0),
    ELF_MSG_FLAG(CHECK, 0x61, false, 0x37), /* eventChkInf[3] & 0x80 */
    ELF_MSG_END(0x64),
    ELF_MSG_FLAG(CHECK, 0x62, false, 0x25), /* eventChkInf[2] & 0x20 */
    ELF_MSG_FLAG(CHECK, 0x63, false, 0x37), /* eventChkInf[3] & 0x80 */
    ELF_MSG_FLAG(CHECK, 0x65, false, 0x43), /* eventChkInf[4] & 0x8 */
    ELF_MSG_MEDALLION(CHECK, 0x66, false, ITEM_MEDALLION_FOREST),
    ELF_MSG_MEDALLION(CHECK, 0x66, false, ITEM_MEDALLION_FIRE),
    ELF_MSG_MEDALLION(CHECK, 0x66, false, ITEM_MEDALLION_WATER),
    ELF_MSG_SONG(CHECK, 0x67, false, ITEM_SONG_STORMS),
    ELF_MSG_MEDALLION(CHECK, 0x68, false, ITEM_MEDALLION_SPIRIT),
    ELF_MSG_MEDALLION(CHECK, 0x68, false, ITEM_MEDALLION_SHADOW),
    ELF_MSG_END(0x69),
};

ElfMessage sAdultSariaMsgs[] = {
    ELF_MSG_MEDALLION(CHECK, 0x6A, false, ITEM_MEDALLION_FOREST),
    ELF_MSG_MEDALLION(CHECK, 0x6B, false, ITEM_MEDALLION_FIRE),
    ELF_MSG_MEDALLION(CHECK, 0x6B, false, ITEM_MEDALLION_WATER),
    ELF_MSG_MEDALLION(CHECK, 0x6C, false, ITEM_MEDALLION_SPIRIT),
    ELF_MSG_MEDALLION(CHECK, 0x6C, false, ITEM_MEDALLION_SHADOW),
    ELF_MSG_END(0x6D),
};

u32 ElfMessage_CheckCondition(ElfMessage* msg) {
    s32 type = msg->byte0 & 0x1E;
    u16 flag;

    switch (type) {
        case (ELF_MSG_CONDITION_FLAG << 1):
            flag = 1 << (msg->byte1 & 0x0F);
            return ((msg->byte0 & 1) == 1) == ((flag & gSaveContext.eventChkInf[(msg->byte1 & 0xF0) >> 4]) != 0);
        case (ELF_MSG_CONDITION_DUNGEON_ITEM << 1):
            return ((msg->byte0 & 1) == 1) ==
                   (CHECK_DUNGEON_ITEM(msg->byte1 - ITEM_KEY_BOSS, gSaveContext.mapIndex) != 0);
        case (ELF_MSG_CONDITION_ITEM << 1):
            return ((msg->byte0 & 1) == 1) == (msg->byte3 == INV_CONTENT(msg->byte1));
        case (ELF_MSG_CONDITION_OTHER << 1):
            switch (msg->byte1 & 0xF0) {
                case (ELF_MSG_CONDITION_STRENGTH_UPG << 4):
                    return ((msg->byte0 & 1) == 1) == ((msg->byte1 & 0x0F) == CUR_UPG_VALUE(UPG_STRENGTH));
                case (ELF_MSG_CONDITION_BOOTS << 4):
                    return ((msg->byte0 & 1) == 1) ==
                           (((gBitFlags[msg->byte3 - ITEM_BOOTS_KOKIRI] << gEquipShifts[EQUIP_BOOTS]) &
                             gSaveContext.inventory.equipment) != 0);
                case (ELF_MSG_CONDITION_SONG << 4):
                    return ((msg->byte0 & 1) == 1) ==
                           (CHECK_QUEST_ITEM(msg->byte3 - ITEM_SONG_MINUET + QUEST_SONG_MINUET) != 0);
                case (ELF_MSG_CONDITION_MEDALLION << 4):
                    return ((msg->byte0 & 1) == 1) ==
                           (CHECK_QUEST_ITEM(msg->byte3 - ITEM_MEDALLION_FOREST + QUEST_MEDALLION_FOREST) != 0);
                case (ELF_MSG_CONDITION_MAGIC << 4):
                    return ((msg->byte0 & 1) == 1) == (((void)0, gSaveContext.isMagicAcquired) != 0);
            }
    }

    LOG_STRING("企画外 条件"); // "Unplanned conditions"
    assert(0);

    return false;
}

u32 func_8006BE88(ElfMessage** msgp) {
    u32 temp = true;

    while (((*msgp)->byte0 & 0xE0) == (ELF_MSG_TYPE_UNK_1 << 5)) {
        if (!ElfMessage_CheckCondition(*msgp)) {
            temp = false;
        }
        *msgp += 1;
    }

    if (temp) {
        return ElfMessage_CheckCondition(*msgp);
    } else {
        return false;
    }
}

u32 func_8006BF1C(ElfMessage** msgp) {
    ElfMessage* msg = *msgp;
    u32 sp44[10];
    s32 temp1 = 0;
    s32 temp2 = 0;
    s32 temp3;

    do {
        sp44[temp2] = ElfMessage_CheckCondition(msg);
        temp1 += sp44[temp2];
        temp2++;
        msg++;
    } while ((msg->byte0 & 0xE0) == (ELF_MSG_TYPE_UNK_2 << 5));

    if (temp1 == 0) {
        return false;
    }

    temp3 = Rand_ZeroFloat(temp1);
    for (temp1 = 0; temp1 < temp2; temp1++) {
        if (sp44[temp1]) {
            if (temp3 > 0) {
                temp3--;
            } else {
                return true;
            }
        }
        *msgp += 1;
    }

    return false;
}

u16 ElfMessage_GetTextFromMsgs(ElfMessage* msg) {
    while (true) {
        switch (msg->byte0 & 0xE0) {
            case (ELF_MSG_TYPE_CHECK << 5):
                if (ElfMessage_CheckCondition(msg)) {
                    return msg->byte2 | 0x100;
                }
                break;
            case (ELF_MSG_TYPE_UNK_1 << 5):
                if (func_8006BE88(&msg)) {
                    return msg->byte2 | 0x100;
                }
                break;
            case (ELF_MSG_TYPE_UNK_2 << 5):
                if (func_8006BF1C(&msg)) {
                    return msg->byte2 | 0x100;
                }
                break;
            case (ELF_MSG_TYPE_SKIP << 5):
                if (ElfMessage_CheckCondition(msg)) {
                    msg += msg->byte2;
                    msg--;
                }
                break;
            case (ELF_MSG_TYPE_END << 5):
                return msg->byte2 | 0x100;
            default:
                LOG_STRING("企画外 条件"); // "Unplanned conditions"
                assert(0);
        }
        msg++;
    }
}

u16 ElfMessage_GetSariaText(PlayState* play) {
    Player* player = GET_PLAYER(play);
    ElfMessage* msgs;

    if (!LINK_IS_ADULT) {
        if (Actor_FindNearby(play, &player->actor, ACTOR_EN_SA, 4, 800.0f) == NULL) {
             if (IS_RANDO && Randomizer_GetSettingValue(RSK_SARIA_HINT)) {
                return 0x161;
            }
            msgs = sChildSariaMsgs;
        } else {
            return 0x0160; // Special text about Saria preferring to talk to you face-to-face
        }
    } else {
        msgs = sAdultSariaMsgs;
    }

    return ElfMessage_GetTextFromMsgs(msgs);
}

u16 ElfMessage_GetCUpText(PlayState* play) {
    if (play->cUpElfMsgs == NULL) {
        return 0;
    } else {
        return ElfMessage_GetTextFromMsgs(play->cUpElfMsgs);
    }
}

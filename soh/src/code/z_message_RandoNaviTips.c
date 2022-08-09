#include "global.h"
#include "message_data_static.h"
#include "vt.h"

#include <string.h>

// Function for letting Navi give general rando tips to the player instead of her
// normal generic quest tips.  

// Only applies to Navi's overworld quest hints - does not apply to enemy information
// or contextual hints (e.g. "This hallway is twisted!" in Forest Temple)

void RandoNaviTip(GlobalContext* globalCtx) {
    u16 randNaviTip = rand() % 4;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    Font* font = &msgCtx->font;

    if (randNaviTip == 0) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08\x05\x44""French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08\x05\x44German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x08\x05\x44Missing a small key in a dungeon?\x01Maybe the\x05\x41 "
                                     "boss\x05\x44 has it!\x02");
                break;
        }
    }
    if (randNaviTip == 1) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08\x05\x44""French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x08\x05\x44Sometimes you can use the \x05\x41Megaton\x01Hammer \x05\x44"
                                     "instead of bombs!\x02");
                break;
        }
    }
    if (randNaviTip == 2) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08\x05\x44""French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf,
                       "\x08\x05\x44There are three\x05\x42 business scrubs\x05\x44 in\x01"
                       "Hyrule who sell \x05\x49mysterious items\x05\x44. Do\x01you know where they are?\x02");
                break;
        }
    }
    if (randNaviTip == 3) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08\x05\x44"
                                     "French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf,
                       "\x08\x05\x44Stuck on this seed? You could\x01"
                       "throw in the towel and check the \x01"
                       "\x05\x49spoiler log\x05\x44...\x02");
                break;
        }
    }
}
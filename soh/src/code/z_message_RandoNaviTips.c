#include "global.h"
#include "message_data_static.h"
#include "vt.h"

#include <string.h>

void RandoNaviTip(GlobalContext* globalCtx) {
    u16 randNaviTip = rand() % 3;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    Font* font = &msgCtx->font;

    if (randNaviTip == 0) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x08Missing a small key in a dungeon?\x01Maybe the\x05\x43 "
                                     "boss\x05\x40 has it!\x02");
                break;
        }
    }
    if (randNaviTip == 1) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x08Sometimes you can use the \x05\x41Megaton\x01Hammer \x05\x40"
                                     "instead of bombs!\x02");
                break;
        }
    }
    if (randNaviTip == 2) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x08French tip about playing rando!\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x08German tip about playing rando!\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf,
                       "\x08There are three\x05\x42 business scrubs\x05\x40 in\x01"
                       "Hyrule who sell \x05\x44mysterious items\x05\x40. Do\x01you know where they are?\x02");
                break;
        }
    }
}
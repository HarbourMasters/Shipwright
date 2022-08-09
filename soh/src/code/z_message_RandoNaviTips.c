#include "global.h"
#include "message_data_static.h"
#include "vt.h"

#include <string.h>

// Function for letting Navi give general rando tips to the player instead of her
// normal generic quest tips.

// Only applies to Navi's overworld quest hints - does not apply to enemy information
// or contextual hints (e.g. "This hallway is twisted!" in Forest Temple)

void RandoNaviTip(GlobalContext* globalCtx) {
    u16 randNaviTip = rand() % 15; // Random number between 0 and total number of Navi tips
    MessageContext* msgCtx = &globalCtx->msgCtx;
    Font* font = &msgCtx->font;

    if (randNaviTip == 0) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 0\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 0\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Missing a small key in a dungeon?\x01"
                                     "Maybe the\x05\x41 boss\x05\x44 has it!\x02");
                break;
        }
    }
    if (randNaviTip == 1) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 1\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 1\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Sometimes you can use the \x05\x41Megaton\x01"
                                     "Hammer \x05\x44instead of bombs!\x02");
                break;
        }
    }
    if (randNaviTip == 2) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 2\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 2\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "There are three\x05\x42 business scrubs\x05\x44 in\x01"
                                     "Hyrule who sell \x05\x49mysterious items\x05\x44. Do\x01"
                                     "you know where they are?\x02");
                break;
        }
    }
    if (randNaviTip == 3) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 3\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 3\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Stuck on this seed? You could\x01"
                                     "throw in the towel and check the \x01"
                                     "\x05\x49spoiler log\x05\x44...\x02");
                break;
        }
    }
    if (randNaviTip == 4) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 4\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 4\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Did you know that the \x05\x46Hover\x01"
                                     "Boots\x05\x44 can be used to cross \x01"
                                     "quicksand?\x02");
                break;
        }
    }
    if (randNaviTip == 5) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 5\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 5\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "You can get into the back of\x01\x05\x49"
                                     "Impa's House\x05\x44 by jumping from the\x01"
                                     "unfinished house with a \x05\x41"
                                     "Cucco\x05\x44!\x02");
                break;
        }
    }
    if (randNaviTip == 6) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 6\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 6\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "The \x05\x46Spirit Trial \x05\x44in \x05\x45Ganon's Castle\x05\x44\x01"
                                     "has a\x05\x40 hidden chest\x05\x44 somewhere.\x01"
                                     "Did you already know that?\x02");
                break;
        }
    }
    if (randNaviTip == 7) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 7\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 7\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "You know the \x05\x40"
                                     "Bombchu Bowling\x01"
                                     "Alley\x05\x44? I heard \x05\x40only two of the\x01"
                                     "prizes\x05\x44 are worthwhile. The rest\x01"
                                     "is junk!\x02");
                break;
        }
    }
    if (randNaviTip == 8) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 8\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 8\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Have you been using \x05\x40"
                                     "Deku Nuts \x05\x44\x01"
                                     "enough? I've seen them blow up\x01"
                                     "a \x05\x41"
                                     "Bombchu\x05\x44!\x02");
                break;
        }
    }
    if (randNaviTip == 9) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 9\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 9\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "You might end up with an \x05\x40"
                                     "extra\x01"
                                     "key\x05\x44 for the \x05\x43Water Temple\x05\x44 or the \x05\x41"
                                     "Fire\x01"
                                     "Temple\x05\x44. It's for your safety!\x02");
                break;
        }
    }
    if (randNaviTip == 10) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 10\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 10\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "If you can't pick up a \x05\x41"
                                     "bomb\x01"
                                     "flower \x05\x44with your hands, you\x01"
                                     "can still detonate it with \x01"
                                     "an \x05\x40"
                                     "arrow\x05\x44!\x02");
                break;
        }
    }
    if (randNaviTip == 11) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 11\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 11\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Even an adult can't push large\x01"
                                     "blocks without some \x05\x40"
                                     "extra\x01"
                                     "strength\x05\x44!\x02");
                break;
        }
    }
    if (randNaviTip == 12) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 12\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 12\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "I've heard that\x05\x41 Flare Dancer\x05\x44\x01"
                                     "is weak to the \x05\x40Master Sword\x05\x44!\x01"
                                     "Have you tried it?\x02");
                break;
        }
    }
    if (randNaviTip == 13) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 12\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 12\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "Don't have a weapon to kill a\x01"
                                     "\x05\x41spider\x05\x44? Try throwing a \x05\x40pot\x01"
                                     "\x05\x44"
                                     "at it!\x02");
                break;
        }
    }
    if (randNaviTip == 14) {
        switch (gSaveContext.language) {
            case LANGUAGE_FRA:
                strcpy(font->msgBuf, "\x05\x44"
                                     "French tip about playing rando! 12\x02");
                break;
            case LANGUAGE_GER:
                strcpy(font->msgBuf, "\x05\x44"
                                     "German tip about playing rando! 12\x02");
                break;
            case LANGUAGE_ENG:
            default:
                strcpy(font->msgBuf, "\x05\x44"
                                     "I hear the patch of \x05\x40soft soil\x05\x44\x01"
                                     "in \x05\x43Zora's River\x05\x44 is the only one\x01"
                                     "that isn't home to a \x05\x41spider\x05\x44!\x02");
                break;
        }
    }
}
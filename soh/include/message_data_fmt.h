#ifndef MESSAGE_DATA_FMT_H
#define MESSAGE_DATA_FMT_H

/*
 * Macros to create both a constant and a string literal from a magic value
 *   The constants are used in code files when parsing text for various purposes
 *   The strings are used in the message_data_static files themselves, as you can only concat strings with other strings
 */

#define GLUE(a, b) a##b

#define STRINGIFY(s) #s
#define EXPAND_AND_STRINGIFY(s) STRINGIFY(s)

#define HEX(N) GLUE(0x, N)
#define STR(N) EXPAND_AND_STRINGIFY(GLUE(\x, N))

/*
 * Text control characters
 */

// Control character magic values, in 2-digit hex without prefix

#define CTRL_NEWLINE                 01
#define CTRL_END                     02
#define CTRL_BOX_BREAK               04
#define CTRL_COLOR                   05
#define CTRL_SHIFT                   06
#define CTRL_TEXTID                  07
#define CTRL_QUICKTEXT_ENABLE        08
#define CTRL_QUICKTEXT_DISABLE       09
#define CTRL_PERSISTENT              0A
#define CTRL_EVENT                   0B
#define CTRL_BOX_BREAK_DELAYED       0C
#define CTRL_AWAIT_BUTTON_PRESS      0D
#define CTRL_FADE                    0E
#define CTRL_NAME                    0F
#define CTRL_OCARINA                 10
#define CTRL_FADE2                   11
#define CTRL_SFX                     12
#define CTRL_ITEM_ICON               13
#define CTRL_TEXT_SPEED              14
#define CTRL_BACKGROUND              15
#define CTRL_MARATHON_TIME           16
#define CTRL_RACE_TIME               17
#define CTRL_POINTS                  18
#define CTRL_TOKENS                  19
#define CTRL_UNSKIPPABLE             1A
#define CTRL_TWO_CHOICE              1B
#define CTRL_THREE_CHOICE            1C
#define CTRL_FISH_INFO               1D
#define CTRL_HIGHSCORE               1E
#define CTRL_TIME                    1F

/*
 *  Colors
 */ 

#define COLOR_STR(N) EXPAND_AND_STRINGIFY(GLUE(\x4, N))

// Color magic values, in single-digit hex without prefix

#define CTRL_DEFAULT    0
#define CTRL_RED        1
#define CTRL_ADJUSTABLE 2
#define CTRL_BLUE       3
#define CTRL_LIGHTBLUE  4
#define CTRL_PURPLE     5
#define CTRL_YELLOW     6
#define CTRL_BLACK      7

#ifdef MESSAGE_DATA_STATIC
// For use in message_data_static files

#define ARG(x) x

// while a control character, newlines are handled in the charmap conversion 
// stage to allow normal newline \n usage in message_data_static files
#define NEWLINE                 STR(CTRL_NEWLINE)
#define END                     STR(CTRL_END)
#define BOX_BREAK               STR(CTRL_BOX_BREAK)
#define COLOR(x)                STR(CTRL_COLOR)                ARG(x) // 1
#define SHIFT(x)                STR(CTRL_SHIFT)                ARG(x) // 1
#define TEXTID(x)               STR(CTRL_TEXTID)               ARG(x) // 2
#define QUICKTEXT_ENABLE        STR(CTRL_QUICKTEXT_ENABLE)
#define QUICKTEXT_DISABLE       STR(CTRL_QUICKTEXT_DISABLE)
#define PERSISTENT              STR(CTRL_PERSISTENT)
#define EVENT                   STR(CTRL_EVENT)
#define BOX_BREAK_DELAYED(x)    STR(CTRL_BOX_BREAK_DELAYED)    ARG(x) // 1
#define AWAIT_BUTTON_PRESS      STR(CTRL_AWAIT_BUTTON_PRESS)
#define FADE(x)                 STR(CTRL_FADE)                 ARG(x) // 1
#define NAME                    STR(CTRL_NAME)
#define OCARINA                 STR(CTRL_OCARINA)
#define FADE2(x)                STR(CTRL_FADE2)                ARG(x) // 2
#define SFX(x)                  STR(CTRL_SFX)                  ARG(x) // 2
#define ITEM_ICON(x)            STR(CTRL_ITEM_ICON)            ARG(x) // 1
#define TEXT_SPEED(x)           STR(CTRL_TEXT_SPEED)           ARG(x) // 1
#define BACKGROUND(x,y,z)       STR(CTRL_BACKGROUND)           ARG(x) ARG(y) ARG(z)
#define MARATHON_TIME           STR(CTRL_MARATHON_TIME)
#define RACE_TIME               STR(CTRL_RACE_TIME)
#define POINTS                  STR(CTRL_POINTS)
#define TOKENS                  STR(CTRL_TOKENS)
#define UNSKIPPABLE             STR(CTRL_UNSKIPPABLE)
#define TWO_CHOICE              STR(CTRL_TWO_CHOICE)
#define THREE_CHOICE            STR(CTRL_THREE_CHOICE)
#define FISH_INFO               STR(CTRL_FISH_INFO)
#define HIGHSCORE(x)            STR(CTRL_HIGHSCORE)            ARG(x) // 1
#define TIME                    STR(CTRL_TIME)

/*
 * Highscore values as strings, for code references the HighScores
 * enum should be used.
 */

#define HS_HORSE_ARCHERY "\x00"
#define HS_POE_POINTS    "\x01"
#define HS_LARGEST_FISH  "\x02"
#define HS_HORSE_RACE    "\x03"
#define HS_MARATHON      "\x04"
#define HS_DAMPE_RACE    "\x06"

/*
 * Color values as strings
 */

#define DEFAULT     COLOR_STR(CTRL_DEFAULT)
#define RED         COLOR_STR(CTRL_RED)
#define ADJUSTABLE  COLOR_STR(CTRL_ADJUSTABLE)
#define BLUE        COLOR_STR(CTRL_BLUE)
#define LIGHTBLUE   COLOR_STR(CTRL_LIGHTBLUE)
#define PURPLE      COLOR_STR(CTRL_PURPLE)
#define YELLOW      COLOR_STR(CTRL_YELLOW)
#define BLACK       COLOR_STR(CTRL_BLACK)

#else
// For use in code files
#define MSGCOL_DEFAULT      HEX(CTRL_DEFAULT)
#define MSGCOL_RED          HEX(CTRL_RED)
#define MSGCOL_ADJUSTABLE   HEX(CTRL_ADJUSTABLE)
#define MSGCOL_BLUE         HEX(CTRL_BLUE)
#define MSGCOL_LIGHTBLUE    HEX(CTRL_LIGHTBLUE)
#define MSGCOL_PURPLE       HEX(CTRL_PURPLE)
#define MSGCOL_YELLOW       HEX(CTRL_YELLOW)
#define MSGCOL_BLACK        HEX(CTRL_BLACK)

#define MESSAGE_NEWLINE                 HEX(CTRL_NEWLINE)
#define MESSAGE_END                     HEX(CTRL_END)
#define MESSAGE_BOX_BREAK               HEX(CTRL_BOX_BREAK)
#define MESSAGE_COLOR                   HEX(CTRL_COLOR)
#define MESSAGE_SHIFT                   HEX(CTRL_SHIFT)
#define MESSAGE_TEXTID                  HEX(CTRL_TEXTID)
#define MESSAGE_QUICKTEXT_ENABLE        HEX(CTRL_QUICKTEXT_ENABLE)
#define MESSAGE_QUICKTEXT_DISABLE       HEX(CTRL_QUICKTEXT_DISABLE)
#define MESSAGE_PERSISTENT              HEX(CTRL_PERSISTENT)
#define MESSAGE_EVENT                   HEX(CTRL_EVENT)
#define MESSAGE_BOX_BREAK_DELAYED       HEX(CTRL_BOX_BREAK_DELAYED)
#define MESSAGE_AWAIT_BUTTON_PRESS      HEX(CTRL_AWAIT_BUTTON_PRESS)
#define MESSAGE_FADE                    HEX(CTRL_FADE)
#define MESSAGE_NAME                    HEX(CTRL_NAME)
#define MESSAGE_OCARINA                 HEX(CTRL_OCARINA)
#define MESSAGE_FADE2                   HEX(CTRL_FADE2)
#define MESSAGE_SFX                     HEX(CTRL_SFX)
#define MESSAGE_ITEM_ICON               HEX(CTRL_ITEM_ICON)
#define MESSAGE_TEXT_SPEED              HEX(CTRL_TEXT_SPEED)
#define MESSAGE_BACKGROUND              HEX(CTRL_BACKGROUND)
#define MESSAGE_MARATHON_TIME           HEX(CTRL_MARATHON_TIME)
#define MESSAGE_RACE_TIME               HEX(CTRL_RACE_TIME)
#define MESSAGE_POINTS                  HEX(CTRL_POINTS)
#define MESSAGE_TOKENS                  HEX(CTRL_TOKENS)
#define MESSAGE_UNSKIPPABLE             HEX(CTRL_UNSKIPPABLE)
#define MESSAGE_TWO_CHOICE              HEX(CTRL_TWO_CHOICE)
#define MESSAGE_THREE_CHOICE            HEX(CTRL_THREE_CHOICE)
#define MESSAGE_FISH_INFO               HEX(CTRL_FISH_INFO)
#define MESSAGE_HIGHSCORE               HEX(CTRL_HIGHSCORE)
#define MESSAGE_TIME                    HEX(CTRL_TIME)
#endif

#endif

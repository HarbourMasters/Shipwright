#ifndef MESSAGE_DATA_TEXTBOX_TYPES_H
#define MESSAGE_DATA_TEXTBOX_TYPES_H

typedef enum {
    /*  0 */ TEXTBOX_TYPE_BLACK,
    /*  1 */ TEXTBOX_TYPE_WOODEN,
    /*  2 */ TEXTBOX_TYPE_BLUE,
    /*  3 */ TEXTBOX_TYPE_OCARINA,
    /*  4 */ TEXTBOX_TYPE_NONE_BOTTOM,
    /*  5 */ TEXTBOX_TYPE_NONE_NO_SHADOW,
    /* 11 */ TEXTBOX_TYPE_CREDITS = 11
} TextBoxType;

typedef enum {
    /* 0 */ TEXTBOX_BG_CROSS
} TextBoxBackground;

typedef enum {
    /* 0 */ TEXTBOX_POS_VARIABLE,
    /* 1 */ TEXTBOX_POS_TOP,
    /* 2 */ TEXTBOX_POS_MIDDLE,
    /* 3 */ TEXTBOX_POS_BOTTOM
} TextBoxPosition;

#endif

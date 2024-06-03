#ifndef MESSAGE_DATA_STATIC_H
#define MESSAGE_DATA_STATIC_H

#include "global.h"
#include "message_data_fmt.h"
#include "message_data_textbox_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u16 textId;
    u8 typePos;
    const char* segment;
    u32 msgSize;
} MessageTableEntry;

/*
    *  Message Symbol Declarations
    */

#define DEFINE_MESSAGE(textId, type, yPos, staffMessage) \
extern const char _message_##textId##_staff[];

//#include "text/message_data_staff.h"

#undef DEFINE_MESSAGE

#define DEFINE_MESSAGE(textId, type, yPos, nesMessage, gerMessage, fraMessage) \
extern const char _message_##textId##_nes[]; \
extern const char _message_##textId##_ger[]; \
extern const char _message_##textId##_fra[];

//#include "text/message_data.h"
extern char* _message_0xFFFC_nes;

#undef DEFINE_MESSAGE

#ifdef __cplusplus
};
#endif

#endif

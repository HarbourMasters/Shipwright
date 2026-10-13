#if 0
#define MESSAGE_DATA_STATIC

#include "message_data_fmt_nes.h"

#define DEFINE_MESSAGE(textId, typePos, msg) \
    const char _message_##textId[sizeof(msg)] = { msg CMD_END };

#include "assets/text/message_data.enc.h"

#undef DEFINE_MESSAGE
#endif

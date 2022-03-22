#ifndef Z64OBJECT_H
#define Z64OBJECT_H

//#define OBJECT_EXCHANGE_BANK_MAX 19
#define OBJECT_EXCHANGE_BANK_MAX 128

#define DEFINE_OBJECT(_0, enum) enum,
#define DEFINE_OBJECT_NULL(_0, enum) enum,
#define DEFINE_OBJECT_UNSET(enum) enum,

typedef enum {
    #include "tables/object_table.h"
    /* 0x0192 */ OBJECT_ID_MAX
} ObjectID;

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_NULL
#undef DEFINE_OBJECT_UNSET

#endif

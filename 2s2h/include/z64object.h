#ifndef Z64OBJECT_H
#define Z64OBJECT_H

#define DEFINE_OBJECT(_name, enumValue) enumValue,
#define DEFINE_OBJECT_UNSET(enumValue) enumValue,
#define DEFINE_OBJECT_SIZE_ZERO(_name, enumValue) enumValue,

typedef enum ObjectId {
    #include "tables/object_table.h"
    /* 0x283 */ OBJECT_ID_MAX
} ObjectId;

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_UNSET
#undef DEFINE_OBJECT_SIZE_ZERO

#endif

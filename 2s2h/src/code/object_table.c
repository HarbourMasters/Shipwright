#include "global.h"

s16 gPlayerFormObjectIds[PLAYER_FORM_MAX] = {
    OBJECT_LINK_BOY,   // PLAYER_FORM_FIERCE_DEITY
    OBJECT_LINK_GORON, // PLAYER_FORM_GORON
    OBJECT_LINK_ZORA,  // PLAYER_FORM_ZORA
    OBJECT_LINK_NUTS,  // PLAYER_FORM_DEKU
    OBJECT_LINK_CHILD, // PLAYER_FORM_HUMAN
};

ObjectId gObjectTableSize = OBJECT_ID_MAX;

// Object Table definition
#define DEFINE_OBJECT(name, _1) { 0, 0, #name },
#define DEFINE_OBJECT_NULL(name, _1) ROM_FILE(name),
#define DEFINE_OBJECT_UNSET(_0) { 0, 0, "" },
#define DEFINE_OBJECT_SIZE_ZERO(name, x) DEFINE_OBJECT_UNSET(aa)
//#define DEFINE_OBJECT(name, _enumValue) { SEGMENT_ROM_START(name), SEGMENT_ROM_END(name) },
//#define DEFINE_OBJECT_UNSET(_enumValue) { 0, 0 },
//#define DEFINE_OBJECT_SIZE_ZERO(name, _enumValue) { SEGMENT_ROM_START(name), SEGMENT_ROM_START(name) },

RomFile gObjectTable[] = {
#include "tables/object_table.h"
};

#undef DEFINE_OBJECT
#undef DEFINE_OBJECT_UNSET
#undef DEFINE_OBJECT_SIZE_ZERO

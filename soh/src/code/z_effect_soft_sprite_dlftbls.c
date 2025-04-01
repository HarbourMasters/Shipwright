#include "global.h"

// Linker symbol declarations (used in the table below)
#define DEFINE_EFFECT_SS(name, _1) DECLARE_OVERLAY_SEGMENT(name)
#define DEFINE_EFFECT_SS_UNSET(_0)

#include "tables/effect_ss_table.h"

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

// Init Vars declarations (also used in the table below)
#define DEFINE_EFFECT_SS(name, _1) extern EffectSsInit name##_InitVars;
#define DEFINE_EFFECT_SS_UNSET(_0)

#include "tables/effect_ss_table.h"

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

// Effect SS Overlay Table definition
//#define DEFINE_EFFECT_SS(name, _1)         \
//    {                                      \
//        (uintptr_t)_ovl_##name##SegmentRomStart, \
//        (uintptr_t)_ovl_##name##SegmentRomEnd,   \
//        _ovl_##name##SegmentStart,         \
//        _ovl_##name##SegmentEnd,           \
//        NULL,                              \
//        &name##_InitVars,                  \
//        1,                                 \
//    },

#define DEFINE_EFFECT_SS(name, _1)                                   \
    {                                                                \
        (uintptr_t)0, (uintptr_t)0, 0, 0, NULL, &name##_InitVars, 1, \
    },

#define DEFINE_EFFECT_SS_UNSET(_0) { 0 },

EffectSsOverlay gEffectSsOverlayTable[] = {
#include "tables/effect_ss_table.h"
};

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

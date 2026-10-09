#include "z64effect_ss.h"
#include "segment_symbols.h"

// Profile and linker symbol declarations (used in the table below)
#define DEFINE_EFFECT_SS(name, _enumValue) \
    extern EffectSsProfile name##_Profile; \
    DECLARE_OVERLAY_SEGMENT(name)

#define DEFINE_EFFECT_SS_UNSET(_enumValue)

#include "tables/effect_ss_table.h"

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

#define DEFINE_EFFECT_SS(name, _enumValue)    \
    {                                         \
        0, 0, 0, 0, NULL, &name##_Profile, 1, \
    },

#define DEFINE_EFFECT_SS_UNSET(_enumValue) { 0 },

EffectSsOverlay gEffectSsOverlayTable[EFFECT_SS_TYPE_MAX] = {
#include "tables/effect_ss_table.h"
};

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

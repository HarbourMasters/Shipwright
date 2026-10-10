#include "z64transition.h"

#include "segment_symbols.h"
#include "z64lib.h"

// Profile and Linker symbol declarations (used in the table below)
#define DEFINE_TRANSITION(_enumValue, structName, _instanceName, name) \
    extern TransitionProfile structName##_Profile;                     \
    DECLARE_OVERLAY_SEGMENT(name)

#define DEFINE_TRANSITION_INTERNAL(_enumValue, structName, _instanceName) extern TransitionProfile structName##_Profile;

#include "tables/transition_table.h"

#undef DEFINE_TRANSITION
#undef DEFINE_TRANSITION_INTERNAL
// BENTODO
#define DEFINE_TRANSITION(_enumValue, structName, _instanceName, name) \
    {                                                                  \
        { 0, 0 },                                                      \
        SEGMENT_START(ovl_##name),                                     \
        SEGMENT_END(ovl_##name),                                       \
        SEGMENT_ROM_START(ovl_##name),                                 \
        SEGMENT_ROM_END(ovl_##name),                                   \
        &structName##_Profile,                                         \
        sizeof(structName),                                            \
    },

#define DEFINE_TRANSITION_INTERNAL(_enumValue, structName, _instanceName) \
    { { 0, 0 }, NULL, NULL, 0, 0, &structName##_Profile, sizeof(structName) },

TransitionOverlay gTransitionOverlayTable[] = {
#include "tables/transition_table.h"
};

#undef DEFINE_TRANSITION
#undef DEFINE_TRANSITION_INTERNAL

void Transition_Init(TransitionContext* transitionCtx) {
    TransitionOverlay* overlayEntry;
    ptrdiff_t relocOffset;
    TransitionProfile* profile[1];

    overlayEntry = &gTransitionOverlayTable[transitionCtx->fbdemoType];
    TransitionOverlay_Load(overlayEntry);

    relocOffset = (uintptr_t)Lib_PhysicalToVirtual(overlayEntry->loadInfo.addr) - (uintptr_t)overlayEntry->vramStart;
    profile[0] = NULL;
    profile[0] = (overlayEntry->profile != NULL) ? (TransitionProfile*)((uintptr_t)overlayEntry->profile + relocOffset)
                                                 : profile[0];

    transitionCtx->init = profile[0]->init;
    transitionCtx->destroy = profile[0]->destroy;
    transitionCtx->start = profile[0]->start;
    transitionCtx->isDone = profile[0]->isDone;
    transitionCtx->draw = profile[0]->draw;
    transitionCtx->update = profile[0]->update;
    transitionCtx->setType = profile[0]->setType;
    transitionCtx->setColor = profile[0]->setColor;
    transitionCtx->setEnvColor = profile[0]->setEnvColor;
}

void Transition_Destroy(TransitionContext* transitionCtx) {
    TransitionOverlay_Free(&gTransitionOverlayTable[transitionCtx->fbdemoType]);
}

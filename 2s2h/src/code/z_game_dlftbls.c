#include "global.h"
#include "overlays/gamestates/ovl_daytelop/z_daytelop.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include "overlays/gamestates/ovl_opening/z_opening.h"
#include "overlays/gamestates/ovl_select/z_select.h"
#include "overlays/gamestates/ovl_title/z_title.h"
#include "z_title_setup.h"

// Linker symbol declarations (used in the table below)
#define DEFINE_GAMESTATE(_typeName, _enumName, segmentName) DECLARE_OVERLAY_SEGMENT(segmentName)
#define DEFINE_GAMESTATE_INTERNAL(_typeName, _enumName)

#include "tables/gamestate_table.h"

#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL

// Gamestate Overlay Table definition
#define DEFINE_GAMESTATE_INTERNAL(typeName, _enumName) \
    { NULL, 0, 0, NULL, NULL, NULL, typeName##_Init, typeName##_Destroy, NULL, NULL, 0, sizeof(typeName##State) },

#define DEFINE_GAMESTATE(typeName, _enumName, segmentName) DEFINE_GAMESTATE_INTERNAL(typeName, _enumName)
//\
//    { NULL,                                                \
//      (uintptr_t)SEGMENT_ROM_START(ovl_##segmentName),     \
//      (uintptr_t)SEGMENT_ROM_END(ovl_##segmentName),       \
//      SEGMENT_START(ovl_##segmentName),                    \
//      SEGMENT_END(ovl_##segmentName),                      \
//      NULL,                                                \
//      typeName##_Init,                                     \
//      typeName##_Destroy,                                  \
//      NULL,                                                \
//      NULL,                                                \
//      0,                                                   \
//      sizeof(typeName##State) },

GameStateOverlay gGameStateOverlayTable[GAMESTATE_ID_MAX] = {
#include "tables/gamestate_table.h"
};

#undef DEFINE_GAMESTATE
#undef DEFINE_GAMESTATE_INTERNAL

GameStateId gGraphNumGameStates = GAMESTATE_ID_MAX;

#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H

#include "math.h"

#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/gameconsole.h"
#include "soh/Enhancements/gameplaystats.h"
#include <libultraship/bridge.h>

#define _AudioseqSegmentRomStart "Audioseq"
#define _AudiobankSegmentRomStart "Audiobank"
#define _AudiotableSegmentRomStart "Audiotable"

#define _icon_item_staticSegmentRomStart 0
#define _icon_item_staticSegmentRomEnd 0
#define _map_i_staticSegmentRomStart 0
#define _map_i_staticSegmentRomEnd 0
#define _message_staticSegmentRomStart 0
#define _message_staticSegmentRomEnd 0
#define _do_action_staticSegmentRomStart 0
#define _do_action_staticSegmentRomEnd 0
#define _nes_font_staticSegmentRomStart 0
#define _nes_font_staticSegmentRomEnd 0
#define _nintendo_rogo_staticSegmentRomStart 0
#define _nintendo_rogo_staticSegmentRomEnd 0
#define _dmadataSegmentStart 0
#define _dmadataSegmentEnd 0
#define _parameter_staticSegmentRomStart 0
#define _parameter_staticSegmentRomEnd 0
#define _map_name_staticSegmentRomStart 0
#define _map_name_staticSegmentRomEnd 0
#define _title_staticSegmentRomStart 0
#define _title_staticSegmentRomEnd 0
#define _z_select_staticSegmentRomStart 0
#define _z_select_staticSegmentRomEnd 0

// TODO: POSIX/BSD Bug, this is a hack to fix the build compilation on any BSD system (Switch, Wii-U, Vita, etc)
// <sys/types.h> defines quad as a macro, which conflicts with the quad parameter on z_collision_check.c
#undef quad

#endif

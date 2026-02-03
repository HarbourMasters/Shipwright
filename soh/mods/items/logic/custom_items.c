/**
 * custom_items.c - Unity build aggregator for custom items
 *
 * This file includes all custom item implementation files for unity build.
 * Unity builds compile multiple .c files as one translation unit for faster
 * compile times and potential optimizations.
 *
 * Add new item logic files here to include them in the build.
 */

#include "../custom_items.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

// Helper modules
#include "../helpers/movement_helper.c"
#include "../helpers/equip_helper.c"
#include "../helpers/camera_helper.c"
#include "../helpers/cutscene_helper.c"
#include "../helpers/fx_helper.c"
#include "../helpers/combat_helper.c"
#include "../helpers/grappling_helper.c"
#include "../custom_items_common.c"
#include "../objects/object_custom_items.c"

// Item implementations
#include "item_rocsfeather.c"
#include "item_rocscape.c"
#include "item_dekuleaf.c"
#include "item_spinner.c"
#include "item_rod_fire.c"
#include "item_rod_ice.c"
#include "item_rod_light.c"
#include "item_pending_1.c"
#include "item_pending_2.c"
#include "item_pending_3.c"
#include "item_hylias_grace.c"
#include "item_demise_destruction.c"
#include "item_zonai_permafrost.c"
#include "item_mitts.c"
#include "item_shovel.c"
#include "item_switchhook.c"
#include "item_desire_sensor.c"
#include "item_whip.c"
#include "item_ballchain.c"
#include "item_bombarrows.c"
#include "item_gustjar.c"
#include "item_beetle.c"
#include "item_dominionrod.c"
#include "item_cane_of_somaria.c"
#include "item_time_gate.c"
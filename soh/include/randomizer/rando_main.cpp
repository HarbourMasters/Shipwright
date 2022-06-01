#include "menu.hpp"
#include "hint_list.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "location_access.hpp"
#include "rando_main.hpp"

#define TICKS_PER_SEC 268123480.0

void RandoMain::GenerateRando() {
    HintTable_Init();
    ItemTable_Init();
    LocationTable_Init();
    GenerateRandomizer();
}
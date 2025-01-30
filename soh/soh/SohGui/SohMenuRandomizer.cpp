#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuRandomizer() {
    // Add Randomizer Menu
    AddMenuEntry("Randomizer", "gSettings.Menu.RandomizerSidebarSection");

    // Menu 1
    WidgetPath path = { "Randomizer", "Menu 1", SECTION_COLUMN_1 };
    AddSidebarEntry("Randomizer", "Menu 1", 3);

    AddWidget(path, "Filler1", WIDGET_TEXT);
}

} // namespace SohGui

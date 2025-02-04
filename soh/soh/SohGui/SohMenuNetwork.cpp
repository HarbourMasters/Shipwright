#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuNetwork() {
    // Add Network Menu
    AddMenuEntry("Network", CVAR_SETTING("Menu.NetworkSidebarSection"));

    // General
    AddSidebarEntry("Network", "General", 3);
    WidgetPath path = { "Network", "General", SECTION_COLUMN_1 };

    AddWidget(path, "Filler1", WIDGET_TEXT);
}

} // namespace SohGui

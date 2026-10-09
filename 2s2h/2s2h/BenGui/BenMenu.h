#ifndef BENMENU_H
#define BENMENU_H

#include "UIWidgets.hpp"
#include "Menu.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/DeveloperTools/DeveloperTools.h"
#include <fast/backends/gfx_rendering_api.h>

namespace BenGui {

class BenMenu : public Ship::Menu {
  public:
    BenMenu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

    void AddSidebarEntry(std::string sectionName, std::string sidbarName, uint32_t columnCount);
    WidgetInfo& AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType);
    void AddMenuElements();
    void AddSettings();
    void AddEnhancements();
    void AddDevTools();

  private:
    bool mMenuElementsInitialized = false;
};
} // namespace BenGui

#endif // BENMENU_H

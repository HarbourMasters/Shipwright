#ifndef SOHMENU_H
#define SOHMENU_H

#include <libultraship/libultraship.h>
#include "UIWidgets2.hpp"
#include "Menu.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

namespace SohGui {

static const std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets2::Colors::Red, "Red" },
    { UIWidgets2::Colors::DarkRed, "Dark Red" },
    { UIWidgets2::Colors::Orange, "Orange" },
    { UIWidgets2::Colors::Green, "Green" },
    { UIWidgets2::Colors::DarkGreen, "Dark Green" },
    { UIWidgets2::Colors::LightBlue, "Light Blue" },
    { UIWidgets2::Colors::Blue, "Blue" },
    { UIWidgets2::Colors::DarkBlue, "Dark Blue" },
    { UIWidgets2::Colors::Indigo, "Indigo" },
    { UIWidgets2::Colors::Violet, "Violet" },
    { UIWidgets2::Colors::Purple, "Purple" },
    { UIWidgets2::Colors::Brown, "Brown" },
    { UIWidgets2::Colors::Gray, "Gray" },
    { UIWidgets2::Colors::DarkGray, "Dark Gray" },
};

static const std::unordered_map<int32_t, const char*> textureFilteringMap = {
    { FILTER_THREE_POINT, "Three-Point" },
    { FILTER_LINEAR, "Linear" },
    { FILTER_NONE, "None" },
};

static const std::unordered_map<int32_t, const char*> logLevels = {
    { DEBUG_LOG_TRACE, "Trace" }, { DEBUG_LOG_DEBUG, "Debug" }, { DEBUG_LOG_INFO, "Info" },
    { DEBUG_LOG_WARN, "Warn" },   { DEBUG_LOG_ERROR, "Error" }, { DEBUG_LOG_CRITICAL, "Critical" },
    { DEBUG_LOG_OFF, "Off" },
};

static const std::unordered_map<int32_t, const char*> notificationPosition = {
    { 0, "Top Left" }, { 1, "Top Right" }, { 2, "Bottom Left" }, { 3, "Bottom Right" }, { 4, "Hidden" },
};

static const std::unordered_map<int32_t, const char*> debugSaveFileModes = {
    { 0, "Off" }, { 1, "Vanilla" }, { 2, "Maxed" },
};

class SohMenu : public Ship::Menu {
  public:
    SohMenu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

    void AddSidebarEntry(std::string sectionName, std::string sidbarName, uint32_t columnCount);
    WidgetInfo& AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType);
    void AddMenuSettings();
    void AddMenuEnhancements();
    void AddMenuDevTools();
    void AddMenuRandomizer();
    void AddMenuNetwork();
};
} // namespace SohGui

#endif // SOHMENU_H

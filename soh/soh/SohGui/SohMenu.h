#ifndef BENMENU_H
#define BENMNEU_H

#include <libultraship/libultraship.h>
#include "UIWidgets2.hpp"
#include "Menu.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/Notification/Notification.h"

namespace SohGui {

static const std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets::Colors::Red, "Red" },
    { UIWidgets::Colors::DarkRed, "Dark Red" },
    { UIWidgets::Colors::Orange, "Orange" },
    { UIWidgets::Colors::Green, "Green" },
    { UIWidgets::Colors::DarkGreen, "Dark Green" },
    { UIWidgets::Colors::LightBlue, "Light Blue" },
    { UIWidgets::Colors::Blue, "Blue" },
    { UIWidgets::Colors::DarkBlue, "Dark Blue" },
    { UIWidgets::Colors::Indigo, "Indigo" },
    { UIWidgets::Colors::Violet, "Violet" },
    { UIWidgets::Colors::Purple, "Purple" },
    { UIWidgets::Colors::Brown, "Brown" },
    { UIWidgets::Colors::Gray, "Gray" },
    { UIWidgets::Colors::DarkGray, "Dark Gray" },
};

static const std::unordered_map<int32_t, const char*> textureFilteringMap = {
    { FILTER_THREE_POINT, "Three-Point" },
    { FILTER_LINEAR, "Linear" },
    { FILTER_NONE, "None" },
};

//static const std::unordered_map<int32_t, const char*> debugSaveOptions = {
//    { DEBUG_SAVE_INFO_COMPLETE, "100\% save" },
//    { DEBUG_SAVE_INFO_VANILLA_DEBUG, "Vanilla debug save" },
//    { DEBUG_SAVE_INFO_NONE, "Empty save" },
//};

static const std::unordered_map<int32_t, const char*> logLevels = {
    { DEBUG_LOG_TRACE, "Trace" }, { DEBUG_LOG_DEBUG, "Debug" }, { DEBUG_LOG_INFO, "Info" },
    { DEBUG_LOG_WARN, "Warn" },   { DEBUG_LOG_ERROR, "Error" }, { DEBUG_LOG_CRITICAL, "Critical" },
    { DEBUG_LOG_OFF, "Off" },
};

static const std::unordered_map<int32_t, const char*> notificationPosition = {
    { 0, "Top Left" }, { 1, "Top Right" }, { 2, "Bottom Left" }, { 3, "Bottom Right" }, { 4, "Hidden" },
};

static const std::unordered_map<int32_t, const char*> damageMultiplierOptions = {
    { 0, "1x" }, { 1, "2x" }, { 2, "4x" }, { 3, "8x" }, { 4, "16x" }, { 10, "1 Hit KO" },
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
    void AddSettings();
    void AddEnhancements();
    void AddDevTools();
};
} // namespace BenGui

#endif // BENMENU_H

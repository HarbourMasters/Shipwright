#ifndef BENMENU_H
#define BENMNEU_H

#include <libultraship/libultraship.h>
#include "UIWidgets2.hpp"
#include "Menu.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/Notification/Notification.h"

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

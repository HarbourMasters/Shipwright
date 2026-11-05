#pragma once

#include "MenuTypes.h"
#include <string_view>

namespace SohGui {

std::string MenuLabel(const std::string& menu);
std::string SidebarLabel(const std::string& menu, const std::string& sidebar);
std::string NormalizeMenuSearch(std::string text);
// Search references remain valid until the widget cache is invalidated or destroyed.
const std::string& WidgetSearchText(const WidgetInfo& source);
const std::string& NormalizedWidgetSearchText(const WidgetInfo& source, std::string_view extraTerms = {},
                                              std::string_view sidebar = {});

// A temporary rendering view. The registered widget, callbacks, canonical
// option values, and CVar identity remain untouched. Choice strings live as
// long as the view so ComboboxOptions' const char* entries cannot dangle.
class LocalizedWidget {
  public:
    explicit LocalizedWidget(const WidgetInfo& source);
    LocalizedWidget(const LocalizedWidget&) = delete;
    LocalizedWidget& operator=(const LocalizedWidget&) = delete;

    WidgetInfo view;

  private:
    std::map<int32_t, std::string> choices;
};

} // namespace SohGui

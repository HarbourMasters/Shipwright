#ifndef SOHMENU_H
#define SOHMENU_H

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "Menu.h"
#include <fast/backends/gfx_rendering_api.h>
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/Presets/Presets.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#ifdef __cplusplus
extern "C" {
#endif
void enableBetaQuest();
void disableBetaQuest();
#ifdef __cplusplus
}
#endif

namespace SohGui {
static std::unordered_map<int32_t, const char*> languages = {
    { LANGUAGE_ENG, "English" },
    { LANGUAGE_GER, "German" },
    { LANGUAGE_FRA, "French" },
    { LANGUAGE_JPN, "Japanese" },
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
    static void UpdateLanguageMap(std::unordered_map<int32_t, const char*>& languageMap);

  private:
    char mGitCommitHashTruncated[8];
    bool mIsTaggedVersion;
};
} // namespace SohGui

#endif // SOHMENU_H

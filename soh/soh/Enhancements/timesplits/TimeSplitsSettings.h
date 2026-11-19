#pragma once
#ifndef TIMESPLITS_SETTINGS_H
#define TIMESPLITS_SETTINGS_H

#include <libultraship/libultraship.h>

#ifdef __cplusplus
#include "soh/SohGui/LocalizedWindow.h"
namespace TimeSplits {

class TimesplitsSettingsWindow : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
} // namespace TimeSplits
#endif

#endif // TIMESPLITS_SETTINGS_H
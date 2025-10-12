#pragma once
#ifndef TIMESPLITS_SETTINGS_H
#define TIMESPLITS_SETTINGS_H

#include <libultraship/libultraship.h>

#ifdef __cplusplus
class TimesplitsSettingsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
#endif

#endif // TIMESPLITS_SETTINGS_H
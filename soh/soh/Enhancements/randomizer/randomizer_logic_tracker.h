#pragma once

#include "randomizerTypes.h"
#include <libultraship/libultraship.h>

class LogicTrackerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    void DrawElement() override;

    static void ShowRandomizerCheck(RandomizerCheck randomizerCheck);
    static void ShowRandomizerRegion(RandomizerRegion toRandomizerRegion, RandomizerRegion fromRandomizerRegion);
    static void ShowRandomizerEvent(const std::string& event);

  protected:
    void InitElement() override;
    void UpdateElement() override;
};

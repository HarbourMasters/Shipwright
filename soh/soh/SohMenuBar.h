#pragma once

#include <libultraship/libultraship.h>
#include "gui/GuiMenuBar.h"
#include "gui/GuiElement.h"

namespace SohGui {
class SohMenuBar : public LUS::GuiMenuBar {
  public:
    using LUS::GuiMenuBar::GuiMenuBar;
  protected:
    void DrawElement() override;
    void InitElement() override {};
    void UpdateElement() override {};
};
} // namespace SohGui
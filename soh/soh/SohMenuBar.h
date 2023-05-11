#pragma once

#include "menu/GuiWindow.h"

namespace SohGui {
class SohMenuBar : public LUS::GuiMenuBar {
  public:
    void Draw() override;
};
} // namespace SohGui
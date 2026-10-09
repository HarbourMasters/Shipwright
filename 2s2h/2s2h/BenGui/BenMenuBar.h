#ifndef BenMenuBar_h
#define BenMenuBar_h

#include <ship/window/gui/GuiMenuBar.h>

namespace BenGui {
class BenMenuBar : public Ship::GuiMenuBar {
  public:
    using Ship::GuiMenuBar::GuiMenuBar;

  protected:
    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};
};
} // namespace BenGui

#endif // BenMenuBar_h

#pragma once

#ifdef __ANDROID__

#include <ship/window/gui/GuiWindow.h>

namespace SohAndroid {
namespace TouchControls {
void Initialize();
void Draw();
void RequestMenuToggle();
} // namespace TouchControls

class TouchControlsWindow final : public Ship::GuiWindow {
  public:
    TouchControlsWindow();
    void Draw() override;

  protected:
    void InitElement() override;
    void UpdateElement() override;
    void DrawElement() override;
};
} // namespace SohAndroid

#endif // __ANDROID__

#pragma once

#include "soh/SohGui/LocalizedWindow.h"

class DLViewerWindow final : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

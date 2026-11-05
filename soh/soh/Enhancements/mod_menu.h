#pragma once

#ifdef __cplusplus
#include "soh/SohGui/LocalizedWindow.h"
class ModMenuWindow : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};
#endif
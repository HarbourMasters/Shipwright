#pragma once
#ifndef PLANDOMIZER_H
#define PLANDOMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // PLANDOMIZER_H

#include <libultraship/libultraship.h>

#ifdef __cplusplus
class PlandomizerWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif
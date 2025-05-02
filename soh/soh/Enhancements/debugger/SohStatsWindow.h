#ifndef SOH_STATS_H
#define SOH_STATS_H

#include <libultraship/libultraship.h>

class SohStatsWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~SohStatsWindow(){};

  protected:
    void InitElement() override{};
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif // SOH_STATS_H

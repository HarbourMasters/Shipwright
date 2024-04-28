#include <libultraship/libultraship.h>
#include "gameplaystats.h"

class GameplayStatsWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};
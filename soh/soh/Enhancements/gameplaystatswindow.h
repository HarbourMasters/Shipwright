#include <libultraship/libultraship.h>
#include "gameplaystats.h"

class GameplayStatsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};
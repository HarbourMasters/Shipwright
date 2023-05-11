#include <libultraship/libultraship.h>
#include "gameplaystats.h"

class GameplayStatsWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Init() override;
    void Draw() override;
};
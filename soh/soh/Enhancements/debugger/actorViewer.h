#pragma once

#include <libultraship/libultraship.h>

#include "z64actor.h"

#include <vector>

class ActorViewerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};

  private:
    Actor* display = nullptr;
    int category = ACTORCAT_SWITCH;
    std::vector<Actor*> list;
};

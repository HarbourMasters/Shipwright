#pragma once

#include "soh/SohGui/LocalizedWindow.h"

#include "z64actor.h"

#include <vector>

class ActorViewerWindow final : public SohGui::LocalizedWindow {
  public:
    using LocalizedWindow::LocalizedWindow;

    void DrawElement() override;
    void InitElement() override;
    void UpdateElement() override{};

  private:
    Actor* display = nullptr;
    int category = ACTORCAT_SWITCH;
    std::vector<Actor*> list;
};

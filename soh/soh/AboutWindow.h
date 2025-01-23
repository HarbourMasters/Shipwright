#pragma once

#include <libultraship/libultraship.h>

class AboutWindow : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~AboutWindow();

  private:
    void InitElement() override;
    void Draw() override;
    void DrawElement() override;
    void UpdateElement() override {};

    const char* GetGameVersionString(uint32_t index);

    bool mIsTaggedVersion;
    char mGitCommitHashTruncated[8];
};

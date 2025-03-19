#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#ifdef __cplusplus

#include <string>
#include <libultraship/libultraship.h>

namespace Notification {

struct Options {
    uint32_t id = 0;
    const char* itemIcon = nullptr;
    std::string prefix = "";
    ImVec4 prefixColor = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
    std::string message = "";
    ImVec4 messageColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
    std::string suffix = "";
    ImVec4 suffixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f);
    float remainingTime = 0.0f; // Seconds
};

class Window : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override {};
    void DrawElement() override {};
    void Draw() override;
    void UpdateElement() override;
};

void Emit(Options notification);

} // namespace Notification

#endif // __cplusplus
#endif // NOTIFICATION_H

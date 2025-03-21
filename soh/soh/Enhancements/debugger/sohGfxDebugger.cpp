#include "sohGfxDebugger.h"
#include "soh/OTRGlobals.h"

void SohGfxDebuggerWindow::InitElement() {
    GfxDebuggerWindow::InitElement();
}

void SohGfxDebuggerWindow::UpdateElement() {
    GfxDebuggerWindow::UpdateElement();
}

void SohGfxDebuggerWindow::DrawElement() {
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    GfxDebuggerWindow::DrawElement();
    ImGui::PopFont();
}

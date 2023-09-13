#pragma once

#ifdef __cplusplus

#include <libultraship/libultraship.h>
#include <ImGui/imgui.h>
#include <vector>

class ModLoadOrderWindow : public LUS::GuiWindow {
    public:
        using LUS::GuiWindow::GuiWindow;

        void DrawElement() override;
        void InitElement() override {};
        void UpdateElement() override {};
        ~ModLoadOrderWindow() {};
    private:
        void RefreshOtrList(bool ignoreCvar);
        void SaveAndQuit();
        bool mOtrListLoaded;
        std::vector<std::string> mOtrList;
};

#endif
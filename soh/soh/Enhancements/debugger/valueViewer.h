#pragma once

#ifdef __cplusplus

#include <libultraship/libultraship.h>

typedef enum {
    TYPE_S8,
    TYPE_U8,
    TYPE_S16,
    TYPE_U16,
    TYPE_S32,
    TYPE_U32,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT,
} ValueType;

typedef void* (*ValueFn)();

typedef struct {
    const char* name;
    const char* path;
    std::string prefix;
    ValueType type;
    bool requiresPlayState;
    ValueFn valueFn;
    ImVec4 color;
    bool isActive;
    bool isPrinted;
    bool typeFormat;
    uint32_t x;
    uint32_t y;
} ValueTableElement;

class ValueViewerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};

extern "C" {
#include <z64.h>
#endif

void ValueViewer_Draw(GfxPrint* printer);

#ifdef __cplusplus
}
#endif
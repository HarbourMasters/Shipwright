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
} ValueTableElement;

typedef struct {
    std::string prefix;
    ImVec4 color;
    bool isPrinted;
    bool typeFormat;
    uint32_t x;
    uint32_t y;
} ValueSetting;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec4, x, y, z, w)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ValueSetting, prefix, color, isPrinted, typeFormat, x, y)

typedef enum {
    VVE_TIME,
    VVE_AGE,
    VVE_HEALTH,
    VVE_NAVI_TIMER,
    VVE_SCENE_ID,
    VVE_ROOM_ID,
    VVE_ENTRANCE_ID,
    VVE_CUTSCENE_ID,
    VVE_LINK_X_POS,
    VVE_LINK_Y_POS,
    VVE_LINK_Z_POS,
    VVE_LINK_YAW,
    VVE_LINK_VELOCITY,
    VVE_LINK_X_VELOCITY,
    VVE_LINK_Y_VELOCITY,
    VVE_LINK_Z_VELOCITY,
    VVE_TEXT_ID,
    VVE_ANALOG_STICK_X,
    VVE_ANALOG_STICK_Y,
    VVE_GETITEMID,
    VVE_GETITEMENTRY,
    VVE_MAX,
} ValueViewerEntry;

class ValueViewerWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override{};
};

#endif

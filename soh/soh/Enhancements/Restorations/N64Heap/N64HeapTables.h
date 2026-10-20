// NTSC 1.2 data derived from the ROM and the decompilation
// clang-format off
#pragma once
#include <stdint.h>

namespace n64heap {

// ---- Actor heap ----

constexpr uint32_t ZELDA_ARENA_START = 0x801db2c0;
constexpr uint32_t CS_SCRIPT_ADDRESS = 0x801caac8; // &play->csCtx.script
constexpr uint32_t ARENA_NODE_SIZE = 0x30;
constexpr uint32_t ABSOLUTE_SPACE_SIZE = 0x24e0;
constexpr uint32_t GI_OBJECT_SEGMENT_SIZE = 0x2008;
constexpr uint32_t JNT_SPH_ELEMENT_SIZE = 0x40;
constexpr uint32_t TRIS_ELEMENT_SIZE = 0x5c;
constexpr uint32_t SKIN_LIMB_VTX_SIZE = 0xc;
constexpr uint32_t CAMERA_SIZE = 0x16c;
constexpr uint32_t POINTER_SIZE = 4;
constexpr int16_t MAGIC_DARK_ACTOR_ID = 0xf4;
constexpr uint32_t ACTOR_BASE_SIZE = 0x13c;
constexpr uint32_t ACTOR_OVERLAY_TABLE = 0x800e8b70;
constexpr uint32_t ACTOR_OVERLAY_ENTRY_SIZE = 0x20;

enum class AllocType : uint8_t { Normal, Absolute, Persistent };

struct ActorEntry {
    uint32_t instanceSize;
    uint32_t overlaySize; // 0 when the actor is part of code
    AllocType allocType;
    bool valid;
};

constexpr int ACTOR_COUNT = 471;
extern const ActorEntry ACTORS[ACTOR_COUNT];

// ---- Cutscene scripts ----

struct ScriptEntry {
    int32_t words[4]; // First four words of the script
    bool isScene;     // Scene file data, otherwise actor overlay data
    int16_t owner;    // Scene id or actor id
    uint32_t value;   // N64 address for scene data, overlay offset for actor data
    const char* name; // "<scene file>/<symbol>" for scene data
};

constexpr int SCRIPT_COUNT = 188;
extern const ScriptEntry SCRIPTS[SCRIPT_COUNT];

// ---- Object space and pause screen ----

struct RomFile {
    uint32_t vrom;
    uint32_t size;
};

constexpr uint32_t KALEIDO_AREA = 0x80389310;
constexpr uint32_t KALEIDO_AREA_SIZE = 0x26740;
constexpr uint32_t MESSAGE_AREA = 0x80387110;
constexpr uint32_t MESSAGE_AREA_SIZE = 0x2200;
constexpr uint32_t EFFECT_AREA = 0x80385130;
constexpr uint32_t EFFECT_AREA_SIZE = 0x1fe0;
constexpr uint32_t PAUSE_RENDER_TEXTURE_SIZE = 0x3800;
constexpr uint32_t PAUSE_KEEP_BUFFER_SIZE = 0x5000;
constexpr uint32_t PAUSE_JOINT_TABLE_SIZE = 24 * 6;
constexpr uint32_t PAUSE_COVERAGE_SIZE = 64 * 112;
constexpr uint32_t ITEM_ICON_SIZE = 32 * 32 * 4;
constexpr uint32_t ITEM_NAME_TEX_SIZE = 0x400;
constexpr uint32_t MAP_NAME_TEX1_SIZE = 0x400;
constexpr uint32_t MAP_NAME_TEX2_SIZE = 0xA00;
constexpr int WORLD_MAP_AREA_COUNT = 22;

constexpr RomFile FILE_ICON_ITEM_STATIC = { 0x7bd000, 0x888a0 };
constexpr RomFile FILE_ICON_ITEM_24_STATIC = { 0x846000, 0xb400 };
constexpr RomFile FILE_ICON_ITEM_FIELD_STATIC = { 0x852000, 0xb930 };
constexpr RomFile FILE_ICON_ITEM_DUNGEON_STATIC = { 0x85e000, 0x1d80 };
constexpr RomFile FILE_ICON_ITEM_GAMEOVER_STATIC = { 0x860000, 0x3300 };
constexpr RomFile FILE_ICON_ITEM_JPN_STATIC = { 0x864000, 0xdc80 };
constexpr RomFile FILE_ICON_ITEM_NES_STATIC = { 0x872000, 0xdc80 };
constexpr RomFile FILE_MAP_NAME_STATIC = { 0x8be000, 0x21800 };

constexpr int OBJECT_COUNT = 402;
extern const RomFile OBJECTS[OBJECT_COUNT];

constexpr int16_t GAMEPLAY_KEEP_OBJECT_ID = 0x1;
constexpr int16_t LINK_OBJECT_IDS[2] = { 0x14, 0x15 }; // Indexed by link age

struct SceneEntry {
    RomFile file;
    uint32_t base; // N64 address the scene file is loaded at
    bool dungeon;
};

constexpr int SCENE_COUNT = 110;
extern const SceneEntry SCENES[SCENE_COUNT];

enum class AgeReq : uint8_t { Adult, Child, None };
constexpr uint32_t NO_ICON = 0xFFFFFFFF;

struct ItemIcon {
    uint32_t offset; // Offset in icon_item_static
    AgeReq ageReq;
};

constexpr int ITEM_ICON_COUNT = 86;
extern const ItemIcon ITEM_ICONS[ITEM_ICON_COUNT];

// Cutscene commands with their own handler; any other command is skipped
constexpr int CUTSCENE_COMMAND_COUNT = 113;
extern const int32_t CUTSCENE_COMMANDS[CUTSCENE_COMMAND_COUNT];
constexpr int32_t CS_CMD_END_OF_SCRIPT = -1;

// N64 files stored in oot.o2r, sorted by vrom
struct ArchiveFile {
    uint32_t vrom;
    uint32_t size;
    const char* path;
};

constexpr int ARCHIVE_FILE_COUNT = 489;
extern const ArchiveFile ARCHIVE_FILES[ARCHIVE_FILE_COUNT];

// ---- Scene layouts ----

struct SceneLayout {
    int16_t scene;
    uint8_t layer;
    uint32_t zeldaArenaSize;
    uint32_t objectSpaceSize;
};

constexpr int SCENE_LAYOUT_COUNT = 303;
extern const SceneLayout SCENE_LAYOUTS[SCENE_LAYOUT_COUNT];

// ---- Effects ----

// Overlay size of each effect type, 0 when the effect is part of code
constexpr int EFFECT_COUNT = 37;
extern const uint32_t EFFECT_OVERLAY_SIZES[EFFECT_COUNT];

} // namespace n64heap
// clang-format on

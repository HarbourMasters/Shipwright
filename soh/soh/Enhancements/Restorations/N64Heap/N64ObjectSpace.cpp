#include "N64ObjectSpace.h"
#include "N64HeapCore.h"
#include "N64HeapTables.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>

namespace n64heap {

static uint32_t Align16(uint32_t value) {
    return (value + 15) & ~15u;
}

static std::string Hex(uint32_t value) {
    char text[16];
    snprintf(text, sizeof(text), "0x%X", value);
    return text;
}

static uint32_t ReadBigEndian32(const uint8_t* bytes) {
    return (uint32_t(bytes[0]) << 24) | (uint32_t(bytes[1]) << 16) | (uint32_t(bytes[2]) << 8) | bytes[3];
}

// Matches KaleidoScope_GrayOutTextureRGBA32, which greys out icons the current age can't use
static uint32_t GreyOutPixel(uint32_t rgba) {
    if ((rgba & 0xFFFFFF00) == 0) {
        return rgba;
    }
    uint32_t red = (rgba >> 24) & 0xFF;
    uint32_t green = (rgba >> 16) & 0xFF;
    uint32_t blue = (rgba >> 8) & 0xFF;
    uint32_t grey = ((red + green * 2 + blue) / 7) & 0xFF;
    return (grey << 24) | (grey << 16) | (grey << 8) | (rgba & 0xFF);
}

// The pause screen places its data at the start of the object space, aligned to 64 bytes
static uint32_t PauseSegmentStart(uint32_t spaceStart) {
    return (spaceStart + 0x30) & ~0x3Fu;
}

void ObjectSpace::Paint(uint32_t start, uint32_t size, Kind kind, uint32_t vrom, uint32_t base,
                        const std::string& label) {
    if (size == 0) {
        return;
    }
    mVersion++;
    uint32_t end = start + size;
    auto it = mSpans.upper_bound(start);
    if (it != mSpans.begin()) {
        --it;
    }
    while (it != mSpans.end() && it->first < end) {
        uint32_t spanStart = it->first;
        Span span = it->second;
        if (span.end <= start) {
            ++it;
            continue;
        }
        it = mSpans.erase(it);
        if (spanStart < start) {
            Span left = span;
            left.end = start;
            mSpans.emplace(spanStart, left);
        }
        if (span.end > end) {
            mSpans.emplace(end, span);
        }
    }
    mSpans.emplace(start, Span{ end, kind, vrom, base, label });
}

void ObjectSpace::PaintFile(uint32_t address, const RomFile& file, const std::string& label) {
    Paint(address, file.size, Kind::File, file.vrom, address, label);
}

void ObjectSpace::PaintRuntimeData(uint32_t address, uint32_t size, const std::string& label) {
    Paint(address, size, Kind::Runtime, 0, 0, label);
}

void ObjectSpace::SceneInit(int16_t sceneId, uint8_t sceneLayer) {
    mSceneId = sceneId;
    mSlotIds.clear();
    mSlotAddresses.clear();
    PaintRuntimeData(KALEIDO_AREA, KALEIDO_AREA_SIZE, "kaleido/player overlay area");
    PaintRuntimeData(MESSAGE_AREA, MESSAGE_AREA_SIZE, "message textbox area");
    PaintRuntimeData(EFFECT_AREA, EFFECT_AREA_SIZE, "EffectSs table");

    const SceneLayout* layout = FindSceneLayout(sceneId, sceneLayer);
    if (sceneId < 0 || sceneId >= SCENE_COUNT || SCENES[sceneId].file.size == 0 || layout == nullptr) {
        mSpaceStart = 0;
        return;
    }
    // The object space sits directly below the scene file
    const SceneEntry& scene = SCENES[sceneId];
    mSpaceStart = scene.base - layout->objectSpaceSize;
    char label[48];
    snprintf(label, sizeof(label), "scene 0x%02X file", sceneId);
    PaintFile(scene.base, scene.file, label);
}

void ObjectSpace::PaintObject(size_t slot) {
    int16_t id = mSlotIds[slot];
    if (id <= 0 || id >= OBJECT_COUNT || OBJECTS[id].size == 0) {
        return;
    }
    char label[48];
    snprintf(label, sizeof(label), "object 0x%03X (slot %zu)", id, slot);
    PaintFile(mSlotAddresses[slot], OBJECTS[id], label);
}

void ObjectSpace::SyncObjects(const int16_t* ids, int count) {
    if (mSpaceStart == 0) {
        return;
    }
    // Slots that still match are kept, the rest are loaded one after another
    size_t firstChanged = 0;
    while (firstChanged < mSlotIds.size() && firstChanged < static_cast<size_t>(count) &&
           mSlotIds[firstChanged] == static_cast<int16_t>(std::abs(ids[firstChanged]))) {
        firstChanged++;
    }
    if (firstChanged == mSlotIds.size() && firstChanged == static_cast<size_t>(count)) {
        return;
    }
    mSlotIds.resize(count);
    mSlotAddresses.resize(count);
    for (size_t i = firstChanged; i < static_cast<size_t>(count); i++) {
        mSlotIds[i] = static_cast<int16_t>(std::abs(ids[i]));
        if (i == 0) {
            mSlotAddresses[i] = mSpaceStart;
        } else {
            int16_t previous = mSlotIds[i - 1];
            uint32_t previousSize = (previous > 0 && previous < OBJECT_COUNT) ? OBJECTS[previous].size : 0;
            mSlotAddresses[i] = Align16(mSlotAddresses[i - 1] + previousSize);
        }
        PaintObject(i);
    }
}

void ObjectSpace::PauseClosed() {
    for (size_t i = 0; i < mSlotIds.size(); i++) {
        PaintObject(i);
    }
}

uint32_t ObjectSpace::PaintIconFiles(uint32_t address, const RomFile& areaIcons, const char* areaLabel, bool japanese) {
    uint32_t icons24 = Align16(address + FILE_ICON_ITEM_STATIC.size);
    PaintFile(icons24, FILE_ICON_ITEM_24_STATIC, "icon_item_24_static");
    uint32_t area = Align16(icons24 + FILE_ICON_ITEM_24_STATIC.size);
    PaintFile(area, areaIcons, areaLabel);
    uint32_t language = Align16(area + areaIcons.size);
    const RomFile& languageFile = japanese ? FILE_ICON_ITEM_JPN_STATIC : FILE_ICON_ITEM_NES_STATIC;
    PaintFile(language, languageFile, japanese ? "icon_item_jpn_static" : "icon_item_nes_static");
    return Align16(language + languageFile.size);
}

void ObjectSpace::PauseOpened(int linkAge, bool japanese, int16_t worldMapArea) {
    if (mSpaceStart == 0 || linkAge < 0 || linkAge > 1) {
        return;
    }
    // The player preview comes first: render texture, gameplay_keep, the link object and its joint table
    uint32_t segment = PauseSegmentStart(mSpaceStart);
    const RomFile& link = OBJECTS[LINK_OBJECT_IDS[linkAge]];
    uint32_t keepAddress = segment + PAUSE_RENDER_TEXTURE_SIZE;
    uint32_t linkAddress = keepAddress + PAUSE_KEEP_BUFFER_SIZE;
    PaintRuntimeData(segment, PAUSE_RENDER_TEXTURE_SIZE, "pause player render texture");
    PaintFile(keepAddress, OBJECTS[GAMEPLAY_KEEP_OBJECT_ID], "pause gameplay_keep");
    PaintFile(linkAddress, link, "pause link object");
    PaintRuntimeData(Align16(linkAddress + link.size), PAUSE_JOINT_TABLE_SIZE, "pause joint table");

    uint32_t icons = Align16(linkAddress + link.size + PAUSE_JOINT_TABLE_SIZE);
    PaintFile(icons, FILE_ICON_ITEM_STATIC, "icon_item_static");
    AgeReq age = linkAge == 0 ? AgeReq::Adult : AgeReq::Child;
    for (const ItemIcon& icon : ITEM_ICONS) {
        if (icon.offset != NO_ICON && icon.ageReq != AgeReq::None && icon.ageReq != age) {
            Paint(icons + icon.offset, ITEM_ICON_SIZE, Kind::GreyIcon, FILE_ICON_ITEM_STATIC.vrom + icon.offset,
                  icons + icon.offset, "icon_item_static (greyed icon)");
        }
    }

    bool dungeon = SCENES[mSceneId].dungeon;
    uint32_t names = PaintIconFiles(icons, dungeon ? FILE_ICON_ITEM_DUNGEON_STATIC : FILE_ICON_ITEM_FIELD_STATIC,
                                    dungeon ? "icon_item_dungeon_static" : "icon_item_field_static", japanese);
    uint32_t nameSize = std::max(MAP_NAME_TEX1_SIZE, ITEM_NAME_TEX_SIZE);
    PaintRuntimeData(names, nameSize, "pause item name texture");
    if (worldMapArea >= 0 && worldMapArea < WORLD_MAP_AREA_COUNT) {
        int language = japanese ? 0 : 1;
        uint32_t offset =
            (worldMapArea + WORLD_MAP_AREA_COUNT * language) * MAP_NAME_TEX2_SIZE + 24 * MAP_NAME_TEX1_SIZE;
        PaintFile(names + nameSize, RomFile{ FILE_MAP_NAME_STATIC.vrom + offset, MAP_NAME_TEX2_SIZE },
                  "map_name_static");
    }
    PaintRuntimeData(Align16(names + nameSize + MAP_NAME_TEX2_SIZE), PAUSE_COVERAGE_SIZE, "pause player coverage");
}

void ObjectSpace::GameOverOpened(bool japanese) {
    if (mSpaceStart == 0) {
        return;
    }
    uint32_t icons = PauseSegmentStart(mSpaceStart);
    PaintFile(icons, FILE_ICON_ITEM_STATIC, "icon_item_static");
    PaintIconFiles(icons, FILE_ICON_ITEM_GAMEOVER_STATIC, "icon_item_gameover_static", japanese);
}

bool ObjectSpace::ReadWord(uint32_t address, const RomReader& rom, uint32_t* value, std::string* source) const {
    auto it = mSpans.upper_bound(address);
    if (it == mSpans.begin()) {
        *source = "not modelled";
        return false;
    }
    --it;
    const Span& span = it->second;
    if (address + 4 > span.end) {
        *source = address < span.end ? span.label + " (word crosses a boundary)" : "not modelled";
        return false;
    }
    *source = span.label + " +" + Hex(address - span.base);
    if (span.kind == Kind::Runtime) {
        return false;
    }
    uint8_t bytes[4];
    if (!rom(span.vrom + (address - span.base), bytes, 4)) {
        *source += " (N64 data not available)";
        return false;
    }
    uint32_t word = ReadBigEndian32(bytes);
    *value = span.kind == Kind::GreyIcon ? GreyOutPixel(word) : word;
    return true;
}

static const int32_t CS_CMD_DESTINATION = 0x3E8;

static bool IsHandledCommand(int32_t command) {
    return std::find(std::begin(CUTSCENE_COMMANDS), std::end(CUTSCENE_COMMANDS), command) !=
           std::end(CUTSCENE_COMMANDS);
}

// How SoH's cutscene importer lays out each N64 word: whole, two halves, two bytes and a half, or a half and two bytes
enum class Field { Word, Halves, BytesHalf, HalfBytes };

static int32_t ToHost(uint32_t word, Field field) {
    uint8_t bytes[4];
    uint16_t halves[2] = { static_cast<uint16_t>(word >> 16), static_cast<uint16_t>(word) };
    switch (field) {
        case Field::Word:
            memcpy(bytes, &word, sizeof(word));
            break;
        case Field::Halves:
            memcpy(bytes, halves, sizeof(halves));
            break;
        case Field::BytesHalf:
            bytes[0] = static_cast<uint8_t>(word >> 24);
            bytes[1] = static_cast<uint8_t>(word >> 16);
            memcpy(bytes + 2, &halves[1], sizeof(uint16_t));
            break;
        case Field::HalfBytes:
            memcpy(bytes, &halves[0], sizeof(uint16_t));
            bytes[2] = static_cast<uint8_t>(word >> 8);
            bytes[3] = static_cast<uint8_t>(word);
            break;
    }
    int32_t value;
    memcpy(&value, bytes, sizeof(value));
    return value;
}

// Copies N64 words into a host script, advancing through N64 memory
class ScriptCopier {
  public:
    ScriptCopier(const WordReader& read, uint32_t address, std::vector<int32_t>* out)
        : mRead(read), mAddress(address), mOut(out) {
    }
    bool Copy(Field field, uint32_t* word = nullptr) {
        uint32_t value;
        if (!mRead(mAddress, &value, &mSource)) {
            return false;
        }
        mAddress += 4;
        mOut->push_back(ToHost(value, field));
        if (word != nullptr) {
            *word = value;
        }
        return true;
    }
    bool CopyFields(std::initializer_list<Field> fields) {
        for (Field field : fields) {
            if (!Copy(field)) {
                return false;
            }
        }
        return true;
    }
    uint32_t Address() const {
        return mAddress;
    }
    const std::string& Source() const {
        return mSource;
    }

  private:
    const WordReader& mRead;
    uint32_t mAddress;
    std::vector<int32_t>* mOut;
    std::string mSource;
};

static bool CopyEntries(ScriptCopier& copier, std::initializer_list<Field> entry) {
    uint32_t count;
    if (!copier.Copy(Field::Word, &count)) {
        return false;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (!copier.CopyFields(entry)) {
            return false;
        }
    }
    return true;
}

// Camera commands are a start and end frame followed by points up to the one flagged CS_CAM_STOP
static bool CopyCamera(ScriptCopier& copier) {
    const uint32_t MAX_POINTS = 0x1000;
    if (!copier.CopyFields({ Field::Halves, Field::Halves })) {
        return false;
    }
    for (uint32_t i = 0; i < MAX_POINTS; i++) {
        uint32_t flags;
        if (!copier.Copy(Field::BytesHalf, &flags) ||
            !copier.CopyFields({ Field::Word, Field::Halves, Field::Halves })) {
            return false;
        }
        if (static_cast<int8_t>(flags >> 24) == -1) {
            return true;
        }
    }
    return false;
}

static bool CopyCommand(ScriptCopier& copier, int32_t command) {
    const std::initializer_list<Field> CUE = { Field::Halves, Field::Halves, Field::Halves, Field::Word,
                                               Field::Word,   Field::Word,   Field::Word,   Field::Word,
                                               Field::Word,   Field::Word,   Field::Word,   Field::Word };
    const std::initializer_list<Field> LIST = { Field::Halves, Field::Halves, Field::Word, Field::Word,
                                                Field::Word,   Field::Word,   Field::Word, Field::Word,
                                                Field::Word,   Field::Word,   Field::Word, Field::Word };
    switch (command) {
        case 0x01: // CS_CMD_CAM_EYE_SPLINE
        case 0x02: // CS_CMD_CAM_AT_SPLINE
        case 0x05: // CS_CMD_CAM_EYE_SPLINE_REL_TO_PLAYER
        case 0x06: // CS_CMD_CAM_AT_SPLINE_REL_TO_PLAYER
        case 0x07: // CS_CMD_CAM_EYE
        case 0x08: // CS_CMD_CAM_AT
            return CopyCamera(copier);
        case 0x03: // CS_CMD_MISC
        case 0x04: // CS_CMD_LIGHT_SETTING
        case 0x56: // CS_CMD_START_SEQ
        case 0x57: // CS_CMD_STOP_SEQ
        case 0x7C: // CS_CMD_FADE_OUT_SEQ
            return CopyEntries(copier, LIST);
        case 0x09: // CS_CMD_RUMBLE_CONTROLLER
            return CopyEntries(copier, { Field::Halves, Field::HalfBytes, Field::BytesHalf });
        case 0x8C: // CS_CMD_TIME
            return CopyEntries(copier, { Field::Halves, Field::HalfBytes, Field::Word });
        case 0x13: // CS_CMD_TEXT
            return CopyEntries(copier, { Field::Halves, Field::Halves, Field::Halves });
        case 0x2D: // CS_CMD_TRANSITION
        case CS_CMD_DESTINATION:
            return copier.CopyFields({ Field::Word, Field::Halves, Field::Halves });
        default: // Player and actor cues
            return CopyEntries(copier, CUE);
    }
}

ScriptSimulation SimulateCutscene(uint32_t address, const WordReader& read) {
    // The same bytes are parsed every frame, so the first frame decides the outcome
    ScriptSimulation sim;
    uint32_t word = 0;
    std::string source;
    bool haveEntries = read(address, &word, &sim.source);
    sim.totalEntries = static_cast<int32_t>(word);
    if (!haveEntries || !read(address + 4, &word, &source)) {
        sim.detail = haveEntries ? source : sim.source;
        return sim;
    }
    sim.frameCount = static_cast<int32_t>(word);
    sim.headerKnown = true;
    if (sim.frameCount < 0) {
        sim.outcome = ScriptSimulation::Outcome::EndsImmediately;
        return sim;
    }

    // The parser's loop counters are s16, so a count above 0x7FFF only stops at CS_CMD_END_OF_SCRIPT
    const int32_t S16_MAX = 0x7FFF;
    const int32_t MAX_COMMANDS = 0x10000;
    std::vector<int32_t> commands;
    int32_t commandCount = 0;
    std::string firstCommand;
    std::string destination;
    uint32_t script = address + 8;
    int32_t entries = sim.totalEntries <= S16_MAX ? sim.totalEntries : MAX_COMMANDS;
    int32_t i = 0;
    for (; i < entries; i++) {
        if (!read(script, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t command = static_cast<int32_t>(word);
        script += 4;
        if (command == CS_CMD_END_OF_SCRIPT) {
            break;
        }
        if (IsHandledCommand(command)) {
            commands.push_back(command);
            ScriptCopier copier(read, script, &commands);
            if (!CopyCommand(copier, command)) {
                sim.detail = "command " + Hex(static_cast<uint32_t>(command)) + " at " + Hex(script - 4) + " reaches " +
                             copier.Source();
                return sim;
            }
            if (command == CS_CMD_DESTINATION) {
                uint32_t value;
                read(script + 4, &value, &source);
                destination = ", destination " + Hex(value >> 16);
            }
            if (commandCount++ == 0) {
                firstCommand = "command " + Hex(static_cast<uint32_t>(command)) + " at " + Hex(script - 4);
            }
            script = copier.Address();
            continue;
        }
        if (!read(script, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t commandEntries = static_cast<int32_t>(word);
        script += 4;
        if (commandEntries > S16_MAX) {
            sim.outcome = ScriptSimulation::Outcome::Hang;
            sim.detail = "unknown command " + Hex(static_cast<uint32_t>(command)) + " with " +
                         std::to_string(commandEntries) + " entries";
            return sim;
        }
        if (commandEntries > 0) {
            script += 0x30u * static_cast<uint32_t>(commandEntries);
        }
    }
    if (i == MAX_COMMANDS) {
        sim.outcome = ScriptSimulation::Outcome::Hang;
        sim.detail = "command loop never reaches totalEntries";
        return sim;
    }
    if (commandCount == 0) {
        sim.outcome = ScriptSimulation::Outcome::NoCommands;
        sim.detail = i < entries ? "reaches CS_CMD_END_OF_SCRIPT" : "only unrecognised commands, all skipped";
        return sim;
    }
    sim.outcome = ScriptSimulation::Outcome::RunsCommands;
    sim.detail = firstCommand + ", " + std::to_string(commandCount) + " commands" + destination;
    sim.script = { commandCount, sim.frameCount };
    sim.script.insert(sim.script.end(), commands.begin(), commands.end());
    sim.script.push_back(CS_CMD_END_OF_SCRIPT);
    sim.script.push_back(0);
    return sim;
}

} // namespace n64heap

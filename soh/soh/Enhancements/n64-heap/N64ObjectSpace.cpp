#include "N64ObjectSpace.h"
#include "N64HeapCore.h"
#include "N64HeapTables.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
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
    PaintRuntimeData(kKaleidoArea, kKaleidoAreaSize, "kaleido/player overlay area");
    PaintRuntimeData(kMessageArea, kMessageAreaSize, "message textbox area");
    PaintRuntimeData(kEffectArea, kEffectAreaSize, "EffectSs table");

    const SceneLayout* layout = FindSceneLayout(sceneId, sceneLayer);
    if (sceneId < 0 || sceneId >= kSceneCount || kScenes[sceneId].file.size == 0 || layout == nullptr) {
        mSpaceStart = 0;
        return;
    }
    // The object space sits directly below the scene file
    const SceneEntry& scene = kScenes[sceneId];
    mSpaceStart = scene.base - layout->objectSpaceSize;
    char label[48];
    snprintf(label, sizeof(label), "scene 0x%02X file", sceneId);
    PaintFile(scene.base, scene.file, label);
}

void ObjectSpace::PaintObject(size_t slot) {
    int16_t id = mSlotIds[slot];
    if (id <= 0 || id >= kObjectCount || kObjects[id].size == 0) {
        return;
    }
    char label[48];
    snprintf(label, sizeof(label), "object 0x%03X (slot %zu)", id, slot);
    PaintFile(mSlotAddresses[slot], kObjects[id], label);
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
            uint32_t previousSize = (previous > 0 && previous < kObjectCount) ? kObjects[previous].size : 0;
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
    uint32_t icons24 = Align16(address + kFileIconItemStatic.size);
    PaintFile(icons24, kFileIconItem24Static, "icon_item_24_static");
    uint32_t area = Align16(icons24 + kFileIconItem24Static.size);
    PaintFile(area, areaIcons, areaLabel);
    uint32_t language = Align16(area + areaIcons.size);
    const RomFile& languageFile = japanese ? kFileIconItemJpnStatic : kFileIconItemNesStatic;
    PaintFile(language, languageFile, japanese ? "icon_item_jpn_static" : "icon_item_nes_static");
    return Align16(language + languageFile.size);
}

void ObjectSpace::PauseOpened(int linkAge, bool japanese, int16_t worldMapArea) {
    if (mSpaceStart == 0 || linkAge < 0 || linkAge > 1) {
        return;
    }
    // The player preview comes first: render texture, gameplay_keep, the link object and its joint table
    uint32_t segment = PauseSegmentStart(mSpaceStart);
    const RomFile& link = kObjects[kLinkObjectIds[linkAge]];
    uint32_t keepAddress = segment + kPauseRenderTextureSize;
    uint32_t linkAddress = keepAddress + kPauseKeepBufferSize;
    PaintRuntimeData(segment, kPauseRenderTextureSize, "pause player render texture");
    PaintFile(keepAddress, kObjects[kObjectGameplayKeep], "pause gameplay_keep");
    PaintFile(linkAddress, link, "pause link object");
    PaintRuntimeData(Align16(linkAddress + link.size), kPauseJointTableSize, "pause joint table");

    uint32_t icons = Align16(linkAddress + link.size + kPauseJointTableSize);
    PaintFile(icons, kFileIconItemStatic, "icon_item_static");
    AgeReq age = linkAge == 0 ? AgeReq::Adult : AgeReq::Child;
    for (const ItemIcon& icon : kItemIcons) {
        if (icon.offset != kNoIcon && icon.ageReq != AgeReq::None && icon.ageReq != age) {
            Paint(icons + icon.offset, kItemIconSize, Kind::GreyIcon, kFileIconItemStatic.vrom + icon.offset,
                  icons + icon.offset, "icon_item_static (greyed icon)");
        }
    }

    bool dungeon = kScenes[mSceneId].dungeon;
    uint32_t names = PaintIconFiles(icons, dungeon ? kFileIconItemDungeonStatic : kFileIconItemFieldStatic,
                                    dungeon ? "icon_item_dungeon_static" : "icon_item_field_static", japanese);
    uint32_t nameSize = std::max(kMapNameTex1Size, kItemNameTexSize);
    PaintRuntimeData(names, nameSize, "pause item name texture");
    if (worldMapArea >= 0 && worldMapArea < kWorldMapAreaCount) {
        int language = japanese ? 0 : 1;
        uint32_t offset = (worldMapArea + kWorldMapAreaCount * language) * kMapNameTex2Size + 24 * kMapNameTex1Size;
        PaintFile(names + nameSize, RomFile{ kFileMapNameStatic.vrom + offset, kMapNameTex2Size }, "map_name_static");
    }
    PaintRuntimeData(Align16(names + nameSize + kMapNameTex2Size), kPauseCoverageSize, "pause player coverage");
}

void ObjectSpace::GameOverOpened(bool japanese) {
    if (mSpaceStart == 0) {
        return;
    }
    uint32_t icons = PauseSegmentStart(mSpaceStart);
    PaintFile(icons, kFileIconItemStatic, "icon_item_static");
    PaintIconFiles(icons, kFileIconItemGameoverStatic, "icon_item_gameover_static", japanese);
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
        *source += " (ROM not readable)";
        return false;
    }
    uint32_t word = ReadBigEndian32(bytes);
    *value = span.kind == Kind::GreyIcon ? GreyOutPixel(word) : word;
    return true;
}

bool ObjectSpace::IsHandledCommand(int32_t command) {
    return std::find(std::begin(kCutsceneCommands), std::end(kCutsceneCommands), command) !=
           std::end(kCutsceneCommands);
}

ScriptSimulation ObjectSpace::Simulate(uint32_t address, const RomReader& rom) const {
    // The same bytes are parsed every frame, so the first frame decides the outcome
    ScriptSimulation sim;
    uint32_t word = 0;
    std::string source;
    bool haveEntries = ReadWord(address, rom, &word, &sim.source);
    sim.totalEntries = static_cast<int32_t>(word);
    if (!haveEntries || !ReadWord(address + 4, rom, &word, &source)) {
        sim.detail = haveEntries ? source : sim.source;
        return sim;
    }
    sim.frameCount = static_cast<int32_t>(word);
    sim.headerKnown = true;
    if (sim.frameCount < 0) {
        sim.outcome = ScriptSimulation::Outcome::EndsImmediately;
        return sim;
    }

    // The parser's loop counters are s16, so a count above 0x7FFF never finishes
    const int32_t kS16Max = 0x7FFF;
    uint32_t script = address + 8;
    int32_t entries = sim.totalEntries <= kS16Max ? sim.totalEntries : kS16Max + 1;
    for (int32_t i = 0; i < entries; i++) {
        if (!ReadWord(script, rom, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t command = static_cast<int32_t>(word);
        script += 4;
        if (command == kCsCmdEndOfScript) {
            sim.outcome = ScriptSimulation::Outcome::NoCommands;
            sim.detail = "reaches CS_CMD_END_OF_SCRIPT";
            return sim;
        }
        if (IsHandledCommand(command)) {
            sim.outcome = ScriptSimulation::Outcome::RunsCommands;
            sim.detail = "command " + Hex(static_cast<uint32_t>(command)) + " at " + Hex(script - 4);
            return sim;
        }
        if (!ReadWord(script, rom, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t commandEntries = static_cast<int32_t>(word);
        script += 4;
        if (commandEntries > kS16Max) {
            sim.outcome = ScriptSimulation::Outcome::Hang;
            sim.detail = "unknown command " + Hex(static_cast<uint32_t>(command)) + " with " +
                         std::to_string(commandEntries) + " entries";
            return sim;
        }
        if (commandEntries > 0) {
            script += 0x30u * static_cast<uint32_t>(commandEntries);
        }
    }
    if (sim.totalEntries > kS16Max) {
        sim.outcome = ScriptSimulation::Outcome::Hang;
        sim.detail = "command loop never reaches totalEntries";
        return sim;
    }
    sim.outcome = ScriptSimulation::Outcome::NoCommands;
    sim.detail = "only unrecognised commands, all skipped";
    return sim;
}

} // namespace n64heap

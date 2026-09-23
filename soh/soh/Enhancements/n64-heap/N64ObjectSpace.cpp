#include "N64ObjectSpace.h"
#include "N64HeapTables.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace n64heap {

static uint32_t Align16(uint32_t value) {
    return (value + 15) & ~15u;
}

static std::string Hex(uint32_t value) {
    char text[16];
    snprintf(text, sizeof(text), "0x%X", value);
    return text;
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

void ObjectSpace::PaintFile(uint32_t address, uint32_t vrom, uint32_t size, const std::string& label) {
    Paint(address, size, Kind::File, vrom, address, label);
}

void ObjectSpace::SceneInit(int16_t sceneId, uint8_t sceneLayer) {
    // The scene file sits below the fixed areas and the object space below the scene file
    mSceneId = sceneId;
    mSlotIds.clear();
    mSlotAddr.clear();
    Paint(kKaleidoArea, kKaleidoAreaSize, Kind::Dynamic, 0, 0, "kaleido/player overlay area (runtime)");
    Paint(kMessageArea, kMessageAreaSize, Kind::Dynamic, 0, 0, "message textbox area (runtime)");
    Paint(kEffectArea, kEffectAreaSize, Kind::Dynamic, 0, 0, "EffectSs table (runtime)");
    const SceneLayout* layout = FindSceneLayout(sceneId, sceneLayer);
    if (sceneId < 0 || sceneId >= kSceneCount || kScenes[sceneId].file.size == 0 || layout == nullptr) {
        mSpaceStart = 0;
        return;
    }
    const SceneEntry& scene = kScenes[sceneId];
    mSpaceStart = scene.base - layout->objectSpaceSize;
    char label[48];
    snprintf(label, sizeof(label), "scene 0x%02X file", sceneId);
    PaintFile(scene.base, scene.file.vrom, scene.file.size, label);
}

void ObjectSpace::PaintObject(size_t slot) {
    int16_t id = mSlotIds[slot];
    if (id <= 0 || id >= kObjectCount || kObjects[id].size == 0) {
        return;
    }
    char label[48];
    snprintf(label, sizeof(label), "object 0x%03X (slot %zu)", id, slot);
    PaintFile(mSlotAddr[slot], kObjects[id].vrom, kObjects[id].size, label);
}

void ObjectSpace::SyncObjects(const int16_t* ids, int count) {
    if (mSpaceStart == 0) {
        return;
    }
    // Matching slots are kept and the rest are loaded one after another
    size_t first = 0;
    while (first < mSlotIds.size() && first < static_cast<size_t>(count) &&
           mSlotIds[first] == static_cast<int16_t>(std::abs(ids[first]))) {
        first++;
    }
    if (first == mSlotIds.size() && first == static_cast<size_t>(count)) {
        return;
    }
    mSlotIds.resize(count);
    mSlotAddr.resize(count);
    for (size_t i = first; i < static_cast<size_t>(count); i++) {
        mSlotIds[i] = static_cast<int16_t>(std::abs(ids[i]));
        if (i == 0) {
            mSlotAddr[i] = mSpaceStart;
        } else {
            int16_t prev = mSlotIds[i - 1];
            uint32_t prevSize = (prev > 0 && prev < kObjectCount) ? kObjects[prev].size : 0;
            mSlotAddr[i] = Align16(mSlotAddr[i - 1] + prevSize);
        }
        PaintObject(i);
    }
}

void ObjectSpace::PauseClosed() {
    for (size_t i = 0; i < mSlotIds.size(); i++) {
        PaintObject(i);
    }
}

void ObjectSpace::PauseOpened(int linkAge, bool japanese, int16_t worldMapArea) {
    if (mSpaceStart == 0 || linkAge < 0 || linkAge > 1) {
        return;
    }
    uint32_t segment = (mSpaceStart + 0x30) & ~0x3Fu;
    const RomFile& keep = kObjects[kObjectGameplayKeep];
    const RomFile& link = kObjects[kLinkObjectIds[linkAge]];
    Paint(segment, kPauseRenderTextureSize, Kind::Dynamic, 0, 0, "pause player render texture (runtime)");
    PaintFile(segment + kPauseRenderTextureSize, keep.vrom, keep.size, "pause gameplay_keep");
    uint32_t linkAddr = segment + kPauseRenderTextureSize + kPauseKeepBufferSize;
    PaintFile(linkAddr, link.vrom, link.size, "pause link object");
    Paint(Align16(linkAddr + link.size), kPauseJointTableSize, Kind::Dynamic, 0, 0, "pause joint table (runtime)");
    uint32_t size1 = kPauseRenderTextureSize + kPauseKeepBufferSize + link.size + kPauseJointTableSize;

    uint32_t icons = Align16(segment + size1);
    PaintFile(icons, kFile_icon_item_static.vrom, kFile_icon_item_static.size, "icon_item_static");
    for (int i = 0; i < kItemAgeReqCount; i++) {
        const ItemIcon& icon = kItemIcons[i];
        if (icon.offset != 0xFFFFFFFFu && icon.ageReq != 9 && icon.ageReq != linkAge) {
            Paint(icons + icon.offset, 32 * 32 * 4, Kind::GreyIcon, kFile_icon_item_static.vrom + icon.offset,
                  icons + icon.offset, "icon_item_static (greyed icon)");
        }
    }
    uint32_t icons24 = Align16(icons + kFile_icon_item_static.size);
    PaintFile(icons24, kFile_icon_item_24_static.vrom, kFile_icon_item_24_static.size, "icon_item_24_static");
    uint32_t alt = Align16(icons24 + kFile_icon_item_24_static.size);
    const RomFile& altFile = kScenes[mSceneId].dungeon ? kFile_icon_item_dungeon_static : kFile_icon_item_field_static;
    PaintFile(alt, altFile.vrom, altFile.size,
              kScenes[mSceneId].dungeon ? "icon_item_dungeon_static" : "icon_item_field_static");
    uint32_t lang = Align16(alt + altFile.size);
    const RomFile& langFile = japanese ? kFile_icon_item_jpn_static : kFile_icon_item_nes_static;
    PaintFile(lang, langFile.vrom, langFile.size, japanese ? "icon_item_jpn_static" : "icon_item_nes_static");
    uint32_t names = Align16(lang + langFile.size);
    uint32_t nameSize = std::max(kMapNameTex1Size, kItemNameTexSize);
    Paint(names, nameSize, Kind::Dynamic, 0, 0, "pause item name texture (cursor dependent)");
    if (worldMapArea >= 0 && worldMapArea < 22) {
        uint32_t offset = (worldMapArea + 22 * (japanese ? 0 : 1)) * kMapNameTex2Size + 24 * kMapNameTex1Size;
        PaintFile(names + nameSize, kFile_map_name_static.vrom + offset, kMapNameTex2Size, "map_name_static");
    }
    // Coverage image of the player pre-render, written while the pause screen draws
    Paint(Align16(names + nameSize + kMapNameTex2Size), 64 * 112, Kind::Dynamic, 0, 0,
          "pause player coverage buffer (runtime)");
}

void ObjectSpace::GameOverOpened(bool japanese) {
    if (mSpaceStart == 0) {
        return;
    }
    uint32_t icons = (mSpaceStart + 0x30) & ~0x3Fu;
    PaintFile(icons, kFile_icon_item_static.vrom, kFile_icon_item_static.size, "icon_item_static");
    uint32_t icons24 = Align16(icons + kFile_icon_item_static.size);
    PaintFile(icons24, kFile_icon_item_24_static.vrom, kFile_icon_item_24_static.size, "icon_item_24_static");
    uint32_t alt = Align16(icons24 + kFile_icon_item_24_static.size);
    PaintFile(alt, kFile_icon_item_gameover_static.vrom, kFile_icon_item_gameover_static.size,
              "icon_item_gameover_static");
    uint32_t lang = Align16(alt + kFile_icon_item_gameover_static.size);
    const RomFile& langFile = japanese ? kFile_icon_item_jpn_static : kFile_icon_item_nes_static;
    PaintFile(lang, langFile.vrom, langFile.size, japanese ? "icon_item_jpn_static" : "icon_item_nes_static");
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
    if (span.kind == Kind::Dynamic) {
        return false;
    }
    uint8_t bytes[4];
    if (!rom(span.vrom + (address - span.base), bytes, 4)) {
        *source += " (ROM not readable)";
        return false;
    }
    uint32_t word = (uint32_t(bytes[0]) << 24) | (uint32_t(bytes[1]) << 16) | (uint32_t(bytes[2]) << 8) | bytes[3];
    if (span.kind == Kind::GreyIcon && (word & 0xFFFFFF00u) != 0) {
        uint32_t rgb = word >> 8;
        uint32_t grey = ((((rgb & 0xFF0000) >> 16) + ((rgb & 0xFF00) >> 7) + (rgb & 0xFF)) / 7) & 0xFF;
        word = (((grey << 16) | (grey << 8) | grey) << 8) | (word & 0xFF);
    }
    *value = word;
    return true;
}

ScriptSimulation ObjectSpace::Simulate(uint32_t address, const RomReader& rom) const {
    // The same bytes are parsed every frame, so the first frame decides the outcome
    ScriptSimulation sim;
    uint32_t word = 0;
    std::string source;
    bool first = ReadWord(address, rom, &word, &sim.source);
    sim.totalEntries = static_cast<int32_t>(word);
    if (!first || !ReadWord(address + 4, rom, &word, &source)) {
        sim.detail = first ? source : sim.source;
        return sim;
    }
    sim.frameCount = static_cast<int32_t>(word);
    sim.headerKnown = true;
    if (sim.frameCount < 0) {
        sim.outcome = ScriptSimulation::Outcome::EndsImmediately;
        return sim;
    }
    uint32_t script = address + 8;
    // The s16 loop counter never reaches a count above 0x7FFF
    const int32_t limit = sim.totalEntries <= 0x7FFF ? sim.totalEntries : 0x10000;
    for (int32_t i = 0; i < limit; i++) {
        if (!ReadWord(script, rom, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t cmdType = static_cast<int32_t>(word);
        script += 4;
        if (cmdType == kCsCmdEndOfScript) {
            sim.outcome = ScriptSimulation::Outcome::NoCommands;
            sim.detail = "reaches CS_CMD_END_OF_SCRIPT";
            return sim;
        }
        for (int c = 0; c < kCutsceneCommandCount; c++) {
            if (kCutsceneCommands[c] == cmdType) {
                sim.outcome = ScriptSimulation::Outcome::RunsCommands;
                sim.detail = "command " + Hex(static_cast<uint32_t>(cmdType)) + " at " + Hex(script - 4);
                return sim;
            }
        }
        if (!ReadWord(script, rom, &word, &source)) {
            sim.detail = source;
            return sim;
        }
        int32_t cmdEntries = static_cast<int32_t>(word);
        script += 4;
        if (cmdEntries > 0x7FFF) {
            sim.outcome = ScriptSimulation::Outcome::Hang;
            sim.detail = "unknown command " + Hex(static_cast<uint32_t>(cmdType)) + " with " +
                         std::to_string(cmdEntries) + " entries";
            return sim;
        }
        if (cmdEntries > 0) {
            script += 0x30u * static_cast<uint32_t>(cmdEntries);
        }
    }
    if (sim.totalEntries > 0x7FFF) {
        sim.outcome = ScriptSimulation::Outcome::Hang;
        sim.detail = "command loop never reaches totalEntries";
        return sim;
    }
    sim.outcome = ScriptSimulation::Outcome::NoCommands;
    sim.detail = "only unrecognised commands, all skipped";
    return sim;
}

} // namespace n64heap

#include "ItemTableManager.h"
#include <stdexcept>

#include "variables.h"
#include "libultraship/libultra/types.h"

ItemTableManager::ItemTableManager() {
}

ItemTableManager::~ItemTableManager() {
    this->itemTables.clear();
}

bool ItemTableManager::AddItemTable(uint16_t tableID) {
    ItemTable newItemTable;
    return itemTables.emplace(tableID, newItemTable).second;
}

bool ItemTableManager::AddItemEntry(uint16_t tableID, uint16_t getItemID, GetItemEntry getItemEntry) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        return itemTable->emplace(getItemID, getItemEntry).second;
    } catch ([[maybe_unused]] const std::out_of_range& oor) { return false; }
}

// This array is structured from input index (getItemID & 0xFF) - 128
// The value is the slot that we get redirected to based on GIM/SRM.
// The comment notes what the value should be when glitch item is aquired.
const uint8_t GimIdTable[] = {
    111, // 128 Poe
    101, // 129 Bombs (+1)
    0, // 130 N/A
    22, // 131 Magic Bean
    2, // 132 Deku Nut (+1)
    101, // 133 Bombs (+1)
    0, // 134 N/A
    102, // 135 Bombs (+10)
    44, // 136 Goron Tunic
    0, // 137 N/A
    88, // 138 Fire Arrow
    47, // 139 Hover Boots
    7, // 140 Deku Stick (+1)
    7, // 141 Deku Stick (+1)
    88, // 142 Fire Arrow
    26, // 143 Keaton Mask
    0, // 144 N/A
    1, // Bombs (+5)
    88, // 146 Fire Arrow
    16, // 147 Red Potion
    7, // 148 Deku Stick (+1)
    89, // 149 Ice Arrow
    7, // 150 Deku Stick (+1)
    101, // 151 Bombs (+1)
    5, // 152 Slingshot
    35, // 153 Prescription
    0, // 154 N/A
    1, // 155 Bombs (+5)
    5, // 156 Slingshot
    102, // 157 Bombs (+10)
    2, // 158 Deku Nut(+1)
    32, // 159 Odd Potion
    7, // 160 Deku Stick (+1)
    2, // 161 Deku Nut
    7, // 162 Deku Stick (+1)
    0, // 163 N/A
    0, // 164 N/A
    102, // 165 Bombs (+10)
    7, // 166 Deku Stick (+1)
    4, // 167 Bow
    7, // 168 Deku Stick (+1)
    7, // 169 Deku Stick (+1)
    2, // 170 Deku Nut(+1)
    2, // 171 Deku Nut(+1)
    7, // 172 Deku Stick (+1)
    44, // 173 Goron Tunic
    7, // 174 Deku Stick (+1)
    44, // 175 Goron Tunic
    0, // 176 N/A
    44, // 177 Goron Tunic
    7, // 178 Deku Stick (+1)
    44, // 179 Goron Tunic
    7, // 180 Deku Stick (+1)
    44, // 181 Goron Tunic
    7, // 182 Deku Stick (+1)
    44, // 183 Goron Tunic
    90, // 184 Light Arrow
    90, // 185 Light Arrow
    18, // 186 Blue Potion
    90, // 187 Light Arrow
    18, // 188 Blue Potion
    18, // 189 Blue Potion
    112, // 190 Big Poe
    18, // 191 Blue Potion
    112, // 192 Big Poe
    90, // 193 Light Arrow
    18, // 194 Blue Potion
    90, // 195 Light Arrow
    18, // 196 Blue Potion
    7, // 197 Deku Stick (+1)
    27, // 198 Bunny Hood
    88, // 199 Fire Arrow
    30, // 200 Pocket Cucco
    88, // 201 Fire Arrow
    30, // 202 Pocket Cucco
    88, // 203 Fire Arrow
    29, // 204 Pocket Egg
    88, // 205 Fire Arrow
    30, // 206 Pocket Cucco
    88, // 207 Fire Arrow
    7, // 208 Deku Stick (+1)
    42, // 209 Hylian Shield
    7, // 210 Deku Stick (+1)
    44, // 211 Goron Tunic
    7, // 212 Deku Stick (+1)
    0, // 213 No Reswap Available! // 213 Kokiri Tunic
    7, // 214 Deku Stick (+1))
    44, // 215 Goron Tunic
    7, // 216 Deku Stick (+1)
    0, // 217 No Reswap Available! // 217 Kokiri Tunic
    7, // 218 Deku Stick (+1)
    0, // 219 N/A
    0, // 220 N/A
    0, // 221 N/A
    7, // 222 Deku Stick (+1)
    7, // 223 Deku Stick (+1)
    0, // 224 N/A
    0, // 225 N/A
    7, // 226 Deku Stick (+1)
    7, // 227 Deku Stick (+1)
    7, // 228 Deku Stick (+1)
    7, // 229 Deku Stick (+1)
    0, // 230 N/A
    0, // 231 N/A
    7, // 232 Deku Stick (+1)
    88, // 233 Fire Arrow
    11, // 234 Zelda's Letter
    88, // 235 Fire Arrow
    11, // 236 Zelda's Letter
    88, // 237 Fire Arrow
    29, // 238 Pocket Egg
    88, // 239 Fire Arrow
    29, // 240 Pocket Egg
    88, // 241 Fire Arrow
    11, // 242 Zelda's Letter
    7, // 243 Deku Stick (+1)
    7, // 244 Deku Stick (+1)
    7, // 245 Deku Stick (+1)
    7, // 246 Deku Stick (+1)
    42, // 247 Hylian Shield
    7, // 248 Deku Stick (+1)
    7, // 249 Deku Stick (+1)
    7, // 250 Deku Stick (+1)
    7, // 251 Deku Stick (+1)
    7, // 252 Deku Stick (+1)
    7, // 253 Deku Stick (+1)
    7, // 254 Deku Stick (+1)
    7, // 255 Deku Stick (+1)
};

GetItemEntry ItemTableManager::RetrieveGimEntry(int16_t tableID, uint16_t getItemID) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);

        auto offset = getItemID & 0xFF;
        if (offset < 128) getItemID = 0;
        else getItemID = GimIdTable[offset - 128];

        GetItemEntry getItemEntry = itemTable->at(getItemID);
        auto item = getItemEntry.itemId;

        // bottle items
        if (item >= 0x15 && item <= 0x20) {
            for (u16 i = 0; i < 4; i++) {
                if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] == ITEM_NONE) {
                    gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] = item;
                    break;
                }
            }
        }

        getItemEntry.drawItemId = getItemEntry.itemId;
        getItemEntry.drawModIndex = getItemEntry.modIndex;
        return getItemEntry;
    } catch (std::out_of_range& oor) { return GET_ITEM_NONE; }
}

GetItemEntry ItemTableManager::RetrieveItemEntry(uint16_t tableID, uint16_t getItemID) {
    if (getItemID > 32767) return RetrieveGimEntry(tableID, getItemID);

    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        GetItemEntry getItemEntry = itemTable->at(getItemID);
        getItemEntry.drawItemId = getItemEntry.itemId;
        getItemEntry.drawModIndex = getItemEntry.modIndex;
        return getItemEntry;
    } catch ([[maybe_unused]] std::out_of_range& oor) { return GET_ITEM_NONE; }
}

bool ItemTableManager::ClearItemTable(uint16_t tableID) {
    try {
        ItemTable* itemTable = RetrieveItemTable(tableID);
        itemTable->clear();
        return true;
    } catch ([[maybe_unused]] const std::out_of_range& oor) { return false; }
}

ItemTable* ItemTableManager::RetrieveItemTable(uint16_t tableID) {
    try {
        return &itemTables.at(tableID);
    } catch (const std::out_of_range& oor) { throw(oor); }
}

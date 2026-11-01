#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

#include "soh/resource/type/Text.h"
#include <spdlog/spdlog.h>
#include <algorithm>

extern "C" {
#include <message_data_static.h>
}

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sJpnMessageEntryTablePtr;
extern "C" MessageTableEntry* sStaffMessageEntryTablePtr;
// extern "C" MessageTableEntry* _message_0xFFFC_nes;

// Keep message files loaded; the tables point into their strings
static std::vector<std::shared_ptr<SOH::Text>> sMessageFiles;

static MessageTableEntry* BuildTable(const std::vector<MessageTableEntry>& entries) {
    // OTRTODO: Should not be malloc'ing here. It's fine for now since we check elsewhere that the message table is
    // already null.
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * entries.size());

    std::copy(entries.begin(), entries.end(), table);

    return table;
}

static MessageTableEntry BuildEntry(const SOH::MessageEntry& msg) {
    MessageTableEntry entry;
    entry.textId = msg.id;
    entry.typePos = (msg.textboxType << 4) | msg.textboxYPos;
    entry.segment = msg.msg.c_str();
    entry.msgSize = static_cast<u32>(msg.msg.size());
    return entry;
}

static std::vector<MessageTableEntry> BuildEntries(const std::vector<SOH::MessageEntry>& messages) {
    std::vector<MessageTableEntry> entries;
    entries.reserve(messages.size());

    for (auto& msg : messages) {
        entries.push_back(BuildEntry(msg));
    }

    return entries;
}

static void UpsertEntry(std::vector<MessageTableEntry>& entries, const SOH::MessageEntry& msg) {
    if (msg.id == 0xFFFF) {
        SPDLOG_WARN("Message override 0xFFFF ends the table and can't be overridden");
        return;
    }

    if (msg.msg.size() > SOH::Text::MaxMessageSize) {
        SPDLOG_WARN("Message override 0x{:04X} is {} bytes, over the {} byte limit", msg.id, msg.msg.size(),
                    SOH::Text::MaxMessageSize);
        return;
    }

    // Lookups stop at the 0xFFFF entry, if the table has one
    auto end = std::find_if(entries.begin(), entries.end(),
                            [](const MessageTableEntry& entry) { return entry.textId == 0xFFFF; });
    auto existing =
        std::find_if(entries.begin(), end, [&](const MessageTableEntry& entry) { return entry.textId == msg.id; });

    if (existing == end) {
        entries.insert(end, BuildEntry(msg));
    } else {
        *existing = BuildEntry(msg);
    }
}

static std::vector<std::string> ListOverrideFiles(const std::string& folder) {
    auto archiveManager = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager();
    std::vector<std::string> files;

    // A .meta alias is loaded by its path without the extension
    auto paths = archiveManager->ListFiles(folder + "/*");
    for (auto& path : *paths) {
        files.push_back(path.ends_with(".meta") ? path.substr(0, path.size() - 5) : path);
    }
    std::sort(files.begin(), files.end());
    files.erase(std::unique(files.begin(), files.end()), files.end());

    return files;
}

static std::vector<std::shared_ptr<SOH::Text>> LoadOverrides(const std::string& folder) {
    auto resourceManager = Ship::Context::GetRawInstance()->GetResourceManager();
    auto archiveManager = resourceManager->GetArchiveManager();
    std::vector<std::shared_ptr<SOH::Text>> files;
    // XML V1 writes single-byte text, but the Japanese table is two-byte
    bool acceptsXmlV1 = !folder.ends_with("/jpn_message_data_static");

    for (auto& path : ListOverrideFiles(folder)) {
        auto file = std::dynamic_pointer_cast<SOH::Text>(resourceManager->LoadResource(path));

        if (file == nullptr) {
            SPDLOG_ERROR("Failed to load message override {}", path);
            continue;
        }

        if (!acceptsXmlV1 && file->GetInitData()->ResourceVersion != 0) {
            SPDLOG_WARN("Message override {} is XML V1, which the Japanese table can't use", path);
            continue;
        }

        files.push_back(file);
    }

    // Apply lowest-to-highest resolved archive priority so higher priority wins; keep path order for ties.
    std::stable_sort(files.begin(), files.end(), [&](const auto& a, const auto& b) {
        return archiveManager->GetFilePriority(a->GetInitData()->Path) <
               archiveManager->GetFilePriority(b->GetInitData()->Path);
    });

    return files;
}

static void ApplyOverrides(std::vector<MessageTableEntry>& entries, const std::string& folder) {
    for (auto& file : LoadOverrides(folder)) {
        for (auto& msg : file->messages) {
            UpsertEntry(entries, msg);
        }
        sMessageFiles.push_back(file);
    }
}

MessageTableEntry* OTRMessage_LoadTable(const std::string& filePath, bool isNES, uint16_t firstTextId) {
    auto file = std::static_pointer_cast<SOH::Text>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;

    sMessageFiles.push_back(file);
    std::vector<MessageTableEntry> entries = BuildEntries(file->messages);
    ApplyOverrides(entries, "override/" + filePath.substr(0, filePath.find_last_of('/')));

    if (isNES) {
        auto entry = std::find_if(entries.begin(), entries.end(),
                                  [](const MessageTableEntry& entry) { return entry.textId == 0xFFFC; });
        if (entry != entries.end()) {
            _message_0xFFFC_nes = (char*)entry->segment;
        }
    }

    // Assert that the first message starts at the first text ID
    assert(!entries.empty() && entries[0].textId == firstTextId);

    return BuildTable(entries);
}

extern "C" void OTRMessage_Init() {
    // OTRTODO: Added a lot of null checks here so that we don't malloc the table multiple times causing a memory leak.
    // We really ought to fix the implementation such that we aren't malloc'ing new tables.
    // Once we fix the implementation, remove these NULL checks.
    if (sNesMessageEntryTablePtr == NULL) {
        sNesMessageEntryTablePtr =
            OTRMessage_LoadTable("text/nes_message_data_static/nes_message_data_static", true, 0x0001);
    }
    if (sGerMessageEntryTablePtr == NULL) {
        sGerMessageEntryTablePtr =
            OTRMessage_LoadTable("text/ger_message_data_static/ger_message_data_static", false, 0x0001);
    }
    if (sFraMessageEntryTablePtr == NULL) {
        sFraMessageEntryTablePtr =
            OTRMessage_LoadTable("text/fra_message_data_static/fra_message_data_static", false, 0x0001);
    }
    if (sJpnMessageEntryTablePtr == NULL) {
        sJpnMessageEntryTablePtr =
            OTRMessage_LoadTable("text/jpn_message_data_static/jpn_message_data_static", false, 0x0001);
    }
    // Note: Make sure this loads after PAL nes_message_data_static, so that message 0xFFFC is definitely loaded if it
    // exists
    if (sNesMessageEntryTablePtr == NULL) {
        sNesMessageEntryTablePtr =
            OTRMessage_LoadTable("text/nes_message_data_static/ntsc_nes_message_data_static", false, 0x0001);
    }

    if (sStaffMessageEntryTablePtr == NULL) {
        sStaffMessageEntryTablePtr =
            OTRMessage_LoadTable("text/staff_message_data_static/staff_message_data_static", false, 0x0500);
    }
}

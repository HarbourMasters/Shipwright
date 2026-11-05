#include "TrackerLocalization.h"

#include <map>
#include <utility>
#include "soh/Enhancements/Lang/Lang.h"

namespace TrackerLocalization {
namespace {
struct Entry {
    std::string fallback;
    std::string text;
};
template <typename Value> using StringMap = std::map<std::string, Value, std::less<>>;
StringMap<StringMap<StringMap<Entry>>> names;
std::string locale;
uint64_t catalogRevision = 0;
uint64_t revision = 0;

void Refresh() {
    const auto currentRevision = Lang::GetUICatalogRevision();
    const auto* currentLocale = Lang::GetUILanguage();
    if (locale != currentLocale || catalogRevision != currentRevision) {
        locale = currentLocale;
        catalogRevision = currentRevision;
        names.clear();
        ++revision;
    }
}

template <typename Value> Value& GetOrInsert(StringMap<Value>& values, std::string_view key) {
    auto found = values.find(key);
    if (found == values.end()) {
        found = values.emplace(std::string(key), Value{}).first;
    }
    return found->second;
}
} // namespace

const std::string& Name(std::string_view section, std::string_view id, std::string_view fallback,
                        std::string_view field) {
    Refresh();
    auto& fields = GetOrInsert(GetOrInsert(names, section), id);
    auto found = fields.find(field);
    if (found == fields.end() || found->second.fallback != fallback) {
        std::string path = "ui.trackers.";
        path.append(section).append(".").append(id);
        if (!field.empty()) {
            path.append(".").append(field);
        }
        // Preserve an existing legacy-language translation when this locale has
        // no tracker entry yet (for example the current German catalog).
        const auto* translated = Lang::FindExactUITranslation(path);
        Entry entry{ std::string(fallback), translated != nullptr ? *translated : std::string(fallback) };
        if (found == fields.end()) {
            found = fields.emplace(std::string(field), std::move(entry)).first;
        } else {
            found->second = std::move(entry);
        }
    }
    return found->second.text;
}

uint64_t Revision() {
    Refresh();
    return revision;
}
} // namespace TrackerLocalization

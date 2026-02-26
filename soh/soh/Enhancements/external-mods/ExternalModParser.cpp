#include "ExternalModParser.h"

#include "ExternalModManager.h"

namespace SOH {

bool ExternalModParser::TryParseManifest(const std::string& content, ExternalModManifest& outManifest,
                                         std::string& outError) {
    return ExternalModManager::TryParseManifest(content, outManifest, outError);
}

bool ExternalModParser::TryParseEntryScript(const std::string& content, int32_t apiVersion,
                                            ExternalModRuntime& outRuntime, std::string& outError) {
    return ExternalModManager::TryParseEntryScript(content, apiVersion, outRuntime, outError);
}

bool ExternalModParser::TryParseItemDefinitions(const std::string& content,
                                                std::vector<ExternalModItemDefinition>& outItems,
                                                std::string& outError) {
    return ExternalModManager::TryParseItemDefinitions(content, outItems, outError);
}

bool ExternalModParser::TryParseInputDefinitions(const std::string& content,
                                                 std::vector<ExternalModInputBinding>& outBindings,
                                                 std::vector<ExternalModCameraHotkeyDefinition>& outCameraHotkeys,
                                                 std::string& outError) {
    return ExternalModManager::TryParseInputDefinitions(content, outBindings, outCameraHotkeys, outError);
}

bool ExternalModParser::TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModHookSubscription>& outSubscriptions,
                                                std::string& outError) {
    return ExternalModManager::TryParseHookDefinitions(content, apiVersion, outSubscriptions, outError);
}

bool ExternalModParser::TryParseActorDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModActorDefinition>& outDefinitions,
                                                 std::string& outError) {
    return ExternalModManager::TryParseActorDefinitions(content, apiVersion, outDefinitions, outError);
}

bool ExternalModParser::TryParseBehaviorDefinitions(const std::string& content, int32_t apiVersion,
                                                    std::vector<ExternalModBehaviorDefinition>& outDefinitions,
                                                    std::string& outError) {
    return ExternalModManager::TryParseBehaviorDefinitions(content, apiVersion, outDefinitions, outError);
}

bool ExternalModParser::TryParseSceneDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModSceneDefinition>& outDefinitions,
                                                 std::string& outError) {
    return ExternalModManager::TryParseSceneDefinitions(content, apiVersion, outDefinitions, outError);
}

} // namespace SOH

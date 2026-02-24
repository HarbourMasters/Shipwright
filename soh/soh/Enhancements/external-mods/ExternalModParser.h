e #pragma once

#include "ExternalModTypes.h"
#include <string>
#include <vector>

namespace SOH {

class ExternalModParser {
public:
    static bool TryParseManifest(const std::string& content, ExternalModManifest& outManifest, std::string& outError);
    static bool TryParseEntryScript(const std::string& content, int32_t apiVersion, ExternalModRuntime& outRuntime,
                                    std::string& outError);
    static bool TryParseItemDefinitions(const std::string& content, std::vector<ExternalModItemDefinition>& outItems,
                                        std::string& outError);
    static bool TryParseInputDefinitions(const std::string& content, std::vector<ExternalModInputBinding>& outBindings,
                                         std::string& outError);
    static bool TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModHookSubscription>& outSubscriptions,
                                        std::string& outError);
    static bool TryParseActorDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModActorDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseBehaviorDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModBehaviorDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseSceneDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModSceneDefinition>& outDefinitions,
                                         std::string& outError);
};

} // namespace SOH

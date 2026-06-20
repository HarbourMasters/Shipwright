#pragma once
#ifdef __cplusplus

#include <cstdint>
#include <memory>
#include <string>

namespace Ship {
class Archive;
}

struct Player;
struct AnchorTextureOverrides {
    bool hasEye;
    bool hasMouth;
    int eyeIndex;
    int mouthIndex;
    void* originalEye;
    void* originalMouth;
};

class AnchorModRegistry {
  public:
    static void Init();
    static const std::string& GetLocalModelId();
    static std::shared_ptr<Ship::Archive> FindArchiveById(const std::string& id);
    static bool ApplyModelToPlayer(const std::string& id, int32_t linkAge, Player* player);
    static bool HasCustomModel(const std::string& id, int32_t linkAge, int32_t expectedLimbCount);
    static void SetAnchorModelOverride(const std::string& modelId, int32_t linkAge);
    static void ClearAnchorModelOverride();
    static void* TryLoadAnchorOverride(const char* path);
    static void* TryLoadAnchorTextureOverride(const char* path);
    static AnchorTextureOverrides ApplyAnchorFlipbookTextures(Player* player, const std::string& modelId,
                                                              int32_t linkAge);
    static void RestoreAnchorFlipbookTextures(const AnchorTextureOverrides& overrides, int32_t linkAge);

  private:
    AnchorModRegistry() = default;
};

#endif // __cplusplus

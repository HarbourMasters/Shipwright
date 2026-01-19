#pragma once
#ifdef __cplusplus

#include <cstdint>
#include <memory>
#include <string>

namespace Ship {
class Archive;
}

struct Player;

class AnchorModRegistry {
  public:
    static void Init();
    static const std::string& GetLocalModelId();
    static std::shared_ptr<Ship::Archive> FindArchiveById(const std::string& id);
    static bool ApplyModelToPlayer(const std::string& id, int32_t linkAge, Player* player);
    static bool HasCustomModel(const std::string& id, int32_t linkAge, int32_t expectedLimbCount);

  private:
    AnchorModRegistry() = default;
};

#endif // __cplusplus

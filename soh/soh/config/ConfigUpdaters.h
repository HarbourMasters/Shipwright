#include "libultraship/libultraship.h"

namespace SOH {
class ConfigVersion1Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion1Updater();
    void Update(Ship::Config* conf);
};

class ConfigVersion2Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion2Updater();
    void Update(Ship::Config* conf);
};

class ConfigVersion3Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion3Updater();
    void Update(Ship::Config* conf);
};
} // namespace SOH

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

class ConfigVersion4Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion4Updater();
    void Update(Ship::Config* conf);
};

class ConfigVersion5Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion5Updater();
    void Update(Ship::Config* conf);
};

class ConfigVersion6Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion6Updater();
    void Update(Ship::Config* conf);
};
} // namespace SOH

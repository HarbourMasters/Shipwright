#include <libultraship/libultraship.h>

namespace Ben {

class ConfigVersion1Updater final : public Ship::ConfigVersionUpdater {
  public:
    ConfigVersion1Updater();
    void Update(Ship::Config* conf) override;
};

} // namespace Ben

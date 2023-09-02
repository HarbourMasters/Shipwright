#include "libultraship/libultraship.h"

namespace LUS {
class ConfigVersion1Updater : public ConfigVersionUpdater {
    public:
    ConfigVersion1Updater();
    void Update(Config* conf);
};
}
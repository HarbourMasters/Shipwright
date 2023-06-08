#include "libultraship/libultraship.h"

namespace LUS {
class ConfigVersion0Updater : public ConfigVersionUpdater {
    public:
    ConfigVersion0Updater();
    void Update(Config* conf);
};
}
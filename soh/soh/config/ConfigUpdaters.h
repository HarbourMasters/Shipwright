#include "libultraship/libultraship.h"

namespace SOH {
    class ConfigVersion1Updater : public Ship::ConfigVersionUpdater {
        public:
        ConfigVersion1Updater();
        void Update(Ship::Config* conf);
    };

    class ConfigVersion2Updater : public Ship::ConfigVersionUpdater {
        public:
        ConfigVersion2Updater();
        void Update(Ship::Config* conf);
    };
}
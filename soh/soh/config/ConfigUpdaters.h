#include "libultraship/libultraship.h"

namespace SOH {
    class ConfigVersion1Updater : public ShipDK::ConfigVersionUpdater {
        public:
        ConfigVersion1Updater();
        void Update(ShipDK::Config* conf);
    };

    class ConfigVersion2Updater : public ShipDK::ConfigVersionUpdater {
        public:
        ConfigVersion2Updater();
        void Update(ShipDK::Config* conf);
    };
}
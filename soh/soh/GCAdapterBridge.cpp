#include "soh/ShipInit.hpp"
#include "public/bridge/controllerbridge.h"

static RegisterShipInitFunc gInitGCAdapter([]() { InitGCAdapter(); });
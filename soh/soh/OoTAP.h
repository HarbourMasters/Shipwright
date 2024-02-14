#pragma once

#include "Archipelago.h"

extern bool OoTAP_ENABLED;
extern bool OoTAP_CONNECTED;

extern void OoTAP_Enable(const char* ip, int port, const char* slotName, const char* password);
extern void OoTAP_Disable();
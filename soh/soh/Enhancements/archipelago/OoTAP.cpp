#include "OoTAP.h"
#include <APCpp/Archipelago.h>

void Archipelago::Enable(const char* ip, int port, const char* slotName, const char* password) {
    if (isEnabled) {
        return;
    }

    isEnabled = true;
    AP_Init(ip, "Ocarina of Time", slotName, password);
}

void Archipelago::Disable() {
    isEnabled = false;
    isConnected = false;
}
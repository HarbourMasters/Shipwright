#include "OoTAP.h"

bool OoTAP_ENABLED = false;
bool OoTAP_CONNECTED = false;

void OoTAP_Enable(const char* ip, int port, const char* slotName, const char* password) {
    if (OoTAP_ENABLED) {
        return;
    }

    OoTAP_ENABLED = true;
    AP_Init(ip, "Ocarina of Time", slotName, password);
}

void OoTAP_Disable() {
    OoTAP_ENABLED = false;
    OoTAP_CONNECTED = false;
}
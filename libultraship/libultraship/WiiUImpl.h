#pragma once

#include <vpad/input.h>
#include <padscore/kpad.h>

namespace Ship {
namespace WiiU {

void Init();

void Exit();

void ThrowMissingOTR(const char* otrPath);

void ThrowInvalidOTR();

void Update();

VPADStatus *GetVPADStatus(VPADReadError *error);

KPADStatus *GetKPADStatus(WPADChan chan, KPADError *error);

};
};

#include "debugger.h"
#include "debugSaveEditor.h"
#include "colViewer.h"

extern "C" {

void Debug_Init(void) {
    InitSaveEditor();
    InitColViewer();
}

void Debug_Draw(void) {
    DrawColViewer();
}

}

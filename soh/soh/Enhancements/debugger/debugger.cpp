#include "debugger.h"
#include "debugSaveEditor.h"
#include "colViewer.h"
#include "actorViewer.h"

extern "C" {

void Debug_Init(void) {
    InitSaveEditor();
    InitColViewer();
    InitActorViewer();
}

void Debug_Draw(void) {
    DrawColViewer();
}

}

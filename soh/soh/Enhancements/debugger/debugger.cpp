#include "debugger.h"
#include "debugSaveEditor.h"
#include "colViewer.h"
#include "actorViewer.h"
#include "dlViewer.h"

extern "C" {

void Debug_Init(void) {
    InitSaveEditor();
    InitColViewer();
    InitActorViewer();
    InitDLViewer();
}

void Debug_Draw(void) {
    DrawColViewer();
}

}

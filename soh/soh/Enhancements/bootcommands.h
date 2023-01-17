#ifndef _BOOTCOMMANDS_H_
#define _BOOTCOMMANDS_H_
#include <libultraship/libultra.h>
#include <z64.h>

typedef s32 (*BootCommandFunc)(char** argv, s32 argc); // Returns the number of arguments it read

typedef struct BootCommand
{
    char* name;
    BootCommandFunc func;
} BootCommand;

extern uint8_t gLoadFileSelect;
extern uint8_t gSkipLogoTest;

void BootCommands_Init();
//void BootCommands_ParseBootArgs(char* str);
void BootCommands_ParseBootArgs(s32 argc, char** argv);

#endif
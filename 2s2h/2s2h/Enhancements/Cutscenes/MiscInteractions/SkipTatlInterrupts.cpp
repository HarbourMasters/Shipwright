#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_Elf_Msg/z_elf_msg.h"
#include "overlays/actors/ovl_Elf_Msg3/z_elf_msg3.h"
#include "overlays/actors/ovl_Elf_Msg4/z_elf_msg4.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipTatlInterrupts() {
    // First time entering Clock Town Interupt
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVAR, {
        if (gSaveContext.save.entrance == ENTRANCE(SOUTH_CLOCK_TOWN, 0) && gSaveContext.save.cutsceneIndex == 0 &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_59_04)) {
            Flags_SetWeekEventReg(WEEKEVENTREG_59_04);
            Sram_SaveSpecialEnterClockTown(gPlayState);
        }
    });

    // General Interupt
    COND_VB_SHOULD(VB_TATL_INTERRUPT_MSG, CVAR, {
        if (*should) {
            Actor* actor = va_arg(args, Actor*);
            *should = false;
            if (ELFMSG_GET_SWITCH_FLAG(actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, ELFMSG_GET_SWITCH_FLAG(actor));
            }
            Actor_Kill(actor);
        }
    });

    // General interupt (3)
    COND_VB_SHOULD(VB_TATL_INTERRUPT_MSG3, CVAR, {
        if (*should) {
            Actor* actor = va_arg(args, Actor*);
            *should = false;
            if (ELFMSG3_GET_SWITCH_FLAG(actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, ELFMSG3_GET_SWITCH_FLAG(actor));
            }
            Actor_Kill(actor);
        }
    });

    // General interupt (4)
    COND_VB_SHOULD(VB_TATL_INTERRUPT_MSG4, CVAR, {
        if (*should) {
            Actor* actor = va_arg(args, Actor*);
            *should = false;
            if (ELFMSG4_GET_SWITCH_FLAG(actor) != 0x7F) {
                Flags_SetSwitch(gPlayState, ELFMSG4_GET_SWITCH_FLAG(actor));
            }
            Actor_Kill(actor);
        }
    });

    // General interupt (6) (the flags were directly copied from the original code)
    COND_VB_SHOULD(VB_TATL_INTERRUPT_MSG6, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        if (*should && actor->csId != -1 && actor->csId != 124) {
            *should = false;
            switch (actor->textId) {
                case 0x224:
                    SET_WEEKEVENTREG(WEEKEVENTREG_79_10);
                    break;

                case 0x25B:
                    SET_WEEKEVENTREG(WEEKEVENTREG_88_20);
                    break;

                case 0x216:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);
                    break;

                case 0x231:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_01);
                    break;

                case 0x232:
                    SET_WEEKEVENTREG(WEEKEVENTREG_31_02);
                    break;

                case 0x233:
                    SET_WEEKEVENTREG(WEEKEVENTREG_80_04);
                    break;
            }
            if (CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_10);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_20);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_40);
            }

            if (CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_87_80);
            }
            Actor_Kill(actor);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipTatlInterrupts, { CVAR_NAME });

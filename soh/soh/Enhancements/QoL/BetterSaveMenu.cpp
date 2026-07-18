#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern PlayState* gPlayState;
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

#define CVAR_BETTERSAVE CVAR_ENHANCEMENT("BetterSaveMenu")
#define CVAR_BETTERSAVE_DEFAULT 0
#define CVAR_BETTERSAVE_VALUE CVarGetInteger(CVAR_BETTERSAVE, CVAR_BETTERSAVE_DEFAULT)
static CustomMessage saveMsg = CustomMessage("\x08Would you like to save?&&" + CustomMessage::TWO_WAY_CHOICE() + "%gYes&No%w\x09", TEXTBOX_TYPE_BLUE);
static CustomMessage continueMsg = CustomMessage("\x08 Continue?&" + CustomMessage::THREE_WAY_CHOICE() + "%gContinue&Restart&Return to Spawn%w\x09", TEXTBOX_TYPE_BLUE);

void HandleSaveMenu(bool* should, PlayState* play) {
  PauseContext* pauseCtx = &play->pauseCtx;
  switch (pauseCtx->unk_1EC) {
    case 0:
      *should = false;
      Message_StartTextbox(play, TEXT_SAVE_MSG, NULL);
      pauseCtx->unk_1EC = 1;
      break;
    case 1:
      *should = false;
      if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex == 0) {
          Audio_PlaySoundGeneral(NA_SE_SY_PIECE_OF_HEART, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
          Play_PerformSave(play);
          pauseCtx->unk_1EC = 4;
          Message_StartTextbox(play, TEXT_CONTINUE_MSG, NULL);
        } else {
          Interface_SetDoAction(play, DO_ACTION_NONE);
          gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
            gSaveContext.buttonStatus[3] = BTN_ENABLED;
          gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
            gSaveContext.buttonStatus[8] = BTN_ENABLED;
          gSaveContext.hudVisibilityMode = 0;
          Interface_ChangeHudVisibilityMode(50);
          pauseCtx->unk_1EC = 2;
          WREG(2) = -6240;
          YREG(8) = pauseCtx->unk_204;
          func_800F64E0(0);
        }
      }
      break;
    case 4:
      *should = false;
      if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        Interface_SetDoAction(play, DO_ACTION_NONE);
        gSaveContext.buttonStatus[0] = gSaveContext.buttonStatus[1] = gSaveContext.buttonStatus[2] =
          gSaveContext.buttonStatus[3] = BTN_ENABLED;
        gSaveContext.buttonStatus[5] = gSaveContext.buttonStatus[6] = gSaveContext.buttonStatus[7] =
          gSaveContext.buttonStatus[8] = BTN_ENABLED;
        gSaveContext.hudVisibilityMode = 0;
        Interface_ChangeHudVisibilityMode(50);
        pauseCtx->unk_1EC = 5;
        WREG(2) = -6240;
        YREG(8) = pauseCtx->unk_204;
        func_800F64E0(0);
      }
      break;
    default:
      *should = true;
  }
  Message_Update(play);
}

void RegisterBetterSave() {
  saveMsg.Format();
  continueMsg.Format();
  REGISTER_VB_SHOULD(VB_LOAD_SAVE_MENU, {
      PlayState* play = va_arg(args, PlayState*);
      HandleSaveMenu(should, play);
  });

  REGISTER_VB_SHOULD(VB_DRAW_SAVE_MENU, {
      *should = false;
  });

  COND_ID_HOOK(OnOpenText, TEXT_SAVE_MSG, true, [](uint16_t* textId, bool* loadFromMessageTable){
      saveMsg.LoadIntoFont();
      *loadFromMessageTable = false;
      return;
  });

  COND_ID_HOOK(OnOpenText, TEXT_CONTINUE_MSG, true, [](uint16_t*textId, bool* loadFromMessageTable) {
      continueMsg.LoadIntoFont();
      *loadFromMessageTable = false;
      return;
  });
}

static RegisterShipInitFunc initFunc(RegisterBetterSave, { CVAR_BETTERSAVE });

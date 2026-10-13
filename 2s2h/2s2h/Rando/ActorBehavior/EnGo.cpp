#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/Rando/StaticData/StaticData.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Go/z_en_go.h"

void Player_StartTalking(PlayState* play, Actor* actor);
void Player_SetupTalk(PlayState* play, Player* player);
s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc afterPutAwayFunc);
}

static std::vector<u8> skipCmds = {};
static bool freePowderKegGrantActive = false;

void Rando::ActorBehavior::InitEnGoBehavior() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_GO, IS_RANDO, [](Actor* actor) { skipCmds.clear(); });

    // Medigoron - Scripted Actors
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, IS_RANDO, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);

        if (actor->id != ACTOR_EN_GO || ENGO_GET_TYPE(actor) != ENGO_MEDIGORON) {
            return;
        }

        if (!skipCmds.empty() && cmdId == skipCmds.at(0)) {
            skipCmds.erase(skipCmds.begin());
            *should = false;

            return;
        }

        MsgScript* script = va_arg(args, MsgScript*);
        Player* player = GET_PLAYER(gPlayState);

        if (cmdId == MSCRIPT_CMD_ID_CHECK_ITEM) {
            MsgScriptCmdCheckItem* cmd = (MsgScriptCmdCheckItem*)script;
            s16 skip = SCRIPT_PACK_16(cmd->offsetH, cmd->offsetL);

            s16 jumpTarget = 0x009A;
            if (!RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_MEDIGORON].cycleObtained) {
                jumpTarget = 0x008C;
            }

            s16 offsetAdjustment = (skip == 0x009A - 0x002B || skip == 0x008C - 0x002B) ? 0x002B : 0x0041;
            s16 skipOffset = jumpTarget - offsetAdjustment;

            script[3] = skipOffset >> 8;
            script[4] = skipOffset & 0xFF;
            return;
        }

        if (cmdId == MSCRIPT_CMD_ID_BEGIN_TEXT || cmdId == MSCRIPT_CMD_ID_CONTINUE_TEXT) {

            // Could also be MsgScriptCmdContinueText, but structs are essentially identical
            MsgScriptCmdBeginText* cmd = (MsgScriptCmdBeginText*)script;
            u16 textId = SCRIPT_PACK_16(cmd->textIdH, cmd->textIdL);

            // Only override behavior if the player has yet to discover powder kegs
            if (textId != 0x0C8C || HAS_ITEM(ITEM_POWDER_KEG)) {
                return;
            }

            *should = false;
            gPlayState->msgCtx.choiceIndex = 0;

            skipCmds.clear();
            skipCmds.push_back(MSCRIPT_CMD_ID_AWAIT_TEXT);

            return;
        }

        // Identify text choice branch at 0x004E by skip offset values
        if (cmdId == MSCRIPT_CMD_ID_CHECK_TEXT_CHOICE) {
            MsgScriptCmdCheckTextChoice* cmd = (MsgScriptCmdCheckTextChoice*)script;
            s16 skipChoice1 = SCRIPT_PACK_16(cmd->offset0H, cmd->offset0L);
            s16 skipChoice2 = SCRIPT_PACK_16(cmd->offset1H, cmd->offset1L);
            s16 skipChoice3 = SCRIPT_PACK_16(cmd->offset2H, cmd->offset2L);

            switch (skipChoice1, skipChoice2, skipChoice3) {
                case (0x0, 0x00AD - 0x00A8, 0x0):
                    // Skip item grant and additional dialogue if player hasn't found a Keg yet
                    if (!HAS_ITEM(ITEM_POWDER_KEG)) {
                        s16 skipOffset = 0x00AE - 0x00A8;

                        script[1] = script[3] = script[5] = skipOffset >> 8;
                        script[2] = script[4] = script[6] = skipOffset & 0xFF;
                        break;
                    }

                    // Restore overwritten script default values
                    script[1] = script[2] = script[5] = script[6] = 0x0;
                    script[3] = (0x00AD - 0x00A8) >> 8;
                    script[4] = (0x00AD - 0x00A8) & 0xFF;
            }

            return;
        }

        if (cmdId == MSCRIPT_CMD_ID_OFFER_ITEM) {
            if (freePowderKegGrantActive) {
                *should = false;

                EnGo* enGo = (EnGo*)actor;
                Player* player = GET_PLAYER(gPlayState);
                player->talkActor = &enGo->actor;

                if (!RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_MEDIGORON].cycleObtained) {
                    RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_MEDIGORON].eligible = true;
                }

                s16 skipOffset = 0x00C3 - 0x00BD;
                script[3] = skipOffset >> 8;
                script[4] = skipOffset & 0xFF;
                return;
            }

            // Restore overwritten script default values
            s16 skipOffset = 0;
            script[3] = skipOffset >> 8;
            script[4] = skipOffset & 0xFF;
            return;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x0C81, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Audio_PlaySfx(NA_SE_EN_GOLON_WAKE_UP); // Original script plays this as part of the text.
        entry.msg = "Want a %rPowder Keg%w?\n";
        entry.msg += "The Goron Elder said I can't sell my\n";
        entry.msg += "Powder Kegs to anyone new,\n";
        entry.msg += "\x12";
        entry.msg += "but I can give you one to\n";
        entry.msg += "blow up the boulder near the track.\x19";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x0C83, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Audio_PlaySfx(NA_SE_EN_GOLON_WAKE_UP); // Original script plays this as part of the text.
        entry.msg = "If you can %rdestroy%w the boulder\n";
        entry.msg += "that blocks the entrance to the\n";
        entry.msg += "%rGoron Racetrack%w near here ...\n";
        entry.msg += "\x12";
        entry.msg += "using the %rPowder Keg%w I'm about\n";
        entry.msg += "to give you, then I'll give you\n";
        entry.msg += "%g{randoItem}%w.";
        entry.msg += "\x19";

        CustomMessage::Replace(&entry.msg, "{randoItem}",
                               Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_GORON_VILLAGE_MEDIGORON].randoItemId,
                                                              true, RC_GORON_VILLAGE_MEDIGORON));
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x0C86, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        freePowderKegGrantActive = true;

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        Audio_PlaySfx(NA_SE_EN_GOLON_VOICE_EATFULL); // Original script plays this as part of the text.
        entry.msg = "I heard you cleared the boulder\n";
        entry.msg += "near the racetrack! Here's the\n";
        entry.msg += "reward the Elder set aside for \n";
        entry.msg += "anyone who could do it.";

        CustomMessage::EnsureMessageEnd(&entry.msg);
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x0C87, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (freePowderKegGrantActive) {
            freePowderKegGrantActive = false;

            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            Audio_PlaySfx(NA_SE_EN_GOLON_VOICE_GENERAL); // Original script plays this as part of the text.
            entry.msg = "Come back if you're interested in\n";
            entry.msg += "my Powder Kegs.";

            CustomMessage::EnsureMessageEnd(&entry.msg);
            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        }
    });

    COND_ID_HOOK(OnOpenText, 0x0C8D, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        if (!HAS_ITEM(ITEM_POWDER_KEG)) {
            auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
            Audio_PlaySfx(NA_SE_EN_GOLON_VOICE_GENERAL); // Original script plays this as part of the text.
            entry.msg = "Sorry, but the Goron Elder revoked\n";
            entry.msg += "my ability to certify you to carry\n";
            entry.msg += "my Powder Kegs. You'll have to find\n";
            entry.msg += "one somewhere else first, goro.";

            CustomMessage::EnsureMessageEnd(&entry.msg);
            CustomMessage::LoadCustomMessageIntoFont(entry);
            *loadFromMessageTable = false;
        }
    });
}

#include "OTRGlobals.h"
#include <unordered_map>
#include <libultraship/ResourceMgr.h>
#include <libultraship/Scene.h>
#include <Utils/StringHelper.h>
#include "global.h"
#include "vt.h"
#include <libultraship/Text.h>
#include <message_data_static.h>
#include "Enhancements/custom-message/CustomMessageManager.h"
#include "Enhancements/custom-message/CustomMessageTypes.h"

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sStaffMessageEntryTablePtr;
//extern "C" MessageTableEntry* _message_0xFFFC_nes;	

MessageTableEntry* OTRMessage_LoadTable(const char* filePath, bool isNES) {
    auto file = std::static_pointer_cast<Ship::Text>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;
    
    // Allocate room for an additional message
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * (file->messages.size() + 1));

    for (int i = 0; i < file->messages.size(); i++) {
        // Look for Owl Text
        if (file->messages[i].id == 0x2066) {
            // Create a new message based on the Owl Text
            char* kaeporaPatch = (char*)malloc(sizeof(char) * file->messages[i].msg.size());
            file->messages[i].msg.copy(kaeporaPatch, file->messages[i].msg.size(), 0);

            // Swap the order of yes and no in this new message
            if (filePath == "text/nes_message_data_static/nes_message_data_static") {
                kaeporaPatch[26] = 'Y';
                kaeporaPatch[27] = 'e';
                kaeporaPatch[28] = 's';
                kaeporaPatch[29] = 1;
                kaeporaPatch[30] = 'N';
                kaeporaPatch[31] = 'o';
            } else if (filePath == "text/ger_message_data_static/ger_message_data_static") {
                kaeporaPatch[30] = 'J';
                kaeporaPatch[31] = 'a';
                kaeporaPatch[32] = '!';
                kaeporaPatch[33] = 1;
                kaeporaPatch[34] = 'N';
                kaeporaPatch[35] = 'e';
                kaeporaPatch[36] = 'i';
                kaeporaPatch[37] = 'n';
            } else {
                kaeporaPatch[26] = 'O';
                kaeporaPatch[27] = 'u';
                kaeporaPatch[28] = 'i';
                kaeporaPatch[29] = 1;
                kaeporaPatch[30] = 'N';
                kaeporaPatch[31] = 'o';
                kaeporaPatch[32] = 'n';
            }

            // load data into message
            table[file->messages.size()].textId = 0x71B3;
            table[file->messages.size()].typePos = (file->messages[i].textboxType << 4) | file->messages[i].textboxYPos;
            table[file->messages.size()].segment = kaeporaPatch;
            table[file->messages.size()].msgSize = file->messages[i].msg.size();
        }

        table[i].textId = file->messages[i].id;
        table[i].typePos = (file->messages[i].textboxType << 4) | file->messages[i].textboxYPos;
        table[i].segment = file->messages[i].msg.c_str();
        table[i].msgSize = file->messages[i].msg.size();

        if (isNES && file->messages[i].id == 0xFFFC)
            _message_0xFFFC_nes = (char*)file->messages[i].msg.c_str();
    }

	return table;
}

static std::unordered_map<u16, std::string> sAccessibilityText;
static std::string sTextInterpolated;
extern "C" void OTRMessage_InitAccessibilityText()
{
    auto file = std::static_pointer_cast<Ship::Text>(
        OTRGlobals::Instance->context->GetResourceManager()->LoadResource("text/accessibility_text/accessibility_text_eng"));

    for (size_t i = 0; i < file->messages.size(); i++) {
        sAccessibilityText[file->messages[i].id] = file->messages[i].msg;
    }
}

extern "C" const char* OTRMessage_GetAccessibilityText(const char* textResourcePath, u16 textId, const char* arg) {
    auto it = sAccessibilityText.find(textId);
 	if (it == sAccessibilityText.end()) {
        return nullptr;
	}

	if (arg != nullptr) {
        sTextInterpolated = it->second;
        std::string searchString = "$0";
        size_t index = sTextInterpolated.find(searchString);
		if (index != std::string::npos) {
            sTextInterpolated.replace(index, searchString.size(), std::string(arg));
            return strdup(sTextInterpolated.c_str());
		}
	}

	return strdup(it->second.c_str());
}

extern "C" void OTRMessage_Init()
{
    sNesMessageEntryTablePtr = OTRMessage_LoadTable("text/nes_message_data_static/nes_message_data_static", true);
    sGerMessageEntryTablePtr = OTRMessage_LoadTable("text/ger_message_data_static/ger_message_data_static", false);
    sFraMessageEntryTablePtr = OTRMessage_LoadTable("text/fra_message_data_static/fra_message_data_static", false);

	auto file2 = std::static_pointer_cast<Ship::Text>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource("text/staff_message_data_static/staff_message_data_static"));

	sStaffMessageEntryTablePtr = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * file2->messages.size());

	for (int i = 0; i < file2->messages.size(); i++)
	{
		sStaffMessageEntryTablePtr[i].textId = file2->messages[i].id;
		sStaffMessageEntryTablePtr[i].typePos = (file2->messages[i].textboxType << 4) | file2->messages[i].textboxYPos;
		sStaffMessageEntryTablePtr[i].segment = file2->messages[i].msg.c_str();
		sStaffMessageEntryTablePtr[i].msgSize = file2->messages[i].msg.size();
	}

    CustomMessageManager::Instance->AddCustomMessageTable(customMessageTableID);
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_GS_NO_FREEZE, ITEM_SKULL_TOKEN,
        { 
            TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
            "You got a %rGold Skulltula Token%w!&You've collected %r{{gsCount}}%w tokens&in total!\x0E\x3C",
            "Du erhälst ein %rGoldene&Skulltula-Symbol%w! Du hast&insgesamt %r{{gsCount}}%w symbol gesammelt!\x0E\x3C",
            "Vous obtenez un %rSymbole de&Skulltula d'or%w! Vous avez&collecté %r{{gsCount}}%w symboles en tout!\x0E\x3C"
        }
    );
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_GS_FREEZE, ITEM_SKULL_TOKEN,
        { 
          TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
          "You got a %rGold Skulltula Token%w!&You've collected %r{{gsCount}}%w tokens&in total!",
          "Du erhälst ein %rGoldene&Skulltula-Symbol%w! Du hast&insgesamt %r{{gsCount}}%w symbol gesammelt!",
          "Vous obtenez un %rSymbole de&Skulltula d'or%w! Vous avez&collecté %r{{gsCount}}%w symboles en tout!"
        }
    );
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BUY_BOMBCHU_10_DESC,
        {
          TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
          "\x08%rBombchu  (10 pieces)  99 Rupees&%wThis looks like a toy mouse, but&it's actually a self-propelled time&bomb!\x09\x0A",
          "\x08%rKrabbelmine  10 Stück  99 Rubine&%wDas ist eine praktische Zeitbombe,&die Du als Distanzwaffe&einsetzen kannst!\x09\x0A",
          "\x08%rMissile  10 unités  99 Rubis&%wProfilée comme une souris&mécanique, cette arme est &destructrice!!!\x09\x0A",
        }
    );
    CustomMessageManager::Instance->CreateMessage(
        customMessageTableID, TEXT_BUY_BOMBCHU_10_PROMPT,
        {
          TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
          "\x08" "Bombchu  10 pieces   99 Rupees\x09&&\x1B%gBuy&Don't buy%w",
          "\x08Krabbelmine  10 Stück  99 Rubine\x09&&\x1B%gKaufen!&Nicht kaufen!%w",
          "\x08Missiles  10 unités   99 Rubis\x09&&\x1B%gAcheter&Ne pas acheter%w",
        }
    );

    OTRMessage_InitAccessibilityText();
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_HEART_CONTAINER, ITEM_HEART_CONTAINER,
        {
            TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
            "You got a %rHeart Container%w!&You've collected %r{{heartContainerCount}}%w containers&in total!",
            "Du erhältst ein %rHerzgefäß%w! Du&hast insgesamt %r{{heartContainerCount}}%w Gefäße&gesammelt!",
            "Vous obtenez un %rRécipient de&coeur%w! Vous avez&collecté %r{{heartContainerCount}}%w récipients en tout!"
        }
    );
    CustomMessageManager::Instance->CreateGetItemMessage(
        customMessageTableID, (GetItemID)TEXT_HEART_PIECE, ITEM_HEART_PIECE,
        {
            TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM,
            "You got a %rHeart Piece%w!&You've collected %r{{heartPieceCount}}%w pieces&in total!",
            "Du erhältst ein %rHerzteil%w! Du hast&insgesamt %r{{heartPieceCount}}%w Teile&gesammelt!",
            "Vous obtenez un %rMorceau de&coeur%w! Vous avez&collecté %r{{heartPieceCount}}%w morceaux en tout!"
        }
    );
}

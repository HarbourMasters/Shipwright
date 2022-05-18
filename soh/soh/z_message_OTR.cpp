#include "OTRGlobals.h"
#include "ResourceMgr.h"
#include "Scene.h"
#include "message_data_static.h"
#include "Utils/StringHelper.h"
#include "global.h"
#include "vt.h"
#include <Text.h>

extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sStaffMessageEntryTablePtr;
//extern "C" MessageTableEntry* _message_0xFFFC_nes;	

MessageTableEntry* OTRMessage_LoadTable(const char* filePath, bool isNES) {
    auto file = std::static_pointer_cast<Ship::Text>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(filePath));

    if (file == nullptr)
        return nullptr;
    
    MessageTableEntry* table = (MessageTableEntry*)malloc(sizeof(MessageTableEntry) * (file->messages.size() + 1));

    for (int i = 0; i < file->messages.size(); i++) {
        if (file->messages[i].id == 0x2066) {
            char* kaeporaPatch = (char*)malloc(sizeof(char) * file->messages[i].msg.size());
            file->messages[i].msg.copy(kaeporaPatch, file->messages[i].msg.size(), 0);

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
}
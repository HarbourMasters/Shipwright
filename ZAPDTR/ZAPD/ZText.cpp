#include "ZText.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Text, ZText);

ZText::ZText(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("CodeOffset");
	RegisterOptionalAttribute("LangOffset", "0");
}

void ZText::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	uint32_t currentPtr = StringHelper::StrToL(registeredAttributes.at("CodeOffset").value, 16);
	uint32_t langPtr = currentPtr;
	bool isPalLang = false;

	if (StringHelper::StrToL(registeredAttributes.at("LangOffset").value, 16) != 0)
	{
		langPtr = StringHelper::StrToL(registeredAttributes.at("LangOffset").value, 16);

		if (langPtr != currentPtr)
			isPalLang = true;
	}

	std::vector<uint8_t> codeData;

	if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
		codeData = Globals::Instance->GetBaseromFile("code");
	else
		codeData = Globals::Instance->GetBaseromFile(Globals::Instance->baseRomPath.string() + "code");

	while (true)
	{
		MessageEntry msgEntry;
		msgEntry.id = BitConverter::ToInt16BE(codeData, currentPtr + 0);
		msgEntry.textboxType = (codeData[currentPtr + 2] & 0xF0) >> 4;
		msgEntry.textboxYPos = (codeData[currentPtr + 2] & 0x0F);

		if (isPalLang)
		{
			msgEntry.segmentId = (codeData[langPtr + 0]);
			msgEntry.msgOffset = BitConverter::ToInt32BE(codeData, langPtr + 0) & 0x00FFFFFF;
		}
		else
		{
			msgEntry.segmentId = (codeData[langPtr + 4]);
			msgEntry.msgOffset = BitConverter::ToInt32BE(codeData, langPtr + 4) & 0x00FFFFFF;
		}

		uint32_t msgPtr = msgEntry.msgOffset;

		unsigned char c = rawData[msgPtr];
		unsigned int extra = 0;
		bool stop = false;

		while ((c != '\0' && !stop) || extra > 0)
		{
			msgEntry.msg += c;
			msgPtr++;

			if (extra == 0)
			{
				if (c == 0x05 || c == 0x13 || c == 0x0E || c == 0x0C || c == 0x1E || c == 0x06 ||
				    c == 0x14)
				{
					extra = 1;
				}
				else if (c == 0x07)
				{
					extra = 2;
					stop = true;
				}
				else if (c == 0x12 || c == 0x11)
				{
					extra = 2;
				}
				else if (c == 0x15)
				{
					extra = 3;
				}
			}
			else
			{
				extra--;
			}

			c = rawData[msgPtr];
		}

		messages.push_back(msgEntry);

		if (msgEntry.id == 0xFFFC || msgEntry.id == 0xFFFF)
			break;

		currentPtr += 8;

		if (isPalLang)
			langPtr += 4;
		else
			langPtr += 8;
	}

	int bp2 = 0;
}

std::string ZText::GetSourceTypeName() const
{
	return "u8";
}

size_t ZText::GetRawDataSize() const
{
	return 1;
}

ZResourceType ZText::GetResourceType() const
{
	return ZResourceType::Text;
}

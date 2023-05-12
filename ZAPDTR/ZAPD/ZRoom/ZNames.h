#pragma once

#include <string>

#include "Globals.h"
#include "Utils/StringHelper.h"

class ZNames
{
public:
	static std::string GetObjectName(size_t id)
	{
		if (id >= Globals::Instance->cfg.objectList.size())
			return StringHelper::Sprintf("0x%04X", id);
		return Globals::Instance->cfg.objectList[id];
	}

	static std::string GetActorName(uint16_t id)
	{
		switch (Globals::Instance->game)
		{
		case ZGame::OOT_RETAIL:
		case ZGame::OOT_SW97:
			if (id < ZNames::GetNumActors())
				return Globals::Instance->cfg.actorList[id];
			else
				return StringHelper::Sprintf("0x%04X", id);
		case ZGame::MM_RETAIL:
		{
			int32_t flags = id & 0xF000;
			id &= 0xFFF;
			std::string name;
			if (id < ZNames::GetNumActors())
				name = Globals::Instance->cfg.actorList[id];
			else
				name = StringHelper::Sprintf("0x%04X", id);

			if (flags == 0)
				return name;
			else
				return StringHelper::Sprintf("%s | 0x%04X", name.c_str(), flags);
		}
		}

		return "";
	}

	static std::string GetEntranceName(uint16_t id)
	{
		if (ZNames::GetNumEntrances() == 0 || ZNames::GetNumSpecialEntrances() == 0)
			return StringHelper::Sprintf("0x%04X", id);	
			
		if (id < ZNames::GetNumEntrances())
			return Globals::Instance->cfg.entranceList[id];
		else if ((id >= 0x7FF9 && id <= 0x7FFF) && !((id - 0x7FF9U) > GetNumSpecialEntrances()))  // Special entrances
			return Globals::Instance->cfg.specialEntranceList[id - 0x7FF9];
		else
			return StringHelper::Sprintf("0x%04X", id);
	}

	static size_t GetNumActors() { return Globals::Instance->cfg.actorList.size(); }
	static size_t GetNumEntrances() { return Globals::Instance->cfg.entranceList.size(); }
	static size_t GetNumSpecialEntrances() { return Globals::Instance->cfg.specialEntranceList.size(); }
};

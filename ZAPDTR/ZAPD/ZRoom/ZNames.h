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
		return Globals::Instance->cfg.objectList.at(id);
	}

	static std::string GetActorName(int32_t id)
	{
		switch (Globals::Instance->game)
		{
		case ZGame::OOT_RETAIL:
		case ZGame::OOT_SW97:
			if (id < ZNames::GetNumActors())
				return Globals::Instance->cfg.actorList.at(id);
			else
				return StringHelper::Sprintf("0x%04X", id);
		case ZGame::MM_RETAIL:
		{
			int32_t flags = id & 0xF000;
			id &= 0xFFF;
			std::string name;
			if (id < ZNames::GetNumActors())
				name = Globals::Instance->cfg.actorList.at(id);
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

	static int32_t GetNumActors() { return Globals::Instance->cfg.actorList.size(); }
};

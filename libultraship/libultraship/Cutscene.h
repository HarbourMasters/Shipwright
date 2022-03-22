#pragma once

#include "Resource.h"

namespace Ship
{
	class CutsceneV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res) override;
	};

	class CutsceneCommand
	{
	public:
		uint32_t commandID;
		uint32_t commandIndex;

		CutsceneCommand() {};
	};

	class Cutscene : public Resource
	{
	public:
		//int32_t endFrame;
		std::vector<uint32_t> commands;
	};
}
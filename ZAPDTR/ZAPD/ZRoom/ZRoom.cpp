#include "ZRoom.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <string_view>

#include "Commands/EndMarker.h"
#include "Commands/SetActorCutsceneList.h"
#include "Commands/SetActorList.h"
#include "Commands/SetAlternateHeaders.h"
#include "Commands/SetAnimatedMaterialList.h"
#include "Commands/SetCameraSettings.h"
#include "Commands/SetCollisionHeader.h"
#include "Commands/SetCsCamera.h"
#include "Commands/SetCutscenes.h"
#include "Commands/SetEchoSettings.h"
#include "Commands/SetEntranceList.h"
#include "Commands/SetExitList.h"
#include "Commands/SetLightList.h"
#include "Commands/SetLightingSettings.h"
#include "Commands/SetMesh.h"
#include "Commands/SetMinimapChests.h"
#include "Commands/SetMinimapList.h"
#include "Commands/SetObjectList.h"
#include "Commands/SetPathways.h"
#include "Commands/SetRoomBehavior.h"
#include "Commands/SetRoomList.h"
#include "Commands/SetSkyboxModifier.h"
#include "Commands/SetSkyboxSettings.h"
#include "Commands/SetSoundSettings.h"
#include "Commands/SetSpecialObjects.h"
#include "Commands/SetStartPositionList.h"
#include "Commands/SetTimeSettings.h"
#include "Commands/SetTransitionActorList.h"
#include "Commands/SetWind.h"
#include "Commands/SetWorldMapVisited.h"
#include "Commands/Unused09.h"
#include "Commands/Unused1D.h"
#include "Commands/ZRoomCommandUnk.h"
#include "Globals.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZBlob.h"
#include "ZCutscene.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Room, ZRoom);
REGISTER_ZFILENODE(Scene, ZRoom);
REGISTER_ZFILENODE(AltHeader, ZRoom);

ZRoom::ZRoom(ZFile* nParent) : ZResource(nParent)
{
	roomCount = -1;
	canHaveInner = true;
	RegisterOptionalAttribute("HackMode");
}

ZRoom::~ZRoom()
{
	for (ZRoomCommand* cmd : commands)
		delete cmd;
}

void ZRoom::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawDataIndex);

	if (hackMode == "syotes_room")
	{
		SyotesRoomHack();
	}
	else
	{
		DeclareVar(name, "");
	}
}

void ZRoom::ExtractFromBinary(uint32_t nRawDataIndex, ZResourceType parentType)
{
	rawDataIndex = nRawDataIndex;
	name = GetDefaultName(parent->GetName());

	zroomType = ZResourceType::AltHeader;
	switch (parentType)
	{
	case ZResourceType::Scene:
	case ZResourceType::Room:
	case ZResourceType::AltHeader:
		parentZroomType = parentType;
		break;

	default:
		// TODO: error message or something
		assert(false);
		break;
	}

	ParseRawData();
	DeclareVar(name, "");
}

void ZRoom::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	// TODO: HACK: remove this specific check when the repo uses the correct HackMode="syotes_room"
	if (name == "syotes_room_0")
	{
		hackMode = "syotes_room";
	}

	std::string nodeName = std::string(reader->Name());
	if (nodeName == "Scene")
	{
		zroomType = ZResourceType::Scene;
	}
	else if (nodeName == "Room")
		zroomType = ZResourceType::Room;
	else if (nodeName == "AltHeader")
		zroomType = ZResourceType::AltHeader;

	if (reader->Attribute("HackMode") != nullptr)
	{
		hackMode = std::string(reader->Attribute("HackMode"));
		if (hackMode != "syotes_room")
		{
			std::string headerError = StringHelper::Sprintf(
				"invalid value found for 'HackMode' attribute: '%s'", hackMode.c_str());
			HANDLE_ERROR_RESOURCE(WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
			                      headerError, "");
		}
	}
}

void ZRoom::ParseRawData()
{
	if (hackMode == "syotes_room")
		return;

	bool shouldContinue = true;
	uint32_t currentIndex = 0;
	uint32_t currentPtr = rawDataIndex;

	const auto& rawData = parent->GetRawData();
	while (shouldContinue)
	{
		RoomCommand opcode = static_cast<RoomCommand>(rawData.at(currentPtr));

		ZRoomCommand* cmd = nullptr;

		auto start = std::chrono::steady_clock::now();

		switch (opcode)
		{
		case RoomCommand::SetStartPositionList:
			cmd = new SetStartPositionList(parent);
			break;  // 0x00
		case RoomCommand::SetActorList:
			cmd = new SetActorList(parent);
			break;  // 0x01
		case RoomCommand::SetCsCamera:
			cmd = new SetCsCamera(parent);
			break;  // 0x02 (MM-ONLY)
		case RoomCommand::SetCollisionHeader:
			cmd = new SetCollisionHeader(parent);
			break;  // 0x03
		case RoomCommand::SetRoomList:
			cmd = new SetRoomList(parent);
			break;  // 0x04
		case RoomCommand::SetWind:
			cmd = new SetWind(parent);
			break;  // 0x05
		case RoomCommand::SetEntranceList:
			cmd = new SetEntranceList(parent);
			break;  // 0x06
		case RoomCommand::SetSpecialObjects:
			cmd = new SetSpecialObjects(parent);
			break;  // 0x07
		case RoomCommand::SetRoomBehavior:
			cmd = new SetRoomBehavior(parent);
			break;  // 0x08
		case RoomCommand::Unused09:
			cmd = new Unused09(parent);
			break;  // 0x09
		case RoomCommand::SetMesh:
			cmd = new SetMesh(parent);
			break;  // 0x0A
		case RoomCommand::SetObjectList:
			cmd = new SetObjectList(parent);
			break;  // 0x0B
		case RoomCommand::SetLightList:
			cmd = new SetLightList(parent);
			break;  // 0x0C (MM-ONLY)
		case RoomCommand::SetPathways:
			cmd = new SetPathways(parent);
			break;  // 0x0D
		case RoomCommand::SetTransitionActorList:
			cmd = new SetTransitionActorList(parent);
			break;  // 0x0E
		case RoomCommand::SetLightingSettings:
			cmd = new SetLightingSettings(parent);
			break;  // 0x0F
		case RoomCommand::SetTimeSettings:
			cmd = new SetTimeSettings(parent);
			break;  // 0x10
		case RoomCommand::SetSkyboxSettings:
			cmd = new SetSkyboxSettings(parent);
			break;  // 0x11
		case RoomCommand::SetSkyboxModifier:
			cmd = new SetSkyboxModifier(parent);
			break;  // 0x12
		case RoomCommand::SetExitList:
			cmd = new SetExitList(parent);
			break;  // 0x13
		case RoomCommand::EndMarker:
			cmd = new EndMarker(parent);
			break;  // 0x14
		case RoomCommand::SetSoundSettings:
			cmd = new SetSoundSettings(parent);
			break;  // 0x15
		case RoomCommand::SetEchoSettings:
			cmd = new SetEchoSettings(parent);
			break;  // 0x16
		case RoomCommand::SetCutscenes:
			cmd = new SetCutscenes(parent);
			break;  // 0x17
		case RoomCommand::SetAlternateHeaders:
			cmd = new SetAlternateHeaders(parent);
			break;  // 0x18
		case RoomCommand::SetCameraSettings:
			if (Globals::Instance->game == ZGame::MM_RETAIL)
				cmd = new SetWorldMapVisited(parent);
			else
				cmd = new SetCameraSettings(parent);
			break;  // 0x19
		case RoomCommand::SetAnimatedMaterialList:
			cmd = new SetAnimatedMaterialList(parent);
			break;  // 0x1A (MM-ONLY)
		case RoomCommand::SetActorCutsceneList:
			cmd = new SetActorCutsceneList(parent);
			break;  // 0x1B (MM-ONLY)
		case RoomCommand::SetMinimapList:
			cmd = new SetMinimapList(parent);
			break;  // 0x1C (MM-ONLY)
		case RoomCommand::Unused1D:
			cmd = new Unused1D(parent);
			break;  // 0x1D
		case RoomCommand::SetMinimapChests:
			cmd = new SetMinimapChests(parent);
			break;  // 0x1E (MM-ONLY)
		default:
			cmd = new ZRoomCommandUnk(parent);
		}

		cmd->commandSet = rawDataIndex;
		cmd->ExtractCommandFromRoom(this, currentPtr);

		if (Globals::Instance->profile)
		{
			auto end = std::chrono::steady_clock::now();
			int64_t diff =
				std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			if (diff > 50)
				printf("OP: %s, TIME: %" PRIi64 "ms\n", cmd->GetCommandCName().c_str(), diff);
		}

		cmd->cmdIndex = currentIndex;

		commands.push_back(cmd);

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;

		currentPtr += 8;
		currentIndex++;
	}
}

void ZRoom::DeclareReferences(const std::string& prefix)
{
	for (auto& cmd : commands)
		cmd->DeclareReferences(prefix);
}

void ZRoom::ParseRawDataLate()
{
	for (auto& cmd : commands)
		cmd->ParseRawDataLate();
}

void ZRoom::DeclareReferencesLate(const std::string& prefix)
{
	for (auto& cmd : commands)
		cmd->DeclareReferencesLate(prefix);
}

Declaration* ZRoom::DeclareVar(const std::string& prefix, const std::string& body)
{
	std::string auxName = name;
	if (auxName == "")
		auxName = GetDefaultName(prefix);
	if (zroomType == ZResourceType::Scene || zroomType == ZResourceType::Room)
		auxName = StringHelper::Sprintf("%sCommands", name.c_str());

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, commands.size(), body);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZRoom::GetBodySourceCode() const
{
	std::string declaration;

	for (size_t i = 0; i < commands.size(); i++)
	{
		ZRoomCommand* cmd = commands[i];
		declaration += StringHelper::Sprintf("\t%s,", cmd->GetBodySourceCode().c_str());

		if (i + 1 < commands.size())
			declaration += "\n";
	}

	return declaration;
}

std::string ZRoom::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sSet_%06X", prefix.c_str(), rawDataIndex);
}

/*
 * There is one room in Ocarina of Time that lacks a header. Room 120, "Syotes", dates
 * back to very early in the game's development. Since this room is a special case,
 * declare automatically the data its contains whitout the need of a header.
 */
void ZRoom::SyotesRoomHack()
{
	PolygonType2 poly(parent, 0, this);

	poly.ParseRawData();
	poly.DeclareReferences(GetName());
	parent->AddDeclaration(0, poly.GetDeclarationAlignment(), poly.GetRawDataSize(),
	                       poly.GetSourceTypeName(), poly.GetDefaultName(GetName()),
	                       poly.GetBodySourceCode());
}

ZRoomCommand* ZRoom::FindCommandOfType(RoomCommand cmdType)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		if (commands[i]->GetRoomCommand() == cmdType)
			return commands[i];
	}

	return nullptr;
}

size_t ZRoom::GetDeclarationSizeFromNeighbor(uint32_t declarationAddress)
{
	auto currentDecl = parent->declarations.find(declarationAddress);
	if (currentDecl == parent->declarations.end())
		return 0;

	auto nextDecl = currentDecl;
	std::advance(nextDecl, 1);
	if (nextDecl == parent->declarations.end())
		return parent->GetRawData().size() - currentDecl->first;

	return nextDecl->first - currentDecl->first;
}

size_t ZRoom::GetCommandSizeFromNeighbor(ZRoomCommand* cmd)
{
	int32_t cmdIndex = -1;

	for (size_t i = 0; i < commands.size(); i++)
	{
		if (commands[i] == cmd)
		{
			cmdIndex = i;
			break;
		}
	}

	if (cmdIndex != -1)
	{
		if (cmdIndex + 1 < (int32_t)commands.size())
			return commands[cmdIndex + 1]->cmdAddress - commands[cmdIndex]->cmdAddress;
		else
			return parent->GetRawData().size() - commands[cmdIndex]->cmdAddress;
	}

	return 0;
}

void ZRoom::GetSourceOutputCode([[maybe_unused]] const std::string& prefix)
{
	if (hackMode != "syotes_room")
		DeclareVar(prefix, GetBodySourceCode());
}

size_t ZRoom::GetRawDataSize() const
{
	size_t size = 0;

	for (ZRoomCommand* cmd : commands)
		size += cmd->GetRawDataSize();

	return size;
}

std::string ZRoom::GetSourceTypeName() const
{
	return "SceneCmd";
}

ZResourceType ZRoom::GetResourceType() const
{
	assert(zroomType == ZResourceType::Scene || zroomType == ZResourceType::Room ||
	       zroomType == ZResourceType::AltHeader);
	return zroomType;
}

#include "CutsceneMM_Commands.h"

#include <cassert>
#include <unordered_map>

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

// Specific for command lists where each entry has size 8 bytes
const std::unordered_map<CutsceneMMCommands, CsCommandListDescriptor> csCommandsDescMM = {
	{CutsceneMMCommands::CS_CMD_MISC, {"CS_MISC", "(0x%02X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_SET_LIGHTING, {"CS_LIGHTING", "(0x%02X, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX, {"CS_SCENE_TRANS_FX", "(%i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_MOTIONBLUR, {"CS_MOTIONBLUR", "(%i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_GIVETATL, {"CS_GIVETATL", "(%i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_PLAYSEQ, {"CS_PLAYSEQ", "(0x%04X, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_130, {"CS_SCENE_UNK_130", "(0x%04X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_131, {"CS_SCENE_UNK_131", "(0x%04X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_132, {"CS_SCENE_UNK_132", "(%i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_STOPSEQ, {"CS_STOPSEQ", "(0x%04X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_PLAYAMBIENCE, {"CS_PLAYAMBIENCE", "(0x%04X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_FADEAMBIENCE, {"CS_FADEAMBIENCE", "(0x%04X, %i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_TERMINATOR, {"CS_TERMINATOR", "(%i, %i, %i)"}},
	{CutsceneMMCommands::CS_CMD_CHOOSE_CREDITS_SCENES,
     {"CS_CHOOSE_CREDITS_SCENES", "(%i, %i, %i)"}},
};

CutsceneSubCommandEntry_GenericMMCmd::CutsceneSubCommandEntry_GenericMMCmd(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex, CutsceneMMCommands cmdId)
	: CutsceneSubCommandEntry(rawData, rawDataIndex), commandId(cmdId)
{
}

std::string CutsceneSubCommandEntry_GenericMMCmd::GetBodySourceCode() const
{
	const auto& element = csCommandsDescMM.find(commandId);
	std::string entryFmt = "CS_UNK_DATA(0x%02X, %i, %i, %i)";

	if (element != csCommandsDescMM.end())
	{
		entryFmt = element->second.cmdMacro;
		entryFmt += element->second.args;
	}

	return StringHelper::Sprintf(entryFmt.c_str(), base, startFrame, endFrame, pad);
}

CutsceneMMCommand_GenericCmd::CutsceneMMCommand_GenericCmd(const std::vector<uint8_t>& rawData,
                                                           offset_t rawDataIndex,
                                                           CutsceneMMCommands cmdId)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	commandID = static_cast<uint32_t>(cmdId);

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_GenericMMCmd(rawData, rawDataIndex, cmdId);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_GenericCmd::GetCommandMacro() const
{
	const auto& element = csCommandsDescMM.find(static_cast<CutsceneMMCommands>(commandID));

	if (element != csCommandsDescMM.end())
	{
		return StringHelper::Sprintf("%s_LIST(%i)", element->second.cmdMacro, numEntries);
	}

	return StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%X, %i)", commandID, numEntries);
}

CutsceneSubCommandEntry_Camera::CutsceneSubCommandEntry_Camera(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Camera::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X)", base, startFrame);
}

size_t CutsceneSubCommandEntry_Camera::GetRawSize() const
{
	return 0x04;
}

CutsceneMMCommand_Camera::CutsceneMMCommand_Camera(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries / 4; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Camera(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Camera::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_CAMERA_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeScreen::CutsceneSubCommandEntry_FadeScreen(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_FadeScreen::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADESCREEN(0x%02X, %i, %i, %i, %i, %i)", base, startFrame,
	                             endFrame, unk_06, unk_07, unk_08);
}

size_t CutsceneSubCommandEntry_FadeScreen::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_FadeScreen::CutsceneMMCommand_FadeScreen(const std::vector<uint8_t>& rawData,
                                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_FadeScreen(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_FadeScreen::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_FADESCREEN_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeSeq::CutsceneSubCommandEntry_FadeSeq(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}

std::string CutsceneSubCommandEntry_FadeSeq::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADESEQ(%i, %i, %i)", base, startFrame, endFrame);
}

size_t CutsceneSubCommandEntry_FadeSeq::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_FadeSeq::CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData,
                                                     offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_FadeSeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_FadeSeq::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_FADESEQ_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_NonImplemented::CutsceneSubCommandEntry_NonImplemented(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

CutsceneMMCommand_NonImplemented::CutsceneMMCommand_NonImplemented(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_NonImplemented(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

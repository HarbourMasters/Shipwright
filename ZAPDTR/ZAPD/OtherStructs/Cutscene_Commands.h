#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Declaration.h"

enum class CutsceneCommands
{
	SetCameraPos = 0x0001,
	SetCameraFocus = 0x0002,
	Misc = 0x0003,
	SetLighting = 0x0004,
	SetCameraPosLink = 0x0005,
	SetCameraFocusLink = 0x0006,
	Cmd07 = 0x0007,
	Cmd08 = 0x0008,
	Cmd09 = 0x0009,  // Rumble
	Textbox = 0x0013,
	SetPlayerAction = 0x000A,
	SetActorAction1 = 0x000F,
	SetActorAction2 = 0x000E,
	SetActorAction3 = 0x0019,
	SetActorAction4 = 0x001D,
	SetActorAction5 = 0x001E,
	SetActorAction6 = 0x002C,
	SetActorAction7 = 0x001F,
	SetActorAction8 = 0x0031,
	SetActorAction9 = 0x003E,
	SetActorAction10 = 0x008F,
	SetSceneTransFX = 0x002D,
	PlayBGM = 0x0056,
	StopBGM = 0x0057,
	FadeBGM = 0x007C,
	SetTime = 0x008C,
	Terminator = 0x03E8,
};

typedef struct CsCommandListDescriptor
{
	const char* cmdMacro;
	const char* args;
} CsCommandListDescriptor;

class CutsceneSubCommandEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t pad;

	uint32_t commandID;

	CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	virtual ~CutsceneSubCommandEntry() = default;

	virtual std::string GetBodySourceCode() const;

	virtual size_t GetRawSize() const;
};

class CutsceneCommand
{
public:
	uint32_t commandID;
	uint32_t commandIndex;

	uint32_t numEntries;
	std::vector<CutsceneSubCommandEntry*> entries;

	CutsceneCommand(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	virtual ~CutsceneCommand();

	virtual std::string GetCommandMacro() const;
	virtual std::string GenerateSourceCode() const;
	virtual size_t GetCommandSize() const;

	virtual void SetCommandID(uint32_t nCommandID);
};

class CutsceneSubCommandEntry_GenericCmd : public CutsceneSubCommandEntry
{
public:
	CutsceneCommands commandId;

	uint32_t word0 = 0;
	uint32_t word1 = 0;

	uint32_t unused1 = 0;
	uint32_t unused2 = 0;
	uint32_t unused3 = 0;
	uint32_t unused4 = 0;
	uint32_t unused5 = 0;
	uint32_t unused6 = 0;
	uint32_t unused7 = 0;
	uint32_t unused8 = 0;
	uint32_t unused9 = 0;
	uint32_t unused10 = 0;

	CutsceneSubCommandEntry_GenericCmd(const std::vector<uint8_t>& rawData, offset_t rawDataIndex,
	                                   CutsceneCommands cmdId);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_GenericCmd : public CutsceneCommand
{
public:
	CutsceneCommand_GenericCmd(const std::vector<uint8_t>& rawData, offset_t rawDataIndex,
	                           CutsceneCommands cmdId);

	std::string GetCommandMacro() const override;
};

class CutsceneCameraPoint : public CutsceneSubCommandEntry
{
public:
	int8_t continueFlag;
	int8_t cameraRoll;
	int16_t nextPointFrame;
	float viewAngle;
	int16_t posX, posY, posZ;
	int16_t unused;

	CutsceneCameraPoint(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommandSetCameraPos : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused;

	CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;

	size_t GetCommandSize() const override;
};

class CutsceneCommandSceneTransFX : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;

	CutsceneCommandSceneTransFX(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneSubCommandEntry_Rumble : public CutsceneSubCommandEntry
{
public:
	uint8_t unk_06;
	uint8_t unk_07;
	uint8_t unk_08;
	uint8_t unk_09;
	uint8_t unk_0A;
	uint8_t unk_0B;

	CutsceneSubCommandEntry_Rumble(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_Rumble : public CutsceneCommand
{
public:
	CutsceneCommand_Rumble(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_SetTime : public CutsceneSubCommandEntry
{
public:
	uint8_t hour;
	uint8_t minute;
	uint32_t unk_08;

	CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_SetTime : public CutsceneCommand
{
public:
	CutsceneCommand_SetTime(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_TextBox : public CutsceneSubCommandEntry
{
public:
	uint16_t type;
	uint16_t textId1;
	uint16_t textId2;

	CutsceneSubCommandEntry_TextBox(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_TextBox : public CutsceneCommand
{
public:
	CutsceneCommand_TextBox(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_ActorAction : public CutsceneSubCommandEntry
{
public:
	uint16_t rotX, rotY, rotZ;
	int32_t startPosX, startPosY, startPosZ;
	int32_t endPosX, endPosY, endPosZ;
	float normalX, normalY, normalZ;

	CutsceneSubCommandEntry_ActorAction(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);
	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_ActorAction : public CutsceneCommand
{
public:
	CutsceneCommand_ActorAction(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneCommand_Terminator : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unknown;

	CutsceneCommand_Terminator(const std::vector<uint8_t>& rawData, offset_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

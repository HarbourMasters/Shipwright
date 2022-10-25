#include "Cutscene.h"
#include "spdlog/spdlog.h"

enum class CutsceneCommands
{
	Cmd00 = 0x0000,
	SetCameraPos = 0x0001,
	SetCameraFocus = 0x0002,
	SpecialAction = 0x0003,
	SetLighting = 0x0004,
	SetCameraPosLink = 0x0005,
	SetCameraFocusLink = 0x0006,
	Cmd07 = 0x0007,
	Cmd08 = 0x0008,
	Cmd09 = 0x0009,
	Unknown = 0x001A,
	Textbox = 0x0013,
	SetActorAction0 = 0x000A,
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
	Nop = 0x000B,
	PlayBGM = 0x0056,
	StopBGM = 0x0057,
	FadeBGM = 0x007C,
	SetTime = 0x008C,
	Terminator = 0x03E8,
	End = 0xFFFF,
	Error = 0xFEAF,
};

static inline uint32_t read_CMD_BBBB(Ship::BinaryReader* reader)
{
	uint32_t v;
	reader->Read((char*)&v, sizeof(uint32_t));

	return v;
}

static inline uint32_t read_CMD_BBH(Ship::BinaryReader* reader)
{
	uint32_t v;
	reader->Read((char*)&v, sizeof(uint32_t));

	// swap the half word to match endianness
	if (reader->GetEndianness() != Ship::Endianness::Native)
	{
		uint8_t* b = (uint8_t*)&v;
		uint8_t tmp = b[2];
		b[2] = b[3];
		b[3] = tmp;
	}

	return v;
}

static inline uint32_t read_CMD_HBB(Ship::BinaryReader* reader)
{
	uint32_t v;
	reader->Read((char*)&v, sizeof(uint32_t));

	// swap the half word to match endianness
	if (reader->GetEndianness() != Ship::Endianness::Native)
	{
		uint8_t* b = (uint8_t*)&v;
		uint8_t tmp = b[0];
		b[0] = b[1];
		b[1] = tmp;
	}

	return v;
}

static inline uint32_t read_CMD_HH(Ship::BinaryReader* reader)
{
	uint32_t v;
	reader->Read((char*)&v, sizeof(uint32_t));

	// swap the half words to match endianness
	if (reader->GetEndianness() != Ship::Endianness::Native)
	{
		uint8_t* b = (uint8_t*)&v;
		uint8_t tmp = b[0];
		b[0] = b[1];
		b[1] = tmp;
		tmp = b[2];
		b[2] = b[3];
		b[3] = tmp;
	}

	return v;
}

void Ship::CutsceneV0::ParseFileBinary(Ship::BinaryReader* reader, Resource* res)
{
	Cutscene* cs = (Cutscene*)res;

	ResourceFile::ParseFileBinary(reader, res);

	uint32_t numEntries = reader->ReadUInt32();
	cs->commands.reserve(numEntries);

	uint32_t numCommands = reader->ReadUInt32();
	cs->commands.push_back(numCommands);

	// endFrame
	cs->commands.push_back(reader->ReadUInt32());

	while (true)
	{
		uint32_t commandId = reader->ReadUInt32();
		cs->commands.push_back(commandId);

		switch (commandId)
		{
		case (uint32_t)CutsceneCommands::SetCameraPos:
		{
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));

			while (true)
			{
				uint32_t val = read_CMD_BBH(reader);
				int8_t continueFlag = ((int8_t*)&val)[0];

				cs->commands.push_back(val);
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));

				if (continueFlag == -1)
					break;
			}
		}
		break;
		case (uint32_t)CutsceneCommands::SetCameraFocus:
		{
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));

			while (true)
			{
				uint32_t val = read_CMD_BBH(reader);
				int8_t continueFlag = ((int8_t*)&val)[0];

				cs->commands.push_back(val);
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));

				if (continueFlag == -1)
					break;
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SpecialAction:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetLighting:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetCameraPosLink:
		{
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));

			while (true)
			{
				uint32_t val = read_CMD_BBH(reader);
				int8_t continueFlag = ((int8_t*)&val)[0];

				cs->commands.push_back(val);
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));

				if (continueFlag == -1)
					break;
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetCameraFocusLink:
		{
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));

			while (true)
			{
				uint32_t val = read_CMD_BBH(reader);
				int8_t continueFlag = ((int8_t*)&val)[0];

				cs->commands.push_back(val);
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));

				if (continueFlag == -1)
					break;
			}
			break;
		}
		case (uint32_t)CutsceneCommands::Cmd09:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HBB(reader));
				cs->commands.push_back(read_CMD_BBH(reader));
			}
			break;
		}
		case 0x15:
		case (uint32_t)CutsceneCommands::Unknown:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
		}
		break;
		case (uint32_t)CutsceneCommands::Textbox:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetActorAction0:
		case (uint32_t)CutsceneCommands::SetActorAction1:
		case 17:
		case 18:
		case 23:
		case 34:
		case 39:
		case 46:
		case 76:
		case 85:
		case 93:
		case 105:
		case 107:
		case 110:
		case 119:
		case 123:
		case 138:
		case 139:
		case 144:
		case (uint32_t)CutsceneCommands::SetActorAction2:
		case 16:
		case 24:
		case 35:
		case 40:
		case 48:
		case 64:
		case 68:
		case 70:
		case 78:
		case 80:
		case 94:
		case 116:
		case 118:
		case 120:
		case 125:
		case 131:
		case 141:
		case (uint32_t)CutsceneCommands::SetActorAction3:
		case 36:
		case 41:
		case 50:
		case 67:
		case 69:
		case 72:
		case 74:
		case 81:
		case 106:
		case 117:
		case 121:
		case 126:
		case 132:
		case (uint32_t)CutsceneCommands::SetActorAction4:
		case 37:
		case 42:
		case 51:
		case 53:
		case 63:
		case 65:
		case 66:
		case 75:
		case 82:
		case 108:
		case 127:
		case 133:
		case (uint32_t)CutsceneCommands::SetActorAction5:
		case 38:
		case 43:
		case 47:
		case 54:
		case 79:
		case 83:
		case 128:
		case 135:
		case (uint32_t)CutsceneCommands::SetActorAction6:
		case 55:
		case 77:
		case 84:
		case 90:
		case 129:
		case 136:
		case (uint32_t)CutsceneCommands::SetActorAction7:
		case 52:
		case 57:
		case 58:
		case 88:
		case 115:
		case 130:
		case 137:
		case (uint32_t)CutsceneCommands::SetActorAction8:
		case 60:
		case 89:
		case 111:
		case 114:
		case 134:
		case 142:
		case (uint32_t)CutsceneCommands::SetActorAction9:
		case (uint32_t)CutsceneCommands::SetActorAction10:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}

			break;
		}
		case (uint32_t)CutsceneCommands::SetSceneTransFX:
		{
			cs->commands.push_back(reader->ReadUInt32());
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));
			break;
		}
		case (uint32_t)CutsceneCommands::PlayBGM:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::StopBGM:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::FadeBGM:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::SetTime:
		{
			uint32_t size = reader->ReadUInt32();
			cs->commands.push_back(size);

			for (uint32_t i = 0; i < size; i++)
			{
				cs->commands.push_back(read_CMD_HH(reader));
				cs->commands.push_back(read_CMD_HBB(reader));
				cs->commands.push_back(reader->ReadUInt32());
			}
			break;
		}
		case (uint32_t)CutsceneCommands::Terminator:
		{
			cs->commands.push_back(reader->ReadUInt32());
			cs->commands.push_back(read_CMD_HH(reader));
			cs->commands.push_back(read_CMD_HH(reader));
			break;
		}
		case 0xFFFFFFFF: // CS_END
		{
			cs->commands.push_back(reader->ReadUInt32());
			return;
		}
		default:
			SPDLOG_TRACE("CutsceneV0: Unknown command {}\n", commandId);
			// error?
			break;
		}
	}
}

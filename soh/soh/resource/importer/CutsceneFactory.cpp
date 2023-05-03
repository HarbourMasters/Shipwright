#include "soh/resource/importer/CutsceneFactory.h"
#include "soh/resource/type/Cutscene.h"
#include "spdlog/spdlog.h"

namespace Ship {
std::shared_ptr<Resource> CutsceneFactory::ReadResource(std::shared_ptr<ResourceManager> resourceMgr,
                                                        std::shared_ptr<ResourceInitData> initData,
                                                        std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Cutscene>(resourceMgr, initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->InitData->ResourceVersion) {
    case 0:
	factory = std::make_shared<CutsceneFactoryV0>();
	break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Cutscene with version {}", resource->InitData->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

static inline uint32_t read_CMD_BBBB(std::shared_ptr<BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    return v;
}

static inline uint32_t read_CMD_BBH(std::shared_ptr<BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half word to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
        uint8_t* b = (uint8_t*)&v;
        uint8_t tmp = b[2];
        b[2] = b[3];
        b[3] = tmp;
    }

    return v;
}

static inline uint32_t read_CMD_HBB(std::shared_ptr<BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half word to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
        uint8_t* b = (uint8_t*)&v;
        uint8_t tmp = b[0];
        b[0] = b[1];
        b[1] = tmp;
    }

    return v;
}

static inline uint32_t read_CMD_HH(std::shared_ptr<BinaryReader> reader) {
    uint32_t v;
    reader->Read((char*)&v, sizeof(uint32_t));

    // swap the half words to match endianness
    if (reader->GetEndianness() != Ship::Endianness::Native) {
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

void Ship::CutsceneFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                              std::shared_ptr<Resource> resource)
{
    std::shared_ptr<Cutscene> cutscene = std::static_pointer_cast<Cutscene>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, cutscene);

    uint32_t numEntries = reader->ReadUInt32();
    cutscene->commands.reserve(numEntries);

    cutscene->numCommands = reader->ReadUInt32();
    cutscene->commands.push_back(cutscene->numCommands);

    cutscene->endFrame = reader->ReadUInt32();
    cutscene->commands.push_back(cutscene->endFrame);

    while (true) {
        uint32_t commandId = reader->ReadUInt32();
        cutscene->commands.push_back(commandId);

        switch (commandId) {
            case (uint32_t)CutsceneCommands::SetCameraPos: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
            } break;
            case (uint32_t)CutsceneCommands::SetCameraFocus: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SpecialAction: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetLighting: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetCameraPosLink: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetCameraFocusLink: {
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));

                while (true) {
                    uint32_t val = read_CMD_BBH(reader);
                    int8_t continueFlag = ((int8_t*)&val)[0];

                    cutscene->commands.push_back(val);
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));

                    if (continueFlag == -1) {
                        break;
                    }
                }
                break;
            }
            case (uint32_t)CutsceneCommands::Cmd09: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(read_CMD_BBH(reader));
                }
                break;
            }
            case 0x15:
            case (uint32_t)CutsceneCommands::Unknown: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
            } break;
            case (uint32_t)CutsceneCommands::Textbox: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
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
            case (uint32_t)CutsceneCommands::SetActorAction10: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }

                break;
            }
            case (uint32_t)CutsceneCommands::SetSceneTransFX: {
                cutscene->commands.push_back(reader->ReadUInt32());
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));
                break;
            }
            case (uint32_t)CutsceneCommands::PlayBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::StopBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::FadeBGM: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::SetTime: {
                uint32_t size = reader->ReadUInt32();
                cutscene->commands.push_back(size);

                for (uint32_t i = 0; i < size; i++) {
                    cutscene->commands.push_back(read_CMD_HH(reader));
                    cutscene->commands.push_back(read_CMD_HBB(reader));
                    cutscene->commands.push_back(reader->ReadUInt32());
                }
                break;
            }
            case (uint32_t)CutsceneCommands::Terminator: {
                cutscene->commands.push_back(reader->ReadUInt32());
                cutscene->commands.push_back(read_CMD_HH(reader));
                cutscene->commands.push_back(read_CMD_HH(reader));
                break;
            }
            case 0xFFFFFFFF: // CS_END
            {
                cutscene->commands.push_back(reader->ReadUInt32());
                return;
            }
            default:
                SPDLOG_TRACE("CutsceneV0: Unknown command {}\n", commandId);
                // error?
                break;
        }
    }
}
} // namespace Ship

#include "Scene.h"

namespace Ship
{
	Scene::~Scene()
	{
		int bp = 0;
	}

	void SceneV0::ParseFileBinary(BinaryReader* reader, Resource* res)
	{	
		Scene* scene = (Scene*)res;

		ResourceFile::ParseFileBinary(reader, res);

		uint32_t cmdCnt = reader->ReadUInt32();
		scene->commands.reserve(cmdCnt);

		for (uint32_t i = 0; i < cmdCnt; i++)
			scene->commands.push_back(ParseSceneCommand(reader));
	}

	SceneCommand* SceneV0::ParseSceneCommand(BinaryReader* reader)
	{
		SceneCommandID cmdID = (SceneCommandID)reader->ReadInt32();

		reader->Seek(-4, SeekOffsetType::Current);

		switch (cmdID)
		{
		case SceneCommandID::SetStartPositionList: return new SetStartPositionList(reader);
		case SceneCommandID::SetActorList: return new SetActorList(reader);
		case SceneCommandID::SetTransitionActorList: return new SetTransitionActorList(reader);
		case SceneCommandID::SetWind: return new SetWind(reader);
		case SceneCommandID::SetTimeSettings: return new SetTimeSettings(reader);
		case SceneCommandID::SetSkyboxModifier: return new SetSkyboxModifier(reader);
		case SceneCommandID::SetEchoSettings: return new SetEchoSettings(reader);
		case SceneCommandID::SetSoundSettings: return new SetSoundSettings(reader);
		case SceneCommandID::SetSkyboxSettings: return new SetSkyboxSettings(reader);
		case SceneCommandID::SetRoomBehavior: return new SetRoomBehavior(reader);
		case SceneCommandID::SetCsCamera: return new SetCsCamera(reader);
		case SceneCommandID::SetMesh: return new SetMesh(reader);
		case SceneCommandID::SetCameraSettings: return new SetCameraSettings(reader);
		case SceneCommandID::SetLightingSettings: return new SetLightingSettings(reader);
		case SceneCommandID::SetLightList: return new SetLightList(reader);
		case SceneCommandID::SetRoomList: return new SetRoomList(reader);
		case SceneCommandID::SetCollisionHeader: return new SetCollisionHeader(reader);
		case SceneCommandID::SetEntranceList: return new SetEntranceList(reader);
		case SceneCommandID::SetSpecialObjects: return new SetSpecialObjects(reader);
		case SceneCommandID::SetObjectList: return new SetObjectList(reader);
		case SceneCommandID::SetAlternateHeaders: return new SetAlternateHeaders(reader);
		case SceneCommandID::SetExitList: return new ExitList(reader);
		case SceneCommandID::SetCutscenes: return new SetCutscenes(reader);
		case SceneCommandID::SetPathways: return new SetPathways(reader);
		case SceneCommandID::EndMarker: return new EndMarker(reader);
		default:
			SPDLOG_ERROR("UNIMPLEMENTED COMMAND: {}", (int)cmdID);
			reader->ReadInt32();
			break;
		}

		return nullptr;
	}

	SceneCommand::SceneCommand(BinaryReader* reader)
	{
		cmdID = (SceneCommandID)reader->ReadInt32();
	}

	SetWind::SetWind(BinaryReader* reader) : SceneCommand(reader)
	{
		windWest = reader->ReadInt8();
		windVertical = reader->ReadInt8();
		windSouth = reader->ReadInt8();
		clothFlappingStrength = reader->ReadInt8();
	}

	ExitList::ExitList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t numExits = reader->ReadUInt32();

		exits.reserve(numExits);
		for (uint32_t i = 0; i < numExits; i++)
			exits.push_back(reader->ReadUInt16());
	}

	SetTimeSettings::SetTimeSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		hour = reader->ReadInt8();
		min = reader->ReadInt8();
		unk = reader->ReadInt8();
	}

	SetSkyboxModifier::SetSkyboxModifier(BinaryReader* reader) : SceneCommand(reader)
	{
		disableSky = reader->ReadInt8();
		disableSunMoon = reader->ReadInt8();
	}

	SetEchoSettings::SetEchoSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		echo = reader->ReadInt8();
	}

	SetSoundSettings::SetSoundSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		reverb = reader->ReadInt8();
		nightTimeSFX = reader->ReadInt8();
		musicSequence = reader->ReadInt8();
	}

	SetSkyboxSettings::SetSkyboxSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		unk1 = reader->ReadInt8();
		skyboxNumber = reader->ReadInt8();
		cloudsType = reader->ReadInt8();
		isIndoors = reader->ReadInt8();
	}

	SetRoomBehavior::SetRoomBehavior(BinaryReader* reader) : SceneCommand(reader)
	{
		gameplayFlags = reader->ReadInt8();
		gameplayFlags2 = reader->ReadInt32();
	}

	SetCsCamera::SetCsCamera(BinaryReader* reader) : SceneCommand(reader)
	{
		reader->ReadInt8(); // camSize
		reader->ReadInt32(); // segOffset

		// OTRTODO: FINISH!
	}

	MeshData::MeshData()
	{
		x = 0;
		y = 0;
		z = 0;
		unk_06 = 0;
		// opa;
		// xlu;
	}

	SetMesh::SetMesh(BinaryReader* reader) : SceneCommand(reader)
	{
		data = reader->ReadInt8();
		meshHeaderType = reader->ReadInt8();

		uint32_t numPoly = 1;

		if (meshHeaderType != 1)
			numPoly = reader->ReadInt8();

		meshes.reserve(numPoly);
		for (uint32_t i = 0; i < numPoly; i++)
		{
			MeshData mesh;

			if (meshHeaderType == 0)
			{
				int polyType = reader->ReadInt8();
				mesh.x = 0;
				mesh.y = 0;
				mesh.z = 0;
				mesh.unk_06 = 0;
			}
			else if (meshHeaderType == 2)
			{
				int polyType = reader->ReadInt8();
				mesh.x = reader->ReadInt16();
				mesh.y = reader->ReadInt16();
				mesh.z = reader->ReadInt16();
				mesh.unk_06 = reader->ReadInt16();
			}
			else
			{
				mesh.imgFmt = reader->ReadUByte();
				mesh.imgOpa = reader->ReadString();
				mesh.imgXlu = reader->ReadString();


				uint32_t imgCnt = reader->ReadUInt32();
				mesh.images.reserve(imgCnt);

				for (uint32_t i = 0; i < imgCnt; i++)
				{
					BGImage img;

					img.unk_00 = reader->ReadUInt16();
					img.id = reader->ReadUByte();
					img.sourceBackground = reader->ReadString();
					img.unk_0C = reader->ReadUInt32();
					img.tlut = reader->ReadUInt32();
					img.width = reader->ReadUInt16();
					img.height = reader->ReadUInt16();
					img.fmt = reader->ReadUByte();
					img.siz = reader->ReadUByte();
					img.mode0 = reader->ReadUInt16();
					img.tlutCount = reader->ReadUInt16();

					mesh.images.push_back(img);
				}

				int polyType = reader->ReadInt8();

				int bp = 0;
			}


			mesh.opa = reader->ReadString();
			mesh.xlu = reader->ReadString();

			meshes.push_back(mesh);
		}
	}

	SetCameraSettings::SetCameraSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		cameraMovement = reader->ReadInt8();
		mapHighlights = reader->ReadInt32();
	}

	SetLightingSettings::SetLightingSettings(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadInt32();
		settings.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			LightingSettings entry = LightingSettings();

			entry.ambientClrR = reader->ReadInt8();
			entry.ambientClrG = reader->ReadInt8();
			entry.ambientClrB = reader->ReadInt8();

			entry.diffuseDirA_X = reader->ReadInt8();
			entry.diffuseDirA_Y = reader->ReadInt8();
			entry.diffuseDirA_Z = reader->ReadInt8();

			entry.diffuseClrA_R = reader->ReadInt8();
			entry.diffuseClrA_G = reader->ReadInt8();
			entry.diffuseClrA_B = reader->ReadInt8();

			entry.diffuseDirB_X = reader->ReadInt8();
			entry.diffuseDirB_Y = reader->ReadInt8();
			entry.diffuseDirB_Z = reader->ReadInt8();

			entry.diffuseClrB_R = reader->ReadInt8();
			entry.diffuseClrB_G = reader->ReadInt8();
			entry.diffuseClrB_B = reader->ReadInt8();

			entry.fogClrR = reader->ReadInt8();
			entry.fogClrG = reader->ReadInt8();
			entry.fogClrB = reader->ReadInt8();

			entry.fogNear = reader->ReadInt16();
			entry.fogFar = reader->ReadUInt16();

			settings.push_back(entry);
		}
	}

	SetRoom::SetRoom(BinaryReader* reader)
	{
		name = reader->ReadString();
		vromStart = reader->ReadInt32();
		vromEnd = reader->ReadInt32();
	}

	SetRoomList::SetRoomList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t numRooms = reader->ReadInt32();
		rooms.reserve(numRooms);

		for (uint32_t i = 0; i < numRooms; i++)
			rooms.push_back(SetRoom(reader));

	}

	SetCollisionHeader::SetCollisionHeader(BinaryReader* reader) : SceneCommand(reader)
	{
		filePath = reader->ReadString();
	}

	SetEntranceList::SetEntranceList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadUInt32();
		entrances.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			EntranceEntry entry = EntranceEntry();
			entry.startPositionIndex = reader->ReadInt8();
			entry.roomToLoad = reader->ReadInt8();

			entrances.push_back(entry);
		}
	}

	SetSpecialObjects::SetSpecialObjects(BinaryReader* reader) : SceneCommand(reader)
	{
		elfMessage = reader->ReadInt8();
		globalObject = reader->ReadInt16();
	}

	SetObjectList::SetObjectList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t numEntries = reader->ReadUInt32();
		objects.reserve(numEntries);

		for (uint32_t i = 0; i < numEntries; i++)
			objects.push_back(reader->ReadUInt16());
	}

	SetStartPositionList::SetStartPositionList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadUInt32();
		entries.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			ActorSpawnEntry entry = ActorSpawnEntry();

			entry.actorNum = reader->ReadUInt16();
			entry.posX = reader->ReadInt16();
			entry.posY = reader->ReadInt16();
			entry.posZ = reader->ReadInt16();
			entry.rotX = reader->ReadInt16();
			entry.rotY = reader->ReadInt16();
			entry.rotZ = reader->ReadInt16();
			entry.initVar = reader->ReadUInt16();

			entries.push_back(entry);
		}
	}

	SetActorList::SetActorList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadUInt32();
		entries.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			ActorSpawnEntry entry = ActorSpawnEntry();

			entry.actorNum = reader->ReadUInt16();
			entry.posX = reader->ReadInt16();
			entry.posY = reader->ReadInt16();
			entry.posZ = reader->ReadInt16();
			entry.rotX = reader->ReadInt16();
			entry.rotY = reader->ReadInt16();
			entry.rotZ = reader->ReadInt16();
			entry.initVar = reader->ReadUInt16();

			entries.push_back(entry);
		}
	}

	SetTransitionActorList::SetTransitionActorList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadUInt32();
		entries.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			TransitionActorEntry entry = TransitionActorEntry();

			entry.frontObjectRoom = reader->ReadUByte();
			entry.frontTransitionReaction = reader->ReadUByte();
			entry.backObjectRoom = reader->ReadUByte();
			entry.backTransitionReaction = reader->ReadUByte();
			entry.actorNum = reader->ReadInt16();
			entry.posX = reader->ReadInt16();
			entry.posY = reader->ReadInt16();
			entry.posZ = reader->ReadInt16();
			entry.rotY = reader->ReadInt16();
			entry.initVar = reader->ReadUInt16();

			entries.push_back(entry);
		}
	}

	EndMarker::EndMarker(BinaryReader* reader) : SceneCommand(reader)
	{
	}

	LightInfo::LightInfo()
	{
	}

	SetLightList::SetLightList(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t cnt = reader->ReadUInt32();
		lights.reserve(cnt);

		for (uint32_t i = 0; i < cnt; i++)
		{
			LightInfo light = LightInfo();

			light.type = reader->ReadUByte();

			light.x = reader->ReadInt16();
			light.y = reader->ReadInt16();
			light.z = reader->ReadInt16();

			light.r = reader->ReadUByte();
			light.g = reader->ReadUByte();
			light.b = reader->ReadUByte();

			light.drawGlow = reader->ReadUByte();
			light.radius = reader->ReadInt16();

			lights.push_back(light);
		}
	}

	SetAlternateHeaders::SetAlternateHeaders(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t numHeaders = reader->ReadUInt32();
		headers.reserve(numHeaders);

		for (uint32_t i = 0; i < numHeaders; i++)
			headers.push_back(reader->ReadString());
	}

	SetCutscenes::SetCutscenes(BinaryReader* reader) : SceneCommand(reader)
	{
		cutscenePath = reader->ReadString();
	}

	SetPathways::SetPathways(BinaryReader* reader) : SceneCommand(reader)
	{
		uint32_t numPaths = reader->ReadUInt32();
		paths.reserve(numPaths);

		for (uint32_t i = 0; i < numPaths; i++)
			paths.push_back(reader->ReadString());
	}
}

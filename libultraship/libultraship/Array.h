#pragma once

#include "Resource.h"
#include "Vertex.h"
#include "Scalar.h"

namespace Ship
{
	// Note: This is only used for V0 archives
	enum class ZResourceType
	{
		Error,
		Animation,
		Array,
		AltHeader,
		Background,
		Blob,
		CollisionHeader,
		Cutscene,
		DisplayList,
		Limb,
		LimbTable,
		Mtx,
		Path,
		PlayerAnimationData,
		Room,
		RoomCommand,
		Scalar,
		Scene,
		Skeleton,
		String,
		Symbol,
		Texture,
		TextureAnimation,
		TextureAnimationParams,
		Vector,
		Vertex,
		Audio
	};

	class ArrayV2 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};


	class ArrayV0 : public ResourceFile
	{
	public:
		void ParseFileBinary(BinaryReader* reader, Resource* res, bool readFullHeader) override;
	};

	class Array : public Resource
	{
	public:
		std::vector<ScalarData> scalars;
		std::vector<Vtx> vertices;

		// For everything else...
		std::vector<Resource*> resources;
	};
}
#include "ResourceLoader.h"
#include "MaterialFactory.h"
#include "SceneFactory.h"
#include "CollisionHeaderFactory.h"
#include "DisplayListFactory.h"
#include "PlayerAnimationFactory.h"
#include "SkeletonFactory.h"
#include "SkeletonLimbFactory.h"
#include "AnimationFactory.h"
#include "VtxFactory.h"
#include "CutsceneFactory.h"
#include "ArrayFactory.h"
#include "PathFactory.h"
#include "TextFactory.h"
#include "TextureFactory.h"
#include "BlobFactory.h"
#include "MtxFactory.h"
#include "AudioFactory.h"
#include <Utils/MemoryStream.h>

namespace Ship
{
    Resource* ResourceLoader::LoadResource(std::shared_ptr<File> FileToLoad)
    {
        auto memStream = std::make_shared<MemoryStream>(FileToLoad->buffer.get(), FileToLoad->dwBufferSize);
        auto reader = std::make_shared<BinaryReader>(memStream);

        Endianness endianness = (Endianness)reader->ReadByte();

        for (int i = 0; i < 3; i++)
            reader->ReadByte();

        reader->SetEndianness(endianness);

        ResourceType resourceType = (ResourceType)reader->ReadUInt32();

        Resource* result = LoadResource(reader.get(), resourceType, true);

        if (result != nullptr) {
            result->file = FileToLoad;
            result->resType = resourceType;
        } else {
            if (FileToLoad != nullptr) {
                SPDLOG_ERROR("Failed to load resource of type {} \"{}\"", resourceType, FileToLoad->path);
            } else {
                SPDLOG_ERROR("Failed to load resource because the file did not load.");
            }
        }

        return result;
    }

    Resource* ResourceLoader::LoadResource(BinaryReader* reader, ResourceType resourceType, bool readFullHeader)
    {
        Resource* result = nullptr;

        switch (resourceType)
        {
        case ResourceType::Material:
            result = MaterialFactory::ReadMaterial(reader, readFullHeader);
            break;
        case ResourceType::Texture:
            result = TextureFactory::ReadTexture(reader, readFullHeader);
            break;
        case ResourceType::Room:
            result = SceneFactory::ReadScene(reader, readFullHeader);
            break;
        case ResourceType::CollisionHeader:
            result = CollisionHeaderFactory::ReadCollisionHeader(reader, readFullHeader);
            break;
        case ResourceType::DisplayList:
            result = DisplayListFactory::ReadDisplayList(reader, readFullHeader);
            break;
        case ResourceType::PlayerAnimation:
            result = PlayerAnimationFactory::ReadPlayerAnimation(reader, readFullHeader);
            break;
        case ResourceType::Skeleton:
            result = SkeletonFactory::ReadSkeleton(reader, readFullHeader);
            break;
        case ResourceType::SkeletonLimb:
            result = SkeletonLimbFactory::ReadSkeletonLimb(reader, readFullHeader);
            break;
        case ResourceType::Vertex:
            result = VertexFactory::ReadVtx(reader, readFullHeader);
            break;
        case ResourceType::Animation:
            result = AnimationFactory::ReadAnimation(reader, readFullHeader);
            break;
        case ResourceType::Cutscene:
            result = CutsceneFactory::ReadCutscene(reader, readFullHeader);
            break;
        case ResourceType::Array:
            result = ArrayFactory::ReadArray(reader, readFullHeader);
            break;
        case ResourceType::Path:
            result = PathFactory::ReadPath(reader, readFullHeader);
            break;
        case ResourceType::Text:
            result = TextFactory::ReadText(reader, readFullHeader);
            break;
        case ResourceType::Blob:
            result = BlobFactory::ReadBlob(reader, readFullHeader);
            break;
        case ResourceType::Matrix:
            result = MtxFactory::ReadMtx(reader, readFullHeader);
            break;
        case ResourceType::Audio:
            result = AudioFactory::ReadAudio(reader.get());
            break;
        case ResourceType::AudioSample:
            result = AudioSampleFactory::ReadAudioSample(reader.get());
            break;
        case ResourceType::AudioSoundFont:
            result = AudioSoundFontFactory::ReadAudioSoundFont(reader.get());
            break;
        case ResourceType::AudioSequence:
            result = AudioSequenceFactory::ReadAudioSequence(reader.get());
            break;
        default:
            // RESOURCE TYPE NOT SUPPORTED
            break;
        }

        return result;
    }
}
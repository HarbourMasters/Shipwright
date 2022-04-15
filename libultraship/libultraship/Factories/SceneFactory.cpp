#include "SceneFactory.h"

namespace Ship
{
    Scene* SceneFactory::ReadScene(BinaryReader* reader, bool readFullHeader)
    {
        Scene* scene = new Scene();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            SceneV0 Scene = SceneV0();
            Scene.ParseFileBinary(reader, scene, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return scene;
    }
}
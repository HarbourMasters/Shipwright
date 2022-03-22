#include "SceneFactory.h"

namespace Ship
{
    Scene* SceneFactory::ReadScene(BinaryReader* reader)
    {
        Scene* scene = new Scene();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            SceneV0 Scene = SceneV0();
            Scene.ParseFileBinary(reader, scene);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return scene;
    }
}
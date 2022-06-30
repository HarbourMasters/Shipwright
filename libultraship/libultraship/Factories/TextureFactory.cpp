#include "TextureFactory.h"

namespace Ship
{
    Texture* TextureFactory::ReadTexture(BinaryReader* reader, bool readFullHeader)
    {
        Texture* tex = new Texture();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            TextureV0 texFac = TextureV0();
            texFac.ParseFileBinary(reader, tex, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return tex;
    }
}
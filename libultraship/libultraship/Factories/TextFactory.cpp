#include "TextFactory.h"

namespace Ship
{
    Text* TextFactory::ReadText(BinaryReader* reader)
    {
        Text* txt = new Text();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            TextV0 txtFac = TextV0();
            txtFac.ParseFileBinary(reader, txt);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return txt;
    }
}
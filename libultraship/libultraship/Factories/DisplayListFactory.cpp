#include "DisplayListFactory.h"

namespace Ship
{
    DisplayList* DisplayListFactory::ReadDisplayList(BinaryReader* reader)
    {
        DisplayList* dl = new DisplayList();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            DisplayListV0 DL = DisplayListV0();
            DL.ParseFileBinary(reader, dl);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return dl;

    }
}
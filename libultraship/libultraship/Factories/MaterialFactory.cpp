#include "MaterialFactory.h"

namespace Ship
{
    Material* MaterialFactory::ReadMaterial(BinaryReader* reader)
    {
        Material* mat = new Material();

        Version version = (Version)reader->ReadUInt32();
        
        switch (version)
        {
        case Version::Deckard:
        {
            MaterialV0 Material = MaterialV0();
            Material.ParseFileBinary(reader, mat);
        }
            break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return mat;
    }
}
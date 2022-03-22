#include "CutsceneFactory.h"

namespace Ship
{
	Cutscene* CutsceneFactory::ReadCutscene(BinaryReader* reader)
	{
        Cutscene* cs = new Cutscene();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            CutsceneV0 cutscene = CutsceneV0();
            cutscene.ParseFileBinary(reader, cs);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return cs;
	}
}
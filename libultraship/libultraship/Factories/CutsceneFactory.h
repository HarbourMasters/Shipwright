#include "../Cutscene.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class CutsceneFactory
    {
    public:
        static Cutscene* ReadCutscene(BinaryReader* reader);
    };
}
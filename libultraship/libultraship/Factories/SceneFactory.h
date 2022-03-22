#include "../Scene.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class SceneFactory
    {
    public:
        static Scene* ReadScene(BinaryReader* reader);
    };
}
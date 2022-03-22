#include "../Animation.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class AnimationFactory
    {
    public:
        static Animation* ReadAnimation(BinaryReader* reader);
    };
}
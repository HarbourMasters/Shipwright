#include "../PlayerAnimation.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class PlayerAnimationFactory
    {
    public:
        static PlayerAnimation* ReadPlayerAnimation(BinaryReader* reader);
    };
}
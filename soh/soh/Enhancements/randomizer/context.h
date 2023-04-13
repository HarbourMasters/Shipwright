#pragma once

#include "randomizerTypes.h"
#include "3drando/text.hpp"
#include "hint.h"

#include <memory>
#include <array>

/**
 * @brief Singleton for storing and accessing dynamic Randomizer-related data
 * 
 * The Context class is a class for storing and accessing dynamic Randomizer-related data.
 * Dynamic Data, in this context, refers to data that can be different for each randomizer seed,
 * i.e. item locations, hint contents and locations, settings, etc. This class is intended to be
 * used as a Singleton.
 */
namespace Rando{
class Context {
    public:
        static std::shared_ptr<Context> CreateInstance();
        static std::shared_ptr<Context> GetInstance();
        void AddHint(RandomizerHintKey hintId, Text text);
    private:
        static std::weak_ptr<Context> mContext;
        std::array<RandoHint, RH_MAX> hintTable;
};
}
#ifndef SHIP_INIT_HPP
#define SHIP_INIT_HPP

#ifdef __cplusplus

#include <vector>
#include <set>
#include <unordered_map>
#include <functional>

struct ShipInit {
    static std::unordered_map<std::string, std::vector<std::function<void()>>>& GetAll() {
        static std::unordered_map<std::string, std::vector<std::function<void()>>> shipInitFuncs;
        return shipInitFuncs;
    }

    static void InitAll() {
        ShipInit::Init("*");
    }

    static void Init(const std::string& path) {
        auto& shipInitFuncs = ShipInit::GetAll();
        for (const auto& initFunc : shipInitFuncs[path]) {
            initFunc();
        }
    }
};

/**
 * @brief Add a function to be conditionally registered/unregistered
 *
 * Usage (with condition):
 * 
 * `RegisterShipInitFunc foo(initFunc, {updatePathString1, updatePathString2, etc...})`
 * 
 * Usage (without condition):
 * 
 * `RegisterShipInitFunc foo(initFunc)`
 * 
 * @param initFunc The function to run on registration
 * @param updatePaths Strings to determine when a function should be registered/unregistered.
 * These can be CVar names (registered/unregistered when the CVar value changes), or
 * "IS_RANDO" (registered/unregistered OnLoadFile)
 * 
 */
struct RegisterShipInitFunc {
    RegisterShipInitFunc(std::function<void()> initFunc, const std::set<std::string>& updatePaths = {}) {
        auto& shipInitFuncs = ShipInit::GetAll();

        shipInitFuncs["*"].push_back(initFunc);

        for (const auto& path : updatePaths) {
            shipInitFuncs[path].push_back(initFunc);
        }
    }
};

#endif // __cplusplus

#endif // SHIP_INIT_HPP

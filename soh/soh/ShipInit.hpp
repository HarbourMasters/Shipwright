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
 * @brief Register a function to execute on boot and (optionally) in other situations
 *
 * @param initFunc The function to execute
 * @param updatePaths Strings to specify additional situations in which to execute the function
 *
 * ### Examples:
 *
 * #### Execute function `bar` on boot
 *
 * ```cpp
 * static RegisterShipInitFunc foo(bar);
 * ```
 *
 * #### Execute function `bar` on boot and when the CVar `baz` might have changed
 *
 * ```cpp
 * static RegisterShipInitFunc foo(bar, { "baz" });
 * ```
 *
 * #### Execute function `bar` on boot and when `IS_RANDO` might have changed
 *
 * ```cpp
 * static RegisterShipInitFunc foo(bar, { "IS_RANDO" });
 * ```
 *
 * ### Additional Information:
 *
 * To get a better sense of when your function will be executed
 * you can look for `ShipInit::Init` calls throughout the codebase
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

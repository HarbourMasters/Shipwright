#pragma once

#ifdef __cplusplus

#include <libultraship/libultraship.h>

#include <any>
#include <limits>
#include <stdint.h>
#include <unordered_map>
#include <vector>

/*
 * This class can attach additional data to pointers. It can only attach a single instance of each type of data.
 * Use the ObjectExtension::Register class to register a type to be used as an object extension.
 * An example usage is:
 *
 * struct MyData {
 *     s32 data = -1;
 * };
 * static ObjectExtension::Register<MyData> MyDataRegister;
 *
 * Then you can get with
 * ObjectExtension::GetInstance().Get<MyData>(ptr);
 * and set with
 * ObjectExtension::GetInstance().Set<MyData>(ptr, MyData{});
 * (or with the returned pointer from Get()).
 */
class ObjectExtension {
  public:
    using Id = uint32_t;

    static constexpr Id InvalidId = std::numeric_limits<Id>::max();

    // Registers type T to be used as an object extension
    template <typename T> class Register {
      public:
        Register() {
            Id = ObjectExtension::GetInstance().RegisterId();
        }

        static ObjectExtension::Id Id;
    };

    // Gets the singleton ObjectExtension instance
    static ObjectExtension& GetInstance();

    // Gets the data of type T associated with an object, or nullptr if no such data has been attached
    template <typename T> T* Get(const void* object) {
        assert(ObjectExtension::Register<T>::Id != InvalidId);
        if (object == nullptr) {
            return nullptr;
        }

        auto it = Data.find(std::make_pair(object, ObjectExtension::Register<T>::Id));
        if (it == Data.end()) {
            return nullptr;
        }

        return std::any_cast<T>(&(it->second));
    }

    // Sets the data of type T for an object. Data will be copied.
    template <typename T> void Set(const void* object, const T&& data) {
        assert(ObjectExtension::Register<T>::Id != InvalidId);
        if (object != nullptr) {
            Data[std::make_pair(object, ObjectExtension::Register<T>::Id)] = data;
        }
    }

    // Returns true if an object has data of type T associated with it
    template <typename T> bool Has(const void* object) {
        assert(ObjectExtension::Register<T>::Id != InvalidId);
        if (object == nullptr) {
            return false;
        }

        return Data.contains(std::make_pair(object, ObjectExtension::Register<T>::Id));
    }

    // Removes data of type T from an object
    template <typename T> void Remove(const void* object) {
        assert(ObjectExtension::Register<T>::Id != InvalidId);

        Data.erase(std::make_pair(object, ObjectExtension::Register<T>::Id));
    }

    // Removes all data from an object
    void Free(const void* object);

  private:
    ObjectExtension() = default;

    // Returns the next free object extension Id
    Id RegisterId();

    ObjectExtension::Id NextId = 0;

    struct KeyHash {
        std::size_t operator()(const std::pair<const void*, ObjectExtension::Id>& key) const {
            return std::hash<const void*>{}(key.first) ^ (std::hash<ObjectExtension::Id>{}(key.second) << 1);
        }
    };

    // Collection of all object extension data.
    std::unordered_map<std::pair<const void*, ObjectExtension::Id>, std::any, KeyHash> Data;
};

// Static template globals
template <typename T> ObjectExtension::Id ObjectExtension::Register<T>::Id = ObjectExtension::InvalidId;

#else // __cplusplus

void ObjectExtension_Free(const void* object);

#endif // __cplusplus

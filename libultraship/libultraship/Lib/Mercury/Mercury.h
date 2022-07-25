#pragma once
#include <any>
#include <vector>
#include <string>
#include "../nlohmann/json.hpp"

class Mercury {
protected:
    std::string path_;
public:
    explicit Mercury(std::string path);

    nlohmann::json vjson;
    nlohmann::json rjson;
    nlohmann::json nested(const std::string& key);
    static std::string formatNestedKey(const std::string& key);
    std::string getString(const std::string& key, const std::string& def = "");
    float getFloat(const std::string& key, float defValue = 0.0f);
    bool getBool(const std::string& key, bool defValue = false);
    int getInt(const std::string& key, int defValue = 0);
    bool contains(const std::string& key);
    template< typename T > std::vector<T> getArray(const std::string& key);
    void setString(const std::string& key, const std::string& value);
    void setFloat(const std::string& key, float value);
    void setBool(const std::string& key, bool value);
    void setInt(const std::string& key, int value);
    void setUInt(const std::string& key, uint32_t value);
    void erase(const std::string& key);
    void set(const std::string& key, std::any value);
    template< typename T > void setArray(const std::string& key, std::vector<T> array);

    void reload();
    void save() const;
    bool isNewInstance = false;
};

template< typename T >
std::vector<T> Mercury::getArray(const std::string& key) {
	if (nlohmann::json tmp = this->nested(key); tmp.is_array())
        return tmp.get<std::vector<T>>();
    return std::vector<T>();
};

template <typename T>
void Mercury::setArray(const std::string& key, std::vector<T> array) {
    this->vjson[formatNestedKey(key)] = nlohmann::json(array);
}

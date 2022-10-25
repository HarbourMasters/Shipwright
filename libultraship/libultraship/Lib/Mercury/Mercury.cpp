#include "Mercury.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <any>
#include <Utils/StringHelper.h>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::unordered_map<std::string, std::any> ramMap;

Mercury::Mercury(std::string path) : path_(std::move(path)) {
	this->reload();
}

std::string Mercury::formatNestedKey(const std::string& key) {
    std::vector<std::string> dots = StringHelper::Split(key, ".");

    std::string tmp;
    if (dots.size() > 1)
        for (const auto& dot : dots) {
            tmp += "/" + dot;
        }
    else
        tmp = "/" + dots[0];

    return tmp;
}

json Mercury::nested(const std::string& key) {
    std::vector<std::string> dots = StringHelper::Split(key, ".");
    if (!this->vjson.is_object())
        return this->vjson;
    json gjson = this->vjson.unflatten();

    if (dots.size() > 1) {
        for (auto& key : dots) {
            if (key == "*" || gjson.contains(key))
                gjson = gjson[key];
        }
        return gjson;
    }

    return gjson[key];
}

std::string Mercury::getString(const std::string& key, const std::string& def) {
    json n = this->nested(key);
    if (n.is_string() && !n.get<std::string>().empty())
        return n;
    return def;
}

float Mercury::getFloat(const std::string& key, float def) {
    json n = this->nested(key);
    if (n.is_number_float())
        return n;
    return def;
}

bool Mercury::getBool(const std::string& key, bool def) {
    json n = this->nested(key);
    if (n.is_boolean())
        return n;
    return def;
}

int Mercury::getInt(const std::string& key, int def) {
    json n = this->nested(key);
    if (n.is_number_integer())
        return n;
    return def;
}

bool Mercury::contains(const std::string& key) {
    return !this->nested(key).is_null();
}

void Mercury::setString(const std::string& key, const std::string& value) {
    this->vjson[formatNestedKey(key)] = value;
}

void Mercury::setFloat(const std::string& key, float value) {
    this->vjson[formatNestedKey(key)] = value;
}

void Mercury::setBool(const std::string& key, bool value) {
    this->vjson[formatNestedKey(key)] = value;
}

void Mercury::setInt(const std::string& key, int value) {
    this->vjson[formatNestedKey(key)] = value;
}

void Mercury::setUInt(const std::string& key, uint32_t value) {
    this->vjson[formatNestedKey(key)] = value;
}

void Mercury::erase(const std::string& key) {
    this->vjson.erase(formatNestedKey(key));
}

void Mercury::reload() {
    if (this->path_ == "None" || !fs::exists(this->path_) || !fs::is_regular_file(this->path_)) {
        this->isNewInstance = true;
        this->vjson = json::object();
        return;
    }
    std::ifstream ifs(this->path_);

    try {
        this->rjson = json::parse(ifs);
        this->vjson = this->rjson.flatten();
    }
    catch (...) {
        this->vjson = json::object();
    }
}

void Mercury::save() const {
    std::ofstream file(this->path_);
    file << this->vjson.unflatten().dump(4);
}

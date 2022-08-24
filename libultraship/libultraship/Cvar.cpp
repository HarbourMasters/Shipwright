#include "Cvar.h"
#include <map>
#include <string.h>
#include <functional>
#include <memory>
#include <utility>
#include <Utils/File.h>
#include "Window.h"

std::map<std::string, std::unique_ptr<CVar>, std::less<>> cvars;

CVar* CVar_Get(const char* name) {
    auto it = cvars.find(name);
    return (it != cvars.end()) ? it->second.get() : nullptr;
}

extern "C" int32_t CVar_GetS32(const char* name, int32_t defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::S32)
            return cvar->value.valueS32;
    }

    return defaultValue;
}

extern "C" float CVar_GetFloat(const char* name, float defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::Float)
            return cvar->value.valueFloat;
    }

    return defaultValue;
}

extern "C" const char* CVar_GetString(const char* name, const char* defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar) {
        if (cvar->type == CVarType::String)
            return cvar->value.valueStr;
    }

    return defaultValue;
}

extern "C" Color_RGB8 CVar_GetRGB(const char* name, Color_RGB8 defaultValue)
{
    Color_RGBA8 defaultValueRGBA;
    defaultValueRGBA.r = defaultValue.r;
    defaultValueRGBA.g = defaultValue.g;
    defaultValueRGBA.b = defaultValue.b;
    defaultValueRGBA.a = 255;

    Color_RGBA8 cvarGet = CVar_GetRGBA(name, defaultValueRGBA);
    Color_RGB8 result;

    result.r = cvarGet.r;
    result.g = cvarGet.g;
    result.b = cvarGet.b;

    return result;
}

extern "C" Color_RGBA8 CVar_GetRGBA(const char* name, Color_RGBA8 defaultValue) {
    CVar* cvar = CVar_Get(name);

    if (cvar != nullptr) {
        if (cvar->type == CVarType::RGBA)
            return cvar->value.valueRGBA;
    }

    return defaultValue;
}

extern "C" void CVar_SetRGBA(const char* name, Color_RGBA8 value)
{
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }

    cvar->type = CVarType::RGBA;
    cvar->value.valueRGBA = value;
}

extern "C" void CVar_SetS32(const char* name, int32_t value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::S32;
    cvar->value.valueS32 = value;
}

extern "C" void CVar_SetFloat(const char* name, float value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::Float;
    cvar->value.valueFloat = value;
}

extern "C" void CVar_SetString(const char* name, const char* value) {
    auto& cvar = cvars[name];
    if (!cvar) {
        cvar = std::make_unique<CVar>();
    }
    cvar->type = CVarType::String;
#ifdef _MSC_VER
    cvar->value.valueStr = _strdup(value);
#else
    cvar->value.valueStr = strdup(value);
#endif
}

extern "C" void CVar_RegisterRGBA(const char* name, Color_RGBA8 defaultValue) {
    if (!CVar_Get(name))
        CVar_SetRGBA(name, defaultValue);
}

extern "C" void CVar_RegisterS32(const char* name, int32_t defaultValue) {
    if (!CVar_Get(name))
        CVar_SetS32(name, defaultValue);
}

extern "C" void CVar_RegisterFloat(const char* name, float defaultValue) {
    if (!CVar_Get(name))
        CVar_SetFloat(name, defaultValue);
}

extern "C" void CVar_RegisterString(const char* name, const char* defaultValue) {
    if (!CVar_Get(name))
        CVar_SetString(name, defaultValue);
}

template <typename Numeric> bool is_number(const std::string& s) {
    Numeric n;
    return ((std::istringstream(s) >> n >> std::ws).eof());
}

void CVar_LoadLegacy() {
    auto cvarsConfig = Ship::Window::GetPathRelativeToAppDirectory("cvars.cfg");
    if (File::Exists(cvarsConfig)) {
        const auto lines = File::ReadAllLines(cvarsConfig);

        for (const std::string& line : lines) {
            std::vector<std::string> cfg = StringHelper::Split(line, " = ");
            if (line.empty()) continue;
            if (cfg.size() < 2) continue;

            if (cfg[1].find("\"") == std::string::npos && (cfg[1].find("#") != std::string::npos))
            {
                std::string value(cfg[1]);
                value.erase(std::remove_if(value.begin(), value.end(), [](char c) { return c == '#'; }), value.end());
                auto splitTest = StringHelper::Split(value, "\r")[0];

                uint32_t val = std::stoul(splitTest, nullptr, 16);
                Color_RGBA8 clr;
                clr.r = val >> 24;
                clr.g = val >> 16;
                clr.b = val >> 8;
                clr.a = val & 0xFF;
                CVar_SetRGBA(cfg[0].c_str(), clr);
            }

            if (cfg[1].find("\"") != std::string::npos) {
                std::string value(cfg[1]);
                value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
#ifdef _MSC_VER
                CVar_SetString(cfg[0].c_str(), _strdup(value.c_str()));
#else
                CVar_SetString(cfg[0].c_str(), strdup(value.c_str()));
#endif
            }
            if (is_number<float>(cfg[1])) {
                CVar_SetFloat(cfg[0].c_str(), std::stof(cfg[1]));
            }
            if (is_number<int>(cfg[1])) {
                CVar_SetS32(cfg[0].c_str(), std::stoi(cfg[1]));
            }
        }

        fs::remove(cvarsConfig);
    }
}


extern "C" void CVar_Load() {
    std::shared_ptr<Mercury> pConf = Ship::Window::GetInstance()->GetConfig();
    pConf->reload();

    for (const auto& item : pConf->rjson["CVars"].items()) {
        auto value = item.value();
        switch (value.type()) {
        case nlohmann::detail::value_t::array:
            break;
        case nlohmann::detail::value_t::object:
            if (value["Type"].get<std::string>() == mercuryRGBAObjectType) {
                Color_RGBA8 clr;
                clr.r = value["R"].get<uint8_t>();
                clr.g = value["G"].get<uint8_t>();
                clr.b = value["B"].get<uint8_t>();
                clr.a = value["A"].get<uint8_t>();
                CVar_SetRGBA(item.key().c_str(), clr);
            }

            break;
        case nlohmann::detail::value_t::string:
            CVar_SetString(item.key().c_str(), value.get<std::string>().c_str());
            break;
        case nlohmann::detail::value_t::boolean:
            CVar_SetS32(item.key().c_str(), value.get<bool>());
            break;
        case nlohmann::detail::value_t::number_unsigned:
        case nlohmann::detail::value_t::number_integer:
            CVar_SetS32(item.key().c_str(), value.get<int>());
            break;
        case nlohmann::detail::value_t::number_float:
            CVar_SetFloat(item.key().c_str(), value.get<float>());
            break;
        default:;
        }
        if (item.key() == "gOpenMenuBar") {
            int bp = 0;
        }
    }

    CVar_LoadLegacy();
}

extern "C" void CVar_Save()
{
    std::shared_ptr<Mercury> pConf = Ship::Window::GetInstance()->GetConfig();

    for (const auto& cvar : cvars) {
        const std::string key = StringHelper::Sprintf("CVars.%s", cvar.first.c_str());

        if (cvar.second->type == CVarType::String && cvar.second->value.valueStr != nullptr)
            pConf->setString(key, std::string(cvar.second->value.valueStr));
        else if (cvar.second->type == CVarType::S32)
            pConf->setInt(key, cvar.second->value.valueS32);
        else if (cvar.second->type == CVarType::Float)
            pConf->setFloat(key, cvar.second->value.valueFloat);
        else if (cvar.second->type == CVarType::RGBA)
        {
            auto keyStr = key.c_str();
            Color_RGBA8 clr = cvar.second->value.valueRGBA;
            pConf->setUInt(StringHelper::Sprintf("%s.R", keyStr), clr.r);
            pConf->setUInt(StringHelper::Sprintf("%s.G", keyStr), clr.r);
            pConf->setUInt(StringHelper::Sprintf("%s.B", keyStr), clr.r);
            pConf->setUInt(StringHelper::Sprintf("%s.A", keyStr), clr.r);
            pConf->setString(StringHelper::Sprintf("%s.Type", keyStr), mercuryRGBAObjectType);
        }
    }

    pConf->save();
}

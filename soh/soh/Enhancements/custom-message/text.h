#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <stdint.h>
#include <nlohmann/json_fwd.hpp>

class Text {
  public:
    Text();
    Text(std::string english_, std::string french_, std::string german_);
    Text(std::string english_, std::string french_, std::string german_, std::string spanish_);
    explicit Text(std::string english_);

    const std::string& GetEnglish() const;
    const std::string& GetFrench() const;
    const std::string& GetGerman() const;
    const std::string& GetSpanish() const;
    const std::string& GetForLanguage(uint8_t language) const;

    const std::string& GetForLocale(const std::string& locale) const;
    void SetForLocale(const std::string& locale, std::string text);
    const std::map<std::string, std::string>& GetLocales() const;

    Text operator+(const Text& right) const;
    Text operator+(const std::string& right) const;

    bool operator==(const Text& right) const;
    bool operator==(const std::string& right) const;
    bool operator!=(const Text& right) const;

    void Replace(const std::string& oldStr, const std::string& newStr);
    void Replace(const std::string& oldStr, const Text& newText);

    void ReplaceRandomVowel(uint64_t* randState = nullptr);

    void DuplicateRandomLetter(uint64_t* randState = nullptr);

    void FillMissingTranslations();

    std::string english = "";
    std::string french = "";
    std::string german = "";
    std::string spanish = "";

  private:
    std::map<std::string, std::string> locales;
    std::vector<std::reference_wrapper<std::string>> AllTexts();
};

// Keep legacy language fields readable by older saves; additional locales are
// stored alongside them. Reading also accepts old single-language seed strings.
void to_json(nlohmann::json& json, const Text& text);
void from_json(const nlohmann::json& json, Text& text);

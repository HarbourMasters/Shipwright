#include "text.h"
#include "soh/ShipUtils.h"
#include <functional>

Text::Text() = default;

Text::Text(std::string english_, std::string french_, std::string german_)
    : english(std::move(english_)), french(std::move(french_)), german(std::move(german_)), spanish("") {
    spanish = english;
}

Text::Text(std::string english_, std::string french_, std::string german_, std::string spanish_)
    : english(std::move(english_)), french(std::move(french_)), german(std::move(german_)),
      spanish(std::move(spanish_)) {
}

Text::Text(std::string english_) : english(std::move(english_)), french(""), german(""), spanish("") {
    french = spanish = german = english;
}

const std::string& Text::GetEnglish() const {
    return english;
}

const std::string& Text::GetFrench() const {
    return french.length() > 0 ? french : english;
}

const std::string& Text::GetGerman() const {
    return german.length() > 0 ? german : english;
}

const std::string& Text::GetSpanish() const {
    return spanish.length() > 0 ? spanish : english;
}

const std::string& Text::GetForLanguage(uint8_t language) const {
    switch (language) {
        case 0:
            return GetEnglish();
        case 2:
            return GetFrench();
        case 1:
            return GetGerman();
        default:
            return GetEnglish();
    }
}

Text Text::operator+(const Text& right) const {
    return Text{
        english + right.GetEnglish(),
        french + right.GetFrench(),
        german + right.GetGerman(),
        spanish + right.GetSpanish(),
    };
}

Text Text::operator+(const std::string& right) const {
    return Text{
        english + right,
        french + right,
        german + right,
        spanish + right,
    };
}

bool Text::operator==(const Text& right) const {
    return english == right.english;
}

bool Text::operator==(const std::string& right) const {
    return english == right || french == right || german == right || spanish == right;
}

bool Text::operator!=(const Text& right) const {
    return !operator==(right);
}

static void replaceAll(std::string& target, const std::string& oldStr, const std::string& replacement) {
    size_t position = target.find(oldStr);
    while (position != std::string::npos) {
        target.replace(position, oldStr.length(), replacement);
        position = target.find(oldStr);
    }
}

void Text::Replace(const std::string& oldStr, const std::string& newStr) {
    for (std::string& str : { std::ref(english), std::ref(french), std::ref(german), std::ref(spanish) }) {
        replaceAll(str, oldStr, newStr);
    }
}

void Text::Replace(const std::string& oldStr, const Text& newText) {
    replaceAll(english, oldStr, newText.GetEnglish());
    replaceAll(french, oldStr, newText.GetFrench());
    replaceAll(german, oldStr, newText.GetGerman());
    replaceAll(spanish, oldStr, newText.GetSpanish());
}

static bool isAsciiVowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'A' || c == 'E' || c == 'I' || c == 'O' ||
           c == 'U';
}

static bool isAsciiLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Uniformly picks one position where pred holds, in a single pass. npos if nothing matches.
static size_t randomMatch(const std::string& target, bool (*pred)(char), uint64_t* randState) {
    uint32_t seen = 0;
    size_t pos = std::string::npos;

    for (size_t i = 0; i < target.size(); ++i) {
        if (pred(target[i]) && ShipUtils::Random(0, ++seen, randState) == 0) {
            pos = i;
        }
    }

    return pos;
}

static void replaceRandomVowel(std::string& target, uint64_t* randState) {
    size_t pos = randomMatch(target, isAsciiVowel, randState);

    if (pos == std::string::npos) {
        return;
    }

    static constexpr char vowels[] = { 'a', 'e', 'i', 'o', 'u' };
    char newVowel = ShipUtils::RandomElement(vowels, randState);

    target[pos] = (target[pos] >= 'A' && target[pos] <= 'Z') ? newVowel - ('a' - 'A') : newVowel;
}

void Text::ReplaceRandomVowel(uint64_t* randState) {
    for (std::string& str : { std::ref(english), std::ref(french), std::ref(german), std::ref(spanish) }) {
        replaceRandomVowel(str, randState);
    }
}

static void duplicateRandomLetter(std::string& target, uint64_t* randState) {
    size_t pos = randomMatch(target, isAsciiLetter, randState);

    if (pos != std::string::npos) {
        target.insert(pos + 1, 1, target[pos]);
    }
}

void Text::DuplicateRandomLetter(uint64_t* randState) {
    for (std::string& str : { std::ref(english), std::ref(french), std::ref(german), std::ref(spanish) }) {
        duplicateRandomLetter(str, randState);
    }
}

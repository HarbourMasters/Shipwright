#pragma once

#include <string>
#include "z64.h"

#define PLURAL 0
#define SINGULAR 1

class Text {
public:
    Text() = default;
    Text(std::string english_, std::string french_, std::string spanish_)
      : english(std::move(english_)),
        french(std::move(french_)),
        spanish(std::move(spanish_)),
        german(std::move("")) {}
    Text(std::string english_, std::string french_, std::string spanish_, std::string german_)
      : english(std::move(english_)),
        french(std::move(french_)),
        spanish(std::move(spanish_)),
        german(std::move(german_)) {}
    Text(std::string english_) : english(std::move(english_)), french(std::move("")), spanish(std::move("")), german(std::move("")) {}

    const std::string& GetEnglish() const {
        return english;
    }

    const std::string& GetFrench() const {
        if (french.length() > 0) {
            return french;
        }
        return english;
    }

    const std::string& GetSpanish() const {
        if (spanish.length() > 0) {
            return spanish;
        }
        return english;
    }

    const std::string& GetGerman() const {
        if (german.length() > 0) {
            return german;
        }
        return english;
    }

    const std::string& GetForLanguage(uint8_t language) const {
        switch (language) {
            case LANGUAGE_ENG:
                return GetEnglish();
            case LANGUAGE_FRA:
                return GetFrench();
            case LANGUAGE_GER:
                return GetGerman();
            default:
                return GetEnglish();
        }
    }

    Text operator+ (const Text& right) const {
        return Text{english + right.GetEnglish(), french + right.GetFrench(), spanish + right.GetSpanish(),
            german + right.GetGerman()};
    }

    Text operator+ (const std::string& right) const {
        return Text{english + right, french + right, spanish + right, german + right};
    }

    bool operator==(const Text& right) const {
        return english == right.english;
    }

    bool operator==(const std::string& right) const {
        return english == right || french == right || spanish == right || german == right;
    }

    bool operator!=(const Text& right) const {
        return !operator==(right);
    }

    void Replace(std::string oldStr, std::string newStr) {

        for (std::string* str : {&english, &french, &spanish, &german}) {
            size_t position = str->find(oldStr);
            while (position != std::string::npos) {
              str->replace(position, oldStr.length(), newStr);
              position = str->find(oldStr);
            }
        }
    }

    // Convert first char to upper case
    Text Capitalize(void) const {
        Text cap = *this + "";
        for (std::string* str : {&cap.english, &cap.french, &cap.spanish, &cap.german}) {
            (*str)[0] = std::toupper((*str)[0]);
        }
        return cap;
    }

    //find the appropriate bars that separate singular from plural
    void SetForm(int form) {
        for (std::string* str : {&english, &french, &spanish, &german}) {

            size_t firstBar = str->find('|');
            if (firstBar != std::string::npos) {

                size_t secondBar = str->find('|', firstBar + 1);
                if (secondBar != std::string::npos) {

                    size_t thirdBar = str->find('|', secondBar + 1);
                    if (thirdBar != std::string::npos) {

                        if (form == SINGULAR) {
                            str->erase(secondBar, thirdBar - secondBar);
                        } else {
                            str->erase(firstBar, secondBar - firstBar);
                        }
                    }
                }
            }
        }
        //remove the remaining bar
        this->Replace("|", "");
    }

    std::string english = "";
    std::string french = "";
    std::string spanish = "";
    std::string german = "";
};

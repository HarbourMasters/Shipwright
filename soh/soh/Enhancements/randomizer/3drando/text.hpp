#pragma once

#include <string>

#define PLURAL 0
#define SINGULAR 1

class Text {
public:
    Text() = default;
    Text(std::string english_, std::string french_, std::string spanish_)
      : english(std::move(english_)),
        french(std::move(french_)),
        spanish(std::move(spanish_)) {}

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

    Text operator+ (const Text& right) const {
        return Text{english + right.GetEnglish(), french + right.GetFrench(), spanish + right.GetSpanish()};
    }

    Text operator+ (const std::string& right) const {
        return Text{english + right, french + right, spanish + right};
    }

    bool operator==(const Text& right) const {
        return english == right.english;
    }

    bool operator!=(const Text& right) const {
        return !operator==(right);
    }

    void Replace(std::string oldStr, std::string newStr) {

        for (std::string* str : {&english, &french, &spanish}) {
            size_t position = str->find(oldStr);
            while (position != std::string::npos) {
              str->replace(position, oldStr.length(), newStr);
              position = str->find(oldStr);
            }
        }
    }

    //find the appropriate bars that separate singular from plural
    void SetForm(int form) {
        for (std::string* str : {&english, &french, &spanish}) {

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
};

#pragma once

#include "3drando/text.hpp"

class RandoHint {
    public:
    RandoHint();
    RandoHint(Text text_);
    const Text& GetText() const;

    private:
    Text text;
};
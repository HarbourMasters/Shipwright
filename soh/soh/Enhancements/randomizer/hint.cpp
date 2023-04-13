#include "hint.h"

RandoHint::RandoHint() {}
RandoHint::RandoHint(Text text_): text(std::move(text_)) {}

const Text& RandoHint::GetText() const {
    return text;
}
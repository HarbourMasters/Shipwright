#pragma once

#include <string>
#include "CustomMessageManager.h"
#include "GameText.h"

namespace CustomMessages {

// Separate from Lang's UI catalog: game text follows Language, never UI Language.
void LoadCatalog();

// Catalog-backed construction preserves the existing language slots and
// formatting metadata. Message bodies and names live only in game-lang JSON.
// Complete messages also inherit locale-specific message_layout alignment rules.
CustomMessage CreateMessage(const std::string& key, TextBoxType type = TEXTBOX_TYPE_BLACK,
                            TextBoxPosition position = TEXTBOX_POS_BOTTOM);
CustomMessage CreateMessage(const std::string& key, std::vector<std::string> colors, std::vector<bool> capital = {},
                            TextBoxType type = TEXTBOX_TYPE_BLACK, TextBoxPosition position = TEXTBOX_POS_BOTTOM);
Text CreateText(const std::string& key);
// Plain UTF-8 labels for PC UI; never pass these directly to the game decoder.
std::string GetTextForUI(const std::string& key, const std::string& fallback);

// The result contains unformatted text, just like a CustomMessage constructor.
// Continue to call Replace/AutoFormat/Format at the existing message call site.
CustomMessage GetMessage(const std::string& key, TextBoxType type = TEXTBOX_TYPE_BLACK,
                         TextBoxPosition position = TEXTBOX_POS_BOTTOM);
CustomMessage GetMessage(const std::string& key, const std::string& fallbackKey, TextBoxType type = TEXTBOX_TYPE_BLACK,
                         TextBoxPosition position = TEXTBOX_POS_BOTTOM);

// Optional per-language overrides, e.g. a localized item name. Missing overrides
// preserve that language's existing text, including its formatting metadata.
CustomMessage GetMessageOrDefault(const std::string& key, const CustomMessage& defaultMessage);

} // namespace CustomMessages

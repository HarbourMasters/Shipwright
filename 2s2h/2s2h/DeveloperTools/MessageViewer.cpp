#include "MessageViewer.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "BenPort.h"

#include <message_data_static.h>
#include <sstream>

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"
}

using namespace UIWidgets;

static std::string ParseEscapeSequences(const std::string& input) {
    std::string output;
    for (size_t i = 0; i < input.length(); ++i) {
        if (i + 3 < input.length() && input[i] == '\\' && input[i + 1] == 'x') {
            char hex[3] = { input[i + 2], input[i + 3], '\0' };
            char* endptr;
            unsigned char byte = (unsigned char)strtol(hex, &endptr, 16);
            if (endptr == hex + 2) { // Successfully parsed 2 hex digits
                output += (char)byte;
                i += 3; // Skip \xXX
            } else {
                output += input[i]; // Not valid hex, keep as-is
            }
        } else {
            output += input[i];
        }
    }
    return output;
}

static bool ValidateTextIdExists(uint16_t textId) {
    if (gPlayState == nullptr) {
        return false;
    }

    MessageTableEntry* msgEntry = gPlayState->msgCtx.messageTableNES;
    if (msgEntry == nullptr) {
        return false;
    }

    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == textId) {
            return true;
        }
        msgEntry++;
    }
    return false;
}

static void FormatByteAsEscapeSequence(std::ostringstream& stream, uint8_t value) {
    stream << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)value;
}

void MessageViewerWindow::InitElement() {
    mTextIdBuf = static_cast<char*>(calloc(MAX_STRING_SIZE, sizeof(char)));
    mCustomMessageBuf = static_cast<char*>(calloc(MAX_STRING_SIZE, sizeof(char)));
}

MessageViewerWindow::~MessageViewerWindow() {
    free(mTextIdBuf);
    free(mCustomMessageBuf);
}

void MessageViewerWindow::DrawElement() {
    // Vanilla Message Viewer Section
    ImGui::SeparatorText("Vanilla Message Viewer");

    PushStyleInput(THEME_COLOR);
    switch (mTextIdBase) {
        case DECIMAL:
            ImGui::InputText("##TextID", mTextIdBuf, MAX_STRING_SIZE, ImGuiInputTextFlags_CharsDecimal);
            break;
        case HEXADECIMAL:
        default:
            ImGui::InputText("##TextID", mTextIdBuf, MAX_STRING_SIZE, ImGuiInputTextFlags_CharsHexadecimal);
            break;
    }

    // Draw placeholder overlay immediately if empty
    if (strlen(mTextIdBuf) == 0) {
        ImVec2 inputMin = ImGui::GetItemRectMin();
        ImVec2 textPos =
            ImVec2(inputMin.x + ImGui::GetStyle().FramePadding.x + 4.0f, inputMin.y + ImGui::GetStyle().FramePadding.y);
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddText(textPos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.4f)), "TextID");
    }
    PopStyleInput();

    // Draw radio buttons on same line as input
    ImGui::SameLine();
    PushStyleCheckbox(THEME_COLOR);
    if (ImGui::RadioButton("Hex", &mTextIdBase, HEXADECIMAL)) {
        memset(mTextIdBuf, 0, sizeof(char) * MAX_STRING_SIZE);
    }
    Tooltip("Hexadecimal Text ID of the message to load. Hexadecimal digits only (0-9/A-F).");
    ImGui::SameLine();
    if (ImGui::RadioButton("Dec", &mTextIdBase, DECIMAL)) {
        memset(mTextIdBuf, 0, sizeof(char) * MAX_STRING_SIZE);
    }
    Tooltip("Decimal Text ID of the message to load. Decimal digits only (0-9).");
    PopStyleCheckbox();

    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Display Message##ExistingMessage")) {
        mDisplayExistingMessageClicked = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Message##ExistingMessage")) {
        mLoadMessageClicked = true;
    }
    PopStyleButton();

    // Custom Message Builder Section
    ImGui::SeparatorText("Custom Message Builder");

    PushStyleInput(THEME_COLOR);
    ImGui::InputTextMultiline("##CustomMessage", mCustomMessageBuf, MAX_STRING_SIZE, ImVec2(-1, 100));
    PopStyleInput();

    Tooltip(
        "Enter text to preview in-game. Supports color codes (%r, %w, %y, %g, %b, %p) and escape sequences (\\xXX).\n"
        "Use 'Load Message' to inspect vanilla message format. Newlines are stripped from simple text.");

    PushStyleButton(THEME_COLOR);
    if (ImGui::Button("Display Message##CustomMessage")) {
        mDisplayCustomMessageClicked = true;
    }
    PopStyleButton();
}

void MessageViewerWindow::UpdateElement() {
    if (mDisplayExistingMessageClicked) {
        if (ParseTextIdFromBuffer(mTextId)) {
            DisplayExistingMessage();
        }
        mDisplayExistingMessageClicked = false;
    }
    if (mLoadMessageClicked) {
        if (ParseTextIdFromBuffer(mTextId)) {
            LoadMessageToEditor();
        }
        mLoadMessageClicked = false;
    }
    if (mDisplayCustomMessageClicked) {
        mCustomMessageString = std::string(mCustomMessageBuf);

        // Check if custom message buffer is empty
        if (mCustomMessageString.empty()) {
            mDisplayCustomMessageClicked = false;
            return;
        }

        std::erase(mCustomMessageString, '\n');
        DisplayCustomMessage();
        mDisplayCustomMessageClicked = false;
    }
}

void MessageViewerWindow::DisplayExistingMessage() const {
    MessageDebug_StartTextBox("", mTextId, LANGUAGE_ENG);
}

void MessageViewerWindow::DisplayCustomMessage() const {
    MessageDebug_DisplayCustomMessage(mCustomMessageString.c_str());
}

void MessageViewerWindow::LoadMessageToEditor() {
    if (!ValidateTextIdExists(mTextId)) {
        return;
    }

    // Load the vanilla message entry
    CustomMessage::Entry entry = CustomMessage::LoadVanillaMessageTableEntry(mTextId);

    // Message header format (11 bytes) - see: https://wiki.cloudmodding.com/mm/Text_Format
    // Bytes 0-1 are stored as raw bytes in segment[] (not packed)
    // Game code reads bytes 0-1 as a 16-bit value and extracts fields using bit masks (see Message_OpenText() in
    // z_message.c:3396-3402) Message_DecodeHeader() reads bytes 2-10 (icon, nextTextId, costs, padding) - see
    // z_message.c:2131-2173 We store/load raw bytes directly to match CustomMessage system format

    std::ostringstream rawMessage;

    // Format header bytes - write raw bytes as stored in segment (matching CustomMessage::LoadCustomMessageIntoFont)
    FormatByteAsEscapeSequence(rawMessage, entry.textboxType);
    FormatByteAsEscapeSequence(rawMessage, entry.textboxYPos);
    FormatByteAsEscapeSequence(rawMessage, entry.icon);
    FormatByteAsEscapeSequence(rawMessage, (entry.nextMessageID & 0xFF00) >> 8);
    FormatByteAsEscapeSequence(rawMessage, entry.nextMessageID & 0x00FF);
    FormatByteAsEscapeSequence(rawMessage, (entry.firstItemCost & 0xFF00) >> 8);
    FormatByteAsEscapeSequence(rawMessage, entry.firstItemCost & 0x00FF);
    FormatByteAsEscapeSequence(rawMessage, (entry.secondItemCost & 0xFF00) >> 8);
    FormatByteAsEscapeSequence(rawMessage, entry.secondItemCost & 0x00FF);
    rawMessage << "\\xff\\xff"; // Padding bytes

    // Format message content bytes
    for (size_t i = 0; i < entry.msg.length(); ++i) {
        unsigned char byte = entry.msg[i];
        if (byte >= 0x20 && byte <= 0x7E) {
            // Printable ASCII - add as-is
            rawMessage << (char)byte;
        } else {
            // Control code - format as \xXX
            FormatByteAsEscapeSequence(rawMessage, byte);
        }
    }

    // Copy the formatted string to the custom message buffer
    std::string formattedMessage = rawMessage.str();
    if (formattedMessage.size() >= MAX_STRING_SIZE) {
        formattedMessage = formattedMessage.substr(0, MAX_STRING_SIZE - 1);
    }
    strncpy(mCustomMessageBuf, formattedMessage.c_str(), MAX_STRING_SIZE - 1);
    mCustomMessageBuf[MAX_STRING_SIZE - 1] = '\0';
}

bool MessageViewerWindow::ParseTextIdFromBuffer(uint16_t& outTextId) {
    if (strlen(mTextIdBuf) == 0) {
        return false;
    }
    try {
        switch (mTextIdBase) {
            case DECIMAL:
                outTextId = std::stoi(std::string(mTextIdBuf), nullptr, 10);
                break;
            case HEXADECIMAL:
            default:
                outTextId = std::stoi(std::string(mTextIdBuf), nullptr, 16);
                break;
        }
        return true;
    } catch (const std::exception&) { return false; }
}

void MessageDebug_StartTextBox(const char* tableId, uint16_t textId, uint8_t language) {
    if (!ValidateTextIdExists(textId)) {
        return;
    }

    const auto player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    Message_StartTextbox(gPlayState, textId, &player->actor);
}

void MessageDebug_DisplayCustomMessage(const char* customMessage) {
    if (gPlayState == nullptr) {
        return;
    }

    const auto player = GET_PLAYER(gPlayState);
    if (player == nullptr) {
        return;
    }

    // Parse escape sequences in the input
    std::string processedMessage = ParseEscapeSequences(customMessage);

    // Create a custom message entry
    CustomMessage::Entry entry;

    // Check if message starts with header (11+ bytes)
    // Header detection: verify processed message has at least MESSAGE_HEADER_SIZE bytes
    // and first byte is in control character range (< 0x20), indicating it's likely a header byte
    if (processedMessage.length() >= MESSAGE_HEADER_SIZE && (unsigned char)processedMessage[0] < 0x20) {

        // Parse header bytes - read raw bytes as stored (matching CustomMessage::LoadVanillaMessageTableEntry)
        // Game code unpacks these bytes using bit masks, but we store/load raw bytes
        entry.textboxType = processedMessage[0];
        entry.textboxYPos = processedMessage[1];
        entry.icon = (unsigned char)processedMessage[2];
        entry.nextMessageID = ((unsigned char)processedMessage[3] << 8) | (unsigned char)processedMessage[4];
        entry.firstItemCost = ((unsigned char)processedMessage[5] << 8) | (unsigned char)processedMessage[6];
        entry.secondItemCost = ((unsigned char)processedMessage[7] << 8) | (unsigned char)processedMessage[8];

        // Skip header, use remaining as message content
        entry.msg = processedMessage.substr(MESSAGE_HEADER_SIZE);
        entry.autoFormat = false; // Already formatted

    } else {
        // No header - use defaults for user-written messages
        entry.textboxType = 0;
        entry.textboxYPos = 0x30;
        entry.icon = 0xFE;
        entry.nextMessageID = 0xFFFF;
        entry.firstItemCost = 0xFFFF;
        entry.secondItemCost = 0xFFFF;
        entry.msg = processedMessage;
        entry.autoFormat = true;
    }

    // Set the active custom message and display it
    CustomMessage::StartTextbox(entry.msg, entry);
}

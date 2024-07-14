#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/type/Text.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<Ship::IResource> ResourceFactoryBinaryTextV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto text = std::make_shared<Text>(file->InitData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    uint32_t msgCount = reader->ReadUInt32();
    text->messages.reserve(msgCount);

    for (uint32_t i = 0; i < msgCount; i++) {
        MessageEntry entry;
        entry.id = reader->ReadUInt16();
        entry.textboxType = reader->ReadUByte();
        entry.textboxYPos = reader->ReadUByte();
        entry.msg = reader->ReadString();

        text->messages.push_back(entry);
    }

    return text;
}

std::shared_ptr<Ship::IResource> ResourceFactoryXMLTextV0::ReadResource(std::shared_ptr<Ship::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto txt = std::make_shared<Text>(file->InitData);
    auto child = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement()->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            MessageEntry entry;
            entry.id = child->IntAttribute("ID");
            entry.textboxType = child->IntAttribute("TextboxType");
            entry.textboxYPos = child->IntAttribute("TextboxYPos");
            entry.msg = child->Attribute("Message");
            entry.msg += "\x2";

            txt->messages.push_back(entry);
            int bp = 0;
        }

        child = child->NextSiblingElement();
    }

    return txt;
}
} // namespace SOH

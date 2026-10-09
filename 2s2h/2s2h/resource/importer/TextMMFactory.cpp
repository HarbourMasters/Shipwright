#include "2s2h/resource/importer/TextMMFactory.h"
#include "2s2h/resource/type/TextMM.h"
#include <tinyxml2.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextMMV0::ReadResource(std::shared_ptr<Ship::File> file,
                                            std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto text = std::make_shared<TextMM>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    const uint32_t msgCount = reader->ReadUInt32();
    text->messages.reserve(msgCount);

    for (uint32_t i = 0; i < msgCount; i++) {
        MessageEntryMM entry;
        entry.id = reader->ReadUInt16();
        entry.textboxType = reader->ReadUByte();
        entry.textboxYPos = reader->ReadUByte();
        // BENTODO: the new ZAPD reads and exports this as an int16 for JP but nothing currently uses that and the game
        // expects an int8. Use this for now.
        entry.icon = (int8_t)reader->ReadUInt16();
        entry.nextMessageID = reader->ReadUInt16();
        entry.firstItemCost = reader->ReadUInt16();
        entry.secondItemCost = reader->ReadUInt16();
        entry.msg = reader->ReadString();

        text->messages.push_back(entry);
    }

    return text;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryXMLTextMMV0::ReadResource(std::shared_ptr<Ship::File> file,
                                         std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto text = std::make_shared<TextMM>(initData);
    auto reader = std::get<std::shared_ptr<tinyxml2::XMLDocument>>(file->Reader)->FirstChildElement();

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            MessageEntryMM entry;
            entry.id = child->IntAttribute("ID");
            entry.textboxType = child->IntAttribute("TextboxType");
            entry.textboxYPos = child->IntAttribute("TextboxYPos");

            // BENTODO: MM Unique Fields

            entry.msg = child->Attribute("Message");
            entry.msg += "\x2";

            text->messages.push_back(entry);
            int bp = 0;
        }

        child = child->NextSiblingElement();
    }

    return text;
}
} // namespace SOH

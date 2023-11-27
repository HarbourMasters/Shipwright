#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/type/Text.h"
#include "spdlog/spdlog.h"

namespace LUS {
std::shared_ptr<IResource>
TextFactory::ReadResource(std::shared_ptr<ResourceInitData> initData, std::shared_ptr<BinaryReader> reader) {
    auto resource = std::make_shared<Text>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
	    factory = std::make_shared<TextFactoryV0>();
	    break;
        default:
            // VERSION NOT SUPPORTED
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Text with version {}", resource->GetInitData()->ResourceVersion);
	return nullptr;
    }

    factory->ParseFileBinary(reader, resource);

    return resource;
}

std::shared_ptr<IResource>
TextFactory::ReadResourceXML(std::shared_ptr<ResourceInitData> initData, tinyxml2::XMLElement *reader) {
    auto resource = std::make_shared<Text>(initData);
    std::shared_ptr<ResourceVersionFactory> factory = nullptr;

    switch (resource->GetInitData()->ResourceVersion) {
        case 0:
            factory = std::make_shared<TextFactoryV0>();
            break;
    }

    if (factory == nullptr) {
        SPDLOG_ERROR("Failed to load Text with version {}", resource->GetInitData()->ResourceVersion);
        return nullptr;
    }

    factory->ParseFileXML(reader, resource);

    return resource;
}

void LUS::TextFactoryV0::ParseFileBinary(std::shared_ptr<BinaryReader> reader,
                                        std::shared_ptr<IResource> resource) {
    std::shared_ptr<Text> text = std::static_pointer_cast<Text>(resource);
    ResourceVersionFactory::ParseFileBinary(reader, text);

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
}

// This is a special character that we use to replace the null terminator in the message, otherwise tinyxml2 will stop reading the string
const char* XML_NULL_TERMINATOR = "\x7F";

void TextFactoryV0::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Text> txt = std::static_pointer_cast<Text>(resource);

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();

        if (childName == "TextEntry") {
            MessageEntry entry;
            entry.id = child->IntAttribute("ID");
            entry.textboxType = child->IntAttribute("TextboxType");
            entry.textboxYPos = child->IntAttribute("TextboxYPos");
            entry.msg = child->Attribute("Message");

            // Replace the special string with a null terminator if it exists
            size_t nullTerminatorPos = entry.msg.find(XML_NULL_TERMINATOR);
            while (nullTerminatorPos != std::string::npos) {
                entry.msg.replace(nullTerminatorPos, strlen(XML_NULL_TERMINATOR), " ");
                entry.msg[nullTerminatorPos] = '\0';
                nullTerminatorPos = entry.msg.find(XML_NULL_TERMINATOR);
            }

            txt->messages.push_back(entry);
        }

        child = child->NextSiblingElement();
    }
}

} // namespace LUS

#include "soh/resource/importer/TextFactory.h"
#include "soh/resource/type/Text.h"
#include "spdlog/spdlog.h"
#include "include/message_data_fmt.h"
#include "include/message_data_textbox_types.h"

namespace LUS {
void WriteFileXML(tinyxml2::XMLDocument* writer, std::shared_ptr<IResource> resource);

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

    // /*
    // See WriteFileXML, this will not create the export path folders, so you will need to create them yourself or the export will fail with error code 4
    // tinyxml2::XMLDocument doc;
    // auto writer = doc.NewElement("Text");
    // writer->SetAttribute("Version", resource->GetInitData()->ResourceVersion);
    // doc.InsertFirstChild(writer);

    // WriteFileXML(&doc, resource);

    // auto res = doc.SaveFile(("export/" + resource->GetInitData()->Path).c_str());
    // if (res != tinyxml2::XML_SUCCESS) {
    //     SPDLOG_ERROR("Failed to export resource {} as XML Error:{}", resource->GetInitData()->Path, res);
    // } else {
    //     SPDLOG_INFO("Exported resource {} as XML", resource->GetInitData()->Path);
    // }
    // */

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
void TextFactoryV0::ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Text> textResource = std::static_pointer_cast<Text>(resource);

    auto child = reader->FirstChildElement();

    while (child != nullptr) {
        std::string childName = child->Name();
        if (childName != "TextEntry") {
            SPDLOG_WARN("Unknown element {}. Text Resource:{}", childName, textResource->GetInitData()->Path);
            child = child->NextSiblingElement();
            continue;
        }

        MessageEntry entry;
        if (child->QueryUnsignedAttribute("ID", (uint*)&entry.id) != tinyxml2::XML_SUCCESS) {
            SPDLOG_WARN("Failed to parse ID attribute. Text Resource:{}", textResource->GetInitData()->Path);
            child = child->NextSiblingElement();
            continue;
        }

        std::string textboxType = child->Attribute("TextboxType");
        if (XMLToTextBoxType.contains(textboxType)) {
            entry.textboxType = XMLToTextBoxType.at(textboxType);
        } else {
            SPDLOG_WARN("Unknown TextboxType {}. TextEntry:{} Text Resource:{}", textboxType, entry.id, textResource->GetInitData()->Path);
            child = child->NextSiblingElement();
            continue;
        }

        std::string textboxYPos = child->Attribute("TextboxYPos");
        if (XMLToTextBoxPosition.contains(textboxYPos)) {
            entry.textboxYPos = XMLToTextBoxPosition.at(textboxYPos);
        } else {
            SPDLOG_WARN("Unknown TextboxYPos {}. TextEntry:{} Text Resource:{}", textboxYPos, entry.id, textResource->GetInitData()->Path);
            child = child->NextSiblingElement();
            continue;
        }

        auto textNode = child->FirstChildElement();
        int dummy = 0;

        while (textNode != nullptr) {
            std::string textNodeName = textNode->Name();
            if (textNodeName == "TextNode") {

                if (textNode->GetText() == nullptr) {
                    entry.msg += ' ';
                    textNode = textNode->NextSiblingElement();
                    continue;
                }

                std::string text = textNode->GetText();
                entry.msg += text;
            } else if (XMLToControlCode.contains(textNodeName)) {
                entry.msg += XMLToControlCode.at(textNodeName);

                switch(XMLToControlCode.at(textNodeName)) {
                    case MESSAGE_COLOR: {
                        std::string value = textNode->Attribute("Value");
                        if (!XMLToColorCode.contains(value)) {
                            SPDLOG_WARN("Unknown color {}. TextEntry:{} Text Resource:{}", value, entry.id, textResource->GetInitData()->Path);
                            entry.msg += XMLToColorCode.at("Default");
                            break;
                        }
                        entry.msg += XMLToColorCode.at(value);
                        break;
                    }
                    case MESSAGE_SHIFT:
                    case MESSAGE_BOX_BREAK_DELAYED:
                    case MESSAGE_FADE:
                    case MESSAGE_ITEM_ICON:
                    case MESSAGE_TEXT_SPEED:
                    case MESSAGE_HIGHSCORE:
                        if (textNode->QueryIntAttribute("Value", &dummy) != tinyxml2::XML_SUCCESS) {
                            SPDLOG_WARN("Failed to get Value attribute. ControlNode:{} TextEntry:{} Text Resource:{}", textNodeName, entry.id, textResource->GetInitData()->Path);
                            entry.msg += '\0';
                            break;
                        }

                        entry.msg += textNode->IntAttribute("Value");
                        break;
                    case MESSAGE_TEXTID:
                    case MESSAGE_FADE2:
                    case MESSAGE_SFX:
                        if (textNode->QueryIntAttribute("Value", &dummy) != tinyxml2::XML_SUCCESS) {
                            SPDLOG_WARN("Failed to get Value attribute. ControlNode:{} TextEntry:{} Text Resource:{}", textNodeName, entry.id, textResource->GetInitData()->Path);
                            entry.msg += '\0';
                            entry.msg += '\0';
                            break;
                        }

                        entry.msg += textNode->IntAttribute("Value") >> 8;
                        entry.msg += textNode->IntAttribute("Value") & 0xFF;
                        break;
                    case MESSAGE_BACKGROUND:
                        if (
                            textNode->QueryIntAttribute("R", &dummy) != tinyxml2::XML_SUCCESS || 
                            textNode->QueryIntAttribute("G", &dummy) != tinyxml2::XML_SUCCESS || 
                            textNode->QueryIntAttribute("B", &dummy) != tinyxml2::XML_SUCCESS
                        ) {
                            SPDLOG_WARN("Failed to get RGB attributes. ControlNode:{} TextEntry:{} Text Resource:{}", textNodeName, entry.id, textResource->GetInitData()->Path);
                            entry.msg += '\0';
                            entry.msg += '\0';
                            entry.msg += '\0';
                            break;
                        }

                        entry.msg += textNode->IntAttribute("R");
                        entry.msg += textNode->IntAttribute("G");
                        entry.msg += textNode->IntAttribute("B");
                        break;
                }
            } else {
                SPDLOG_WARN("Unknown element {}. TextEntry:{} Text Resource:{}", textNodeName, entry.id, textResource->GetInitData()->Path);
            }
            textNode = textNode->NextSiblingElement();
        }

        // Replace special characters
        for (auto [key, value] : XMLToSpecialChar) {
            size_t pos = 0;
            while ((pos = entry.msg.find(key)) != std::string::npos) {
                entry.msg.replace(pos, key.size(), value);
            }
        }

        textResource->messages.push_back(entry);
        child = child->NextSiblingElement();
    }
}

/**
 * This is an example implementation of how to write a V0 Text resource to XML.
 * It is currently not in use anywhere, this will eventually be migrated to a separate system or tool.
 * 
 * To make use of this tool, uncomment the lines in TextFactory::ReadResource
 */
void WriteFileXML(tinyxml2::XMLDocument* writer, std::shared_ptr<IResource> resource) {
    std::shared_ptr<Text> textResource = std::static_pointer_cast<Text>(resource);

    auto root = writer->FirstChildElement();

    for (auto& entry : textResource->messages) {
        tinyxml2::XMLElement* textEntryNode = writer->NewElement("TextEntry");
        textEntryNode->SetAttribute("ID", entry.id);
        textEntryNode->SetAttribute("TextboxType", TextBoxTypeToXML.at(entry.textboxType).c_str());
        textEntryNode->SetAttribute("TextboxYPos", TextBoxPositionToXML.at(entry.textboxYPos).c_str());

        std::string currentText = "";
        bool shouldCreateControlNode = false;
        auto nextControlNode = writer->NewElement("ControlNode");

        for (size_t i = 0; i < entry.msg.size(); i++) {
            int skip = 0;

            if (ControlCodeToXML.contains(entry.msg[i])) {
                nextControlNode->SetName(ControlCodeToXML.at(entry.msg[i]).c_str());
                shouldCreateControlNode = true;

                switch(entry.msg[i]) {
                    case MESSAGE_COLOR: {
                        skip = 1;
                        int color = entry.msg[i + 1] - 0x40;
                        if (!ColorCodeToXML.contains(color)) {
                            SPDLOG_WARN("Unknown color code {}. TextEntry:{} Text Resource:{}", color, entry.id, textResource->GetInitData()->Path);
                            nextControlNode->SetAttribute("Value", ColorCodeToXML.at(MSGCOL_DEFAULT).c_str());
                            break;
                        }
                        nextControlNode->SetAttribute("Value", ColorCodeToXML.at(color).c_str());
                        break;
                    }
                    case MESSAGE_SHIFT:
                    case MESSAGE_BOX_BREAK_DELAYED:
                    case MESSAGE_FADE:
                    case MESSAGE_ITEM_ICON:
                    case MESSAGE_TEXT_SPEED:
                    case MESSAGE_HIGHSCORE:
                        skip = 1;
                        nextControlNode->SetAttribute("Value", entry.msg[i + 1]);
                        break;
                    case MESSAGE_TEXTID:
                    case MESSAGE_FADE2:
                    case MESSAGE_SFX: {
                        skip = 2;
                        nextControlNode->SetAttribute("Value", entry.msg[i + 1] << 8 | entry.msg[i + 2]);
                        break;
                    }
                    case MESSAGE_BACKGROUND: {
                        skip = 3;
                        nextControlNode->SetAttribute("R", entry.msg[i + 1]);
                        nextControlNode->SetAttribute("G", entry.msg[i + 2]);
                        nextControlNode->SetAttribute("B", entry.msg[i + 3]);
                        break;
                    }
                }
            } else if (SpecialCharToXML.contains(entry.msg[i])) {
                currentText += SpecialCharToXML.at(entry.msg[i]);
            } else {
                currentText += entry.msg[i];
            }

            // If (we're at the end of the message or we need to create a control node) and we have text, create a text node
            if ((shouldCreateControlNode || i == entry.msg.size() - 1) && currentText.size() > 0) {
                auto textNode = writer->NewElement("TextNode");
                textNode->SetText(currentText.c_str());
                textEntryNode->InsertEndChild(textNode);
                currentText = "";
            }

            if (shouldCreateControlNode) {
                textEntryNode->InsertEndChild(nextControlNode);
                nextControlNode = writer->NewElement("ControlNode");
                shouldCreateControlNode = false;
            }

            i += skip;
        }

        root->InsertEndChild(textEntryNode);
    }
}

} // namespace LUS

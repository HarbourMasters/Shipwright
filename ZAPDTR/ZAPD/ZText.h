#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class MessageEntry
{
public:
	uint16_t id;
	uint8_t textboxType;
	uint8_t textboxYPos;
	uint32_t segmentId;
	uint32_t msgOffset;
	std::string msg;
};

class ZText : public ZResource
{
public:
	std::vector<MessageEntry> messages;

	ZText(ZFile* nParent);

	void ParseRawData() override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};

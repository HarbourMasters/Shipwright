#pragma once

#include <cstdint>
#include <vector>
#include "ZResource.h"

class ZBackground : public ZResource
{
protected:
	std::vector<uint8_t> data;

public:
	ZBackground(ZFile* nParent);

	void ParseBinaryFile(const std::string& inFolder, bool appendOutName);

	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	void Save(const fs::path& outFolder) override;

	bool IsExternalResource() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
	std::string GetExternalExtension() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;

	void CheckValidJpeg(const std::string& filepath);
};

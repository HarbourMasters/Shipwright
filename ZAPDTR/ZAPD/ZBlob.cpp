#include "ZBlob.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Blob, ZBlob);

ZBlob::ZBlob(ZFile* nParent) : ZResource(nParent)
{
	genOTRDef = true;
	RegisterRequiredAttribute("Size");
}

ZBlob* ZBlob::FromFile(const std::string& filePath)
{
	ZBlob* blob = new ZBlob(nullptr);
	blob->name = StringHelper::Split(Path::GetFileNameWithoutExtension(filePath), ".")[0];
	blob->blobData = File::ReadAllBytes(filePath);

	return blob;
}

void ZBlob::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	blobSize = StringHelper::StrToL(registeredAttributes.at("Size").value, 16);
}

void ZBlob::ParseRawData()
{
	blobData.assign(parent->GetRawData().begin() + rawDataIndex,
	                parent->GetRawData().begin() + rawDataIndex + blobSize);
}

Declaration* ZBlob::DeclareVar(const std::string& prefix,
                               [[maybe_unused]] const std::string& bodyStr)
{
	std::string auxName = name;
	std::string auxOutName = outName;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	if (auxOutName == "")
		auxOutName = GetDefaultName(prefix);

	std::string path = Path::GetFileNameWithoutExtension(auxOutName);

	std::string assetOutDir =
		(Globals::Instance->outputPath / Path::GetFileNameWithoutExtension(GetOutName())).string();

	std::string incStr =
		StringHelper::Sprintf("%s.%s.inc.c", assetOutDir.c_str(), GetExternalExtension().c_str());

	return parent->AddDeclarationIncludeArray(rawDataIndex, incStr, GetRawDataSize(),
	                                          GetSourceTypeName(), auxName, blobData.size());
}

std::string ZBlob::GetBodySourceCode() const
{
	std::string sourceOutput;

	for (size_t i = 0; i < blobData.size(); i += 1)
	{
		if (i % 16 == 0)
			sourceOutput += "\t";

		sourceOutput += StringHelper::Sprintf("0x%02X, ", blobData[i]);

		if (i % 16 == 15)
			sourceOutput += "\n";
	}

	// Ensure there's always a trailing line feed to prevent dumb warnings.
	// Please don't remove this line, unless you somehow made a way to prevent
	// that warning when building the OoT repo.
	sourceOutput += "\n";

	return sourceOutput;
}

void ZBlob::Save(const fs::path& outFolder)
{
	if (!Globals::Instance->otrMode)
		File::WriteAllBytes((outFolder / (name + ".bin")).string(), blobData);
}

bool ZBlob::IsExternalResource() const
{
	return true;
}

std::string ZBlob::GetExternalExtension() const
{
	return "bin";
}

std::string ZBlob::GetSourceTypeName() const
{
	return "u8";
}

ZResourceType ZBlob::GetResourceType() const
{
	return ZResourceType::Blob;
}

size_t ZBlob::GetRawDataSize() const
{
	return blobSize;
}

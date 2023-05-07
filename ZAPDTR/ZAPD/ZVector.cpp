#include "ZVector.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Vector, ZVector);

ZVector::ZVector(ZFile* nParent) : ZResource(nParent)
{
	scalarType = ZScalarType::ZSCALAR_NONE;
	dimensions = 0;

	RegisterRequiredAttribute("Type");
	RegisterRequiredAttribute("Dimensions");
}

void ZVector::ExtractFromBinary(uint32_t nRawDataIndex, ZScalarType nScalarType,
                                uint32_t nDimensions)
{
	rawDataIndex = nRawDataIndex;
	scalarType = nScalarType;
	dimensions = nDimensions;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
}

void ZVector::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	this->scalarType = ZScalar::MapOutputTypeToScalarType(registeredAttributes.at("Type").value);

	this->dimensions = StringHelper::StrToL(registeredAttributes.at("Dimensions").value, 16);
}

void ZVector::ParseRawData()
{
	int32_t currentRawDataIndex = rawDataIndex;
	// TODO: this shouldn't be necessary.
	scalars.clear();
	scalars.reserve(dimensions);
	for (uint32_t i = 0; i < dimensions; i++)
	{
		ZScalar scalar(parent);
		scalar.ExtractFromBinary(currentRawDataIndex, scalarType);
		currentRawDataIndex += scalar.GetRawDataSize();

		scalars.push_back(scalar);
	}

	// Ensure the scalars vector has the same number of elements as the vector dimension.
	assert(scalars.size() == dimensions);
}

size_t ZVector::GetRawDataSize() const
{
	size_t size = 0;

	for (size_t i = 0; i < this->scalars.size(); i++)
		size += this->scalars[i].GetRawDataSize();

	return size;
}

bool ZVector::DoesSupportArray() const
{
	return true;
}

std::string ZVector::GetSourceTypeName() const
{
	if (dimensions == 3 && scalarType == ZScalarType::ZSCALAR_F32)
		return "Vec3f";
	else if (dimensions == 3 && scalarType == ZScalarType::ZSCALAR_S16)
		return "Vec3s";
	else if (dimensions == 3 && scalarType == ZScalarType::ZSCALAR_S32)
		return "Vec3i";
	else
	{
		std::string msgHeader = StringHelper::Sprintf(
			"encountered unsupported vector type: %d dimensions, %s type", dimensions,
			ZScalar::MapScalarTypeToOutputType(scalarType).c_str());

		HANDLE_ERROR_RESOURCE(WarningType::NotImplemented, parent, this, rawDataIndex, msgHeader,
		                      "");
	}
}

std::string ZVector::GetBodySourceCode() const
{
	std::string body = "";

	for (size_t i = 0; i < scalars.size(); i++)
	{
		body += StringHelper::Sprintf("%6s", scalars[i].GetBodySourceCode().c_str());

		if (i + 1 < scalars.size())
			body += ", ";
	}

	return body;
}

ZResourceType ZVector::GetResourceType() const
{
	return ZResourceType::Vector;
}

DeclarationAlignment ZVector::GetDeclarationAlignment() const
{
	if (scalars.size() == 0)
	{
		return DeclarationAlignment::Align4;
	}
	return scalars.at(0).GetDeclarationAlignment();
}

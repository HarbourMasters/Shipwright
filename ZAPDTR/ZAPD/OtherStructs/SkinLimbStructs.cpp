#include "SkinLimbStructs.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZDisplayList.h"
#include "ZFile.h"

/* Struct_800A57C0 */

Struct_800A57C0::Struct_800A57C0(ZFile* nParent) : ZResource(nParent)
{
}

void Struct_800A57C0::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x04);
	unk_6 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x06);
	unk_7 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x07);
	unk_8 = BitConverter::ToInt8BE(rawData, rawDataIndex + 0x08);
	unk_9 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
}

std::string Struct_800A57C0::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, %i, %i, %i, %i, %i, 0x%02X", unk_0, unk_2, unk_4, unk_6,
	                             unk_7, unk_8, unk_9);
}

std::string Struct_800A57C0::GetSourceTypeName() const
{
	return "Struct_800A57C0";
}

ZResourceType Struct_800A57C0::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t Struct_800A57C0::GetRawDataSize() const
{
	return 0x0A;
}

/* Struct_800A598C_2 */

Struct_800A598C_2::Struct_800A598C_2(ZFile* nParent) : ZResource(nParent)
{
}

void Struct_800A598C_2::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x00);
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x02);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x04);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 0x06);
	unk_8 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
}

std::string Struct_800A598C_2::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, %i, %i, %i, 0x%02X", unk_0, x, y, z, unk_8);
}

std::string Struct_800A598C_2::GetSourceTypeName() const
{
	return "Struct_800A598C_2";
}

ZResourceType Struct_800A598C_2::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t Struct_800A598C_2::GetRawDataSize() const
{
	return 0x0A;
}

/* Struct_800A598C */

Struct_800A598C::Struct_800A598C(ZFile* nParent) : ZResource(nParent)
{
}

void Struct_800A598C::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x04);
	unk_8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x08);
	unk_C = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x0C);

	if (unk_8 != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);

		unk_8_arr.reserve(unk_0);
		for (size_t i = 0; i < unk_0; i++)
		{
			Struct_800A57C0 unk8_data(parent);
			unk8_data.ExtractFromFile(unk_8_Offset);
			unk_8_arr.push_back(unk8_data);

			unk_8_Offset += unk8_data.GetRawDataSize();
		}
	}

	if (unk_C != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_C_Offset = Seg2Filespace(unk_C, parent->baseAddress);

		unk_C_arr.reserve(unk_2);
		for (size_t i = 0; i < unk_2; i++)
		{
			Struct_800A598C_2 unkC_data(parent);
			unkC_data.ExtractFromFile(unk_C_Offset);
			unk_C_arr.push_back(unkC_data);

			unk_C_Offset += unkC_data.GetRawDataSize();
		}
	}
}

void Struct_800A598C::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = prefix;
	if (name != "")
		varPrefix = name;

	if (unk_8 != 0 && GETSEGNUM(unk_8) == parent->segment)
	{
		const auto& res = unk_8_arr.at(0);
		std::string unk_8_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_8_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_8_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_8_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_8_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_8_Str, arrayItemCnt, entryStr);
		}
		else
			decl->text = entryStr;
	}

	if (unk_C != 0 && GETSEGNUM(unk_C) == parent->segment)
	{
		const auto& res = unk_C_arr.at(0);
		std::string unk_C_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_C_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_C_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_C_Offset = Seg2Filespace(unk_C, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_C_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_C_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_C_Str, arrayItemCnt, entryStr);
		}
		else
			decl->text = entryStr;
	}
}

std::string Struct_800A598C::GetBodySourceCode() const
{
	std::string unk_8_Str;
	std::string unk_C_Str;
	Globals::Instance->GetSegmentedPtrName(unk_8, parent, "Struct_800A57C0", unk_8_Str,
	                                       parent->workerID);
	Globals::Instance->GetSegmentedPtrName(unk_C, parent, "Struct_800A598C_2", unk_C_Str,
	                                       parent->workerID);

	std::string entryStr = StringHelper::Sprintf("\n\t\tARRAY_COUNTU(%s), ARRAY_COUNTU(%s),\n",
	                                             unk_8_Str.c_str(), unk_C_Str.c_str());
	entryStr +=
		StringHelper::Sprintf("\t\t%i, %s, %s\n\t", unk_4, unk_8_Str.c_str(), unk_C_Str.c_str());

	return entryStr;
}

std::string Struct_800A598C::GetSourceTypeName() const
{
	return "Struct_800A598C";
}

ZResourceType Struct_800A598C::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t Struct_800A598C::GetRawDataSize() const
{
	return 0x10;
}

/* Struct_800A5E28 */

Struct_800A5E28::Struct_800A5E28(ZFile* nParent) : ZResource(nParent)
{
}

void Struct_800A5E28::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	unk_0 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x00);
	unk_2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x02);
	unk_4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x04);
	unk_8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x08);

	if (unk_4 != 0 && GETSEGNUM(unk_4) == parent->segment)
	{
		uint32_t unk_4_Offset = Seg2Filespace(unk_4, parent->baseAddress);

		unk_4_arr.reserve(unk_2);
		for (size_t i = 0; i < unk_2; i++)
		{
			Struct_800A598C unk_4_data(parent);
			unk_4_data.ExtractFromFile(unk_4_Offset);
			unk_4_arr.push_back(unk_4_data);

			unk_4_Offset += unk_4_data.GetRawDataSize();
		}
	}
}

void Struct_800A5E28::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = prefix;
	if (name != "")
		varPrefix = name;

	ZResource::DeclareReferences(varPrefix);

	if (unk_4 != SEGMENTED_NULL && GETSEGNUM(unk_4) == parent->segment)
	{
		const auto& res = unk_4_arr.at(0);
		std::string unk_4_Str = res.GetDefaultName(varPrefix);

		size_t arrayItemCnt = unk_4_arr.size();
		std::string entryStr = "";

		for (size_t i = 0; i < arrayItemCnt; i++)
		{
			auto& child = unk_4_arr[i];
			child.DeclareReferences(varPrefix);
			entryStr += StringHelper::Sprintf("\t{ %s },", child.GetBodySourceCode().c_str());

			if (i < arrayItemCnt - 1)
				entryStr += "\n";
		}

		uint32_t unk_4_Offset = Seg2Filespace(unk_4, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(unk_4_Offset);
		if (decl == nullptr)
		{
			parent->AddDeclarationArray(unk_4_Offset, res.GetDeclarationAlignment(),
			                            arrayItemCnt * res.GetRawDataSize(),
			                            res.GetSourceTypeName(), unk_4_Str, arrayItemCnt, entryStr);
		}
		else
			decl->text = entryStr;
	}

	if (unk_8 != SEGMENTED_NULL && GETSEGNUM(unk_8) == parent->segment)
	{
		uint32_t unk_8_Offset = Seg2Filespace(unk_8, parent->baseAddress);

		int32_t dlistLength = ZDisplayList::GetDListLength(
			parent->GetRawData(), unk_8_Offset,
			Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
		unk_8_dlist = new ZDisplayList(parent);
		unk_8_dlist->ExtractFromBinary(unk_8_Offset, dlistLength);

		std::string dListStr =
			StringHelper::Sprintf("%sSkinLimbDL_%06X", varPrefix.c_str(), unk_8_Offset);
		unk_8_dlist->SetName(dListStr);
		unk_8_dlist->DeclareVar(varPrefix, "");
		unk_8_dlist->DeclareReferences(varPrefix);
		parent->AddResource(unk_8_dlist);
	}
}

std::string Struct_800A5E28::GetBodySourceCode() const
{
	std::string unk_4_Str;
	std::string unk_8_Str;
	Globals::Instance->GetSegmentedPtrName(unk_4, parent, "Struct_800A598C", unk_4_Str,
	                                       parent->workerID);
	Globals::Instance->GetSegmentedPtrName(unk_8, parent, "Gfx", unk_8_Str, parent->workerID);

	std::string entryStr = "\n";
	entryStr += StringHelper::Sprintf("\t%i, ARRAY_COUNTU(%s),\n", unk_0, unk_4_Str.c_str());
	entryStr += StringHelper::Sprintf("\t%s, %s\n", unk_4_Str.c_str(), unk_8_Str.c_str());

	return entryStr;
}

std::string Struct_800A5E28::GetSourceTypeName() const
{
	return "Struct_800A5E28";
}

ZResourceType Struct_800A5E28::GetResourceType() const
{
	// TODO
	return ZResourceType::Error;
}

size_t Struct_800A5E28::GetRawDataSize() const
{
	return 0x0C;
}

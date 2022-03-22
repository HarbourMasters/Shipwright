#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ZResource.h"
#include "ZDisplayList.h"

// TODO: check if more types exists
enum class ZLimbSkinType
{
	SkinType_0,           // Segment = 0
	SkinType_4 = 4,       // Segment = segmented address // Struct_800A5E28
	SkinType_5 = 5,       // Segment = 0
	SkinType_DList = 11,  // Segment = DList address
};

class Struct_800A57C0 : public ZResource
{
public:
	Struct_800A57C0(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

public:
	uint16_t unk_0;
	int16_t unk_2;
	int16_t unk_4;
	int8_t unk_6;
	int8_t unk_7;
	int8_t unk_8;
	uint8_t unk_9;
};

class Struct_800A598C_2 : public ZResource
{
public:
	Struct_800A598C_2(ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

public:
	uint8_t unk_0;
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t unk_8;
};

class Struct_800A598C : public ZResource
{
public:
	Struct_800A598C(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

public:
	uint16_t unk_0;  // Length of unk_8
	uint16_t unk_2;  // Length of unk_C
	uint16_t unk_4;  // 0 or 1 // Used as an index for unk_C
	segptr_t unk_8;  // Struct_800A57C0*
	segptr_t unk_C;  // Struct_800A598C_2*

	std::vector<Struct_800A57C0> unk_8_arr;
	std::vector<Struct_800A598C_2> unk_C_arr;
};

class Struct_800A5E28 : public ZResource
{
public:
	Struct_800A5E28(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

public:
	uint16_t unk_0;  // Vtx count
	uint16_t unk_2;  // Length of unk_4
	segptr_t unk_4;  // Struct_800A598C*
	segptr_t unk_8;  // Gfx*

	std::vector<Struct_800A598C> unk_4_arr;
	ZDisplayList* unk_8_dlist = nullptr;
};

#pragma once

#include <memory>
#include "ZBackground.h"
#include "ZDisplayList.h"
#include "ZRoom/ZRoomCommand.h"

class PolygonDlist : public ZResource
{
public:
	ZRoom* zRoom;

	uint8_t polyType;

	int16_t x, y, z;  // polyType == 2
	int16_t unk_06;   // polyType == 2

	segptr_t opa = 0;  // Gfx*
	segptr_t xlu = 0;  // Gfx*

	ZDisplayList* opaDList = nullptr;  // Gfx*
	ZDisplayList* xluDList = nullptr;  // Gfx*

	PolygonDlist(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	void GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

	void SetPolyType(uint8_t nPolyType);

protected:
	ZDisplayList* MakeDlist(segptr_t ptr, const std::string& prefix);
};

class BgImage : public ZResource
{
public:
	uint16_t unk_00;
	uint8_t id;
	segptr_t source;
	uint32_t unk_0C;
	uint32_t tlut;
	uint16_t width;
	uint16_t height;
	uint8_t fmt;
	uint8_t siz;
	uint16_t mode0;
	uint16_t tlutCount;

	ZBackground* sourceBackground;

	bool isSubStruct;

	BgImage(ZFile* nParent);
	BgImage(bool nIsSubStruct, const std::string& prefix, uint32_t nRawDataIndex, ZFile* nParent);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

protected:
	ZBackground* MakeBackground(segptr_t ptr, const std::string& prefix);
};

class PolygonTypeBase : public ZResource
{
public:
	uint8_t type;
	std::vector<PolygonDlist> polyDLists;

	PolygonTypeBase(ZFile* nParent, uint32_t nRawDataIndex, ZRoom* nRoom);

	void DeclareAndGenerateOutputCode(const std::string& prefix);

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

protected:
	ZRoom* zRoom;
};

class PolygonType1 : public PolygonTypeBase
{
public:
	uint8_t format;
	segptr_t dlist;

	// single
	BgImage single;

	// multi
	uint8_t count;
	segptr_t list;  // BgImage*
	std::vector<BgImage> multiList;

	PolygonType1(ZFile* nParent, uint32_t nRawDataIndex, ZRoom* nRoom);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;

	size_t GetRawDataSize() const override;
};

class PolygonType2 : public PolygonTypeBase
{
public:
	uint8_t num;
	segptr_t start;
	segptr_t end;

	PolygonType2(ZFile* nParent, uint32_t nRawDataIndex, ZRoom* nRoom);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;
};

class SetMesh : public ZRoomCommand
{
public:
	uint8_t data;
	uint8_t meshHeaderType;
	std::shared_ptr<PolygonTypeBase> polyType;

	SetMesh(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	std::string GenDListExterns(ZDisplayList* dList);
};

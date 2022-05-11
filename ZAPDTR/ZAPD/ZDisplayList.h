#pragma once

#include "ZMtx.h"
#include "ZResource.h"
#include "ZRoom/ZRoom.h"
#include "ZTexture.h"
#include "ZVtx.h"
#include "tinyxml2.h"

#include <map>
#include <string>
#include <vector>

enum class F3DEXOpcode
{
	G_SPNOOP = 0x00,
	G_MTX = 0x01,
	G_MOVEMEM = 0x03,
	G_VTX = 0x04,
	G_DL = 0x06,
	G_LOAD_UCODE = 0xAF,
	G_BRANCH_Z = 0xB0,
	G_TRI2 = 0xB1,
	G_MODIFYVTX = 0xB2,
	G_RDPHALF_2 = 0xB3,
	G_RDPHALF_1 = 0xB4,
	G_QUAD = 0xB5,
	G_CLEARGEOMETRYMODE = 0xB6,
	G_SETGEOMETRYMODE = 0xB7,
	G_ENDDL = 0xB8,
	G_SETOTHERMODE_L = 0xB9,
	G_SETOTHERMODE_H = 0xBA,
	G_TEXTURE = 0xBB,
	G_MOVEWORD = 0xBC,
	G_POPMTX = 0xBD,
	G_CULLDL = 0xBE,
	G_TRI1 = 0xBF,
	G_NOOP = 0xC0,
	G_TEXRECT = 0xE4,
	G_TEXRECTFLIP = 0xE5,
	G_RDPLOADSYNC = 0xE6,
	G_RDPPIPESYNC = 0xE7,
	G_RDPTILESYNC = 0xE8,
	G_RDPFULLSYNC = 0xE9,
	G_SETKEYGB = 0xEA,
	G_SETKEYR = 0xEB,
	G_SETCONVERT = 0xEC,
	G_SETSCISSOR = 0xED,
	G_SETPRIMDEPTH = 0xEE,
	G_RDPSETOTHERMODE = 0xEF,
	G_LOADTLUT = 0xF0,
	G_SETTILESIZE = 0xF2,
	G_LOADBLOCK = 0xF3,
	G_LOADTILE = 0xF4,
	G_SETTILE = 0xF5,
	G_FILLRECT = 0xF6,
	G_SETFILLCOLOR = 0xF7,
	G_SETFOGCOLOR = 0xF8,
	G_SETBLENDCOLOR = 0xF9,
	G_SETPRIMCOLOR = 0xFA,
	G_SETENVCOLOR = 0xFB,
	G_SETCOMBINE = 0xFC,
	G_SETTIMG = 0xFD,
	G_SETZIMG = 0xFE,
	G_SETCIMG = 0xFF
};

enum class F3DZEXOpcode
{
	G_NOOP = 0x00,
	G_VTX = 0x01,
	G_MODIFYVTX = 0x02,
	G_CULLDL = 0x03,
	G_BRANCH_Z = 0x04,
	G_TRI1 = 0x05,
	G_TRI2 = 0x06,
	G_QUAD = 0x07,
	G_SPECIAL_3 = 0xD3,
	G_SPECIAL_2 = 0xD4,
	G_SPECIAL_1 = 0xD5,
	G_DMA_IO = 0xD6,
	G_TEXTURE = 0xD7,
	G_POPMTX = 0xD8,
	G_GEOMETRYMODE = 0xD9,
	G_MTX = 0xDA,
	G_MOVEWORD = 0xDB,
	G_MOVEMEM = 0xDC,
	G_LOAD_UCODE = 0xDD,
	G_DL = 0xDE,
	G_ENDDL = 0xDF,
	G_SPNOOP = 0xE0,
	G_RDPHALF_1 = 0xE1,
	G_SETOTHERMODE_L = 0xE2,
	G_SETOTHERMODE_H = 0xE3,
	G_TEXRECT = 0xE4,
	G_TEXRECTFLIP = 0xE5,
	G_RDPLOADSYNC = 0xE6,
	G_RDPPIPESYNC = 0xE7,
	G_RDPTILESYNC = 0xE8,
	G_RDPFULLSYNC = 0xE9,
	G_SETKEYGB = 0xEA,
	G_SETKEYR = 0xEB,
	G_SETCONVERT = 0xEC,
	G_SETSCISSOR = 0xED,
	G_SETPRIMDEPTH = 0xEE,
	G_RDPSETOTHERMODE = 0xEF,
	G_LOADTLUT = 0xF0,
	G_RDPHALF_2 = 0xF1,
	G_SETTILESIZE = 0xF2,
	G_LOADBLOCK = 0xF3,
	G_LOADTILE = 0xF4,
	G_SETTILE = 0xF5,
	G_FILLRECT = 0xF6,
	G_SETFILLCOLOR = 0xF7,
	G_SETFOGCOLOR = 0xF8,
	G_SETBLENDCOLOR = 0xF9,
	G_SETPRIMCOLOR = 0xFA,
	G_SETENVCOLOR = 0xFB,
	G_SETCOMBINE = 0xFC,
	G_SETTIMG = 0xFD,
	G_SETZIMG = 0xFE,
	G_SETCIMG = 0xFF,
};

enum class F3DZEXTexFormats
{
	G_IM_FMT_RGBA,
	G_IM_FMT_YUV,
	G_IM_FMT_CI,
	G_IM_FMT_IA,
	G_IM_FMT_I
};

enum class F3DZEXTexSizes
{
	G_IM_SIZ_4b,
	G_IM_SIZ_8b,
	G_IM_SIZ_16b,
	G_IM_SIZ_32b
};

enum class DListType
{
	F3DZEX,
	F3DEX,
};

enum class OoTSegments
{
	DirectReference = 0,
	TitleStatic = 1,
	Scene = 2,
	Room = 3,
	GameplayKeep = 4,
	FieldDungeonKeep = 5,
	Object = 6,
	LinkAnimation = 7,
	IconItemStatic = 8,
	IconItem24Static = 9,
	Unknown10 = 10,
	Unknown11 = 11,
	Unknown12 = 12,
	IconFieldDungeonStatic = 13,
	IconItemLanguageStatic = 14,
	ZBuffer = 15,
	FrameBuffer = 16,
};


class ZDisplayList : public ZResource
{
protected:
	static TextureType TexFormatToTexType(F3DZEXTexFormats fmt, F3DZEXTexSizes siz);

	void ParseF3DZEX(F3DZEXOpcode opcode, uint64_t data, int32_t i, const std::string& prefix,
	                 char* line);
	void ParseF3DEX(F3DEXOpcode opcode, uint64_t data, const std::string& prefix, char* line);

	// Various Instruction Optimizations
	bool SequenceCheck(std::vector<F3DZEXOpcode> sequence, int32_t startIndex);
	int32_t OptimizationChecks(int32_t startIndex, std::string& output, const std::string& prefix);
	int32_t OptimizationCheck_LoadTextureBlock(int32_t startIndex, std::string& output,
	                                           const std::string& prefix);
	// int32_t OptimizationCheck_LoadMultiBlock(int32_t startIndex, std::string& output, std::string
	// prefix);

	// F3DEX Specific Opcode Values
	void Opcode_F3DEX_G_SETOTHERMODE_L(uint64_t data, char* line);

	// Shared Opcodes between F3DZEX and F3DEX
	void Opcode_G_DL(uint64_t data, const std::string& prefix, char* line);
	void Opcode_G_MODIFYVTX(uint64_t data, char* line);
	void Opcode_G_CULLDL(uint64_t data, char* line);
	void Opcode_G_TRI1(uint64_t data, char* line);
	void Opcode_G_TRI2(uint64_t data, char* line);
	void Opcode_G_MTX(uint64_t data, char* line);
	void Opcode_G_VTX(uint64_t data, char* line);
	void Opcode_G_TEXTURE(uint64_t data, char* line);
	void Opcode_G_SETTIMG(uint64_t data, const std::string& prefix, char* line);
	void Opcode_G_SETTILE(uint64_t data, char* line);
	void Opcode_G_SETTILESIZE(uint64_t data, const std::string& prefix, char* line);
	void Opcode_G_LOADBLOCK(uint64_t data, char* line);
	void Opcode_G_SETCOMBINE(uint64_t data, char* line);
	void Opcode_G_SETPRIMCOLOR(uint64_t data, char* line);
	void Opcode_G_SETOTHERMODE_L(uint64_t data, char* line);
	void Opcode_G_SETOTHERMODE_H(uint64_t data, char* line);
	void Opcode_G_LOADTLUT(uint64_t data, const std::string& prefix, char* line);
	void Opcode_G_ENDDL(const std::string& prefix, char* line);

public:
	std::vector<uint64_t> instructions;

	int32_t lastTexWidth, lastTexHeight, lastTexAddr, lastTexSeg;
	F3DZEXTexFormats lastTexFmt;
	F3DZEXTexSizes lastTexSiz, lastTexSizTest, lastCISiz;
	bool lastTexLoaded;
	bool lastTexIsPalette;

	DListType dListType;

	std::map<uint32_t, std::vector<ZVtx>> vertices;
	std::vector<ZDisplayList*> otherDLists;

	ZTexture* lastTexture = nullptr;
	ZTexture* lastTlut = nullptr;

	std::vector<segptr_t> references;
	std::vector<ZMtx> mtxList;

	ZDisplayList(ZFile* nParent);
	~ZDisplayList();

	void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex) override;
	void ExtractFromBinary(uint32_t nRawDataIndex, int32_t rawDataSize);

	void ParseRawData() override;

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetDefaultName(const std::string& prefix) const override;

	void TextureGenCheck();
	static bool TextureGenCheck(int32_t texWidth, int32_t texHeight, uint32_t texAddr,
	                            uint32_t texSeg, F3DZEXTexFormats texFmt, F3DZEXTexSizes texSiz,
	                            bool texLoaded, bool texIsPalette, ZDisplayList* self);
	static int32_t GetDListLength(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex,
	                              DListType dListType);

	size_t GetRawDataSize() const override;
	DeclarationAlignment GetDeclarationAlignment() const override;
	void DeclareReferences(const std::string& prefix) override;
	std::string ProcessLegacy(const std::string& prefix);
	std::string ProcessGfxDis(const std::string& prefix);

	bool IsExternalResource() const override;
	std::string GetExternalExtension() const override;
	std::string GetSourceTypeName() const override;

	ZResourceType GetResourceType() const override;

protected:
	size_t numInstructions;
};

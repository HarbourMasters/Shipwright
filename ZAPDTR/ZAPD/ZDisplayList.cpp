#include "ZDisplayList.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cinttypes>
#include <cmath>

#include "Globals.h"
#include "OutputFormatter.h"
#include "Utils/BitConverter.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "gfxd.h"


#define G_MDSFT_ALPHACOMPARE 0
#define G_MDSFT_ZSRCSEL 2
#define G_MDSFT_RENDERMODE 3
#define G_MDSFT_BLENDER 16

#define G_RM_FOG_SHADE_A 0xC8000000
#define G_RM_FOG_PRIM_A 0xC4000000
#define G_RM_PASS 0x0C080000
#define G_RM_AA_ZB_OPA_SURF 0x442078
#define G_RM_AA_ZB_OPA_SURF2 0x112078
#define G_RM_AA_ZB_XLU_SURF 0x4049D8
#define G_RM_AA_ZB_XLU_SURF2 0x1049D8
#define G_RM_AA_ZB_OPA_DECAL 0x442D58
#define G_RM_AA_ZB_OPA_DECAL2 0x112D58
#define G_RM_AA_ZB_XLU_DECAL 0x404DD8
#define G_RM_AA_ZB_XLU_DECAL2 0x104DD8
#define G_RM_AA_ZB_OPA_INTER 0x442478
#define G_RM_AA_ZB_OPA_INTER2 0x112478
#define G_RM_AA_ZB_XLU_INTER 0x4045D8
#define G_RM_AA_ZB_XLU_INTER2 0x1045D8
#define G_RM_AA_ZB_XLU_LINE 0x407858
#define G_RM_AA_ZB_XLU_LINE2 0x107858
#define G_RM_AA_ZB_DEC_LINE 0x407F58
#define G_RM_AA_ZB_DEC_LINE2 0x107F58
#define G_RM_AA_ZB_TEX_EDGE 0x443078
#define G_RM_AA_ZB_TEX_EDGE2 0x113078
#define G_RM_AA_ZB_TEX_INTER 0x443478
#define G_RM_AA_ZB_TEX_INTER2 0x113478
#define G_RM_AA_ZB_SUB_SURF 0x442878
#define G_RM_AA_ZB_SUB_SURF2 0x112278
#define G_RM_AA_ZB_PCL_SURF 0x40007B
#define G_RM_AA_ZB_PCL_SURF2 0x10007B
#define G_RM_AA_ZB_OPA_TERR 0x402078
#define G_RM_AA_ZB_OPA_TERR2 0x102078
#define G_RM_AA_ZB_TEX_TERR 0x403078
#define G_RM_AA_ZB_TEX_TERR2 0x103078
#define G_RM_AA_ZB_SUB_TERR 0x402278
#define G_RM_AA_ZB_SUB_TERR2 0x102278
#define G_RM_RA_ZB_OPA_SURF 0x442038
#define G_RM_RA_ZB_OPA_SURF2 0x112038
#define G_RM_RA_ZB_OPA_DECAL 0x442D18
#define G_RM_RA_ZB_OPA_DECAL2 0x112D18
#define G_RM_RA_ZB_OPA_INTER 0x442438
#define G_RM_RA_ZB_OPA_INTER2 0x112438
#define G_RM_AA_OPA_SURF 0x442048
#define G_RM_AA_OPA_SURF2 0x112048
#define G_RM_AA_XLU_SURF 0x4041C8
#define G_RM_AA_XLU_SURF2 0x1041C8
#define G_RM_AA_XLU_LINE 0x407048
#define G_RM_AA_XLU_LINE2 0x107048
#define G_RM_AA_DEC_LINE 0x407248
#define G_RM_AA_DEC_LINE2 0x107248
#define G_RM_AA_TEX_EDGE 0x443048
#define G_RM_AA_TEX_EDGE2 0x113048
#define G_RM_AA_SUB_SURF 0x442248
#define G_RM_AA_SUB_SURF2 0x112248
#define G_RM_AA_PCL_SURF 0x40004B
#define G_RM_AA_PCL_SURF2 0x10004B
#define G_RM_AA_OPA_TERR 0x402048
#define G_RM_AA_OPA_TERR2 0x102048
#define G_RM_AA_TEX_TERR 0x403048
#define G_RM_AA_TEX_TERR2 0x103048
#define G_RM_AA_SUB_TERR 0x402248
#define G_RM_AA_SUB_TERR2 0x102248
#define G_RM_RA_OPA_SURF 0x442008
#define G_RM_RA_OPA_SURF2 0x112008
#define G_RM_ZB_OPA_SURF 0x442230
#define G_RM_ZB_OPA_SURF2 0x112230
#define G_RM_ZB_XLU_SURF 0x404A50
#define G_RM_ZB_XLU_SURF2 0x104A50
#define G_RM_ZB_OPA_DECAL 0x442E10
#define G_RM_ZB_OPA_DECAL2 0x112E10
#define G_RM_ZB_XLU_DECAL 0x404E50
#define G_RM_ZB_XLU_DECAL2 0x104E50
#define G_RM_ZB_CLD_SURF 0x404B50
#define G_RM_ZB_CLD_SURF2 0x104B50
#define G_RM_ZB_OVL_SURF 0x404F50
#define G_RM_ZB_OVL_SURF2 0x104F50
#define G_RM_ZB_PCL_SURF 0x0C080233
#define G_RM_ZB_PCL_SURF2 0x03020233
#define G_RM_OPA_SURF 0x0C084000
#define G_RM_OPA_SURF2 0x03024000
#define G_RM_XLU_SURF 0x00404200
#define G_RM_XLU_SURF2 0x00104240
#define G_RM_CLD_SURF 0x00404340
#define G_RM_CLD_SURF2 0x00104340
#define G_RM_TEX_EDGE 0x0C087008
#define G_RM_TEX_EDGE2 0x03027008
#define G_RM_PCL_SURF 0x0C084203
#define G_RM_PCL_SURF2 0x03024203
#define G_RM_ADD 0x04484340
#define G_RM_ADD2 0x01124340
#define G_RM_NOOP 0x00000000
#define G_RM_NOOP2 0x00000000
#define G_RM_VISCVG 0x0C844040
#define G_RM_VISCVG2 0x03214040
#define G_RM_OPA_CI 0x0C080000
#define G_RM_OPA_CI2 0x03020000

#define AA_EN 0x8
#define Z_CMP 0x10
#define Z_UPD 0x20
#define IM_RD 0x40
#define CLR_ON_CVG 0x80
#define CVG_DST_CLAMP 0
#define CVG_DST_WRAP 0x100
#define CVG_DST_FULL 0x200
#define CVG_DST_SAVE 0x300
#define ZMODE_OPA 0
#define ZMODE_INTER 0x400
#define ZMODE_XLU 0x800
#define ZMODE_DEC 0xc00
#define CVG_X_ALPHA 0x1000
#define ALPHA_CVG_SEL 0x2000
#define FORCE_BL 0x4000
#define TEX_EDGE 0x0000

REGISTER_ZFILENODE(DList, ZDisplayList);

ZDisplayList::ZDisplayList(ZFile* nParent) : ZResource(nParent)
{
	lastTexWidth = 0;
	lastTexHeight = 0;
	lastTexAddr = 0;
	lastTexFmt = F3DZEXTexFormats::G_IM_FMT_RGBA;
	lastTexSiz = F3DZEXTexSizes::G_IM_SIZ_16b;
	lastTexSizTest = F3DZEXTexSizes::G_IM_SIZ_16b;
	lastTexLoaded = false;
	lastTexIsPalette = false;
	dListType = Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX;
	genOTRDef = true;
	RegisterOptionalAttribute("Ucode");
}

ZDisplayList::~ZDisplayList()
{
	for (auto o : otherDLists)
	{
		delete o;
	}
}

// EXTRACT MODE
void ZDisplayList::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;
	ParseXML(reader);
	// TODO add error handling here
	bool ucodeSet = registeredAttributes.at("Ucode").wasSet;
	std::string ucodeValue = registeredAttributes.at("Ucode").value;
	if ((Globals::Instance->game == ZGame::OOT_SW97) || (ucodeValue == "f3dex"))
	{
		dListType = DListType::F3DEX;
	}
	else if (!ucodeSet || ucodeValue == "f3dex2")
	{
		dListType = DListType::F3DZEX;
	}
	else
	{
		HANDLE_ERROR_RESOURCE(
			WarningType::InvalidAttributeValue, parent, this, rawDataIndex,
			StringHelper::Sprintf("Invalid ucode type in node: %s\n", reader->Name()), "");
	}

	// Don't parse raw data of external files
	if (parent->GetMode() != ZFileMode::ExternalFile)
	{
		int32_t rawDataSize =
			ZDisplayList::GetDListLength(parent->GetRawData(), rawDataIndex, dListType);
		numInstructions = rawDataSize / 8;
		ParseRawData();
	}

	Declaration* decl = DeclareVar("", "");
	decl->declaredInXml = true;
}

void ZDisplayList::ExtractFromBinary(uint32_t nRawDataIndex, int32_t rawDataSize)
{
	rawDataIndex = nRawDataIndex;
	name = GetDefaultName(parent->GetName());
	numInstructions = rawDataSize / 8;

	// Don't parse raw data of external files
	if (parent->GetMode() == ZFileMode::ExternalFile)
		return;

	ParseRawData();
}

void ZDisplayList::ParseRawData()
{
	const auto& rawData = parent->GetRawData();
	instructions.reserve(numInstructions);
	uint32_t ptr = rawDataIndex;

	for (size_t i = 0; i < numInstructions; i++)
	{
		instructions.push_back(BitConverter::ToUInt64BE(rawData, ptr));
		ptr += 8;
	}
}

Declaration* ZDisplayList::DeclareVar([[maybe_unused]] const std::string& prefix,
                                      const std::string& bodyStr)
{
	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), name, numInstructions, bodyStr);
	decl->isExternal = true;
	decl->staticConf = staticConf;
	return decl;
}

std::string ZDisplayList::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sDL_%06X", prefix.c_str(), rawDataIndex);
}

void ZDisplayList::ParseF3DZEX(F3DZEXOpcode opcode, uint64_t data, int32_t i,
                               const std::string& prefix, char* line)
{
	switch (opcode)
	{
	case F3DZEXOpcode::G_NOOP:
		sprintf(line, "gsDPNoOpTag(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
		break;
	case F3DZEXOpcode::G_DL:
		Opcode_G_DL(data, prefix, line);
		break;
	case F3DZEXOpcode::G_MODIFYVTX:
		Opcode_G_MODIFYVTX(data, line);
		break;
	case F3DZEXOpcode::G_CULLDL:
		Opcode_G_CULLDL(data, line);
		break;
	case F3DZEXOpcode::G_TRI1:
		Opcode_G_TRI1(data, line);
		break;
	case F3DZEXOpcode::G_TRI2:
		Opcode_G_TRI2(data, line);
		break;
	case F3DZEXOpcode::G_QUAD:
	{
		int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
		int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
		int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
		int32_t dd = ((data & 0x000000000000FFULL)) / 2;
		sprintf(line, "gsSP1Quadrangle(%i, %i, %i, %i, 0),", aa, bb, cc, dd);
	}
	break;
	case F3DZEXOpcode::G_VTX:
		Opcode_G_VTX(data, line);
		break;
	case F3DZEXOpcode::G_SETTIMG:  // HOTSPOT
		Opcode_G_SETTIMG(data, prefix, line);
		break;
	case F3DZEXOpcode::G_GEOMETRYMODE:
	{
		int32_t cccccc = (data & 0x00FFFFFF00000000) >> 32;
		int32_t ssssssss = (data & 0xFFFFFFFF);
		std::string geoModeStr = "G_TEXTURE_ENABLE";

		int32_t geoModeParam = ~cccccc;

		if (ssssssss != 0)
			geoModeParam = ssssssss;

		if (geoModeParam & 0x00000001)
			geoModeStr += " | G_ZBUFFER";

		if (geoModeParam & 0x00000004)
			geoModeStr += " | G_SHADE";

		if (geoModeParam & 0x00000200)
			geoModeStr += " | G_CULL_FRONT";

		if (geoModeParam & 0x00000400)
			geoModeStr += " | G_CULL_BACK";

		if (geoModeParam & 0x00010000)
			geoModeStr += " | G_FOG";

		if (geoModeParam & 0x00020000)
			geoModeStr += " | G_LIGHTING";

		if (geoModeParam & 0x00040000)
			geoModeStr += " | G_TEXTURE_GEN";

		if (geoModeParam & 0x00080000)
			geoModeStr += " | G_TEXTURE_GEN_LINEAR";

		if (geoModeParam & 0x00200000)
			geoModeStr += " | G_SHADING_SMOOTH";

		if (geoModeParam & 0x00800000)
			geoModeStr += " | G_CLIPPING";

		if (ssssssss != 0)
		{
			if ((~cccccc & 0xFF000000) != 0)
				sprintf(line, "gsSPSetGeometryMode(%s),", geoModeStr.c_str());
			else
				sprintf(line, "gsSPLoadGeometryMode(%s),", geoModeStr.c_str());
		}
		else
			sprintf(line, "gsSPClearGeometryMode(%s),", geoModeStr.c_str());
	}
	break;
	case F3DZEXOpcode::G_SETPRIMCOLOR:
		Opcode_G_SETPRIMCOLOR(data, line);
		break;
	case F3DZEXOpcode::G_SETOTHERMODE_L:
		Opcode_G_SETOTHERMODE_L(data, line);
		break;
	case F3DZEXOpcode::G_SETOTHERMODE_H:
		Opcode_G_SETOTHERMODE_H(data, line);
		break;
	case F3DZEXOpcode::G_SETTILE:
		Opcode_G_SETTILE(data, line);
		break;
	case F3DZEXOpcode::G_SETTILESIZE:
		Opcode_G_SETTILESIZE(data, prefix, line);
		break;
	case F3DZEXOpcode::G_LOADBLOCK:
		Opcode_G_LOADBLOCK(data, line);
		break;
	case F3DZEXOpcode::G_TEXTURE:
		Opcode_G_TEXTURE(data, line);
		break;
	case F3DZEXOpcode::G_RDPSETOTHERMODE:
	{
		int32_t hhhhhh = (data & 0x00FFFFFF00000000) >> 32;
		int32_t llllllll = (data & 0x00000000FFFFFFFF);

		sprintf(line, "gsDPSetOtherMode(%i, %i),", hhhhhh, llllllll);
	}
	break;
	case F3DZEXOpcode::G_POPMTX:
	{
		sprintf(line, "gsSPPopMatrix(%" PRIi64 "),", data);
	}
	break;
	case F3DZEXOpcode::G_LOADTLUT:
		Opcode_G_LOADTLUT(data, prefix, line);
		break;
	case F3DZEXOpcode::G_SETENVCOLOR:
	{
		uint8_t r = (uint8_t)((data & 0xFF000000) >> 24);
		uint8_t g = (uint8_t)((data & 0x00FF0000) >> 16);
		uint8_t b = (uint8_t)((data & 0xFF00FF00) >> 8);
		uint8_t a = (uint8_t)((data & 0x000000FF) >> 0);

		sprintf(line, "gsDPSetEnvColor(%i, %i, %i, %i),", r, g, b, a);
	}
	break;
	case F3DZEXOpcode::G_SETCOMBINE:
	{
		Opcode_G_SETCOMBINE(data, line);
	}
	break;
	case F3DZEXOpcode::G_RDPLOADSYNC:
		sprintf(line, "gsDPLoadSync(),");
		break;
	case F3DZEXOpcode::G_RDPPIPESYNC:
		sprintf(line, "gsDPPipeSync(),");
		break;
	case F3DZEXOpcode::G_RDPTILESYNC:
		sprintf(line, "gsDPTileSync(),");
		break;
	case F3DZEXOpcode::G_RDPFULLSYNC:
		sprintf(line, "gsDPFullSync(),");
		break;
	case F3DZEXOpcode::G_ENDDL:
		Opcode_G_ENDDL(prefix, line);
		break;
	case F3DZEXOpcode::G_RDPHALF_1:
	{
		uint64_t data2 = instructions[i + 1];
		uint32_t h = (data & 0xFFFFFFFF);
		F3DZEXOpcode opcode2 = (F3DZEXOpcode)(instructions[i + 1] >> 56);

		if (opcode2 == F3DZEXOpcode::G_BRANCH_Z)
		{
			uint32_t a = (data2 & 0x00FFF00000000000) >> 44;
			uint32_t b = (data2 & 0x00000FFF00000000) >> 32;
			uint32_t z = (data2 & 0x00000000FFFFFFFF) >> 0;

			// sprintf(line, "gsDPWord(%i, 0),", h);
			sprintf(line, "gsSPBranchLessZraw(%sDlist0x%06X, 0x%02X, 0x%02X),", prefix.c_str(),
			        h & 0x00FFFFFF, (a / 5) | (b / 2), z);

			ZDisplayList* nList = new ZDisplayList(parent);
			nList->ExtractFromBinary(
				h & 0x00FFFFFF, GetDListLength(parent->GetRawData(), h & 0x00FFFFFF, dListType));
			nList->SetName(nList->GetDefaultName(prefix));
			otherDLists.push_back(nList);

			i++;
		}
	}
	break;
	case F3DZEXOpcode::G_MTX:
		Opcode_G_MTX(data, line);
		break;
	default:
		sprintf(line, "// Opcode 0x%02X unimplemented!", (uint32_t)opcode);
		break;
	}
}

void ZDisplayList::ParseF3DEX(F3DEXOpcode opcode, uint64_t data, const std::string& prefix,
                              char* line)
{
	switch (opcode)
	{
	case F3DEXOpcode::G_NOOP:
		sprintf(line, "gsDPNoOpTag(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
		break;
	case F3DEXOpcode::G_VTX:
		Opcode_G_VTX(data, line);
		break;
	case F3DEXOpcode::G_DL:
		Opcode_G_DL(data, prefix, line);
		break;
	case F3DEXOpcode::G_CULLDL:
		Opcode_G_CULLDL(data, line);
		break;
	case F3DEXOpcode::G_MODIFYVTX:
		Opcode_G_MODIFYVTX(data, line);
		break;
	case F3DEXOpcode::G_MTX:
		Opcode_G_MTX(data, line);
		break;
	case F3DEXOpcode::G_TRI1:
		Opcode_G_TRI1(data, line);
		break;
	case F3DEXOpcode::G_TRI2:
		Opcode_G_TRI2(data, line);
		break;
	case F3DEXOpcode::G_ENDDL:
		Opcode_G_ENDDL(prefix, line);
		break;
	case F3DEXOpcode::G_RDPLOADSYNC:
		sprintf(line, "gsDPLoadSync(),");
		break;
	case F3DEXOpcode::G_RDPPIPESYNC:
		sprintf(line, "gsDPPipeSync(),");
		break;
	case F3DEXOpcode::G_RDPTILESYNC:
		sprintf(line, "gsDPTileSync(),");
		break;
	case F3DEXOpcode::G_RDPFULLSYNC:
		sprintf(line, "gsDPFullSync(),");
		break;
	case F3DEXOpcode::G_TEXTURE:
		Opcode_G_TEXTURE(data, line);
		break;
	case F3DEXOpcode::G_SETTIMG:
		Opcode_G_SETTIMG(data, prefix, line);
		break;
	case F3DEXOpcode::G_SETTILE:
		Opcode_G_SETTILE(data, line);
		break;
	case F3DEXOpcode::G_SETTILESIZE:
		Opcode_G_SETTILESIZE(data, prefix, line);
		break;
	case F3DEXOpcode::G_LOADBLOCK:
		Opcode_G_LOADBLOCK(data, line);
		break;
	case F3DEXOpcode::G_SETCOMBINE:
		Opcode_G_SETCOMBINE(data, line);
		break;
	case F3DEXOpcode::G_SETPRIMCOLOR:
		Opcode_G_SETPRIMCOLOR(data, line);
		break;
	case F3DEXOpcode::G_SETOTHERMODE_L:
		Opcode_G_SETOTHERMODE_L(data, line);
		break;
	case F3DEXOpcode::G_SETOTHERMODE_H:
		Opcode_G_SETOTHERMODE_H(data, line);
		break;
	case F3DEXOpcode::G_LOADTLUT:
		Opcode_G_LOADTLUT(data, prefix, line);
		break;
	case F3DEXOpcode::G_CLEARGEOMETRYMODE:
	case F3DEXOpcode::G_SETGEOMETRYMODE:
	{
		int32_t cccccc = (data & 0x00FFFFFF00000000) >> 32;
		int32_t ssssssss = (data & 0xFFFFFFFF);
		std::string geoModeStr = "G_TEXTURE_ENABLE";

		int32_t geoModeParam = ~cccccc;

		if (ssssssss != 0)
			geoModeParam = ssssssss;

		if (geoModeParam & 0x00000002)
			geoModeStr += " | G_TEXTURE_ENABLE";

		if (geoModeParam & 0x00000200)
			geoModeStr += " | G_SHADING_SMOOTH";

		if (geoModeParam & 0x00001000)
			geoModeStr += " | G_CULL_FRONT";

		if (geoModeParam & 0x00002000)
			geoModeStr += " | G_CULL_BACK";

		if (geoModeParam & 0x00000001)
			geoModeStr += " | G_ZBUFFER";

		if (geoModeParam & 0x00000004)
			geoModeStr += " | G_SHADE";

		if (geoModeParam & 0x00010000)
			geoModeStr += " | G_FOG";

		if (geoModeParam & 0x00020000)
			geoModeStr += " | G_LIGHTING";

		if (geoModeParam & 0x00040000)
			geoModeStr += " | G_TEXTURE_GEN";

		if (geoModeParam & 0x00080000)
			geoModeStr += " | G_TEXTURE_GEN_LINEAR";

		if (geoModeParam & 0x00800000)
			geoModeStr += " | G_CLIPPING";

		if (opcode == F3DEXOpcode::G_SETGEOMETRYMODE)
			sprintf(line, "gsSPSetGeometryMode(%s),", geoModeStr.c_str());
		else
			sprintf(line, "gsSPClearGeometryMode(%s),", geoModeStr.c_str());
	}
	break;
	default:
		sprintf(line, "// Opcode 0x%02X unimplemented!", (uint32_t)opcode);
		break;
	}
}

int32_t ZDisplayList::GetDListLength(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex,
                                     DListType dListType)
{
	uint8_t endDLOpcode;
	uint8_t branchListOpcode;

	if (dListType == DListType::F3DZEX)
	{
		endDLOpcode = static_cast<uint8_t>(F3DZEXOpcode::G_ENDDL);
		branchListOpcode = static_cast<uint8_t>(F3DZEXOpcode::G_DL);
	}
	else
	{
		endDLOpcode = static_cast<uint8_t>(F3DEXOpcode::G_ENDDL);
		branchListOpcode = static_cast<uint8_t>(F3DEXOpcode::G_DL);
	}

	uint32_t ptr = rawDataIndex;
	size_t rawDataSize = rawData.size();
	while (true)
	{
		if (ptr >= rawDataSize)
		{
			std::string errorHeader =
				StringHelper::Sprintf("reached end of file when trying to find the end of the "
			                          "DisplayList starting at offset 0x%X",
			                          rawDataIndex);
			std::string errorBody = StringHelper::Sprintf("Raw data size: 0x%zX.", rawDataSize);
			HANDLE_ERROR_PROCESS(WarningType::Always, errorHeader, errorBody);
		}

		uint8_t opcode = rawData.at(ptr);
		bool dlNoPush = rawData.at(ptr + 1) == 1;
		ptr += 8;

		if (opcode == endDLOpcode || (opcode == branchListOpcode && dlNoPush))
		{
			return ptr - rawDataIndex;
		}
	}
}

bool ZDisplayList::SequenceCheck(std::vector<F3DZEXOpcode> sequence, int32_t startIndex)
{
	bool success = true;

	for (size_t j = 0; j < sequence.size(); j++)
	{
		F3DZEXOpcode opcode = (F3DZEXOpcode)(instructions[startIndex + j] >> 56);

		if (sequence[j] != opcode)
		{
			success = false;
			break;
		}
	}

	if (success)
		return true;

	return false;
}

int32_t ZDisplayList::OptimizationChecks(int32_t startIndex, std::string& output,
                                         const std::string& prefix)
{
	int32_t result = -1;

	result = OptimizationCheck_LoadTextureBlock(startIndex, output, prefix);

	if (result != -1)
		return result;

	return -1;
}

int32_t ZDisplayList::OptimizationCheck_LoadTextureBlock(int32_t startIndex, std::string& output,
                                                         [[maybe_unused]] const std::string& prefix)
{
	std::vector<F3DZEXOpcode> sequence = {F3DZEXOpcode::G_SETTIMG,     F3DZEXOpcode::G_SETTILE,
	                                      F3DZEXOpcode::G_RDPLOADSYNC, F3DZEXOpcode::G_LOADBLOCK,
	                                      F3DZEXOpcode::G_RDPPIPESYNC, F3DZEXOpcode::G_SETTILE,
	                                      F3DZEXOpcode::G_SETTILESIZE};

	bool seqRes = SequenceCheck(sequence, startIndex);

	if (seqRes)
	{
		// gsDPLoadTextureBlock(texAddr, fmt, siz, width, height, pal, cms, cmt, masks, maskt,
		// shifts, shiftt) gsDPLoadMultiBlock(texAddr, tmem, rtile, fmt, siz, width, height, pal,
		// cms, cmt, masks, maskt, shifts, shiftt) gsDPLoadTextureBlock_4b(texAddr, fmt, width,
		// height, pal, cms, cmt, masks, maskt, shifts, shiftt) gsDPLoadMultiBlock_4b(texAddr, tmem,
		// rtile, fmt, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt)

		uint32_t texAddr, tmem, rtile, fmt, siz, sizB, width, height, width2, height2, pal, cms,
			cmt, masks, maskt, shifts, shiftt;
		std::string texStr;

		// gsDPSetTextureImage
		{
			uint64_t data = instructions[startIndex + 0];

			int32_t __ = (data & 0x00FF000000000000) >> 48;
			// int32_t www = (data & 0x00000FFF00000000) >> 32;

			fmt = (__ & 0xE0) >> 5;
			siz = (__ & 0x18) >> 3;
			texAddr = Seg2Filespace(data, parent->baseAddress);
			uint32_t segmentNumber = GETSEGNUM(data);

			lastTexSeg = segmentNumber;

			Globals::Instance->GetSegmentedPtrName(data & 0xFFFFFFFF, parent, "", texStr,
			                                       parent->workerID);
		}

		// gsDPSetTile
		{
			uint64_t data = instructions[startIndex + 1];

			tmem =
				(data & 0b0000000000000000111111111111111100000000000000000000000000000000) >> 32;

			cmt = (data & 0b0000000000000000000000000000000000000000000011000000000000000000) >> 18;
			maskt =
				(data & 0b0000000000000000000000000000000000000000000000111100000000000000) >> 14;
			shiftt =
				(data & 0b0000000000000000000000000000000000000000000000000011110000000000) >> 10;
			cms = (data & 0b0000000000000000000000000000000000000000000000000000001100000000) >> 8;
			masks =
				(data & 0b0000000000000000000000000000000000000000000000000000000011110000) >> 4;
			shifts = (data & 0b0000000000000000000000000000000000000000000000000000000000001111);

			// sprintf(line, "gsDPSetTile(%s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
			// fmtTbl[fff].c_str(), sizTbl[ii].c_str(), nnnnnnnnn, mmmmmmmmm, ttt, pppp, cc, aaaa,
			// ssss, dd, bbbb, uuuu);
		}

		// gsDPLoadSync

		// gsDPLoadBlock

		// gsDPPipeSync

		// gsDPSetTile
		{
			uint64_t data = instructions[startIndex + 5];
			int32_t __ = (data & 0x00FF000000000000) >> 48;
			pal = (data & 0b0000000000000000000000000000000000000000111100000000000000000000) >> 20;
			// siz = (__ & 0x18) >> 3;
			rtile =
				(data & 0b0000000000000000000000000000000011111111000000000000000000000000) >> 24;
			sizB = (__ & 0x18) >> 3;
		}

		// gsDPSetTileSize
		{
			uint64_t data = instructions[startIndex + 6];
			int32_t uuu = (data & 0x0000000000FFF000) >> 12;
			int32_t vvv = (data & 0x0000000000000FFF);

			int32_t shiftAmtW = 2;
			int32_t shiftAmtH = 2;

			if (sizB == (int32_t)F3DZEXTexSizes::G_IM_SIZ_8b &&
			    fmt == (int32_t)F3DZEXTexFormats::G_IM_FMT_IA)
				shiftAmtW = 3;

			if (sizB == (int32_t)F3DZEXTexSizes::G_IM_SIZ_4b)
				shiftAmtW = 3;

			if (sizB == (int32_t)F3DZEXTexSizes::G_IM_SIZ_4b &&
			    fmt == (int32_t)F3DZEXTexFormats::G_IM_FMT_IA)
				shiftAmtH = 3;

			width = (uuu >> shiftAmtW) + 1;
			height = (vvv >> shiftAmtH) + 1;

			width2 = (uuu >> 2) + 1;
			height2 = (vvv >> 2) + 1;
		}

		const char* fmtTbl[] = {"G_IM_FMT_RGBA", "G_IM_FMT_YUV", "G_IM_FMT_CI", "G_IM_FMT_IA",
		                        "G_IM_FMT_I"};
		const char* sizTbl[] = {"G_IM_SIZ_4b", "G_IM_SIZ_8b", "G_IM_SIZ_16b", "G_IM_SIZ_32b"};

		// output += StringHelper::Sprintf("gsDPLoadTextureBlock(%s, %s, %s, %i, %i, %i, %i, %i, %i,
		// %i, %i, %i),", texStr.c_str(), fmtTbl[fmt], sizTbl[siz].c_str(), width, height,
		// pal, cms, cmt, masks, maskt, shifts, shiftt);

		if (siz == 2 && sizB == 0)
		{
			if (tmem != 0)
				output += StringHelper::Sprintf(
					"gsDPLoadMultiBlock_4b(%s, %i, %i, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
					texStr.c_str(), tmem, rtile, fmtTbl[fmt], width2, height2, pal, cms, cmt, masks,
					maskt, shifts, shiftt);
			else
				output += StringHelper::Sprintf(
					"gsDPLoadTextureBlock_4b(%s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
					texStr.c_str(), fmtTbl[fmt], width2, height2, pal, cms, cmt, masks, maskt,
					shifts, shiftt);
		}
		else if (siz == 2 && sizB != 0)
		{
			if (tmem != 0)
				output += StringHelper::Sprintf(
					"gsDPLoadMultiBlock(%s, %i, %i, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
					texStr.c_str(), tmem, rtile, fmtTbl[fmt], sizTbl[sizB], width2, height2, pal,
					cms, cmt, masks, maskt, shifts, shiftt);
			else
				output += StringHelper::Sprintf(
					"gsDPLoadTextureBlock(%s, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
					texStr.c_str(), fmtTbl[fmt], sizTbl[sizB], width2, height2, pal, cms, cmt,
					masks, maskt, shifts, shiftt);
		}
		else
		{
			if (siz != sizB)
			{
				lastTexAddr = texAddr;
				lastTexFmt = (F3DZEXTexFormats)fmt;
				lastTexWidth = width;
				lastTexHeight = height;
				lastTexSiz = (F3DZEXTexSizes)siz;
				lastTexLoaded = true;

				TextureGenCheck();

				return -1;
			}

			output += StringHelper::Sprintf(
				"gsDPLoadMultiBlock(%s, %i, %i, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
				texStr.c_str(), tmem, rtile, fmtTbl[fmt], sizTbl[siz], width, height, pal, cms, cmt,
				masks, maskt, shifts, shiftt);
		}

		lastTexAddr = texAddr;
		lastTexFmt = (F3DZEXTexFormats)fmt;
		lastTexWidth = width;
		lastTexHeight = height;
		lastTexSiz = (F3DZEXTexSizes)siz;
		lastTexLoaded = true;

		TextureGenCheck();

		return (int32_t)sequence.size();
	}

	return -1;
}

void ZDisplayList::Opcode_G_DL(uint64_t data, const std::string& prefix, char* line)
{
	int32_t pp = (data & 0x00FF000000000000) >> 56;
	int32_t segNum = GETSEGNUM(data);

	Declaration* dListDecl = nullptr;

	if (parent != nullptr)
		dListDecl = parent->GetDeclaration(GETSEGOFFSET(data));

	if (pp != 0)
	{
		if (!Globals::Instance->HasSegment(segNum, parent->workerID))
			sprintf(line, "gsSPBranchList(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
		else if (dListDecl != nullptr)
			sprintf(line, "gsSPBranchList(%s),", dListDecl->varName.c_str());
		else
			sprintf(line, "gsSPBranchList(%sDlist0x%06" PRIX64 "),", prefix.c_str(),
			        GETSEGOFFSET(data));
	}
	else
	{
		if (!Globals::Instance->HasSegment(segNum, parent->workerID))
			sprintf(line, "gsSPDisplayList(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
		else if (dListDecl != nullptr)
			sprintf(line, "gsSPDisplayList(%s),", dListDecl->varName.c_str());
		else
			sprintf(line, "gsSPDisplayList(%sDlist0x%06" PRIX64 "),", prefix.c_str(),
			        GETSEGOFFSET(data));
	}

	// if (segNum == 8 || segNum == 9 || segNum == 10 || segNum == 11 || segNum == 12 || segNum ==
	// 13) // Used for runtime-generated display lists
	if (!Globals::Instance->HasSegment(segNum, parent->workerID))
	{
		if (pp != 0)
			sprintf(line, "gsSPBranchList(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
		else
			sprintf(line, "gsSPDisplayList(0x%08" PRIX64 "),", data & 0xFFFFFFFF);
	}
	else
	{
		ZDisplayList* nList = new ZDisplayList(parent);
		nList->ExtractFromBinary(GETSEGOFFSET(data), GetDListLength(parent->GetRawData(),
		                                                            GETSEGOFFSET(data), dListType));
		nList->SetName(nList->GetDefaultName(prefix));

		otherDLists.push_back(nList);
	}
}

void ZDisplayList::Opcode_G_MODIFYVTX(uint64_t data, char* line)
{
	int32_t ww = (data & 0x00FF000000000000ULL) >> 48;
	int32_t nnnn = (data & 0x0000FFFF00000000ULL) >> 32;
	int32_t vvvvvvvv = (data & 0x00000000FFFFFFFFULL);

	sprintf(line, "gsSPModifyVertex(%i, %i, %i),", nnnn / 2, ww, vvvvvvvv);
}

void ZDisplayList::Opcode_G_CULLDL(uint64_t data, char* line)
{
	int32_t vvvv = (data & 0xFFFF00000000) >> 32;
	int32_t wwww = (data & 0x0000FFFF);

	sprintf(line, "gsSPCullDisplayList(%i, %i),", vvvv / 2, wwww / 2);
}

void ZDisplayList::Opcode_G_TRI1(uint64_t data, char* line)
{
	if (dListType == DListType::F3DZEX)
	{
		int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
		int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
		int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
		sprintf(line, "gsSP1Triangle(%i, %i, %i, 0),", aa, bb, cc);
	}
	else
	{
		int32_t aa = ((data & 0x0000000000FF0000ULL) >> 16) / 2;
		int32_t bb = ((data & 0x000000000000FF00ULL) >> 8) / 2;
		int32_t cc = ((data & 0x00000000000000FFULL) >> 0) / 2;
		sprintf(line, "gsSP1Triangle(%i, %i, %i, 0),", aa, bb, cc);
	}
}

void ZDisplayList::Opcode_G_TRI2(uint64_t data, char* line)
{
	int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
	int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
	int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
	int32_t dd = ((data & 0x00000000FF0000ULL) >> 16) / 2;
	int32_t ee = ((data & 0x0000000000FF00ULL) >> 8) / 2;
	int32_t ff = ((data & 0x000000000000FFULL) >> 0) / 2;
	sprintf(line, "gsSP2Triangles(%i, %i, %i, 0, %i, %i, %i, 0),", aa, bb, cc, dd, ee, ff);
}

void ZDisplayList::Opcode_G_MTX(uint64_t data, char* line)
{
	uint32_t pp = 0;
	uint32_t mm = (data & 0x00000000FFFFFFFF);
	bool push = false;
	bool load = false;
	bool projection = false;

	if (dListType == DListType::F3DEX)
		pp = (data & 0x00FF000000000000) >> 48;
	else
		pp = (data & 0x000000FF00000000) >> 32;

	std::string matrixRef;

	if (Globals::Instance->cfg.symbolMap.find(mm) != Globals::Instance->cfg.symbolMap.end())
		matrixRef = StringHelper::Sprintf("&%s", Globals::Instance->cfg.symbolMap[mm].c_str());
	else
		matrixRef = StringHelper::Sprintf("0x%08X", mm);

	pp ^= 0x01;

	if (pp & 0x01)
		push = true;

	if (pp & 0x02)
		load = true;

	if (pp & 0x04)
		projection = true;

	sprintf(line, "gsSPMatrix(%s, %s | %s | %s),", matrixRef.c_str(),
	        projection ? "G_MTX_PROJECTION" : "G_MTX_MODELVIEW",
	        push ? "G_MTX_PUSH" : "G_MTX_NOPUSH", load ? "G_MTX_LOAD" : "G_MTX_MUL");
}

void ZDisplayList::Opcode_G_VTX(uint64_t data, char* line)
{
	int32_t nn = (data & 0x000FF00000000000ULL) >> 44;
	int32_t aa = (data & 0x000000FF00000000ULL) >> 32;

	uint32_t vtxAddr = Seg2Filespace(data, parent->baseAddress);

	if (dListType == DListType::F3DZEX)
		sprintf(line, "gsSPVertex(@r, %i, %i),", nn, ((aa >> 1) - nn));
	else
	{
		uint32_t hi = data >> 32;

#define _SHIFTR(v, s, w) (((uint32_t)v >> s) & ((0x01 << w) - 1))

		nn = _SHIFTR(hi, 10, 6);

		sprintf(line, "gsSPVertex(@r, %i, %i),", nn, _SHIFTR(hi, 17, 7));
	}

	// Hack: Don't extract vertices from a unknown segment.
	if (!Globals::Instance->HasSegment(GETSEGNUM(data), parent->workerID))
	{
		segptr_t segmented = data & 0xFFFFFFFF;
		references.push_back(segmented);
		parent->AddDeclaration(segmented, DeclarationAlignment::Align8, 16, "Vtx",
		                       StringHelper::Sprintf("0x%08X", segmented), "");
		return;
	}
	references.push_back(data);

	{
		uint32_t currentPtr = Seg2Filespace(data, parent->baseAddress);
		Declaration* decl;

		// Check for vertex intersections from other display lists
		// TODO: These two could probably be condenced to one...
		decl = parent->GetDeclarationRanged(vtxAddr + (nn * 16));
		if (decl != nullptr)
		{
			int32_t diff = decl->address - vtxAddr;
			if (diff > 0)
				nn = diff / 16;
			else
				nn = 0;
		}

		decl = parent->GetDeclarationRanged(vtxAddr);
		if (decl != nullptr)
		{
			int32_t diff = decl->address - vtxAddr;
			if (diff > 0)
				nn = diff / 16;
			else
				nn = 0;
		}

		if (nn > 0)
		{
			std::vector<ZVtx> vtxList;
			vtxList.reserve(nn);

			for (int32_t i = 0; i < nn; i++)
			{
				ZVtx vtx(parent);
				vtx.ExtractFromFile(currentPtr);
				vtxList.push_back(vtx);

				currentPtr += 16;
			}

			vertices[vtxAddr] = vtxList;
		}
	}
}

void ZDisplayList::Opcode_G_TEXTURE(uint64_t data, char* line)
{
	int32_t ____ = (data & 0x0000FFFF00000000) >> 32;
	int32_t ssss = (data & 0x00000000FFFF0000) >> 16;
	int32_t tttt = (data & 0x000000000000FFFF);
	int32_t lll = (____ & 0x3800) >> 11;
	int32_t ddd = (____ & 0x700) >> 8;
	int32_t nnnnnnn = 0;

	if (dListType == DListType::F3DEX)
		nnnnnnn = (____ & 0xFF);
	else
		nnnnnnn = (____ & 0xFE) >> 1;

	sprintf(line, "gsSPTexture(%i, %i, %i, %i, %s),", ssss, tttt, lll, ddd,
	        nnnnnnn == 1 ? "G_ON" : "G_OFF");
}

void ZDisplayList::Opcode_G_SETTIMG(uint64_t data, const std::string& prefix, char* line)
{
	int32_t __ = (data & 0x00FF000000000000) >> 48;
	int32_t www = (data & 0x00000FFF00000000) >> 32;
	const char* fmtTbl[] = {"G_IM_FMT_RGBA", "G_IM_FMT_YUV", "G_IM_FMT_CI", "G_IM_FMT_IA",
	                        "G_IM_FMT_I"};
	const char* sizTbl[] = {"G_IM_SIZ_4b", "G_IM_SIZ_8b", "G_IM_SIZ_16b", "G_IM_SIZ_32b"};

	uint32_t fmt = (__ & 0xE0) >> 5;
	uint32_t siz = (__ & 0x18) >> 3;

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("TextureGenCheck G_SETTIMG\n");

	TextureGenCheck();  // HOTSPOT

	lastTexFmt = (F3DZEXTexFormats)fmt;
	lastTexSiz = (F3DZEXTexSizes)siz;
	lastTexSeg = data;
	lastTexAddr = Seg2Filespace(data, parent->baseAddress);

	int32_t segmentNumber = GETSEGNUM(data);

	if (segmentNumber != 2)
	{
		char texStr[2048];
		int32_t texAddress = Seg2Filespace(data, parent->baseAddress);
		Declaration* texDecl = nullptr;

		if (parent != nullptr)
		{
			if (Globals::Instance->HasSegment(segmentNumber, parent->workerID))
				texDecl = parent->GetDeclaration(texAddress);
			else
				texDecl = parent->GetDeclaration(data);
		}

		if (texDecl != nullptr)
			sprintf(texStr, "%s", texDecl->varName.c_str());
		else if (data != 0 && Globals::Instance->HasSegment(segmentNumber, parent->workerID))
			sprintf(texStr, "%sTex_%06X", prefix.c_str(), texAddress);
		else
		{
			sprintf(texStr, "0x%08" PRIX64, data & 0xFFFFFFFF);
		}

		sprintf(line, "gsDPSetTextureImage(%s, %s, %i, %s),", fmtTbl[fmt], sizTbl[siz], www + 1,
		        texStr);
	}
	else
	{
		std::string texName;
		Globals::Instance->GetSegmentedPtrName(data, parent, "", texName, parent->workerID);
		sprintf(line, "gsDPSetTextureImage(%s, %s, %i, %s),", fmtTbl[fmt], sizTbl[siz], www + 1,
		        texName.c_str());
	}
}

void ZDisplayList::Opcode_G_SETTILE(uint64_t data, char* line)
{
	int32_t fff = (data & 0b0000000011100000000000000000000000000000000000000000000000000000) >> 53;
	int32_t ii = (data & 0b0000000000011000000000000000000000000000000000000000000000000000) >> 51;
	int32_t nnnnnnnnn =
		(data & 0b0000000000000011111111100000000000000000000000000000000000000000) >> 41;
	int32_t mmmmmmmmm =
		(data & 0b0000000000000000000000011111111100000000000000000000000000000000) >> 32;
	int32_t ttt = (data & 0b0000000000000000000000000000000000000111000000000000000000000000) >> 24;
	int32_t pppp =
		(data & 0b0000000000000000000000000000000000000000111100000000000000000000) >> 20;
	int32_t cc = (data & 0b0000000000000000000000000000000000000000000011000000000000000000) >> 18;
	int32_t aaaa =
		(data & 0b0000000000000000000000000000000000000000000000111100000000000000) >> 14;
	int32_t ssss =
		(data & 0b0000000000000000000000000000000000000000000000000011110000000000) >> 10;
	int32_t dd = (data & 0b0000000000000000000000000000000000000000000000000000001100000000) >> 8;
	int32_t bbbb = (data & 0b0000000000000000000000000000000000000000000000000000000011110000) >> 4;
	int32_t uuuu = (data & 0b0000000000000000000000000000000000000000000000000000000000001111);

	const char* fmtTbl[] = {"G_IM_FMT_RGBA", "G_IM_FMT_YUV", "G_IM_FMT_CI", "G_IM_FMT_IA",
	                        "G_IM_FMT_I"};
	const char* sizTbl[] = {"G_IM_SIZ_4b", "G_IM_SIZ_8b", "G_IM_SIZ_16b", "G_IM_SIZ_32b"};

	if (fff == (int32_t)F3DZEXTexFormats::G_IM_FMT_CI)
		lastCISiz = (F3DZEXTexSizes)ii;

	lastTexSizTest = (F3DZEXTexSizes)ii;

	sprintf(line, "gsDPSetTile(%s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", fmtTbl[fff],
	        sizTbl[ii], nnnnnnnnn, mmmmmmmmm, ttt, pppp, cc, aaaa, ssss, dd, bbbb, uuuu);
}

void ZDisplayList::Opcode_G_SETTILESIZE(uint64_t data, [[maybe_unused]] const std::string& prefix,
                                        char* line)
{
	int32_t sss = (data & 0x00FFF00000000000) >> 44;
	int32_t ttt = (data & 0x00000FFF00000000) >> 32;
	int32_t uuu = (data & 0x0000000000FFF000) >> 12;
	int32_t vvv = (data & 0x0000000000000FFF);
	int32_t i = (data & 0x000000000F000000) >> 24;

	int32_t shiftAmtW = 2;
	int32_t shiftAmtH = 2;

	if (lastTexSizTest == F3DZEXTexSizes::G_IM_SIZ_8b &&
	    lastTexFmt == F3DZEXTexFormats::G_IM_FMT_IA)
		shiftAmtW = 3;

	// if (lastTexFmt == F3DZEXTexFormats::G_IM_FMT_I || lastTexFmt ==
	// F3DZEXTexFormats::G_IM_FMT_CI)
	if (lastTexSizTest == F3DZEXTexSizes::G_IM_SIZ_4b)
		shiftAmtW = 3;

	if (lastTexSizTest == F3DZEXTexSizes::G_IM_SIZ_4b &&
	    lastTexFmt == F3DZEXTexFormats::G_IM_FMT_IA)
		shiftAmtH = 3;

	lastTexWidth = (uuu >> shiftAmtW) + 1;
	lastTexHeight = (vvv >> shiftAmtH) + 1;

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("lastTexWidth: %i lastTexHeight: %i, lastTexSizTest: 0x%x, lastTexFmt: 0x%x\n",
		       lastTexWidth, lastTexHeight, (uint32_t)lastTexSizTest, (uint32_t)lastTexFmt);

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("TextureGenCheck G_SETTILESIZE\n");

	TextureGenCheck();

	sprintf(line, "gsDPSetTileSize(%i, %i, %i, %i, %i),", i, sss, ttt, uuu, vvv);
}

void ZDisplayList::Opcode_G_LOADBLOCK(uint64_t data, char* line)
{
	int32_t sss = (data & 0x00FFF00000000000) >> 48;
	int32_t ttt = (data & 0x00000FFF00000000) >> 36;
	int32_t i = (data & 0x000000000F000000) >> 24;
	int32_t xxx = (data & 0x0000000000FFF000) >> 12;
	int32_t ddd = (data & 0x0000000000000FFF);

	lastTexLoaded = true;

	sprintf(line, "gsDPLoadBlock(%i, %i, %i, %i, %i),", i, sss, ttt, xxx, ddd);
}

void ZDisplayList::Opcode_G_SETCOMBINE(uint64_t data, char* line)
{
	int32_t a0 = (data & 0b000000011110000000000000000000000000000000000000000000000000000) >> 52;
	int32_t c0 = (data & 0b000000000001111100000000000000000000000000000000000000000000000) >> 47;
	int32_t aa0 = (data & 0b00000000000000011100000000000000000000000000000000000000000000) >> 44;
	int32_t ac0 = (data & 0b00000000000000000011100000000000000000000000000000000000000000) >> 41;
	int32_t a1 = (data & 0b000000000000000000000011110000000000000000000000000000000000000) >> 37;
	int32_t c1 = (data & 0b000000000000000000000000001111100000000000000000000000000000000) >> 32;
	int32_t b0 = (data & 0b000000000000000000000000000000011110000000000000000000000000000) >> 28;
	int32_t b1 = (data & 0b000000000000000000000000000000000001111000000000000000000000000) >> 24;
	int32_t aa1 = (data & 0b00000000000000000000000000000000000000111000000000000000000000) >> 21;
	int32_t ac1 = (data & 0b00000000000000000000000000000000000000000111000000000000000000) >> 18;
	int32_t d0 = (data & 0b000000000000000000000000000000000000000000000111000000000000000) >> 15;
	int32_t ab0 = (data & 0b00000000000000000000000000000000000000000000000111000000000000) >> 12;
	int32_t ad0 = (data & 0b00000000000000000000000000000000000000000000000000111000000000) >> 9;
	int32_t d1 = (data & 0b000000000000000000000000000000000000000000000000000000111000000) >> 6;
	int32_t ab1 = (data & 0b00000000000000000000000000000000000000000000000000000000111000) >> 3;
	int32_t ad1 = (data & 0b00000000000000000000000000000000000000000000000000000000000111) >> 0;

	const char* modesA[] = {"COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT",
	                        "1",        "NOISE",  "0",      "9",         "10",    "11",
	                        "12",       "13",     "14",     "0"};
	const char* modesB[] = {"COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT",
	                        "CENTER",   "K4",     "8",      "9",         "10",    "11",
	                        "12",       "13",     "14",     "0"};
	const char* modesC[] = {"COMBINED",
	                        "TEXEL0",
	                        "TEXEL1",
	                        "PRIMITIVE",
	                        "SHADE",
	                        "ENVIRONMENT",
	                        "1",
	                        "COMBINED_ALPHA",
	                        "TEXEL0_ALPHA",
	                        "TEXEL1_ALPHA",
	                        "PRIMITIVE_ALPHA",
	                        "SHADE_ALPHA",
	                        "ENV_ALPHA",
	                        "LOD_FRACTION",
	                        "PRIM_LOD_FRAC",
	                        "K5",
	                        "16",
	                        "17",
	                        "18",
	                        "19",
	                        "20",
	                        "21",
	                        "22",
	                        "23",
	                        "24",
	                        "25",
	                        "26",
	                        "27",
	                        "28",
	                        "29",
	                        "30",
	                        "0"};
	const char* modesD[] = {"COMBINED", "TEXEL0",      "TEXEL1", "PRIMITIVE",
	                        "SHADE",    "ENVIRONMENT", "1",      "0"};

	const char* modes2[] = {"COMBINED", "TEXEL0",      "TEXEL1", "PRIMITIVE",
	                        "SHADE",    "ENVIRONMENT", "1",      "0"};
	const char* modes2C[] = {"LOD_FRACTION", "TEXEL0",      "TEXEL1",        "PRIMITIVE",
	                         "SHADE",        "ENVIRONMENT", "PRIM_LOD_FRAC", "0"};

	sprintf(line,
	        "gsDPSetCombineLERP(%s, %s, %s, %s, %s, %s, %s, %s,\n                       %s, %s, "
	        "%s, %s, %s, %s, %s, %s),",
	        modesA[a0], modesB[b0], modesC[c0], modesD[d0], modes2[aa0], modes2[ab0], modes2C[ac0],
	        modes2[ad0], modesA[a1], modesB[b1], modesC[c1], modesD[d1], modes2[aa1], modes2[ab1],
	        modes2C[ac1], modes2[ad1]);
}

void ZDisplayList::Opcode_G_SETPRIMCOLOR(uint64_t data, char* line)
{
	int32_t mm = (data & 0x0000FF0000000000) >> 40;
	int32_t ff = (data & 0x000000FF00000000) >> 32;
	int32_t rr = (data & 0x00000000FF000000) >> 24;
	int32_t gg = (data & 0x0000000000FF0000) >> 16;
	int32_t bb = (data & 0x000000000000FF00) >> 8;
	int32_t aa = (data & 0x00000000000000FF) >> 0;
	sprintf(line, "gsDPSetPrimColor(%i, %i, %i, %i, %i, %i),", mm, ff, rr, gg, bb, aa);
}

void ZDisplayList::Opcode_F3DEX_G_SETOTHERMODE_L(uint64_t data, char* line)
{
	int32_t sft = (data & 0x0000FF0000000000) >> 40;
	int32_t len = (data & 0x000000FF00000000) >> 32;
	int32_t dat = (data & 0xFFFFFFFF);

	// TODO: Output the correct render modes in data

	sprintf(line, "gsSPSetOtherMode(0xE2, %i, %i, 0x%08X),", sft, len, dat);
}

void ZDisplayList::Opcode_G_SETOTHERMODE_L(uint64_t data, char* line)
{
	int32_t dd = (data & 0xFFFFFFFF);
	int32_t sft = 0;
	int32_t len = 0;

	if (dListType == DListType::F3DEX)
	{
		sft = (data & 0x0000FF0000000000) >> 40;
		len = (data & 0x000000FF00000000) >> 32;
	}
	else
	{
		int32_t ss = (data & 0x0000FF0000000000) >> 40;
		len = ((data & 0x000000FF00000000) >> 32) + 1;
		sft = 32 - (len)-ss;
	}

	if (sft == G_MDSFT_RENDERMODE)
	{
		uint32_t mode1 = (dd & 0xCCCC0000) >> 0;
		uint32_t mode2 = (dd & 0x3333FFFF);

		// TODO: Jesus Christ This is Messy

		uint32_t tblA[] = {G_RM_FOG_SHADE_A,
		                   G_RM_FOG_PRIM_A,
		                   G_RM_PASS,
		                   G_RM_AA_ZB_OPA_SURF,
		                   G_RM_AA_ZB_XLU_SURF,
		                   G_RM_AA_ZB_OPA_DECAL,
		                   G_RM_AA_ZB_XLU_DECAL,
		                   G_RM_AA_ZB_OPA_INTER,
		                   G_RM_AA_ZB_XLU_INTER,
		                   G_RM_AA_ZB_XLU_LINE,
		                   G_RM_AA_ZB_DEC_LINE,
		                   G_RM_AA_ZB_TEX_EDGE,
		                   G_RM_AA_ZB_TEX_INTER,
		                   G_RM_AA_ZB_SUB_SURF,
		                   G_RM_AA_ZB_PCL_SURF,
		                   G_RM_AA_ZB_OPA_TERR,
		                   G_RM_AA_ZB_TEX_TERR,
		                   G_RM_AA_ZB_SUB_TERR,
		                   G_RM_RA_ZB_OPA_SURF,
		                   G_RM_RA_ZB_OPA_DECAL,
		                   G_RM_RA_ZB_OPA_INTER,
		                   G_RM_AA_OPA_SURF,
		                   G_RM_AA_XLU_SURF,
		                   G_RM_AA_XLU_LINE,
		                   G_RM_AA_DEC_LINE,
		                   G_RM_AA_TEX_EDGE,
		                   G_RM_AA_SUB_SURF,
		                   G_RM_AA_PCL_SURF,
		                   G_RM_AA_OPA_TERR,
		                   G_RM_AA_TEX_TERR,
		                   G_RM_AA_SUB_TERR,
		                   G_RM_RA_OPA_SURF,
		                   G_RM_ZB_OPA_SURF,
		                   G_RM_ZB_XLU_SURF,
		                   G_RM_ZB_OPA_DECAL,
		                   G_RM_ZB_XLU_DECAL,
		                   G_RM_ZB_CLD_SURF,
		                   G_RM_ZB_OVL_SURF,
		                   G_RM_ZB_PCL_SURF,
		                   G_RM_OPA_SURF,
		                   G_RM_XLU_SURF,
		                   G_RM_CLD_SURF,
		                   G_RM_TEX_EDGE,
		                   G_RM_PCL_SURF,
		                   G_RM_ADD,
		                   G_RM_NOOP,
		                   G_RM_VISCVG,
		                   G_RM_OPA_CI};

		uint32_t tblB[] = {G_RM_AA_ZB_OPA_SURF2,
		                   G_RM_AA_ZB_XLU_SURF2,
		                   G_RM_AA_ZB_OPA_DECAL2,
		                   G_RM_AA_ZB_XLU_DECAL2,
		                   G_RM_AA_ZB_OPA_INTER2,
		                   G_RM_AA_ZB_XLU_INTER2,
		                   G_RM_AA_ZB_XLU_LINE2,
		                   G_RM_AA_ZB_DEC_LINE2,
		                   G_RM_AA_ZB_TEX_EDGE2,
		                   G_RM_AA_ZB_TEX_INTER2,
		                   G_RM_AA_ZB_SUB_SURF2,
		                   G_RM_AA_ZB_PCL_SURF2,
		                   G_RM_AA_ZB_OPA_TERR2,
		                   G_RM_AA_ZB_TEX_TERR2,
		                   G_RM_AA_ZB_SUB_TERR2,
		                   G_RM_RA_ZB_OPA_SURF2,
		                   G_RM_RA_ZB_OPA_DECAL2,
		                   G_RM_RA_ZB_OPA_INTER2,
		                   G_RM_AA_OPA_SURF2,
		                   G_RM_AA_XLU_SURF2,
		                   G_RM_AA_XLU_LINE2,
		                   G_RM_AA_DEC_LINE2,
		                   G_RM_AA_TEX_EDGE2,
		                   G_RM_AA_SUB_SURF2,
		                   G_RM_AA_PCL_SURF2,
		                   G_RM_AA_OPA_TERR2,
		                   G_RM_AA_TEX_TERR2,
		                   G_RM_AA_SUB_TERR2,
		                   G_RM_RA_OPA_SURF2,
		                   G_RM_ZB_OPA_SURF2,
		                   G_RM_ZB_XLU_SURF2,
		                   G_RM_ZB_OPA_DECAL2,
		                   G_RM_ZB_XLU_DECAL2,
		                   G_RM_ZB_CLD_SURF2,
		                   G_RM_ZB_OVL_SURF2,
		                   G_RM_ZB_PCL_SURF2,
		                   G_RM_OPA_SURF2,
		                   G_RM_XLU_SURF2,
		                   G_RM_CLD_SURF2,
		                   G_RM_TEX_EDGE2,
		                   G_RM_PCL_SURF2,
		                   G_RM_ADD2,
		                   G_RM_NOOP2,
		                   G_RM_VISCVG2,
		                   G_RM_OPA_CI2};

		std::map<uint32_t, const char*> str = {
			{G_RM_FOG_SHADE_A, "G_RM_FOG_SHADE_A"},
			{G_RM_FOG_PRIM_A, "G_RM_FOG_PRIM_A"},
			{G_RM_PASS, "G_RM_PASS"},
			{G_RM_AA_ZB_OPA_SURF, "G_RM_AA_ZB_OPA_SURF"},
			{G_RM_AA_ZB_OPA_SURF2, "G_RM_AA_ZB_OPA_SURF2"},
			{G_RM_AA_ZB_XLU_SURF, "G_RM_AA_ZB_XLU_SURF"},
			{G_RM_AA_ZB_XLU_SURF2, "G_RM_AA_ZB_XLU_SURF2"},
			{G_RM_AA_ZB_OPA_DECAL, "G_RM_AA_ZB_OPA_DECAL"},
			{G_RM_AA_ZB_OPA_DECAL2, "G_RM_AA_ZB_OPA_DECAL2"},
			{G_RM_AA_ZB_XLU_DECAL, "G_RM_AA_ZB_XLU_DECAL"},
			{G_RM_AA_ZB_XLU_DECAL2, "G_RM_AA_ZB_XLU_DECAL2"},
			{G_RM_AA_ZB_OPA_INTER, "G_RM_AA_ZB_OPA_INTER"},
			{G_RM_AA_ZB_OPA_INTER2, "G_RM_AA_ZB_OPA_INTER2"},
			{G_RM_AA_ZB_XLU_INTER, "G_RM_AA_ZB_XLU_INTER"},
			{G_RM_AA_ZB_XLU_INTER2, "G_RM_AA_ZB_XLU_INTER2"},
			{G_RM_AA_ZB_XLU_LINE, "G_RM_AA_ZB_XLU_LINE"},
			{G_RM_AA_ZB_XLU_LINE2, "G_RM_AA_ZB_XLU_LINE2"},
			{G_RM_AA_ZB_DEC_LINE, "G_RM_AA_ZB_DEC_LINE"},
			{G_RM_AA_ZB_DEC_LINE2, "G_RM_AA_ZB_DEC_LINE2"},
			{G_RM_AA_ZB_TEX_EDGE, "G_RM_AA_ZB_TEX_EDGE"},
			{G_RM_AA_ZB_TEX_EDGE2, "G_RM_AA_ZB_TEX_EDGE2"},
			{G_RM_AA_ZB_TEX_INTER, "G_RM_AA_ZB_TEX_INTER"},
			{G_RM_AA_ZB_TEX_INTER2, "G_RM_AA_ZB_TEX_INTER2"},
			{G_RM_AA_ZB_SUB_SURF, "G_RM_AA_ZB_SUB_SURF"},
			{G_RM_AA_ZB_SUB_SURF2, "G_RM_AA_ZB_SUB_SURF2"},
			{G_RM_AA_ZB_PCL_SURF, "G_RM_AA_ZB_PCL_SURF"},
			{G_RM_AA_ZB_PCL_SURF2, "G_RM_AA_ZB_PCL_SURF2"},
			{G_RM_AA_ZB_OPA_TERR, "G_RM_AA_ZB_OPA_TERR"},
			{G_RM_AA_ZB_OPA_TERR2, "G_RM_AA_ZB_OPA_TERR2"},
			{G_RM_AA_ZB_TEX_TERR, "G_RM_AA_ZB_TEX_TERR"},
			{G_RM_AA_ZB_TEX_TERR2, "G_RM_AA_ZB_TEX_TERR2"},
			{G_RM_AA_ZB_SUB_TERR, "G_RM_AA_ZB_SUB_TERR"},
			{G_RM_AA_ZB_SUB_TERR2, "G_RM_AA_ZB_SUB_TERR2"},
			{G_RM_RA_ZB_OPA_SURF, "G_RM_RA_ZB_OPA_SURF"},
			{G_RM_RA_ZB_OPA_SURF2, "G_RM_RA_ZB_OPA_SURF2"},
			{G_RM_RA_ZB_OPA_DECAL, "G_RM_RA_ZB_OPA_DECAL"},
			{G_RM_RA_ZB_OPA_DECAL2, "G_RM_RA_ZB_OPA_DECAL2"},
			{G_RM_RA_ZB_OPA_INTER, "G_RM_RA_ZB_OPA_INTER"},
			{G_RM_RA_ZB_OPA_INTER2, "G_RM_RA_ZB_OPA_INTER2"},
			{G_RM_AA_OPA_SURF, "G_RM_AA_OPA_SURF"},
			{G_RM_AA_OPA_SURF2, "G_RM_AA_OPA_SURF2"},
			{G_RM_AA_XLU_SURF, "G_RM_AA_XLU_SURF"},
			{G_RM_AA_XLU_SURF2, "G_RM_AA_XLU_SURF2"},
			{G_RM_AA_XLU_LINE, "G_RM_AA_XLU_LINE"},
			{G_RM_AA_XLU_LINE2, "G_RM_AA_XLU_LINE2"},
			{G_RM_AA_DEC_LINE, "G_RM_AA_DEC_LINE"},
			{G_RM_AA_DEC_LINE2, "G_RM_AA_DEC_LINE2"},
			{G_RM_AA_TEX_EDGE, "G_RM_AA_TEX_EDGE"},
			{G_RM_AA_TEX_EDGE2, "G_RM_AA_TEX_EDGE2"},
			{G_RM_AA_SUB_SURF, "G_RM_AA_SUB_SURF"},
			{G_RM_AA_SUB_SURF2, "G_RM_AA_SUB_SURF2"},
			{G_RM_AA_PCL_SURF, "G_RM_AA_PCL_SURF"},
			{G_RM_AA_PCL_SURF2, "G_RM_AA_PCL_SURF2"},
			{G_RM_AA_OPA_TERR, "G_RM_AA_OPA_TERR"},
			{G_RM_AA_OPA_TERR2, "G_RM_AA_OPA_TERR2"},
			{G_RM_AA_TEX_TERR, "G_RM_AA_TEX_TERR"},
			{G_RM_AA_TEX_TERR2, "G_RM_AA_TEX_TERR2"},
			{G_RM_AA_TEX_TERR, "G_RM_AA_TEX_TERR"},
			{G_RM_AA_TEX_TERR2, "G_RM_AA_TEX_TERR2"},
			{G_RM_AA_SUB_TERR, "G_RM_AA_SUB_TERR"},
			{G_RM_AA_SUB_TERR2, "G_RM_AA_SUB_TERR2"},
			{G_RM_RA_OPA_SURF, "G_RM_RA_OPA_SURF"},
			{G_RM_RA_OPA_SURF2, "G_RM_RA_OPA_SURF2"},
			{G_RM_ZB_OPA_SURF, "G_RM_ZB_OPA_SURF"},
			{G_RM_ZB_OPA_SURF2, "G_RM_ZB_OPA_SURF2"},
			{G_RM_ZB_XLU_SURF, "G_RM_ZB_XLU_SURF"},
			{G_RM_ZB_XLU_SURF2, "G_RM_ZB_XLU_SURF2"},
			{G_RM_ZB_OPA_DECAL, "G_RM_ZB_OPA_DECAL"},
			{G_RM_ZB_OPA_DECAL2, "G_RM_ZB_OPA_DECAL2"},
			{G_RM_ZB_XLU_DECAL, "G_RM_ZB_XLU_DECAL"},
			{G_RM_ZB_XLU_DECAL2, "G_RM_ZB_XLU_DECAL2"},
			{G_RM_ZB_CLD_SURF, "G_RM_ZB_CLD_SURF"},
			{G_RM_ZB_CLD_SURF2, "G_RM_ZB_CLD_SURF2"},
			{G_RM_ZB_OVL_SURF, "G_RM_ZB_OVL_SURF"},
			{G_RM_ZB_OVL_SURF2, "G_RM_ZB_OVL_SURF2"},
			{G_RM_ZB_PCL_SURF, "G_RM_ZB_PCL_SURF"},
			{G_RM_ZB_PCL_SURF2, "G_RM_ZB_PCL_SURF2"},
			{G_RM_OPA_SURF, "G_RM_OPA_SURF"},
			{G_RM_OPA_SURF2, "G_RM_OPA_SURF2"},
			{G_RM_XLU_SURF, "G_RM_XLU_SURF"},
			{G_RM_XLU_SURF2, "G_RM_XLU_SURF2"},
			{G_RM_CLD_SURF, "G_RM_CLD_SURF"},
			{G_RM_CLD_SURF2, "G_RM_CLD_SURF2"},
			{G_RM_TEX_EDGE, "G_RM_TEX_EDGE"},
			{G_RM_TEX_EDGE2, "G_RM_TEX_EDGE2"},
			{G_RM_PCL_SURF, "G_RM_PCL_SURF"},
			{G_RM_PCL_SURF2, "G_RM_PCL_SURF2"},
			{G_RM_ADD, "G_RM_ADD"},
			{G_RM_ADD2, "G_RM_ADD2"},
			{G_RM_NOOP, "G_RM_NOOP"},
			{G_RM_NOOP2, "G_RM_NOOP2"},
			{G_RM_VISCVG, "G_RM_VISCVG"},
			{G_RM_VISCVG2, "G_RM_VISCVG2"},
			{G_RM_OPA_CI, "G_RM_OPA_CI"},
			{G_RM_OPA_CI2, "G_RM_OPA_CI2"},
		};

		for (uint32_t k = 0; k < sizeof(tblA) / 4; k++)
		{
			if ((dd & tblA[k]) == tblA[k])
			{
				if (tblA[k] > mode1)
					mode1 = tblA[k];
			}
		}

		for (uint32_t k = 0; k < sizeof(tblB) / 4; k++)
		{
			if ((dd & tblB[k]) == tblB[k])
			{
				if (tblB[k] > mode2)
					mode2 = tblB[k];
			}
		}

		std::string mode1Str = str[mode1];
		std::string mode2Str = str[mode2];

		if (mode1Str == "")
		{
			mode1Str = StringHelper::Sprintf("0x%08X", mode1);
		}

		if (mode2Str == "")
		{
			if (mode2 & AA_EN)
			{
				mode2Str += "AA_EN | ";
			}

			if (mode2 & Z_CMP)
			{
				mode2Str += "Z_CMP | ";
			}

			if (mode2 & Z_UPD)
			{
				mode2Str += "Z_UPD | ";
			}

			if (mode2 & IM_RD)
			{
				mode2Str += "IM_RD | ";
			}

			if (mode2 & CLR_ON_CVG)
			{
				mode2Str += "CLR_ON_CVG | ";
			}

			if (mode2 & CVG_DST_CLAMP)
			{
				mode2Str += "CVG_DST_CLAMP | ";
			}

			if (mode2 & CVG_DST_WRAP)
			{
				mode2Str += "CVG_DST_WRAP | ";
			}

			if (mode2 & CVG_DST_FULL)
			{
				mode2Str += "CVG_DST_FULL | ";
			}

			if (mode2 & CVG_DST_SAVE)
			{
				mode2Str += "CVG_DST_SAVE | ";
			}

			int32_t zMode = mode2 & 0xC00;

			if (zMode == ZMODE_INTER)
			{
				mode2Str += "ZMODE_INTER | ";
			}
			else if (zMode == ZMODE_XLU)
			{
				mode2Str += "ZMODE_XLU | ";
			}
			else if (zMode == ZMODE_DEC)
			{
				mode2Str += "ZMODE_DEC | ";
			}

			if (mode2 & CVG_X_ALPHA)
			{
				mode2Str += "CVG_X_ALPHA | ";
			}

			if (mode2 & ALPHA_CVG_SEL)
			{
				mode2Str += "ALPHA_CVG_SEL | ";
			}

			if (mode2 & FORCE_BL)
			{
				mode2Str += "FORCE_BL | ";
			}

			int32_t bp = (mode2 >> 28) & 0b11;
			int32_t ba = (mode2 >> 24) & 0b11;
			int32_t bm = (mode2 >> 20) & 0b11;
			int32_t bb = (mode2 >> 16) & 0b11;

			mode2Str += StringHelper::Sprintf("GBL_c2(%i, %i, %i, %i)", bp, ba, bm, bb);
			// mode2Str = StringHelper::Sprintf("0x%08X", mode2);
		}

		sprintf(line, "gsDPSetRenderMode(%s, %s),", mode1Str.c_str(), mode2Str.c_str());
	}
	else
	{
		sprintf(line, "gsSPSetOtherMode(0xE2, %i, %i, 0x%08X),", sft, len, dd);
	}
}

void ZDisplayList::Opcode_G_SETOTHERMODE_H(uint64_t data, char* line)
{
	int32_t ss = (data & 0x0000FF0000000000) >> 40;
	int32_t nn = (data & 0x000000FF00000000) >> 32;
	int32_t dd = (data & 0xFFFFFFFF);

	int32_t sft = 32 - (nn + 1) - ss;

	if (sft == 14)  // G_MDSFT_TEXTLUT
	{
		const char* types[] = {"G_TT_NONE", "G_TT_NONE", "G_TT_RGBA16", "G_TT_IA16"};
		sprintf(line, "gsDPSetTextureLUT(%s),", types[dd >> 14]);
	}
	else
		sprintf(line, "gsSPSetOtherMode(0xE3, %i, %i, 0x%08X),", sft, nn + 1, dd);
}

void ZDisplayList::Opcode_G_LOADTLUT(uint64_t data, [[maybe_unused]] const std::string& prefix,
                                     char* line)
{
	int32_t t = (data & 0x0000000007000000) >> 24;
	int32_t ccc = (data & 0x00000000003FF000) >> 14;

	lastTexWidth = sqrt(ccc + 1);
	lastTexHeight = sqrt(ccc + 1);

	lastTexLoaded = true;
	lastTexIsPalette = true;

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("TextureGenCheck G_LOADTLUT (lastCISiz: %i)\n", (uint32_t)lastCISiz);

	TextureGenCheck();

	sprintf(line, "gsDPLoadTLUTCmd(%i, %i),", t, ccc);
}

void ZDisplayList::Opcode_G_ENDDL([[maybe_unused]] const std::string& prefix, char* line)
{
	sprintf(line, "gsSPEndDisplayList(),");

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("TextureGenCheck G_ENDDL\n");

	TextureGenCheck();
}

static int32_t GfxdCallback_FormatSingleEntry()
{
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());
	gfxd_puts("\t");
	gfxd_macro_dflt();
	gfxd_puts(",");

	auto macroId = gfxd_macro_id();

	switch (macroId)
	{
	case gfxd_SP1Triangle:
	case gfxd_SP2Triangles:
		if (self->lastTexture != nullptr && self->lastTexture->IsColorIndexed() &&
		    !self->lastTexture->HasTlut())
		{
			auto tex = self->lastTexture;
			auto tlut = self->lastTlut;

			if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
			{
				if (tlut != nullptr)
					printf("CI texture '%s' (0x%X), TLUT: '%s' (0x%X)\n", tex->GetName().c_str(),
					       tex->GetRawDataIndex(), tlut->GetName().c_str(),
					       tlut->GetRawDataIndex());
				else
					printf("CI texture '%s' (0x%X), TLUT: null\n", tex->GetName().c_str(),
					       tex->GetRawDataIndex());
			}

			if (tlut != nullptr && !tex->HasTlut())
				tex->SetTlut(tlut);
		}
		break;
	}

	// dont print a new line after the last command
	switch (macroId)
	{
	case gfxd_SPEndDisplayList:
	case gfxd_SPBranchList:
		break;

	default:
		gfxd_puts("\n");
		break;
	}

	return 0;
}

static int32_t GfxdCallback_Vtx(uint32_t seg, int32_t count)
{
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());
	uint32_t vtxOffset = Seg2Filespace(seg, self->parent->baseAddress);

	if (GETSEGNUM(seg) == self->parent->segment)
	{
		Declaration* decl;

		// Check for vertex intersections from other display lists
		// TODO: These two could probably be condenced to one...
		decl = self->parent->GetDeclarationRanged(vtxOffset + (count * 16));
		if (decl != nullptr)
		{
			int32_t diff = decl->address - vtxOffset;

			if (diff > 0)
				count = diff / 16;
			else
				count = 0;
		}

		decl = self->parent->GetDeclarationRanged(vtxOffset);
		if (decl != nullptr)
		{
			int32_t diff = decl->address - vtxOffset;

			if (diff > 0)
				count = diff / 16;
			else
				count = 0;
		}

		if (count > 0)
		{
			std::vector<ZVtx> vtxList;
			vtxList.reserve(count);

			uint32_t currentPtr = vtxOffset;
			for (int32_t i = 0; i < count; i++)
			{
				ZVtx vtx(self->parent);
				vtx.ExtractFromFile(currentPtr);

				vtxList.push_back(vtx);
				currentPtr += 16;
			}
			self->vertices[vtxOffset] = vtxList;
		}
	}

	self->references.push_back(seg);
	
	if (!Globals::Instance->otrMode)
		gfxd_puts("@r");

	return 1;
}

static int32_t GfxdCallback_Texture(segptr_t seg, int32_t fmt, int32_t siz, int32_t width,
                                    int32_t height, [[maybe_unused]] int32_t pal)
{
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());
	uint32_t texOffset = Seg2Filespace(seg, self->parent->baseAddress);

	self->lastTexWidth = width;
	self->lastTexHeight = height;
	self->lastTexAddr = texOffset;
	self->lastTexSeg = seg;
	self->lastTexFmt = static_cast<F3DZEXTexFormats>(fmt);
	self->lastTexSiz = static_cast<F3DZEXTexSizes>(siz);
	self->lastTexLoaded = true;
	self->lastTexIsPalette = false;

	self->TextureGenCheck();

	std::string texName;
	Globals::Instance->GetSegmentedPtrName(seg, self->parent, "", texName, self->parent->workerID);

	gfxd_puts(texName.c_str());

	return 1;
}

static int32_t GfxdCallback_Palette(uint32_t seg, [[maybe_unused]] int32_t idx, int32_t count)
{
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());
	uint32_t palOffset = Seg2Filespace(seg, self->parent->baseAddress);

	self->lastTexWidth = sqrt(count);
	self->lastTexHeight = sqrt(count);
	self->lastTexAddr = palOffset;
	self->lastTexSeg = seg;
	self->lastTexSiz = F3DZEXTexSizes::G_IM_SIZ_16b;
	self->lastTexFmt = F3DZEXTexFormats::G_IM_FMT_RGBA;
	self->lastTexLoaded = true;
	self->lastTexIsPalette = true;

	self->TextureGenCheck();

	std::string palName;
	Globals::Instance->GetSegmentedPtrName(seg, self->parent, "", palName, self->parent->workerID);

	gfxd_puts(palName.c_str());

	return 1;
}

static int32_t GfxdCallback_DisplayList(uint32_t seg)
{
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());
	uint32_t dListOffset = GETSEGOFFSET(seg);
	uint32_t dListSegNum = GETSEGNUM(seg);

	std::string dListName = "";
	bool addressFound = Globals::Instance->GetSegmentedPtrName(seg, self->parent, "Gfx", dListName,
	                                                           self->parent->workerID);

	if (!addressFound && self->parent->segment == dListSegNum)
	{
		ZDisplayList* newDList = new ZDisplayList(self->parent);
		newDList->ExtractFromBinary(
			dListOffset,
			self->GetDListLength(self->parent->GetRawData(), dListOffset, self->dListType));
		newDList->SetName(newDList->GetDefaultName(self->parent->GetName()));
		self->otherDLists.push_back(newDList);
		dListName = newDList->GetName();
	}

	gfxd_puts(dListName.c_str());

	return 1;
}

static int32_t GfxdCallback_Matrix(uint32_t seg)
{
	std::string mtxName;
	ZDisplayList* self = static_cast<ZDisplayList*>(gfxd_udata_get());

	bool addressFound = Globals::Instance->GetSegmentedPtrName(seg, self->parent, "Mtx", mtxName,
	                                                           self->parent->workerID);
	if (!addressFound && GETSEGNUM(seg) == self->parent->segment)
	{
		Declaration* decl =
			self->parent->GetDeclaration(Seg2Filespace(seg, self->parent->baseAddress));
		if (decl == nullptr)
		{
			ZMtx* mtx = new ZMtx(self->parent);
			mtx->SetName(mtx->GetDefaultName(self->GetName()));
			mtx->ExtractFromFile(Seg2Filespace(seg, self->parent->baseAddress));
			mtx->DeclareVar(self->GetName(), "");

			mtx->GetSourceOutputCode(self->GetName());
			self->mtxList.push_back(*mtx);
			self->parent->resources.push_back(mtx);
			mtxName = "&" + mtx->GetName();
		}
	}

	gfxd_puts(mtxName.c_str());

	return 1;
}

void ZDisplayList::DeclareReferences(const std::string& prefix)
{
	std::string sourceOutput;

	if (Globals::Instance->useLegacyZDList)
		sourceOutput += ProcessLegacy(prefix);
	else
		sourceOutput += ProcessGfxDis(prefix);

	// Iterate through our vertex lists, connect intersecting lists.
	if (vertices.size() > 0)
	{
		std::vector<std::pair<uint32_t, std::vector<ZVtx>>> verticesSorted(vertices.begin(),
		                                                                   vertices.end());

		for (size_t i = 0; i < verticesSorted.size() - 1; i++)
		{
			size_t vtxSize = vertices[verticesSorted[i].first].size() * 16;

			if ((verticesSorted[i].first + vtxSize) > verticesSorted[i + 1].first)
			{
				int32_t intersectAmt =
					(verticesSorted[i].first + vtxSize) - verticesSorted[i + 1].first;
				int32_t intersectIndex = intersectAmt / 16;

				for (size_t j = intersectIndex; j < verticesSorted[i + 1].second.size(); j++)
					vertices[verticesSorted[i].first].push_back(verticesSorted[i + 1].second[j]);

				vertices.erase(verticesSorted[i + 1].first);
				verticesSorted.erase(verticesSorted.begin() + i + 1);

				i--;
			}
		}

		// Generate Vertex Declarations
		for (auto& item : vertices)
		{
			std::string declaration = "";

			offset_t curAddr = item.first;
			auto& firstVtx = item.second.at(0);

			for (auto vtx : item.second)
				declaration += StringHelper::Sprintf("\t%s,\n", vtx.GetBodySourceCode().c_str());

			Declaration* decl = parent->AddDeclarationArray(
				curAddr, firstVtx.GetDeclarationAlignment(),
				item.second.size() * firstVtx.GetRawDataSize(), firstVtx.GetSourceTypeName(),
				firstVtx.GetDefaultName(name), item.second.size(), declaration);

			/*for (auto vtx : item.second)
			{
				ZVtx* nVtx = new ZVtx(vtx.parent);
				nVtx->x = vtx.x;
				nVtx->y = vtx.y;
				nVtx->z = vtx.z;
				nVtx->flag = vtx.flag;
				nVtx->s = vtx.s;
				nVtx->t = vtx.t;
				nVtx->r = vtx.r;
				nVtx->g = vtx.g;
				nVtx->b = vtx.b;
				nVtx->a = vtx.a;
				decl->vertexHack.push_back(nVtx);
			}*/

			decl->isExternal = true;
		}
	}

	Declaration* decl = DeclareVar("", sourceOutput);
	decl->references = references;

	// Iterate through our vertex lists, connect intersecting lists.
	if (vertices.size() > 0)
	{
		std::vector<std::pair<uint32_t, std::vector<ZVtx>>> verticesSorted(vertices.begin(),
		                                                                   vertices.end());

		for (size_t i = 0; i < verticesSorted.size() - 1; i++)
		{
			// int32_t vtxSize = verticesSorted[i].second.size() * 16;
			size_t vtxSize = vertices[verticesSorted[i].first].size() * 16;

			if ((verticesSorted[i].first + vtxSize) > verticesSorted[i + 1].first)
			{
				int32_t intersectAmt =
					(verticesSorted[i].first + vtxSize) - verticesSorted[i + 1].first;
				int32_t intersectIndex = intersectAmt / 16;

				for (size_t j = intersectIndex; j < verticesSorted[i + 1].second.size(); j++)
					vertices[verticesSorted[i].first].push_back(verticesSorted[i + 1].second[j]);

				vertices.erase(verticesSorted[i + 1].first);
				verticesSorted.erase(verticesSorted.begin() + i + 1);

				i--;
			}
		}

		// Generate Vertex Declarations
		std::vector<int32_t> vtxKeys;

		for (auto& item : vertices)
			vtxKeys.push_back(item.first);

		// for (pair<int32_t, vector<Vertex>> item : vertices)
		for (size_t i = 0; i < vtxKeys.size(); i++)
		{
			auto& item = vertices[vtxKeys[i]];

			//std::string declaration;

			//for (auto& vtx : item)
				//declaration += StringHelper::Sprintf("\t%s,\n", vtx.GetBodySourceCode().c_str());

			// Ensure there's always a trailing line feed to prevent dumb warnings.
			// Please don't remove this line, unless you somehow made a way to prevent
			// that warning when building the OoT repo.
			//declaration += "\n";

			if (parent != nullptr)
			{
				std::string vtxName;
				ZResource* vtxRes = parent->FindResource(vtxKeys[i]);

				if (vtxRes != nullptr)
					vtxName = vtxRes->GetName();
				else
					vtxName = StringHelper::Sprintf("%sVtx_%06X", prefix.c_str(), vtxKeys[i]);

				auto filepath = Globals::Instance->outputPath / vtxName;
				std::string incStr = StringHelper::Sprintf("%s.%s.inc", filepath.string().c_str(), "vtx");

				Declaration* vtxDecl = parent->AddDeclarationIncludeArray(
					vtxKeys[i], incStr, item.size() * 16, "Vtx", vtxName, item.size());
				vtxDecl->isExternal = true;
			}
		}
	}
}

std::string ZDisplayList::ProcessLegacy(const std::string& prefix)
{
	char line[4096];
	std::string sourceOutput;

	for (size_t i = 0; i < instructions.size(); i++)
	{
		uint8_t opcode = (uint8_t)(instructions[i] >> 56);
		uint64_t data = instructions[i];
		sourceOutput += "    ";

		auto start = std::chrono::steady_clock::now();

		int32_t optimizationResult = OptimizationChecks(i, sourceOutput, prefix);

		if (optimizationResult != -1)
		{
			i += optimizationResult - 1;
			line[0] = '\0';
		}
		else
		{
			if (dListType == DListType::F3DZEX)
				ParseF3DZEX((F3DZEXOpcode)opcode, data, i, prefix, line);
			else
				ParseF3DEX((F3DEXOpcode)opcode, data, prefix, line);
		}

		auto end = std::chrono::steady_clock::now();
		int64_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG && diff > 5)
			printf("F3DOP: 0x%02X, TIME: %" PRIi64 "ms\n", opcode, diff);

		sourceOutput += line;

		if (i < instructions.size() - 1)
			sourceOutput += "\n";
	}

	return sourceOutput;
}

std::string ZDisplayList::ProcessGfxDis([[maybe_unused]] const std::string& prefix)
{
	std::string sourceOutput;

	OutputFormatter outputformatter;
	int32_t dListSize = instructions.size() * sizeof(instructions[0]);

	gfxd_input_buffer(instructions.data(), dListSize);
	gfxd_endian(gfxd_endian_little, sizeof(uint64_t));  // tell gfxdis what format the data is

	gfxd_macro_fn(GfxdCallback_FormatSingleEntry);  // format for each command entry
	gfxd_vtx_callback(GfxdCallback_Vtx);            // handle vertices
	gfxd_timg_callback(GfxdCallback_Texture);       // handle textures
	gfxd_tlut_callback(GfxdCallback_Palette);       // handle palettes
	gfxd_dl_callback(GfxdCallback_DisplayList);     // handle child display lists
	gfxd_mtx_callback(GfxdCallback_Matrix);         // handle matrices
	gfxd_output_callback(
		outputformatter.StaticWriter());  // convert tabs to 4 spaces and enforce 120 line limit

	gfxd_enable(gfxd_emit_dec_color);  // use decimal for colors

	// set microcode. see gfxd.h for more options.
	if (dListType == DListType::F3DZEX)
		gfxd_target(gfxd_f3dex2);
	else
		gfxd_target(gfxd_f3dex);

	gfxd_udata_set(this);
	gfxd_execute();                               // generate display list
	sourceOutput += outputformatter.GetOutput();  // write formatted display list

	return sourceOutput;
}

void ZDisplayList::TextureGenCheck()
{
	if (TextureGenCheck(lastTexWidth, lastTexHeight, lastTexAddr, lastTexSeg, lastTexFmt,
	                    lastTexSiz, lastTexLoaded, lastTexIsPalette, this))
	{
		lastTexAddr = 0;
		lastTexLoaded = false;
		lastTexIsPalette = false;
	}
}

bool ZDisplayList::TextureGenCheck(int32_t texWidth, int32_t texHeight, uint32_t texAddr,
                                   uint32_t texSeg, F3DZEXTexFormats texFmt, F3DZEXTexSizes texSiz,
                                   bool texLoaded, bool texIsPalette, ZDisplayList* self)
{
	uint32_t segmentNumber = GETSEGNUM(texSeg);

	if (!texIsPalette)
		self->lastTexture = nullptr;
	else
		self->lastTlut = nullptr;

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		printf("TextureGenCheck seg=%i width=%i height=%i ispal=%i addr=0x%06X\n", segmentNumber,
		       texWidth, texHeight, texIsPalette, texAddr);

	if ((texSeg != 0 || texAddr != 0) && texWidth > 0 && texHeight > 0 && texLoaded &&
	    Globals::Instance->HasSegment(segmentNumber, self->parent->workerID))
	{
		ZFile* auxParent = nullptr;
		if (segmentNumber == self->parent->segment)
		{
			auxParent = self->parent;
		}
		else
		{
			// Try to find a non-external file (i.e., one we are actually extracting)
			// which has the same segment number we are looking for.
			auto segs = Globals::Instance->GetSegmentRefFiles(self->parent->workerID);
			for (auto& otherFile : segs[segmentNumber])
			{
				if (!otherFile->isExternalFile)
				{
					auxParent = otherFile;
				}
			}
		}

		if (auxParent == nullptr)
		{
			// We can't declare the texture in any of the files we are extracting.
			return false;
		}

		if (auxParent->IsOffsetInFileRange(texAddr))
		{
			ZTexture* tex = auxParent->GetTextureResource(texAddr);
			if (tex != nullptr)
				tex->isPalette = texIsPalette;
			else
			{
				tex = new ZTexture(auxParent);
				tex->ExtractFromBinary(texAddr, texWidth, texHeight,
				                       TexFormatToTexType(texFmt, texSiz), texIsPalette);
				auxParent->AddTextureResource(texAddr, tex);
			}

			if (!texIsPalette)
				self->lastTexture = tex;
			else
				self->lastTlut = tex;

			if (auxParent->GetDeclaration(texAddr) == nullptr)
			{
				tex->DeclareVar(self->GetName(), "");
			}

			return true;
		}
	}

	return false;
}

TextureType ZDisplayList::TexFormatToTexType(F3DZEXTexFormats fmt, F3DZEXTexSizes siz)
{
	if (fmt == F3DZEXTexFormats::G_IM_FMT_RGBA)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::RGBA16bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_32b)
			return TextureType::RGBA32bpp;
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_CI)
	{
		if (Globals::Instance->useLegacyZDList)
			return TextureType::Palette8bpp;
		else
		{
			if (siz == F3DZEXTexSizes::G_IM_SIZ_4b)
				return TextureType::Palette4bpp;
			else if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
				return TextureType::Palette8bpp;
		}
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_IA)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_4b)
			return TextureType::GrayscaleAlpha4bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
			return TextureType::GrayscaleAlpha8bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::GrayscaleAlpha16bpp;
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_I)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_4b)
			return TextureType::Grayscale4bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
			return TextureType::Grayscale8bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::Grayscale8bpp;
	}

	return TextureType::RGBA16bpp;
}

bool ZDisplayList::IsExternalResource() const
{
	return false;
}

std::string ZDisplayList::GetExternalExtension() const
{
	return "dlist";
}

std::string ZDisplayList::GetSourceTypeName() const
{
	return "Gfx";
}

ZResourceType ZDisplayList::GetResourceType() const
{
	return ZResourceType::DisplayList;
}

size_t ZDisplayList::GetRawDataSize() const
{
	return instructions.size() * 8;
}

DeclarationAlignment ZDisplayList::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align8;
}

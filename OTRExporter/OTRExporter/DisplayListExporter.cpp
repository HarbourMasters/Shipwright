#include "DisplayListExporter.h"
#include "Main.h"
#include "../ZAPD/ZFile.h"
#include <Utils/MemoryStream.h>
#include <Utils/BitConverter.h>
#include "Lib/StrHash64.h"
#include "spdlog/spdlog.h"
#include "PR/ultra64/gbi.h"
#include <Globals.h>
#include <iostream>
#include <string>
#include "MtxExporter.h"
#include <Utils/File.h>
#include "VersionInfo.h"

#define GFX_SIZE 8

#define gsDPSetCombineLERP2(a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0,      \
        a1, b1, c1, d1, Aa1, Ab1, Ac1, Ad1)         \
{                                   \
    _SHIFTL(G_SETCOMBINE, 24, 8) |                  \
    _SHIFTL(GCCc0w0(a0, c0,         \
               Aa0, Ac0) |          \
           GCCc1w0(a1, c1), 0, 24),     \
    (unsigned int)(GCCc0w1(b0, d0,      \
                   Ab0, Ad0) |      \
               GCCc1w1(b1, Aa1,     \
                   Ac1, d1,     \
                   Ab1, Ad1))       \
}

#define gsSPBranchLessZraw2(dl, vtx, zval)               \
{   _SHIFTL(G_BRANCH_Z,24,8)|_SHIFTL((vtx)*5,12,12)|_SHIFTL((vtx)*2,0,12),\
    (unsigned int)(zval),                       }

#define gsSPBranchLessZraw3(dl)               \
{   _SHIFTL(G_RDPHALF_1,24,8),                        \
    (unsigned int)(dl),                     }

#define gsDPWordLo(wordlo)            \
    gsImmp1(G_RDPHALF_2, (unsigned int)(wordlo))

#define gsSPTextureRectangle2(xl, yl, xh, yh, tile)    \
{    (_SHIFTL(G_TEXRECT, 24, 8) | _SHIFTL(xh, 12, 12) | _SHIFTL(yh, 0, 12)),\
    (_SHIFTL(tile, 24, 3) | _SHIFTL(xl, 12, 12) | _SHIFTL(yl, 0, 12)) }

void OTRExporter_DisplayList::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZDisplayList* dList = (ZDisplayList*)res;

	//printf("Exporting DList %s\n", dList->GetName().c_str());

	WriteHeader(res, outPath, writer, Ship::ResourceType::DisplayList);

	while (writer->GetBaseAddress() % 8 != 0)
		writer->Write((uint8_t)0xFF);

	// DEBUG: Write in a marker
	Declaration* dbgDecl = dList->parent->GetDeclaration(dList->GetRawDataIndex());
	std::string dbgName = StringHelper::Sprintf("%s/%s", GetParentFolderName(res).c_str(), dbgDecl->varName.c_str());
	uint64_t hash = CRC64(dbgName.c_str());
	writer->Write((uint32_t)(G_MARKER << 24));
	writer->Write((uint32_t)0xBEEFBEEF);
	writer->Write((uint32_t)(hash >> 32));
	writer->Write((uint32_t)(hash & 0xFFFFFFFF));

	auto dlStart = std::chrono::steady_clock::now();

	//for (auto data : dList->instructions)
	for (size_t dataIdx = 0; dataIdx < dList->instructions.size(); dataIdx++)
	{
		auto data = dList->instructions[dataIdx];
		uint32_t word0 = 0;
		uint32_t word1 = 0;
		uint8_t opcode = (uint8_t)(data >> 56);
		F3DZEXOpcode opF3D = (F3DZEXOpcode)opcode;

		if ((int)opF3D == G_DL)// || (int)opF3D == G_BRANCH_Z)
			opcode = (uint8_t)G_DL_OTR;

		if ((int)opF3D == G_MTX)
			opcode = (uint8_t)G_MTX_OTR;

		if ((int)opF3D == G_BRANCH_Z)
			opcode = (uint8_t)G_BRANCH_Z_OTR;

		if ((int)opF3D == G_VTX)
			opcode = (uint8_t)G_VTX_OTR;

		if ((int)opF3D == G_SETTIMG)
			opcode = (uint8_t)G_SETTIMG_OTR;

		word0 += (opcode << 24);

		switch ((int)opF3D)
		{
		case G_NOOP:
		{
			Gfx value = {gsDPNoOp()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_ENDDL:
		{
			Gfx value = {gsSPEndDisplayList()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_MODIFYVTX:
		{
			int32_t ww = (data & 0x00FF000000000000ULL) >> 48;
			int32_t nnnn = (data & 0x0000FFFF00000000ULL) >> 32;
			int32_t vvvvvvvv = (data & 0x00000000FFFFFFFFULL);

			Gfx value = {gsSPModifyVertex(nnnn / 2, ww, vvvvvvvv)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
			break;
		default:
		{
			printf("Undefined opcode: %02X\n", opcode);
			//word0 = _byteswap_ulong((uint32_t)(data >> 32));
			//word1 = _byteswap_ulong((uint32_t)(data & 0xFFFFFFFF));
		}
		break;
		case G_GEOMETRYMODE:
		{
			int32_t cccccc = (data & 0x00FFFFFF00000000) >> 32;
			int32_t ssssssss = (data & 0xFFFFFFFF);

			Gfx value = {gsSPGeometryMode(~cccccc, ssssssss)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPPIPESYNC:
		{
			Gfx value = {gsDPPipeSync()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPLOADSYNC:
		{
			Gfx value = {gsDPLoadSync()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPTILESYNC:
		{
			Gfx value = {gsDPTileSync()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPFULLSYNC:
		{
			Gfx value = {gsDPFullSync()};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPSETOTHERMODE:
		{
			int32_t hhhhhh = (data & 0x00FFFFFF00000000) >> 32;
			int32_t llllllll = (data & 0x00000000FFFFFFFF);

			Gfx value = {gsDPSetOtherMode(hhhhhh, llllllll)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_POPMTX:
		{
			Gfx value = {gsSPPopMatrix(data)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETENVCOLOR:
		{
			uint8_t r = (uint8_t)((data & 0xFF000000) >> 24);
			uint8_t g = (uint8_t)((data & 0x00FF0000) >> 16);
			uint8_t b = (uint8_t)((data & 0xFF00FF00) >> 8);
			uint8_t a = (uint8_t)((data & 0x000000FF) >> 0);

			Gfx value = {gsDPSetEnvColor(r, g, b, a)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_MTX:
		{
			if ((!Globals::Instance->HasSegment(GETSEGNUM(data), res->parent->workerID)) || ((data & 0xFFFFFFFF) == 0x07000000)) // En_Zf and En_Ny place a DL in segment 7
			{
				uint32_t pp = (data & 0x000000FF00000000) >> 32;
				uint32_t mm = (data & 0x00000000FFFFFFFF);

				pp ^= G_MTX_PUSH;

				mm = (mm & 0x0FFFFFFF) + 1;

				Gfx value = {gsSPMatrix(mm, pp)};
				word0 = value.words.w0;
				word1 = value.words.w1;
			}
			else
			{
				uint32_t pp = (data & 0x000000FF00000000) >> 32;
				uint32_t mm = (data & 0x00000000FFFFFFFF);
				pp ^= G_MTX_PUSH;

				Gfx value = {gsSPMatrix(mm, pp)};
				word0 = value.words.w0;
				word1 = value.words.w1;

				word0 = (word0 & 0x00FFFFFF) + (G_MTX_OTR << 24);

				Declaration* mtxDecl = dList->parent->GetDeclaration(GETSEGOFFSET(mm));

				int bp = 0;

				writer->Write(word0);
				writer->Write(word1);

				if (mtxDecl != nullptr)
				{
					std::string vName = StringHelper::Sprintf("%s/%s", (GetParentFolderName(res).c_str()), mtxDecl->varName.c_str());

					uint64_t hash = CRC64(vName.c_str());

					word0 = hash >> 32;
					word1 = hash & 0xFFFFFFFF;
				}
				else
				{
					word0 = 0;
					word1 = 0;
					spdlog::error(StringHelper::Sprintf("dListDecl == nullptr! Addr = {:08X}", GETSEGOFFSET(data)));
				}
			}
		}
		break;
		case G_LOADBLOCK:
		{
			int32_t sss = (data & 0x00FFF00000000000) >> 48;
			int32_t ttt = (data & 0x00000FFF00000000) >> 36;
			int32_t i = (data & 0x000000000F000000) >> 24;
			int32_t xxx = (data & 0x0000000000FFF000) >> 12;
			int32_t ddd = (data & 0x0000000000000FFF);

			Gfx value = {gsDPLoadBlock(i, sss, ttt, xxx, ddd)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_CULLDL:
		{
			int32_t vvvv = (data & 0xFFFF00000000) >> 32;
			int32_t wwww = (data & 0x0000FFFF);

			Gfx value = {gsSPCullDisplayList(vvvv / 2, wwww / 2)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_RDPHALF_1:
		{
			auto data2 = dList->instructions[dataIdx + 1];

			if ((data2 >> 56) != G_BRANCH_Z)
			{
				uint32_t a = (data & 0x00FFF00000000000) >> 44;
				uint32_t b = (data & 0x00000FFF00000000) >> 32;
				uint32_t z = (data & 0x00000000FFFFFFFF) >> 0;
				uint32_t h = (data & 0xFFFFFFFF);

				Gfx value = {gsSPBranchLessZraw3(h & 0x00FFFFFF)};
				word0 = value.words.w0;
				word1 = value.words.w1;
			}
			else
			{
				word0 = (G_NOOP << 24);
				word1 = 0;
			}
		}
			break;
		case G_RDPHALF_2:
		{
			Gfx value = {gsDPWordLo(data & 0xFFFFFFFF)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_TEXRECT:
		{
			int32_t xxx = (data & 0x00FFF00000000000) >> 44;
			int32_t yyy = (data & 0x00000FFF00000000) >> 32;
			int32_t i = (data & 0x000000000F000000) >> 24;
			int32_t XXX = (data & 0x0000000000FFF000) >> 12;
			int32_t YYY = (data & 0x0000000000000FFF);

			Gfx value = {gsSPTextureRectangle2(XXX, YYY, xxx, yyy, i)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
			break;
		case G_BRANCH_Z:
		{
			uint32_t a = (data & 0x00FFF00000000000) >> 44;
			uint32_t b = (data & 0x00000FFF00000000) >> 32;
			uint32_t z = (data & 0x00000000FFFFFFFF) >> 0;
			uint32_t h = (data & 0xFFFFFFFF);

			auto data2 = dList->instructions[dataIdx - 1];
			uint32_t dListPtr = GETSEGOFFSET(data2);

			Declaration* dListDecl = dList->parent->GetDeclaration(dListPtr);

			int bp = 0;

			Gfx value = {gsSPBranchLessZraw2(0xDEADABCD, (a / 5) | (b / 2), z)};
			word0 = (value.words.w0 & 0x00FFFFFF) + (G_BRANCH_Z_OTR << 24);
			word1 = value.words.w1;

			writer->Write(word0);
			writer->Write(word1);

			if (dListDecl != nullptr)
			{
				std::string vName = StringHelper::Sprintf("%s/%s", (GetParentFolderName(res).c_str()), dListDecl->varName.c_str());

				uint64_t hash = CRC64(vName.c_str());

				word0 = hash >> 32;
				word1 = hash & 0xFFFFFFFF;
			}
			else
			{
				word0 = 0;
				word1 = 0;
				spdlog::error(StringHelper::Sprintf("dListDecl == nullptr! Addr = {:08X}", GETSEGOFFSET(data)));
			}

			for (size_t i = 0; i < dList->otherDLists.size(); i++)
			{
				Declaration* dListDecl2 = dList->parent->GetDeclaration(GETSEGOFFSET(dList->otherDLists[i]->GetRawDataIndex()));

				if (dListDecl2 != nullptr)
				{
					//std::string fName = StringHelper::Sprintf("%s\\%s", GetParentFolderName(res).c_str(), dListDecl2->varName.c_str());
					std::string fName = OTRExporter_DisplayList::GetPathToRes(res, dListDecl2->varName.c_str());

					if (files.find(fName) == files.end() && !File::Exists("Extract/" + fName))
					{
						MemoryStream* dlStream = new MemoryStream();
						BinaryWriter dlWriter = BinaryWriter(dlStream);

						Save(dList->otherDLists[i], outPath, &dlWriter);
						AddFile(fName, dlStream->ToVector());
					}
				}
				else
				{
					spdlog::error(StringHelper::Sprintf("dListDecl2 == nullptr! Addr = {:08X}", GETSEGOFFSET(data)));
				}
			}

			//Gfx value = gsSPBranchLessZraw2(h & 0x00FFFFFF, (a / 5) | (b / 2), z);
			//word0 = value.words.w0;
			//word1 = value.words.w1;
		}
			break;
		//case G_BRANCH_Z:
		case G_DL:
		{
			if ((!Globals::Instance->HasSegment(GETSEGNUM(data), res->parent->workerID) && (int)opF3D != G_BRANCH_Z)
				|| ((data & 0xFFFFFFFF) == 0x07000000)) // En_Zf and En_Ny place a DL in segment 7
			{
				int32_t pp = (data & 0x00FF000000000000) >> 56;

				Gfx value;

				u32 dListVal = (data & 0x0FFFFFFF) + 1;

				if (pp != 0)
					value = {gsSPBranchList(dListVal)};
				else
					value = {gsSPDisplayList(dListVal)};

				word0 = value.words.w0;
				word1 = value.words.w1;
			}
			else
			{
				uint32_t dListPtr = GETSEGOFFSET(data);

				if ((int)opF3D == G_BRANCH_Z)
				{
					auto data2 = dList->instructions[dataIdx - 1];
					dListPtr = GETSEGOFFSET(data2);
				}
				else
				{
					int bp = 0;
				}

				Declaration* dListDecl = dList->parent->GetDeclaration(dListPtr);

				int bp = 0;

				writer->Write(word0);
				writer->Write(word1);

				if (dListDecl != nullptr)
				{
					std::string vName = StringHelper::Sprintf("%s/%s", (GetParentFolderName(res).c_str()), dListDecl->varName.c_str());

					uint64_t hash = CRC64(vName.c_str());

					word0 = hash >> 32;
					word1 = hash & 0xFFFFFFFF;
				}
				else
				{
					word0 = 0;
					word1 = 0;
					spdlog::error(StringHelper::Sprintf("dListDecl == nullptr! Addr = {:08X}", GETSEGOFFSET(data)));
				}

				for (size_t i = 0; i < dList->otherDLists.size(); i++)
				{
					Declaration* dListDecl2 = dList->parent->GetDeclaration(GETSEGOFFSET(dList->otherDLists[i]->GetRawDataIndex()));

					if (dListDecl2 != nullptr)
					{
						//std::string fName = StringHelper::Sprintf("%s\\%s", GetParentFolderName(res).c_str(), dListDecl2->varName.c_str());
						std::string fName = OTRExporter_DisplayList::GetPathToRes(res, dListDecl2->varName.c_str());

						if (files.find(fName) == files.end() && !File::Exists("Extract/" + fName))
						{
							MemoryStream* dlStream = new MemoryStream();
							BinaryWriter dlWriter = BinaryWriter(dlStream);

							Save(dList->otherDLists[i], outPath, &dlWriter);

							AddFile(fName, dlStream->ToVector());
						}
					}
					else
					{
						spdlog::error(StringHelper::Sprintf("dListDecl2 == nullptr! Addr = {:08X}", GETSEGOFFSET(data)));
					}
				}
			}
		}
		break;
		case G_TEXTURE:
		{
			int32_t ____ = (data & 0x0000FFFF00000000) >> 32;
			int32_t ssss = (data & 0x00000000FFFF0000) >> 16;
			int32_t tttt = (data & 0x000000000000FFFF);
			int32_t lll = (____ & 0x3800) >> 11;
			int32_t ddd = (____ & 0x700) >> 8;
			int32_t nnnnnnn = (____ & 0xFE) >> 1;

			Gfx value = {gsSPTexture(ssss, tttt, lll, ddd, nnnnnnn)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_TRI1:
		{
			int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
			int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
			int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;

			Gfx test = {gsSP1Triangle(aa, bb, cc, 0)};
			word0 = test.words.w0;
			word1 = test.words.w1;
		}
		break;
		case G_TRI2:
		{
			int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
			int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
			int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
			int32_t dd = ((data & 0x00000000FF0000ULL) >> 16) / 2;
			int32_t ee = ((data & 0x0000000000FF00ULL) >> 8) / 2;
			int32_t ff = ((data & 0x000000000000FFULL) >> 0) / 2;

			Gfx test = {gsSP2Triangles(aa, bb, cc, 0, dd, ee, ff, 0)};
			word0 = test.words.w0;
			word1 = test.words.w1;
		}
		break;
		case G_QUAD:
		{
			int32_t aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
			int32_t bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
			int32_t cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
			int32_t dd = ((data & 0x000000000000FFULL)) / 2;

			Gfx test = {gsSP1Quadrangle(aa, bb, cc, dd, 0)};
			word0 = test.words.w0;
			word1 = test.words.w1;
		}
			break;
		case G_SETPRIMCOLOR:
		{
			int32_t mm = (data & 0x0000FF0000000000) >> 40;
			int32_t ff = (data & 0x000000FF00000000) >> 32;
			int32_t rr = (data & 0x00000000FF000000) >> 24;
			int32_t gg = (data & 0x0000000000FF0000) >> 16;
			int32_t bb = (data & 0x000000000000FF00) >> 8;
			int32_t aa = (data & 0x00000000000000FF) >> 0;

			Gfx value = {gsDPSetPrimColor(mm, ff, rr, gg, bb, aa)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETOTHERMODE_L:
		{
			int32_t ss = (data & 0x0000FF0000000000) >> 40;
			int32_t len = ((data & 0x000000FF00000000) >> 32) + 1;
			int32_t sft = 32 - (len)-ss;
			int32_t dd = (data & 0xFFFFFFFF);

			// TODO: Output the correct render modes in data

			Gfx value = {gsSPSetOtherMode(0xE2, sft, len, dd)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETOTHERMODE_H:
		{
			int32_t ss = (data & 0x0000FF0000000000) >> 40;
			int32_t nn = (data & 0x000000FF00000000) >> 32;
			int32_t dd = (data & 0xFFFFFFFF);

			int32_t sft = 32 - (nn + 1) - ss;

			Gfx value;

			if (sft == 14)  // G_MDSFT_TEXTLUT
			{
				const char* types[] = { "G_TT_NONE", "G_TT_NONE", "G_TT_RGBA16", "G_TT_IA16" };
				value = {gsDPSetTextureLUT(dd >> 14)};
			}
			else
			{
				value = {gsSPSetOtherMode(0xE3, sft, nn + 1, dd)};
			}

			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETTILE:
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

			Gfx value = {gsDPSetTile(fff, ii, nnnnnnnnn, mmmmmmmmm, ttt, pppp, cc, aaaa, ssss, dd, bbbb, uuuu)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETCOMBINE:
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

			Gfx value = {gsDPSetCombineLERP2(a0, b0, c0, d0, aa0, ab0, ac0, ad0, a1, b1, c1, d1, aa1, ab1, ac1, ad1)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_SETTILESIZE:
		{
			int32_t sss = (data & 0x00FFF00000000000) >> 44;
			int32_t ttt = (data & 0x00000FFF00000000) >> 32;
			int32_t uuu = (data & 0x0000000000FFF000) >> 12;
			int32_t vvv = (data & 0x0000000000000FFF);
			int32_t i = (data & 0x000000000F000000) >> 24;

			Gfx value = {gsDPSetTileSize(i, sss, ttt, uuu, vvv)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_LOADTLUT:
		{
			int32_t t = (data & 0x0000000007000000) >> 24;
			int32_t ccc = (data & 0x00000000003FF000) >> 14;

			Gfx value = {gsDPLoadTLUTCmd(t, ccc)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
		break;
		case G_LOADTILE:
		{
			int sss =	(data & 0x00FFF00000000000) >> 44;
			int ttt =	(data & 0x00000FFF00000000) >> 32;
			int i =		(data & 0x000000000F000000) >> 24;
			int uuu =	(data & 0x0000000000FFF000) >> 12;
			int vvv=	(data & 0x0000000000000FFF);

			Gfx value = {gsDPLoadTile(i, sss, ttt, uuu, vvv)};
			word0 = value.words.w0;
			word1 = value.words.w1;
		}
			break;
		case G_SETTIMG:
		{
			uint32_t seg = data & 0xFFFFFFFF;
			int32_t texAddress = Seg2Filespace(data, dList->parent->baseAddress);

			if (!Globals::Instance->HasSegment(GETSEGNUM(seg), res->parent->workerID) || (res->GetName() == "sShadowMaterialDL"))
			{
				if (res->GetName() == "sShadowMaterialDL") {
					// sShadowMaterialDL (In ovl_En_Jsjutan) has a texture in bss. This is a hack to override the reference to one
					// to segment C. The actor has been modified to load the texture into segment C.
					seg = 0x0C000000;
				}
				int32_t __ = (data & 0x00FF000000000000) >> 48;
				int32_t www = (data & 0x00000FFF00000000) >> 32;

				uint32_t fmt = (__ & 0xE0) >> 5;
				uint32_t siz = (__ & 0x18) >> 3;

				Gfx value = {gsDPSetTextureImage(fmt, siz, www + 1, (seg & 0x0FFFFFFF) + 1)};
				word0 = value.words.w0;
				word1 = value.words.w1;

				writer->Write(word0);
				writer->Write(word1);
			}
			else
			{
				std::string texName = "";
				bool foundDecl = Globals::Instance->GetSegmentedPtrName(seg, dList->parent, "", texName, res->parent->workerID);

				int32_t __ = (data & 0x00FF000000000000) >> 48;
				int32_t www = (data & 0x00000FFF00000000) >> 32;

				uint32_t fmt = (__ & 0xE0) >> 5;
				uint32_t siz = (__ & 0x18) >> 3;

				Gfx value = {gsDPSetTextureImage(fmt, siz, www + 1, __)};
				word0 = value.words.w0 & 0x00FFFFFF;
				word0 += (G_SETTIMG_OTR << 24);
				//word1 = value.words.w1;
				word1 = 0;

				writer->Write(word0);
				writer->Write(word1);

				if (foundDecl)
				{
					ZFile* assocFile = Globals::Instance->GetSegment(GETSEGNUM(seg), res->parent->workerID);
					std::string assocFileName = assocFile->GetName();
					std::string fName = "";

					if (GETSEGNUM(seg) == SEGMENT_SCENE || GETSEGNUM(seg) == SEGMENT_ROOM)
						fName = GetPathToRes(res, texName.c_str());
					else
						fName = GetPathToRes(assocFile->resources[0], texName.c_str());

					uint64_t hash = CRC64(fName.c_str());

					word0 = hash >> 32;
					word1 = hash & 0xFFFFFFFF;
				}
				else
				{
					word0 = 0;
					word1 = 0;
					spdlog::error("texDecl == nullptr! PTR = 0x{:08X}", texAddress);
				}
			}
		}
		break;
		case G_VTX:
		{
			if (GETSEGNUM(data) == 0xC || GETSEGNUM(data) == 0x8)
			{
				// hack for dynamic verticies used in en_ganon_mant and en_jsjutan
				// TODO is there a better way?
				int32_t aa = (data & 0x000000FF00000000ULL) >> 32;
				int32_t nn = (data & 0x000FF00000000000ULL) >> 44;

				Gfx value = {gsSPVertex(data & 0xFFFFFFFF, nn, ((aa >> 1) - nn))};

				word0 = value.words.w0;
				word1 = value.words.w1 | 1;
			}
			else
			{
				// Write CRC64 of vtx file name
				uint32_t addr = data & 0xFFFFFFFF;

				if (GETSEGNUM(data) == 0x80)
					addr -= dList->parent->baseAddress;

				auto segOffset = GETSEGOFFSET(addr);
				Declaration* vtxDecl = dList->parent->GetDeclarationRanged(segOffset);

				int32_t aa = (data & 0x000000FF00000000ULL) >> 32;
				int32_t nn = (data & 0x000FF00000000000ULL) >> 44;

				if (vtxDecl != nullptr && vtxDecl->varType != "Gfx")
				{
					uint32_t diff = segOffset - vtxDecl->address;

					Gfx value = {gsSPVertex(diff, nn, ((aa >> 1) - nn))};

					word0 = value.words.w0;
					word0 &= 0x00FFFFFF;
					word0 += (G_VTX_OTR << 24);
					word1 = value.words.w1;

					writer->Write(word0);
					writer->Write(word1);

					std::string fName = OTRExporter_DisplayList::GetPathToRes(res, vtxDecl->varName);

					uint64_t hash = CRC64(fName.c_str());

					word0 = hash >> 32;
					word1 = hash & 0xFFFFFFFF;

					if (files.find(fName) == files.end() && !File::Exists("Extract/" + fName))
					{
						// Write vertices to file
						MemoryStream* vtxStream = new MemoryStream();
						BinaryWriter vtxWriter = BinaryWriter(vtxStream);

						int arrCnt = 0;

						auto split = StringHelper::Split(vtxDecl->text, "\n");

						for (size_t i = 0; i < split.size(); i++)
						{
							std::string line = split[i];

							if (StringHelper::Contains(line, "VTX("))
								arrCnt++;
						}

						// OTRTODO: Once we aren't relying on text representations, we should call ArrayExporter...
						OTRExporter::WriteHeader(nullptr, "", &vtxWriter, Ship::ResourceType::Array);

						vtxWriter.Write((uint32_t)ZResourceType::Vertex);
						vtxWriter.Write((uint32_t)arrCnt);

						auto start = std::chrono::steady_clock::now();

						// God dammit this is so dumb
						for (size_t i = 0; i < split.size(); i++)
						{
							std::string line = split[i];

							if (StringHelper::Contains(line, "VTX("))
							{
								auto split2 = StringHelper::Split(StringHelper::Split(StringHelper::Split(line, "VTX(")[1], ")")[0], ",");

								vtxWriter.Write((int16_t)std::stoi(split2[0], nullptr, 10)); // v.x
								vtxWriter.Write((int16_t)std::stoi(split2[1], nullptr, 10)); // v.y
								vtxWriter.Write((int16_t)std::stoi(split2[2], nullptr, 10)); // v.z

								vtxWriter.Write((int16_t)0);								 // v.flag

								vtxWriter.Write((int16_t)std::stoi(split2[3], nullptr, 10)); // v.s
								vtxWriter.Write((int16_t)std::stoi(split2[4], nullptr, 10)); // v.t

								vtxWriter.Write((uint8_t)std::stoi(split2[5], nullptr, 10)); // v.r
								vtxWriter.Write((uint8_t)std::stoi(split2[6], nullptr, 10)); // v.g
								vtxWriter.Write((uint8_t)std::stoi(split2[7], nullptr, 10)); // v.b
								vtxWriter.Write((uint8_t)std::stoi(split2[8], nullptr, 10)); // v.a
							}
						}

						AddFile(fName, vtxStream->ToVector());

						auto end = std::chrono::steady_clock::now();
						size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
					}
				}
				else
				{
					spdlog::error("vtxDecl == nullptr!");
				}
			}
		}
		break;
		}

		writer->Write(word0);
		writer->Write(word1);
	}

	auto dlEnd = std::chrono::steady_clock::now();
	size_t dlDiff = std::chrono::duration_cast<std::chrono::milliseconds>(dlEnd - dlStart).count();

	//printf("Display List Gen in %zums\n", dlDiff);
}

std::string OTRExporter_DisplayList::GetPathToRes(ZResource* res, std::string varName)
{
	std::string prefix = GetPrefix(res);
	std::string fName = StringHelper::Sprintf("%s/%s", GetParentFolderName(res).c_str(), varName.c_str());

	return fName;
}

std::string OTRExporter_DisplayList::GetParentFolderName(ZResource* res)
{
	std::string prefix = GetPrefix(res);
	std::string oName = res->parent->GetOutName();

	if (StringHelper::Contains(oName, "_scene"))
	{
		auto split = StringHelper::Split(oName, "_");
		oName = "";
		for (size_t i = 0; i < split.size() - 1; i++)
			oName += split[i] + "_";

		oName += "scene";
	}
	else if (StringHelper::Contains(oName, "_room"))
	{
		oName = StringHelper::Split(oName, "_room")[0] + "_scene";
	}

	if (prefix != "")
		oName = prefix + "/" + oName;

	return oName;
}

std::string OTRExporter_DisplayList::GetPrefix(ZResource* res)
{
	std::string oName = res->parent->GetOutName();
	std::string prefix = "";
	std::string xmlPath = StringHelper::Replace(res->parent->GetXmlFilePath().string(), "\\", "/");

	if (StringHelper::Contains(oName, "_scene") || StringHelper::Contains(oName, "_room")) {
		prefix = "scenes";
        if (Globals::Instance->rom->IsMQ()) {
            prefix += "/mq";
        } else {
            prefix += "/nonmq";
        }
    }
	else if (StringHelper::Contains(xmlPath, "objects/"))
		prefix = "objects";
	else if (StringHelper::Contains(xmlPath, "textures/"))
		prefix = "textures";
	else if (StringHelper::Contains(xmlPath, "overlays/"))
		prefix = "overlays";
	else if (StringHelper::Contains(xmlPath, "misc/"))
		prefix = "misc";
	else if (StringHelper::Contains(xmlPath, "text/"))
		prefix = "text";
	else if (StringHelper::Contains(xmlPath, "code/"))
		prefix = "code";

	return prefix;
}

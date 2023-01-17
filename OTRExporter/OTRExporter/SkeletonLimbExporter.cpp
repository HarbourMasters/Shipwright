#include "SkeletonLimbExporter.h"
#include "DisplayListExporter.h"
#include <libultraship/bridge.h>
#include <Globals.h>

void OTRExporter_SkeletonLimb::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZLimb* limb = (ZLimb*)res;

	WriteHeader(res, outPath, writer, Ship::ResourceType::SOH_SkeletonLimb);

	writer->Write((uint8_t)limb->type);
	writer->Write((uint8_t)limb->skinSegmentType);

	if (limb->skinSegmentType == ZLimbSkinType::SkinType_DList && limb->type == ZLimbType::Skin)
	{
		auto childDecl = limb->parent->GetDeclaration(GETSEGOFFSET(limb->skinSegment));

		if (childDecl != nullptr)
			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, childDecl->varName));
		else
			writer->Write("");
	}
	else
	{
		writer->Write("");
	}

	writer->Write((uint16_t)limb->segmentStruct.unk_0);
	writer->Write((uint32_t)limb->segmentStruct.unk_4_arr.size());
	
	for (auto item : limb->segmentStruct.unk_4_arr)
	{
		writer->Write(item.unk_4);

		writer->Write((uint32_t)item.unk_8_arr.size());

		for (auto item2 : item.unk_8_arr)
		{
			writer->Write(item2.unk_0);
			writer->Write(item2.unk_2);
			writer->Write(item2.unk_4);
			writer->Write(item2.unk_6);
			writer->Write(item2.unk_7);
			writer->Write(item2.unk_8);
			writer->Write(item2.unk_9);
		}

		writer->Write((uint32_t)item.unk_C_arr.size());

		for (auto item2 : item.unk_C_arr)
		{
			writer->Write(item2.unk_0);
			writer->Write(item2.x);
			writer->Write(item2.y);
			writer->Write(item2.z);
			writer->Write(item2.unk_8);
		}
	}

	if (limb->segmentStruct.unk_8 != 0)
	{
		auto skinGfxDecl = limb->parent->GetDeclaration(GETSEGOFFSET(limb->segmentStruct.unk_8));

		if (skinGfxDecl != nullptr)
		{
			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, skinGfxDecl->varName));
		}
		else
		{
			writer->Write("");
		}
	}
	else
	{
		writer->Write("");
	}

	writer->Write(limb->legTransX);
	writer->Write(limb->legTransY);
	writer->Write(limb->legTransZ);
	writer->Write(limb->rotX);
	writer->Write(limb->rotY);
	writer->Write(limb->rotZ);

	if (limb->childPtr != 0)
	{
		std::string name;
		bool foundDecl = Globals::Instance->GetSegmentedPtrName(limb->childPtr, limb->parent, "", name, res->parent->workerID);
		if (foundDecl)
		{
			if (name.at(0) == '&')
				name.erase(0, 1);

			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, name));
		}
		else
		{
			writer->Write("");
		}
	}
	else
	{
		writer->Write("");
	}

	if (limb->siblingPtr != 0)
	{
		std::string name;
		bool foundDecl = Globals::Instance->GetSegmentedPtrName(limb->siblingPtr, limb->parent, "", name, res->parent->workerID);
		if (foundDecl)
		{
			if (name.at(0) == '&')
				name.erase(0, 1);

			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, name));
		}
		else
		{
			writer->Write("");
		}
	}
	else
	{
		writer->Write("");
	}

	if (limb->dListPtr != 0)
	{
		std::string name;
		bool foundDecl = Globals::Instance->GetSegmentedPtrName(limb->dListPtr, limb->parent, "", name, res->parent->workerID);
		if (foundDecl)
		{
			if (name.at(0) == '&')
				name.erase(0, 1);

			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, name));
		}
		else
		{
			writer->Write("");
		}
	}
	else
	{
		writer->Write("");
	}

	if (limb->dList2Ptr != 0)
	{
		std::string name;
		bool foundDecl = Globals::Instance->GetSegmentedPtrName(limb->dList2Ptr, limb->parent, "", name, res->parent->workerID);
		if (foundDecl)
		{
			if (name.at(0) == '&')
				name.erase(0, 1);

			writer->Write(OTRExporter_DisplayList::GetPathToRes(limb, name));
		}
		else
		{
			writer->Write("");
		}
	}
	else
	{
		writer->Write("");
	}

	writer->Write(limb->transX);
	writer->Write(limb->transY);
	writer->Write(limb->transZ);

	writer->Write(limb->childIndex);
	writer->Write(limb->siblingIndex);
}
#include "SkeletonExporter.h"
#include <libultraship/bridge.h>
#include <Globals.h>
#include "DisplayListExporter.h"

void OTRExporter_Skeleton::Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer)
{
	ZSkeleton* skel = (ZSkeleton*)res;

	WriteHeader(res, outPath, writer, Ship::ResourceType::SOH_Skeleton);

	writer->Write((uint8_t)skel->type);
	writer->Write((uint8_t)skel->limbType);

	writer->Write((uint32_t)skel->limbCount);
	writer->Write((uint32_t)skel->dListCount);

	writer->Write((uint8_t)skel->limbsTable.limbType);
	writer->Write((uint32_t)skel->limbsTable.count);

	for (size_t i = 0; i < skel->limbsTable.count; i++)
	{
		Declaration* skelDecl = skel->parent->GetDeclarationRanged(GETSEGOFFSET(skel->limbsTable.limbsAddresses[i]));

		std::string name;
		bool foundDecl = Globals::Instance->GetSegmentedPtrName(skel->limbsTable.limbsAddresses[i], skel->parent, "", name, res->parent->workerID);
		if (foundDecl)
		{
			if (name.at(0) == '&')
				name.erase(0, 1);

			writer->Write(OTRExporter_DisplayList::GetPathToRes(res, name));
		}
		else
		{
			writer->Write("");
		}
	}
}

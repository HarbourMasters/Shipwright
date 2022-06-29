#include "ModManager.h"
#include "TextureMod.h"
#include "ModModule.h"
#include "ImGuiImpl.h"

namespace Ship {
	std::vector<ModModule*> modules;

	void ModManager::Init() {
		// ResManager->GetArchive()->loa
		// modules.push_back(new TextureModule(this));
		// std::shared_ptr<Ship::Archive> archive = std::make_shared<Ship::Archive>("mods/TexMods.otr", "", false);
		for (auto& mod : modules) {
			mod->Init();
			// mod->Open(archive);
		}
	}

	void ModManager::Exit() {
		for (auto& mod : modules)
			mod->Exit();
	}
}
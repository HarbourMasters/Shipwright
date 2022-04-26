#include "ModManager.h"
#include "TextureMod.h"
#include "ModModule.h"
#include "SohImGuiImpl.h"

namespace Ship {

	void ModManager::Init() {
		TextureMod = new TextureModule(this);
		const std::shared_ptr<Archive> archive = std::make_shared<Archive>("mods/TexModDemo.otr", "", false);
		static_cast<ModModule*>(TextureMod)->Init();
		static_cast<ModModule*>(TextureMod)->Open(archive);
	}

	void ModManager::Exit() {
		static_cast<ModModule*>(TextureMod)->Exit();
	}
}
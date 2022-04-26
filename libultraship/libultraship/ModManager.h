#pragma once
#include "ResourceMgr.h"

namespace Ship {
	class TextureModule;

	class ModManager {
	public:
		std::shared_ptr<ResourceMgr> ResManager;
		TextureModule* TextureMod;
		explicit ModManager(std::shared_ptr<ResourceMgr> manager) : ResManager(manager) {}
		void Init();
		void Exit();
	};
}

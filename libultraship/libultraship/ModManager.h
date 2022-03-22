#pragma once
#include "ResourceMgr.h"

namespace Ship {
	class ModManager {
	public:
		std::shared_ptr<Ship::ResourceMgr> ResManager;
		explicit ModManager(std::shared_ptr<Ship::ResourceMgr> manager) : ResManager(manager) {}
		void Init();
		void Exit();
	};
}
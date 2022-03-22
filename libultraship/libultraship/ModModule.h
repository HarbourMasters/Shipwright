#pragma once
#include "ModManager.h"
#include "SohHooks.h"
#include "SohConsole.h"
#include "Archive.h"

namespace Ship {
	class ModModule {
	protected:
		ModManager* Manager;
	public:
		virtual ~ModModule() = default;
		explicit ModModule(ModManager* Manager) : Manager(Manager) {}
		virtual void Init() = 0;
		virtual void Open(std::shared_ptr<Ship::Archive> archive) = 0;
		virtual void Close(Ship::Archive archive) = 0;
		virtual void Exit() = 0;
	};
}
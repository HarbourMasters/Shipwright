#ifndef GLOBAL_CTX_2
#define GLOBAL_CTX_2

#pragma once

#ifdef __cplusplus
#include <memory>
#include "spdlog/spdlog.h"
#include "ConfigFile.h"

namespace Ship {
	class ResourceMgr;
	class Window;

	class GlobalCtx2 {
		public:
			static std::shared_ptr<GlobalCtx2> GetInstance();
			static std::shared_ptr<GlobalCtx2> CreateInstance(const std::string& Name);

			std::string GetName() { return Name; }
			std::shared_ptr<Window> GetWindow() { return Win; }
			std::shared_ptr<ResourceMgr> GetResourceManager() { return ResMan; }
			std::shared_ptr<spdlog::logger> GetLogger() { return Logger; }
			std::shared_ptr<ConfigFile> GetConfig() { return Config; }

			GlobalCtx2(const std::string& Name);
			~GlobalCtx2();

		protected:
			void InitWindow();
			void InitLogging();

		private:
			static std::weak_ptr <GlobalCtx2> Context;
			std::shared_ptr<spdlog::logger> Logger;
			std::shared_ptr<Window> Win;
			std::shared_ptr<ConfigFile> Config; // Config needs to be after the Window because we call the Window during it's destructor.
			std::shared_ptr<ResourceMgr> ResMan;
			std::string Name;
			std::string MainPath;
			std::string PatchesPath;
	};
}
#endif

#endif

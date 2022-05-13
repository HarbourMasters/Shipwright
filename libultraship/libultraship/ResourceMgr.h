#pragma once

#include <map>
#include <string>
#include <thread>
#include <queue>
#include "Resource.h"
#include "GlobalCtx2.h"

namespace Ship
{
	class Archive;
	class File;

	// Resource manager caches any and all files it comes across into memory. This will be unoptimal in the future when modifications have gigabytes of assets.
	// It works with the original game's assets because the entire ROM is 64MB and fits into RAM of any semi-modern PC.
	class ResourceMgr {
	public:
		ResourceMgr(std::shared_ptr<GlobalCtx2> Context, std::string MainPath, std::string PatchesPath);
		~ResourceMgr();

		bool IsRunning();
		bool DidLoadSuccessfully();

		std::shared_ptr<Archive> GetArchive() { return OTR; }
		std::shared_ptr<GlobalCtx2> GetContext() { return Context.lock(); }

		std::string HashToString(uint64_t Hash);

		void InvalidateResourceCache();

		uint32_t GetGameVersion();
		void SetGameVersion(uint32_t newGameVersion);
		std::shared_ptr<File> LoadFileAsync(std::string FilePath);
		std::shared_ptr<File> LoadFile(std::string FilePath);
		std::shared_ptr<Ship::Resource> GetCachedFile(std::string FilePath);
		std::shared_ptr<Resource> LoadResource(std::string FilePath);
		std::shared_ptr<ResourcePromise> LoadResourceAsync(std::string FilePath);
		std::shared_ptr<std::vector<std::shared_ptr<Resource>>> CacheDirectory(std::string SearchMask);
		std::shared_ptr<std::vector<std::shared_ptr<ResourcePromise>>> CacheDirectoryAsync(std::string SearchMask);
		std::shared_ptr<std::vector<std::shared_ptr<Resource>>> DirtyDirectory(std::string SearchMask);

	protected:
		void Start();
		void Stop();
		void LoadFileThread();
		void LoadResourceThread();

	private:
		std::weak_ptr<GlobalCtx2> Context;
		volatile bool bIsRunning;
		std::map<std::string, std::shared_ptr<File>> FileCache;
		std::map<std::string, std::shared_ptr<Resource>> ResourceCache;
		std::queue<std::shared_ptr<File>> FileLoadQueue;
		std::queue<std::shared_ptr<ResourcePromise>> ResourceLoadQueue;
		std::shared_ptr<Archive> OTR;
		std::shared_ptr<std::thread> FileLoadThread;
		std::shared_ptr<std::thread> ResourceLoadThread;
		std::mutex FileLoadMutex;
		std::mutex ResourceLoadMutex;
		std::condition_variable FileLoadNotifier;
		std::condition_variable ResourceLoadNotifier;
		uint32_t gameVersion;
	};
}
#pragma once

#include <unordered_map>
#include <string>
#include <thread>
#include <queue>
#include <variant>
#include "Window.h"
#include "Resource.h"
#include "Archive.h"
#include "File.h"

namespace Ship {
	class Window;

	// Resource manager caches any and all files it comes across into memory. This will be unoptimal in the future when modifications have gigabytes of assets.
	// It works with the original game's assets because the entire ROM is 64MB and fits into RAM of any semi-modern PC.
	class ResourceMgr {
	public:
		ResourceMgr(std::shared_ptr<Window> Context, const std::string& MainPath, const std::string& PatchesPath, const std::unordered_set<uint32_t> &ValidHashes);
        ResourceMgr(std::shared_ptr<Window> Context, const std::vector<std::string> &OTRFiles, const std::unordered_set<uint32_t> &ValidHashes);
        ~ResourceMgr();

		bool IsRunning();
		bool DidLoadSuccessfully();

		std::shared_ptr<Archive> GetArchive();
		std::shared_ptr<Window> GetContext();
		const std::string* HashToString(uint64_t Hash) const;
		void InvalidateResourceCache();
		uint32_t GetGameVersion();
		void SetGameVersion(uint32_t newGameVersion);
        std::vector<uint32_t> GetGameVersions();
        void PushGameVersion(uint32_t newGameVersion);
        std::shared_ptr<File> LoadFileAsync(const std::string& FilePath);
		std::shared_ptr<File> LoadFile(const std::string& FilePath);
		std::shared_ptr<Resource> GetCachedFile(const char* FilePath) const;
		std::shared_ptr<Resource> LoadResource(const char* FilePath);
		std::shared_ptr<Resource> LoadResource(const std::string& FilePath);
		std::variant<std::shared_ptr<Resource>, std::shared_ptr<ResourcePromise>> LoadResourceAsync(const char* FilePath);
		std::shared_ptr<std::vector<std::shared_ptr<Resource>>> CacheDirectory(const std::string& SearchMask);
		std::shared_ptr<std::vector<std::shared_ptr<ResourcePromise>>> CacheDirectoryAsync(const std::string& SearchMask);
		std::shared_ptr<std::vector<std::shared_ptr<Resource>>> DirtyDirectory(const std::string& SearchMask);
		std::shared_ptr<std::vector<std::string>> ListFiles(std::string SearchMask);

	protected:
		void Start();
		void Stop();
		void LoadFileThread();
		void LoadResourceThread();

	private:
		std::shared_ptr<Window> Context;
		volatile bool bIsRunning;
		std::unordered_map<std::string, std::shared_ptr<File>> FileCache;
		std::unordered_map<std::string, std::shared_ptr<Resource>> ResourceCache;
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
        std::vector<uint32_t> gameVersions;
	};
}
#include "ResourceMgr.h"
#include "Factories/ResourceLoader.h"
#include "spdlog/spdlog.h"
#include "File.h"
#include "Archive.h"
#include "GameVersions.h"
#include <Utils/StringHelper.h>
#include "StormLib.h"

namespace Ship {

	ResourceMgr::ResourceMgr(std::shared_ptr<Window> Context, const std::string& MainPath, const std::string& PatchesPath, const std::unordered_set<uint32_t>& ValidHashes) 
        : Context(Context), bIsRunning(false), FileLoadThread(nullptr) {
		OTR = std::make_shared<Archive>(MainPath, PatchesPath, ValidHashes, false);

		gameVersion = UNKNOWN;

		if (OTR->IsMainMPQValid()) {
			Start();
		}
	}

    ResourceMgr::ResourceMgr(std::shared_ptr<Window> Context, const std::vector<std::string> &OTRFiles, const std::unordered_set<uint32_t> &ValidHashes)
        : Context(Context), bIsRunning(false), FileLoadThread(nullptr)
    {
        OTR = std::make_shared<Archive>(OTRFiles, ValidHashes, false);

        gameVersion = UNKNOWN;

        if (OTR->IsMainMPQValid()) {
            Start();
        }
    }

    ResourceMgr::~ResourceMgr() {
		SPDLOG_INFO("destruct ResourceMgr");
		Stop();

		FileCache.clear();
		ResourceCache.clear();
	}

	void ResourceMgr::Start() {
		const std::lock_guard<std::mutex> FileLock(FileLoadMutex);
		const std::lock_guard<std::mutex> ResLock(ResourceLoadMutex);
		if (!IsRunning()) {
			bIsRunning = true;
			FileLoadThread = std::make_shared<std::thread>(&ResourceMgr::LoadFileThread, this);
			ResourceLoadThread = std::make_shared<std::thread>(&ResourceMgr::LoadResourceThread, this);
		}
	}

	void ResourceMgr::Stop() {
		if (IsRunning()) {
			{
				const std::lock_guard<std::mutex> FileLock(FileLoadMutex);
				const std::lock_guard<std::mutex> ResLock(ResourceLoadMutex);
				bIsRunning = false;
			}

			FileLoadNotifier.notify_all();
			ResourceLoadNotifier.notify_all();
			FileLoadThread->join();
			ResourceLoadThread->join();

			if (!FileLoadQueue.empty()) {
				SPDLOG_INFO("Resource manager stopped, but has {} Files left to load.", FileLoadQueue.size());
			}

			if (!ResourceLoadQueue.empty()) {
				SPDLOG_INFO("Resource manager stopped, but has {} Resources left to load.", FileLoadQueue.size());
			}
		}
	}

	bool ResourceMgr::IsRunning() {
		return bIsRunning && FileLoadThread != nullptr;
	}

	bool ResourceMgr::DidLoadSuccessfully()
	{
		return OTR != nullptr && OTR->IsMainMPQValid();
	}

	void ResourceMgr::LoadFileThread() {
		SPDLOG_INFO("Resource Manager LoadFileThread started");

		while (true) {
			std::unique_lock<std::mutex> Lock(FileLoadMutex);

			while (bIsRunning && FileLoadQueue.empty()) {
				FileLoadNotifier.wait(Lock);
			}

			if (!bIsRunning) {
				break;
			}

			std::shared_ptr<File> ToLoad = FileLoadQueue.front();
			FileLoadQueue.pop();

			OTR->LoadFile(ToLoad->path, true, ToLoad);

			if (!ToLoad->bHasLoadError) {
				FileCache[ToLoad->path] = ToLoad->bIsLoaded && !ToLoad->bHasLoadError ? ToLoad : nullptr;
			}

			SPDLOG_TRACE("Loaded File {} on ResourceMgr thread", ToLoad->path);

			ToLoad->FileLoadNotifier.notify_all();
		}

		SPDLOG_INFO("Resource Manager LoadFileThread ended");
	}

	void ResourceMgr::LoadResourceThread() {
		SPDLOG_INFO("Resource Manager LoadResourceThread started");

		while (true) {
			std::unique_lock<std::mutex> ResLock(ResourceLoadMutex);
			while (bIsRunning && ResourceLoadQueue.empty()) {
				ResourceLoadNotifier.wait(ResLock);
			}

			if (!bIsRunning) {
				break;
			}

			std::shared_ptr<ResourcePromise> ToLoad = nullptr;
			ToLoad = ResourceLoadQueue.front();
			ResourceLoadQueue.pop();

			// Wait for the underlying File to complete loading
			{
				std::unique_lock<std::mutex> FileLock(ToLoad->file->FileLoadMutex);
				while (!ToLoad->file->bIsLoaded && !ToLoad->file->bHasLoadError) {
					ToLoad->file->FileLoadNotifier.wait(FileLock);
				}
			}

			if (!ToLoad->file->bHasLoadError) {
				auto UnmanagedRes = ResourceLoader::LoadResource(ToLoad->file);

				if (UnmanagedRes != nullptr) {
					UnmanagedRes->resMgr = this;
					auto Res = std::shared_ptr<Resource>(UnmanagedRes);

					if (Res != nullptr) {
						std::unique_lock<std::mutex> Lock(ToLoad->resourceLoadMutex);

						ToLoad->bHasResourceLoaded = true;
						ToLoad->resource = Res;
						ResourceCache[Res->file->path] = Res;

						SPDLOG_TRACE("Loaded Resource {} on ResourceMgr thread", ToLoad->file->path);

						Res->file = nullptr;
					} else {
						ToLoad->bHasResourceLoaded = false;
						ToLoad->resource = nullptr;

						SPDLOG_ERROR("Resource load FAILED {} on ResourceMgr thread", ToLoad->file->path);
					}
				}
			} else {
				ToLoad->bHasResourceLoaded = false;
				ToLoad->resource = nullptr;
			}

			ToLoad->resourceLoadNotifier.notify_all();
		}

		SPDLOG_INFO("Resource Manager LoadResourceThread ended");
	}

	uint32_t ResourceMgr::GetGameVersion() {
		return gameVersion;
	}

	void ResourceMgr::SetGameVersion(uint32_t newGameVersion) {
		gameVersion = newGameVersion;
	}

    std::vector<uint32_t> ResourceMgr::GetGameVersions() {
        return OTR->gameVersions;
    }

    void ResourceMgr::PushGameVersion(uint32_t newGameVersion) {
        OTR->gameVersions.push_back(newGameVersion);
    }

	std::shared_ptr<File> ResourceMgr::LoadFileAsync(const std::string& FilePath) {
		const std::lock_guard<std::mutex> Lock(FileLoadMutex);
		// File NOT already loaded...?
		auto fileCacheFind = FileCache.find(FilePath);
		if (fileCacheFind == FileCache.end()) {
			SPDLOG_TRACE("Cache miss on File load: {}", FilePath.c_str());
			std::shared_ptr<File> ToLoad = std::make_shared<File>();
			ToLoad->path = FilePath;

			FileLoadQueue.push(ToLoad);
			FileLoadNotifier.notify_all();

			return ToLoad;
		}

		return fileCacheFind->second;
	}

	std::shared_ptr<File> ResourceMgr::LoadFile(const std::string& FilePath) {
		auto ToLoad = LoadFileAsync(FilePath);
		// Wait for the File to actually be loaded if we are told to block.
		std::unique_lock<std::mutex> Lock(ToLoad->FileLoadMutex);
		while (!ToLoad->bIsLoaded && !ToLoad->bHasLoadError) {
			ToLoad->FileLoadNotifier.wait(Lock);
		}

		return ToLoad;
	}

	std::shared_ptr<Ship::Resource> ResourceMgr::GetCachedFile(const char* FilePath) const {
		auto resCacheFind = ResourceCache.find(FilePath);

		if (resCacheFind != ResourceCache.end() &&
			resCacheFind->second.use_count() > 0) {
			return resCacheFind->second;
		} else {
			return nullptr;
		}
	}

	std::shared_ptr<Resource> ResourceMgr::LoadResource(const char* FilePath) {
		auto Res = LoadResourceAsync(FilePath);

		if (std::holds_alternative<std::shared_ptr<Resource>>(Res)) {
			return std::get<std::shared_ptr<Resource>>(Res);
		}

		auto& Promise = std::get<std::shared_ptr<ResourcePromise>>(Res);

		if (!Promise->bHasResourceLoaded) {
			std::unique_lock<std::mutex> Lock(Promise->resourceLoadMutex);
			while (!Promise->bHasResourceLoaded) {
				Promise->resourceLoadNotifier.wait(Lock);
			}
		}

		return Promise->resource;
	}

	std::variant<std::shared_ptr<Resource>, std::shared_ptr<ResourcePromise>> ResourceMgr::LoadResourceAsync(const char* FilePath) {
		if (FilePath[0] == '_' && FilePath[1] == '_' && FilePath[2] == 'O' && FilePath[3] == 'T' && FilePath[4] == 'R' && FilePath[5] == '_' && FilePath[6] == '_') {
			FilePath += 7;
		}

		const std::lock_guard<std::mutex> ResLock(ResourceLoadMutex);
		auto resCacheFind = ResourceCache.find(FilePath);
		if (resCacheFind == ResourceCache.end() || resCacheFind->second->isDirty/* || !FileData->bIsLoaded*/) {
			if (resCacheFind == ResourceCache.end()) {
				SPDLOG_TRACE("Cache miss on Resource load: {}", FilePath);
			}

			std::shared_ptr<ResourcePromise> Promise = std::make_shared<ResourcePromise>();
			std::shared_ptr<File> FileData = LoadFile(FilePath);
			Promise->file = FileData;

			if (Promise->file->bHasLoadError) {
				Promise->bHasResourceLoaded = true;
			} else {
				Promise->bHasResourceLoaded = false;
				ResourceLoadQueue.push(Promise);
				ResourceLoadNotifier.notify_all();
			}

			return Promise;
		} else {
			return resCacheFind->second;
		}
	}

	std::shared_ptr<std::vector<std::shared_ptr<ResourcePromise>>> ResourceMgr::CacheDirectoryAsync(const std::string& SearchMask) {
		auto loadedList = std::make_shared<std::vector<std::shared_ptr<ResourcePromise>>>();
		auto fileList = OTR->ListFiles(SearchMask);

		for (DWORD i = 0; i < fileList.size(); i++) {
			auto resource = LoadResourceAsync(fileList.operator[](i).cFileName);
			if (std::holds_alternative<std::shared_ptr<Resource>>(resource)) {
				auto promise = std::make_shared<ResourcePromise>();
				promise->bHasResourceLoaded = true;
				promise->resource = std::get<std::shared_ptr<Resource>>(resource);
				resource = promise;
			}
			loadedList->push_back(std::get<std::shared_ptr<ResourcePromise>>(resource));
		}

		return loadedList;
	}

	std::shared_ptr<std::vector<std::shared_ptr<Resource>>> ResourceMgr::CacheDirectory(const std::string& SearchMask) {
		auto PromiseList = CacheDirectoryAsync(SearchMask);
		auto LoadedList = std::make_shared<std::vector<std::shared_ptr<Resource>>>();

		for (size_t i = 0; i < PromiseList->size(); i++) {
			auto Promise = PromiseList->at(i);

			std::unique_lock<std::mutex> Lock(Promise->resourceLoadMutex);
			while (!Promise->bHasResourceLoaded) {
				Promise->resourceLoadNotifier.wait(Lock);
			}

			LoadedList->push_back(Promise->resource);
		}

		return LoadedList;
	}

	std::shared_ptr<std::vector<std::shared_ptr<Resource>>> ResourceMgr::DirtyDirectory(const std::string& SearchMask) {
		auto PromiseList = CacheDirectoryAsync(SearchMask);
		auto LoadedList = std::make_shared<std::vector<std::shared_ptr<Resource>>>();

		for (size_t i = 0; i < PromiseList->size(); i++) {
			auto Promise = PromiseList->at(i);

			std::unique_lock<std::mutex> Lock(Promise->resourceLoadMutex);
			while (!Promise->bHasResourceLoaded) {
				Promise->resourceLoadNotifier.wait(Lock);
			}

			if (Promise->resource != nullptr) {
				Promise->resource->isDirty = true;
			}

			LoadedList->push_back(Promise->resource);
		}

		return LoadedList;
	}

	std::shared_ptr<std::vector<std::string>> ResourceMgr::ListFiles(std::string SearchMask) {
		auto result = std::make_shared<std::vector<std::string>>();
		auto fileList = OTR->ListFiles(SearchMask);

		for (DWORD i = 0; i < fileList.size(); i++) {
			result->push_back(fileList[i].cFileName);
		}

		return result;
	}

	void ResourceMgr::InvalidateResourceCache() {
		ResourceCache.clear();
	}

	const std::string* ResourceMgr::HashToString(uint64_t Hash) const {
		return OTR->HashToString(Hash);
	}

	std::shared_ptr<Archive> ResourceMgr::GetArchive() {
		return OTR;
	}

	std::shared_ptr<Window> ResourceMgr::GetContext() {
		return Context;
	}

	std::shared_ptr<Resource> ResourceMgr::LoadResource(const std::string& FilePath) {
		return LoadResource(FilePath.c_str());
	}

}

#include "ResourceMgr.h"
#include "Factories/ResourceLoader.h"
#include "spdlog/spdlog.h"
#include "File.h"
#include "Archive.h"
#include "GameVersions.h"
#include <Utils/StringHelper.h>
#include "Lib/StormLib/StormLib.h"

namespace Ship {

	ResourceMgr::ResourceMgr(std::shared_ptr<GlobalCtx2> Context, std::string MainPath, std::string PatchesPath) : Context(Context), bIsRunning(false), FileLoadThread(nullptr) {
		OTR = std::make_shared<Archive>(MainPath, PatchesPath, false);

		gameVersion = OOT_UNKNOWN;

		if (OTR->IsMainMPQValid())
			Start();
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

			//Lock.lock();
			std::shared_ptr<File> ToLoad = FileLoadQueue.front();
			FileLoadQueue.pop();
			//Lock.unlock();

			OTR->LoadFile(ToLoad->path, true, ToLoad);
			//Lock.lock();

			if (!ToLoad->bHasLoadError)
				FileCache[ToLoad->path] = ToLoad->bIsLoaded && !ToLoad->bHasLoadError ? ToLoad : nullptr;

			//Lock.unlock();

			SPDLOG_DEBUG("Loaded File {} on ResourceMgr thread", ToLoad->path);

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
			//ResLock.lock();
			ToLoad = ResourceLoadQueue.front();
			ResourceLoadQueue.pop();
			//ResLock.unlock();

			// Wait for the underlying File to complete loading
			{
				std::unique_lock<std::mutex> FileLock(ToLoad->file->FileLoadMutex);
				while (!ToLoad->file->bIsLoaded && !ToLoad->file->bHasLoadError) {
					ToLoad->file->FileLoadNotifier.wait(FileLock);
				}
			}

			if (!ToLoad->file->bHasLoadError)
			{
				auto UnmanagedRes = ResourceLoader::LoadResource(ToLoad->file);

				if (UnmanagedRes != nullptr)
				{
					UnmanagedRes->resMgr = this;
					auto Res = std::shared_ptr<Resource>(UnmanagedRes);

					if (Res != nullptr) {
						std::unique_lock<std::mutex> Lock(ToLoad->resourceLoadMutex);

						ToLoad->bHasResourceLoaded = true;
						ToLoad->resource = Res;
						ResourceCache[Res->file->path] = Res;

						SPDLOG_DEBUG("Loaded Resource {} on ResourceMgr thread", ToLoad->file->path);

						// Disabled for now because it can cause random crashes
						//FileCache[Res->File->path] = nullptr;
						//FileCache.erase(FileCache.find(Res->File->path));
						Res->file = nullptr;
					}
					else {
						ToLoad->bHasResourceLoaded = false;
						ToLoad->resource = nullptr;

						SPDLOG_ERROR("Resource load FAILED {} on ResourceMgr thread", ToLoad->file->path);
					}

					//ResLock.lock();
					//ResLock.unlock();
				}
			}
			else
			{
				ToLoad->bHasResourceLoaded = false;
				ToLoad->resource = nullptr;
			}

			ToLoad->resourceLoadNotifier.notify_all();
		}

		SPDLOG_INFO("Resource Manager LoadResourceThread ended");
	}

	uint32_t ResourceMgr::GetGameVersion()
	{
		return gameVersion;
	}

	void ResourceMgr::SetGameVersion(uint32_t newGameVersion)
	{
		gameVersion = newGameVersion;
	}

	std::shared_ptr<File> ResourceMgr::LoadFileAsync(std::string FilePath) {
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

	std::shared_ptr<File> ResourceMgr::LoadFile(std::string FilePath) {
		auto ToLoad = LoadFileAsync(FilePath);
		// Wait for the File to actually be loaded if we are told to block.
		std::unique_lock<std::mutex> Lock(ToLoad->FileLoadMutex);
		while (!ToLoad->bIsLoaded && !ToLoad->bHasLoadError) {
			ToLoad->FileLoadNotifier.wait(Lock);
		}

		return ToLoad;
	}

	std::shared_ptr<Ship::Resource> ResourceMgr::GetCachedFile(std::string FilePath) {
		auto resCacheFind = ResourceCache.find(FilePath);

		if (resCacheFind != ResourceCache.end() &&
			resCacheFind->second.use_count() > 0)
		{
			return resCacheFind->second;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Resource> ResourceMgr::LoadResource(std::string FilePath) {
		auto Promise = LoadResourceAsync(FilePath);

		if (!Promise->bHasResourceLoaded)
		{
			std::unique_lock<std::mutex> Lock(Promise->resourceLoadMutex);
			while (!Promise->bHasResourceLoaded) {
				Promise->resourceLoadNotifier.wait(Lock);
			}
		}

		return Promise->resource;
	}

	std::shared_ptr<ResourcePromise> ResourceMgr::LoadResourceAsync(std::string FilePath) {
		StringHelper::ReplaceOriginal(FilePath, "\\", "/");

		if (StringHelper::StartsWith(FilePath, "__OTR__"))
			FilePath = StringHelper::Split(FilePath, "__OTR__")[1];

		std::shared_ptr<ResourcePromise> Promise = std::make_shared<ResourcePromise>();

		const std::lock_guard<std::mutex> ResLock(ResourceLoadMutex);
		auto resCacheFind = ResourceCache.find(FilePath);
		if (resCacheFind == ResourceCache.end() || resCacheFind->second->isDirty/* || !FileData->bIsLoaded*/) {
			if (resCacheFind == ResourceCache.end()) {
				SPDLOG_TRACE("Cache miss on Resource load: {}", FilePath.c_str());
			}

			std::shared_ptr<File> FileData = LoadFile(FilePath);
			Promise->file = FileData;

			if (Promise->file->bHasLoadError)
			{
				Promise->bHasResourceLoaded = true;
			}
			else
			{
				Promise->bHasResourceLoaded = false;
				ResourceLoadQueue.push(Promise);
				ResourceLoadNotifier.notify_all();
			}
		} else {
			Promise->bHasResourceLoaded = true;
			Promise->resource = resCacheFind->second;
		}

		return Promise;
	}

	std::shared_ptr<std::vector<std::shared_ptr<ResourcePromise>>> ResourceMgr::CacheDirectoryAsync(std::string SearchMask) {
		auto loadedList = std::make_shared<std::vector<std::shared_ptr<ResourcePromise>>>();
		auto fileList = OTR->ListFiles(SearchMask);

		for (DWORD i = 0; i < fileList.size(); i++) {
			auto file = LoadResourceAsync(fileList.operator[](i).cFileName);
			if (file != nullptr) {
				loadedList->push_back(file);
			}
		}

		return loadedList;
	}

	std::shared_ptr<std::vector<std::shared_ptr<Resource>>> ResourceMgr::CacheDirectory(std::string SearchMask) {
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

	std::shared_ptr<std::vector<std::shared_ptr<Resource>>> ResourceMgr::DirtyDirectory(std::string SearchMask)
	{
		auto PromiseList = CacheDirectoryAsync(SearchMask);
		auto LoadedList = std::make_shared<std::vector<std::shared_ptr<Resource>>>();

		for (size_t i = 0; i < PromiseList->size(); i++) {
			auto Promise = PromiseList->at(i);

			std::unique_lock<std::mutex> Lock(Promise->resourceLoadMutex);
			while (!Promise->bHasResourceLoaded) {
				Promise->resourceLoadNotifier.wait(Lock);
			}

			if (Promise->resource != nullptr)
				Promise->resource->isDirty = true;

			LoadedList->push_back(Promise->resource);
		}

		return LoadedList;
	}

	void ResourceMgr::InvalidateResourceCache() {
		ResourceCache.clear();
	}

	std::string ResourceMgr::HashToString(uint64_t Hash) {
		return OTR->HashToString(Hash);
	}
}
#pragma once

#undef _DLL

#include <string>

#include <stdint.h>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <unordered_set>
#include "Resource.h"
#include "StormLib.h"


namespace Ship
{
	class File;

	class Archive : public std::enable_shared_from_this<Archive>
	{
	public:
		Archive(const std::string& MainPath, bool enableWriting);
        Archive(const std::string &MainPath, const std::string &PatchesPath, const std::unordered_set<uint32_t> &ValidHashes, bool enableWriting, bool genCRCMap = true);
        Archive(const std::vector<std::string> &OTRFiles, const std::unordered_set<uint32_t> &ValidHashes, bool enableWriting, bool genCRCMap = true);
        ~Archive();

		bool IsMainMPQValid();

		static std::shared_ptr<Archive> CreateArchive(const std::string& archivePath, int fileCapacity);
		
		std::shared_ptr<File> LoadFile(const std::string& filePath, bool includeParent = true, std::shared_ptr<File> FileToLoad = nullptr);
		std::shared_ptr<File> LoadPatchFile(const std::string& filePath, bool includeParent = true, std::shared_ptr<File> FileToLoad = nullptr);

		bool AddFile(const std::string& path, uintptr_t fileData, DWORD dwFileSize);
		bool RemoveFile(const std::string& path);
		bool RenameFile(const std::string& oldPath, const std::string& newPath);
		std::vector<SFILE_FIND_DATA> ListFiles(const std::string& searchMask) const;
		bool HasFile(const std::string& searchMask) const;
		const std::string* HashToString(uint64_t hash) const;
        std::vector<uint32_t> gameVersions;
	protected:
		bool Load(bool enableWriting, bool genCRCMap);
		bool Unload();
	private:
		std::string MainPath;
		std::string PatchesPath;
        std::vector<std::string> OTRFiles;
        std::unordered_set<uint32_t> ValidHashes;
        std::map<std::string, HANDLE> mpqHandles;
		std::vector<std::string> addedFiles;
		std::unordered_map<uint64_t, std::string> hashes;
		HANDLE mainMPQ;

		bool LoadMainMPQ(bool enableWriting, bool genCRCMap);
		bool LoadPatchMPQs();
		bool LoadPatchMPQ(const std::string& path, bool validateVersion = false);
        void GenerateCRCMap();
        bool PushGameVersion(HANDLE mpqHandle = nullptr);
        std::shared_ptr<File> LoadFileFromHandle(const std::string &filePath, bool includeParent = true, std::shared_ptr<File> FileToLoad = nullptr, HANDLE mpqHandle = nullptr);
    };
}
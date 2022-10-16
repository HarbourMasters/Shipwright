#include "Archive.h"
#include "Resource.h"
#include "File.h"
#include "Window.h"
#include "ResourceMgr.h"
#include "spdlog/spdlog.h"
#include "Utils/StringHelper.h"
#include "Lib/StrHash64.h"
#include <filesystem>
#include "Lib/BinaryTools/BinaryTools/BinaryReader.h"

#ifdef __SWITCH__
#include "SwitchImpl.h"
#endif

namespace Ship {
	Archive::Archive(const std::string& MainPath, bool enableWriting) : Archive(MainPath, "", std::unordered_set<uint32_t>(), enableWriting)
	{
		mainMPQ = nullptr;
	}

	Archive::Archive(const std::string& MainPath, const std::string& PatchesPath, const std::unordered_set<uint32_t>& ValidHashes, bool enableWriting, bool genCRCMap)
        : MainPath(MainPath), PatchesPath(PatchesPath), OTRFiles({}), ValidHashes(ValidHashes) {
		mainMPQ = nullptr;
		Load(enableWriting, genCRCMap);
	}

    Archive::Archive(const std::vector<std::string> &OTRFiles, const std::unordered_set<uint32_t> &ValidHashes, bool enableWriting, bool genCRCMap)
        : OTRFiles(OTRFiles), ValidHashes(ValidHashes)
    {
        mainMPQ = nullptr;
        Load(enableWriting, genCRCMap);
    }

    Archive::~Archive() {
		Unload();
	}

	bool Archive::IsMainMPQValid()
	{
		return mainMPQ != nullptr;
	}

	std::shared_ptr<Archive> Archive::CreateArchive(const std::string& archivePath, int fileCapacity)
	{
		auto archive = std::make_shared<Archive>(archivePath, true);

		TCHAR* t_filename = new TCHAR[archivePath.size() + 1];
		t_filename[archivePath.size()] = 0;
		std::copy(archivePath.begin(), archivePath.end(), t_filename);

		bool success = SFileCreateArchive(t_filename, MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_CREATE_ARCHIVE_V2, fileCapacity, &archive->mainMPQ);
		int32_t error = GetLastError();

		delete[] t_filename;

		if (success)
		{
			archive->mpqHandles[archivePath] = archive->mainMPQ;
			return archive;
		}
		else
		{
			SPDLOG_ERROR("({}) We tried to create an archive, but it has fallen and cannot get up.", error);
			return nullptr;
		}
	}

    std::shared_ptr<File> Archive::LoadFileFromHandle(const std::string& filePath, bool includeParent, std::shared_ptr<File> FileToLoad, HANDLE mpqHandle)
    {
        HANDLE fileHandle = NULL;

        if (FileToLoad == nullptr)
        {
            FileToLoad = std::make_shared<File>();
            FileToLoad->path = filePath;
        }

        if (mpqHandle == nullptr)
        {
            mpqHandle = mainMPQ;
        }

        bool attempt = SFileOpenFileEx(mpqHandle, filePath.c_str(), 0, &fileHandle);

        if (!attempt)
        {
            SPDLOG_ERROR("({}) Failed to open file {} from mpq archive  {}.", GetLastError(), filePath.c_str(), MainPath.c_str());
            std::unique_lock<std::mutex> Lock(FileToLoad->FileLoadMutex);
            FileToLoad->bHasLoadError = true;
            return FileToLoad;
        }

        DWORD dwFileSize = SFileGetFileSize(fileHandle, 0);
        std::shared_ptr<char[]> fileData(new char[dwFileSize]);
        DWORD dwBytes;

        if (!SFileReadFile(fileHandle, fileData.get(), dwFileSize, &dwBytes, NULL))
        {
            SPDLOG_ERROR("({}) Failed to read file {} from mpq archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
            if (!SFileCloseFile(fileHandle))
            {
                SPDLOG_ERROR("({}) Failed to close file {} from mpq after read failure in archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
            }
            std::unique_lock<std::mutex> Lock(FileToLoad->FileLoadMutex);
            FileToLoad->bHasLoadError = true;
            return FileToLoad;
        }

        if (!SFileCloseFile(fileHandle))
        {
            SPDLOG_ERROR("({}) Failed to close file {} from mpq archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
        }

        std::unique_lock<std::mutex> Lock(FileToLoad->FileLoadMutex);
        FileToLoad->parent = includeParent ? shared_from_this() : nullptr;
        FileToLoad->buffer = fileData;
        FileToLoad->dwBufferSize = dwFileSize;
        FileToLoad->bIsLoaded = true;

        return FileToLoad;
    }

	std::shared_ptr<File> Archive::LoadFile(const std::string& filePath, bool includeParent, std::shared_ptr<File> FileToLoad) {
		return LoadFileFromHandle(filePath, includeParent, FileToLoad, nullptr);
	}

	std::shared_ptr<File> Archive::LoadPatchFile(const std::string& filePath, bool includeParent, std::shared_ptr<File> FileToLoad) {
		HANDLE fileHandle = NULL;
		HANDLE mpqHandle = NULL;

		if (FileToLoad == nullptr) {
			FileToLoad = std::make_shared<File>();
			FileToLoad->path = filePath;
		}

		for(auto [path, handle] : mpqHandles) {
			if (SFileOpenFileEx(mpqHandle, filePath.c_str(), 0, &fileHandle)) {
				std::unique_lock Lock(FileToLoad->FileLoadMutex);
				FileToLoad->bHasLoadError = false;
				mpqHandle = handle;
			}
		}

		if(!mpqHandle) {
			FileToLoad->bHasLoadError = true;
			return FileToLoad;
		}

		DWORD dwFileSize = SFileGetFileSize(fileHandle, 0);
		std::shared_ptr<char[]> fileData(new char[dwFileSize]);
		DWORD dwBytes;

		if (!SFileReadFile(fileHandle, fileData.get(), dwFileSize, &dwBytes, NULL)) {
			SPDLOG_ERROR("({}) Failed to read file {} from mpq archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
			if (!SFileCloseFile(fileHandle)) {
				SPDLOG_ERROR("({}) Failed to close file {} from mpq after read failure in archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
			}
			std::unique_lock<std::mutex> Lock(FileToLoad->FileLoadMutex);
			FileToLoad->bHasLoadError = true;
			return FileToLoad;
		}

		if (!SFileCloseFile(fileHandle)) {
			SPDLOG_ERROR("({}) Failed to close file {} from mpq archive {}", GetLastError(), filePath.c_str(), MainPath.c_str());
		}

		std::unique_lock<std::mutex> Lock(FileToLoad->FileLoadMutex);
		FileToLoad->parent = includeParent ? shared_from_this() : nullptr;
		FileToLoad->buffer = fileData;
		FileToLoad->dwBufferSize = dwFileSize;
		FileToLoad->bIsLoaded = true;

		return FileToLoad;
	}

	bool Archive::AddFile(const std::string& oPath, uintptr_t fileData, DWORD dwFileSize) {
		HANDLE hFile;
#ifdef _WIN32
		SYSTEMTIME sysTime;
		GetSystemTime(&sysTime);
		FILETIME t;
		SystemTimeToFileTime(&sysTime, &t);
		ULONGLONG stupidHack = static_cast<uint64_t>(t.dwHighDateTime) << (sizeof(t.dwHighDateTime) * 8) | t.dwLowDateTime;
#else
		time_t stupidHack;
		time(&stupidHack);
#endif

		std::string path = oPath;

		StringHelper::ReplaceOriginal(path, "\\", "/");

		if (!SFileCreateFile(mainMPQ, path.c_str(), stupidHack, dwFileSize, 0, MPQ_FILE_COMPRESS, &hFile)) {
			SPDLOG_ERROR("({}) Failed to create file of {} bytes {} in archive {}", GetLastError(), dwFileSize, path.c_str(), MainPath.c_str());
			return false;
		}

		if (!SFileWriteFile(hFile, (void*)fileData, dwFileSize, MPQ_COMPRESSION_ZLIB)) {
			SPDLOG_ERROR("({}) Failed to write {} bytes to {} in archive {}", GetLastError(), dwFileSize, path.c_str(), MainPath.c_str());
			if (!SFileCloseFile(hFile)) {
				SPDLOG_ERROR("({}) Failed to close file {} after write failure in archive {}", GetLastError(), path.c_str(), MainPath.c_str());
			}
			return false;
		}

		if (!SFileFinishFile(hFile)) {
			SPDLOG_ERROR("({}) Failed to finish file {} in archive {}", GetLastError(), path.c_str(), MainPath.c_str());
			if (!SFileCloseFile(hFile)) {
				SPDLOG_ERROR("({}) Failed to close file {} after finish failure in archive {}", GetLastError(), path.c_str(), MainPath.c_str());
			}
			return false;
		}
		// SFileFinishFile already frees the handle, so no need to close it again.

		addedFiles.push_back(path);
		hashes[CRC64(path.c_str())] = path;

		return true;
	}

	bool Archive::RemoveFile(const std::string& path) {
		// TODO: Notify the resource manager and child Files

		if (!SFileRemoveFile(mainMPQ, path.c_str(), 0)) {
			SPDLOG_ERROR("({}) Failed to remove file {} in archive {}", GetLastError(), path.c_str(), MainPath.c_str());
			return false;
		}

		return true;
	}

	bool Archive::RenameFile(const std::string& oldPath, const std::string& newPath) {
		// TODO: Notify the resource manager and child Files

		if (!SFileRenameFile(mainMPQ, oldPath.c_str(), newPath.c_str())) {
			SPDLOG_ERROR("({}) Failed to rename file {} to {} in archive {}", GetLastError(), oldPath.c_str(), newPath.c_str(), MainPath.c_str());
			return false;
		}

		return true;
	}

	std::vector<SFILE_FIND_DATA> Archive::ListFiles(const std::string& searchMask) const {
		auto fileList = std::vector<SFILE_FIND_DATA>();
		SFILE_FIND_DATA findContext;
		HANDLE hFind;


		hFind = SFileFindFirstFile(mainMPQ, searchMask.c_str(), &findContext, nullptr);
		//if (hFind && GetLastError() != ERROR_NO_MORE_FILES) {
		if (hFind != nullptr) {
			fileList.push_back(findContext);

			bool fileFound;
			do {
				fileFound = SFileFindNextFile(hFind, &findContext);

				if (fileFound) {
					fileList.push_back(findContext);
				}
				else if (!fileFound && GetLastError() != ERROR_NO_MORE_FILES)
				//else if (!fileFound)
				{
					SPDLOG_ERROR("({}), Failed to search with mask {} in archive {}", GetLastError(), searchMask.c_str(), MainPath.c_str());
					if (!SListFileFindClose(hFind)) {
						SPDLOG_ERROR("({}) Failed to close file search {} after failure in archive {}", GetLastError(), searchMask.c_str(), MainPath.c_str());
					}
					return fileList;
				}
			} while (fileFound);
		}
		else if (GetLastError() != ERROR_NO_MORE_FILES) {
			SPDLOG_ERROR("({}), Failed to search with mask {} in archive {}", GetLastError(), searchMask.c_str(), MainPath.c_str());
			return fileList;
		}

		if (hFind != nullptr)
		{
			if (!SFileFindClose(hFind)) {
				SPDLOG_ERROR("({}) Failed to close file search {} in archive {}", GetLastError(), searchMask.c_str(), MainPath.c_str());
			}
		}

		return fileList;
	}

	bool Archive::HasFile(const std::string& filename) const {
		bool result = false;
		auto start = std::chrono::steady_clock::now();

		auto lst = ListFiles(filename);

		for (const auto& item : lst) {
			if (item.cFileName == filename) {
				result = true;
				break;
			}
		}

		auto end = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		return result;
	}

	const std::string* Archive::HashToString(uint64_t hash) const {
		auto it = hashes.find(hash);
		return it != hashes.end() ? &it->second : nullptr;
	}

	bool Archive::Load(bool enableWriting, bool genCRCMap) {
		return LoadMainMPQ(enableWriting, genCRCMap) && LoadPatchMPQs();
	}

	bool Archive::Unload()
	{
		bool success = true;
		for (const auto& mpqHandle : mpqHandles) {
			if (!SFileCloseArchive(mpqHandle.second)) {
				SPDLOG_ERROR("({}) Failed to close mpq {}", GetLastError(), mpqHandle.first.c_str());
				success = false;
			}
		}

		mainMPQ = nullptr;

		return success;
	}

	bool Archive::LoadPatchMPQs() {
		// OTRTODO: We also want to periodically scan the patch directories for new MPQs. When new MPQs are found we will load the contents to fileCache and then copy over to gameResourceAddresses
		if (PatchesPath.length() > 0) {
			if (std::filesystem::is_directory(PatchesPath)) {
				for (const auto& p : std::filesystem::recursive_directory_iterator(PatchesPath)) {
					if (StringHelper::IEquals(p.path().extension().string(), ".otr") || StringHelper::IEquals(p.path().extension().string(), ".mpq")) {
						SPDLOG_ERROR("Reading {} mpq patch", p.path().string().c_str());
						if (!LoadPatchMPQ(p.path().string())) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}

    void Archive::GenerateCRCMap() {
        auto listFile = LoadFile("(listfile)", false);

        std::vector<std::string> lines = StringHelper::Split(std::string(listFile->buffer.get(), listFile->dwBufferSize), "\n");

        for (size_t i = 0; i < lines.size(); i++)
        {
            std::string line = StringHelper::Replace(StringHelper::Strip(lines[i], "\r"), "/", "\\");
            std::string line2 = StringHelper::Replace(line, "\\", "/");

            uint64_t hash = CRC64(StringHelper::Replace(line, "/", "\\").c_str());
            uint64_t hash2 = CRC64(StringHelper::Replace(line, "\\", "/").c_str());
            hashes[hash] = line;
            hashes[hash2] = line2;
        }
    }

    bool Archive::PushGameVersion(HANDLE mpqHandle) {
        auto t = LoadFileFromHandle("version", false, nullptr, mpqHandle);
        if (!t->bHasLoadError)
        {
            BinaryReader *reader = new BinaryReader(t->buffer.get(), t->dwBufferSize);
            Ship::Endianness endianness = (Ship::Endianness)reader->ReadUByte();
            reader->SetEndianness(endianness);
            uint32_t version = reader->ReadUInt32();
            if (ValidHashes.empty() || ValidHashes.contains(version)) {
                gameVersions.push_back(version);
                return true;
            }
        }
        return false;
    }

	bool Archive::LoadMainMPQ(bool enableWriting, bool genCRCMap) {
        HANDLE mpqHandle = NULL;
        if (OTRFiles.empty()) {
            if (MainPath.length() > 0) {
                if (std::filesystem::is_directory(MainPath)) {
                    for (const auto &p : std::filesystem::recursive_directory_iterator(MainPath)) {
                        if (StringHelper::IEquals(p.path().extension().string(), ".otr")) {
                            SPDLOG_ERROR("Reading {} mpq", p.path().string().c_str());
                            OTRFiles.push_back(p.path().string());
                        }
                    }
                } else if (std::filesystem::is_regular_file(MainPath)) {
                    OTRFiles.push_back(MainPath);
                } else {
                    SPDLOG_ERROR("The directory {} does not exist", MainPath.c_str());
                    return false;
                }
            } else {
                SPDLOG_ERROR("No OTR file list or Main Path provided.");
                return false;
            }
            if (OTRFiles.empty()) {
                SPDLOG_ERROR("No OTR files present in {}", MainPath.c_str());
                return false;
            }
        }
        bool baseLoaded = false;
        int i = 0;
        while (!baseLoaded && i < OTRFiles.size()) {
#ifdef _WIN32
            std::wstring wfullPath = std::filesystem::absolute(OTRFiles[i]).wstring();
#endif
#if defined(__SWITCH__)
            std::string fullPath = OTRFiles[0];
#else  
            std::string fullPath = std::filesystem::absolute(OTRFiles[i]).string();
#endif
#ifdef _WIN32
            if (SFileOpenArchive(wfullPath.c_str(), 0, enableWriting ? 0 : MPQ_OPEN_READ_ONLY, &mpqHandle))
            {
#else
            if (SFileOpenArchive(fullPath.c_str(), 0, enableWriting ? 0 : MPQ_OPEN_READ_ONLY, &mpqHandle))
            {
#endif
                SPDLOG_INFO("Opened mpq file {}.", fullPath.c_str());
                mainMPQ = mpqHandle;
                if (!PushGameVersion()) {
                    SPDLOG_WARN("Attempted to load invalid OTR file {}", OTRFiles[i].c_str());
                    SFileCloseArchive(mpqHandle);
                    mainMPQ = nullptr;
                } else {
                    mpqHandles[fullPath] = mpqHandle;
                    if (genCRCMap)
                    {
                        GenerateCRCMap();
                    }
                    baseLoaded = true;
                }
            }
            i++;
        }
        // If we exited the above loop without setting baseLoaded to true, then we've
        // attemtped to load all the OTRs available to us.
        if (!baseLoaded) {
            SPDLOG_ERROR("No valid OTR file was provided.");
            return false;
        }
        for (int j = i; j < OTRFiles.size(); j++) {
#ifdef _WIN32
            std::wstring wfullPath = std::filesystem::absolute(OTRFiles[i]).wstring();
#endif
#if defined(__SWITCH__)
            std::string fullPath = OTRFiles[i];
#else
            std::string fullPath = std::filesystem::absolute(OTRFiles[i]).string();
#endif
            if (LoadPatchMPQ(fullPath, true))
            {
                SPDLOG_INFO("({}) Patched in mpq file.", fullPath.c_str());
            }
            if (genCRCMap)
            {
                GenerateCRCMap();
            }
        }
        
		return true;
	}

	bool Archive::LoadPatchMPQ(const std::string& path, bool validateVersion) {
		HANDLE patchHandle = NULL;
#if defined(__SWITCH__)
		std::string fullPath = path;
#else
		std::string fullPath = std::filesystem::absolute(path).string();
#endif
		if (mpqHandles.contains(fullPath)) {
			return true;
		}

		std::wstring wPath = std::filesystem::absolute(path).wstring();

#ifdef _WIN32
		if (!SFileOpenArchive(wPath.c_str(), 0, MPQ_OPEN_READ_ONLY, &patchHandle)) {
#else
		if (!SFileOpenArchive(fullPath.c_str(), 0, MPQ_OPEN_READ_ONLY, &patchHandle)) {
#endif
			SPDLOG_ERROR("({}) Failed to open patch mpq file {} while applying to {}.", GetLastError(), path.c_str(), MainPath.c_str());
			return false;
		} else {
            // We don't always want to validate the "version" file, only when we're loading standalone OTRs as patches
            // i.e. Ocarina of Time along with Master Quest.
            if (validateVersion) {
                if (!PushGameVersion(patchHandle)) {
                    SPDLOG_WARN("({}) Invalid MQP file.", path.c_str());
                    return false;
                }
            }
        }
#ifdef _WIN32
		if (!SFileOpenPatchArchive(mainMPQ, wPath.c_str(), "", 0)) {
#else
		if (!SFileOpenPatchArchive(mainMPQ, fullPath.c_str(), "", 0)) {
#endif
			SPDLOG_ERROR("({}) Failed to apply patch mpq file {} to main mpq {}.", GetLastError(), path.c_str(), MainPath.c_str());
			return false;
		}

		mpqHandles[fullPath] = patchHandle;

		return true;
	}
}
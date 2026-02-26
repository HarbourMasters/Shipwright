#include "ExternalModPackage.h"
#include <zip.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace SOH {

namespace {

std::string NormalizeZipEntryPath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    while (!path.empty() && path.front() == '/') {
        path.erase(path.begin());
    }
    return path;
}

bool IsPathWithinBase(const std::filesystem::path& basePath, const std::filesystem::path& candidatePath) {
    try {
        const auto canonicalBase = std::filesystem::weakly_canonical(basePath);
        const auto canonicalCandidate = std::filesystem::weakly_canonical(candidatePath);
        const auto mismatch = std::mismatch(canonicalBase.begin(), canonicalBase.end(), canonicalCandidate.begin(),
                                            canonicalCandidate.end());
        return mismatch.first == canonicalBase.end();
    } catch (...) {
        return false;
    }
}

} // namespace

bool ExternalModPackageIO::ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                                     std::string& outError) {
    return ReadFileFromDirectory(dirPath / "mod.json", 256 * 1024, outContent, outError);
}

bool ExternalModPackageIO::ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent,
                                               std::string& outError) {
    std::vector<char> bytes;
    if (!ReadFileFromZip(zipPath, "mod.json", 256 * 1024, bytes, outError)) {
        return false;
    }
    outContent.assign(bytes.begin(), bytes.end());
    return true;
}

bool ExternalModPackageIO::ReadFileFromDirectory(const std::filesystem::path& filePath, uint64_t maxBytes,
                                                 std::string& outContent, std::string& outError) {
    if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) {
        outError = "File not found: " + filePath.generic_string();
        return false;
    }
    if (std::filesystem::file_size(filePath) > maxBytes) {
        outError = "File too large: " + filePath.generic_string();
        return false;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        outError = "Unable to open file: " + filePath.generic_string();
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    outContent = ss.str();
    return true;
}

bool ExternalModPackageIO::ReadFileFromZip(const std::filesystem::path& zipPath,
                                           const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                           std::vector<char>& outBytes, std::string& outError) {
    int err = 0;
    zip_t* archive = zip_open(zipPath.string().c_str(), ZIP_RDONLY, &err);
    if (archive == nullptr) {
        outError = "Unable to open zip archive: " + zipPath.generic_string();
        return false;
    }

    std::string zipEntry = packageRelativePath.generic_string();
    std::string zipEntryBackslash = zipEntry;
    std::replace(zipEntryBackslash.begin(), zipEntryBackslash.end(), '/', '\\');

    zip_stat_t stat;
    zip_stat_init(&stat);
    if (zip_stat(archive, zipEntry.c_str(), ZIP_FL_ENC_GUESS, &stat) != 0) {
        if (zipEntryBackslash == zipEntry || zip_stat(archive, zipEntryBackslash.c_str(), ZIP_FL_ENC_GUESS, &stat) != 0) {
            outError = "Zip entry not found: " + zipEntry;
            zip_close(archive);
            return false;
        }
        zipEntry = zipEntryBackslash;
    }
    if (stat.size > maxBytes) {
        outError = "Zip entry too large: " + zipEntry;
        zip_close(archive);
        return false;
    }

    outBytes.clear();
    outBytes.resize(static_cast<size_t>(stat.size));

    if (stat.size > 0) {
        zip_file_t* file = zip_fopen(archive, zipEntry.c_str(), ZIP_FL_ENC_GUESS);
        if (file == nullptr) {
            outError = "Unable to open zip entry: " + zipEntry;
            zip_close(archive);
            return false;
        }

        uint64_t totalRead = 0;
        while (totalRead < stat.size) {
            const auto toRead = static_cast<zip_uint64_t>(stat.size - totalRead);
            const auto readCount = zip_fread(file, outBytes.data() + totalRead, toRead);
            if (readCount < 0) {
                outError = "Failed reading zip entry: " + zipEntry;
                zip_fclose(file);
                zip_close(archive);
                return false;
            }
            if (readCount == 0) {
                break;
            }
            totalRead += static_cast<uint64_t>(readCount);
        }
        zip_fclose(file);
        outBytes.resize(static_cast<size_t>(totalRead));
    }

    zip_close(archive);
    return true;
}

bool ExternalModPackageIO::ReadFileFromPackage(const ExternalModPackage& package,
                                               const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                               std::string& outContent, std::string& outError) {
    if (package.isZip) {
        std::vector<char> bytes;
        if (!ReadFileFromZip(package.sourcePath, packageRelativePath, maxBytes, bytes, outError)) {
            return false;
        }
        outContent.assign(bytes.begin(), bytes.end());
        return true;
    }
    return ReadFileFromDirectory(package.sourcePath / packageRelativePath, maxBytes, outContent, outError);
}

bool ExternalModPackageIO::ReadBinaryFromPackage(const ExternalModPackage& package,
                                                 const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                                 std::vector<uint8_t>& outBytes, std::string& outError) {
    outBytes.clear();

    if (package.isZip) {
        std::vector<char> bytes;
        if (!ReadFileFromZip(package.sourcePath, packageRelativePath, maxBytes, bytes, outError)) {
            return false;
        }
        outBytes.assign(bytes.begin(), bytes.end());
        return true;
    }

    if (!std::filesystem::exists(package.sourcePath / packageRelativePath) ||
        !std::filesystem::is_regular_file(package.sourcePath / packageRelativePath)) {
        outError = "File not found: " + (package.sourcePath / packageRelativePath).generic_string();
        return false;
    }
    if (std::filesystem::file_size(package.sourcePath / packageRelativePath) > maxBytes) {
        outError = "File too large: " + (package.sourcePath / packageRelativePath).generic_string();
        return false;
    }

    std::ifstream file(package.sourcePath / packageRelativePath, std::ios::binary);
    if (!file.is_open()) {
        outError = "Unable to open file: " + (package.sourcePath / packageRelativePath).generic_string();
        return false;
    }

    file.seekg(0, std::ios::end);
    const std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > 0) {
        outBytes.resize(static_cast<size_t>(size));
        if (!file.read(reinterpret_cast<char*>(outBytes.data()), size)) {
            outError = "Failed to read file: " + (package.sourcePath / packageRelativePath).generic_string();
            return false;
        }
    }
    return true;
}

bool ExternalModPackageIO::IsSafePackageRelativePath(const std::string& pathValue,
                                                     std::filesystem::path& outNormalizedPath, std::string& outError) {
    if (pathValue.empty()) {
        outError = "Path cannot be empty";
        return false;
    }

    std::filesystem::path path(pathValue);
    if (path.is_absolute()) {
        outError = "Path must be relative";
        return false;
    }

    const std::filesystem::path rootPath = std::filesystem::path("root");
    const std::filesystem::path combinedPath = rootPath / path;

    if (!IsPathWithinBase(rootPath, combinedPath)) {
        outError = "Path traverses outside package root";
        return false;
    }

    outNormalizedPath = path.lexically_normal();
    return true;
}

} // namespace SOH

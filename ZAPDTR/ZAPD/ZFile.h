#pragma once

#include <string>
#include <vector>

#include "ZSymbol.h"
#include "ZTexture.h"
#include "tinyxml2.h"

enum class ZFileMode
{
	BuildTexture,
	BuildOverlay,
	BuildBlob,
	BuildSourceFile,
	BuildBackground,
	Extract,
	ExternalFile,
	Invalid,
	Custom = 1000,  // Used for exporter file modes
};

enum class ZGame
{
	OOT_RETAIL,
	OOT_SW97,
	MM_RETAIL
};

class ZFile
{
public:
	std::map<offset_t, Declaration*> declarations;
	std::string defines;
	std::vector<ZResource*> resources;

	// Default to using virtual addresses
	uint32_t segment = 0x80;
	uint32_t baseAddress, rangeStart, rangeEnd;
	bool isExternalFile = false;

	ZFile(const fs::path& nOutPath, const std::string& nName);
	ZFile(ZFileMode nMode, tinyxml2::XMLElement* reader, const fs::path& nBasePath,
	      const fs::path& nOutPath, const std::string& filename, const fs::path& nXmlFilePath);
	~ZFile();

	std::string GetName() const;
	std::string GetOutName() const;
	ZFileMode GetMode() const;
	const fs::path& GetXmlFilePath() const;
	const std::vector<uint8_t>& GetRawData() const;
	void ExtractResources();
	void BuildSourceFile();
	void AddResource(ZResource* res);
	ZResource* FindResource(uint32_t rawDataIndex);
	std::vector<ZResource*> GetResourcesOfType(ZResourceType resType);

	Declaration* AddDeclaration(offset_t address, DeclarationAlignment alignment, size_t size,
	                            const std::string& varType, const std::string& varName,
	                            const std::string& body);

	Declaration* AddDeclarationArray(offset_t address, DeclarationAlignment alignment, size_t size,
	                                 const std::string& varType, const std::string& varName,
	                                 size_t arrayItemCnt, const std::string& body);
	Declaration* AddDeclarationArray(offset_t address, DeclarationAlignment alignment, size_t size,
	                                 const std::string& varType, const std::string& varName,
	                                 const std::string& arrayItemCntStr, const std::string& body);

	Declaration* AddDeclarationPlaceholder(offset_t address, const std::string& varName);

	Declaration* AddDeclarationInclude(offset_t address, const std::string& includePath,
	                                   size_t size, const std::string& varType,
	                                   const std::string& varName);
	Declaration* AddDeclarationIncludeArray(offset_t address, std::string& includePath, size_t size,
	                                        const std::string& varType, const std::string& varName,
	                                        size_t arrayItemCnt);

	bool GetDeclarationPtrName(segptr_t segAddress, const std::string& expectedType,
	                           std::string& declName) const;
	bool GetDeclarationArrayIndexedName(segptr_t segAddress, size_t elementSize,
	                                    const std::string& expectedType,
	                                    std::string& declName) const;

	Declaration* GetDeclaration(uint32_t address) const;
	Declaration* GetDeclarationRanged(uint32_t address) const;
	bool HasDeclaration(uint32_t address);

	std::string GetHeaderInclude() const;
	std::string GetZRoomHeaderInclude() const;
	std::string GetExternalFileHeaderInclude() const;

	void GeneratePlaceholderDeclarations();

	void AddTextureResource(uint32_t offset, ZTexture* tex);
	ZTexture* GetTextureResource(uint32_t offset) const;

	void AddSymbolResource(uint32_t offset, ZSymbol* sym);
	ZSymbol* GetSymbolResource(uint32_t offset) const;
	ZSymbol* GetSymbolResourceRanged(uint32_t offset) const;

	fs::path GetSourceOutputFolderPath() const;

	bool IsOffsetInFileRange(uint32_t offset) const;
	bool IsSegmentedInFilespaceRange(segptr_t segAddress) const;

	static std::map<std::string, ZResourceFactoryFunc*>* GetNodeMap();
	static void RegisterNode(std::string nodeName, ZResourceFactoryFunc* nodeFunc);

protected:
	std::vector<uint8_t> rawData;
	std::string name;
	fs::path outName = "";
	fs::path basePath;
	fs::path outputPath;
	fs::path xmlFilePath;

	// Keep track of every texture of this ZFile.
	// The pointers declared here are "borrowed" (somebody else is the owner),
	// so ZFile shouldn't delete/free those textures.
	std::map<uint32_t, ZTexture*> texturesResources;
	std::map<uint32_t, ZSymbol*> symbolResources;
	ZFileMode mode = ZFileMode::Invalid;

	ZFile();
	void ParseXML(tinyxml2::XMLElement* reader, const std::string& filename);
	void DeclareResourceSubReferences();
	void GenerateSourceFiles();
	void GenerateSourceHeaderFiles();
	bool AddDeclarationChecks(uint32_t address, const std::string& varName);
	std::string ProcessDeclarations();
	void ProcessDeclarationText(Declaration* decl);
	std::string ProcessExterns();

	std::string ProcessTextureIntersections(const std::string& prefix);
	void HandleUnaccountedData();
	bool HandleUnaccountedAddress(offset_t currentAddress, offset_t lastAddr, uint32_t& lastSize);
};

#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include "Declaration.h"
#include "Utils/BinaryWriter.h"
#include "Utils/Directory.h"
#include "tinyxml2.h"

#define SEGMENT_SCENE 2
#define SEGMENT_ROOM 3
#define SEGMENT_KEEP 4
#define SEGMENT_FIELDDANGEON_KEEP 5
#define SEGMENT_OBJECT 6
#define SEGMENT_LINKANIMETION 7

#define GETSEGOFFSET(x) (x & 0x00FFFFFF)
#define GETSEGNUM(x) ((x >> 24) & 0xFF)

class ZFile;

enum class ZResourceType
{
	Error,
	Animation,
	Array,
	AltHeader,
	Background,
	Blob,
	CollisionHeader,
	Cutscene,
	DisplayList,
	Limb,
	LimbTable,
	Mtx,
	Path,
	PlayerAnimationData,
	Room,
	RoomCommand,
	Scalar,
	Scene,
	Skeleton,
	String,
	Symbol,
	Texture,
	TextureAnimation,
	TextureAnimationParams,
	Vector,
	Vertex,
	Text,
	Audio
};

class ResourceAttribute
{
public:
	std::string key;
	std::string value;
	bool isRequired = false;
	bool wasSet = false;
};

class ZResource
{
public:
	ZFile* parent;
	bool outputDeclaration = true;
	uint32_t hash = 0;
	bool genOTRDef = false;

	/**
	 * Constructor.
	 * Child classes should not declare any other constructor besides this one
	 */
	ZResource(ZFile* nParent);
	virtual ~ZResource() = default;

	// Parsing from File
	virtual void ExtractFromXML(tinyxml2::XMLElement* reader, offset_t nRawDataIndex);
	virtual void ExtractFromFile(offset_t nRawDataIndex);

	// Misc
	/**
	 * Parses additional attributes of the XML node.
	 * Extra attritbutes have to be registered using `RegisterRequiredAttribute` or
	 * `RegisterOptionalAttribute` in the constructor of the ZResource
	 */
	virtual void ParseXML(tinyxml2::XMLElement* reader);
	/**
	 * Extracts data from the binary file
	 */
	virtual void ParseRawData();
	/**
	 * Declares any data pointed by this resource that has not been declared already.
	 * For example, a Vtx referenced by a Dlist should be declared here if it wasn't
	 * declared previously by something else
	 */
	virtual void DeclareReferences(const std::string& prefix);
	virtual void ParseRawDataLate();
	virtual void DeclareReferencesLate(const std::string& prefix);

	/**
	 * Adds this resource as a Declaration of its parent ZFile
	 */
	virtual Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr);
	/**
	 * Returns the body of the variable of the extracted resource, without any side-effect
	 */
	[[nodiscard]] virtual std::string GetBodySourceCode() const;
	/**
	 * Creates an automatically generated variable name for the current resource
	 */
	[[nodiscard]] virtual std::string GetDefaultName(const std::string& prefix) const;

	virtual void GetSourceOutputCode(const std::string& prefix);
	virtual std::string GetSourceOutputHeader(const std::string& prefix, std::set<std::string> *nameSet);
	virtual void CalcHash();
	/**
	 * Exports the resource to binary format
	 */
	virtual void Save(const fs::path& outFolder);

	// Properties
	/**
	 * Returns true if the resource will be externalized, and included back to the C file using
	 * `#include`s
	 */
	virtual bool IsExternalResource() const;
	/**
	 * Can this type be wrapped in an <Array> node?
	 */
	virtual bool DoesSupportArray() const;
	/**
	 * The type of the resource as a C struct
	 */
	[[nodiscard]] virtual std::string GetSourceTypeName() const = 0;
	/**
	 * The type in the ZResource enum
	 */
	[[nodiscard]] virtual ZResourceType GetResourceType() const = 0;
	/**
	 * The filename extension for assets extracted as standalone files
	 */
	[[nodiscard]] virtual std::string GetExternalExtension() const;

	// Getters/Setters
	[[nodiscard]] const std::string& GetName() const;
	void SetName(const std::string& nName);
	[[nodiscard]] const std::string& GetOutName() const;
	void SetOutName(const std::string& nName);
	[[nodiscard]] offset_t GetRawDataIndex() const;
	/**
	 * The size of the current struct being extracted, not counting data referenced by it
	 */
	[[nodiscard]] virtual size_t GetRawDataSize() const = 0;
	/**
	 * The alignment of the extracted struct
	 */
	[[nodiscard]] virtual DeclarationAlignment GetDeclarationAlignment() const;
	void SetInnerNode(bool inner);
	/**
	 * Returns `true` if this ZResource was declared using an XML node,
	 * `false` otherwise (for example, a Vtx extracted indirectly by a DList)
	 */
	[[nodiscard]] bool WasDeclaredInXml() const;
	[[nodiscard]] StaticConfig GetStaticConf() const;

protected:
	std::string name;
	std::string outName;
	offset_t rawDataIndex;
	std::string sourceOutput;

	// Inner is used mostly for <Array> nodes
	/**
	 * Is this resource an inner node of another resource?
	 * (namely inside an <Array>)
	 */
	bool isInner = false;
	/**
	 * Can this type have an inner node?
	 */
	bool canHaveInner = false;

	/**
	 * If set to true, create a reference for the asset in the file, but don't
	 * actually try to extract it from the file
	 */
	bool isCustomAsset;
	bool declaredInXml = false;
	StaticConfig staticConf = StaticConfig::Global;

	// Reading from this XMLs attributes should be performed in the overrided `ParseXML` method.
	std::map<std::string, ResourceAttribute> registeredAttributes;

	// XML attributes registers.
	// Registering XML attributes should be done in constructors.

	// The resource needs this attribute. If it is not provided, then the program will throw an
	// exception.
	void RegisterRequiredAttribute(const std::string& attr);
	// Optional attribute. The resource has to do manual checks and manual warnings. It may or may
	// not have a value.
	void RegisterOptionalAttribute(const std::string& attr, const std::string& defaultValue = "");
};

class ZResourceExporter
{
public:
	ZResourceExporter() = default;
	virtual ~ZResourceExporter() = default;

	virtual void Save(ZResource* res, const fs::path& outPath, BinaryWriter* writer) = 0;
};

offset_t Seg2Filespace(segptr_t segmentedAddress, uint32_t parentBaseAddress);

typedef ZResource*(ZResourceFactoryFunc)(ZFile* nParent);

#define REGISTER_ZFILENODE(nodeName, zResClass)                                                    \
	static ZResource* ZResourceFactory_##zResClass_##nodeName(ZFile* nParent)                      \
	{                                                                                              \
		return static_cast<ZResource*>(new zResClass(nParent));                                    \
	}                                                                                              \
                                                                                                   \
	class ZRes_##nodeName                                                                          \
	{                                                                                              \
	public:                                                                                        \
		ZRes_##nodeName()                                                                          \
		{                                                                                          \
			ZFile::RegisterNode(#nodeName, &ZResourceFactory_##zResClass_##nodeName);              \
		}                                                                                          \
	};                                                                                             \
	static ZRes_##nodeName inst_ZRes_##nodeName

#define REGISTER_EXPORTER(expFunc)                                                                 \
	class ZResExp_##expFunc                                                                        \
	{                                                                                              \
	public:                                                                                        \
		ZResExp_##expFunc() { expFunc(); }                                                         \
	};                                                                                             \
	static ZResExp_##expFunc inst_ZResExp_##expFunc

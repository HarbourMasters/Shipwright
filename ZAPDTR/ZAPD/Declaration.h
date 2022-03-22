#pragma once

#include <string>
#include <vector>

// TODO: should we drop the `_t` suffix because of UNIX compliance?
typedef uint32_t segptr_t;
typedef uint32_t offset_t;

#define SEGMENTED_NULL ((segptr_t)0)

enum class DeclarationAlignment
{
	Align4,
	Align8
};

enum class StaticConfig
{
	Off,
	Global,
	On
};

class Declaration
{
public:
	offset_t address;
	DeclarationAlignment alignment;
	size_t size;
	std::string preText;
	std::string text;
	std::string rightText;
	std::string postText;
	std::string preComment;
	std::string postComment;
	std::string varType;
	std::string varName;
	std::string includePath;

	bool isExternal = false;
	bool isArray = false;
	bool forceArrayCnt = false;
	size_t arrayItemCnt = 0;
	std::string arrayItemCntStr = "";
	std::vector<segptr_t> references;
	bool isUnaccounted = false;
	bool isPlaceholder = false;
	bool declaredInXml = false;
	StaticConfig staticConf = StaticConfig::Global;

	Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            const std::string& nText);
	Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            size_t nArrayItemCnt, const std::string& nText);
	Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            const std::string& nArrayItemCntStr, const std::string& nText);
	Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            size_t nArrayItemCnt, const std::string& nText, bool nIsExternal);

	Declaration(offset_t nAddress, const std::string& nIncludePath, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName);

	bool IsStatic() const;

	std::string GetNormalDeclarationStr() const;
	std::string GetExternalDeclarationStr() const;

	std::string GetExternStr() const;

	std::string GetStaticForwardDeclarationStr() const;

protected:
	Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
	            const std::string& nText);
};

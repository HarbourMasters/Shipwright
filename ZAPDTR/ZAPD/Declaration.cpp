#include "Declaration.h"

#include "Globals.h"
#include "ZVtx.h"
#include "Utils/StringHelper.h"

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         const std::string& nText)
{
	address = nAddress;
	alignment = nAlignment;
	size = nSize;
	text = nText;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         const std::string& nVarType, const std::string& nVarName, bool nIsArray,
                         const std::string& nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         const std::string& nVarType, const std::string& nVarName, bool nIsArray,
                         size_t nArrayItemCnt, const std::string& nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCnt = nArrayItemCnt;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         const std::string& nVarType, const std::string& nVarName, bool nIsArray,
                         const std::string& nArrayItemCntStr, const std::string& nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCntStr = nArrayItemCntStr;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         const std::string& nVarType, const std::string& nVarName, bool nIsArray,
                         size_t nArrayItemCnt, const std::string& nText, bool nIsExternal)
	: Declaration(nAddress, nAlignment, nSize, nVarType, nVarName, nIsArray, nArrayItemCnt, nText)
{
	isExternal = nIsExternal;
}

Declaration::Declaration(offset_t nAddress, const std::string& nIncludePath, size_t nSize,
                         const std::string& nVarType, const std::string& nVarName)
	: Declaration(nAddress, DeclarationAlignment::Align4, nSize, "")
{
	includePath = nIncludePath;
	varType = nVarType;
	varName = nVarName;
}

Declaration::~Declaration()
{
	//for (auto item : vertexHack)
		//delete item;
}

bool Declaration::IsStatic() const
{
	switch (staticConf)
	{
	case StaticConfig::Off:
		return false;

	case StaticConfig::Global:
		return Globals::Instance->forceStatic;

	case StaticConfig::On:
		return true;
	}

	return false;
}

std::string Declaration::GetNormalDeclarationStr() const
{
	std::string output;

	if (preText != "")
		output += preText + "\n";

	if (IsStatic())
	{
		output += "static ";
	}

	if (isArray)
	{
		if (arrayItemCntStr != "" && (IsStatic() || forceArrayCnt))
		{
			output += StringHelper::Sprintf("%s %s[%s];\n", varType.c_str(), varName.c_str(),
			                                arrayItemCntStr.c_str());
		}
		else if (arrayItemCnt != 0 && (IsStatic() || forceArrayCnt))
		{
			output += StringHelper::Sprintf("%s %s[%i] = {\n", varType.c_str(), varName.c_str(),
			                                arrayItemCnt);
		}
		else
		{
			output += StringHelper::Sprintf("%s %s[] = {\n", varType.c_str(), varName.c_str());
		}

		output += text + "\n";
	}
	else
	{
		output += StringHelper::Sprintf("%s %s = { ", varType.c_str(), varName.c_str());
		output += text;
	}

	if (output.back() == '\n')
		output += "};";
	else
		output += " };";

	if (rightText != "")
		output += " " + rightText + "";

	output += "\n";

	if (postText != "")
		output += postText + "\n";

	output += "\n";

	return output;
}

std::string Declaration::GetExternalDeclarationStr() const
{
	std::string output;

	if (preText != "")
		output += preText + "\n";

	if (IsStatic())
	{
		output += "static ";
	}

	if (arrayItemCntStr != "" && (IsStatic() || forceArrayCnt))
		output += StringHelper::Sprintf("%s %s[%s] = ", varType.c_str(), varName.c_str(),
		                                arrayItemCntStr.c_str());
	else if (arrayItemCnt != 0 && (IsStatic() || forceArrayCnt))
		output +=
			StringHelper::Sprintf("%s %s[%i] = ", varType.c_str(), varName.c_str(), arrayItemCnt);
	else
		output += StringHelper::Sprintf("%s %s[] = ", varType.c_str(), varName.c_str());

	output += StringHelper::Sprintf("{\n#include \"%s\"\n};", includePath.c_str());

	if (rightText != "")
		output += " " + rightText + "";

	output += "\n";

	if (postText != "")
		output += postText + "\n";

	output += "\n";

	return output;
}

std::string Declaration::GetExternStr() const
{
	if (IsStatic() || varType == "")
	{
		return "";
	}

	if (Globals::Instance->otrMode) /* && (varType == "Gfx" || varType == "u64" || varType == "AnimationHeader" || varType == "LinkAnimationHeader" ||
		varType == "StandardLimb" || varType == "JointIndex" || varType == "Vtx" || varType == "FlexSkeletonHeader" || varType == "SkeletonHeader") ||
		varType == "CollisionHeader") */
		return "";

	if (isArray)
	{
		if (arrayItemCntStr != "" && (IsStatic() || forceArrayCnt))
		{
			return StringHelper::Sprintf("extern %s %s[%s];\n", varType.c_str(), varName.c_str(),
			                             arrayItemCntStr.c_str());
		}
		else if (arrayItemCnt != 0 && (IsStatic() || forceArrayCnt))
		{
			return StringHelper::Sprintf("extern %s %s[%i];\n", varType.c_str(), varName.c_str(),
			                             arrayItemCnt);
		}
		else
			return StringHelper::Sprintf("extern %s %s[];\n", varType.c_str(), varName.c_str());
	}

	return StringHelper::Sprintf("extern %s %s;\n", varType.c_str(), varName.c_str());
}

std::string Declaration::GetStaticForwardDeclarationStr() const
{
	if (!IsStatic() || isUnaccounted)
		return "";

	if (isArray)
	{
		if (arrayItemCntStr == "" && arrayItemCnt == 0)
		{
			// Forward declaring static arrays without specifying the size is not allowed.
			return "";
		}

		if (arrayItemCntStr != "")
		{
			return StringHelper::Sprintf("static %s %s[%s];\n", varType.c_str(), varName.c_str(),
			                             arrayItemCntStr.c_str());
		}
		else
		{
			return StringHelper::Sprintf("static %s %s[%i];\n", varType.c_str(), varName.c_str(),
			                             arrayItemCnt);
		}
	}

	return StringHelper::Sprintf("static %s %s;\n", varType.c_str(), varName.c_str());
}

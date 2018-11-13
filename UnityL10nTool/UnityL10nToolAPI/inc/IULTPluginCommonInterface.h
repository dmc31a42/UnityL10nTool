#pragma once
#include <map>
#include <algorithm>
#include "json/json.h"

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"

#include "GeneralPurposeFunctions.h"

struct UnityL10nToolAPI;
struct FontAssetMap;

struct AssetMapOption {
public:
	enum Type
	{
		OPTION_TYPE_NONE = 0,
		OPTION_TYPE_WSTRING = 1,
		OPTION_TYPE_INT = 2,
		OPTION_TYPE_DOUBLE = 3,
		OPTION_TYPE_BOOL = 4
	};
	std::wstring OptionName;
	std::wstring OptionDescription;
	void* Value;
	void* ValueAsChild;
	Type type;
	Type typeAsChild;
	std::vector<AssetMapOption> nestedOptions;
	Json::Value ToJson() {
		Json::Value result;
		Json::Value null_value;
		result["OptionName"] = WideMultiStringConverter->to_bytes(this->OptionName);
		result["OptionDescription"] = WideMultiStringConverter->to_bytes(this->OptionDescription);
		switch (this->type) {
		case OPTION_TYPE_NONE:
			result["type"] = "NONE";
			result["Value"] = null_value;
			break;
		case OPTION_TYPE_WSTRING:
			result["type"] = "WSTRING";
			if (this->Value) {
				result["Value"] = WideMultiStringConverter->to_bytes(*(std::wstring*)this->Value);
			}
			else {
				result["Value"] = null_value;
			}
			break;
		case OPTION_TYPE_INT:
			result["type"] = "INT";
			if (this->Value) {
				result["Value"] = *(int*)this->Value;
			}
			else {
				result["Value"] = null_value;
			}
			break;
		case OPTION_TYPE_DOUBLE:
			result["type"] = "DOUBLE";
			if (this->Value) {
				result["Value"] = *(double*)this->Value;
			}
			else {
				result["Value"] = null_value;
			}
			break;
		case OPTION_TYPE_BOOL:
			result["type"] = "BOOL";
			if (this->Value) {
				result["Value"] = *(bool*)this->Value;
			}
			else {
				result["Value"] = null_value;
			}
			break;
		}
		switch (this->typeAsChild) {
		case OPTION_TYPE_NONE:
			result["typeAsChild"] = "NONE";
			result["ValueAsChild"] = null_value;
			break;
		case OPTION_TYPE_WSTRING:
			result["typeAsChild"] = "WSTRING";
			if (this->ValueAsChild) {
				result["ValueAsChild"] = WideMultiStringConverter->to_bytes(*(std::wstring*)this->ValueAsChild);
			}
			else {
				result["ValueAsChild"] = null_value;
			}
			break;
		case OPTION_TYPE_INT:
			result["typeAsChild"] = "INT";
			if (this->ValueAsChild) {
				result["ValueAsChild"] = *(int*)this->ValueAsChild;
			}
			else {
				result["ValueAsChild"] = null_value;
			}
			break;
		case OPTION_TYPE_DOUBLE:
			result["typeAsChild"] = "DOUBLE";
			if (this->ValueAsChild) {
				result["ValueAsChild"] = *(double*)this->ValueAsChild;
			}
			else {
				result["ValueAsChild"] = null_value;
			}
			break;
		case OPTION_TYPE_BOOL:
			result["typeAsChild"] = "BOOL";
			if (this->ValueAsChild) {
				result["ValueAsChild"] = *(bool*)this->ValueAsChild;
			}
			else {
				result["ValueAsChild"] = null_value;
			}
			break;
		}
		for (std::vector<AssetMapOption>::iterator iterator = this->nestedOptions.begin();
			iterator != this->nestedOptions.end(); iterator++) {
			//if (!(iterator->Value == NULL && iterator->ValueAsChild == NULL)) {
			if (!(iterator->Value == NULL)) {
				result["nestedOptions"].append(iterator->ToJson());
			}
		}
		return result;
	}
	AssetMapOption() {}
	AssetMapOption(std::wstring OptionName, std::wstring OptionDescription, void* Value, void* ValueAsChild, Type type, Type typeAsChild, std::vector<AssetMapOption> nestedOptions)
		:OptionName(OptionName), OptionDescription(OptionDescription), Value(Value), ValueAsChild(ValueAsChild), type(type), typeAsChild(typeAsChild), nestedOptions(nestedOptions) {}
	AssetMapOption(Json::Value json) {
		this->OptionName = WideMultiStringConverter->from_bytes(json["OptionName"].asString());
		this->OptionDescription = WideMultiStringConverter->from_bytes(json["OptionDescription"].asString());
		std::string tempType = json["type"].asString();
		if (tempType == "NONE") {
			this->type = OPTION_TYPE_NONE;
		}
		else if (tempType == "WSTRING") {
			this->type = OPTION_TYPE_WSTRING;
			if (!json["Value"].isNull()) {
				this->Value = new std::wstring(WideMultiStringConverter->from_bytes(json["Value"].asString()));
			}
		}
		else if (tempType == "INT") {
			this->type = OPTION_TYPE_INT;
			if (!json["Value"].isNull()) {
				this->Value = new int(json["Value"].asInt());
			}
		}
		else if (tempType == "DOUBLE") {
			this->type = OPTION_TYPE_DOUBLE;
			if (!json["Value"].isNull()) {
				this->Value = new double(json["Value"].asDouble());
			}
		}
		else if (tempType == "BOOL") {
			this->type = OPTION_TYPE_BOOL;
			if (!json["Value"].isNull()) {
				this->Value = new bool(json["Value"].asBool());
			}
		}
		else {
			throw new std::exception(("Unknown Type" + tempType).c_str());
		}
		std::string tempTypeAsChild = json["typeAsChild"].asString();
		if (tempTypeAsChild == "NONE") {
			this->typeAsChild = OPTION_TYPE_NONE;
		}
		else if (tempTypeAsChild == "WSTRING") {
			this->typeAsChild = OPTION_TYPE_WSTRING;
			if (!json["ValueAsChild"].isNull()) {
				this->ValueAsChild = new std::wstring(WideMultiStringConverter->from_bytes(json["ValueAsChild"].asString()));
			}
		}
		else if (tempTypeAsChild == "INT") {
			this->typeAsChild = OPTION_TYPE_INT;
			if (!json["ValueAsChild"].isNull()) {
				this->ValueAsChild = new int(json["ValueAsChild"].asInt());
			}
		}
		else if (tempTypeAsChild == "DOUBLE") {
			this->typeAsChild = OPTION_TYPE_DOUBLE;
			if (!json["ValueAsChild"].isNull()) {
				this->ValueAsChild = new double(json["ValueAsChild"].asDouble());
			}
		}
		else if (tempTypeAsChild == "BOOL") {
			this->typeAsChild = OPTION_TYPE_BOOL;
			if (!json["ValueAsChild"].isNull()) {
				this->ValueAsChild = new bool(json["ValueAsChild"].asBool());
			}
		}
		else {
			throw new std::exception(("Unknown Type" + tempType).c_str());
		}
		Json::Value nestedOptionsJson = json["nestedOptions"];
		for (Json::ArrayIndex i = 0; i < nestedOptionsJson.size(); i++) {
			this->nestedOptions.push_back(AssetMapOption(nestedOptionsJson[i]));
		}
	}
};

struct UnityL10nToolAPI {
	std::string version;
	std::string versionFirstTwoNumbers;
	ClassDatabaseFile* BasicClassDatabaseFile;
	ClassDatabaseFile* MonoClassDatabaseFile;
	const ResourceManagerFile* ResourceManagerFileGlobal;
	const AssetsFileTable* GlobalgamemanagersAssetsTable;
	std::vector<std::string>* AssetsFileNames;
	/* FindAFromB */
	const std::map <std::string, AssetsFile*>* FindAssetsFilesFromAssetsName;
	const std::map <std::string, AssetsFileTable*>* FindAssetsFileTablesFromAssetsName;
	const std::map <AssetsFileTable*, std::string>* FindAssetsNameFromAssetsFileTables;
	const std::map <INT32, UINT32>* FindBasicClassIndexFromClassID;
	const std::map <std::string, UINT32>* FindBasicClassIndexFromClassName;
	const std::map<std::pair<std::string, INT64>, std::string>* FindMonoClassNameFromAssetsNameANDPathId;
	const std::map<std::string, UINT32>* FindMonoClassIndexFromMonoClassName;
	const std::map<std::pair<INT32, INT64>, std::string>* FindContainerPathFromFileIDPathID;
	const std::map<std::string, std::pair<INT32, INT64>>* FindFileIDPathIDFromContainerPath;
	const std::map<std::string, INT32>* FindPathIDOfContainerPathFromAssetsName;
	const std::map<INT32, std::string>* FindAssetsNameFromPathIDOfContainerPath;

	
	void GetClassIdFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, int & classId, UINT16 & monoClassId);
	AssetTypeInstance * GetBasicAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx);
	AssetTypeInstance * GetDetailAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx);
	AssetTypeTemplateField * GetMonoAssetTypeTemplateFieldFromClassName(std::string MonoClassName);
	AssetsReplacer * makeAssetsReplacer(std::string assetsFileName, AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, AssetTypeInstance * assetTypeInstance, Json::Value modifyJson);
	AssetsReplacer * makeAssetsReplacer(std::string assetsFileName, AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, AssetTypeInstance * assetTypeInstance);
	std::string GetJsonKeyFromAssetTypeTemplateField(AssetTypeTemplateField * assetTypeTemplateField);
	std::string GetJsonKeyFromAssetTypeValueField(AssetTypeValueField * assetTypeValueField);
	std::string GetJsonFromAssetTypeValueFieldRecursive(AssetTypeValueField * field);
	std::string GetJsonFromAssetTypeValueField(AssetTypeValueField * field);
	AssetTypeValueField * GetAssetTypeValueFieldFromJsonRecursive(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	AssetTypeValueField * GetAssetTypeValueFieldFromJson(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	AssetTypeValueField * GetAssetTypeValueFieldArrayFromJson(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	bool ModifyAssetTypeValueFieldFromJSONRecursive(AssetTypeValueField * assetTypeValueField, Json::Value json);
	bool ModifyAssetTypeValueFieldFromJSON(AssetTypeValueField * assetTypeValueField, Json::Value json);
	std::string GetClassNameFromBaseAssetTypeValueField(AssetsFileTable* assetsFileTable, AssetTypeValueField * baseAssetTypeValueField);
	INT64 FindAssetIndexFromName(AssetsFileTable * assetsFileTable, std::string assetName);
	std::wstring GetAssetNameW(AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx);
	std::wstring GetAssetNameW(AssetsFile* assetsFile, AssetFileInfoEx* assetFileInfoEx);
};





inline void UnityL10nToolAPI::GetClassIdFromAssetFileInfoEx(AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, int& classId, UINT16& monoClassId) {
	if (assetsFileTable->getAssetsFile()->header.format <= 0x10) {
		classId = assetFileInfoEx->curFileType;
	}
	else {
		classId = assetsFileTable->getAssetsFile()->typeTree.pTypes_Unity5[assetFileInfoEx->curFileTypeOrIndex].classId;
		if (classId == 0x72) {
			monoClassId = (WORD)(0xFFFFFFFF - assetFileInfoEx->curFileType); // same as monoScriptIndex in AssetsReplacer
		}
	}
}

inline std::string UnityL10nToolAPI::GetClassNameFromBaseAssetTypeValueField(AssetsFileTable* assetsFileTable, AssetTypeValueField* baseAssetTypeValueField) {
	if (baseAssetTypeValueField) {
		std::string m_Name = baseAssetTypeValueField->Get("m_Name")->GetValue()->AsString();
		AssetTypeValueField* m_ScriptATVF = baseAssetTypeValueField->Get("m_Script");
		if (m_ScriptATVF) {
			int m_FileId = m_ScriptATVF->Get("m_FileID")->GetValue()->AsInt();
			unsigned __int64 m_PathID = m_ScriptATVF->Get("m_PathID")->GetValue()->AsUInt64();
			std::string assetsName;
			if (m_FileId == 0) {
				assetsName = FindAssetsNameFromAssetsFileTables->find(assetsFileTable)->second;
			}
			else {
				assetsName = std::string(assetsFileTable->getAssetsFile()->dependencies.pDependencies[m_FileId - 1].assetPath);
			}
			std::map<std::pair<std::string, INT64>, std::string>::const_iterator iterator = FindMonoClassNameFromAssetsNameANDPathId->find(std::pair<std::string, INT64>(assetsName, m_PathID));
			if (iterator != FindMonoClassNameFromAssetsNameANDPathId->end()) {
				return iterator->second;
			}
			else {
				return "";
			}
		}
		else {
			throw new std::exception("GetClassNameFromBaseAssetTypeValueField: m_ScriptATVF not exist");
		}
	}
	else {
		throw new std::exception("GetClassNameFromBaseAssetTypeValueField: baseAssetTypeValueField not exist");
	}
}

inline INT64 UnityL10nToolAPI::FindAssetIndexFromName(AssetsFileTable* assetsFileTable, std::string assetName) {
	char readName[100];
	AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
	for (unsigned int i = 0; i < assetsFileTable->assetFileInfoCount; i++) {
		AssetFileInfoEx *assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
		int classId;
		UINT16 monoClassId;
		GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
		if (classId == 0x72) {
			AssetTypeInstance* assetTypeValueField = GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
			AssetTypeValueField* pbase = assetTypeValueField->GetBaseField();
			if (pbase) {
				std::string m_Name = pbase->Get("m_Name")->GetValue()->AsString();
				if (assetName == m_Name) {
					assetTypeValueField->~AssetTypeInstance();
					return assetFileInfoEx->index;
				}
			}
			assetTypeValueField->~AssetTypeInstance();
		}
		else {
			assetFileInfoEx->ReadName(assetsFile, readName);
			if (assetName == readName) {
				return assetFileInfoEx->index;
			}
		}
	}
	return -1;
}

inline AssetTypeInstance* UnityL10nToolAPI::GetBasicAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx) {
	int classId;
	WORD monoClassId;
	GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
	AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
	baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID->find(classId)->second], (DWORD)0, false);
	AssetTypeInstance* baseAssetTypeInstance = new AssetTypeInstance(
		(DWORD)1,
		&baseAssetTypeTemplateField,
		assetFileInfoEx->curFileSize,
		assetsFileTable->getReader(),
		assetsFileTable->getAssetsFile()->header.endianness ? true : false,
		assetFileInfoEx->absolutePos);
	return baseAssetTypeInstance;
}

inline AssetTypeInstance* UnityL10nToolAPI::GetDetailAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx) {
	int classId;
	WORD monoClassId;
	GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
	AssetTypeInstance* baseAssetTypeInstance = GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
	if (classId == 0x72) {
		AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance->GetBaseField();
		std::string monoScriptFullName = GetClassNameFromBaseAssetTypeValueField(assetsFileTable, baseAssetTypeValueField);
		baseAssetTypeInstance->~AssetTypeInstance();
		AssetTypeTemplateField* baseMonoTypeTemplateField = GetMonoAssetTypeTemplateFieldFromClassName(monoScriptFullName);
		AssetTypeInstance* baseMonoTypeInstance = new AssetTypeInstance(
			(DWORD)1,
			&baseMonoTypeTemplateField,
			assetFileInfoEx->curFileSize,
			assetsFileTable->getReader(),
			assetsFileTable->getAssetsFile()->header.endianness ? true : false,
			assetFileInfoEx->absolutePos);
		return baseMonoTypeInstance;
	}
	else {
		return baseAssetTypeInstance;
	}
}

inline AssetTypeTemplateField* UnityL10nToolAPI::GetMonoAssetTypeTemplateFieldFromClassName(std::string MonoClassName) {
	std::map<std::string, UINT32>::const_iterator indexOfMonoclassIterator = FindMonoClassIndexFromMonoClassName->find(MonoClassName);
	if (indexOfMonoclassIterator == FindMonoClassIndexFromMonoClassName->end()) {
		return new AssetTypeTemplateField;
	}
	int indexOfMonoclass = indexOfMonoclassIterator->second;

	AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
	baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID->find(0x72)->second], (DWORD)0, false);
	AssetTypeTemplateField* baseMonoTypeTemplateField = new AssetTypeTemplateField;
	baseMonoTypeTemplateField->FromClassDatabase(MonoClassDatabaseFile, &MonoClassDatabaseFile->classes[indexOfMonoclass], (DWORD)0, true);
	int prevBaseAssetTypeTemplateFieldChildrenCount = baseAssetTypeTemplateField->childrenCount;
	int prevBaseMonoTypeTemplateFieldChildrenCount = baseMonoTypeTemplateField->childrenCount;
	baseAssetTypeTemplateField->AddChildren(prevBaseMonoTypeTemplateFieldChildrenCount);
	for (int i = 0; i < prevBaseMonoTypeTemplateFieldChildrenCount; i++) {
		baseAssetTypeTemplateField->children[prevBaseAssetTypeTemplateFieldChildrenCount + i] =
			baseMonoTypeTemplateField->children[i];
	}
	return baseAssetTypeTemplateField;
}

inline AssetsReplacer* UnityL10nToolAPI::makeAssetsReplacer(std::string assetsFileName, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance, Json::Value modifyJson) {
	INT64 PathId = assetFileInfoEx->index;
	int classId;
	WORD monoClassId;
	AssetTypeValueField* baseAssetTypeValueField = assetTypeInstance->GetBaseField();
	GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);

	ModifyAssetTypeValueFieldFromJSON(baseAssetTypeValueField, modifyJson);

	QWORD newByteSize = baseAssetTypeValueField->GetByteSize(0);
	void* newAssetBuffer = malloc((size_t)newByteSize);
	if (newAssetBuffer) {
		IAssetsWriter *pWriter = Create_AssetsWriterToMemory(newAssetBuffer, (size_t)newByteSize);
		if (pWriter) {
			newByteSize = baseAssetTypeValueField->Write(pWriter, 0, assetsFileTable->getAssetsFile()->header.endianness ? true : false);
			AssetsReplacer *pReplacer = MakeAssetModifierFromMemory(0, PathId, classId, monoClassId, newAssetBuffer, (size_t)newByteSize, free);
			if (pReplacer) {
				return pReplacer;
			}
		}
	}
	return NULL;
}

inline AssetsReplacer* UnityL10nToolAPI::makeAssetsReplacer(std::string assetsFileName, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance) {
	return makeAssetsReplacer(assetsFileName, assetsFileTable, assetFileInfoEx, assetTypeInstance, Json::Value());
}

inline std::string UnityL10nToolAPI::GetJsonKeyFromAssetTypeTemplateField(AssetTypeTemplateField* assetTypeTemplateField) {
	std::string align;
	if (assetTypeTemplateField->align || assetTypeTemplateField->valueType == ValueType_String) {
		align = "1";
	}
	else {
		align = "0";
	}
	std::string key = align + " " + std::string(assetTypeTemplateField->type) + " " + std::string(assetTypeTemplateField->name);
	return key;
}

inline std::string UnityL10nToolAPI::GetJsonKeyFromAssetTypeValueField(AssetTypeValueField* assetTypeValueField) {
	return GetJsonKeyFromAssetTypeTemplateField(assetTypeValueField->GetTemplateField());
}

inline std::string UnityL10nToolAPI::GetJsonFromAssetTypeValueFieldRecursive(AssetTypeValueField *field) {
	AssetTypeTemplateField* templateField = field->GetTemplateField();
	AssetTypeValueField** fieldChildren = field->GetChildrenList();
	DWORD childrenCount = field->GetChildrenCount();
	std::string str;
	if (templateField->isArray) {
		if (childrenCount == 0) {
			str = "[";
		}
		else {
			str = "[\r\n";
		}
	}
	else {
		str = "{\r\n";
	}

	for (DWORD i = 0; i < childrenCount; i++) {
		AssetTypeValueField* fieldChild = fieldChildren[i];
		AssetTypeTemplateField* templateFieldChild = fieldChild->GetTemplateField();
		std::string align;
		if (templateFieldChild->align || templateFieldChild->valueType == ValueType_String) {
			align = "1";
		}
		else {
			align = "0";
		}
		std::string key = align + " " + std::string(templateFieldChild->type) + " " + std::string(templateFieldChild->name);
		std::string value;
		switch (templateFieldChild->valueType) {
		case ValueType_None:
			if (templateFieldChild->isArray) {
				value = GetJsonFromAssetTypeValueFieldRecursive(fieldChild);
			}
			else {
				value = "\r\n" + GetJsonFromAssetTypeValueFieldRecursive(fieldChild);
			}
			break;
		case ValueType_Int8:
		case ValueType_Int16:
		case ValueType_Int32:
		case ValueType_Int64:
			value = std::to_string((long long)fieldChild->GetValue()->AsInt());
			break;
		case ValueType_UInt8:
		case ValueType_UInt16:
		case ValueType_UInt32:
		case ValueType_UInt64:
			value = std::to_string(fieldChild->GetValue()->AsUInt64());
			break;
		case ValueType_Float:
			value = std::to_string((long double)fieldChild->GetValue()->AsFloat());
			break;
		case ValueType_Double:
			value = std::to_string((long double)fieldChild->GetValue()->AsDouble());
			break;
		case ValueType_Bool:
			if (fieldChild->GetValue()->AsBool()) {
				value = "true";
			}
			else {
				value = "false";
			}
			break;
		case ValueType_String:
			value = "\"" + std::string(fieldChild->GetValue()->AsString()) + "\"";
			break;
		}
		if (templateField->isArray) {
			str += "    {\"" + key + "\": ";
			str += ReplaceAll(value, "\r\n", "\r\n    ");
			str += "}";
			if ((i + 1) < childrenCount) {
				str += ",";
				str += "\r\n";
			}
		}
		else {
			str += "    \"" + key + "\": ";
			str += ReplaceAll(value, "\r\n", "\r\n    ");
			if ((i + 1) < childrenCount) {
				str += ",";
				str += "\r\n";
			};
		}
	}
	if (templateField->isArray) {
		if (childrenCount == 0) {
			str += "]";
		}
		else {
			str += "\r\n]";
		}
	}
	else {
		str += "\r\n}";
	}
	return str;
}

inline std::string UnityL10nToolAPI::GetJsonFromAssetTypeValueField(AssetTypeValueField *field) {
	std::string str = "{\r\n";
	AssetTypeTemplateField* templateField = field->GetTemplateField();
	std::string key = std::string(templateField->align ? "1" : "0") + " " + std::string(templateField->type) + " " + std::string(templateField->name);
	str += "    \"" + key + "\": \r\n    ";
	std::string value = GetJsonFromAssetTypeValueFieldRecursive(field);
	str += ReplaceAll(value, "\r\n", "\r\n    ");
	str += "\r\n}";
	return str;
}

inline AssetTypeValueField* UnityL10nToolAPI::GetAssetTypeValueFieldFromJsonRecursive(AssetTypeTemplateField* assetTypeTemplateField, Json::Value json) {
	std::vector<AssetTypeValueField*>* assetTypeValueFieldArray = new std::vector<AssetTypeValueField*>();
	AssetTypeValue* assetTypeValue = new AssetTypeValue(ValueType_None, 0);
	AssetTypeValueField* assetTypeValueField = new AssetTypeValueField();
	std::string align;
	if (assetTypeTemplateField->align || assetTypeTemplateField->valueType == ValueType_String) {
		align = "1";
	}
	else {
		align = "0";
	}
	std::string key = align + " " + std::string(assetTypeTemplateField->type) + " " + std::string(assetTypeTemplateField->name);
	//Json::Value thisJson = json[key];
	Json::Value thisJson = json;
	// 이전코드가 잘못되 수정하는도중 임시로 재할당
	std::vector<std::string> testStrs1 = thisJson.getMemberNames();
	/*Json::Value thisJson = */
	for (unsigned int i = 0; i < assetTypeTemplateField->childrenCount; i++) {
		AssetTypeTemplateField* childAssetTypeTemplateField = &assetTypeTemplateField->children[i];
		std::string alignChild;
		if (childAssetTypeTemplateField->align || childAssetTypeTemplateField->valueType == ValueType_String) {
			alignChild = "1";
		}
		else {
			alignChild = "0";
		}
		std::string keyChild = alignChild + " " + std::string(childAssetTypeTemplateField->type) + " " + std::string(childAssetTypeTemplateField->name);
		//void* container;
		AssetTypeValue* childAssetTypeValue;
		AssetTypeValueField* childAssetTypeValueField = new AssetTypeValueField();
		AssetTypeByteArray* assetTypeByteArray;
		std::string* tempStr;

		//only test
		INT32 testInt = 0;
		switch (childAssetTypeTemplateField->valueType) {
		case ValueType_Int8:
			childAssetTypeValue = new AssetTypeValue(ValueType_Int8, new INT8((INT8)thisJson[keyChild].asInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_Int16:
			childAssetTypeValue = new AssetTypeValue(ValueType_Int16, new INT16((INT16)thisJson[keyChild].asInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_Int32:
			testInt = thisJson[keyChild].asInt();
			childAssetTypeValue = new AssetTypeValue(ValueType_Int32, new INT32((INT32)thisJson[keyChild].asInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_Int64:
			childAssetTypeValue = new AssetTypeValue(ValueType_Int64, new INT64((INT64)thisJson[keyChild].asInt64()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_UInt8:
			childAssetTypeValue = new AssetTypeValue(ValueType_UInt8, new UINT8((UINT8)thisJson[keyChild].asUInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_UInt16:
			childAssetTypeValue = new AssetTypeValue(ValueType_UInt16, new UINT16((UINT16)thisJson[keyChild].asUInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_UInt32:
			childAssetTypeValue = new AssetTypeValue(ValueType_UInt32, new UINT32((UINT32)thisJson[keyChild].asUInt()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_UInt64:
			childAssetTypeValue = new AssetTypeValue(ValueType_UInt64, new UINT64((UINT64)thisJson[keyChild].asUInt64()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_Float:
			childAssetTypeValue = new AssetTypeValue(ValueType_Float, new FLOAT((FLOAT)thisJson[keyChild].asFloat()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_Double:
			childAssetTypeValue = new AssetTypeValue(ValueType_Double, new double((double)thisJson[keyChild].asFloat()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_Bool:
			childAssetTypeValue = new AssetTypeValue(ValueType_Bool, new BOOL((BOOL)thisJson[keyChild].asBool()));
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_String:
			tempStr = new std::string(thisJson[keyChild].asString());
			childAssetTypeValue = new AssetTypeValue(ValueType_String, (void*)tempStr->c_str());
			childAssetTypeValueField->Read(childAssetTypeValue, childAssetTypeTemplateField, 0, 0);
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;

		case ValueType_None:
			if (childAssetTypeTemplateField->isArray) {
				childAssetTypeValueField = GetAssetTypeValueFieldArrayFromJson(childAssetTypeTemplateField, thisJson[keyChild]);
			}
			else {
				childAssetTypeValueField = GetAssetTypeValueFieldFromJsonRecursive(childAssetTypeTemplateField, thisJson[keyChild]);
			}
			assetTypeValueFieldArray->push_back(childAssetTypeValueField);
			break;
		case ValueType_Array:
			throw new std::exception("No implement");
			break;
		}
	}
	assetTypeValueField->Read(assetTypeValue, assetTypeTemplateField, assetTypeValueFieldArray->size(), assetTypeValueFieldArray->data());
	return assetTypeValueField;
}

inline AssetTypeValueField* UnityL10nToolAPI::GetAssetTypeValueFieldFromJson(AssetTypeTemplateField* assetTypeTemplateField, Json::Value json) {
	return GetAssetTypeValueFieldFromJsonRecursive(assetTypeTemplateField, json[json.getMemberNames()[0]]);
}

inline AssetTypeValueField* UnityL10nToolAPI::GetAssetTypeValueFieldArrayFromJson(AssetTypeTemplateField* assetTypeTemplateField, Json::Value json) {
	std::string testStr = JsonToStyleString(json);
	std::vector<AssetTypeValueField*>* assetTypeValueFieldArray = new std::vector<AssetTypeValueField*>();
	for (Json::ArrayIndex i = 0; i < json.size(); i++) {
		Json::Value childJson = json[i];
		std::string key = childJson.getMemberNames()[0];
		assetTypeValueFieldArray->push_back(GetAssetTypeValueFieldFromJsonRecursive(&assetTypeTemplateField->children[1], childJson[key]));
	}

	AssetTypeArray* assetTypeArray = new AssetTypeArray();
	assetTypeArray->size = assetTypeValueFieldArray->size();
	AssetTypeValueField* assetTypeValueField = new AssetTypeValueField();
	assetTypeValueField->Read(new AssetTypeValue(ValueType_Array, assetTypeArray), assetTypeTemplateField, assetTypeValueFieldArray->size(), assetTypeValueFieldArray->data());

	return assetTypeValueField;

}

inline bool UnityL10nToolAPI::ModifyAssetTypeValueFieldFromJSONRecursive(AssetTypeValueField* assetTypeValueField, Json::Value json) {
	std::string key = GetJsonKeyFromAssetTypeValueField(assetTypeValueField);
	std::vector<std::string> jsonKeyList = json.getMemberNames();
	for (unsigned int i = 0; i < assetTypeValueField->GetChildrenCount(); i++) {
		AssetTypeValueField* childAssetTypeValueField = assetTypeValueField->GetChildrenList()[i];
		std::string keyChild = GetJsonKeyFromAssetTypeValueField(childAssetTypeValueField);
		std::vector<std::string>::iterator iterator = find(jsonKeyList.begin(), jsonKeyList.end(), keyChild);

		if (iterator != jsonKeyList.end()) {
			switch (childAssetTypeValueField->GetTemplateField()->valueType) {
			case ValueType_Int8:
				childAssetTypeValueField->GetValue()->Set(new INT8((INT8)json[keyChild].asInt()));
				break;
			case ValueType_Int16:
				childAssetTypeValueField->GetValue()->Set(new INT16((INT16)json[keyChild].asInt()));
				break;
			case ValueType_Int32:
				childAssetTypeValueField->GetValue()->Set(new INT32((INT32)json[keyChild].asInt()));
				break;
			case ValueType_Int64:
				childAssetTypeValueField->GetValue()->Set(new INT64((INT64)json[keyChild].asInt64()));
				break;

			case ValueType_UInt8:
				childAssetTypeValueField->GetValue()->Set(new UINT8((UINT8)json[keyChild].asUInt()));
				break;
			case ValueType_UInt16:
				childAssetTypeValueField->GetValue()->Set(new UINT16((UINT16)json[keyChild].asUInt()));
				break;
			case ValueType_UInt32:
				childAssetTypeValueField->GetValue()->Set(new UINT32((UINT32)json[keyChild].asUInt()));
				break;
			case ValueType_UInt64:
				childAssetTypeValueField->GetValue()->Set(new UINT64((UINT64)json[keyChild].asUInt64()));
				break;

			case ValueType_Float:
				childAssetTypeValueField->GetValue()->Set(new FLOAT((FLOAT)json[keyChild].asFloat()));
				break;
			case ValueType_Double:
				childAssetTypeValueField->GetValue()->Set(new double((double)json[keyChild].asDouble()));
				break;

			case ValueType_Bool:
				childAssetTypeValueField->GetValue()->Set(new BOOL((BOOL)json[keyChild].asBool()));
				break;

			case ValueType_String:
				childAssetTypeValueField->GetValue()->Set(new std::string(json[keyChild].asString()));
				break;

			case ValueType_None:
				if (childAssetTypeValueField->GetTemplateField()->isArray) {
					//ClearAssetTypeValueField(childAssetTypeValueField); // 해야할지 안해도 될지 모르겠
					assetTypeValueField->GetChildrenList()[i] = GetAssetTypeValueFieldArrayFromJson(childAssetTypeValueField->GetTemplateField(), json[keyChild]);
				}
				else {
					ModifyAssetTypeValueFieldFromJSONRecursive(childAssetTypeValueField, json[keyChild]);
				}
				break;

			case ValueType_Array:
				throw new std::exception("No implement");
				break;
			}
		}
	}
	return true;
}

inline bool UnityL10nToolAPI::ModifyAssetTypeValueFieldFromJSON(AssetTypeValueField* assetTypeValueField, Json::Value json) {
	std::vector<std::string> jsonMembers = json.getMemberNames();
	if (jsonMembers.size() == 0) {
		return true;
	}
	else {
		return ModifyAssetTypeValueFieldFromJSONRecursive(assetTypeValueField, json[json.getMemberNames()[0]]);
	}
}

inline std::wstring UnityL10nToolAPI::GetAssetNameW(AssetsFileTable* assetsFileTable, AssetFileInfoEx * assetFileInfoEx) {
	return UnityL10nToolAPI::GetAssetNameW(assetsFileTable->getAssetsFile(), assetFileInfoEx);
}

inline std::wstring UnityL10nToolAPI::GetAssetNameW(AssetsFile* assetsFile, AssetFileInfoEx * assetFileInfoEx)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> WideMultiStringConverterLocal;
	char tempName[100];
	assetFileInfoEx->ReadName(assetsFile, tempName);
	return std::wstring(WideMultiStringConverterLocal.from_bytes(tempName));
}
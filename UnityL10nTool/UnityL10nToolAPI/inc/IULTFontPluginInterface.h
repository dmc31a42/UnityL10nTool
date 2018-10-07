#pragma once
#include <algorithm>
#include "json/json.h"

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"

#include "GeneralPurposeFunctions.h"

using namespace std;

struct UnityL10nToolAPI;
struct FontPluginInfo;
struct FontAssetMap;
struct FontAssetMaps;

typedef bool(_cdecl *GetFontPluginInfoCallback)(UnityL10nToolAPI* unityL10nToolAPI, FontPluginInfo* fontPluginInfo);
/* Used in Project */
typedef bool(_cdecl *SetProjectConfigJsonCallback)(Json::Value pluginConfig);
typedef FontAssetMaps(_cdecl *GetPluginSupportAssetMapCallback)();
typedef bool(_cdecl *SetPluginAssetMapCallback)(std::vector<FontAssetMap>);
typedef Json::Value(_cdecl *GetProjectConfigJsonCallback)();
typedef Json::Value(_cdecl *GetPacherConfigJsonCallback)();
typedef map<string, vector<AssetsReplacer*>>(_cdecl *GetProjectAssetReplacerCallback)();
typedef bool(_cdecl *CopyBuildFileToBuildFolderCallback)(wstring FontPluginRelativePath, wstring targetPath);
/* Used in Patcher */
typedef bool(_cdecl *SetPacherConfigJsonCallback)(Json::Value patcherConfig);
typedef map<string, vector<AssetsReplacer*>>(_cdecl *GetPatcherAssetReplacerCallback)();
typedef bool(_cdecl *CopyResourceFileToGameFolderCallback)(wstring FontPluginRelativePath, wstring targetPath);

struct FontAssetMapOption {
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
	std::vector<FontAssetMapOption> nestedOptions;
};

struct FontAssetMap {
	std::string assetsName;
	std::string assetName;
	std::string containerPath;
	bool useContainerPath;
	std::vector<FontAssetMapOption> options;
};

struct FontAssetMaps {
	std::vector<FontAssetMap> News;
	std::vector<FontAssetMap> Saveds;
};

struct FontPluginInfo {
	wchar_t FontPluginName[64];
	std::wstring relativePluginPath;
	SetProjectConfigJsonCallback SetProjectConfigJson;
	GetPluginSupportAssetMapCallback GetPluginSupportAssetMap;
	SetPluginAssetMapCallback SetPluginAssetMap;
	GetProjectConfigJsonCallback GetProjectConfigJson;
	GetPacherConfigJsonCallback GetPacherConfigJson;
	CopyBuildFileToBuildFolderCallback CopyBuildFileToBuildFolder;

	SetPacherConfigJsonCallback SetPacherConfigJson;
	GetPatcherAssetReplacerCallback GetPatcherAssetReplacer;
	CopyResourceFileToGameFolderCallback CopyResourceFileToGameFolder;
};

struct UnityL10nToolAPI {
	string version;
	string versionFirstTwoNumbers;
	ClassDatabaseFile* BasicClassDatabaseFile;
	ClassDatabaseFile* MonoClassDatabaseFile;
	const ResourceManagerFile* ResourceManagerFileGlobal;
	const AssetsFileTable* GlobalgamemanagersAssetsTable;
	vector<string>* AssetsFileNames;
	/* FindAFromB */
	const map <string, AssetsFile*>* FindAssetsFilesFromAssetsName;
	const map <string, AssetsFileTable*>* FindAssetsFileTablesFromAssetsName;
	const map <INT32, UINT32>* FindBasicClassIndexFromClassID;
	const map <string, UINT32>* FindBasicClassIndexFromClassName;
	const map<pair<string, INT64>, string>* FindMonoClassNameFromAssetsNameANDPathId;
	const map<string, UINT32>* FindMonoClassIndexFromMonoClassName;
	const map<pair<INT32, INT64>, string>* FindContainerPathFromFileIDPathID;
	const map<string, pair<INT32, INT64>>* FindFileIDPathIDFromContainerPath;
	const map<string, INT32>* FindPathIDOfContainerPathFromAssetsName;
	const map<INT32, string>* FindAssetsNameFromPathIDOfContainerPath;

	vector<FontAssetMap> GetFontAssetMapListFromMonoClassName(string AssetsName, string monoClassName);
	void GetClassIdFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, int & classId, UINT16 & monoClassId);
	AssetTypeInstance * GetBasicAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx);
	AssetTypeInstance * GetDetailAssetTypeInstanceFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx);
	AssetTypeTemplateField * GetMonoAssetTypeTemplateFieldFromClassName(string MonoClassName);
	AssetsReplacer * makeAssetsReplacer(string assetsFileName, AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, AssetTypeInstance * assetTypeInstance, Json::Value modifyJson);
	AssetsReplacer * makeAssetsReplacer(string assetsFileName, AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, AssetTypeInstance * assetTypeInstance);
	string GetJsonKeyFromAssetTypeTemplateField(AssetTypeTemplateField * assetTypeTemplateField);
	string GetJsonKeyFromAssetTypeValueField(AssetTypeValueField * assetTypeValueField);
	string GetJsonFromAssetTypeValueFieldRecursive(AssetTypeValueField * field);
	string GetJsonFromAssetTypeValueField(AssetTypeValueField * field);
	AssetTypeValueField * GetAssetTypeValueFieldFromJsonRecursive(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	AssetTypeValueField * GetAssetTypeValueFieldFromJson(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	AssetTypeValueField * GetAssetTypeValueFieldArrayFromJson(AssetTypeTemplateField * assetTypeTemplateField, Json::Value json);
	bool ModifyAssetTypeValueFieldFromJSONRecursive(AssetTypeValueField * assetTypeValueField, Json::Value json);
	bool ModifyAssetTypeValueFieldFromJSON(AssetTypeValueField * assetTypeValueField, Json::Value json);
	string GetClassNameFromBaseAssetTypeValueField(AssetsFileTable* assetsFileTable, AssetTypeValueField * baseAssetTypeValueField);
	INT32 FindAssetIndexFromName(AssetsFileTable * assetsFileTable, string assetName);
};



inline vector<FontAssetMap> UnityL10nToolAPI::GetFontAssetMapListFromMonoClassName(string assetsName, string monoClassName) {
	vector<FontAssetMap> fontAssetMapList;
	int AssetsPathId = distance(
		AssetsFileNames->begin(),
		find(AssetsFileNames->begin(),
			AssetsFileNames->end(),
			assetsName));
	map<string, AssetsFileTable*>::const_iterator assetsFileTableIterator = FindAssetsFileTablesFromAssetsName->find(assetsName);
	if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName->end()) {
		AssetsFileTable* assetsFileTable = assetsFileTableIterator->second;
		AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
		for (unsigned int i = 0; i < assetsFileTable->assetFileInfoCount; i++) {
			AssetFileInfoEx* assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
			int classId;
			WORD monoClassId;
			GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
			if (classId == 0x72) {
				AssetTypeInstance* baseAssetTypeInstance = GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
				AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance->GetBaseField();
				if (baseAssetTypeValueField) {
					string tempMonoClassName = GetClassNameFromBaseAssetTypeValueField(assetsFileTable, baseAssetTypeValueField);
					if (tempMonoClassName == monoClassName) {
						std::string assetName;
						std::string containerPath;
						bool useContainerPath = false;
						if (!baseAssetTypeValueField->Get("m_Name")->IsDummy()) {
							assetName = baseAssetTypeValueField->Get("m_Name")->GetValue()->AsString();
						}
						map<string, INT32>::const_iterator FileIDIterator = FindPathIDOfContainerPathFromAssetsName->find(assetsName);
						if (FileIDIterator != FindPathIDOfContainerPathFromAssetsName->end()) {
							INT32 FileID = FileIDIterator->second;
							INT64 PathID = assetFileInfoEx->index;
							map<pair<INT32, INT64>, string>::const_iterator containerPathIterator = FindContainerPathFromFileIDPathID->find(pair<INT32, INT64>(FileID, PathID));
							if (containerPathIterator != FindContainerPathFromFileIDPathID->end()) {
								containerPath = containerPathIterator->second;
							}
						}
						FontAssetMap tempFontAssetMap = {
							assetsName,
							assetName,
							containerPath,
							false,
							std::vector<FontAssetMapOption> ()};
						fontAssetMapList.push_back(tempFontAssetMap);
					}
				}
			}
		}
	}
	return fontAssetMapList;
}

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

inline string UnityL10nToolAPI::GetClassNameFromBaseAssetTypeValueField(AssetsFileTable* assetsFileTable, AssetTypeValueField* baseAssetTypeValueField) {
	if (baseAssetTypeValueField) {
		string m_Name = baseAssetTypeValueField->Get("m_Name")->GetValue()->AsString();
		AssetTypeValueField* m_ScriptATVF = baseAssetTypeValueField->Get("m_Script");
		if (m_ScriptATVF) {
			int m_FileId = m_ScriptATVF->Get("m_FileID")->GetValue()->AsInt();
			unsigned __int64 m_PathID = m_ScriptATVF->Get("m_PathID")->GetValue()->AsUInt64();
			string assetsName = string(assetsFileTable->getAssetsFile()->dependencies.pDependencies[m_FileId - 1].assetPath);
			return FindMonoClassNameFromAssetsNameANDPathId->find(pair<string, INT64>(assetsName, m_PathID))->second;
		}
		else {
			throw new exception("GetClassNameFromBaseAssetTypeValueField: m_ScriptATVF not exist");
		}
	}
	else {
		throw new exception("GetClassNameFromBaseAssetTypeValueField: baseAssetTypeValueField not exist");
	}
}

inline INT32 UnityL10nToolAPI::FindAssetIndexFromName(AssetsFileTable* assetsFileTable, string assetName) {
	char readName[100];
	AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
	for (unsigned int i = 0; i < assetsFileTable->assetFileInfoCount; i++) {
		AssetFileInfoEx *assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
		assetFileInfoEx->ReadName(assetsFile, readName);
		if (assetName == readName) {
			return i;
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
		string monoScriptFullName = GetClassNameFromBaseAssetTypeValueField(assetsFileTable, baseAssetTypeValueField);
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

inline AssetTypeTemplateField* UnityL10nToolAPI::GetMonoAssetTypeTemplateFieldFromClassName(string MonoClassName) {
	map<string, UINT32>::const_iterator indexOfMonoclassIterator = FindMonoClassIndexFromMonoClassName->find(MonoClassName);
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

inline AssetsReplacer* UnityL10nToolAPI::makeAssetsReplacer(string assetsFileName, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance, Json::Value modifyJson) {
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

inline AssetsReplacer* UnityL10nToolAPI::makeAssetsReplacer(string assetsFileName, AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, AssetTypeInstance* assetTypeInstance) {
	return makeAssetsReplacer(assetsFileName, assetsFileTable, assetFileInfoEx, assetTypeInstance, Json::Value());
}

inline string UnityL10nToolAPI::GetJsonKeyFromAssetTypeTemplateField(AssetTypeTemplateField* assetTypeTemplateField) {
	string align;
	if (assetTypeTemplateField->align || assetTypeTemplateField->valueType == ValueType_String) {
		align = "1";
	}
	else {
		align = "0";
	}
	string key = align + " " + string(assetTypeTemplateField->type) + " " + string(assetTypeTemplateField->name);
	return key;
}

inline string UnityL10nToolAPI::GetJsonKeyFromAssetTypeValueField(AssetTypeValueField* assetTypeValueField) {
	return GetJsonKeyFromAssetTypeTemplateField(assetTypeValueField->GetTemplateField());
}

inline string UnityL10nToolAPI::GetJsonFromAssetTypeValueFieldRecursive(AssetTypeValueField *field) {
	AssetTypeTemplateField* templateField = field->GetTemplateField();
	AssetTypeValueField** fieldChildren = field->GetChildrenList();
	DWORD childrenCount = field->GetChildrenCount();
	string str;
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
		string align;
		if (templateFieldChild->align || templateFieldChild->valueType == ValueType_String) {
			align = "1";
		}
		else {
			align = "0";
		}
		string key = align + " " + string(templateFieldChild->type) + " " + string(templateFieldChild->name);
		string value;
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
			value = to_string((long long)fieldChild->GetValue()->AsInt());
			break;
		case ValueType_UInt8:
		case ValueType_UInt16:
		case ValueType_UInt32:
		case ValueType_UInt64:
			value = to_string(fieldChild->GetValue()->AsUInt64());
			break;
		case ValueType_Float:
			value = to_string((long double)fieldChild->GetValue()->AsFloat());
			break;
		case ValueType_Double:
			value = to_string((long double)fieldChild->GetValue()->AsDouble());
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
			value = "\"" + string(fieldChild->GetValue()->AsString()) + "\"";
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

inline string UnityL10nToolAPI::GetJsonFromAssetTypeValueField(AssetTypeValueField *field) {
	string str = "{\r\n";
	AssetTypeTemplateField* templateField = field->GetTemplateField();
	string key = string(templateField->align ? "1" : "0") + " " + string(templateField->type) + " " + string(templateField->name);
	str += "    \"" + key + "\": \r\n    ";
	string value = GetJsonFromAssetTypeValueFieldRecursive(field);
	str += ReplaceAll(value, "\r\n", "\r\n    ");
	str += "\r\n}";
	return str;
}

inline AssetTypeValueField* UnityL10nToolAPI::GetAssetTypeValueFieldFromJsonRecursive(AssetTypeTemplateField* assetTypeTemplateField, Json::Value json) {
	vector<AssetTypeValueField*>* assetTypeValueFieldArray = new vector<AssetTypeValueField*>();
	AssetTypeValue* assetTypeValue = new AssetTypeValue(ValueType_None, 0);
	AssetTypeValueField* assetTypeValueField = new AssetTypeValueField();
	string align;
	if (assetTypeTemplateField->align || assetTypeTemplateField->valueType == ValueType_String) {
		align = "1";
	}
	else {
		align = "0";
	}
	string key = align + " " + string(assetTypeTemplateField->type) + " " + string(assetTypeTemplateField->name);
	//Json::Value thisJson = json[key];
	Json::Value thisJson = json;
	// 이전코드가 잘못되 수정하는도중 임시로 재할당
	vector<string> testStrs1 = thisJson.getMemberNames();
	/*Json::Value thisJson = */
	for (unsigned int i = 0; i < assetTypeTemplateField->childrenCount; i++) {
		AssetTypeTemplateField* childAssetTypeTemplateField = &assetTypeTemplateField->children[i];
		string alignChild;
		if (childAssetTypeTemplateField->align || childAssetTypeTemplateField->valueType == ValueType_String) {
			alignChild = "1";
		}
		else {
			alignChild = "0";
		}
		string keyChild = alignChild + " " + string(childAssetTypeTemplateField->type) + " " + string(childAssetTypeTemplateField->name);
		//void* container;
		AssetTypeValue* childAssetTypeValue;
		AssetTypeValueField* childAssetTypeValueField = new AssetTypeValueField();
		AssetTypeByteArray* assetTypeByteArray;
		string* tempStr;

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
			tempStr = new string(thisJson[keyChild].asString());
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
			throw new exception("No implement");
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
	Json::StyledWriter writer;
	string testStr = writer.write(json);
	vector<AssetTypeValueField*>* assetTypeValueFieldArray = new vector<AssetTypeValueField*>();
	for (Json::ArrayIndex i = 0; i < json.size(); i++) {
		Json::Value childJson = json[i];
		string key = childJson.getMemberNames()[0];
		assetTypeValueFieldArray->push_back(GetAssetTypeValueFieldFromJsonRecursive(&assetTypeTemplateField->children[1], childJson[key]));
	}

	AssetTypeArray* assetTypeArray = new AssetTypeArray();
	assetTypeArray->size = assetTypeValueFieldArray->size();
	AssetTypeValueField* assetTypeValueField = new AssetTypeValueField();
	assetTypeValueField->Read(new AssetTypeValue(ValueType_Array, assetTypeArray), assetTypeTemplateField, assetTypeValueFieldArray->size(), assetTypeValueFieldArray->data());

	return assetTypeValueField;

}

inline bool UnityL10nToolAPI::ModifyAssetTypeValueFieldFromJSONRecursive(AssetTypeValueField* assetTypeValueField, Json::Value json) {
	string key = GetJsonKeyFromAssetTypeValueField(assetTypeValueField);
	vector<string> jsonKeyList = json.getMemberNames();
	for (unsigned int i = 0; i < assetTypeValueField->GetChildrenCount(); i++) {
		AssetTypeValueField* childAssetTypeValueField = assetTypeValueField->GetChildrenList()[i];
		string keyChild = GetJsonKeyFromAssetTypeValueField(childAssetTypeValueField);
		vector<string>::iterator iterator = find(jsonKeyList.begin(), jsonKeyList.end(), keyChild);

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
				childAssetTypeValueField->GetValue()->Set(new string(json[keyChild].asString()));
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
				throw new exception("No implement");
				break;
			}
		}
	}
	return true;
}

inline bool UnityL10nToolAPI::ModifyAssetTypeValueFieldFromJSON(AssetTypeValueField* assetTypeValueField, Json::Value json) {
	vector<string> jsonMembers = json.getMemberNames();
	if (jsonMembers.size() == 0) {
		return true;
	}
	else {
		return ModifyAssetTypeValueFieldFromJSONRecursive(assetTypeValueField, json[json.getMemberNames()[0]]);
	}
}
#pragma once
#include <algorithm>
#include "json/json.h"
#include <string>
#include <vector>

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"
#include "GeneralPurposeFunctions.h"
#include "IULTPluginCommonInterface.h"

struct FontPluginInfo;
struct FontAssetMap;
struct FontAssetMaps;

typedef bool(_cdecl *GetFontPluginInfoCallback)(UnityL10nToolAPI* unityL10nToolAPI, FontPluginInfo* fontPluginInfo);
/* Used in Project */
typedef bool(_cdecl *SetProjectConfigJsonCallback)(Json::Value pluginConfig);
typedef FontAssetMaps(_cdecl *GetPluginSupportAssetMapCallback)();
typedef bool(_cdecl *SetPluginSupportAssetMapCallback)(FontAssetMaps supportAssetMaps);
typedef Json::Value(_cdecl *GetProjectConfigJsonCallback)();
typedef Json::Value(_cdecl *GetPacherConfigJsonCallback)();
typedef std::map<std::string, std::vector<AssetsReplacer*>>(_cdecl *GetProjectAssetReplacerCallback)();
typedef bool(_cdecl *CopyBuildFileToBuildFolderCallback)(std::wstring FontPluginRelativePath, std::wstring targetPath);
/* Used in Patcher */
typedef bool(_cdecl *SetPacherConfigJsonCallback)(Json::Value patcherConfig);
typedef std::map<std::string, std::vector<AssetsReplacer*>>(_cdecl *GetPatcherAssetReplacerCallback)();
typedef bool(_cdecl *CopyResourceFileToGameFolderCallback)(std::wstring FontPluginRelativePath, std::wstring targetPath);

struct FontAssetMap {
public:
	std::wstring Id;
	std::string assetsName;
	std::string assetName;
	std::string containerPath;
	bool useContainerPath;
	std::vector<AssetMapOption> options;
	INT64 _m_PathID;
	bool LooseEquals(FontAssetMap fontAssetMap);
	FontAssetMap();
	FontAssetMap(std::wstring Id, std::string assetsName, std::string assetName, std::string containerPath, bool useContainerPath, std::vector<AssetMapOption> options);
	FontAssetMap(Json::Value json);
	Json::Value ToJson();
};

struct FontAssetMaps {
	std::vector<FontAssetMap> News;
	std::vector<FontAssetMap> Saveds;
};

struct FontPluginInfo {
	wchar_t FontPluginName[64];
	std::wstring relativePluginPath;
	std::wstring pluginFileName;
	SetProjectConfigJsonCallback SetProjectConfigJson;
	GetPluginSupportAssetMapCallback GetPluginSupportAssetMap;
	SetPluginSupportAssetMapCallback SetPluginSupportAssetMap;
	GetProjectConfigJsonCallback GetProjectConfigJson;
	GetPacherConfigJsonCallback GetPacherConfigJson;
	CopyBuildFileToBuildFolderCallback CopyBuildFileToBuildFolder;

	SetPacherConfigJsonCallback SetPacherConfigJson;
	GetPatcherAssetReplacerCallback GetPatcherAssetReplacer;
	CopyResourceFileToGameFolderCallback CopyResourceFileToGameFolder;
};

inline std::vector<FontAssetMap> GetFontAssetMapListFromMonoClassName(UnityL10nToolAPI* unityL10nToolAPI, std::string assetsName, std::string monoClassName);

inline bool FontAssetMap::LooseEquals(FontAssetMap fontAssetMap) {
	if (this->assetName == fontAssetMap.assetName &&
		this->assetsName == fontAssetMap.assetsName &&
		this->containerPath == fontAssetMap.containerPath &&
		this->Id == fontAssetMap.Id) {
		return true;
	}
	else {
		return false;
	}
}

inline FontAssetMap::FontAssetMap() {}

inline FontAssetMap::FontAssetMap(std::wstring Id, std::string assetsName, std::string assetName, std::string containerPath, bool useContainerPath, std::vector<AssetMapOption> options)
	: Id(Id), assetsName(assetsName), assetName(assetName), containerPath(containerPath), useContainerPath(useContainerPath), options(options) {

}

inline FontAssetMap::FontAssetMap(Json::Value json) {
	if (json.isMember("assetsName") && json["assetsName"].isString()) {
		this->assetsName = json["assetsName"].asString();
	}
	else {
		this->assetsName = "";
	}
	if (json.isMember("assetName") && json["assetName"].isString()) {
		this->assetName = json["assetName"].asString();
	}
	else {
		this->assetName = "";
	}
	if (json.isMember("containerPath") && json["containerPath"].isString()) {
		this->containerPath = json["containerPath"].asString();
	}
	else {
		this->containerPath = "";
	}
	if (json.isMember("Id") && json["Id"].isString()) {
		this->Id = WideMultiStringConverter->from_bytes(json["Id"].asString());
	}
	else {
		this->Id = L"";
	}
	if (json.isMember("useContainerPath") && json["useContainerPath"].isBool()) {
		this->useContainerPath = json["useContainerPath"].asBool();
	}
	else {
		this->useContainerPath = false;
	}
	if (json.isMember("options") && json["options"].isArray()) {
		Json::Value optionsJsonArray = json["options"];
		for (Json::ArrayIndex i = 0; i < optionsJsonArray.size(); i++) {
			this->options.push_back(AssetMapOption((Json::Value)optionsJsonArray[i]));
		}
	}
}

inline Json::Value FontAssetMap::ToJson() {
	Json::Value json;
	json["assetsName"] = this->assetsName;
	json["assetName"] = this->assetName;
	json["containerPath"] = this->containerPath;
	json["Id"] = WideMultiStringConverter->to_bytes(this->Id);
	json["useContainerPath"] = this->useContainerPath;
	Json::Value optionsJsonArray(Json::arrayValue);
	for (std::vector<AssetMapOption>::iterator iterator = this->options.begin(); iterator != this->options.end(); iterator++) {
		optionsJsonArray.append(iterator->ToJson());
	}
	json["options"] = optionsJsonArray;
	return json;
}

inline std::vector<FontAssetMap> GetFontAssetMapListFromMonoClassName(UnityL10nToolAPI * unityL10nToolAPI, std::string assetsName, std::string monoClassName) {
	std::vector<FontAssetMap> fontAssetMapList;
	int AssetsPathId = distance(
		unityL10nToolAPI->AssetsFileNames->begin(),
		find(unityL10nToolAPI->AssetsFileNames->begin(),
			unityL10nToolAPI->AssetsFileNames->end(),
			assetsName));
	std::map<std::string, AssetsFileTable*>::const_iterator assetsFileTableIterator = unityL10nToolAPI->FindAssetsFileTablesFromAssetsName->find(assetsName);
	if (assetsFileTableIterator != unityL10nToolAPI->FindAssetsFileTablesFromAssetsName->end()) {
		AssetsFileTable* assetsFileTable = assetsFileTableIterator->second;
		AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
		for (unsigned int i = 0; i < assetsFileTable->assetFileInfoCount; i++) {
			AssetFileInfoEx* assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
			int classId;
			WORD monoClassId;
			unityL10nToolAPI->GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
			if (classId == 0x72) {
				AssetTypeInstance* baseAssetTypeInstance = unityL10nToolAPI->GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
				AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance->GetBaseField();
				if (baseAssetTypeValueField) {
					std::string tempMonoClassName = unityL10nToolAPI->GetClassNameFromBaseAssetTypeValueField(assetsFileTable, baseAssetTypeValueField);
					if (tempMonoClassName == monoClassName) {
						std::string assetName;
						std::string containerPath;
						bool useContainerPath = false;
						if (!baseAssetTypeValueField->Get("m_Name")->IsDummy()) {
							assetName = baseAssetTypeValueField->Get("m_Name")->GetValue()->AsString();
						}
						std::map<std::string, INT32>::const_iterator FileIDIterator = unityL10nToolAPI->FindPathIDOfContainerPathFromAssetsName->find(assetsName);
						if (FileIDIterator != unityL10nToolAPI->FindPathIDOfContainerPathFromAssetsName->end()) {
							INT32 FileID = FileIDIterator->second;
							INT64 PathID = assetFileInfoEx->index;
							std::map<std::pair<INT32, INT64>, std::string>::const_iterator containerPathIterator = unityL10nToolAPI->FindContainerPathFromFileIDPathID->find(std::pair<INT32, INT64>(FileID, PathID));
							if (containerPathIterator != unityL10nToolAPI->FindContainerPathFromFileIDPathID->end()) {
								containerPath = containerPathIterator->second;
							}
						}
						FontAssetMap tempFontAssetMap = FontAssetMap(
							L"",
							assetsName,
							assetName,
							containerPath,
							false,
							std::vector<AssetMapOption>());
						tempFontAssetMap._m_PathID = assetFileInfoEx->index;
						fontAssetMapList.push_back(tempFontAssetMap);
					}
				}
			}
		}
	}
	return fontAssetMapList;
}




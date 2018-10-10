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
#include "IULTPluginCommonInterface.h"

using namespace std;

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

inline vector<FontAssetMap> GetFontAssetMapListFromMonoClassName(UnityL10nToolAPI* unityL10nToolAPI, string assetsName, string monoClassName) {
	vector<FontAssetMap> fontAssetMapList;
	int AssetsPathId = distance(
		unityL10nToolAPI->AssetsFileNames->begin(),
		find(unityL10nToolAPI->AssetsFileNames->begin(),
			unityL10nToolAPI->AssetsFileNames->end(),
			assetsName));
	map<string, AssetsFileTable*>::const_iterator assetsFileTableIterator = unityL10nToolAPI->FindAssetsFileTablesFromAssetsName->find(assetsName);
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
					string tempMonoClassName = unityL10nToolAPI->GetClassNameFromBaseAssetTypeValueField(assetsFileTable, baseAssetTypeValueField);
					if (tempMonoClassName == monoClassName) {
						std::string assetName;
						std::string containerPath;
						bool useContainerPath = false;
						if (!baseAssetTypeValueField->Get("m_Name")->IsDummy()) {
							assetName = baseAssetTypeValueField->Get("m_Name")->GetValue()->AsString();
						}
						map<string, INT32>::const_iterator FileIDIterator = unityL10nToolAPI->FindPathIDOfContainerPathFromAssetsName->find(assetsName);
						if (FileIDIterator != unityL10nToolAPI->FindPathIDOfContainerPathFromAssetsName->end()) {
							INT32 FileID = FileIDIterator->second;
							INT64 PathID = assetFileInfoEx->index;
							map<pair<INT32, INT64>, string>::const_iterator containerPathIterator = unityL10nToolAPI->FindContainerPathFromFileIDPathID->find(pair<INT32, INT64>(FileID, PathID));
							if (containerPathIterator != unityL10nToolAPI->FindContainerPathFromFileIDPathID->end()) {
								containerPath = containerPathIterator->second;
							}
						}
						FontAssetMap tempFontAssetMap = {
							assetsName,
							assetName,
							containerPath,
							false,
							std::vector<FontAssetMapOption>() };
						fontAssetMapList.push_back(tempFontAssetMap);
					}
				}
			}
		}
	}
	return fontAssetMapList;
}
#pragma once
#include <string>

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"

#include "IULTFontPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"
using namespace std;

class UnityL10nToolCpp
{
	wstring GameFolderPath;
	wstring CurrentDirectory;
	string FirstAssetsFileName;
	string version;
	string versionFirstTwoNumbers;
	ClassDatabaseFile* BasicClassDatabaseFile;
	ClassDatabaseFile* MonoClassDatabaseFile;
	ResourceManagerFile* ResourceManagerFileGlobal;
	AssetsFileTable* GlobalgamemanagersAssetsTable;
	vector<string> AssetsFileNames;
	/* FindAFromB */
	map <string, AssetsFile*> FindAssetsFilesFromAssetsName;
	map <string, AssetsFileTable*> FindAssetsFileTablesFromAssetsName;
	map <INT32, UINT32> FindBasicClassIndexFromClassID;
	map <string, UINT32> FindBasicClassIndexFromClassName;
	map<INT64, string> FindMonoClassNameFromMonoScriptPathId;
	map<string, UINT32> FindMonoClassIndexFromMonoClassName;
	map<pair<INT32, INT64>, string> FindContainerPathFromFileIDPathID;
	map<string, pair<INT32, INT64>> FindFileIDPathIDFromContainerPath;
	map<string, INT32> FindPathIDOfContainerPathFromAssetsName;
	map<INT32, string> FindAssetsNameFromPathIDOfContainerPath;

	/* FontPlugin */
	map<wstring, HINSTANCE> FontPluginMap;
	map<wstring, FontPluginInfo*> FontPluginInfoMap;

	UnityL10nToolAPI _unityL10nToolAPI;

	Json::Value projectJson;

public:
	UnityL10nToolCpp(wstring gameFolderPath);	
	map<wstring, vector<FontAssetMap>> GetPluginsSupportAssetMap();
	~UnityL10nToolCpp();

protected:
	bool LoadAssetsFile(string assetsFileName);
	bool LoadBasicClassDatabase();
	bool ProcessResourceAndMonoManger(AssetsFileTable * globalgamemanagersTable, string globalgamemanagersName);
	void GetClassIdFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, int & classId, UINT16 & monoClassId);
	bool LoadMonoClassDatabase(vector<string> AssemblyNames);
	bool LoadFindMonoClassNameFromMonoScriptPathId(AssetsFileTable * globalgamemanagersAssetsTable);
	bool LoadUnityL10nToolAPI();
	bool LoadFontPlugins();

};
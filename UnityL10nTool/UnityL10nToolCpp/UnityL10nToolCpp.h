#pragma once
#include <string>

#include "AssetsTools/AssetsFileReader.h"
#include "AssetsTools/AssetsFileFormat.h"
#include "AssetsTools/ClassDatabaseFile.h"
#include "AssetsTools/AssetsFileTable.h"
#include "AssetsTools/ResourceManagerFile.h"
#include "AssetsTools/AssetTypeClass.h"

#include "IULTFontPluginInterface.h"
#include "IULTTextPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"
using namespace std;

struct UnityL10nToolProjectInfo
{
public :
	wstring GameName;
	wstring MakerName;
	wstring GamePath;
	wstring JSONPath;
	wstring DataFolderName;
	wstring ProjectRelativeFolder;
	/*UnityL10nToolProjectInfo(wstring GameName, wstring MakerName, wstring GamePath, wstring JSONPath, wstring DataFolderName)
	{
		this->GameName = GameName;
		this->MakerName = MakerName;
		this->GamePath = GamePath;
		this->JSONPath = JSONPath;
		this->DataFolderName = DataFolderName;
	}*/
};

struct CopyResourcesManualPair {
public:
	wstring ResourcesFileFromProjectFolder;
	wstring ResourcesFileTargetRelativePath;
	CopyResourcesManualPair() {}
	CopyResourcesManualPair(Json::Value json) {
		if (json.isMember("ResourcesFileFromProjectFolder")) {
			this->ResourcesFileFromProjectFolder = WideMultiStringConverter->from_bytes(json["ResourcesFileFromProjectFolder"].asString());
		}
		if (json.isMember("ResourcesFileTargetRelativePath")) {
			this->ResourcesFileTargetRelativePath = WideMultiStringConverter->from_bytes(json["ResourcesFileTargetRelativePath"].asString());
		}
	}
	Json::Value toString() {
		Json::Value json;
		json["ResourcesFileFromProjectFolder"] = WideMultiStringConverter->to_bytes(this->ResourcesFileFromProjectFolder);
		json["ResourcesFileTargetRelativePath"] = WideMultiStringConverter->to_bytes(this->ResourcesFileTargetRelativePath);
		return json;
	}
};

struct OnlineResourcePair {
public:
	wstring filePath;
	wstring URL;
	OnlineResourcePair() {}
	OnlineResourcePair(Json::Value json) {
		if (json.isMember("filePath")) {
			this->filePath = WideMultiStringConverter->from_bytes(json["filePath"].asString());
		}
		if (json.isMember("URL")) {
			this->URL = WideMultiStringConverter->from_bytes(json["URL"].asString());
		}
	}
	Json::Value toJson() {
		Json::Value json;
		json["filePath"] = WideMultiStringConverter->to_bytes(this->filePath);
		json["URL"] = WideMultiStringConverter->to_bytes(this->URL);
		return json;
	}
};

struct ProjectSettings {
public:
	bool DownloadOnlineResourcesWhenBuild;
	bool ZipBuildFolderAfterBuild;
	bool CreateCurrentVersionTxtFileAfterBuild;
	bool RemoveBuildFolderBeforeBuild;
	wstring ZipFileName;
	bool IsZipFileNameConatinVersion;
	ProjectSettings() {
		this->DownloadOnlineResourcesWhenBuild = false;
		this->ZipBuildFolderAfterBuild = false;
		this->CreateCurrentVersionTxtFileAfterBuild = false;
		this->RemoveBuildFolderBeforeBuild = false;
		this->ZipFileName = L"";
		this->IsZipFileNameConatinVersion = false;
	}
	ProjectSettings(Json::Value json) {
		if (json.isMember("DownloadOnlineResourcesWhenBuild")) {
			this->DownloadOnlineResourcesWhenBuild = json["DownloadOnlineResourcesWhenBuild"].asBool();
		}
		else {
			this->DownloadOnlineResourcesWhenBuild = false;
		}
		if (json.isMember("ZipBuildFolderAfterBuild")) {
			this->ZipBuildFolderAfterBuild = json["ZipBuildFolderAfterBuild"].asBool();
		}
		else {
			this->ZipBuildFolderAfterBuild = false;
		}
		if (json.isMember("CreateCurrentVersionTxtFileAfterBuild")) {
			this->CreateCurrentVersionTxtFileAfterBuild = json["CreateCurrentVersionTxtFileAfterBuild"].asBool();
		}
		else {
			this->CreateCurrentVersionTxtFileAfterBuild = false;
		}
		if (json.isMember("RemoveBuildFolderBeforeBuild")) {
			this->RemoveBuildFolderBeforeBuild = json["RemoveBuildFolderBeforeBuild"].asBool();
		}
		if (json.isMember("ZipFileName")) {
			this->ZipFileName = WideMultiStringConverter->from_bytes(json["ZipFileName"].asString());
		}
		else {
			this->ZipFileName = L"";
		}
		if (json.isMember("IsZipFileNameConatinVersion")) {
			this->IsZipFileNameConatinVersion = json["IsZipFileNameConatinVersion"].asBool();
		}
		else {
			this->IsZipFileNameConatinVersion = false;
		}
	}
	Json::Value toJson() {
		Json::Value json;
		json["DownloadOnlineResourcesWhenBuild"] = this->DownloadOnlineResourcesWhenBuild;
		json["ZipBuildFolderAfterBuild"] = this->ZipBuildFolderAfterBuild;
		json["CreateCurrentVersionTxtFileAfterBuild"] = this->CreateCurrentVersionTxtFileAfterBuild;
		json["RemoveBuildFolderBeforeBuild"] = this->RemoveBuildFolderBeforeBuild;
		json["ZipFileName"] = WideMultiStringConverter->to_bytes(this->ZipFileName);
		json["IsZipFileNameConatinVersion"] = this->IsZipFileNameConatinVersion;
		return json;
	}
};

struct OnlineUpdate {
public:
	enum SelectedEnum
	{
		None = 0,
		Manual = 1,
		GitHub = 2,
	};
	SelectedEnum Selected;
	wstring currentVersion;
	wstring currentVersionURL; // for Manual
	wstring manualZipURL; // for Manual
	wstring gitHubOwner;
	wstring gitHubRepo;
	OnlineUpdate() {
		Selected = OnlineUpdate::None;
	}
	OnlineUpdate(Json::Value json) {
		if(json.isMember("Selected")) {
			string selectedStr = json["Selected"].asString();
			if (selectedStr == "Manual") {
				this->Selected = OnlineUpdate::Manual;
			}
			else if (selectedStr == "GitHub") {
				this->Selected = OnlineUpdate::GitHub;
			}
			else {
				this->None;
			}
			if (json.isMember("currentVersion")) {
				this->currentVersion = WideMultiStringConverter->from_bytes(json["currentVersion"].asString());
				switch (this->Selected) {
				case Manual:
					if (json.isMember("currentVersionURL") && json.isMember("manualZipURL")) {
						this->currentVersionURL = WideMultiStringConverter->from_bytes(json["currentVersionURL"].asString());
						this->manualZipURL = WideMultiStringConverter->from_bytes(json["manualZipURL"].asString());
					}
					else {
						this->Selected = OnlineUpdate::None;
					}
					break;
				case GitHub:
					if(json.isMember("gitHubOwner") && json.isMember("gitHubRepo")) {
						this->gitHubOwner = WideMultiStringConverter->from_bytes(json["gitHubOwner"].asString());
						this->gitHubRepo = WideMultiStringConverter->from_bytes(json["gitHubRepo"].asString());
					}
					else {
						this->Selected = OnlineUpdate::None;
					}
					break;
				default:
					this->Selected = OnlineUpdate::None;
				}
			}
			else {
				this->Selected = OnlineUpdate::None;
			}
		}
		else {
			this->Selected = OnlineUpdate::None;
		}
	}
	Json::Value toJson() {
		Json::Value json;
		if (this->currentVersion != L"") {
			json["currentVersion"] = WideMultiStringConverter->to_bytes(this->currentVersion);
			switch (this->Selected) {
			case OnlineUpdate::Manual:
				if (currentVersionURL != L"" && manualZipURL != L"") {
					json["Selected"] = "Manual";
					json["currentVersionURL"] = WideMultiStringConverter->to_bytes(this->currentVersionURL);
					json["manualZipURL"] = WideMultiStringConverter->to_bytes(this->manualZipURL);
					return json;
				}
			case OnlineUpdate::GitHub:
				if (this->gitHubOwner != L"" && this->gitHubRepo != L"") {
					json["Selected"] = "GitHub";
					json["gitHubOwner"] = WideMultiStringConverter->to_bytes(this->gitHubOwner);
					json["gitHubRepo"] = WideMultiStringConverter->to_bytes(this->gitHubRepo);
					return json;
				}
			}
		}
		json["Selected"] = "None";
		return json;
	}
};

class UnityL10nToolCpp
{
	UnityL10nToolProjectInfo UnityL10nToolProjectInfoGlobal;
	wstring GameFolderPath;
	wstring CurrentDirectory;
	wstring ProjectJsonFolderPath;
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
	map<AssetsFileTable*, string> FindAssetsNameFromAssetsFileTables;
	map<AssetsFile*, string> FindAssetsNameFromAssetsFiles;
	map <INT32, UINT32> FindBasicClassIndexFromClassID;
	map <string, UINT32> FindBasicClassIndexFromClassName;
	map<pair<string, INT64>, string> FindMonoClassNameFromAssetsNameANDPathId;
	map<string, UINT32> FindMonoClassIndexFromMonoClassName;
	map<pair<INT32, INT64>, string> FindContainerPathFromFileIDPathID;
	map<string, pair<INT32, INT64>> FindFileIDPathIDFromContainerPath;
	map<string, INT32> FindPathIDOfContainerPathFromAssetsName;
	map<INT32, string> FindAssetsNameFromPathIDOfContainerPath;

	/* FontPlugin */
	map<wstring, HINSTANCE> FontPluginMap;
	map<wstring, FontPluginInfo*> FontPluginInfoMap;
	map<wstring, FontAssetMaps> FontAssetMapsGlobal;
#pragma region TextPlugin member
	TextAssetMaps TextAssetMapsGlobal;
	map<wstring, HINSTANCE> TextplugInMap;
	vector<wstring> TextPluginNames;
	map<wstring, TextPluginInfo*> TextPluginInfoInteractWithAssetMap;
	map<wstring, TextPluginInfo*> TextPluginInfoInteractWithFileTextMap;
#pragma endregion

#pragma region OnlineResourcePair member
	vector<OnlineResourcePair> OnlineResourcePairsGlobal;
#pragma endregion
#pragma region OnlineUpdate member
	OnlineUpdate OnlineUpdateGlobal;
#pragma endregion
#pragma region CopyResourcesManualPair member
	vector<CopyResourcesManualPair> CopyResourcesManualPairsGlobal;
#pragma endregion
	ProjectSettings ProjectSettingsGlobal;

	UnityL10nToolAPI _unityL10nToolAPI;
	vector<string> AssemblyNames;
	Json::Value projectJson;
	map<string, vector<AssetsReplacer*>> AssetsReplacersMap;

public:
	UnityL10nToolCpp(wstring projectJsonFolderPath, wstring gameFolderPath = L"");
	bool LoadGlobalgamemanagersFile();
	bool ProcessResourceAndMonoManger();
	bool LoadMonoClassDatabase();
	bool LoadUnityL10nToolAPI();
	vector<wstring> LoadFontPlugins();
	map<wstring, FontAssetMaps> GetPluginsSupportAssetMap();
#pragma region TextAssetPluginProject
	bool LoadTextPlugins();
	vector<wstring> GetInteractWithAssetPluginNames();
	vector<wstring> GetInteractWithFileTextPluginNames();
	TextAssetMaps GetTextAssetMaps();
	TextAssetMap GetTextAssetMapFromExternalFile(wstring ExternalRelativeFilePath);
	bool SetTextAssetMaps(TextAssetMap textAssetMap, TextAssetMap::ToWhere toWhere);
	//vector<wstring> GetTextPluginNameInteractWithAssetList();
	//vector<wstring> GetTextPluginNameInteractWithFileTextList();
	//vector<wstring> GetTextPluginNameInteractWithMonoAssetList();
	// deprecated
	//TextAssetMap GetTextAssetOptions(TextAssetMap textAssetMap);
	//wstring GetOriginalText(TextAssetMap textAssetMap);
	TextAssetMap GetOriginalLanguagePairDics(TextAssetMap textAssetMap, bool IsFinal);
	TextAssetMap GetTranslatedText(TextAssetMap textAssetMap, bool IsFinal);
	TextAssetMap GetUpdateFileText(TextAssetMap textAssetMap, bool IsFinal);
	bool SaveUpdateFileToTempFolder(TextAssetMap textAssetMap);
	TextAssetMap LoadTranslatedFileTextFromTempAndResourceFolder(TextAssetMap textAssetMap);
	bool UpdateTextAssetMap(TextAssetMap textAssetMap);
	TextAssetMap GetTranslatedLanguagePairDics(TextAssetMap textAssetMap, bool IsFinal);
#pragma endregion

#pragma region MonoTextAssetPluginProject
	vector<TextAssetMap> GetMonoTextAssetMaps();
#pragma endregion

#pragma region TextAssetPluginPatcher
	bool GetTranslatedTextAssetsFromFile();
	bool GetTranslatedMaps();
	bool GetAssetReplacerFromTextAssets();
#pragma endregion

#pragma region MonoTextAssetPluginPatcher
	bool GetAssetReplacerFromMonoTexts();
#pragma endregion
	vector<OnlineResourcePair> GetOnlineResourcePairs();
	void AddOnlineResoucesPair(OnlineResourcePair onlineResourcePair);
	void SetOnlineResourcePairs(vector<OnlineResourcePair> onlineResourcePairs);
	Json::Value SetOnlineResourcePairsToProjectJson();
	// deprecated
	void DownloadResourcesFromInternetToTempFolder();
	// deprecated
	void DownloadResourcesFromInternetToBuildResourceFolder();
	void DownloadResourcesFromInternetToFolder(wstring folderPath);

#pragma region OnlineUpdate
	OnlineUpdate GetOnlineUpdate();
	void SetOnlineUpdate(OnlineUpdate onlineUpdate);
#pragma endregion

#pragma region CopyResourcesManual
	vector<CopyResourcesManualPair> GetCopyResourcesManualPairs();
	void AddCopyResourcesManualPair(CopyResourcesManualPair copyResourcesManualPair);
	void SetCopyResourcesManualPairs(vector<CopyResourcesManualPair> copyResourcesManualPairs);
	Json::Value SetCopyResourcesManualPairsToProjectJson();
	void CopyResourcesManualPairsForPatcher();
#pragma endregion

	bool SetPluginsSupportAssetMap(map<wstring, FontAssetMaps> pluginSupportAssetMaps);
	bool GetProjectConfigJsonFromFontPlugin();
	bool SetTextPluginConfigToJsonValue();
	void LoadProjectSettingsFromJson();
	ProjectSettings GetProjectSettings();
	void SetProjectSettings(ProjectSettings projectSettings);
	bool SaveProjectConfigJson();
	Json::Value GetPacherConfigJson();
	bool BuildProject(wstring buildTargetFolder);
	
	bool SetPacherConfigJson();
	bool LoadAssetsReplacer();
	bool MakeModifiedAssetsFile();
	bool CopyResourceFileToGameFolder();
	~UnityL10nToolCpp();

	static bool DetermineUnityGameFolder(wstring path);
	static bool DetermineProjectGamePath(wstring path, wstring GameName, wstring MakerName);
	static wstring FindUnityGameFolderFromDataFolderName(wstring dataFolderName, wstring GameName, wstring MakerName);
	static wstring MakeSureBackslashEndOfFolderPath(wstring path);

	static wstring GetAssetNameW(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx);
	static wstring GetAssetNameW(AssetsFile * assetsFile, AssetFileInfoEx * assetFileInfoEx);

	static wstring NewGameProjectFromFolder(wstring folder);
protected:
	bool LoadAssetsFile(string assetsFileName);
	bool LoadBasicClassDatabase();
	
	void GetClassIdFromAssetFileInfoEx(AssetsFileTable * assetsFileTable, AssetFileInfoEx * assetFileInfoEx, int & classId, UINT16 & monoClassId);
	
	//bool LoadFindMonoClassNameFromMonoScriptPathId(AssetsFileTable * globalgamemanagersAssetsTable);
	//bool LoadFindMonoClassNameFromMonoScriptPathId();
};

bool GetGameMakerNameFromGlobalgamemanager(wstring path, wstring & GameName, wstring & MakerName);

#include "stdafx.h"
#include <vcclr.h>
#include "UnityL10nToolCppCLI.h"
#include "../UnityL10nToolAPI/inc/IULTFontPluginInterface.h"

String^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::DetermineUnityGameFolder(String^ path) {
	pin_ptr<const wchar_t> ptr = PtrToStringChars(path);
	return ConvertFromWstringToManagedString(UnityL10nToolCpp::DetermineUnityGameFolder(ptr));
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::DetermineProjectGamePath(String ^ path, String ^ GameName, String ^ MakerName)
{
	pin_ptr<const wchar_t> ptrpath = PtrToStringChars(path);
	pin_ptr<const wchar_t> ptrGameName = PtrToStringChars(GameName);
	pin_ptr<const wchar_t> ptrMakerName = PtrToStringChars(MakerName);
	return UnityL10nToolCpp::DetermineProjectGamePath(ptrpath, ptrGameName, ptrMakerName);
	return false;
}

String^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::FindUnityGameFolderFromDataFolderName(String^ dataFolderName, String^ GameName, String^ MakerName) {
	pin_ptr<const wchar_t> ptrdataFolderName = PtrToStringChars(dataFolderName);
	pin_ptr<const wchar_t> ptrGameName = PtrToStringChars(GameName);
	pin_ptr<const wchar_t> ptrMakerName = PtrToStringChars(MakerName);
	wstring result = UnityL10nToolCpp::FindUnityGameFolderFromDataFolderName(ptrdataFolderName, ptrGameName, ptrMakerName);
	return gcnew String(result.c_str());
}

String ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::NewGameProjectFromFolder(String ^ folder)
{
	return ConvertFromWstringToManagedString(UnityL10nToolCpp::NewGameProjectFromFolder(ConvertFromManagedStringToWstring(folder)));
}

UnityL10nToolCppCLI::UnityL10nToolCppManaged::UnityL10nToolCppManaged(String ^ projectJsonFolderPath, String ^ gameFolderPath)
{
	unityL10nToolCpp = new UnityL10nToolCpp(ConvertFromManagedStringToWstring(projectJsonFolderPath), ConvertFromManagedStringToWstring(gameFolderPath));
}

UnityL10nToolCppCLI::UnityL10nToolCppManaged::UnityL10nToolCppManaged(String ^ projectJsonFolderPath)
{
	unityL10nToolCpp = new UnityL10nToolCpp(ConvertFromManagedStringToWstring(projectJsonFolderPath));
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadGlobalgamemanagersFile() {
	return unityL10nToolCpp->LoadGlobalgamemanagersFile();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::ProcessResourceAndMonoManger() {
	return unityL10nToolCpp->ProcessResourceAndMonoManger();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadMonoClassDatabase() {
	return unityL10nToolCpp->LoadMonoClassDatabase();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadUnityL10nToolAPI()
{
	return unityL10nToolCpp->LoadUnityL10nToolAPI();
}

List<String^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadFontPlugins()
{
	vector<wstring> tempLoadedFontPlugins = unityL10nToolCpp->LoadFontPlugins();
	List<String^>^ loadedFontPlugins = gcnew List<String^>();
	for (vector<wstring>::iterator iterator = tempLoadedFontPlugins.begin();
		iterator != tempLoadedFontPlugins.end(); iterator++) {
		loadedFontPlugins->Add(gcnew String(iterator->c_str()));
	}
	return loadedFontPlugins;
}

Dictionary<String^, UnityL10nToolCppCLI::FontAssetMapsCLI^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetPluginsSupportAssetMap()
{
	map<wstring, FontAssetMaps> u_pluginsSupportAssetMap = unityL10nToolCpp->GetPluginsSupportAssetMap();
	Dictionary<String^, FontAssetMapsCLI^>^ result = gcnew Dictionary<String^, FontAssetMapsCLI^>(u_pluginsSupportAssetMap.size());
	for(map<wstring, FontAssetMaps>::iterator keyValueIterator = u_pluginsSupportAssetMap.begin();
		keyValueIterator != u_pluginsSupportAssetMap.end(); keyValueIterator++) {
		result->Add(gcnew String(keyValueIterator->first.c_str()), gcnew FontAssetMapsCLI(keyValueIterator->second));
	}
	return result;
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetPluginsSupportAssetMap(Dictionary<String^, FontAssetMapsCLI^>^ pluginsSupportAssetMapsCLI)
{
	map<wstring, FontAssetMaps> u_pluginsSupportAssetMap;
	for each(KeyValuePair<String^, FontAssetMapsCLI^> item in pluginsSupportAssetMapsCLI) {
		pin_ptr<const wchar_t> ptrKey = PtrToStringChars(item.Key);
		wstring keyWS = wstring(ptrKey);
		u_pluginsSupportAssetMap[keyWS] = item.Value->toUnmanaged();
	}
	return unityL10nToolCpp->SetPluginsSupportAssetMap(u_pluginsSupportAssetMap);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetProjectConfigJsonFromFontPlugin()
{
	return unityL10nToolCpp->GetProjectConfigJsonFromFontPlugin();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetTextPluginConfigToJsonValue()
{
	return unityL10nToolCpp->SetTextPluginConfigToJsonValue();
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadProjectSettingsFromJson()
{
	unityL10nToolCpp->LoadProjectSettingsFromJson();
}

UnityL10nToolCppCLI::ProjectSettingsCLI^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetProjectSettings()
{
	return gcnew ProjectSettingsCLI(unityL10nToolCpp->GetProjectSettings());
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetProjectSettings(ProjectSettingsCLI^ projectSettingsCLI)
{
	unityL10nToolCpp->SetProjectSettings(projectSettingsCLI->toUnmanaged());
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SaveProjectConfigJson()
{
	return unityL10nToolCpp->SaveProjectConfigJson();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::BuildProject(String ^ buildTargetFolder)
{
	pin_ptr<const wchar_t> ptrbuildTargetFolder = PtrToStringChars(buildTargetFolder);
	return unityL10nToolCpp->BuildProject(ptrbuildTargetFolder);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetPacherConfigJson()
{
	return unityL10nToolCpp->SetPacherConfigJson();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadAssetsReplacer()
{
	return unityL10nToolCpp->LoadAssetsReplacer();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::MakeModifiedAssetsFile()
{
	return unityL10nToolCpp->MakeModifiedAssetsFile();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::CopyResourceFileToGameFolder()
{
	return unityL10nToolCpp->CopyResourceFileToGameFolder();
}

UnityL10nToolCppCLI::TextAssetMapsCLI^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTextAssetMaps()
{
	return gcnew TextAssetMapsCLI(unityL10nToolCpp->GetTextAssetMaps());
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTextAssetMapFromExternalFile(String ^ ExternalRelativeFilePath)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->GetTextAssetMapFromExternalFile(ConvertFromManagedStringToWstring(ExternalRelativeFilePath)));
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetTextAssetMaps(TextAssetMapCLI^ textAssetMapCLI, TextAssetMapCLI::ToWhere toWhereCLI)
{
	TextAssetMap::ToWhere toWhere;
	switch (toWhereCLI) {
	case TextAssetMapCLI::ToWhere::ToInteractWithAsset:
		toWhere = TextAssetMap::ToInteractWithAsset;
		break;
	case TextAssetMapCLI::ToWhere::ToInteractWithFileText:
		toWhere = TextAssetMap::ToInteractWithFileText;
		break;
	case TextAssetMapCLI::ToWhere::ToInteractWithMonoAsset:
		toWhere = TextAssetMap::ToInteractWithMonoAsset;
		break;
	case TextAssetMapCLI::ToWhere::ToDone:
		toWhere = TextAssetMap::ToDone;
		break;
	}
	return unityL10nToolCpp->SetTextAssetMaps(textAssetMapCLI->ToUnmanaged(), toWhere);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadTextPlugins()
{
	return unityL10nToolCpp->LoadTextPlugins();
}

List<String^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetInteractWithAssetPluginNames()
{
	List<String^>^ results = gcnew List<String^>();
	vector<wstring> resultsUnmanaged = unityL10nToolCpp->GetInteractWithAssetPluginNames();
	for (vector<wstring>::iterator iterator = resultsUnmanaged.begin();
		iterator != resultsUnmanaged.end(); iterator++) {
		results->Add(ConvertFromWstringToManagedString(*iterator));
	}
	return results;
}

List<String^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetInteractWithFileTextPluginNames()
{
	List<String^>^ results = gcnew List<String^>();
	vector<wstring> resultsUnmanaged = unityL10nToolCpp->GetInteractWithFileTextPluginNames();
	for (vector<wstring>::iterator iterator = resultsUnmanaged.begin();
		iterator != resultsUnmanaged.end(); iterator++) {
		results->Add(ConvertFromWstringToManagedString(*iterator));
	}
	return results;
}

//String ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetOriginalText(TextAssetMapCLI ^ textAssetMap)
//{
//	return ConvertFromWstringToManagedString(unityL10nToolCpp->GetOriginalText(textAssetMap->ToUnmanaged()));
//	// TODO: 여기에 반환 구문을 삽입합니다.
//}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetOriginalLanguagePairDics(TextAssetMapCLI^ textAssetMapCLI, bool IsFinal)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->GetOriginalLanguagePairDics(textAssetMapCLI->ToUnmanaged(), IsFinal));
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetOriginalLanguagePairDics(TextAssetMapCLI^ textAssetMapCLI)
{
	return GetOriginalLanguagePairDics(textAssetMapCLI, false);
}

ObservableCollection<UnityL10nToolCppCLI::OnlineResourcePairCLI^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetOnlineResourcePairs()
{
	ObservableCollection<OnlineResourcePairCLI^>^ listCLI = gcnew ObservableCollection<OnlineResourcePairCLI^>();
	vector<OnlineResourcePair> list = unityL10nToolCpp->GetOnlineResourcePairs();
	for (vector<OnlineResourcePair>::iterator itr = list.begin(); itr != list.end(); itr++) {
		listCLI->Add(gcnew OnlineResourcePairCLI(*itr));
	}
	return listCLI;
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::AddOnlineResoucesPair(OnlineResourcePairCLI ^ onlineResourcePair)
{
	unityL10nToolCpp->AddOnlineResoucesPair(onlineResourcePair->toUnmanaged());
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetOnlineResourcePairs(ObservableCollection<OnlineResourcePairCLI^>^ onlineResourcePairs)
{
	vector<OnlineResourcePair> list;
	for each(OnlineResourcePairCLI^ orpCLI in onlineResourcePairs) {
		list.push_back(orpCLI->toUnmanaged());
	}
	unityL10nToolCpp->SetOnlineResourcePairs(list);
}

ObservableCollection<UnityL10nToolCppCLI::CopyResourcesManualPairCLI^>^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetCopyResourcesManualPairs()
{
	ObservableCollection<CopyResourcesManualPairCLI^>^ listCLI = gcnew ObservableCollection<CopyResourcesManualPairCLI^>();
	vector<CopyResourcesManualPair> list = unityL10nToolCpp->GetCopyResourcesManualPairs();
	for (vector<CopyResourcesManualPair>::iterator itr = list.begin(); itr != list.end(); itr++) {
		listCLI->Add(gcnew CopyResourcesManualPairCLI(*itr));
	}
	return listCLI;
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::AddCopyResourcesManualPair(CopyResourcesManualPairCLI ^ copyResourcesManualPair)
{
	unityL10nToolCpp->AddCopyResourcesManualPair(copyResourcesManualPair->toUnmanaged());
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetCopyResourcesManualPairs(ObservableCollection<CopyResourcesManualPairCLI^>^ copyResourcesManualPairs)
{
	vector<CopyResourcesManualPair> list;
	for each(CopyResourcesManualPairCLI^ orpCLI in copyResourcesManualPairs) {
		list.push_back(orpCLI->toUnmanaged());
	}
	unityL10nToolCpp->SetCopyResourcesManualPairs(list);
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::CopyResourcesManualPairsForPatcher()
{
	unityL10nToolCpp->CopyResourcesManualPairsForPatcher();
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::DownloadResourcesFromInternetToTempFolder()
{
	unityL10nToolCpp->DownloadResourcesFromInternetToTempFolder();
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::DownloadResourcesFromInternetToBuildResourceFolder()
{
	unityL10nToolCpp->DownloadResourcesFromInternetToBuildResourceFolder();
}

UnityL10nToolCppCLI::OnlineUpdateCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetOnlineUpdate()
{
	return gcnew OnlineUpdateCLI(unityL10nToolCpp->GetOnlineUpdate());
}

void UnityL10nToolCppCLI::UnityL10nToolCppManaged::SetOnlineUpdate(OnlineUpdateCLI ^ onlineUpdate)
{
	unityL10nToolCpp->SetOnlineUpdate(onlineUpdate->toUnmanaged());
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTranslatedText(TextAssetMapCLI ^ textAssetMapCLI, bool IsFinal)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->GetTranslatedText(textAssetMapCLI->ToUnmanaged(),IsFinal));
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTranslatedText(TextAssetMapCLI ^ textAssetMapCLI)
{
	return GetTranslatedText(textAssetMapCLI, false);
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetUpdateFileText(TextAssetMapCLI ^ textAssetMapCLI, bool IsFinal)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->GetUpdateFileText(textAssetMapCLI->ToUnmanaged(),IsFinal));
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetUpdateFileText(TextAssetMapCLI ^ textAssetMapCLI)
{
	return GetUpdateFileText(textAssetMapCLI, false);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::SaveUpdateFileToTempFolder(TextAssetMapCLI ^ textAssetMapCLI)
{
	return unityL10nToolCpp->SaveUpdateFileToTempFolder(textAssetMapCLI->ToUnmanaged());
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::LoadTranslatedFileTextFromTempAndResourceFolder(TextAssetMapCLI ^ textAssetMapCLI)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->LoadTranslatedFileTextFromTempAndResourceFolder(textAssetMapCLI->ToUnmanaged()));
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTranslatedLanguagePairDics(TextAssetMapCLI ^ textAssetMapCLI, bool IsFinal)
{
	return gcnew TextAssetMapCLI(unityL10nToolCpp->GetTranslatedLanguagePairDics(textAssetMapCLI->ToUnmanaged(),IsFinal));
}

UnityL10nToolCppCLI::TextAssetMapCLI ^ UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTranslatedLanguagePairDics(TextAssetMapCLI ^ textAssetMapCLI)
{
	return GetTranslatedLanguagePairDics(textAssetMapCLI, false);
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetTranslatedTextAssetsFromFile()
{
	return unityL10nToolCpp->GetTranslatedTextAssetsFromFile();
}

bool UnityL10nToolCppCLI::UnityL10nToolCppManaged::GetAssetReplacerFromTextAssets()
{
	return unityL10nToolCpp->GetAssetReplacerFromTextAssets();
}

UnityL10nToolCppCLI::UnityL10nToolCppManaged::~UnityL10nToolCppManaged() {
	unityL10nToolCpp->~UnityL10nToolCpp();
	unityL10nToolCpp = NULL;
}

UnityL10nToolCppCLI::FontAssetMapCLI^ UnityL10nToolCppCLI::FontAssetMapCLI::Clone() {
	FontAssetMapCLI^ newFontAssetMapCLI = gcnew FontAssetMapCLI();
	newFontAssetMapCLI->Id = (String^)this->Id->Clone();
	newFontAssetMapCLI->assetsName = (String^)this->assetsName->Clone();
	newFontAssetMapCLI->assetName = (String^)this->assetName->Clone();
	newFontAssetMapCLI->containerPath = (String^)this->containerPath->Clone();
	newFontAssetMapCLI->useContainerPath = this->useContainerPath;
	newFontAssetMapCLI->options = gcnew List<AssetMapOptionCLI^>();
	for each(AssetMapOptionCLI^ child in this->options) {
		newFontAssetMapCLI->options->Add((AssetMapOptionCLI^)child->Clone());
	}
	return newFontAssetMapCLI;
}

UnityL10nToolCppCLI::AssetMapOptionCLI^ UnityL10nToolCppCLI::AssetMapOptionCLI::Clone() {
	AssetMapOptionCLI^ newFontAssetMapOptionCLI = gcnew AssetMapOptionCLI();
	newFontAssetMapOptionCLI->OptionName = (String^)this->OptionName->Clone();
	newFontAssetMapOptionCLI->OptionDescription = (String^)this->OptionDescription->Clone();
	switch (this->type) {
	case Type::OPTION_TYPE_BOOL:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_BOOL;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (bool)this->Value;
		}
		break;
	case Type::OPTION_TYPE_DOUBLE:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_DOUBLE;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (double)this->Value;
		}
		break;
	case Type::OPTION_TYPE_INT:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_INT;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = (int)this->Value;
		}
		break;
	case Type::OPTION_TYPE_STRING:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_STRING;
		if (this->Value) {
			newFontAssetMapOptionCLI->Value = ((String^)this->Value)->Clone();
		}
		break;
	case Type::OPTION_TYPE_NONE:
		newFontAssetMapOptionCLI->type = Type::OPTION_TYPE_NONE;
		break;
	}
	switch (this->typeAsChild) {
	case Type::OPTION_TYPE_BOOL:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_BOOL;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (bool)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_DOUBLE:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_DOUBLE;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (double)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_INT:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_INT;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = (int)this->ValueAsChild;
		}
		break;
	case Type::OPTION_TYPE_STRING:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_STRING;
		if (this->ValueAsChild) {
			newFontAssetMapOptionCLI->ValueAsChild = ((String^)this->ValueAsChild)->Clone();
		}
		break;
	case Type::OPTION_TYPE_NONE:
		newFontAssetMapOptionCLI->typeAsChild = Type::OPTION_TYPE_NONE;
		break;
	}
	newFontAssetMapOptionCLI->nestedOptions = gcnew List<AssetMapOptionCLI^>();
	if (this->nestedOptions) {
		for each(AssetMapOptionCLI^ child in this->nestedOptions) {
			newFontAssetMapOptionCLI->nestedOptions->Add((AssetMapOptionCLI^)child->Clone());
		}
	}
	return newFontAssetMapOptionCLI;
}
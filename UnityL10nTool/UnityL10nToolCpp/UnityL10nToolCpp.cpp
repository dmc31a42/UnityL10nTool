#include "stdafx.h"
#include "UnityL10nToolCpp.h"

#include <string>
#include <map>

#include "GeneralPurposeFunctions.h"
#include "IULTFontPluginInterface.h"

using namespace std;

UnityL10nToolCpp::UnityL10nToolCpp(wstring gameFolderPath)
{
	FirstAssetsFileName = "globalgamemanagers";
	/* gameFolderPath should end by \ */
	if (gameFolderPath.back() != L'\\') {
		GameFolderPath = gameFolderPath + L'\\';
	} else {
		GameFolderPath = gameFolderPath;
	}

	wchar_t WcharCurrentDirectory[255] = {};
	_wgetcwd(WcharCurrentDirectory, 255);
	CurrentDirectory = WcharCurrentDirectory;
	CurrentDirectory += L"\\";

	string projectJsonStr = readFile2(CurrentDirectory + L"project.json");
	JsonReader.parse(projectJsonStr, projectJson);
	
	if (CreateDirectory(L".\\temp\\", NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
	}
}

bool UnityL10nToolCpp::LoadGlobalgamemanagersFile() {
	return LoadAssetsFile(FirstAssetsFileName);
}

bool UnityL10nToolCpp::LoadAssetsFile(std::string assetsFileName) {
	map<string, AssetsFile*>::iterator iterator = FindAssetsFilesFromAssetsName.find(assetsFileName);
	if (iterator == FindAssetsFilesFromAssetsName.end()) {
		string assetsFilePath = assetsFileName;
		ReplaceAll(assetsFilePath, "library/", "resources/");
		IAssetsReader* assetsReader = Create_AssetsReaderFromFile((GameFolderPath + WideMultiStringConverter.from_bytes(assetsFilePath)).c_str(), true, RWOpenFlags_None);
		AssetsFile* assetsFile = new AssetsFile(assetsReader);
		AssetsFileTable* assetsFileTable = new AssetsFileTable(assetsFile);
		assetsFileTable->GenerateQuickLookupTree();
		FindAssetsFilesFromAssetsName.insert(pair<string, AssetsFile*>(assetsFileName, assetsFile));
		FindAssetsFileTablesFromAssetsName.insert(pair<string, AssetsFileTable*>(assetsFileName, assetsFileTable));
		AssetsFileNames.push_back(assetsFileName);
		if (assetsFileName == "globalgamemanagers") {
			GlobalgamemanagersAssetsTable = assetsFileTable;
			version = assetsFile->typeTree.unityVersion;
			size_t lastDotOffset = version.find_last_of('.');
			versionFirstTwoNumbers = version.substr(0, lastDotOffset);
			LoadBasicClassDatabase();
			//ProcessResourceAndMonoManger(assetsFileTable, assetsFileName);
		}
		else if (assetsFileName == "globalgamemanagers.assets") {
			//LoadFindMonoClassNameFromMonoScriptPathId(assetsFileTable);
		}
		DWORD dependencyCount = assetsFile->dependencies.dependencyCount;
		if (dependencyCount > 0) {
			for (DWORD i = 0; i < dependencyCount; i++) {
				string newAssetsFileName = assetsFile->dependencies.pDependencies[i].assetPath;
				LoadAssetsFile(newAssetsFileName);
			}
			return true;
		}
		else {
			return true;
		}
	}
	else {
		return true;
	}
}

bool UnityL10nToolCpp::LoadBasicClassDatabase() {
	wstring filter = L"ClassDatabase\\U" + WideMultiStringConverter.from_bytes(versionFirstTwoNumbers) + L".*.dat";
	vector<wstring> classDatabasePathList = get_all_files_names_within_folder(filter);
	wstring classDatabaseFileName = classDatabasePathList[0];
	IAssetsReader* classDatabaseReader = Create_AssetsReaderFromFile((L"ClassDatabase\\" + classDatabaseFileName).c_str(), true, RWOpenFlags_None);
	BasicClassDatabaseFile = new ClassDatabaseFile();
	BasicClassDatabaseFile->Read(classDatabaseReader);
	for (size_t i = 0; i < BasicClassDatabaseFile->classes.size(); i++)
	{
		int classid = BasicClassDatabaseFile->classes[i].classId;
		FindBasicClassIndexFromClassID.insert(map<int, unsigned int>::value_type(classid, (unsigned int)i));
		const char* classDatabaseTypeName = BasicClassDatabaseFile->classes[i].name.GetString(BasicClassDatabaseFile);
		FindBasicClassIndexFromClassName.insert(map<string, UINT32>::value_type(classDatabaseTypeName, (UINT32)i));
	}
	return true;
}

bool UnityL10nToolCpp::ProcessResourceAndMonoManger() {
	string globalgamemanagersName = "globalgamemanagers";
	AssetsFileTable* globalgamemanagersTable = FindAssetsFileTablesFromAssetsName[globalgamemanagersName];
	AssetsFile* globalgamemanagersFile = globalgamemanagersTable->getAssetsFile();
	int ResourceManagerClassId;
	int MonoManagerClassId;

	FindPathIDOfContainerPathFromAssetsName.insert(pair<string, INT32>("globalgamemanagers", 0));
	FindAssetsNameFromPathIDOfContainerPath.insert(pair<INT32, string>(0, "globalgamemanagers"));
	for (INT32 i = 0; i < GlobalgamemanagersAssetsTable->getAssetsFile()->dependencies.dependencyCount; i++) {
		string dependencyAssetsPath = string(GlobalgamemanagersAssetsTable->getAssetsFile()->dependencies.pDependencies[i].assetPath);
		FindPathIDOfContainerPathFromAssetsName.insert(pair<string, INT32>(dependencyAssetsPath, i+1));
		FindAssetsNameFromPathIDOfContainerPath.insert(pair<INT32, string>(i + 1, dependencyAssetsPath));
	}
	ResourceManagerClassId = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName["ResourceManager"]].classId;
	MonoManagerClassId = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName["MonoManager"]].classId;
	int AssetSearchCount = 2;
	for (unsigned int i = 0;
		(i < globalgamemanagersTable->assetFileInfoCount) && AssetSearchCount > 0;
		i++) {
		AssetFileInfoEx* assetFileInfoEx = &globalgamemanagersTable->pAssetFileInfo[i];
		int classId;
		WORD monoId;
		GetClassIdFromAssetFileInfoEx(GlobalgamemanagersAssetsTable, assetFileInfoEx, classId, monoId);
		if (classId == ResourceManagerClassId) {
			//assetFileInfoEx.absolutePos
			ResourceManagerFileGlobal = new ResourceManagerFile();
			std::ifstream ifsGlobalgamemanagers(GameFolderPath + WideMultiStringConverter.from_bytes(globalgamemanagersName), std::ios::binary | std::ios::ate);
			ifsGlobalgamemanagers.seekg(assetFileInfoEx->absolutePos, std::ios::beg);

			std::vector<char> resourceManagerBuffer(assetFileInfoEx->curFileSize);
			if (ifsGlobalgamemanagers.read(resourceManagerBuffer.data(), assetFileInfoEx->curFileSize))
			{
				/* worked! */
			}
			int* resourceManagerFilePos = new int(0);
			ResourceManagerFileGlobal->Read(
				(void*)resourceManagerBuffer.data(),
				assetFileInfoEx->curFileSize,
				resourceManagerFilePos,
				globalgamemanagersFile->header.format,
				globalgamemanagersFile->header.endianness ? true : false);
			ResourceManager_PPtr resourceManager_PPtr;
			for (int i = 0; i < ResourceManagerFileGlobal->containerArrayLen; i++) {
				resourceManager_PPtr = ResourceManagerFileGlobal->containerArray[i].ids;
				pair<INT32, INT64> tempFilePathID = pair<INT32, INT64>(resourceManager_PPtr.fileId, resourceManager_PPtr.pathId);
				FindContainerPathFromFileIDPathID.insert(pair<pair<INT32, INT64>, string>(tempFilePathID, ResourceManagerFileGlobal->containerArray[i].name));
				FindFileIDPathIDFromContainerPath.insert(pair<string, pair<INT32, INT64>>(ResourceManagerFileGlobal->containerArray[i].name, tempFilePathID));
			}
			AssetSearchCount--;
		}
		else if (classId == MonoManagerClassId) {
			AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
			baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID[classId]], (DWORD)0, false);
			AssetTypeInstance baseAssetTypeInstance(
				(DWORD)1,
				&baseAssetTypeTemplateField,
				assetFileInfoEx->curFileSize,
				globalgamemanagersTable->getReader(),
				globalgamemanagersFile->header.endianness ? true : false,
				assetFileInfoEx->absolutePos);
			AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance.GetBaseField();
			if (baseAssetTypeValueField) {
				AssetTypeValueField* m_ScriptsArrayATVF =
					baseAssetTypeValueField->Get("m_Scripts")->Get("Array");
				if (m_ScriptsArrayATVF) {
					AssetTypeValueField** m_ScriptsChildrenListATVF = m_ScriptsArrayATVF->GetChildrenList();
					int classId = 0;
					UINT16 monoClassId;

					for (DWORD i = 0; i < m_ScriptsArrayATVF->GetChildrenCount(); i++) {
						INT32 m_FileID = m_ScriptsChildrenListATVF[i]->Get("m_FileID")->GetValue()->AsInt();
						INT64 m_PathID = m_ScriptsChildrenListATVF[i]->Get("m_PathID")->GetValue()->AsInt64();
						string assetsName = string(globalgamemanagersFile->dependencies.pDependencies[m_FileID - 1].assetPath);
						AssetsFileTable* assetsFileTable = FindAssetsFileTablesFromAssetsName[assetsName];
						AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
						AssetFileInfoEx* assetFileInfoEx = assetsFileTable->getAssetInfo(m_PathID);
						if (classId == 0) {
							GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
						}
						AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
						baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID[classId]], (DWORD)0, false);
						AssetTypeInstance baseAssetTypeInstance(
							(DWORD)1,
							&baseAssetTypeTemplateField,
							assetFileInfoEx->curFileSize,
							assetsFileTable->getReader(),
							assetsFile->header.endianness ? true : false,
							assetFileInfoEx->absolutePos);
						AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance.GetBaseField();
						if (baseAssetTypeValueField) {
							AssetTypeValueField* m_ClassNameATVF = baseAssetTypeValueField->Get("m_ClassName");
							AssetTypeValueField* m_NamespaceATVF = baseAssetTypeValueField->Get("m_Namespace");
							if (m_ClassNameATVF && m_NamespaceATVF) {
								string monoScriptFullName = string(m_NamespaceATVF->GetValue()->AsString()) + "." + m_ClassNameATVF->GetValue()->AsString();
								FindMonoClassNameFromAssetsNameANDPathId.insert(pair<pair<string, INT64>, string>(pair<string, INT64>(assetsName, assetFileInfoEx->index), monoScriptFullName));
							}
						}
					}
				}

				AssetTypeValueField* m_AssemblyNamesArrayATVF =
					baseAssetTypeValueField->Get("m_AssemblyNames")->Get("Array");
				if (m_AssemblyNamesArrayATVF) {
					AssetTypeValueField** m_AssemblyNamesChildrenListATVF = m_AssemblyNamesArrayATVF->GetChildrenList();
					for (DWORD i = 0; i < m_AssemblyNamesArrayATVF->GetChildrenCount(); i++) {
						AssemblyNames.push_back(m_AssemblyNamesChildrenListATVF[i]->GetValue()->AsString());
					}
				}
			}
			AssetSearchCount--;
		}
	}
	
	return true;
}

void UnityL10nToolCpp::GetClassIdFromAssetFileInfoEx(AssetsFileTable* assetsFileTable, AssetFileInfoEx* assetFileInfoEx, int& classId, UINT16& monoClassId) {
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

bool CreateTypeTreeGenerator(wstring TypeTreeGeneratorParams) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(
		L".\\Resource\\TypeTreeGenerator.exe",   // No module name (use command line)
		(LPWSTR)TypeTreeGeneratorParams.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		L".\\temp\\",           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return true;
}

bool UnityL10nToolCpp::LoadMonoClassDatabase() {
	wstring classDatabaseFileName = L".\\temp\\behaviourdb.dat";
	_wremove(classDatabaseFileName.c_str());

	wstring TypeTreeGeneratorParams;
	for (vector<string>::iterator iterator = AssemblyNames.begin(); iterator != AssemblyNames.end(); iterator++) {
		if (!(iterator->empty())) {
			TypeTreeGeneratorParams += L"-f \"" + GameFolderPath + L"Managed\\" + WideMultiStringConverter.from_bytes(*iterator) + L"\" ";
		}
	}
	//TypeTreeGeneratorParams += " 2>&1 > baseList.txt";
	//CreateProcessCustom(L".\\Resource\\TypeTreeGenerator.exe " + TypeTreeGeneratorParams);
	// behaviourdb.dat

	CreateTypeTreeGenerator(TypeTreeGeneratorParams);

	IAssetsReader* classDatabaseReader = Create_AssetsReaderFromFile((classDatabaseFileName).c_str(), true, RWOpenFlags_None);
	MonoClassDatabaseFile = new ClassDatabaseFile();
	MonoClassDatabaseFile->Read(classDatabaseReader);
	for (size_t i = 0; i < MonoClassDatabaseFile->classes.size(); i++)
	{
		string monoClassDatabaseTypeName = string(MonoClassDatabaseFile->classes[i].name.GetString(MonoClassDatabaseFile));
		FindMonoClassIndexFromMonoClassName.insert(map<string, unsigned int>::value_type(monoClassDatabaseTypeName, (unsigned int)i));
	}
	return true;
}

//bool UnityL10nToolCpp::LoadFindMonoClassNameFromMonoScriptPathId(AssetsFileTable* globalgamemanagersAssetsTable) {
//	AssetsFile* globalgamemanagersAssetsFile = globalgamemanagersAssetsTable->getAssetsFile();
//	int MonoScriptClassId = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName["MonoScript"]].classId;
//	for (unsigned int i = 0; i < globalgamemanagersAssetsTable->assetFileInfoCount; i++) {
//		int classId;
//		UINT16 monoClassId;
//		AssetFileInfoEx* assetFileInfoEx = &globalgamemanagersAssetsTable->pAssetFileInfo[i];
//		GetClassIdFromAssetFileInfoEx(globalgamemanagersAssetsTable, assetFileInfoEx, classId, monoClassId);
//		if (classId == MonoScriptClassId) {
//			AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
//			baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID[classId]], (DWORD)0, false);
//			AssetTypeInstance baseAssetTypeInstance(
//				(DWORD)1,
//				&baseAssetTypeTemplateField,
//				assetFileInfoEx->curFileSize,
//				globalgamemanagersAssetsTable->getReader(),
//				globalgamemanagersAssetsFile->header.endianness ? true : false,
//				assetFileInfoEx->absolutePos);
//			AssetTypeValueField* baseAssetTypeValueField = baseAssetTypeInstance.GetBaseField();
//			if (baseAssetTypeValueField) {
//				AssetTypeValueField* m_ClassNameATVF = baseAssetTypeValueField->Get("m_ClassName");
//				AssetTypeValueField* m_NamespaceATVF = baseAssetTypeValueField->Get("m_Namespace");
//				if (m_ClassNameATVF && m_NamespaceATVF) {
//					string monoScriptFullName = string(m_NamespaceATVF->GetValue()->AsString()) + "." + m_ClassNameATVF->GetValue()->AsString();
//					FindMonoClassNameFromAssetsNameANDPathId.insert(pair<INT64, string>(assetFileInfoEx->index, monoScriptFullName));
//				}
//			}
//		}
//	}
//	return true;
//}

bool UnityL10nToolCpp::LoadUnityL10nToolAPI() {
	_unityL10nToolAPI.version = version;
	_unityL10nToolAPI.versionFirstTwoNumbers = versionFirstTwoNumbers;
	_unityL10nToolAPI.BasicClassDatabaseFile = BasicClassDatabaseFile;
	_unityL10nToolAPI.MonoClassDatabaseFile = MonoClassDatabaseFile;
	_unityL10nToolAPI.ResourceManagerFileGlobal = ResourceManagerFileGlobal;
	_unityL10nToolAPI.GlobalgamemanagersAssetsTable = GlobalgamemanagersAssetsTable;
	_unityL10nToolAPI.AssetsFileNames = &AssetsFileNames;
	_unityL10nToolAPI.FindAssetsFilesFromAssetsName = &FindAssetsFilesFromAssetsName;
	_unityL10nToolAPI.FindAssetsFileTablesFromAssetsName = &FindAssetsFileTablesFromAssetsName;
	_unityL10nToolAPI.FindBasicClassIndexFromClassID = &FindBasicClassIndexFromClassID;
	_unityL10nToolAPI.FindBasicClassIndexFromClassName = &FindBasicClassIndexFromClassName;
	_unityL10nToolAPI.FindMonoClassNameFromAssetsNameANDPathId = &FindMonoClassNameFromAssetsNameANDPathId;
	_unityL10nToolAPI.FindMonoClassIndexFromMonoClassName = &FindMonoClassIndexFromMonoClassName;
	_unityL10nToolAPI.FindContainerPathFromFileIDPathID = &FindContainerPathFromFileIDPathID;
	_unityL10nToolAPI.FindFileIDPathIDFromContainerPath = &FindFileIDPathIDFromContainerPath;
	_unityL10nToolAPI.FindPathIDOfContainerPathFromAssetsName = &FindPathIDOfContainerPathFromAssetsName;
	_unityL10nToolAPI.FindAssetsNameFromPathIDOfContainerPath = &FindAssetsNameFromPathIDOfContainerPath;
	return true;
}

vector<wstring> UnityL10nToolCpp::LoadFontPlugins() {
	vector<wstring> pluginLoadedList;
	vector<wstring> PluginFileNameList = GetAllFilesFilterWithinAllSubFolder(CurrentDirectory + L"Plugins\\FontPlugins\\", L"*.ULTFontPlugin");
	for (vector<wstring>::iterator iterator = PluginFileNameList.begin();
		iterator != PluginFileNameList.end(); iterator++) {
		HINSTANCE PluginHInstance = LoadLibraryW((CurrentDirectory + L"Plugins\\FontPlugins\\" + *iterator).c_str());
		GetFontPluginInfoCallback getFontPluginInfoCallback;
		if (PluginHInstance) {
			getFontPluginInfoCallback = (GetFontPluginInfoCallback)GetProcAddress(PluginHInstance, "GetFontPluginInfo");
			if (getFontPluginInfoCallback) {
				FontPluginInfo* fontPluginInfo = new FontPluginInfo();
				wstring relativePluginPath = L"Plugins\\FontPlugins\\" + *iterator;
				size_t lastBackslash = relativePluginPath.find_last_of('\\');
				fontPluginInfo->relativePluginPath = relativePluginPath.replace(lastBackslash, relativePluginPath.size() - lastBackslash, L"") + L"\\";
				// It must use actual Json::Value
				Json::Value tempJson;
				bool result = getFontPluginInfoCallback(&_unityL10nToolAPI, fontPluginInfo);
				if (result) {
					FontPluginMap.insert(pair<wstring, HINSTANCE>(fontPluginInfo->FontPluginName, PluginHInstance));
					FontPluginInfoMap.insert(pair<wstring, FontPluginInfo*>(fontPluginInfo->FontPluginName, fontPluginInfo));
					wstring pluginName = fontPluginInfo->FontPluginName;
					pluginLoadedList.push_back(pluginName);
					string pluginNameA = WideMultiStringConverter.to_bytes(pluginName);
					if (projectJson["FontPlugin"].isMember(pluginNameA)) {
						fontPluginInfo->SetProjectConfigJson(projectJson["FontPlugin"][pluginNameA]);
					}
					else {
						fontPluginInfo->SetProjectConfigJson(Json::Value());
					}
				}
				else {
					FreeLibrary(PluginHInstance);
				}
			}
			else {
				FreeLibrary(PluginHInstance);
				continue;
			}
		}
		else {
			FreeLibrary(PluginHInstance);
			continue;
		}
	}
	return pluginLoadedList;
}

map<wstring, FontAssetMaps> UnityL10nToolCpp::GetPluginsSupportAssetMap() {
	map<wstring, FontAssetMaps> supportAssetMaps;
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		FontAssetMaps pluginSupportAssetMaps = fontPluginInfo->GetPluginSupportAssetMap();
		supportAssetMaps[fontPluginInfo->FontPluginName] = pluginSupportAssetMaps;
	}
	return supportAssetMaps;
}

UnityL10nToolCpp::~UnityL10nToolCpp()
{
	BasicClassDatabaseFile->~ClassDatabaseFile();
	BasicClassDatabaseFile = NULL;
	MonoClassDatabaseFile->~ClassDatabaseFile();
	MonoClassDatabaseFile = NULL;
	ResourceManagerFileGlobal->~ResourceManagerFile();
	ResourceManagerFileGlobal = NULL;
	for (map<string, AssetsFileTable*>::iterator iterator = FindAssetsFileTablesFromAssetsName.begin();
		iterator != FindAssetsFileTablesFromAssetsName.end(); iterator++) {
		iterator->second->~AssetsFileTable();
	}
	FindAssetsFileTablesFromAssetsName.clear();
	for (map<string, AssetsFile*>::iterator iterator = FindAssetsFilesFromAssetsName.begin();
		iterator != FindAssetsFilesFromAssetsName.end(); iterator++) {
		iterator->second->~AssetsFile();
	}
	FindAssetsFilesFromAssetsName.clear();
	GlobalgamemanagersAssetsTable = NULL;
}

bool UnityL10nToolCpp::DetermineUnityGameFolder(wstring path)
{
	path = MakeSureBackslashEndOfFolderPath(path);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFileW((path + L"globalgamemanagers").c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			::FindClose(hFind);
			return true;
		}
	}
	::FindClose(hFind);
	return false;
}

void GetHardDiskDrivesNames(vector<wstring> &strIPAddresses)
{
	DWORD dwBufferSize = GetLogicalDriveStrings(0, NULL);
	wchar_t* pDrives = (wchar_t *)malloc(dwBufferSize + 1);
	wchar_t* pDrivesToDelete = pDrives;
	if (pDrives != NULL)
	{
		GetLogicalDriveStrings(dwBufferSize, pDrives);
		while (*pDrives)
		{
			UINT nDriveType = GetDriveType(pDrives);

			if (DRIVE_FIXED == nDriveType)
			{
				strIPAddresses.push_back(wstring(pDrives));
			}
			pDrives += lstrlen(pDrives) + 1;
		}

		free(pDrivesToDelete);
		pDrivesToDelete = NULL;
	}
}

wstring FindUnityGameFolderFromDataFolderNameInternal2(wstring path, wstring GameName, wstring MakerName) {
	WIN32_FIND_DATA fdAppInfo;
	HANDLE hFindAppInfo = ::FindFirstFileW((path + L"app.info").c_str(), &fdAppInfo);
	if (hFindAppInfo != INVALID_HANDLE_VALUE) {
		string str = readFile2(path + L"app.info");
		wstring wstr = WideMultiStringConverter.from_bytes(str);
		int index = wstr.find('\n');
		if (index != -1) {
			wstring tempMakerName = wstr.substr(0, index);
			wstring tempGameName = wstr.substr(index+1, str.length() - index-1);
			if (GameName == tempGameName && MakerName == tempMakerName) {
				::FindClose(hFindAppInfo);
				return path;
			}
		}
	}
	::FindClose(hFindAppInfo);
	return L"";
}

wstring FindUnityGameFolderFromDataFolderNameInternal1(wstring path, wstring DataFolderPath, wstring GameName, wstring MakerName) {
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFileW((path + L"*").c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if ((!lstrcmpW(fd.cFileName, L".")) || (!lstrcmpW(fd.cFileName, L".."))) {
					continue;
				}
				wstring tempPath = FindUnityGameFolderFromDataFolderNameInternal2((path + fd.cFileName + L"\\" + DataFolderPath + L"_Data\\").c_str(), GameName, MakerName);
				if(!tempPath.empty()){
					::FindClose(hFind);
					return tempPath;
				}
			}
		} while (FindNextFileW(hFind, &fd));
		::FindClose(hFind);
	}
	return L"";
}

bool UnityL10nToolCpp::DetermineProjectGamePath(wstring path, wstring GameName, wstring MakerName) {
	
	wstring tempPath = FindUnityGameFolderFromDataFolderNameInternal2(path, GameName, MakerName);
	return !tempPath.empty() ? true : false;
}

wstring UnityL10nToolCpp::FindUnityGameFolderFromDataFolderName(wstring dataFolderName, wstring GameName, wstring MakerName)
{
	wstring result = L"";
	wstring steamRelativePath = L"Steam\\steamapps\\common\\";
	wstring steamRelativePath2 = L"SteamLibrary\\steamapps\\common\\";
	wstring programFilesx86Path = L"Program Files (x86)\\";
	wstring programFilesx64Path = L"Program Files\\";
	vector<wstring> diskDriveNames;
	GetHardDiskDrivesNames(diskDriveNames);
	//get_all_files_names_within_folder();

	FindUnityGameFolderFromDataFolderNameInternal2(L".\\", GameName, MakerName);
	if (!result.empty()) {
		return result;
	}
	FindUnityGameFolderFromDataFolderNameInternal2(L"..\\", GameName, MakerName);
	if (!result.empty()) {
		return result;
	}
	FindUnityGameFolderFromDataFolderNameInternal2(L"..\\" + dataFolderName + L"_Data\\", GameName, MakerName);
	if (!result.empty()) {
		return result;
	}
	FindUnityGameFolderFromDataFolderNameInternal1(L"..\\", dataFolderName, GameName, MakerName);
	if (!result.empty()) {
		return result;
	}
	for (vector<wstring>::iterator iterator = diskDriveNames.begin();
		iterator != diskDriveNames.end(); iterator++) {
		result = FindUnityGameFolderFromDataFolderNameInternal1(*iterator + programFilesx86Path + steamRelativePath, dataFolderName, GameName, MakerName);
		if (!result.empty()) {
			return result;
		}
		result = FindUnityGameFolderFromDataFolderNameInternal1(*iterator + programFilesx64Path + steamRelativePath, dataFolderName, GameName, MakerName);
		if (!result.empty()) {
			return result;
		}
		result = FindUnityGameFolderFromDataFolderNameInternal1(*iterator + steamRelativePath, dataFolderName, GameName, MakerName);
		if (!result.empty()) {
			return result;
		}
		result = FindUnityGameFolderFromDataFolderNameInternal1(*iterator + steamRelativePath, dataFolderName, GameName, MakerName);
		if (!result.empty()) {
			return result;
		}
		result = FindUnityGameFolderFromDataFolderNameInternal1(*iterator + steamRelativePath2, dataFolderName, GameName, MakerName);
		if (!result.empty()) {
			return result;
		}
	}
	return L"";
}

wstring UnityL10nToolCpp::MakeSureBackslashEndOfFolderPath(wstring path)
{
	if (path.back() != L'\\') {
		return path + L'\\';
	}
	else {
		return path;
	}
}

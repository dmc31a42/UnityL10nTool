#include "stdafx.h"
#include "UnityL10nToolCpp.h"

#include <string>
#include <map>
#include <fstream>
#include <sstream>

#include <iostream>
#include <locale>
#include <codecvt>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
#include "GeneralPurposeFunctions.h"
#include "IULTPluginCommonInterface.h"
#include "IULTFontPluginInterface.h"
#include "log_util.h"

using namespace std;

UnityL10nToolCpp::UnityL10nToolCpp(wstring projectJsonFolderPath, wstring gameFolderPath)
{
	LOGsetInfo(".", "UnityL10nTool");
	LOGsetLevel(LOG_LVL_TRACE);

	//FirstAssetsFileName = "globalgamemanagers";
	ProjectJsonFolderPath = MakeSureBackslashEndOfFolderPath(projectJsonFolderPath);
	//LOG_TRACE("ProjectJsonFolderPath: %ls", ProjectJsonFolderPath);
	/* gameFolderPath should end by \ */
	/*if (gameFolderPath.back() != L'\\') {
		GameFolderPath = gameFolderPath + L'\\';
	} else {
		GameFolderPath = gameFolderPath;
	}*/
	
	wchar_t WcharCurrentDirectory[255] = {};
	_wgetcwd(WcharCurrentDirectory, 255);
	CurrentDirectory = WcharCurrentDirectory;
	CurrentDirectory += L"\\";

	UnityL10nToolProjectInfoGlobal.JSONPath = ProjectJsonFolderPath + L"setting.json";
	UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder = ProjectJsonFolderPath;
	string projectJsonStr = readFile2(UnityL10nToolProjectInfoGlobal.JSONPath);
	//JsonReader->parse(projectJsonStr, projectJson);
	JsonParseFromString(projectJsonStr, projectJson);
	UnityL10nToolProjectInfoGlobal.GameName = WideMultiStringConverter->from_bytes(projectJson["GameName"].asString());
	UnityL10nToolProjectInfoGlobal.MakerName = WideMultiStringConverter->from_bytes(projectJson["MakerName"].asString());
	UnityL10nToolProjectInfoGlobal.GamePath = MakeSureBackslashEndOfFolderPath(WideMultiStringConverter->from_bytes(projectJson["GamePath"].asString()));
	UnityL10nToolProjectInfoGlobal.DataFolderName = WideMultiStringConverter->from_bytes(projectJson["DataFolderName"].asString());
	
	if (gameFolderPath != L"" && DetermineProjectGamePath(
		gameFolderPath,
		UnityL10nToolProjectInfoGlobal.GameName,
		UnityL10nToolProjectInfoGlobal.MakerName)) {
		UnityL10nToolProjectInfoGlobal.GamePath = gameFolderPath;
		projectJson["GamePath"] = WideMultiStringConverter->to_bytes(gameFolderPath);
	}
	else if (DetermineProjectGamePath(
		UnityL10nToolProjectInfoGlobal.GamePath,
		UnityL10nToolProjectInfoGlobal.GameName,
		UnityL10nToolProjectInfoGlobal.MakerName)) {
	}
	else {
		wstring tempGamePath = FindUnityGameFolderFromDataFolderName(UnityL10nToolProjectInfoGlobal.DataFolderName, UnityL10nToolProjectInfoGlobal.GameName, UnityL10nToolProjectInfoGlobal.MakerName);
		if (tempGamePath.empty()) {
			throw new std::exception("Cannot Find Game Folder");
		}
		else {
			UnityL10nToolProjectInfoGlobal.GamePath = tempGamePath;
			projectJson["GamePath"] = WideMultiStringConverter->to_bytes(tempGamePath);
		}
	}
	GameFolderPath = UnityL10nToolProjectInfoGlobal.GamePath;
	FirstAssetsFileName = WideMultiStringConverter->to_bytes(DetermineUnityGameFolder(GameFolderPath));
	if (CreateDirectory(L".\\temp\\", NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
	}

	if (projectJson.isMember("OnlineUpdate")) {
		OnlineUpdateGlobal = OnlineUpdate(projectJson["OnlineUpdate"]);
	}
	
}

bool UnityL10nToolCpp::LoadGlobalgamemanagersFile() {
	return LoadAssetsFile(FirstAssetsFileName);
}

wstring UnityL10nToolCpp::NewGameProjectFromFolder(wstring folder) {
	folder = MakeSureBackslashEndOfFolderPath(folder);
	wstring appInfoFileText;
	wifstream wif(folder + L"app.info");
	if (wif.good()) {
		wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
		std::wstringstream wss;
		wss << wif.rdbuf();
		appInfoFileText = wss.str();
	}
	else {
		vector<wstring> folderList = GetAllFolderName(folder);
		for (vector<wstring>::iterator iterator = folderList.begin();
			iterator != folderList.end(); iterator++) {
			size_t index = iterator->find(L"_Data");
			if (index != std::wstring::npos) {
				wstring tempfolder = MakeSureBackslashEndOfFolderPath(folder + *iterator);
				wifstream wif2(tempfolder + L"app.info");
				if (wif2.good()) {
					wif2.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
					std::wstringstream wss;
					wss << wif2.rdbuf();
					appInfoFileText = wss.str();
					folder = tempfolder;
					break;
				}
				else {
					wstring refGameName;
					wstring refMakerName;
					if (GetGameMakerNameFromGlobalgamemanager(tempfolder, refGameName, refMakerName)) {
						appInfoFileText = refMakerName + L"\n" + refGameName;
						break;
					}
				}
			}
		}
		wstring refGameName;
		wstring refMakerName;
		if (GetGameMakerNameFromGlobalgamemanager(folder, refGameName, refMakerName)) {
			appInfoFileText = refMakerName + L"\n" + refGameName;
		}
	}
	if (appInfoFileText == L"") {
		return L"";
	}
	int index = appInfoFileText.find('\n');
	if (index == std::wstring::npos) {
		return L"";
	}
	wstring tempMakerName = appInfoFileText.substr(0, index);
	wstring tempGameName = appInfoFileText.substr(index + 1, appInfoFileText.length() - index - 1);
	int backslashIndex = folder.find_last_of(L"\\", folder.size() - 2);
	if (backslashIndex == std::wstring::npos) {
		return L"";
	}
	wstring tempGamePath = folder.substr(0, backslashIndex);
	wstring tempDataFolderName = folder.substr(backslashIndex + 1);
	tempDataFolderName = ReplaceAll(tempDataFolderName, L"_Data\\", L"");
	Json::Value json;
	json["GameName"] = WideMultiStringConverter->to_bytes(tempGameName);
	json["MakerName"] = WideMultiStringConverter->to_bytes(tempMakerName);
	json["GamePath"] = WideMultiStringConverter->to_bytes(folder);
	json["DataFolderName"] = WideMultiStringConverter->to_bytes(tempDataFolderName);
	wchar_t WcharCurrentDirectory[255] = {};
	_wgetcwd(WcharCurrentDirectory, 255);
	wstring tempCurrentDirectory = WcharCurrentDirectory;
	tempCurrentDirectory += L"\\";

	wstring ProjectDirectory = tempGameName;
	const wchar_t PathSpecialCharactor[] = { L'\\', L'/', L':', L'*', L'?', L'"', L'<', L'>', L'|'};
	RemoveAll(ProjectDirectory, PathSpecialCharactor, sizeof(PathSpecialCharactor)/sizeof(PathSpecialCharactor[0]));

	if (!DirExists(tempCurrentDirectory + L"Projects\\")) {
		CreateDirectory((tempCurrentDirectory + L"Projects\\").c_str(), NULL);
		CreateDirectory((tempCurrentDirectory + L"Projects\\Resources").c_str(), NULL);
	}
	if (DirExists(tempCurrentDirectory + L"Projects\\" + ProjectDirectory + L"\\")) {
		return L"";
	}
	if (CreateDirectory((tempCurrentDirectory + L"Projects\\" + ProjectDirectory + L"\\").c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
	}
	std::wofstream wof;
	wof.clear();
	wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	wof.open(tempCurrentDirectory + L"Projects\\" + ProjectDirectory + L"\\setting.json");
	wof << WideMultiStringConverter->from_bytes(json.toStyledString());
	wof.close();
	return tempCurrentDirectory + L"Projects\\" + ProjectDirectory + L"\\";
}

bool UnityL10nToolCpp::LoadAssetsFile(std::string assetsFileName) {
	map<string, AssetsFile*>::iterator iterator = FindAssetsFilesFromAssetsName.find(assetsFileName);
	if (iterator == FindAssetsFilesFromAssetsName.end()) {
		string assetsFilePath = assetsFileName;
		ReplaceAll(assetsFilePath, "library/", "resources/");
		IAssetsReader* assetsReader = Create_AssetsReaderFromFile((GameFolderPath + WideMultiStringConverter->from_bytes(assetsFilePath)).c_str(), true, RWOpenFlags_None);
		AssetsFile* assetsFile = new AssetsFile(assetsReader);
		AssetsFileTable* assetsFileTable = new AssetsFileTable(assetsFile);
		assetsFileTable->GenerateQuickLookupTree();
		FindAssetsFilesFromAssetsName.insert(pair<string, AssetsFile*>(assetsFileName, assetsFile));
		FindAssetsFileTablesFromAssetsName.insert(pair<string, AssetsFileTable*>(assetsFileName, assetsFileTable));
		FindAssetsNameFromAssetsFileTables.insert(pair<AssetsFileTable*, string>(assetsFileTable, assetsFileName));
		FindAssetsNameFromAssetsFiles.insert(pair<AssetsFile*, string>(assetsFile, assetsFileName));
		AssetsFileNames.push_back(assetsFileName);
		if (assetsFileName == FirstAssetsFileName) {
			GlobalgamemanagersAssetsTable = assetsFileTable;
			version = assetsFile->typeTree.unityVersion;
			size_t lastDotOffset = version.find_last_of('.');
			versionFirstTwoNumbers = version.substr(0, lastDotOffset);
			size_t majorDotMinorOffser = versionFirstTwoNumbers.find_last_of('.');
			MajorVersion = atoi(versionFirstTwoNumbers.substr(0, majorDotMinorOffser).c_str());
			MinorVersion = atoi(versionFirstTwoNumbers.substr(majorDotMinorOffser + 1).c_str());
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
	wstring filter = L"ClassDatabase\\U" + WideMultiStringConverter->from_bytes(versionFirstTwoNumbers) + L".*.dat";
	vector<wstring> classDatabasePathList = get_all_files_names_within_folder(filter);
	if (classDatabasePathList.size() == 0) {
		wstringstream errorMessage;
		errorMessage << L"Unsupport Unity version: " << WideMultiStringConverter->from_bytes(versionFirstTwoNumbers);
		MessageBoxW(NULL, errorMessage.str().c_str(), L"", 0x00000000L | 0x00000010L);
		throw errorMessage.str().c_str();
	}
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
	LOG_TRACE("UnityL10nToolCpp::ProcessResourceAndMonoManger() start");
	string globalgamemanagersName = FirstAssetsFileName;
	AssetsFileTable* globalgamemanagersTable = FindAssetsFileTablesFromAssetsName[globalgamemanagersName];
	AssetsFile* globalgamemanagersFile = globalgamemanagersTable->getAssetsFile();
	int ResourceManagerClassId;
	int MonoManagerClassId;

	FindPathIDOfContainerPathFromAssetsName.insert(pair<string, INT32>(FirstAssetsFileName, 0));
	FindAssetsNameFromPathIDOfContainerPath.insert(pair<INT32, string>(0, FirstAssetsFileName));
	LOG_TRACE("dependencyCount==%d", GlobalgamemanagersAssetsTable->getAssetsFile()->dependencies.dependencyCount);
	for (INT32 i = 0; i < GlobalgamemanagersAssetsTable->getAssetsFile()->dependencies.dependencyCount; i++) {
		string dependencyAssetsPath = string(GlobalgamemanagersAssetsTable->getAssetsFile()->dependencies.pDependencies[i].assetPath);
		LOG_TRACE("dependencyAssetsPath == %s", dependencyAssetsPath);
		LOG_TRACE("i==%d", i);
		FindPathIDOfContainerPathFromAssetsName.insert(pair<string, INT32>(dependencyAssetsPath, i + 1));
		FindAssetsNameFromPathIDOfContainerPath.insert(pair<INT32, string>(i + 1, dependencyAssetsPath));
	}
	ResourceManagerClassId = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName["ResourceManager"]].classId;
	LOG_TRACE("ResourceManagerClassId == %d", ResourceManagerClassId);
	MonoManagerClassId = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName["MonoManager"]].classId;
	LOG_TRACE("MonoManagerClassId == %d", MonoManagerClassId);
	int AssetSearchCount = 2;
	LOG_TRACE("AssetSearchCount == %d", AssetSearchCount);
	LOG_TRACE("globalgamemanagersTable->assetFileInfoCount == %d", globalgamemanagersTable->assetFileInfoCount);
	for (unsigned int i = 0;
		(i < globalgamemanagersTable->assetFileInfoCount) && AssetSearchCount > 0;
		i++) {
		LOG_TRACE("i == %d", i);
		AssetFileInfoEx* assetFileInfoEx = &globalgamemanagersTable->pAssetFileInfo[i];
		int classId;
		WORD monoId;
		GetClassIdFromAssetFileInfoEx(GlobalgamemanagersAssetsTable, assetFileInfoEx, classId, monoId);
		LOG_TRACE("classId == %d", classId);
		if (classId == ResourceManagerClassId) {
			//assetFileInfoEx.absolutePos
			ResourceManagerFileGlobal = new ResourceManagerFile();
			LOG_TRACE("ifsGlobalgamemanagers(%ls)", GameFolderPath + WideMultiStringConverter->from_bytes(globalgamemanagersName));
			std::ifstream ifsGlobalgamemanagers(GameFolderPath + WideMultiStringConverter->from_bytes(globalgamemanagersName), std::ios::binary | std::ios::ate);
			LOG_TRACE("assetFileInfoEx->absolutePos == %d", assetFileInfoEx->absolutePos);
			ifsGlobalgamemanagers.seekg(assetFileInfoEx->absolutePos, std::ios::beg);
			LOG_TRACE("assetFileInfoEx->curFileSize == %d", assetFileInfoEx->curFileSize);
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
			LOG_TRACE("ResourceManagerFileGlobal->containerArrayLen == %d", ResourceManagerFileGlobal->containerArrayLen);
			for (int i = 0; i < ResourceManagerFileGlobal->containerArrayLen; i++) {
				resourceManager_PPtr = ResourceManagerFileGlobal->containerArray[i].ids;
				LOG_TRACE(("resourceManager_PPtr{ fileId == " + to_string((long long)resourceManager_PPtr.fileId) + ", pathId == " + to_string((long long)resourceManager_PPtr.pathId)).c_str());
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
					LOG_TRACE("m_ScriptsArrayATVF->GetChildrenCount() == %d", m_ScriptsArrayATVF->GetChildrenCount());
					for (DWORD i = 0; i < m_ScriptsArrayATVF->GetChildrenCount(); i++) {
						INT32 m_FileID = m_ScriptsChildrenListATVF[i]->Get("m_FileID")->GetValue()->AsInt();
						INT64 m_PathID = m_ScriptsChildrenListATVF[i]->Get("m_PathID")->GetValue()->AsInt64();
						if (m_PathID != 0) {
							LOG_TRACE(("i: " + to_string((long long)i) + " m_FileID == " + to_string((long long)m_FileID) + ", m_PathID == " + to_string((long long)m_PathID)).c_str());
							string assetsName = _unityL10nToolAPI.FindAssetsNameFromFileIdDependencies(m_FileID, globalgamemanagersFile);
							LOG_TRACE("assetsName == %s", assetsName);
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
									LOG_TRACE(("FindMonoClassNameFromAssetsNameANDPathId.insert<<" + assetsName + "," + to_string((long long)assetFileInfoEx->index) + ">," + monoScriptFullName + ">").c_str());
								}
							}
						}
					}
				}

				AssetTypeValueField* m_AssemblyNamesArrayATVF =
					baseAssetTypeValueField->Get("m_AssemblyNames")->Get("Array");
				if (m_AssemblyNamesArrayATVF) {
					AssetTypeValueField** m_AssemblyNamesChildrenListATVF = m_AssemblyNamesArrayATVF->GetChildrenList();
					LOG_TRACE("m_AssemblyNamesArrayATVF->GetChildrenCount() == %d", m_AssemblyNamesArrayATVF->GetChildrenCount());
					for (DWORD i = 0; i < m_AssemblyNamesArrayATVF->GetChildrenCount(); i++) {
						AssemblyNames.push_back(m_AssemblyNamesChildrenListATVF[i]->GetValue()->AsString());
						LOG_TRACE("m_AssemblyNamesChildrenListATVF[i]->GetValue()->AsString() == %s", m_AssemblyNamesChildrenListATVF[i]->GetValue()->AsString());
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
		L".\\Libraries\\TypeTreeGenerator.exe",   // No module name (use command line)
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
			TypeTreeGeneratorParams += L"-f \"" + GameFolderPath + L"Managed\\" + WideMultiStringConverter->from_bytes(*iterator) + L"\" ";
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
	_unityL10nToolAPI.MajorVersion = MajorVersion;
	_unityL10nToolAPI.MinorVersion = MinorVersion;
	_unityL10nToolAPI.BasicClassDatabaseFile = BasicClassDatabaseFile;
	_unityL10nToolAPI.MonoClassDatabaseFile = MonoClassDatabaseFile;
	_unityL10nToolAPI.ResourceManagerFileGlobal = ResourceManagerFileGlobal;
	_unityL10nToolAPI.GlobalgamemanagersAssetsTable = GlobalgamemanagersAssetsTable;
	_unityL10nToolAPI.AssetsFileNames = &AssetsFileNames;
	_unityL10nToolAPI.FindAssetsFilesFromAssetsName = &FindAssetsFilesFromAssetsName;
	_unityL10nToolAPI.FindAssetsFileTablesFromAssetsName = &FindAssetsFileTablesFromAssetsName;
	_unityL10nToolAPI.FindAssetsNameFromAssetsFileTables = &FindAssetsNameFromAssetsFileTables;
	_unityL10nToolAPI.FindAssetsNameFromAssetsFiles = &FindAssetsNameFromAssetsFiles;
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
				fontPluginInfo->pluginFileName = relativePluginPath.substr(lastBackslash+1, relativePluginPath.size() - lastBackslash-1);
				fontPluginInfo->relativePluginPath = relativePluginPath.replace(lastBackslash, relativePluginPath.size() - lastBackslash, L"") + L"\\";
				// It must use actual Json::Value
				Json::Value tempJson;
				bool result = getFontPluginInfoCallback(&_unityL10nToolAPI, fontPluginInfo);
				if (result) {
					FontPluginMap.insert(pair<wstring, HINSTANCE>(fontPluginInfo->FontPluginName, PluginHInstance));
					FontPluginInfoMap.insert(pair<wstring, FontPluginInfo*>(fontPluginInfo->FontPluginName, fontPluginInfo));
					wstring pluginName = fontPluginInfo->FontPluginName;
					pluginLoadedList.push_back(pluginName);
					string pluginNameA = WideMultiStringConverter->to_bytes(pluginName);
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

bool UnityL10nToolCpp::LoadTextPlugins()
{
	vector<wstring> PluginFileNameList = GetAllFilesFilterWithinAllSubFolder(CurrentDirectory + L"Plugins\\TextPlugins\\", L"*.ULTTextPlugin");
	for (vector<wstring>::iterator iterator = PluginFileNameList.begin();
		iterator != PluginFileNameList.end(); iterator++) {
		HINSTANCE PluginHInstance = LoadLibraryW((CurrentDirectory + L"Plugins\\TextPlugins\\" + *iterator).c_str());
		GetTextPluginInfoCallback getTextPluginInfoCallback;
		if (PluginHInstance) {
			getTextPluginInfoCallback = (GetTextPluginInfoCallback)GetProcAddress(PluginHInstance, "GetTextPluginInfo");
			if (getTextPluginInfoCallback) {
				TextPluginInfo* textPluginInfo = new TextPluginInfo();
				textPluginInfo->GetOriginalMapFromText = NULL;
				textPluginInfo->GetTranslatedMapFromFileText = NULL;
				textPluginInfo->GetTranslatedTextFromMap = NULL;
				textPluginInfo->GetUpdateFileTextFromMap = NULL;
				bool result = getTextPluginInfoCallback(textPluginInfo);
				if (result) {
					textPluginInfo->TextPluginFileRelativePath = L"Plugins\\TextPlugins\\" + *iterator;
					bool loadSuccess = false;
					if (textPluginInfo->GetOriginalMapFromText && textPluginInfo->GetTranslatedTextFromMap) {
						TextPluginInfoInteractWithAssetMap.insert(pair<wstring, TextPluginInfo*>(textPluginInfo->TextPluginName, textPluginInfo));
						loadSuccess = true;
					} 
					if (textPluginInfo->GetUpdateFileTextFromMap && textPluginInfo->GetTranslatedMapFromFileText) {
						TextPluginInfoInteractWithFileTextMap.insert(pair<wstring, TextPluginInfo*>(textPluginInfo->TextPluginName, textPluginInfo));
						loadSuccess = true;
					}
					if(!loadSuccess){
						FreeLibrary(PluginHInstance);
						continue;
					}
					TextPluginNames.push_back(textPluginInfo->TextPluginName);
					TextplugInMap.insert(pair<wstring, HINSTANCE>(textPluginInfo->TextPluginName, PluginHInstance));
				}
				else {
					FreeLibrary(PluginHInstance);
				}
			}
			else {
				FreeLibrary(PluginHInstance);
			}
		} else { 
			FreeLibrary(PluginHInstance); 
		}
	}
	return true;
	
}

vector<wstring> UnityL10nToolCpp::GetInteractWithAssetPluginNames()
{
	vector<wstring> results;
	for (map<wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithAssetMap.begin();
		iterator != TextPluginInfoInteractWithAssetMap.end(); iterator++) {
		results.push_back(iterator->first);
	}
	return results;
}

vector<wstring> UnityL10nToolCpp::GetInteractWithFileTextPluginNames()
{
	vector<wstring> results;
	for (map<wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithFileTextMap.begin();
		iterator != TextPluginInfoInteractWithFileTextMap.end(); iterator++) {
		results.push_back(iterator->first);
	}
	return results;
}

TextAssetMaps UnityL10nToolCpp::GetTextAssetMaps()
{
	string className = "TextAsset";
	ClassDatabaseType textAssetCDT = BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassName[className]];
	int TextAssetClassId = textAssetCDT.classId;
	for (map<string, AssetsFileTable*>::iterator itrAssetsFileTable = FindAssetsFileTablesFromAssetsName.begin();
		itrAssetsFileTable != FindAssetsFileTablesFromAssetsName.end(); itrAssetsFileTable++) {
		AssetsFileTable* assetsFileTable = itrAssetsFileTable->second;
		AssetsFile* assetsFile = assetsFileTable->getAssetsFile();
		wstring assetsName = WideMultiStringConverter->from_bytes(itrAssetsFileTable->first);
		INT32 FileID = distance(AssetsFileNames.begin(), find(AssetsFileNames.begin(), AssetsFileNames.end(), string(itrAssetsFileTable->first)));
		if (FileID != AssetsFileNames.size()) {
			for (unsigned int i = 0; i < assetsFileTable->assetFileInfoCount; i++) {
				AssetFileInfoEx* assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
				int classId = 0;
				UINT16 monoClassId = 0;
				GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
				if (classId == TextAssetClassId) {
					wstring assetName = _unityL10nToolAPI.GetAssetNameW(assetsFile, assetFileInfoEx);
					INT64 PathID = assetFileInfoEx->index;
					wstring containerPath;
					map<string, INT32>::const_iterator FileIDIterator = FindPathIDOfContainerPathFromAssetsName.find(itrAssetsFileTable->first);
					if (FileIDIterator != FindPathIDOfContainerPathFromAssetsName.end()) {
						map<pair<INT32, INT64>, string>::iterator containerPathItr = FindContainerPathFromFileIDPathID.find(pair<INT32, INT64>(FileIDIterator->second, PathID));
						if (containerPathItr != FindContainerPathFromFileIDPathID.end()) {
							containerPath = WideMultiStringConverter->from_bytes(containerPathItr->second);
						}
					}
					AssetTypeInstance* assetTypeInstance
						= _unityL10nToolAPI.GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
					AssetTypeValueField* pbase = assetTypeInstance->GetBaseField();
					wstring m_Script;
					if (pbase) {
						try {
							// 이럴때 문자열을 온전히 받아올 수 없으면 AssetTypeByteArray를 써서 문자열을 받아와야함.
							m_Script = WideMultiStringConverter->from_bytes(pbase->Get("m_Script")->GetValue()->AsString());
						}
						catch (exception e1) {
							m_Script = L"";
						}
					}
					TextAssetMap textAssetMap;
					textAssetMap.assetsName = assetsName;
					textAssetMap.assetName = assetName;
					textAssetMap.containerPath = containerPath;
					textAssetMap.useContainerPath = false;
					textAssetMap.OriginalText = m_Script;
					TextAssetMapsGlobal.InteractWithAssetNews.push_back(textAssetMap);
				}
			}
		}
	}
	//GetPacherConfigJson();
	if (projectJson.isMember("TextPlugin")) {
		Json::Value TextAssetMapsProjectJson = projectJson["TextPlugin"];
		TextAssetMaps textAssetMapsLocal = TextAssetMaps(TextAssetMapsProjectJson);
		for (vector<TextAssetMap>::iterator iterator = textAssetMapsLocal.InteractWithFileTextNews.begin();
			iterator != textAssetMapsLocal.InteractWithFileTextNews.end(); iterator++) {
			for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.InteractWithAssetNews.begin();
				iterator != TextAssetMapsGlobal.InteractWithAssetNews.end(); textAssetMapItr++) {
				if (iterator->assetsName == textAssetMapItr->assetsName &&
					iterator->assetName == textAssetMapItr->assetName &&
					iterator->containerPath == textAssetMapItr->containerPath) {
					map<wstring, TextPluginInfo*>::iterator textPluginInfoInteractWithAssetItr = TextPluginInfoInteractWithAssetMap.find(iterator->InteractWithAssetPluginName);
					if (textPluginInfoInteractWithAssetItr != TextPluginInfoInteractWithAssetMap.end()) {
						iterator->OriginalText = textAssetMapItr->OriginalText;
						TextAssetMapsGlobal.InteractWithFileTextNews.push_back(*iterator);
						TextAssetMapsGlobal.InteractWithAssetNews.erase(textAssetMapItr);
						break;
					}
					else {
						break;
					}
				}
			}
		}
		for (vector<TextAssetMap>::iterator iterator = textAssetMapsLocal.Done.begin();
			iterator != textAssetMapsLocal.Done.end(); iterator++) {
			if (iterator->ExternalRelativeFilePath.empty()) {
				for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.InteractWithAssetNews.begin();
					iterator != TextAssetMapsGlobal.InteractWithAssetNews.end(); textAssetMapItr++) {
					if (iterator->assetsName == textAssetMapItr->assetsName &&
						iterator->assetName == textAssetMapItr->assetName &&
						iterator->containerPath == textAssetMapItr->containerPath) {
						vector<wstring>::const_iterator InteractWithAssetPluginFound = find(TextPluginNames.begin(), TextPluginNames.end(), iterator->InteractWithAssetPluginName);
						vector<wstring>::const_iterator InteractWithFileTextPluginFound = find(TextPluginNames.begin(), TextPluginNames.end(), iterator->InteractWithFileTextPluginName);
						map<wstring, TextPluginInfo*>::iterator textPluginInfoInteractWithAssetItr = TextPluginInfoInteractWithAssetMap.find(iterator->InteractWithAssetPluginName);
						map<wstring, TextPluginInfo*>::iterator textPluginInfoInteractWithFileTextItr = TextPluginInfoInteractWithFileTextMap.find(iterator->InteractWithFileTextPluginName);
						if (textPluginInfoInteractWithAssetItr != TextPluginInfoInteractWithAssetMap.end() &&
							textPluginInfoInteractWithFileTextItr != TextPluginInfoInteractWithFileTextMap.end()) {
							iterator->OriginalText = textAssetMapItr->OriginalText;
							TextAssetMapsGlobal.Done.push_back(*iterator);
							TextAssetMapsGlobal.InteractWithAssetNews.erase(textAssetMapItr);
							break;
						}
						else {
							break;
						}
					}
				}
			}
			else {
				vector<wstring>::const_iterator InteractWithAssetPluginFound = find(TextPluginNames.begin(), TextPluginNames.end(), iterator->InteractWithAssetPluginName);
				vector<wstring>::const_iterator InteractWithFileTextPluginFound = find(TextPluginNames.begin(), TextPluginNames.end(), iterator->InteractWithFileTextPluginName);
				map<wstring, TextPluginInfo*>::iterator textPluginInfoInteractWithAssetItr = TextPluginInfoInteractWithAssetMap.find(iterator->InteractWithAssetPluginName);
				map<wstring, TextPluginInfo*>::iterator textPluginInfoInteractWithFileTextItr = TextPluginInfoInteractWithFileTextMap.find(iterator->InteractWithFileTextPluginName);
				if (textPluginInfoInteractWithAssetItr != TextPluginInfoInteractWithAssetMap.end() &&
					textPluginInfoInteractWithFileTextItr != TextPluginInfoInteractWithFileTextMap.end()) {
					if (FileExist(UnityL10nToolProjectInfoGlobal.GameRootPath() + iterator->ExternalRelativeFilePath)) {
						iterator->OriginalText = WideMultiStringConverter->from_bytes(readFile2(UnityL10nToolProjectInfoGlobal.GameRootPath() + iterator->ExternalRelativeFilePath));
						TextAssetMapsGlobal.Done.push_back(*iterator);
						break;
					}
				}
			}			
		}
	}
	return TextAssetMapsGlobal;
}

TextAssetMap UnityL10nToolCpp::GetTextAssetMapFromExternalFile(wstring ExternalRelativeFilePath)
{
	ReplaceAll(ExternalRelativeFilePath, UnityL10nToolProjectInfoGlobal.GameRootPath(), L"");
	if (FileExist(UnityL10nToolProjectInfoGlobal.GameRootPath() + ExternalRelativeFilePath)) {
		wstring originalText = WideMultiStringConverter->from_bytes(readFile2(UnityL10nToolProjectInfoGlobal.GameRootPath() + ExternalRelativeFilePath));
		TextAssetMap textAssetMap = TextAssetMap();
		textAssetMap.ExternalRelativeFilePath = ExternalRelativeFilePath;
		textAssetMap.OriginalText = originalText;
		TextAssetMapsGlobal.InteractWithAssetNews.push_back(textAssetMap);
		return textAssetMap;
	}
	else {
		return TextAssetMap();
	}
}

bool UnityL10nToolCpp::SetTextAssetMaps(TextAssetMap textAssetMap, TextAssetMap::ToWhere toWhere)
{
	vector<TextAssetMap>* InteractWithAssetNewsPtr = &(TextAssetMapsGlobal.InteractWithAssetNews);
	vector<TextAssetMap>* InteractWithFileTextNewsPtr = &(TextAssetMapsGlobal.InteractWithFileTextNews);
	//vector<TextAssetMap>* InteractWithMonoAssetNewsPtr = &(TextAssetMapsGlobal.InteractWithMonoAssetNews);
	vector<TextAssetMap>* DonePtr = &(TextAssetMapsGlobal.Done);
	vector< vector<TextAssetMap>*> vectorPtrs;
	vector<TextAssetMap>* targetPtrs;
	switch (toWhere) {
	case TextAssetMap::ToWhere::None:
		//targetPtrs = InteractWithAssetNewsPtr;
		vectorPtrs.push_back(InteractWithAssetNewsPtr);
		vectorPtrs.push_back(InteractWithFileTextNewsPtr);
		//vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
		vectorPtrs.push_back(DonePtr);
		break;
	case TextAssetMap::ToWhere::ToInteractWithAsset:
		targetPtrs = InteractWithAssetNewsPtr;
		//vectorPtrs.push_back(InteractWithAssetNewsPtr);
		vectorPtrs.push_back(InteractWithFileTextNewsPtr);
		//vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
		vectorPtrs.push_back(DonePtr);
		break;
	case TextAssetMap::ToWhere::ToInteractWithFileText:
		targetPtrs = InteractWithFileTextNewsPtr;
		vectorPtrs.push_back(InteractWithAssetNewsPtr);
		//vectorPtrs.push_back(InteractWithFileTextNewsPtr);
		//vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
		vectorPtrs.push_back(DonePtr);
		break;
	/*case TextAssetMap::ToWhere::ToInteractWithMonoAsset:
	targetPtrs
		vectorPtrs.push_back(InteractWithAssetNewsPtr);
		vectorPtrs.push_back(InteractWithFileTextNewsPtr);
		vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
		vectorPtrs.push_back(DonePtr);
		break;*/
	case TextAssetMap::ToWhere::ToDone:
		targetPtrs = DonePtr;
		vectorPtrs.push_back(InteractWithAssetNewsPtr);
		vectorPtrs.push_back(InteractWithFileTextNewsPtr);
		//vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
		//vectorPtrs.push_back(DonePtr);
		break;
	}
	for (vector<TextAssetMap>::iterator iterator = targetPtrs->begin();
		iterator != targetPtrs->end(); iterator++) {
		if (TextAssetMap::LooseCompare(textAssetMap, *iterator)) {
			return true;
		}
	}
	for (vector<vector<TextAssetMap>*>::iterator vectorPtr = vectorPtrs.begin();
		vectorPtr != vectorPtrs.end(); vectorPtr++) {
		for (vector<TextAssetMap>::iterator textAssetMapPtr = (*vectorPtr)->begin();
			textAssetMapPtr != (*vectorPtr)->end(); textAssetMapPtr++) {
			if (TextAssetMap::LooseCompare(textAssetMap, *textAssetMapPtr)) {
				if (toWhere == TextAssetMap::None) {
					*textAssetMapPtr = textAssetMap;
				}
				else {
					(*vectorPtr)->erase(textAssetMapPtr);
					targetPtrs->push_back(textAssetMap);
					return true;
				}
			}
		}
	}
	return false;
}

bool UnityL10nToolCpp::GetTranslatedTextAssetsFromFile()
{
	for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.Done.begin();
		textAssetMapItr != TextAssetMapsGlobal.Done.end(); textAssetMapItr++) {
		TextAssetMap textAssetMap = *textAssetMapItr;
		textAssetMap = GetOriginalLanguagePairDics(textAssetMap, true);
		textAssetMap = LoadTranslatedFileTextFromTempAndResourceFolder(textAssetMap);
		textAssetMap = GetTranslatedLanguagePairDics(textAssetMap, true);
		textAssetMap = GetTranslatedText(textAssetMap, true);
		*textAssetMapItr = textAssetMap;
	}
	return true;
}

bool UnityL10nToolCpp::GetAssetReplacerFromTextAssets()
{
	for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.Done.begin();
		textAssetMapItr != TextAssetMapsGlobal.Done.end(); textAssetMapItr++) {
		if (textAssetMapItr->ExternalRelativeFilePath.empty()) {
			AssetsFileTable* assetsFileTable;
			INT64 PathID;
			string assetsName;
			if (textAssetMapItr->useContainerPath) {
				map<string, pair<INT32, INT64>>::iterator FileIDPathIDiterator
					= FindFileIDPathIDFromContainerPath.find(WideMultiStringConverter->to_bytes(textAssetMapItr->containerPath));
				if (FileIDPathIDiterator != FindFileIDPathIDFromContainerPath.end()) {
					INT32 FileIDOfContainerPath = FileIDPathIDiterator->second.first;
					PathID = FileIDPathIDiterator->second.second;
					map<INT32, string>::iterator assetsNameIterator
						= FindAssetsNameFromPathIDOfContainerPath.find(FileIDOfContainerPath);
					if (assetsNameIterator != FindAssetsNameFromPathIDOfContainerPath.end()) {
						assetsName = assetsNameIterator->second;
						map<string, AssetsFileTable*>::iterator assetsFileTableIterator
							= FindAssetsFileTablesFromAssetsName.find(assetsName);
						if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
							assetsFileTable = assetsFileTableIterator->second;
						}
						else {
							throw new std::exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
						}
					}
					else {
						throw new std::exception("assetsNameIterator == FindAssetsNameFromPathIDOfContainerPath.end()");
					}
				}
				else {
					throw new std::exception("FileIDPathIDiterator == FindFileIDPathIDFromContainerPath.end()");
				}
			}
			else {
				assetsName = WideMultiStringConverter->to_bytes(textAssetMapItr->assetsName);
				map<string, AssetsFileTable*>::iterator assetsFileTableIterator
					= FindAssetsFileTablesFromAssetsName.find(WideMultiStringConverter->to_bytes(textAssetMapItr->assetsName));
				if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
					assetsFileTable = assetsFileTableIterator->second;
					PathID = _unityL10nToolAPI.FindAssetIndexFromName(assetsFileTable, WideMultiStringConverter->to_bytes(textAssetMapItr->assetName));
					if (PathID == -1) {
						throw new std::exception("PathID == -1");
					}
				}
				else {
					throw new std::exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
				}
			}
			AssetFileInfoEx* assetFileInfoEx = assetsFileTable->getAssetInfo(PathID);
			if (assetFileInfoEx == NULL) {
				throw new std::exception("assetFileInfoEx == NULL");
			}
			AssetTypeTemplateField templateBase;
			//char *name = "";
			//char *pathName = "";
			int classId;
			WORD monoClassId;
			GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classId, monoClassId);
			templateBase.FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[FindBasicClassIndexFromClassID.find(classId)->second], (DWORD)0, false);
			for (DWORD i = 0; i < templateBase.childrenCount; i++)
			{
				if (!strcmp(templateBase.children[i].name, "m_Script"))
				{
					templateBase.children[i].type = "_string";
					templateBase.children[i].valueType = ValueType_None;
					for (DWORD k = 0; k < templateBase.children[i].childrenCount; k++)
					{
						if (!strcmp(templateBase.children[i].children[k].name, "Array"))
						{
							templateBase.children[i].children[k].type = "TypelessData";
							break;
						}
					}
					break;
				}
			}
			AssetTypeTemplateField *pTemplateBase = &templateBase;
			AssetTypeInstance* baseAssetTypeInstance = new AssetTypeInstance(
				(DWORD)1,
				&pTemplateBase,
				assetFileInfoEx->curFileSize,
				assetsFileTable->getReader(),
				assetsFileTable->getAssetsFile()->header.endianness ? true : false,
				assetFileInfoEx->absolutePos);
			AssetTypeValueField *pBase = baseAssetTypeInstance->GetBaseField();
			if (pBase) {
				AssetTypeValueField *nameField = pBase->Get("m_Name");
				AssetTypeValueField *scriptField = pBase->Get("m_Script");
				AssetTypeValueField *dataArrayField = scriptField->Get("Array");
				if (!nameField->IsDummy() && !scriptField->IsDummy() && !dataArrayField->IsDummy())
				{
					string* str = new string(WideMultiStringConverter->to_bytes(textAssetMapItr->TranslatedText));
					//name = nameField->GetValue()->AsString();
					AssetTypeByteArray byteArrayData;
					byteArrayData.size = (DWORD)str->size();
					byteArrayData.data = (BYTE*)str->c_str();
					dataArrayField->GetValue()->Set(&byteArrayData);
					AssetsReplacer* assetsReplacer = _unityL10nToolAPI.makeAssetsReplacer(assetsFileTable, assetFileInfoEx, baseAssetTypeInstance);
					AssetsReplacersMap[assetsName].push_back(assetsReplacer);
					continue;
				}
			}
			else {
				throw new std::exception("pBase == NULL");
			}
			throw new std::exception("UNKNOWN");
		}
		else {
			std::wofstream wof;
			wof.clear();
			wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			wof.open(UnityL10nToolProjectInfoGlobal.GameRootPath() + textAssetMapItr->ExternalRelativeFilePath);
			wof << textAssetMapItr->TranslatedText;
			wof.close();
		}
	}
	return true;

	//	AssetsFileTable* assetsFileTable;
//	INT64 PathID;
//	if (textAssetMap.useContainerPath) {
//		map<string, pair<INT32, INT64>>::iterator FileIDPathIDiterator
//			= FindFileIDPathIDFromContainerPath.find(WideMultiStringConverter->to_bytes(textAssetMap.containerPath));
//		if (FileIDPathIDiterator != FindFileIDPathIDFromContainerPath.end()) {
//			INT32 FileIDOfContainerPath = FileIDPathIDiterator->second.first;
//			PathID = FileIDPathIDiterator->second.second;
//			map<INT32, string>::iterator assetsNameIterator
//				= FindAssetsNameFromPathIDOfContainerPath.find(FileIDOfContainerPath);
//			if (assetsNameIterator != FindAssetsNameFromPathIDOfContainerPath.end()) {
//				string assetsName = assetsNameIterator->second;
//				map<string, AssetsFileTable*>::iterator assetsFileTableIterator
//					= FindAssetsFileTablesFromAssetsName.find(assetsName);
//				if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
//					assetsFileTable = assetsFileTableIterator->second;
//				}
//				else {
//					throw new exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
//				}
//			}
//			else {
//				throw new exception("assetsNameIterator == FindAssetsNameFromPathIDOfContainerPath.end()");
//			}
//		}
//		else {
//			throw new exception("FileIDPathIDiterator == FindFileIDPathIDFromContainerPath.end()");
//		}
//	}
//	else {
//		map<string, AssetsFileTable*>::iterator assetsFileTableIterator
//			= FindAssetsFileTablesFromAssetsName.find(WideMultiStringConverter->to_bytes(textAssetMap.assetsName));
//		if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
//			assetsFileTable = assetsFileTableIterator->second;
//			PathID = _unityL10nToolAPI.FindAssetIndexFromName(assetsFileTable, WideMultiStringConverter->to_bytes(textAssetMap.assetName));
//			if (PathID == -1) {
//				throw new exception("PathID == -1");
//			}
//		}
//		else {
//			throw new exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
//		}
//	}
//	AssetFileInfoEx* assetFileInfoEx = assetsFileTable->getAssetInfo(PathID);
//	if (assetFileInfoEx == NULL) {
//		throw new exception("assetFileInfoEx == NULL");
//	}
//	AssetTypeInstance* assetTypeInstance
//		= _unityL10nToolAPI.GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
//	if (assetTypeInstance == NULL) {
//		throw new exception("assetTypeInstance == NULL");
//	}
//	AssetTypeValueField* pbase = assetTypeInstance->GetBaseField();
//	if (pbase) {
//		wstring m_Script = WideMultiStringConverter->from_bytes(pbase->Get("m_Script")->GetValue()->AsString());
//		return m_Script;
//	}
//	else {
//		throw new exception("pBase == NULL");
//	}
//	throw new exception("UNKNOWN");
}

bool UnityL10nToolCpp::SetPluginsSupportAssetMap(map<wstring, FontAssetMaps> pluginSupportAssetMaps)
{
	FontAssetMapsGlobal = pluginSupportAssetMaps;
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		bool result = fontPluginInfo->SetPluginSupportAssetMap(pluginSupportAssetMaps[iterator->first]);
		if (result == false) {
			return false;
		}
	}
	return true;
}

bool UnityL10nToolCpp::GetProjectConfigJsonFromFontPlugin()
{
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		Json::Value fontAssetPluginProjectConfigJson = fontPluginInfo->GetProjectConfigJson();
		projectJson["FontPlugin"][WideMultiStringConverter->to_bytes(iterator->first)] = fontAssetPluginProjectConfigJson;
	}
	return true;
}

bool UnityL10nToolCpp::SetTextPluginConfigToJsonValue() {
	
	projectJson["TextPlugin"] = TextAssetMapsGlobal.ToJSON();
	return true;
}

void UnityL10nToolCpp::LoadProjectSettingsFromJson()
{
	if (projectJson.isMember("ProjectSettings")) {
		ProjectSettingsGlobal = ProjectSettings(projectJson["ProjectSettings"]);
	}
}

ProjectSettings UnityL10nToolCpp::GetProjectSettings()
{
	return ProjectSettingsGlobal;
}

void UnityL10nToolCpp::SetProjectSettings(ProjectSettings projectSettings)
{
	ProjectSettingsGlobal = projectSettings;
	projectJson["ProjectSettings"] = ProjectSettingsGlobal.toJson();
}

// https://sockbandit.wordpress.com/2012/05/31/c-read-and-write-utf-8-file-using-standard-libarary/
bool UnityL10nToolCpp::SaveProjectConfigJson() {
	std::wofstream wof;
	wof.clear();
	wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	wof.open(UnityL10nToolProjectInfoGlobal.JSONPath);
	wof << WideMultiStringConverter->from_bytes(projectJson.toStyledString());
	wof.close();
	return true;
}

bool UnityL10nToolCpp::BuildProject(wstring buildTargetFolder) {
	if (CreateDirectoryW(buildTargetFolder.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{} else {}
	
	//copyFileCustom(buildTargetFolder)
	Json::Value patcherJson = GetPacherConfigJson();
	std::wofstream wof;
	wof.clear();
	wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	wof.open(buildTargetFolder + L"setting.json");
	wof << WideMultiStringConverter->from_bytes(patcherJson.toStyledString());
	wof.close();

	CopyDirTo(CurrentDirectory + L"PatcherLibs\\", buildTargetFolder + L"");
#pragma region ModifyFromIconFileToExeDll
	if (FileExist(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customIcon.ico")) {
		wstring moduleName = CurrentDirectory + L"Libraries\\ModifyFromIconFileToExeDll.exe";
		wstring command = (L"\"" + buildTargetFolder + L"UnityL10nToolPatcherCShop.exe\" \"" + UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customIcon.ico\" 32512").c_str();
		string systemCommand = WideMultiStringConverter->to_bytes(moduleName + L" " + command);
		system(systemCommand.c_str());
		/*STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));
		if (!CreateProcess(
			(CurrentDirectory + L"Libraries\\ModifyFromIconFileToExeDll.exe").c_str(),   // No module name (use command line)
			(LPWSTR)(command.c_str()),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			//CREATE_NO_WINDOW,              // No creation flags
			NULL,
			NULL,           // Use parent's environment block
			(CurrentDirectory + L"Libraries\\").c_str(),           // Use parent's starting directory
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
		CloseHandle(pi.hThread);*/
	}
#pragma endregion
	CopyFileW((CurrentDirectory + L"msvcp100.dll").c_str(), (buildTargetFolder + L"msvcp100.dll").c_str(), false);
	CopyFileW((CurrentDirectory + L"msvcr100.dll").c_str(), (buildTargetFolder + L"msvcr100.dll").c_str(), false);
	CopyDirTo(CurrentDirectory + L"Libraries\\", buildTargetFolder + L"Libraries\\");
	CopyDirTo(CurrentDirectory + L"ClassDatabase\\", buildTargetFolder + L"ClassDatabase\\");
	if (FileExist(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customSplash.png")) {
		CopyFileW((UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customSplash.png").c_str(), (buildTargetFolder + L"customSplash.png").c_str(), false);
	}
	if (FileExist(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customMain.png")) {
		CopyFileW((UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"customMain.png").c_str(), (buildTargetFolder + L"customMain.png").c_str(), false);
	}

	if (CreateDirectoryW((buildTargetFolder +L"Plugins\\").c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
	}
	else {}
	if (CreateDirectoryW((buildTargetFolder + L"Plugins\\FontPlugins\\").c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
	}
	else {}
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin(); iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		map<wstring, FontAssetMaps>::iterator foundFAMs = FontAssetMapsGlobal.find(fontPluginInfo->FontPluginName);
		if (foundFAMs != FontAssetMapsGlobal.end()) {
			if (foundFAMs->second.Saveds.size() != 0) {
				if (fontPluginInfo->CopyBuildFileToBuildFolder(fontPluginInfo->relativePluginPath, buildTargetFolder + fontPluginInfo->relativePluginPath)) {
					copyFileCustom((fontPluginInfo->relativePluginPath + fontPluginInfo->pluginFileName).c_str(), (buildTargetFolder + fontPluginInfo->relativePluginPath + fontPluginInfo->pluginFileName).c_str());
				}
			}
		}
	}
	map<wstring, TextPluginInfo*> textPluginInfos;
	for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.Done.begin();
		textAssetMapItr != TextAssetMapsGlobal.Done.end(); textAssetMapItr++) {
		textPluginInfos[textAssetMapItr->InteractWithAssetPluginName] =
			TextPluginInfoInteractWithAssetMap[textAssetMapItr->InteractWithAssetPluginName];
		textPluginInfos[textAssetMapItr->InteractWithFileTextPluginName] =
			TextPluginInfoInteractWithFileTextMap[textAssetMapItr->InteractWithFileTextPluginName];
	}
	if (textPluginInfos.size() != 0) {
		if (CreateDirectoryW((buildTargetFolder + L"Plugins\\TextPlugins\\").c_str(), NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
		}
	}
	for (map<wstring, TextPluginInfo*>::iterator textPluginInfoItr = textPluginInfos.begin();
		textPluginInfoItr != textPluginInfos.end(); textPluginInfoItr++) {
		copyFileCustom((CurrentDirectory + textPluginInfoItr->second->TextPluginFileRelativePath).c_str(), (buildTargetFolder + textPluginInfoItr->second->TextPluginFileRelativePath).c_str());
	}
	if (ProjectSettingsGlobal.DownloadOnlineResourcesWhenBuild) {
		DownloadResourcesFromInternetToBuildResourceFolder();
	}
	CopyDirTo(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"Resources\\", buildTargetFolder + L"Resources\\");
	return true;
}

Json::Value UnityL10nToolCpp::GetPacherConfigJson() {
	Json::Value patcherConfigJson;
	patcherConfigJson["GameName"] = WideMultiStringConverter->to_bytes(UnityL10nToolProjectInfoGlobal.GameName);
	patcherConfigJson["MakerName"] = WideMultiStringConverter->to_bytes(UnityL10nToolProjectInfoGlobal.MakerName);
	patcherConfigJson["DataFolderName"] = WideMultiStringConverter->to_bytes(UnityL10nToolProjectInfoGlobal.DataFolderName);
	patcherConfigJson["GamePath"] = WideMultiStringConverter->to_bytes(UnityL10nToolProjectInfoGlobal.GamePath);
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		Json::Value fontAssetPluginPatcherConfigJson = fontPluginInfo->GetPacherConfigJson();
		patcherConfigJson["FontPlugin"][WideMultiStringConverter->to_bytes(iterator->first)] = fontAssetPluginPatcherConfigJson;
	}
	patcherConfigJson["TextPlugin"]["Done"] = TextAssetMapsGlobal.ToJSON()["Done"];
	patcherConfigJson["OnlineResources"] = SetOnlineResourcePairsToProjectJson();
	patcherConfigJson["CopyResourcesManuals"] = SetCopyResourcesManualPairsToProjectJson();
	patcherConfigJson["OnlineUpdate"] = projectJson["OnlineUpdate"];
	return patcherConfigJson;
}

bool UnityL10nToolCpp::SetPacherConfigJson()
{
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		string pluginNameA = WideMultiStringConverter->to_bytes(fontPluginInfo->FontPluginName);
		if (projectJson["FontPlugin"].isMember(pluginNameA)) {
			fontPluginInfo->SetPacherConfigJson(projectJson["FontPlugin"][pluginNameA]);
		}
		else {
			fontPluginInfo->SetPacherConfigJson(Json::Value());
		}
	}
	return true;
}

bool UnityL10nToolCpp::LoadAssetsReplacer()
{
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		map<string, vector<AssetsReplacer*>> fontPluginAssetsReplacerMap = fontPluginInfo->GetPatcherAssetReplacer();
		for (map<string, vector<AssetsReplacer*>>::iterator iterator = fontPluginAssetsReplacerMap.begin();
			iterator != fontPluginAssetsReplacerMap.end(); iterator++) {
			string key = iterator->first;
			vector<AssetsReplacer*> assetsReplacers = iterator->second;
			AssetsReplacersMap[key].insert(AssetsReplacersMap[key].end(), assetsReplacers.begin(), assetsReplacers.end());
		}
	}
	return true;
}

bool UnityL10nToolCpp::MakeModifiedAssetsFile() {
	for (map<string, vector<AssetsReplacer*>>::iterator iterator = AssetsReplacersMap.begin();
		iterator != AssetsReplacersMap.end(); iterator++) {
		string key = iterator->first;
		vector<AssetsReplacer*> assetsReplacers = iterator->second;
		AssetsFileTable* assetsFileTable = FindAssetsFileTablesFromAssetsName[key];
		if (assetsReplacers.size() > 0) {
			wstring fullPath = GameFolderPath + WideMultiStringConverter->from_bytes(key);
			IAssetsWriter* assetsWriter = Create_AssetsWriterToFile((fullPath + L".mod").c_str(), true, true, RWOpenFlags_None);
			assetsFileTable->getAssetsFile()->Write(assetsWriter, 0, assetsReplacers.data(), assetsReplacers.size(), 0);
			assetsFileTable->getReader()->Close();
			assetsWriter->Close();
			int removeResult = _wremove((fullPath).c_str());
			if (removeResult == -1) {
				MessageBoxW(NULL, (L"Cannot modify " + fullPath + L"\nPlease close Game and Steam and retry.").c_str(), L"Please Close Game and steam.", MB_OK);
				throw new exception("Please Close Game and steam.");
			}
			int renameResult = _wrename((fullPath + L".mod").c_str(), fullPath.c_str());
		}
	}
	return true;
}

bool UnityL10nToolCpp::CopyResourceFileToGameFolder() {
	for (map<wstring, FontPluginInfo*>::iterator iterator = FontPluginInfoMap.begin();
		iterator != FontPluginInfoMap.end(); iterator++) {
		FontPluginInfo* fontPluginInfo = iterator->second;
		fontPluginInfo->CopyResourceFileToGameFolder(fontPluginInfo->relativePluginPath,GameFolderPath);
	}
	return true;
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

wstring UnityL10nToolCpp::DetermineUnityGameFolder(wstring path)
{
	path = MakeSureBackslashEndOfFolderPath(path);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFileW((path + L"globalgamemanagers").c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			::FindClose(hFind);
			return L"globalgamemanagers";
		}
	}
	::FindClose(hFind);
	hFind = ::FindFirstFileW((path + L"mainData").c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			::FindClose(hFind);
			return L"mainData";
		}
	}
	return L"";
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

bool GetGameMakerNameFromGlobalgamemanager(wstring path, wstring& GameName, wstring& MakerName) {
	wstring globalgamemanagersFileName = UnityL10nToolCpp::DetermineUnityGameFolder(path);
	if (globalgamemanagersFileName == L"") return false;
	WIN32_FIND_DATA fdGlobalgamemanager;
	HANDLE hFindGlobalgamemanager = ::FindFirstFileW((path + globalgamemanagersFileName).c_str(), &fdGlobalgamemanager);
	if (hFindGlobalgamemanager != INVALID_HANDLE_VALUE) {
		::FindClose(hFindGlobalgamemanager);
		IAssetsReader* assetsReader = Create_AssetsReaderFromFile((path + globalgamemanagersFileName).c_str(), true, RWOpenFlags_None);
		AssetsFile* assetsFile = new AssetsFile(assetsReader);
		AssetsFileTable* assetsFileTable = new AssetsFileTable(assetsFile);
		string version = assetsFile->typeTree.unityVersion;
		size_t lastDotOffset = version.find_last_of('.');
		string versionFirstTwoNumbers = version.substr(0, lastDotOffset);
		wstring filter = L"ClassDatabase\\U" + WideMultiStringConverter->from_bytes(versionFirstTwoNumbers) + L".*.dat";
		vector<wstring> classDatabasePathList = get_all_files_names_within_folder(filter);
		wstring classDatabaseFileName = classDatabasePathList[0];
		IAssetsReader* classDatabaseReader = Create_AssetsReaderFromFile((L"ClassDatabase\\" + classDatabaseFileName).c_str(), true, RWOpenFlags_None);
		ClassDatabaseFile* BasicClassDatabaseFile = new ClassDatabaseFile();
		BasicClassDatabaseFile->Read(classDatabaseReader);
		int PlayerSettingsClassid = 0;
		int PlayerSettingsItr = 0;
		for (size_t i = 0; i < BasicClassDatabaseFile->classes.size(); i++)
		{
			const char* classDatabaseTypeName = BasicClassDatabaseFile->classes[i].name.GetString(BasicClassDatabaseFile);
			if (strcmp(classDatabaseTypeName, "PlayerSettings") == 0) {
				PlayerSettingsClassid = BasicClassDatabaseFile->classes[i].classId;
				PlayerSettingsItr = i;
				break;
			}
		}
		if (PlayerSettingsClassid != 0) {
			for (int i = 0; i < assetsFileTable->assetFileInfoCount; i++)
			{
				int classid = 0;
				UINT16 MonoClassid = 0;
				AssetFileInfoEx* assetFileInfoEx = &assetsFileTable->pAssetFileInfo[i];
				UnityL10nToolAPI::GetClassIdFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx, classid, MonoClassid);
				if (PlayerSettingsClassid == classid) {
					AssetTypeTemplateField* baseAssetTypeTemplateField = new AssetTypeTemplateField;
					baseAssetTypeTemplateField->FromClassDatabase(BasicClassDatabaseFile, &BasicClassDatabaseFile->classes[PlayerSettingsItr], (DWORD)0, false);
					AssetTypeInstance baseAssetTypeInstance = AssetTypeInstance(
						(DWORD)1,
						&baseAssetTypeTemplateField,
						assetFileInfoEx->curFileSize,
						assetsFileTable->getReader(),
						assetsFileTable->getAssetsFile()->header.endianness ? true : false,
						assetFileInfoEx->absolutePos);
					baseAssetTypeTemplateField->Clear();
					AssetTypeValueField* pBase = baseAssetTypeInstance.GetBaseField();
					if (pBase) {
						AssetTypeValueField* companyNameField = pBase->Get("companyName");
						AssetTypeValueField* productNameField = pBase->Get("productName");
						if (companyNameField && productNameField &&
							!companyNameField->IsDummy() && !productNameField->IsDummy()) {
							MakerName = WideMultiStringConverter->from_bytes(companyNameField->GetValue()->AsString());
							GameName = WideMultiStringConverter->from_bytes(productNameField->GetValue()->AsString());
							
							assetsFile->~AssetsFile();
							assetsFileTable->~AssetsFileTable();
							assetsReader->Close();
							return true;
						}
					}
					
					break;
				}
			}
		}
		assetsFile->~AssetsFile();
		assetsFileTable->~AssetsFileTable();
		assetsReader->Close();
	}
	::FindClose(hFindGlobalgamemanager);

	return false;
}

wstring FindUnityGameFolderFromDataFolderNameInternal2(wstring path, wstring GameName, wstring MakerName) {
	path = UnityL10nToolCpp::MakeSureBackslashEndOfFolderPath(path);
	WIN32_FIND_DATA fdAppInfo;
	HANDLE hFindAppInfo = ::FindFirstFileW((path + L"app.info").c_str(), &fdAppInfo);
	if (hFindAppInfo != INVALID_HANDLE_VALUE) {
		string str = readFile2(path + L"app.info");
		wstring wstr = WideMultiStringConverter->from_bytes(str);
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
	else {
		wstring tempGameName;
		wstring tempMakerName;
		if (GetGameMakerNameFromGlobalgamemanager(path, tempGameName, tempMakerName)) {
			return path;
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

//wstring UnityL10nToolCpp::GetOriginalText(TextAssetMap textAssetMap) {
//	AssetsFileTable* assetsFileTable;
//	INT64 PathID;
//	if (textAssetMap.useContainerPath) {
//		map<string, pair<INT32, INT64>>::iterator FileIDPathIDiterator
//			= FindFileIDPathIDFromContainerPath.find(WideMultiStringConverter->to_bytes(textAssetMap.containerPath));
//		if (FileIDPathIDiterator != FindFileIDPathIDFromContainerPath.end()) {
//			INT32 FileIDOfContainerPath = FileIDPathIDiterator->second.first;
//			PathID = FileIDPathIDiterator->second.second;
//			map<INT32, string>::iterator assetsNameIterator
//				= FindAssetsNameFromPathIDOfContainerPath.find(FileIDOfContainerPath);
//			if (assetsNameIterator != FindAssetsNameFromPathIDOfContainerPath.end()) {
//				string assetsName = assetsNameIterator->second;
//				map<string, AssetsFileTable*>::iterator assetsFileTableIterator
//					= FindAssetsFileTablesFromAssetsName.find(assetsName);
//				if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
//					assetsFileTable = assetsFileTableIterator->second;
//				}
//				else {
//					throw new exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
//				}
//			}
//			else {
//				throw new exception("assetsNameIterator == FindAssetsNameFromPathIDOfContainerPath.end()");
//			}
//		}
//		else {
//			throw new exception("FileIDPathIDiterator == FindFileIDPathIDFromContainerPath.end()");
//		}
//	}
//	else {
//		map<string, AssetsFileTable*>::iterator assetsFileTableIterator
//			= FindAssetsFileTablesFromAssetsName.find(WideMultiStringConverter->to_bytes(textAssetMap.assetsName));
//		if (assetsFileTableIterator != FindAssetsFileTablesFromAssetsName.end()) {
//			assetsFileTable = assetsFileTableIterator->second;
//			PathID = _unityL10nToolAPI.FindAssetIndexFromName(assetsFileTable, WideMultiStringConverter->to_bytes(textAssetMap.assetName));
//			if (PathID == -1) {
//				throw new exception("PathID == -1");
//			}
//		}
//		else {
//			throw new exception("assetsFileTableIterator == FindAssetsFileTablesFromAssetsName.end()");
//		}
//	}
//	AssetFileInfoEx* assetFileInfoEx = assetsFileTable->getAssetInfo(PathID);
//	if (assetFileInfoEx == NULL) {
//		throw new exception("assetFileInfoEx == NULL");
//	}
//	AssetTypeInstance* assetTypeInstance
//		= _unityL10nToolAPI.GetBasicAssetTypeInstanceFromAssetFileInfoEx(assetsFileTable, assetFileInfoEx);
//	if (assetTypeInstance == NULL) {
//		throw new exception("assetTypeInstance == NULL");
//	}
//	AssetTypeValueField* pbase = assetTypeInstance->GetBaseField();
//	if (pbase) {
//		wstring m_Script = WideMultiStringConverter->from_bytes(pbase->Get("m_Script")->GetValue()->AsString());
//		return m_Script;
//	}
//	else {
//		throw new exception("pBase == NULL");
//	}
//	throw new exception("UNKNOWN");
//}

TextAssetMap UnityL10nToolCpp::GetOriginalLanguagePairDics(TextAssetMap textAssetMap, bool IsFinal = false)
{
	if (textAssetMap.InteractWithAssetPluginName != L"") {
		map<wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithAssetMap.find(textAssetMap.InteractWithAssetPluginName);
		if (iterator != TextPluginInfoInteractWithAssetMap.end()) {
			/*wstring m_Script = GetOriginalText(textAssetMap);*/
			map<wstring, LanguagePairDic> result = iterator->second->GetOriginalMapFromText(textAssetMap.OriginalText, textAssetMap.languagePairDics);
			textAssetMap.languagePairDics = result;
			if (IsFinal == false) {
				for (vector<TextAssetMap>::iterator iterator = TextAssetMapsGlobal.InteractWithAssetNews.begin();
					iterator != TextAssetMapsGlobal.InteractWithAssetNews.end(); iterator++) {
					if (TextAssetMap::LooseCompare(textAssetMap, *iterator)) {
						*iterator = textAssetMap;
						break;
					}
				}
			}
			else {
				for (vector<TextAssetMap>::iterator iterator = TextAssetMapsGlobal.Done.begin();
					iterator != TextAssetMapsGlobal.Done.end(); iterator++) {
					if (TextAssetMap::LooseCompare(textAssetMap, *iterator)) {
						*iterator = textAssetMap;
						break;
					}
				}
			}
			return textAssetMap;
		}
	}
	else {
		return textAssetMap;
	}
	throw new std::exception("UNKNOWN");
}

TextAssetMap UnityL10nToolCpp::GetTranslatedText(TextAssetMap textAssetMap, bool IsFinal = false)
{
	if (textAssetMap.InteractWithAssetPluginName != L"") {
		map<wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithAssetMap.find(textAssetMap.InteractWithAssetPluginName);
		if (iterator != TextPluginInfoInteractWithAssetMap.end()) {
			wstring result = iterator->second->GetTranslatedTextFromMap(textAssetMap.OriginalText, textAssetMap.languagePairDics);
			textAssetMap.TranslatedText = result;
			for (vector<TextAssetMap>::iterator iterator = TextAssetMapsGlobal.Done.begin();
				iterator != TextAssetMapsGlobal.Done.end(); iterator++) {
				if (TextAssetMap::LooseCompare(textAssetMap, *iterator)) {
					*iterator = textAssetMap;
					break;
				}
			}			
			return textAssetMap;
		}
	}
	return textAssetMap;
}

TextAssetMap UnityL10nToolCpp::GetUpdateFileText(TextAssetMap textAssetMap, bool IsFinal = false)
{
	if (textAssetMap.InteractWithFileTextPluginName != L"") {
		map<wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithFileTextMap.find(textAssetMap.InteractWithFileTextPluginName);
		if (iterator != TextPluginInfoInteractWithFileTextMap.end()) {
			map<wstring, LanguagePairDic> result = iterator->second->GetUpdateFileTextFromMap(textAssetMap.languagePairDics);
			textAssetMap.languagePairDics = result;
			if (IsFinal == false) {
				for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.InteractWithFileTextNews.begin();
					textAssetMapItr != TextAssetMapsGlobal.InteractWithFileTextNews.end(); textAssetMapItr++) {
					if (TextAssetMap::LooseCompare(*textAssetMapItr, textAssetMap)) {
						*textAssetMapItr = textAssetMap;
						break;
					}
				}
			}
			else {
				for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.Done.begin();
					textAssetMapItr != TextAssetMapsGlobal.Done.end(); textAssetMapItr++) {
					if (TextAssetMap::LooseCompare(*textAssetMapItr, textAssetMap)) {
						*textAssetMapItr = textAssetMap;
						break;
					}
				}
			}			
			return textAssetMap;
		}
	}
	else {
		return textAssetMap;
	}
	return textAssetMap;
}

bool UnityL10nToolCpp::SaveUpdateFileToTempFolder(TextAssetMap textAssetMap)
{
	for (std::map<wstring, LanguagePairDic>::iterator iterator = textAssetMap.languagePairDics.begin();
		iterator != textAssetMap.languagePairDics.end(); iterator++) {
		std::wofstream wof;
		wof.clear();
		wof.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
		wstring fileName;
		if (!textAssetMap.ExternalRelativeFilePath.empty()) {
			fileName = iterator->second.TranslatedFileName;
		} else if (textAssetMap.useContainerPath) {
			fileName = ReplaceAll(textAssetMap.containerPath, L"\\", L"_") + iterator->second.TranslatedFileName;
		}
		else {
			fileName = textAssetMap.assetsName + L"_" + textAssetMap.assetName + L"_" + iterator->second.TranslatedFileName;
		}
		wof.open(CurrentDirectory + L"temp\\" + fileName);
		wof << iterator->second.TranslatedFileText;
		wof.close();
	}
	return true;
}

TextAssetMap UnityL10nToolCpp::LoadTranslatedFileTextFromTempAndResourceFolder(TextAssetMap textAssetMap)
{
	for (std::map<wstring, LanguagePairDic>::iterator iterator = textAssetMap.languagePairDics.begin();
		iterator != textAssetMap.languagePairDics.end(); iterator++) {
		wstring fileName;
		if (!textAssetMap.ExternalRelativeFilePath.empty()) {
			fileName = iterator->second.TranslatedFileName;
		} else if (textAssetMap.useContainerPath) {
			fileName = textAssetMap.containerPath;
			fileName = ReplaceAll(fileName, L"/", L"_") + L"_" + iterator->second.TranslatedFileName;
		}
		else {
			fileName = textAssetMap.assetsName + L"_" + textAssetMap.assetName + L"_" + iterator->second.TranslatedFileName;
		}
		wifstream wif(CurrentDirectory + L"temp\\" + fileName);
		if (wif.good()) {
			wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
			std::wstringstream wss;
			wss << wif.rdbuf();
			iterator->second.TranslatedFileText = wss.str();
		}
		else {
			wifstream wif2(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"Resources\\" + fileName);
			if (wif2.good()) {
				wif2.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
				std::wstringstream wss2;
				wss2 << wif2.rdbuf();
				iterator->second.TranslatedFileText = wss2.str();
			}
		}
		wif.close();
	}
	UpdateTextAssetMap(textAssetMap);
	return textAssetMap;
}

bool UnityL10nToolCpp::UpdateTextAssetMap(TextAssetMap textAssetMap) {
	vector<TextAssetMap>* InteractWithAssetNewsPtr = &(TextAssetMapsGlobal.InteractWithAssetNews);
	vector<TextAssetMap>* InteractWithFileTextNewsPtr = &(TextAssetMapsGlobal.InteractWithFileTextNews);
	//vector<TextAssetMap>* InteractWithMonoAssetNewsPtr = &(TextAssetMapsGlobal.InteractWithMonoAssetNews);
	vector<TextAssetMap>* DonePtr = &(TextAssetMapsGlobal.Done);
	vector<vector<TextAssetMap>*> vectorPtrs;
	vectorPtrs.push_back(InteractWithAssetNewsPtr);
	vectorPtrs.push_back(InteractWithFileTextNewsPtr);
	//vectorPtrs.push_back(InteractWithMonoAssetNewsPtr);
	vectorPtrs.push_back(DonePtr);
	for (vector<vector<TextAssetMap>*>::iterator vectorPtr = vectorPtrs.begin();
		vectorPtr != vectorPtrs.end(); vectorPtr++) {
		for (vector<TextAssetMap>::iterator storeVector = (*vectorPtr)->begin();
			storeVector != (*vectorPtr)->end(); storeVector++) {
			if (TextAssetMap::LooseCompare(textAssetMap, *storeVector))
			{
				*storeVector = textAssetMap;
				return true;
			}
		}
	}
	return false;
}

TextAssetMap UnityL10nToolCpp::GetTranslatedLanguagePairDics(TextAssetMap textAssetMap, bool IsFinal = false)
{
	if (textAssetMap.InteractWithFileTextPluginName != L"") {
		map<std::wstring, TextPluginInfo*>::iterator iterator = TextPluginInfoInteractWithFileTextMap.find(textAssetMap.InteractWithFileTextPluginName);
		if (iterator != TextPluginInfoInteractWithFileTextMap.end()) {
			map<std::wstring, LanguagePairDic> result = iterator->second->GetTranslatedMapFromFileText(textAssetMap.languagePairDics);
			textAssetMap.languagePairDics = result;
			if(IsFinal == false) {
				for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.InteractWithFileTextNews.begin();
					textAssetMapItr != TextAssetMapsGlobal.InteractWithFileTextNews.end(); textAssetMapItr++) {
					if (TextAssetMap::LooseCompare(*textAssetMapItr, textAssetMap)) {
						for (map<std::wstring, LanguagePairDic>::iterator lpdItr = textAssetMapItr->languagePairDics.begin();
							lpdItr != textAssetMapItr->languagePairDics.end(); lpdItr++) {
							map<wstring, LanguagePairDic>::iterator lpdResultItr = result.find(lpdItr->first);
							if (lpdResultItr != result.end()) {
								lpdItr->second.AddDicFromTranslated(lpdResultItr->second);
							}
						}
						return *textAssetMapItr;
					}
				}
			}
			else {
				for (vector<TextAssetMap>::iterator textAssetMapItr = TextAssetMapsGlobal.Done.begin();
					textAssetMapItr != TextAssetMapsGlobal.Done.end(); textAssetMapItr++) {
					if (TextAssetMap::LooseCompare(*textAssetMapItr, textAssetMap)) {
						for (map<wstring, LanguagePairDic>::iterator lpdItr = textAssetMapItr->languagePairDics.begin();
							lpdItr != textAssetMapItr->languagePairDics.end(); lpdItr++) {
							map<wstring, LanguagePairDic>::iterator lpdResultItr = result.find(lpdItr->first);
							if (lpdResultItr != result.end()) {
								lpdItr->second.AddDicFromTranslated(lpdResultItr->second);
							}
						}
						return *textAssetMapItr;
					}
				}
			}
		}
	}
	else {
		return textAssetMap;
	}
	return textAssetMap;
}

vector<OnlineResourcePair> UnityL10nToolCpp::GetOnlineResourcePairs()
{
	if (OnlineResourcePairsGlobal.size() == 0) {
		if (projectJson.isMember("OnlineResources")) {
			Json::Value json = projectJson["OnlineResources"];
			if (json.isArray()) {
				for (Json::ArrayIndex i = 0; i < json.size(); i++) {
					OnlineResourcePairsGlobal.push_back(OnlineResourcePair(json[i]));
				}
			}
		}
	}
	return OnlineResourcePairsGlobal;
}

void UnityL10nToolCpp::AddOnlineResoucesPair(OnlineResourcePair onlineResourcePair)
{
	OnlineResourcePairsGlobal.push_back(onlineResourcePair);
}

void UnityL10nToolCpp::SetOnlineResourcePairs(vector<OnlineResourcePair> onlineResourcePairs)
{
	OnlineResourcePairsGlobal = onlineResourcePairs;
	SetOnlineResourcePairsToProjectJson();
}

Json::Value UnityL10nToolCpp::SetOnlineResourcePairsToProjectJson()
{
	Json::Value json(Json::arrayValue);
	for (vector<OnlineResourcePair>::iterator itr = OnlineResourcePairsGlobal.begin();
		itr != OnlineResourcePairsGlobal.end(); itr++) {
		Json::Value jsonChild;
		jsonChild["filePath"] = WideMultiStringConverter->to_bytes(itr->filePath);
		jsonChild["URL"] = WideMultiStringConverter->to_bytes(itr->URL);
		json.append(jsonChild);
	}
	projectJson["OnlineResources"] = json;
	return json;
}

bool DownloadResourcesFromURLToFolder(wstring URL, wstring folderPath) {
	HRESULT hr;
	hr = URLDownloadToFileW(0, URL.c_str(), folderPath.c_str(), 0, 0);
	switch (hr)
	{
	case S_OK:
		cout << "Successful download\n";
		return true;
	case E_OUTOFMEMORY:
		cout << "Out of memory error\n";
		return false;
	case INET_E_DOWNLOAD_FAILURE:
		cout << "Cannot access server data\n";
		return false;
	default:
		cout << "Unknown error\n";
		return false;
	}
}

void UnityL10nToolCpp::DownloadResourcesFromInternetToTempFolder()
{
	DownloadResourcesFromInternetToFolder(CurrentDirectory + L"temp\\");
}

void UnityL10nToolCpp::DownloadResourcesFromInternetToBuildResourceFolder()
{
	if (CreateDirectory((UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"Build\\Resources\\").c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
	}
	DownloadResourcesFromInternetToFolder(UnityL10nToolProjectInfoGlobal.ProjectRelativeFolder + L"Build\\Resources\\");
}

void UnityL10nToolCpp::DownloadResourcesFromInternetToFolder(wstring folderPath)
{
	for (vector<OnlineResourcePair>::iterator onlineResourcePairItr = OnlineResourcePairsGlobal.begin();
		onlineResourcePairItr != OnlineResourcePairsGlobal.end(); onlineResourcePairItr++) {
		if (onlineResourcePairItr->filePath != L"" && onlineResourcePairItr->URL != L"") {
			DownloadResourcesFromURLToFolder(onlineResourcePairItr->URL, folderPath + onlineResourcePairItr->filePath);
		}
	}
}

OnlineUpdate UnityL10nToolCpp::GetOnlineUpdate()
{
	return OnlineUpdateGlobal;
}

void UnityL10nToolCpp::SetOnlineUpdate(OnlineUpdate onlineUpdate)
{
	OnlineUpdateGlobal = onlineUpdate;
	projectJson["OnlineUpdate"] = onlineUpdate.toJson();
}

vector<CopyResourcesManualPair> UnityL10nToolCpp::GetCopyResourcesManualPairs()
{
	if (CopyResourcesManualPairsGlobal.size() == 0) {
		if (projectJson.isMember("CopyResourcesManuals")) {
			Json::Value json = projectJson["CopyResourcesManuals"];
			if (json.isArray()) {
				for (Json::ArrayIndex i = 0; i < json.size(); i++) {
					CopyResourcesManualPairsGlobal.push_back(CopyResourcesManualPair(json[i]));
				}
			}
		}
	}
	return CopyResourcesManualPairsGlobal;
}

void UnityL10nToolCpp::AddCopyResourcesManualPair(CopyResourcesManualPair copyResourcesManualPair)
{
	CopyResourcesManualPairsGlobal.push_back(copyResourcesManualPair);
}

void UnityL10nToolCpp::SetCopyResourcesManualPairs(vector<CopyResourcesManualPair> copyResourcesManualPairs)
{
	CopyResourcesManualPairsGlobal = copyResourcesManualPairs;
	SetCopyResourcesManualPairsToProjectJson();
}

Json::Value UnityL10nToolCpp::SetCopyResourcesManualPairsToProjectJson()
{
	Json::Value json(Json::arrayValue);
	for (vector<CopyResourcesManualPair>::iterator itr = CopyResourcesManualPairsGlobal.begin();
		itr != CopyResourcesManualPairsGlobal.end(); itr++) {
		Json::Value jsonChild;
		jsonChild["ResourcesFileFromProjectFolder"] = WideMultiStringConverter->to_bytes(itr->ResourcesFileFromProjectFolder);
		jsonChild["ResourcesFileTargetRelativePath"] = WideMultiStringConverter->to_bytes(itr->ResourcesFileTargetRelativePath);
		json.append(jsonChild);
	}
	projectJson["CopyResourcesManuals"] = json;
	return json;
}

void UnityL10nToolCpp::CopyResourcesManualPairsForPatcher()
{
	for (vector<CopyResourcesManualPair>::iterator CopyResourcesManualPairItr = CopyResourcesManualPairsGlobal.begin();
		CopyResourcesManualPairItr != CopyResourcesManualPairsGlobal.end(); CopyResourcesManualPairItr++) {
		if (CopyResourcesManualPairItr->ResourcesFileFromProjectFolder != L"" && CopyResourcesManualPairItr->ResourcesFileTargetRelativePath != L"") {
			if (FileExist(CopyResourcesManualPairItr->ResourcesFileFromProjectFolder)) {
				if (FileExist(UnityL10nToolProjectInfoGlobal.GameRootPath() + CopyResourcesManualPairItr->ResourcesFileTargetRelativePath)) {
					DeleteFileW((UnityL10nToolProjectInfoGlobal.GameRootPath() + CopyResourcesManualPairItr->ResourcesFileTargetRelativePath).c_str());
				}
				copyFileCustom((CopyResourcesManualPairItr->ResourcesFileFromProjectFolder).c_str(),
					(UnityL10nToolProjectInfoGlobal.GameRootPath() + CopyResourcesManualPairItr->ResourcesFileTargetRelativePath).c_str());
			}
		}
	}
}

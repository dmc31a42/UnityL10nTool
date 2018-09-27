#pragma once
#include <codecvt> // for std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#include "json/json.h"
using namespace std;

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> WideMultiStringConverter; // #include <codecvt>
static Json::Reader JsonReader;

std::string readFile2(const std::wstring & fileName);
vector<wstring> GetAllFilesFilterWithinAllSubFolder(wstring directory, wstring filter);
std::string ReplaceAll(std::string & str, const std::string & from, const std::string & to);
bool copyFileCustom(const char *SRC, const char* DEST);
bool copyFileCustom(const wchar_t *SRC, const wchar_t* DEST);
std::vector<std::wstring> get_all_files_names_within_folder(std::wstring filter);
bool CreateProcessCustom(std::wstring commandLine);

// https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c#
inline string readFile2(const wstring &fileName)
{
	ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

	ifstream::pos_type fileSize = ifs.tellg();
	if (fileSize < 0)
		return std::string();

	ifs.seekg(0, ios::beg);

	vector<char> bytes(fileSize);
	ifs.read(&bytes[0], fileSize);

	return string(&bytes[0], fileSize);
}

inline bool copyFileCustom(const char *SRC, const char* DEST)
{
	std::ifstream src(SRC, std::ios::binary);
	std::ofstream dest(DEST, std::ios::binary);
	dest << src.rdbuf();
	return src && dest;
}

inline bool copyFileCustom(const wchar_t *SRC, const wchar_t* DEST)
{
	std::ifstream src(SRC, std::ios::binary);
	std::ofstream dest(DEST, std::ios::binary);
	dest << src.rdbuf();
	return src && dest;
}

//https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
inline vector<wstring> get_all_files_names_within_folder(wstring filter)
{
	vector<wstring> names;
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFileW(filter.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

inline vector<wstring> GetAllFilesFilterWithinAllSubFolderRecursive(wstring firstDirectory, wstring subDirectory, wstring filter) {
	vector<wstring> files;
	WIN32_FIND_DATAW fd;
	HANDLE hFind = ::FindFirstFileW((firstDirectory + subDirectory + filter).c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				files.push_back(subDirectory + fd.cFileName);
			}
		} while (::FindNextFileW(hFind, &fd));
		::FindClose(hFind);
	}
	WIN32_FIND_DATAW fdSub;
	HANDLE hFindSub = ::FindFirstFileW((firstDirectory + subDirectory + L"*").c_str(), &fdSub);
	if (hFindSub != INVALID_HANDLE_VALUE) {
		do {
			if (fdSub.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if ((!lstrcmpW(fdSub.cFileName, L".")) || (!lstrcmpW(fdSub.cFileName, L".."))) {
					continue;
				}
				vector<wstring> subFiles = GetAllFilesFilterWithinAllSubFolderRecursive(firstDirectory, subDirectory + fdSub.cFileName + L"\\", filter);
				files.insert(files.end(), subFiles.begin(), subFiles.end());
			}
		} while (FindNextFileW(hFindSub, &fdSub));
	}
	return files;
}

inline vector<wstring> GetAllFilesFilterWithinAllSubFolder(wstring directory, wstring filter) {
	return GetAllFilesFilterWithinAllSubFolderRecursive(directory, L"", filter);
}

inline bool CreateProcessCustom(wstring commandLine) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		(LPWSTR)commandLine.c_str(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
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

inline std::string ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
	size_t start_pos = 0; //string처음부터 검사
	while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
	}
	return str;
}
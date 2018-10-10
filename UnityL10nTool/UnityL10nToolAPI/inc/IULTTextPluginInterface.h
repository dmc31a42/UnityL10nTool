#pragma once
#include <string>
#include <vector>
#include <map>

#include "UnityL10nToolAPI.h"
using namespace std;

struct TextAssetMapOption {
	enum Type
	{
		OPTION_TYPE_NONE = 0,
		OPTION_TYPE_WSTRING = 1,
		OPTION_TYPE_INT = 2,
		OPTION_TYPE_DOUBLE = 3,
		OPTION_TYPE_BOOL = 4
	};
	wstring OptionName;
	wstring OptionDescription;
	void* Value;
	void* ValueAsChild;
	Type type;
	Type typeAsChild;
	vector<TextAssetMapOption> nestedOptions;
};

/* If split one asset to multiple file, set keys as plugin want.
Otherwise set key to "" (empty wstring) */
struct LanguagePairDic {
	map<wstring, LanguagePair> languagePairDic;
	vector<TextAssetMapOption> InteractWithAssetOptions;
	vector<TextAssetMapOption> InteractWithFileTextOptions;
};
typedef map<wstring, LanguagePairDic> LanguagePairDics;

struct TextAssetMap {
	string assetsName;
	string assetName;
	string containerPath;
	string InteractWithAssetPluginName;
	string InteractWithFileTextPluginName;
	string InteractWithMonoAssetPluginName;
	bool useContainerPath;
	LanguagePairDics languagePairDics;
};

struct LanguagePair {
	wstring Original;
	wstring Translated;
};

struct TextPluginInfo;
typedef bool(_cdecl *GetTextPluginInfoCallback)(TextPluginInfo* textPluginInfo);

#pragma region InteractWithAssetCallback
typedef LanguagePairDics(_cdecl *GetAssetParserOptionsCallback)(wstring OriginalText);
typedef LanguagePairDics(_cdecl *GetOriginalMapFromTextCallback)(wstring OriginalText, LanguagePairDics);
typedef wstring(_cdecl *GetTranslatedTextFromMapCallback)(LanguagePairDics TranslatedMap, wstring OriginalText);
#pragma endregion

#pragma region InteractWithFileCallback
typedef LanguagePairDics(_cdecl *GetFileParserOptionsCallback)(LanguagePairDics UpdateMap);
typedef wstring(_cdecl *GetUpdateFileTextFromMapCallback)(LanguagePairDics UpdateMap);
typedef LanguagePairDics(_cdecl *GetTranslatedMapFromFileTextCallback)(wstring TranslatedFileText, LanguagePairDics);
#pragma endregion

struct TextPluginInfo {

	wchar_t TextPluginName[64];

#pragma region InteractWithAsset
	GetAssetParserOptionsCallback GetAssetParserOptions;
	GetOriginalMapFromTextCallback GetOriginalMapFromText;
	GetTranslatedTextFromMapCallback GetTranslatedTextFromMap;
#pragma endregion

#pragma region InteractWithFile
	GetFileParserOptionsCallback GetFileParserOptions;
	GetUpdateFileTextFromMapCallback GetUpdateFileTextFromMap;
	GetTranslatedMapFromFileTextCallback GetTranslatedMapFromFileText;
#pragma endregion

};

struct MonoTextPluginInfo;
typedef bool(_cdecl *GetMonoTextPluginInfoCallback)(UnityL10nToolAPI* unityL10nToolAPI, MonoTextPluginInfo* monoTextPluginInfo);

typedef vector<TextAssetMap>(_cdecl *GetMonoTextAssetMapCallback)();
typedef TextAssetMap(_cdecl *GetOriginalMapFromMonoTextCallback)(TextAssetMap OriginalMap);
typedef pair<string, AssetsReplacer*>(_cdecl *GetAssetReplacerForTranslatedTextFromMapCallback)(TextAssetMap TranslatedMap);

struct MonoTextPluginInfo {
	wchar_t MonoTextPluginInfo[64];
	
	GetMonoTextAssetMapCallback GetMonoTextAssetMap;
	GetOriginalMapFromMonoTextCallback GetOriginalMapFromMonoText;
	GetAssetReplacerForTranslatedTextFromMapCallback GetAssetReplacerForTranslatedTextFromMap;
};
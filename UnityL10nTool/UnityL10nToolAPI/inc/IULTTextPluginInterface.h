#pragma once
#include <string>
#include <vector>
#include <map>

#include "IULTPluginCommonInterface.h"
using namespace std;

struct LanguagePair {
	wstring Original;
	wstring Translated;
};
/* If split one asset to multiple file, set keys as plugin want.
Otherwise set key to "" (empty wstring) */
struct LanguagePairDic {
	map<wstring, LanguagePair> Dic;
	map<wstring, wstring> TranslatedText;
	vector<AssetMapOption> InteractWithAssetOptions;
	vector<AssetMapOption> InteractWithFileTextOptions;
};
typedef map<wstring, LanguagePairDic> LanguagePairDics;

struct TextAssetMap {
	wstring assetsName;
	wstring assetName;
	wstring containerPath;
	wstring InteractWithAssetPluginName;
	wstring InteractWithFileTextPluginName;
	wstring InteractWithMonoAssetPluginName;
	bool useContainerPath;
	wstring OriginalText;
	LanguagePairDics languagePairDics;
};

struct TextAssetMaps {
	vector<TextAssetMap> InteractWithAssetNews;
	vector<TextAssetMap> InteractWithFileTextNews;
	vector<TextAssetMap> Done;
};

struct TextPluginInfo;
typedef bool(_cdecl *GetTextPluginInfoCallback)(TextPluginInfo* textPluginInfo);

#pragma region InteractWithAssetCallback
typedef LanguagePairDics(_cdecl *GetAssetParserOptionsCallback)(wstring OriginalText);
typedef LanguagePairDics(_cdecl *GetOriginalMapFromTextCallback)(wstring OriginalText, LanguagePairDics languagePairDics);
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
	/* Member of InteractWithAsset. temperary deprecated*/
	GetAssetParserOptionsCallback GetAssetParserOptions;
	/* Member of InteractWithAsset*/
	GetOriginalMapFromTextCallback GetOriginalMapFromText;
	/* Member of InteractWithAsset*/
	GetTranslatedTextFromMapCallback GetTranslatedTextFromMap;
#pragma endregion

#pragma region InteractWithFile
	/* Member of InteractWithFile. temperary deprecated*/
	GetFileParserOptionsCallback GetFileParserOptions;
	/* Member of InteractWithFile*/
	GetUpdateFileTextFromMapCallback GetUpdateFileTextFromMap;
	/* Member of InteractWithFile*/
	GetTranslatedMapFromFileTextCallback GetTranslatedMapFromFileText;
#pragma endregion

};

struct MonoTextPluginInfo;
typedef bool(_cdecl *GetMonoTextPluginInfoCallback)(UnityL10nToolAPI* unityL10nToolAPI, MonoTextPluginInfo* monoTextPluginInfo);
typedef vector<TextAssetMap>(_cdecl *GetMonoTextAssetMapCallback)();
typedef TextAssetMap(_cdecl *GetOriginalMapFromMonoTextCallback)(TextAssetMap OriginalMap);
typedef pair<string, AssetsReplacer*>(_cdecl *GetAssetReplacerForTranslatedTextFromMapCallback)(TextAssetMap TranslatedMap);

struct MonoTextPluginInfo {
	wchar_t MonoTextPluginName[64];

	GetMonoTextAssetMapCallback GetMonoTextAssetMap;
	GetOriginalMapFromMonoTextCallback GetOriginalMapFromMonoText;
	GetAssetReplacerForTranslatedTextFromMapCallback GetAssetReplacerForTranslatedTextFromMap;
};
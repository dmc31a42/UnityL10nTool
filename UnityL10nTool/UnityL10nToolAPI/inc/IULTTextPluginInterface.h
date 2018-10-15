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
public:
	map<wstring, LanguagePair> Dic;
	map<wstring, wstring> TranslatedText;
	vector<AssetMapOption> InteractWithAssetOptions;
	vector<AssetMapOption> InteractWithFileTextOptions;
	Json::Value ToJSON() {
		Json::Value result;
		Json::Value InteractWithAssetOptionsJson(Json::arrayValue);
		Json::Value InteractWithFileTextOptionsJson(Json::arrayValue);
		for (vector<AssetMapOption>::iterator iterator = this->InteractWithAssetOptions.begin();
			iterator != this->InteractWithAssetOptions.end(); iterator++) {
			InteractWithAssetOptionsJson.append(iterator->ToJson());
		}
		for (vector<AssetMapOption>::iterator iterator = this->InteractWithFileTextOptions.begin();
			iterator != this->InteractWithFileTextOptions.end(); iterator++) {
			InteractWithFileTextOptionsJson.append(iterator->ToJson());
		}
		result["InteractWithAssetOptions"] = InteractWithAssetOptionsJson;
		result["InteractWithFileTextOptions"] = InteractWithFileTextOptionsJson;
		return result;
	}
	LanguagePairDic() {}
	LanguagePairDic(Json::Value json) {
		Json::Value InteractWithAssetOptionsJson = json["InteractWithAssetOptions"];
		Json::Value InteractWithFileTextOptionsJson = json["InteractWithFileTextOptions"];
		for (Json::ArrayIndex i = 0; i < InteractWithAssetOptionsJson.size(); i++) {
			this->InteractWithAssetOptions.push_back(AssetMapOption(InteractWithAssetOptionsJson[i]));
		}
		for (Json::ArrayIndex i = 0; i < InteractWithFileTextOptionsJson.size(); i++) {
			this->InteractWithFileTextOptions.push_back(AssetMapOption(InteractWithFileTextOptionsJson[i]));
		}
	}
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
	Json::Value ToJSON() {
		Json::Value result;
		result["assetsName"] = WideMultiStringConverter.to_bytes(this->assetsName);
		result["assetName"] = WideMultiStringConverter.to_bytes(this->assetName);
		result["containerPath"] = WideMultiStringConverter.to_bytes(this->containerPath);
		result["InteractWithAssetPluginName"] = WideMultiStringConverter.to_bytes(this->InteractWithAssetPluginName);
		result["InteractWithFileTextPluginName"] = WideMultiStringConverter.to_bytes(this->InteractWithFileTextPluginName);
		result["InteractWithMonoAssetPluginName"] = WideMultiStringConverter.to_bytes(this->InteractWithMonoAssetPluginName);
		result["useContainerPath"] = this->useContainerPath;
		for (LanguagePairDics::iterator iterator = this->languagePairDics.begin();
			iterator != this->languagePairDics.end(); iterator++) {
			result["languagePairDics"][WideMultiStringConverter.to_bytes(iterator->first)] = iterator->second.ToJSON();
		}
		return result;
	}
	TextAssetMap() {}
	TextAssetMap(Json::Value json) {
		this->assetsName = WideMultiStringConverter.from_bytes(json["assetsName"].asString());
		this->assetName = WideMultiStringConverter.from_bytes(json["assetName"].asString());
		this->containerPath = WideMultiStringConverter.from_bytes(json["containerPath"].asString());
		this->InteractWithAssetPluginName = WideMultiStringConverter.from_bytes(json["InteractWithAssetPluginName"].asString());
		this->InteractWithFileTextPluginName = WideMultiStringConverter.from_bytes(json["InteractWithFileTextPluginName"].asString());
		this->InteractWithMonoAssetPluginName = WideMultiStringConverter.from_bytes(json["InteractWithMonoAssetPluginName"].asString());
		this->useContainerPath = json["useContainerPath"].asBool();
		Json::Value languagePairDicsJson = json["languagePairDics"];
		vector<string> languagePairDicsJsonKeys = languagePairDicsJson.getMemberNames();
		for (vector<string>::iterator iterator = languagePairDicsJsonKeys.begin();
			iterator != languagePairDicsJsonKeys.end(); iterator++) {
			this->languagePairDics.insert(pair<wstring, LanguagePairDic>(
				WideMultiStringConverter.from_bytes(*iterator),
				LanguagePairDic(languagePairDicsJson[*iterator])
				));
		}
	}
};

struct TextAssetMaps {
	vector<TextAssetMap> InteractWithAssetNews;
	vector<TextAssetMap> InteractWithFileTextNews;
	vector<TextAssetMap> Done;
	Json::Value ToJSON() {
		Json::Value result;
		Json::Value InteractWithFileTextNewsJson(Json::arrayValue);
		Json::Value DoneJson(Json::arrayValue);
		for (vector<TextAssetMap>::iterator iterator = InteractWithFileTextNews.begin();
			iterator != InteractWithFileTextNews.end(); iterator++) {
			InteractWithFileTextNewsJson.append(iterator->ToJSON());
		}
		for (vector<TextAssetMap>::iterator iterator = Done.begin();
			iterator != Done.end(); iterator++) {
			DoneJson.append(iterator->ToJSON());
		}
		result["InteractWithFileTextNews"] = InteractWithFileTextNewsJson;
		result["Done"] = DoneJson;
		return result;
	}
	TextAssetMaps() {}
	TextAssetMaps(Json::Value json) {
		for (Json::ArrayIndex i = 0; i < json["InteractWithFileTextNews"].size(); i++) {
			this->InteractWithFileTextNews.push_back(TextAssetMap(json["InteractWithFileTextNews"][i]));
		}
		for (Json::ArrayIndex i = 0; i < json["Done"].size(); i++) {
			this->Done.push_back(TextAssetMap(json["Done"][i]));
		}
	}
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
#pragma once

#include <string>
#include <vector>
#include <map>

#include "IULTPluginCommonInterface.h"

struct LanguagePair {
	std::wstring Original;
	std::wstring Translated;
};
///* If split one asset to multiple file, set keys as plugin want.
//Otherwise set key to "" (empty std::wstring) */
struct LanguagePairDic {
public:
	std::map<std::wstring, LanguagePair> Dic;
	std::wstring TranslatedFileText;
	std::wstring TranslatedFileName;
	std::vector<AssetMapOption> InteractWithAssetOptions;
	std::vector<AssetMapOption> InteractWithFileTextOptions;
	Json::Value ToJSON() {
		Json::Value result;
		Json::Value InteractWithAssetOptionsJson(Json::arrayValue);
		Json::Value InteractWithFileTextOptionsJson(Json::arrayValue);
		result["TranslatedFileName"] = WideMultiStringConverter->to_bytes(TranslatedFileName);
		for (std::vector<AssetMapOption>::iterator iterator = this->InteractWithAssetOptions.begin();
			iterator != this->InteractWithAssetOptions.end(); iterator++) {
			InteractWithAssetOptionsJson.append(iterator->ToJson());
		}
		for (std::vector<AssetMapOption>::iterator iterator = this->InteractWithFileTextOptions.begin();
			iterator != this->InteractWithFileTextOptions.end(); iterator++) {
			InteractWithFileTextOptionsJson.append(iterator->ToJson());
		}
		result["InteractWithAssetOptions"] = InteractWithAssetOptionsJson;
		result["InteractWithFileTextOptions"] = InteractWithFileTextOptionsJson;
		return result;
	}
	LanguagePairDic() {}
	LanguagePairDic(Json::Value json) {
		this->TranslatedFileName = WideMultiStringConverter->from_bytes(json["TranslatedFileName"].asString());
		Json::Value InteractWithAssetOptionsJson = json["InteractWithAssetOptions"];
		Json::Value InteractWithFileTextOptionsJson = json["InteractWithFileTextOptions"];
		for (Json::ArrayIndex i = 0; i < InteractWithAssetOptionsJson.size(); i++) {
			this->InteractWithAssetOptions.push_back(AssetMapOption((Json::Value)InteractWithAssetOptionsJson[i]));
		}
		for (Json::ArrayIndex i = 0; i < InteractWithFileTextOptionsJson.size(); i++) {
			this->InteractWithFileTextOptions.push_back(AssetMapOption((Json::Value)InteractWithFileTextOptionsJson[i]));
		}
	}
	bool AddDicFromTranslated(LanguagePairDic other) {
		std::map<std::wstring, LanguagePair> tempDic;
		for (std::map<std::wstring, LanguagePair>::iterator iterator = this->Dic.begin(); iterator != this->Dic.end(); iterator++) {
			std::map<std::wstring, LanguagePair>::iterator otherDicItr = other.Dic.find(iterator->first);
			if (otherDicItr != other.Dic.end()) {
				iterator->second.Translated = otherDicItr->second.Translated;
			}
			else {
				tempDic.insert(std::pair<std::wstring, LanguagePair>(otherDicItr->first,otherDicItr->second));
			}
		}
		for (std::map<std::wstring, LanguagePair>::iterator iterator = tempDic.begin();
			iterator != tempDic.end(); iterator++) {
			this->Dic.insert(std::pair<std::wstring, LanguagePair>(iterator->first, iterator->second));
		}
		return true;
	}
};
typedef std::map<std::wstring, LanguagePairDic> LanguagePairDics;

struct TextAssetMap {
	enum ToWhere {
		None = 0,
		ToInteractWithAsset = 1,
		ToInteractWithFileText = 2,
		ToInteractWithMonoAsset = 3,
		ToDone = 4,
	};
	std::wstring assetsName;
	std::wstring assetName;
	std::wstring containerPath;
	std::wstring InteractWithAssetPluginName;
	std::wstring InteractWithFileTextPluginName;
	std::wstring InteractWithMonoAssetPluginName;
	bool useContainerPath;
	std::wstring OriginalText;
	std::wstring TranslatedText;
	bool IsFileTextMerged;
	std::vector<AssetMapOption> InteractWithFileTextMergedOptions;
	LanguagePairDics languagePairDics;
	Json::Value ToJSON() {
		Json::Value result;
		result["assetsName"] = WideMultiStringConverter->to_bytes(this->assetsName);
		result["assetName"] = WideMultiStringConverter->to_bytes(this->assetName);
		result["containerPath"] = WideMultiStringConverter->to_bytes(this->containerPath);
		result["InteractWithAssetPluginName"] = WideMultiStringConverter->to_bytes(this->InteractWithAssetPluginName);
		result["InteractWithFileTextPluginName"] = WideMultiStringConverter->to_bytes(this->InteractWithFileTextPluginName);
		result["InteractWithMonoAssetPluginName"] = WideMultiStringConverter->to_bytes(this->InteractWithMonoAssetPluginName);
		result["useContainerPath"] = this->useContainerPath;
		for (LanguagePairDics::iterator iterator = this->languagePairDics.begin(); iterator != this->languagePairDics.end(); iterator++) {
			result["languagePairDics"][WideMultiStringConverter->to_bytes(iterator->first)] = iterator->second.ToJSON();
		}
		return result;
	}
	TextAssetMap() {}
	TextAssetMap(Json::Value json) {
		this->assetsName = WideMultiStringConverter->from_bytes(json["assetsName"].asString());
		this->assetName = WideMultiStringConverter->from_bytes(json["assetName"].asString());
		this->containerPath = WideMultiStringConverter->from_bytes(json["containerPath"].asString());
		this->InteractWithAssetPluginName = WideMultiStringConverter->from_bytes(json["InteractWithAssetPluginName"].asString());
		this->InteractWithFileTextPluginName = WideMultiStringConverter->from_bytes(json["InteractWithFileTextPluginName"].asString());
		this->InteractWithMonoAssetPluginName = WideMultiStringConverter->from_bytes(json["InteractWithMonoAssetPluginName"].asString());
		this->useContainerPath = json["useContainerPath"].asBool();
		Json::Value languagePairDicsJson = json["languagePairDics"];
		std::vector<std::string> languagePairDicsJsonKeys = languagePairDicsJson.getMemberNames();
		for (std::vector<std::string>::iterator iterator = languagePairDicsJsonKeys.begin(); iterator != languagePairDicsJsonKeys.end(); iterator++) {
			this->languagePairDics.insert(std::pair<std::wstring, LanguagePairDic>(
				WideMultiStringConverter->from_bytes(*iterator),
				LanguagePairDic(languagePairDicsJson[*iterator])
				));
		}
	}
	static bool LooseCompare(TextAssetMap tam1, TextAssetMap tam2) {
		if (tam1.assetsName == tam2.assetsName &&
			tam1.assetName == tam2.assetName &&
			tam1.containerPath == tam2.containerPath) {
			return true;
		}
		else {
			return false;
		}
	}
};

struct TextAssetMaps {
	std::vector<TextAssetMap> InteractWithAssetNews;
	std::vector<TextAssetMap> InteractWithFileTextNews;
	std::vector<TextAssetMap> Done;
	Json::Value ToJSON() {
		Json::Value result;
		Json::Value InteractWithFileTextNewsJson(Json::arrayValue);
		Json::Value DoneJson(Json::arrayValue);
		for (std::vector<TextAssetMap>::iterator iterator = InteractWithFileTextNews.begin(); iterator != InteractWithFileTextNews.end(); iterator++) {
			InteractWithFileTextNewsJson.append(iterator->ToJSON());
		}
		for (std::vector<TextAssetMap>::iterator iterator = Done.begin(); iterator != Done.end(); iterator++) {
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
typedef LanguagePairDics(_cdecl *GetAssetParserOptionsCallback)(std::wstring OriginalText);
typedef LanguagePairDics(_cdecl *GetOriginalMapFromTextCallback)(std::wstring OriginalText, LanguagePairDics languagePairDics);
typedef std::wstring(_cdecl *GetTranslatedTextFromMapCallback)(std::wstring OriginalText, LanguagePairDics TranslatedMap);
#pragma endregion

#pragma region InteractWithFileCallback
typedef LanguagePairDics(_cdecl *GetFileParserOptionsCallback)(LanguagePairDics UpdateMap);
typedef LanguagePairDics(_cdecl *GetUpdateFileTextFromMapCallback)(LanguagePairDics UpdateMap);
typedef LanguagePairDics(_cdecl *GetUpdateFileTextMergedFromMapCallback)(LanguagePairDics UpdateMap, std::vector<AssetMapOption> assetMapOptions);
typedef LanguagePairDics(_cdecl *GetTranslatedMapFromFileTextCallback)(LanguagePairDics translatedMap);
typedef LanguagePairDics(_cdecl *GetTranslatedMapFromFileTextMergedCallback)(std::map<std::wstring, std::wstring> TranslatedFileNameText, LanguagePairDics translatedMap, std::vector<AssetMapOption> assetMapOptions);
#pragma endregion

struct TextPluginInfo {

	wchar_t TextPluginName[64];

	std::wstring TextPluginFileRelativePath;
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
typedef std::vector<TextAssetMap>(_cdecl *GetMonoTextAssetMapCallback)();
typedef TextAssetMap(_cdecl *GetOriginalMapFromMonoTextCallback)(TextAssetMap OriginalMap);
typedef TextAssetMap(_cdecl *GetTranslatedTextFromMonoTextMapCallback)(TextAssetMap TranslatedMap);
typedef std::pair<std::string, AssetsReplacer*>(_cdecl *GetAssetReplacerForTranslatedTextFromMapCallback)(TextAssetMap TranslatedMap);

struct MonoTextPluginInfo {
	wchar_t MonoTextPluginName[64];

	GetMonoTextAssetMapCallback GetMonoTextAssetMap;
	GetOriginalMapFromMonoTextCallback GetOriginalMapFromMonoText;
	GetTranslatedTextFromMonoTextMapCallback GetTranslatedTextFromMonoTextMap;
	GetAssetReplacerForTranslatedTextFromMapCallback GetAssetReplacerForTranslatedTextFromMap;
};
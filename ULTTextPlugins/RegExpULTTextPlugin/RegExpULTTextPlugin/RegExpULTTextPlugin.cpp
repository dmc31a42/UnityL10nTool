// RegExpULTTextPlugin.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
using namespace std;
#include <string>
#include <algorithm>
#include <regex>

#include "IULTPluginCommonInterface.h"
#include "IULTTextPluginInterface.h"
#include "GeneralPurposeFunctions.h"
#include "json/json.h"

TextPluginInfo* TextPluginInfoGlobal;

LanguagePairDics _cdecl GetOriginalMapFromText(wstring OriginalText, LanguagePairDics languagePairDics) {
	if (languagePairDics.size() == 0) {
		languagePairDics.insert(pair<wstring,LanguagePairDic>(L"0", LanguagePairDic()));
	}
	for (map<wstring, LanguagePairDic>::iterator lpdItr = languagePairDics.begin();
		lpdItr != languagePairDics.end(); lpdItr++) {
		if (lpdItr->second.InteractWithAssetOptions.size() != 5) {
			lpdItr->second.InteractWithAssetOptions.clear();
			AssetMapOption firstRegExp = AssetMapOption(
				L"RegExp Match expression",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keySelectOption = AssetMapOption(
				L"Select Key Group",
				L"Select which group is key.",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_INT,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption valueSelectOption = AssetMapOption(
				L"Select Value Group",
				L"Select which group is value.",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_INT,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keyRexExpReplaceFrom = AssetMapOption(
				L"Key Replace [0] From",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption keyRexExpReplaceTo = AssetMapOption(
				L"Key Replace [0] To",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			keyRexExpReplaceFrom.nestedOptions.push_back(keyRexExpReplaceTo);

			AssetMapOption valueRexExpReplaceFrom = AssetMapOption(
				L"Value Replace [0] From",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			AssetMapOption valueRexExpReplaceTo = AssetMapOption(
				L"Value Replace [0] To",
				L"",
				NULL,
				NULL,
				AssetMapOption::OPTION_TYPE_WSTRING,
				AssetMapOption::OPTION_TYPE_NONE,
				vector<AssetMapOption>()
			);
			valueRexExpReplaceFrom.nestedOptions.push_back(valueRexExpReplaceTo);

			lpdItr->second.InteractWithAssetOptions.push_back(firstRegExp);
			lpdItr->second.InteractWithAssetOptions.push_back(keySelectOption);
			lpdItr->second.InteractWithAssetOptions.push_back(valueSelectOption);
			lpdItr->second.InteractWithAssetOptions.push_back(keyRexExpReplaceFrom);
			lpdItr->second.InteractWithAssetOptions.push_back(valueRexExpReplaceFrom);
		} else {
			AssetMapOption firstRegExp = lpdItr->second.InteractWithAssetOptions[0];
			
			if (firstRegExp.Value && *(wstring*)firstRegExp.Value != L"") {
				wregex firstRegex(*(wstring*)firstRegExp.Value);
				wsmatch firstTest;
				regex_match(OriginalText, firstTest, firstRegex);
				if (firstTest.size() >= 3) {
					if (firstTest.size() - 1 != lpdItr->second.InteractWithAssetOptions[1].nestedOptions.size()) {
						lpdItr->second.InteractWithAssetOptions[1].nestedOptions.clear();
						for (int i = 1; i < firstTest.size(); i++) {
							AssetMapOption keySelectOptionChild = AssetMapOption(
								L"",
								L"",
								new int(i),
								NULL,
								AssetMapOption::OPTION_TYPE_NONE,
								AssetMapOption::OPTION_TYPE_INT,
								vector<AssetMapOption>()
							);
							lpdItr->second.InteractWithAssetOptions[1].nestedOptions.push_back(keySelectOptionChild);
						}
						lpdItr->second.InteractWithAssetOptions[2].nestedOptions.clear();
						for (int i = 1; i < firstTest.size(); i++) {
							AssetMapOption valueSelectOptionChild = AssetMapOption(
								L"",
								L"",
								new int(i),
								NULL,
								AssetMapOption::OPTION_TYPE_NONE,
								AssetMapOption::OPTION_TYPE_INT,
								vector<AssetMapOption>()
							);
							lpdItr->second.InteractWithAssetOptions[2].nestedOptions.push_back(valueSelectOptionChild);
						}
					}
					// Recursive로 수정해야함.
					AssetMapOption keyRexExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[3];
					AssetMapOption keyRexExpReplaceTo = keyRexExpReplaceFrom.nestedOptions[0];
					AssetMapOption valueRexExpReplaceFrom = lpdItr->second.InteractWithAssetOptions[4];
					AssetMapOption valueRexExpReplaceTo = keyRexExpReplaceFrom.nestedOptions[0];
					// 
					AssetMapOption keySelectOption = lpdItr->second.InteractWithAssetOptions[1];
					AssetMapOption valueSelectOption = lpdItr->second.InteractWithAssetOptions[2];
					if (keySelectOption.Value && valueSelectOption.Value &&
						*(int*)keySelectOption.Value != *(int*)valueSelectOption.Value) {
						for (wsregex_iterator it(OriginalText.begin(), OriginalText.end(), firstRegex);
							it != wsregex_iterator(); ++it) {
							wstring key = (*it)[*(int*)keySelectOption.Value].str();
							wstring value = (*it)[*(int*)valueSelectOption.Value].str();
							// Recursive로 key, value의 replace를 수행
						}
					}
				}
			}
		}
	}
	return languagePairDics;
}

wstring _cdecl GetTranslatedTextFromMap(wstring OriginalText, LanguagePairDics TranslatedMap) {

}

LanguagePairDics _cdecl GetUpdateFileTextFromMap(LanguagePairDics UpdateMap) {

}

LanguagePairDics _cdecl GetTranslatedMapFromFileText(LanguagePairDics translatedMap) {

}

bool _cdecl GetTextPluginInfo(TextPluginInfo* textPluginInfo) {
	TextPluginInfoGlobal = textPluginInfo;

	wcsncpy_s(textPluginInfo->TextPluginName, L"Json", 4);
	textPluginInfo->GetOriginalMapFromText = GetOriginalMapFromText;
	textPluginInfo->GetTranslatedTextFromMap = GetTranslatedTextFromMap;

	textPluginInfo->GetUpdateFileTextFromMap = GetUpdateFileTextFromMap;
	textPluginInfo->GetTranslatedMapFromFileText = GetTranslatedMapFromFileText;
	return true;
}
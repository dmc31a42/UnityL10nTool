#pragma once
#define GENERALPURPOSEFUNCTIONSSTATIC
#include "..\UnityL10nToolCpp\\UnityL10nToolCpp.h"
#include <map>


using namespace System::Collections::Generic;
using namespace System;

namespace UnityL10nToolCppCLI {
	public ref struct FontAssetMapCLI {
		String^ assetsName;
		String^ assetName;
		String^ containerPath;
		List<String^>^ options;
		String^ selectedOption;
		bool useContainerPath;
	};

	public ref class UnityL10nToolCppManaged
	{
	public:
		static bool DetermineUnityGameFolder(String ^ path);
		static bool DetermineProjectGamePath(String^ path, String^ GameName, String^ MakerName);
		static String^ FindUnityGameFolderFromDataFolderName(String ^ dataFolderName, String ^ GameName, String ^ MakerName);
		
		UnityL10nToolCppManaged(String^ gameFolderPath);
		Dictionary<String^, List<FontAssetMapCLI^>^>^ GetPluginsSupportAssetMap();
		bool LoadGlobalgamemanagersFile();
		bool LoadMonoClassDatabase();
		bool LoadUnityL10nToolAPI();
		List<String^>^ LoadFontPlugins();

		~UnityL10nToolCppManaged();

	protected:
		UnityL10nToolCpp *unityL10nToolCpp;	
	};
}

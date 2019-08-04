#pragma once

AssetsReplacer* ReplaceOriginalMono(AssetTypeValueField* monoOrigBaseATVF, const INT64& newMonoPathID, AssetsFileTable* assetsFileTable, AssetFileInfoEx* MonoAFIEx, AssetTypeInstance* MonoOrigATI);

void ReplaceOriginalMonoInternal(AssetTypeValueField* monoOrigBaseATVF, const INT64& newMonoPathID, const char* fallbackFontAssetFieldName);

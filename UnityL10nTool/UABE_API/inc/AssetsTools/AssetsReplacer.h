#pragma once
#include "defines.h"
#include "AssetsFileReader.h"

enum AssetsReplacementType
{
	AssetsReplacement_AddOrModify,
	AssetsReplacement_Remove
};
class AssetsReplacer
{
	public:
		virtual AssetsReplacementType GetType() = 0;
		virtual ~AssetsReplacer()
			#ifndef ASSETSTOOLS_EXPORTS
			= 0
			#endif
			;

		virtual DWORD GetFileID() = 0;
		virtual QWORD GetPathID() = 0;
		virtual int GetClassID() = 0;
		virtual WORD GetMonoScriptID() = 0;
		//For add and modify
		virtual QWORD GetSize() = 0;
		virtual QWORD Write(QWORD pos, IAssetsWriter *pWriter) = 0;
		//always writes 0 for the file id
		virtual QWORD WriteReplacer(QWORD pos, IAssetsWriter *pWriter) = 0;
};

ASSETSTOOLS_API AssetsReplacer *ReadAssetsReplacer(QWORD &pos, IAssetsReader *pReader, bool prefReplacerInMemory = false);
ASSETSTOOLS_API AssetsReplacer *MakeAssetRemover(DWORD fileID, QWORD pathID, int classID, WORD monoScriptIndex = 0xFFFF);
ASSETSTOOLS_API AssetsReplacer *MakeAssetModifierFromReader(DWORD fileID, QWORD pathID, int classID, WORD monoScriptIndex, 
		IAssetsReader *pReader, QWORD size, QWORD readerPos=0, 
		size_t copyBufferLen=0, bool freeReader=false);
ASSETSTOOLS_API AssetsReplacer *MakeAssetModifierFromMemory(DWORD fileID, QWORD pathID, int classID, WORD monoScriptIndex, void *buffer, size_t size, cbFreeMemoryResource freeResourceCallback);
ASSETSTOOLS_API AssetsReplacer *MakeAssetModifierFromFile(DWORD fileID, QWORD pathID, int classID, WORD monoScriptIndex, FILE *pFile, QWORD offs, QWORD size, size_t copyBufferLen=0, bool freeFile=true);
ASSETSTOOLS_API void FreeAssetsReplacer(AssetsReplacer *pReplacer);
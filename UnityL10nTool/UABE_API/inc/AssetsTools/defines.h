#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <fstream>

#ifndef POINTERTYPE
typedef unsigned __int64 QWORD;
#ifdef WIN64
#define POINTERTYPE QWORD
#else
#define POINTERTYPE DWORD
#endif
#endif

#ifdef ASSETSTOOLS_EXPORTS
#define ASSETSTOOLS_API __declspec(dllexport) 
#else
#define ASSETSTOOLS_API __declspec(dllimport) 
typedef unsigned __int64 QWORD;
#endif

#ifndef __AssetsTools_AssetsFileFunctions_Read
#define __AssetsTools_AssetsFileFunctions_Read
typedef void(_cdecl *AssetsFileVerifyLogger)(char *message);
#endif

#ifndef __AssetsTools_AssetsReplacerFunctions_FreeCallback
#define __AssetsTools_AssetsReplacerFunctions_FreeCallback
typedef void(_cdecl *cbFreeMemoryResource)(void *pResource);
typedef void(_cdecl *cbFreeReaderResource)(class IAssetsReader *pReader);
#endif
#ifndef __AssetsTools_Hash128
#define __AssetsTools_Hash128
union Hash128
{
	BYTE bValue[16];
	WORD wValue[8];
	DWORD dValue[4];
	QWORD qValue[2];
};
#endif

#ifndef _SearchTree_NodeStruct
#define _SearchTree_NodeStruct
template<typename V>
struct BinarySearchTree_LookupTreeNode
{
	BinarySearchTree_LookupTreeNode<V> *pLeft;
	BinarySearchTree_LookupTreeNode<V> *pMiddle; //Special path for duplicate values.
	BinarySearchTree_LookupTreeNode<V> *pRight;
	unsigned int listIndex;
	V value;
};
#endif
#include "AssetsFileReader.h"
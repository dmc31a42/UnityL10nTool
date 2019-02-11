#pragma once
#include <string>
#include "json/json.h"
class TMPFontResource
{
private:
	std::wstring NoExtName;
public:
	std::wstring FileName;
	std::wstring FontFamilyName;
	int m_Width;
	int m_Height;
	int m_CompleteImageSize;
	TMPFontResource();
	TMPFontResource(std::wstring FileName, Json::Value json);
	std::wstring GetMonoFileName();
	std::wstring GetResSFileName();
	~TMPFontResource();
};


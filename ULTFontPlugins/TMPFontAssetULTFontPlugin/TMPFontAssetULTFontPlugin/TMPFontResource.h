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

TMPFontResource::TMPFontResource()
{
}

TMPFontResource::TMPFontResource(std::wstring FileName, Json::Value json)
{
	this->FileName = FileName;
	this->FontFamilyName = WideMultiStringConverter->from_bytes(json["FontFamilyName"].asString());
	this->m_Width = json["m_Width"].asInt();
	this->m_Height = json["m_Height"].asInt();
	this->m_CompleteImageSize = json["m_CompleteImageSize"].asInt();

	size_t itrDot = this->FileName.find(L".Font.json", 0);
	if (itrDot <= 0) {
		throw std::exception("TMP font resource file name is wrong");
	}
	this->NoExtName = this->FileName.substr(0, itrDot);
}

std::wstring TMPFontResource::GetMonoFileName()
{
	return this->NoExtName + L".Mono.json";
}

std::wstring TMPFontResource::GetResSFileName()
{
	return this->NoExtName + L".assets.resS";
}


TMPFontResource::~TMPFontResource()
{
}

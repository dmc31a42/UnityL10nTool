#include <codecvt>
#include "TMPFontResource.h"
#include "GeneralPurposeFunctions.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf8conv;

TMPFontResource::TMPFontResource()
{
}

TMPFontResource::TMPFontResource(std::wstring FileName, Json::Value json)
{
	this->FileName = FileName;
	this->FontFamilyName = utf8conv.from_bytes(json["FontFamilyName"].asString());
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

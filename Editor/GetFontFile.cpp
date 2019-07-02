#include "stdafx.h"
#include "GetFontFile.h"
#include "GetNameValue.h"
#include "EditorGlobal.h"
#include "TextFont.h"

BOOL IsNt()
{

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo))
		return FALSE;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF;	// Win 95 needs this
	
	if (dwPlatformId == VER_PLATFORM_WIN32_NT) return true;
	else return false;	
}

bool GetFontFile(const LOGFONT & getLf, std::wstring * strFontFile, int * index, LOGFONT * retLf)
{
	wchar_t szName[2 * MAX_PATH];
	wchar_t szData[2 * MAX_PATH];

	std::wstring strFont;
	if (IsNt()) 
		strFont = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
	else
		strFont = L"Software\\Microsoft\\Windows\\CurrentVersion\\Fonts";

	strFontFile->clear();
	bool bResult = false;

	int maxMatches = -1;
	while (GetNextNameValue(HKEY_LOCAL_MACHINE, strFont.c_str(), szName, szData) == ERROR_SUCCESS)
	{		
		DWORD size = 0;
		EditorGlobal::instance().getFontResourceInfo(szData, &size, 0, 2);

		LOGFONT * lfArray = (LOGFONT *)malloc(size);
		
		EditorGlobal::instance().getFontResourceInfo(szData, &size, lfArray, 2);

		int numFaces = size / sizeof(LOGFONT);
		int faceIndex = 0;
		for (int i = 0; i < numFaces; ++i)
		{
			if (lfArray[i].lfFaceName[0] == L'@') continue;
			bool matchFace = !lstrcmp(getLf.lfFaceName, lfArray[i].lfFaceName);
			bool matchStyle = getLf.lfItalic == lfArray[i].lfItalic;
			bool matchWeight = getLf.lfWeight == lfArray[i].lfWeight;

			if (matchFace)
			{
				int numMatches = (int)matchStyle + (int)matchWeight;
				if (numMatches > maxMatches)
				{				
					maxMatches = numMatches;
					//found font				
					//TODO change this to query the system for the font directory

					QString fontDir = 
						QDesktopServices::storageLocation(QDesktopServices::FontsLocation);

					*strFontFile = fontDir.toStdWString() + L"/" + szData;
					*index = faceIndex;	
					*retLf = lfArray[i];
					
					bResult = true;
				}
			}
			++faceIndex;
		}
		
		free(lfArray);

		
		strFont.clear();	// this will get next value, same key
		
	}

	GetNextNameValue(HKEY_LOCAL_MACHINE, NULL, NULL, NULL);	// close the registry key

	return bResult;
}


LOGFONT getFontData(const std::string & fontFile, int pfaceIndex)
{
	DWORD size = 0;

	std::wstring fileNameW(fontFile.begin(), fontFile.end());

	EditorGlobal::instance().getFontResourceInfo(fileNameW.c_str(), &size, 0, 2);
	LOGFONT * lfArray = (LOGFONT *)malloc(size);	
	EditorGlobal::instance().getFontResourceInfo(fileNameW.c_str(), &size, lfArray, 2);

	LOGFONT ret;
	memset(&ret, 0, sizeof(LOGFONT));

	int numArrayElem = size/sizeof(LOGFONT);
	int faceIndex = 0;
	for (int i = 0; i < numArrayElem; ++i)
	{
		if (lfArray[i].lfFaceName[0] == L'@') continue;
		if (faceIndex == pfaceIndex) ret = lfArray[i];
		++faceIndex;
	}

	free(lfArray);
	HDC hdc = GetDC(0);
	ReleaseDC(0, hdc);

	return ret;
}
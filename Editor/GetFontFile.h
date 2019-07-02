#ifndef GETFONTFILE_H
#define GETFONTFILE_H


class TextFont;

bool GetFontFile(
	const LOGFONT & getLf, std::wstring * strFontFile, int * index,
	LOGFONT * retLf);
LOGFONT getFontData(const std::string & fontFile, int faceIndex);



#endif //GETFONTFILE_H

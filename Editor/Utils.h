#pragma once

/**
VK_SHIFT, VK_MENU, VK_CONTROL,
VK_LBUTTON, VK_MBUTTON, VK_RBUTTON
*/
bool isKeyDown(int keyCode);

QString stdStringToQString(const std::string & str);

std::string qStringToStdString(const QString & str);

QString getOpenFileName(
	const QString & title, const QString & curDir, const QString & filter);

QString getSaveFileName(
	const QString & title, const QString & curDir, const QString & filter,
	const QString & suffix);

QString getSaveFileNameNoOverwrite(
	const QString & title, const QString & curDir, const QString & filter,
	const QString & suffix);

QStringList getOpenFileNames(
	const QString & title, const QString & curDir, const QString & filter);

QString getExistingDirectory(const QString & title = QString(), const QString & dir = QString());

void regexReplace(
	std::string * str, const char * searchRegEx, const char * replaceStr,
	bool replaceOnce = false);

bool regexMatch(const std::string & str, const char * searchRegEx);

//more automated versions of converToRelativePath in FileUtils.h

QString convertToRelativePath(const QString & file);
std::string convertToRelativePath(const std::string & file);


//In UriCodec.cpp
std::string UriEncode(const std::string & sSrc);
std::string UriDecode(const std::string & sSrc);

QString removeSpecialCharacter(QAction * action);
QString reverseFilePath(QString * fileName);

float removeFloatPointNumber(float fOrigin, int Point);

std::wstring mbs_to_wcs(std::string const& str, std::locale loc);
std::string wcs_to_mbs(std::wstring const& str, std::locale loc);
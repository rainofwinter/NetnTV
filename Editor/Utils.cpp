#include "stdafx.h"
#include "Utils.h"
#include "FileUtils.h"
#include <boost/regex.hpp>

using namespace std;

bool isKeyDown(int keyCode)
{
	return (GetAsyncKeyState(keyCode) & ~1) != 0;
}

QString stdStringToQString(const std::string & str)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
	return codec->toUnicode(str.c_str());	
}

std::string qStringToStdString(const QString & str)
{
	QTextCodec * codec = QTextCodec::codecForLocale();
		
	return codec->fromUnicode(str).constData();

}

QString convertToRelativePath(const QString & file)
{
	string dir = Global::instance().readDirectory();
	return stdStringToQString(convertToRelativePath(dir, qStringToStdString(file)));
}

std::string convertToRelativePath(const std::string & file)
{
	string dir = Global::instance().readDirectory();
	return convertToRelativePath(dir, file);
}

/*

QStringList getOpenFileNames(
	const QString & title, const QString & requestedCurDir, 
	const QString & filter)
{
	std::wstring filterStr = filter.toStdWString();

	wstring::size_type offset = 0, lParenOffset, rParenOffset;

	vector<wchar_t> winFilter;
	while(1)
	{
		lParenOffset = filterStr.find_first_of(L'(', offset);
		rParenOffset = filterStr.find_first_of(L')', lParenOffset);
		if (rParenOffset == wstring::npos) break;	

		wstring fileDesc = filterStr.substr(offset, rParenOffset - offset + 1);
		wstring extensionStr = 
			filterStr.substr(lParenOffset + 1, rParenOffset - lParenOffset - 1);
					
		winFilter.insert(winFilter.end(), fileDesc.begin(), fileDesc.end());
		winFilter.push_back(L'\0');

		vector<wstring> extensions;
		boost::split(extensions, extensionStr, boost::is_any_of(L" "));	
		BOOST_FOREACH(wstring extension, extensions)
		{
			winFilter.insert(winFilter.end(), extension.begin(), extension.end());
			winFilter.push_back(L';');			
		}

		winFilter.push_back(L'\0');
		offset = rParenOffset + 1;
	}
	winFilter.push_back(L'\0');

	OPENFILENAME OpenFileName;
	std::wstring titleStr = title.toStdWString();
	wchar_t szFile[128*MAX_PATH];
	szFile[0] = 0;
	
	wstring curDirW = requestedCurDir.toStdWString();
	if (curDirW.empty())
	{		
		string curDir = getCurDir();
		curDirW = wstring(curDir.begin(), curDir.end());
	}

	OpenFileName.lStructSize = sizeof( OPENFILENAME );
	OpenFileName.hwndOwner = NULL;
	OpenFileName.lpstrFilter = &winFilter[0];
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter = 0;
	OpenFileName.nFilterIndex = 0;
	OpenFileName.lpstrFile = szFile;
	OpenFileName.nMaxFile = sizeof( szFile );
	OpenFileName.lpstrFileTitle = NULL;
	OpenFileName.nMaxFileTitle = 0;
	OpenFileName.lpstrInitialDir = curDirW.c_str();
	OpenFileName.lpstrTitle = titleStr.c_str();
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = NULL;
	OpenFileName.lCustData = 0;
	OpenFileName.lpfnHook = NULL;
	OpenFileName.lpTemplateName = NULL;
	OpenFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_ALLOWMULTISELECT;
	

	QStringList fileNames;

		

	if (GetOpenFileName(&OpenFileName))
	{
		
		wchar_t buffer[MAX_PATH];

		wchar_t * ptr = szFile;	
		if (wcslen(ptr) > MAX_PATH - 1) return fileNames;
		if (!*ptr) return fileNames;
		wcscpy_s(buffer, MAX_PATH, ptr);	
		QString dirStr = QString::fromStdWString(buffer);
		if (dirStr[dirStr.size() - 1] != L'\\') dirStr.push_back(L'\\');

		while(1)
		{
			while(*ptr++);			
			if (wcslen(ptr) > MAX_PATH - 1) continue;	
			if (!*ptr) break;
			
			wcscpy_s(buffer, MAX_PATH, ptr);			

			fileNames.push_back(
				dirStr + 
				QString::fromStdWString(buffer));
		}
	}

	return fileNames;
}

QString getOpenFileName(
	const QString & title, const QString & requestedCurDir, 
	const QString & filter)
{
	/*
	std::wstring filterStr = filter.toStdWString();

	wstring::size_type offset = 0, lParenOffset, rParenOffset;

	vector<wchar_t> winFilter;
	while(1)
	{
		lParenOffset = filterStr.find_first_of(L'(', offset);
		rParenOffset = filterStr.find_first_of(L')', lParenOffset);
		if (rParenOffset == wstring::npos) break;	

		wstring fileDesc = filterStr.substr(offset, rParenOffset - offset + 1);
		wstring extensionStr = 
			filterStr.substr(lParenOffset + 1, rParenOffset - lParenOffset - 1);
					
		winFilter.insert(winFilter.end(), fileDesc.begin(), fileDesc.end());
		winFilter.push_back(L'\0');

		vector<wstring> extensions;
		boost::split(extensions, extensionStr, boost::is_any_of(L" "));	
		BOOST_FOREACH(wstring extension, extensions)
		{
			winFilter.insert(winFilter.end(), extension.begin(), extension.end());
			winFilter.push_back(L';');			
		}

		winFilter.push_back(L'\0');
		offset = rParenOffset + 1;
	}
	winFilter.push_back(L'\0');

	OPENFILENAME OpenFileName;
	std::wstring titleStr = title.toStdWString();
	wchar_t szFile[MAX_PATH];
	szFile[0] = 0;
	
	wstring curDirW = requestedCurDir.toStdWString();
	if (curDirW.empty())
	{		
		string curDir = getCurDir();
		curDirW = wstring(curDir.begin(), curDir.end());
	}

	OpenFileName.lStructSize = sizeof( OPENFILENAME );
	OpenFileName.hwndOwner = NULL;
	OpenFileName.lpstrFilter = &winFilter[0];
	OpenFileName.lpstrCustomFilter = NULL;
	OpenFileName.nMaxCustFilter = 0;
	OpenFileName.nFilterIndex = 0;
	OpenFileName.lpstrFile = szFile;
	OpenFileName.nMaxFile = sizeof( szFile );
	OpenFileName.lpstrFileTitle = NULL;
	OpenFileName.nMaxFileTitle = 0;
	OpenFileName.lpstrInitialDir = curDirW.c_str();
	OpenFileName.lpstrTitle = titleStr.c_str();
	OpenFileName.nFileOffset = 0;
	OpenFileName.nFileExtension = 0;
	OpenFileName.lpstrDefExt = NULL;
	OpenFileName.lCustData = 0;
	OpenFileName.lpfnHook = NULL;
	OpenFileName.lpTemplateName = NULL;
	OpenFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON;
	
	if (GetOpenFileName(&OpenFileName))
	{
		return QString::fromStdWString(szFile);
	}

	return QString();	
}

*/

#define USE_NATIVE


QString lastDlgDir;

QString getOpenFileName(const QString & title, 
	const QString & directory, const QString & filter)
{
	QString dlgDir = directory;
	if (dlgDir.isEmpty()) dlgDir = lastDlgDir;
	if (dlgDir.isEmpty())
	{		
		string curDirStr = getCurDir();
		dlgDir = stdStringToQString(curDirStr);		
	}
	QString ret;

#ifdef USE_NATIVE
	ret = QFileDialog::getOpenFileName(0, title, dlgDir, filter, 0, 0);
#else
	QFileDialog dlg;
	dlg.setNameFilter(filter);
	dlg.setWindowTitle(title);	

	dlg.setDirectory(dlgDir);
	dlg.setFileMode(QFileDialog::ExistingFile);
	if (dlg.exec())	
		ret = dlg.selectedFiles()[0];	
	else 
		ret = QString();
#endif

	if (!ret.isEmpty()) lastDlgDir = QFileInfo(ret).path();
	return ret;
}

QString getSaveFileName(const QString & title, 
	const QString & directory, const QString & filter, const QString & suffix)
{
	QString dlgDir = directory;
	if (dlgDir.isEmpty()) dlgDir = lastDlgDir;
	if (dlgDir.isEmpty())
	{		
		string curDirStr = getCurDir();
		dlgDir = stdStringToQString(curDirStr);		
	}
	QString ret;

#ifdef USE_NATIVE
	ret = QFileDialog::getSaveFileName(0, title, dlgDir, filter, 0, 0);
#else
	QFileDialog dlg;
	dlg.setNameFilter(filter);
	dlg.setWindowTitle(title);

	dlg.setDirectory(dlgDir);
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	dlg.setDefaultSuffix(suffix);
	if (dlg.exec()) ret =  dlg.selectedFiles()[0];	
	else ret =  QString();
#endif

	if (!ret.isEmpty()) lastDlgDir = QFileInfo(ret).path();
	return ret;
}

QString getSaveFileNameNoOverwrite(const QString & title, 
	const QString & directory, const QString & filter, const QString & suffix)
{
	QString dlgDir = directory;
	if (dlgDir.isEmpty()) dlgDir = lastDlgDir;
	if (dlgDir.isEmpty())
	{		
		string curDirStr = getCurDir();
		dlgDir = stdStringToQString(curDirStr);		
	}
	QString ret;

#ifdef USE_NATIVE
	ret = QFileDialog::getSaveFileName(0, title, dlgDir, filter, 0, QFileDialog::DontConfirmOverwrite);
#else
	TODO: implement non native dialog vesrion
#endif

	if (!ret.isEmpty()) lastDlgDir = QFileInfo(ret).path();
	return ret;
}

QStringList getOpenFileNames(
	const QString & title, const QString & directory, const QString & filter)
{
	QString dlgDir = directory;
	if (dlgDir.isEmpty()) dlgDir = lastDlgDir;
	if (dlgDir.isEmpty())
	{		
		string curDirStr = getCurDir();
		dlgDir = stdStringToQString(curDirStr);		
	}
	QStringList ret;

#ifdef USE_NATIVE

	ret = QFileDialog::getOpenFileNames(0, title, dlgDir, filter);
#else
	QFileDialog dlg;
	dlg.setNameFilter(filter);
	dlg.setWindowTitle(title);
	dlg.setDirectory(dlgDir);
	dlg.setFileMode(QFileDialog::ExistingFiles);
	if (dlg.exec())
	{
		ret = dlg.selectedFiles();
	}
	else ret = QStringList();
#endif

	if (!ret.isEmpty())
	{
		lastDlgDir = QFileInfo(ret[0]).path();
	}

	return ret;
}

QString getExistingDirectory(const QString & title, const QString & directory)
{
	QString dlgDir = directory;
	//if (dlgDir.isEmpty()) dlgDir = lastDlgDir;
	if (dlgDir.isEmpty())
	{		
		string curDirStr = getCurDir();
		dlgDir = stdStringToQString(curDirStr);		
	}
	QStringList ret;

#ifdef USE_NATIVE
	return QFileDialog::getExistingDirectory(0, title, dlgDir);
#else
	QFileDialog dlg;
	if (!title.isEmpty())
		dlg.setWindowTitle(title);

	dlg.setDirectory(dlgDir);
	dlg.setFileMode(QFileDialog::Directory);
	if (dlg.exec())
	{
		return dlg.selectedFiles()[0];
	}
	else return QString();
#endif
}

void regexReplace(
	std::string * str, const char * searchRegEx, const char * replaceStr,
	bool replaceOnce)
{
	using namespace boost;
	std::stringstream ss;
	std::ostream_iterator<char> ssi(ss);
	boost::regex_replace(ssi, str->begin(), str->end(), 
		regex(searchRegEx), replaceStr, 
		match_default | format_all | 
		((replaceOnce)?format_first_only:match_default));	
	*str = ss.str();
}

bool regexMatch(const std::string & str, const char * searchRegEx)
{
	using namespace boost;
	return regex_match(str.begin(), str.end(), 
		regex(searchRegEx), match_default | format_all);
}

QString removeSpecialCharacter(QAction * action)
{
	QString toolTipText = action->text();

	toolTipText.remove(QChar('&'), Qt::CaseInsensitive);
	toolTipText.remove(QChar('.'), Qt::CaseInsensitive);

	return toolTipText;
}

QString reverseFilePath(QString * fileName)
{
	QString reverseFileName;
	
	int prev = fileName->count('/');
	for(int i = prev; i >= 0; i--)
	{
		reverseFileName.append(fileName->section('/', i, prev));
		reverseFileName.append('/');
		prev--;
	}
	
	return reverseFileName;
}

float removeFloatPointNumber(float fOrigin, int fPoint)
{
	int targetNum = 1;
	for(int i = 0; i < fPoint; i++)
	{
		targetNum *= 10;
	}

	int iNumber = fOrigin * targetNum;
	
	return (float)iNumber / targetNum;
}

std::wstring mbs_to_wcs(std::string const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<wchar_t> buf(str.size() + 1);
	char const* in_next = str.c_str();
	wchar_t* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.in(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert string to wstring");   
	return std::wstring(&buf[0]);
}

std::string wcs_to_mbs(std::wstring const& str, std::locale loc)
{
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
	std::mbstate_t state = std::mbstate_t();
	std::vector<char> buf((str.size() + 1) * codecvt.max_length());
	wchar_t const* in_next = str.c_str();
	char* out_next = &buf[0];
	std::codecvt_base::result r = codecvt.out(state, 
		str.c_str(), str.c_str() + str.size(), in_next, 
		&buf[0], &buf[0] + buf.size(), out_next);
	if (r == std::codecvt_base::error)
		throw std::runtime_error("can't convert wstring to string");   
	return std::string(&buf[0]);
}
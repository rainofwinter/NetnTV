#include "stdafx.h"
#include "FileUtils.h"
#include <ctype.h>

using namespace std;

std::string getFileNameWithoutDirectory(const std::string & absFileName)
{
	using namespace std;

	string::size_type pos = absFileName.find_last_of("\\/");
	if (pos == string::npos) return absFileName;
	else return absFileName.substr(pos+1);
}

std::string getDirectory(const std::string & absFileName)
{
	using namespace std;

	string::size_type pos = absFileName.find_last_of("\\/");
	if (pos == string::npos) return "";
	else return absFileName.substr(0, pos+1);
}

std::string getFileExtension(const std::string & fileName)
{
	string::size_type pos = fileName.find_last_of('.');
	if (pos == string::npos) return "";
	return fileName.substr(pos + 1);
}

bool checkFileExtension(const std::string & fileName, const std::string & extension)
{
	string ext = getFileExtension(fileName);
	return (boost::iequals(ext, extension));
}

std::string getFileTitle(const std::string & absFileName)
{
	std::string file = getFileNameWithoutDirectory(absFileName);
	string::size_type pos = file.find_last_of('.');
	if (pos != string::npos)
		return file.substr(0, pos);
	else
		return file;
}

//------------------------------------------------------------------------------
string strToLower(const std::string & str)
{
	string out = str;
	for (int i = 0; i < (int)out.size(); ++i) out[i] = tolower(out[i]);	
	return out;
}

vector<string> getDirTokens(const string & abspath)
{
	vector<string> tokens;
	string::size_type offset1 = 0, offset2 = 0;
	while(1)
	{
		offset2 = abspath.find_first_of("\\/", offset1);
		string dir = abspath.substr(offset1, offset2 - offset1);
		
		if (offset1 == 0 || !dir.empty()) tokens.push_back(dir);
		if (offset2 == string::npos) break;
		offset1 = offset2 + 1;
	}

	return tokens;
}

std::string convertToRelativePath(
	const std::string & curDir, const std::string & absFilePath)
{
	string relPath;	

	vector<string> dirs1 = getDirTokens(curDir);
	vector<string> dirs2 = getDirTokens(absFilePath);

	int c = 0;
	while(1)
	{
		if (c >= (int)dirs1.size() || c >= (int)dirs2.size()) break;
		if (strToLower(dirs1[c]) != strToLower(dirs2[c])) break;
		c++;
	}

	if (c == 0) return absFilePath;

	for (int i = c; i < (int)dirs1.size(); ++i) relPath += "../";
	for (int i = c; i < (int)dirs2.size(); ++i) 
	{
		relPath += dirs2[i];
		if (i < (int)dirs2.size() - 1) relPath += "/";
	}
	
	return relPath;
}

std::string getAbsFileName(
	const std::string & absDir, const std::string & relFileName)
{
	if (absDir.empty()) return relFileName;
	
	//check that relFileName is not already an absolute path
	if (relFileName.find_first_of(':') != string::npos || relFileName.find_first_of('/') == 0)
		return relFileName;
	
	string absPath = absDir;
	
	if (absPath[absPath.size() - 1] != '/' && absPath[absPath.size() - 1] != '\\')
		absPath += "/";

	absPath += relFileName;

	vector<string> tokens = getDirTokens(absPath);
	
	absPath.clear();
	vector<string>::iterator iter;

	
	for (iter = tokens.begin(); iter != tokens.end();)
	{		
		if (*iter == "..")
		{	
			if (iter != tokens.begin() && iter - 1 != tokens.begin())
				iter = tokens.erase(iter - 1);
			iter = tokens.erase(iter);
		}
		else if (*iter == ".")
		{
			iter = tokens.erase(iter);
		}
		else
			++iter;
	}

	for (iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		absPath += *iter;

		if (iter + 1 != tokens.end())
			absPath += '/';
	}

	return absPath;
	

}

//-----------------------------------------------------------------------------

std::string getCurDir()
{
	static const int numChars = 1024;
	char curDir[numChars];
	getcwd(curDir, numChars);
	return std::string(curDir);
}
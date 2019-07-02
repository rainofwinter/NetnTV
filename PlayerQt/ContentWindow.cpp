#include "stdafx.h"
#include "ContentWindow.h"
#include "DecompressingDlg.h"
#include <boost/lexical_cast.hpp>
#include <direct.h>
#include "Utils.h"

bool ContentWindow::deleteDirFiles(const QString &dirName, bool rmdir) const
{
	if (dirName.isEmpty()) false;

    bool result = true;   
	
	QDir dir(dirName);
	 
    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = deleteDirFiles(info.absoluteFilePath(), true);
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }
 
            if (!result) {
                return result;
            }
        }
        if (rmdir) 
			result = dir.rmdir(dirName);
    }
 
    return result;
}

bool ContentWindow::decompress(
	const QString & decompressDir, const QString & fileName) const
{
	deleteDirFiles(decompressDir, false);
	_chdir(qStringToStdString(decompressDir).c_str());

	DecompressingDlg dlg(
		decompressDir, fileName, 0);
	
	dlg.exec();

	if (dlg.state() != DecompressingDlg::Success)
	{		
		return false;
	}	
	return true;	
}
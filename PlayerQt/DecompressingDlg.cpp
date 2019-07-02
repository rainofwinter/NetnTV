#include "stdafx.h"
#include "Utils.h"
#include "FileUtils.h"
#include "decompress.h"
#include "DecompressingDlg.h"
#include <direct.h>

using namespace boost;
using namespace std;

DecompressingDlg::DecompressingDlg(
	const QString & directory, const QString & fileName,
	QWidget * parent) : QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint); 

	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

	fileName_ = fileName;
	directory_ = directory;

	progressBarRange_ = 100;
	ui.progressBar->setRange(0, progressBarRange_);	

	
	progress_ = 0.0f;	
	state_ = InProgress;
	ended_ = false;	
}

DecompressingDlg::~DecompressingDlg()
{
	timer_.stop();
}

void DecompressingDlg::showEvent(QShowEvent * event)
{
	thread_ = boost::thread(boost::ref(*this));
	timer_.start(200, this);
}

void DecompressingDlg::closeEvent(QCloseEvent * event)
{	
	onCancel();
	event->ignore();	
}

void DecompressingDlg::operator()()
{
	std::string strFileName = qStringToStdString(fileName_);
	std::string strDirectory = qStringToStdString(directory_);
	std::string curDir = getCurDir();
	_chdir(strDirectory.c_str());

	unzFile uf = NULL;
	uf = unzOpen64(strFileName.c_str());
	if (uf == NULL)
	{
		//zip file open failed	
		unique_lock<mutex> lk(mutex_);
		state_ = Error;
		return;		
	}

	uLong i;
    unz_global_info64 gi;
    int err;
    FILE* fout=NULL;

    err = unzGetGlobalInfo64(uf,&gi);
    if (err!=UNZ_OK)
	{
		//printf("error %d with zipfile in unzGetGlobalInfo \n",err);
		unique_lock<mutex> lk(mutex_);
		state_ = Error;
		unzClose(uf);
		return;
	}

    for (i=0;i<gi.number_entry;i++)
    {
        if (do_extract_currentfile(uf, false, 0) != UNZ_OK)
		{
			unique_lock<mutex> lk(mutex_);
			state_ = Error;
            break;
		}

        if ((i+1)<gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err!=UNZ_OK)
            {
				unique_lock<mutex> lk(mutex_);
				state_ = Error;
				break;
            }
        }
		
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		
		unique_lock<mutex> lk(mutex_);
		if (state_ == Cancelled) break;
		progress_ = float(i + 1) / gi.number_entry;
    }	

	unzClose(uf);
	_chdir(curDir.c_str());

	unique_lock<mutex> lk(mutex_);
	if (state_ == InProgress) state_ = Success;
	ended_ = true;
}


void DecompressingDlg::onCancel()
{	
	unique_lock<mutex> lk(mutex_);
	state_ = Cancelled;
}

void DecompressingDlg::timerEvent(QTimerEvent * event)
{
	//printf("DecompressingDlg::timerEvent\n");
	unique_lock<mutex> lk(mutex_);
	ui.progressBar->setValue(progress_ * progressBarRange_);
	if (ended_ == true)
	{
		if (state_ == Success) 
		{
			timer_.stop();
			accept();
		}
		else if (state_ == Error)
		{
			timer_.stop();
			QMessageBox::information(this, tr("Error"), tr("Couldn't decompress content"));
			reject();
		}
		else if (state_ == Cancelled)
		{
			timer_.stop();
			reject();
		}
	}
}

#include "stdafx.h"
#include "PublishingDlg.h"
#include "EditorGlobal.h"
#include "Document.h"
#include "DocumentTemplate.h"
#include "FileUtils.h"
#include "Image.h"
#include "ImageGrid.h"
#include "ImageChanger.h"
#include "ImageAnimation.h"
#include "Scene.h"
#include "Utils.h"
#include "Root.h"
#include "Writer.h"
#include "Texture.h"
#include <boost/scope_exit.hpp>
#include "ZipPublish.h"
#include "Model.h"
#include "TAnimation.h"
#include "Texture.h"
#include "Model.h"
#include "Xml.h"
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <qfile.h>

#include "ActionListPane.h"

using namespace boost;
using namespace std;


template <typename T>
bool getXmlNodeContent(T & val, xmlNode * node, const char * name)
{
	if (!node) return false;
	if (!xmlStrEqual(node->name, (xmlChar *)name)) return false;
	if (node->type != XML_ELEMENT_NODE) return false;
	if (node->children->type != XML_TEXT_NODE) return false;
	
	try 
	{
		val = lexical_cast<T>(node->children->content);
	} catch(...) 
	{
		return false;
	}

	return true;
}


PublishingDlg::PublishingDlg(Document * document, 
	const QString & publishDir, const QString & publishFile, 
	bool isCatalogDocument, QWidget * parent) : QDialog(parent)
{
	isCatalogDocument_ = isCatalogDocument;
	document_ = document;

	string curDir = Global::instance().readDirectory();

	publishDir_ = 
		stdStringToQString(getAbsFileName(curDir, qStringToStdString(publishDir)));
	
	publishFile_ = 
		stdStringToQString(getAbsFileName(curDir, qStringToStdString(publishFile)));

	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); 
	
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));	

	ui.okButton->setEnabled(false);
	ui.cancelButton->setEnabled(true);

	xmlcnt_ = 0;
}

void PublishingDlg::showEvent(QShowEvent * event)
{
	progress_ = 0;
	cancel_ = false;
	complete_ = false;
	document_->referencedFiles(&refFiles_);
	ui.progressBar->setRange(0, (int)refFiles_.size());
	progressMax_ = (int)refFiles_.size();
	ui.progressBar->setValue(0);


	timer_.start(100, this);
	thread_ = boost::thread(boost::ref(*this));	
}

void PublishingDlg::timerEvent(QTimerEvent * event)
{
	lock_guard<mutex> lk(mutex_);
	ui.progressBar->setRange(0, progressMax_);
	ui.progressBar->setValue(progress_);	
	ui.label->setText(displayMsg_);

	if (complete_)
	{
		if (!errorMsg_.isEmpty())
		{
			QMessageBox::information(this, tr("Error"),errorMsg_);
			reject();
		}
		

		ui.cancelButton->setEnabled(false);
		ui.okButton->setEnabled(true);
	}
}

void PublishingDlg::closeEvent(QCloseEvent * event)
{
	timer_.stop();
}

void PublishingDlg::operator()()
{
	
	try{
		publish(publishDir_);
	} catch (std::exception & e)
	{
		QString err;
		err.sprintf("%s", e.what());
		errorMsg_ = err;
	}

	lock_guard<mutex> lk(mutex_);
	complete_ = true;	
}

void PublishingDlg::onOk()
{
	
	accept();
}

void PublishingDlg::onCancel()
{
	unique_lock<mutex> lk(mutex_);
	cancel_ = true;
	lk.unlock();
	thread_.join();

	reject();
}

inline void addToPixel(float * scanLine, int x, int width, float factor, float delta[4])
{
	if (x < 0 || x >= width) return;
	float * pixel = scanLine + x*4;
	pixel[0] += factor * delta[0];
	pixel[1] += factor * delta[1];
	pixel[2] += factor * delta[2];
	pixel[3] += factor * delta[3];	
}

Texture::Format PublishingDlg::doReduceBitDepth(unsigned char * bits, int width, int height)
{
	bool hasAlpha = false;
	Texture::Format ret;
	unsigned char * srcBits = (unsigned char *)bits;

	srcBits += 3;
	for (int i = 0; i < width*height; ++i)
	{
		if (*srcBits != 255) hasAlpha = true;
		srcBits += 4;
	}

	ret = Texture::UncompressedRgb16;
	if (hasAlpha) ret = Texture::UncompressedRgba16;


	//dither
	float quantError[4];	

	float * curScanLine = new float[width*4];
	float * nextScanLine = new float[width * 4];
	unsigned char * optr = (unsigned char *)bits;
	srcBits = (unsigned char *)bits;
	for (int x = 0; x < width; ++x)
	{
		nextScanLine[x*4] = srcBits[x*4];
		nextScanLine[x*4 + 1] = srcBits[x*4 + 1];
		nextScanLine[x*4 + 2] = srcBits[x*4 + 2];
		nextScanLine[x*4 + 3] = srcBits[x*4 + 3];
	}

	for (int y = 0; y < height; ++y)
	{

		memcpy(curScanLine, nextScanLine, sizeof(float) * 4 * width);		
		if (y < height - 1)
		{
			int nextY = y + 1;
			for (int x = 0; x < width; ++x)
			{
				nextScanLine[x*4] = srcBits[(nextY * width + x)*4];
				nextScanLine[x*4 + 1] = srcBits[(nextY * width + x)*4 + 1];
				nextScanLine[x*4 + 2] = srcBits[(nextY * width + x)*4 + 2];
				nextScanLine[x*4 + 3] = srcBits[(nextY * width + x)*4 + 3];
			}
		}		


		for (int x = 0; x < width; ++x)
		{			
			float o0 = curScanLine[x*4];
			float o1 = curScanLine[x*4 + 1];
			float o2 = curScanLine[x*4 + 2];
			float o3 = curScanLine[x*4 + 3];


			float f0, f1, f2, f3;
			if (hasAlpha)
			{
				//4 4 4 4			
				f0 = roundFloat(o0 * 15.0f / 255.0f) * 255.0f / 15.0f;
				f1 = roundFloat(o1 * 15.0f / 255.0f) * 255.0f / 15.0f;
				f2 = roundFloat(o2 * 15.0f / 255.0f) * 255.0f / 15.0f;
				f3 = roundFloat(o3 * 15.0f / 255.0f) * 255.0f / 15.0f;
			}
			else
			{
				//5 6 5			
				f0 = roundFloat(o0 * 31.0f / 255.0f) * 255.0f / 31.0f;
				f1 = roundFloat(o1 * 63.0f / 255.0f) * 255.0f / 63.0f;
				f2 = roundFloat(o2 * 31.0f / 255.0f) * 255.0f / 31.0f;
				f3 = 255;	

				
			}

			if (f0 < 0) f0 = 0;
			if (f0 > 255) f0 = 255;
			if (f1 < 0) f1 = 0;
			if (f1 > 255) f1 = 255;
			if (f2 < 0) f2 = 0;
			if (f2 > 255) f2 = 255;
			if (f3 < 0) f3 = 0;
			if (f3 > 255) f3 = 255;

			quantError[0] = o0 - f0;
			quantError[1] = o1 - f1;
			quantError[2] = o2 - f2;
			quantError[3] = o3 - f3;	
			
			*optr++ = (unsigned char)(f0);
			*optr++ = (unsigned char)(f1);
			*optr++ = (unsigned char)(f2);
			*optr++ = (unsigned char)(f3);			

			addToPixel(curScanLine, x+1, width, 7.0f/16.0f, quantError);
			addToPixel(nextScanLine, x-1, width, 3.0f/16.0f, quantError);
			addToPixel(nextScanLine, x, width, 5.0f/16.0f, quantError);
			addToPixel(nextScanLine, x+1, width, 1.0f/16.0f, quantError);
		}
	}

	delete [] curScanLine;
	delete [] nextScanLine;

	return ret;
}

void PublishingDlg::determineTexCompressImages(
	Document * document, std::set<std::string> * imgs)
{
	BOOST_FOREACH(SceneSPtr scene, document->scenes())
	{
		determineTexCompressImages(scene->root(), imgs);
	}
}
void PublishingDlg::determineTexCompressImages(
	SceneObject * parent, std::set<std::string> * imgs)
{
	static boost::uuids::uuid imgType = Image().type();
	static boost::uuids::uuid imgChangerType = ImageChanger().type();
	static boost::uuids::uuid imgGridType = ImageGrid().type();
	static boost::uuids::uuid imgAnimationType = ImageAnimation().type();

	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (child->type() == imgType)
		{
			Image * img = (Image *)child.get();
			imgs->insert(img->fileName());
		}
		else if (child->type() == imgChangerType)
		{
			ImageChanger * imgChanger = (ImageChanger *)child.get();
			BOOST_FOREACH(const string & fileName, imgChanger->fileNames())
			{
				imgs->insert(fileName);
			}			
		}
		else if (child->type() == imgGridType)
		{
			ImageGrid * imgGrid = (ImageGrid *)child.get();
			BOOST_FOREACH(const string & fileName, imgGrid->fileNames())
			{
				imgs->insert(fileName);
			}			
		}
		else if (child->type() == imgAnimationType)
		{
			ImageAnimation * obj = (ImageAnimation *)child.get();
			BOOST_FOREACH(const string & fileName, obj->fileNames())
			{
				imgs->insert(fileName);
			}
		}
		else
		{
			determineTexCompressImages(child.get(), imgs);
		}
	}
}

void PublishingDlg::determine16BitImages(
	Document * document, std::set<std::string> * imgs)
{
	BOOST_FOREACH(SceneSPtr scene, document->scenes())
	{
		determine16BitImages(scene->root(), imgs);
	}
}
void PublishingDlg::determine16BitImages(
	SceneObject * parent, std::set<std::string> * imgs)
{
	static boost::uuids::uuid imgType = Image().type();
	static boost::uuids::uuid imgChangerType = ImageChanger().type();
	static boost::uuids::uuid imgGridType = ImageGrid().type();

	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (child->type() == imgType)
		{
			Image * img = (Image *)child.get();
			imgs->insert(img->fileName());
		}
		else if (child->type() == imgChangerType)
		{
			ImageChanger * imgChanger = (ImageChanger *)child.get();
			BOOST_FOREACH(const string & fileName, imgChanger->fileNames())
			{
				imgs->insert(fileName);
			}			
		}
		else if (child->type() == imgGridType)
		{
			ImageGrid * imgGrid = (ImageGrid *)child.get();
			BOOST_FOREACH(const string & fileName, imgGrid->fileNames())
			{
				imgs->insert(fileName);
			}			
		}
		else
		{
			determine16BitImages(child.get(), imgs);
		}
	}
}

void PublishingDlg::changeTo16BitFormat(
	Document * document, std::map<std::string, Texture::Format> * imgFormats)
{
	BOOST_FOREACH(SceneSPtr scene, document->scenes())
	{
		changeTo16BitFormat(scene->root(), imgFormats);
	}
}
void PublishingDlg::changeTo16BitFormat(
	SceneObject * parent, std::map<std::string, Texture::Format> * imgFormats)
{
	static boost::uuids::uuid imgType = Image().type();
	static boost::uuids::uuid imgChangerType = ImageChanger().type();
	static boost::uuids::uuid imgGridType = ImageGrid().type();

	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (child->type() == imgType)
		{
			Image * img = (Image *)child.get();
			string fileName = img->fileName();
			if (imgFormats->find(fileName) != imgFormats->end())
			{
				img->setFormat((*imgFormats)[img->fileName()]);
			}
		}
		else if (child->type() == imgChangerType)
		{
			ImageChanger * imgChanger = (ImageChanger *)child.get();			
			bool do16Bit = false;
			Texture::Format useFormat = Texture::UncompressedRgb16;
			BOOST_FOREACH(const string & fileName, imgChanger->fileNames())
			{
				if (imgFormats->find(fileName) != imgFormats->end())
				{
					do16Bit = true;
					if ((*imgFormats)[fileName] == Texture::UncompressedRgba16)				
						useFormat = Texture::UncompressedRgba16;				
				}
			}			
			
			if (do16Bit) imgChanger->setFormat(useFormat);
		}
		else if (child->type() == imgGridType)
		{
			ImageGrid * obj = (ImageGrid *)child.get();
			bool do16Bit = false;
			Texture::Format useFormat = Texture::UncompressedRgb16;
			BOOST_FOREACH(const string & fileName, obj->fileNames())
			{
				if (imgFormats->find(fileName) != imgFormats->end())
				{
					do16Bit = true;
					if ((*imgFormats)[fileName] == Texture::UncompressedRgba16)				
						useFormat = Texture::UncompressedRgba16;			
				}
			}		

			if (do16Bit) obj->setFormat(useFormat);
					
		}
		else
		{
			changeTo16BitFormat(child.get(), imgFormats);
		}
	}
}

void PublishingDlg::changeToOrigFormat(Document * document)
{
	BOOST_FOREACH(SceneSPtr scene, document->scenes())
	{
		changeToOrigFormat(scene->root());
	}
}
void PublishingDlg::changeToOrigFormat(SceneObject * parent)
{
	static boost::uuids::uuid imgType = Image().type();
	static boost::uuids::uuid imgChangerType = ImageChanger().type();
	static boost::uuids::uuid imgGridType = ImageGrid().type();

	BOOST_FOREACH(SceneObjectSPtr child, parent->children())
	{
		if (child->type() == imgType)
		{
			Image * img = (Image *)child.get();
			img->setFormat(Texture::UncompressedRgba32);
		}
		else if (child->type() == imgChangerType)
		{
			ImageChanger * imgChanger = (ImageChanger *)child.get();			
			imgChanger->setFormat(Texture::UncompressedRgba32);	
		}
		else if (child->type() == imgGridType)
		{
			ImageGrid * obj = (ImageGrid *)child.get();			
			obj->setFormat(Texture::UncompressedRgba32);		
		}
		else
		{
			changeToOrigFormat(child.get());
		}
	}
}


void PublishingDlg::copyRefFile(
	std::vector<std::string> * copiedFiles, 
	const std::string & absFileName, const std::string & destAbsFileName)
{
	string fileExt = getFileExtension(absFileName);	
	string origDirectory = getDirectory(absFileName);
	string destDirectory = getDirectory(destAbsFileName);
	string destJustFileTitle = getFileTitle(destAbsFileName);

//	TAnimationObject * Swf = new TAnimationObject();
	bool isXml = boost::iequals(fileExt, "xml");	
	if(isXml)
	{
		TAnimationObject * Swf = new TAnimationObject();		
		if(Swf->loadXml(absFileName))
		{
			curXml_ = destAbsFileName;
			xmlitems_ = Swf->allDataFileName();
		}
		delete Swf;
		
		xmlfiles_.push_back(curXml_);
		++xmlcnt_;			
	}
	else if(!xmlitems_.empty())
	{	
		string origfile = getFileNameWithoutDirectory(absFileName);
		string destName = getFileNameWithoutDirectory(destAbsFileName);
		origNames_[xmlfiles_[xmlcnt_-1]].push_back(origfile);		//cnt>>>>
		destNames_[xmlfiles_[xmlcnt_-1]].push_back(destName);		//cnt>>>>		
	}

	bool isModel = boost::iequals(fileExt, "stm");
	if (!isModel)
	{
		remove(destAbsFileName.c_str());
		QFile::copy(
			stdStringToQString(absFileName),  stdStringToQString(destAbsFileName));
		copiedFiles->push_back(destAbsFileName);		
	}
	else
	{

		string prevCurDir = getCurDir();
		Model * model = NULL;
		chdir(origDirectory.c_str());

		BOOST_SCOPE_EXIT( (&prevCurDir)(&model)) {
			chdir(prevCurDir.c_str());
			delete model;
		} BOOST_SCOPE_EXIT_END

		model = Model::read(getDirectory(absFileName).c_str(), absFileName.c_str());	
		
		vector<string> refFiles;
		vector<string> destFiles;

		model->referencedFiles(&refFiles);

		for (int i = 0; i < (int)refFiles.size(); ++i)
		{
			string ext = getFileExtension(refFiles[i]);		
			stringstream ss;
			ss << i;
			string relFileName = destJustFileTitle + "_" + ss.str() + "." + ext;
			string absFileName = destDirectory + relFileName;
			destFiles.push_back(relFileName);

			string origAbsFileName = getAbsFileName(origDirectory, refFiles[i]);
			remove(absFileName.c_str());
			QFile::copy(
				stdStringToQString(origAbsFileName),  stdStringToQString(absFileName));

			copiedFiles->push_back(absFileName);

		}

		model->setReferencedFiles(destDirectory, destFiles, 0);
		Model::write(destAbsFileName.c_str(), model);
		copiedFiles->push_back(destAbsFileName);
	}	
}

void PublishingDlg::publish(const QString & dir)
{
	std::string origDir = Global::instance().readDirectory();
	int origWidth = document_->width();
	int origHeight = document_->height();

	//absolute paths
	set<string> imageFiles;
	set<string> imageObjectFiles;
	set<string> imgsForTexCompression;
	map<string, Texture::Format> imageFormats;
	
	vector<string> destFiles;
	vector<string> copiedFiles;
	
	QDir::root().mkpath(dir);

	if (!QDir(dir).exists())
	{
		lock_guard<mutex> lk(mutex_);
		errorMsg_ = "Couldn't access content directory";
		return;
	}

	FILE * file = fopen(qStringToStdString(publishFile_).c_str(), "wb");

	if (file)
	{
		fclose(file);
	}
	else
	{
		lock_guard<mutex> lk(mutex_);
		errorMsg_ = "Couldn't create publish output file";
		return;
	}

	//key orig file name, value - dest file name
	map<string, string> fileNames;

	string curDir = Global::instance().readDirectory();
	string dirStr = qStringToStdString(dir);
	int curFile = 0;
	
	BOOST_FOREACH(const string & file, refFiles_)
	{
		{lock_guard<mutex> lk(mutex_); if (cancel_) break;}

		string origAbsFileName = getAbsFileName(curDir, file);
		string destFileAbs;
		string destFileRel;

		BOOST_SCOPE_EXIT( 
			(&mutex_) (&destFiles) (&destFileAbs) 
			(&fileNames) (&origAbsFileName)
			(&progress_))
		{
			fileNames[origAbsFileName] = destFileAbs;
			destFiles.push_back(getFileNameWithoutDirectory(destFileAbs));
			lock_guard<mutex> lk(mutex_);			
			progress_++;
		} BOOST_SCOPE_EXIT_END

		
		if (getFileNameWithoutDirectory(file).empty()) 
		{
			destFileAbs = "";
			continue;
		}
		if (fileNames.find(origAbsFileName) != fileNames.end()) 
		{
			destFileAbs = fileNames[origAbsFileName];
			continue;
		}
			
		stringstream ss;
		ss << "f" << curFile;
		string ext = getFileExtension(file);	
		destFileAbs = dirStr + ss.str() + "." + ext;

		bool imageProcessed = false;


		if (!imageProcessed)
		{			
			unique_lock<boost::mutex> lk(mutex_);
			displayMsg_ = tr("Copying: ") + stdStringToQString(file);
			lk.unlock();
			bool destAndSrcFileSame = 
				QDir(stdStringToQString(curDir)) == QDir(dir) &&
				getFileNameWithoutDirectory(file) == getFileNameWithoutDirectory(destFileAbs);
				
			if (!destAndSrcFileSame)			
				copyRefFile(&copiedFiles, origAbsFileName,  destFileAbs);
		}
		
		curFile++;
		
	}//end foreach refFiles_

	BOOST_SCOPE_EXIT( (&copiedFiles) )
	{
		//delete files
		BOOST_FOREACH(string file, copiedFiles)
		{
			QString fileQStr = stdStringToQString(file);
			QFile::setPermissions(fileQStr, (QFile::Permissions)0xFFF);
			QFile::remove(fileQStr);			
		}
	} BOOST_SCOPE_EXIT_END

	

	
	unique_lock<boost::mutex> lk(mutex_);
	if (cancel_) return;
	displayMsg_ = tr("Writing scene description");
	lk.unlock();

	//replace Flash(swf) xml file name
	exchangeXmltext(xmlfiles_, origNames_, destNames_);


	string documentFile = dirStr + "/document.st";
	copiedFiles.push_back(documentFile);

	changeTo16BitFormat(document_, &imageFormats);
	Writer(documentFile.c_str(), 0).write(document_);
	document_->setReferencedFiles(dirStr, destFiles);
	Writer(documentFile.c_str(), 0).write(document_);		

	document_->setReferencedFiles(origDir, refFiles_);
	changeToOrigFormat(document_);	

	lk.lock();
	if (cancel_) return;
	lk.unlock();

	//Compress into zip file
	lk.lock();
	displayMsg_ = tr("Compressing into zip file...");
	progress_ = 0;
	progressMax_ = (int)copiedFiles.size();
	lk.unlock();
	
	boost::uuids::basic_random_generator<boost::mt19937> gen(
		&EditorGlobal::instance().ran());	
	boost::uuids::uuid u = gen();
	string zipFile = qStringToStdString(dir) + "/" + boost::lexical_cast<string>(u) + ".zip";		
	
	if (isCatalogDocument_)
	{
		FILE * txtlog;
		vector<string> catalogZipFiles;
		catalogZipFiles.push_back(documentFile);
		int temp = (copiedFiles.size() - 10) / 2 + 9;
		string firstSceneImage = copiedFiles[temp];
		catalogZipFiles.push_back(firstSceneImage);
		if (!makeIntoZipFile(&mutex_, &progress_, &displayMsg_, zipFile, catalogZipFiles, cancel_))
			return;

		QDir dir;
		string publishFolder = qStringToStdString(publishFile_);
		publishFolder = getDirectory(publishFolder) + getFileTitle(publishFolder);
		dir.setPath(stdStringToQString(publishFolder));
		dir.mkpath(dir.absolutePath());
		vector<string> outFiles;
		string txtlogFile = publishFolder + "/txtlog.txt";
		txtlog = fopen(txtlogFile.c_str(), "w");

		for (map<string, string>::iterator itr = fileNames.begin() ; itr != fileNames.end() ; itr++)
		{
			lk.lock();
			string log = (*itr).first + " -> " + getFileNameWithoutDirectory((*itr).second) + "\n";
			fwrite(log.c_str(), sizeof(char), log.length(), txtlog);
			lk.unlock();
		}
		fclose(txtlog);

		string xmlFile = publishFolder + "/PageInfo.xml";
		QFile file(stdStringToQString(xmlFile));
		file.open(QFile::WriteOnly | QFile::Text);

		QXmlStreamWriter xmlWriter(&file);
		xmlWriter.setAutoFormatting(true);
		xmlWriter.writeStartDocument();
		xmlWriter.writeStartElement("Catalog");
		xmlWriter.writeAttribute("id", stdStringToQString(getFileTitle(qStringToStdString(publishFile_))));
		xmlWriter.writeStartElement("PageCount");
		QString tempNum;
		tempNum.setNum(document_->scenes().size());
		xmlWriter.writeAttribute("count", tempNum);
		xmlWriter.writeEndElement();
		
		//QString xml = QString("<Catalog id = \'") + stdStringToQString(getFileTitle(qStringToStdString(publishFile_))) + "\'/>";

		int xmlpagecount = 1;
		int thmbcount = 0;
		BOOST_FOREACH(const string & file, copiedFiles)
		{
			string ext = getFileExtension(file);
			if (ext != "st")
			{
				string copiedfile = publishFolder + "/" + getFileNameWithoutDirectory(file);
				//if (ext == "jpg" ||
				//	ext == "jpeg")
				//	outFiles.push_back(copiedfile);
				FILE *in, *out;
				char* buf;
				size_t len;
				if ((in  = fopen(file.c_str(), "rb")) == NULL) return ;
				if ((out = fopen(copiedfile.c_str(), "wb")) == NULL) { fclose(in); return; }

				if ((buf = (char *) malloc(16777216)) == NULL) { fclose(in); fclose(out); return; }

				lk.lock();
				if (ext == "jpg" ||
					ext == "jpeg")
				{
					//outFiles.push_back(copiedfile);
					if (temp <= thmbcount)
					{
						xmlWriter.writeStartElement("PageMatching");
						tempNum.setNum(xmlpagecount++);
						xmlWriter.writeAttribute("page", tempNum);
						xmlWriter.writeAttribute("file", stdStringToQString(getFileTitle(copiedfile)));
						xmlWriter.writeEndElement();
					}
					//else
					//{
					//	outFiles.push_back(copiedfile + ".base.jpg");
					//	outFiles.push_back(copiedfile + ".enh.jpg");
					//}
				}
				progress_++;
				displayMsg_.sprintf("Compressing file: %s", 
					getFileNameWithoutDirectory(file).c_str());
				while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
				{
					if (fwrite(buf, sizeof(char), len, out) == 0) {
						fclose(in); fclose(out);
						free(buf);
						_unlink(copiedfile.c_str());
						return;
					}
				}
				lk.unlock();
				fclose(in); fclose(out);
				free(buf);
			}
			thmbcount++;
		}
		xmlpagecount = 1;
		xmlWriter.writeEndDocument();
		file.close();
		std::string jpgdir = qStringToStdString(EditorGlobal::instance().programDirectory());
		
		vector<string> inFiles;
		inFiles.push_back(jpgdir+"/split.bat");
		inFiles.push_back(jpgdir+"/jpegsplit.exe");
		inFiles.push_back(jpgdir+"/jpeg62.dll");
		inFiles.push_back(jpgdir+"/jpegtran.exe");
		BOOST_FOREACH(const string & file, inFiles)
		{
			string copiedfile = publishFolder + "/" + getFileNameWithoutDirectory(file);
			outFiles.push_back(copiedfile);
			FILE *in, *out;
			char* buf;
			size_t len;
			if ((in  = fopen(file.c_str(), "rb")) == NULL) return ;
			if ((out = fopen(copiedfile.c_str(), "wb")) == NULL) { fclose(in); return; }

			if ((buf = (char *) malloc(16777216)) == NULL) { fclose(in); fclose(out); return; }

			lk.lock();
			while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
			{
				if (fwrite(buf, sizeof(char), len, out) == 0) {
					fclose(in); fclose(out);
					free(buf);
					_unlink(copiedfile.c_str());
					return;
				}
			}
			lk.unlock();
			fclose(in); fclose(out);
			free(buf);
		}
		string splitfile = jpgdir + "/split.bat";
		QString tempqstring = stdStringToQString(splitfile);
		std::wstring ws = tempqstring.toStdWString();
		LPWSTR s =  const_cast<LPWSTR>(ws.c_str());

		QString tempqstring2 = stdStringToQString(publishFolder);
		std::wstring ws2 = tempqstring2.toStdWString();
		LPWSTR s2 =  const_cast<LPWSTR>(ws2.c_str());

		SHELLEXECUTEINFO sei;
		ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof (SHELLEXECUTEINFO);
		sei.fMask  = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		sei.lpVerb = L"open";
		sei.lpFile = s;
		sei.lpDirectory = s2;
		sei.lpParameters = 0;
		sei.nShow  = SW_HIDE;

		lk.lock();
		ShellExecuteEx (&sei);
		WaitForSingleObject (sei.hProcess, INFINITE);
		lk.unlock();

		// catalog title
		QSize qsize(336, 448);
		QImage qimage;
		qimage.load(stdStringToQString(firstSceneImage));
		QImage trqimage = qimage.scaled(qsize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		trqimage.save(stdStringToQString(publishFolder +  "/catalog_title.jpg"));
		outFiles.push_back(publishFolder +  "/catalog_title.jpg");


		inFiles.clear();
		inFiles.push_back(publishFolder +  "/catalog_title.jpg");
		inFiles.push_back(copiedFiles[9]);

		for (int i = 0 ; i < inFiles.size() ; i++)
		{
			const string file = inFiles[i];
			string copiedfile;
			if (i == 0)
				copiedfile = publishFolder +  "/catalog_title";
			else
				copiedfile = publishFolder +  "/catalog_thumb.jpg";
			FILE *in, *out;
			char* buf;
			size_t len;
			if ((in  = fopen(file.c_str(), "rb")) == NULL) return ;
			if ((out = fopen(copiedfile.c_str(), "wb")) == NULL) { fclose(in); return; }

			if ((buf = (char *) malloc(16777216)) == NULL) { fclose(in); fclose(out); return; }

			lk.lock();
			progress_++;
			displayMsg_.sprintf("Compressing file: %s", 
				getFileNameWithoutDirectory(copiedfile).c_str());
			while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
			{
				if (fwrite(buf, sizeof(char), len, out) == 0) {
					fclose(in); fclose(out);
					free(buf);
					_unlink(copiedfile.c_str());
					return;
				}
			}
			lk.unlock();
			fclose(in); fclose(out);
			free(buf);
		}

		QFileInfoList fileList;
		for (int i = 0 ; i < outFiles.size() ; i++)
		{
			remove(outFiles[i].c_str());
		}
		string zoomFolder = refFiles_[temp];
		vector<string> zoomFiles;
		zoomFolder = getDirectory(zoomFolder) + "zoom";
		dir.setPath(stdStringToQString(zoomFolder));
		if (dir.exists())
		{
			int checkcount = 0;
			fileList = dir.entryInfoList();
			for (int i = 2 ; i < fileList.size() ; i++)
			{
				QFileInfo fileInfo = fileList.at(i);
				string origZoomfile = qStringToStdString(fileInfo.filePath());
				char* c;
				c = (char *)malloc(sizeof(int));
				itoa(temp+i+checkcount-2, c, 10);
				string copiedZoomfile = publishFolder + "/f"+ c +".jpg.zoom.jpg";
				string existFile = publishFolder + "/f"+ c +".jpg";
				string existBaseFile = publishFolder + "/f"+ c +".jpg.base.jpg";

				bool exists = false;

				do
				{
					FILE * file = fopen(existFile.c_str(), "rb");
					exists = file != NULL;
					if (file) fclose(file);
					if (!exists)
					{
						file = fopen(existBaseFile.c_str(), "rb");
						exists = file != NULL;
						if (file) fclose(file);
					}

					if (!exists)
					{
						checkcount++;
						itoa(temp+i+checkcount-2, c, 10);
						copiedZoomfile = publishFolder + "/f"+ c +".jpg.zoom.jpg";
						existFile = publishFolder + "/f"+ c +".jpg";
						existBaseFile = publishFolder + "/f"+ c +".jpg.base.jpg";
					}
				}while (!exists);

				free(c);


				FILE *in, *out;
				char* buf;
				size_t len;
				if ((in  = fopen(origZoomfile.c_str(), "rb")) == NULL) return ;
				if ((out = fopen(copiedZoomfile.c_str(), "wb")) == NULL) { fclose(in); return; }

				if ((buf = (char *) malloc(16777216)) == NULL) { fclose(in); fclose(out); return; }

				lk.lock();
				progress_++;
				displayMsg_.sprintf("Compressing file: %s", 
					getFileNameWithoutDirectory(copiedZoomfile).c_str());
				while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
				{
					if (fwrite(buf, sizeof(char), len, out) == 0) {
						fclose(in); fclose(out);
						free(buf);
						_unlink(copiedZoomfile.c_str());
						return;
					}
				}
				lk.unlock();
				fclose(in); fclose(out);
				free(buf);
			}
		}
	}
	else if(document_->documentTemplate()->isPreDownload())
	{
		vector<string> predownloadZipFiles;
		//predownloadZipFiles.push_back(documentFile);

		QDir dir;
		string publishFolder = qStringToStdString(publishFile_);
		publishFolder = getDirectory(publishFolder) + getFileTitle(publishFolder);
		dir.setPath(stdStringToQString(publishFolder));
		dir.mkpath(dir.absolutePath());

		BOOST_FOREACH(const string & file, copiedFiles)
		{
			string ext = getFileExtension(file);
			if (ext == "jpg" || ext == "png")
			{
				string copiedfile = publishFolder + "/" + getFileNameWithoutDirectory(file);
				FILE *in, *out;
				char* buf;
				size_t len;
				if ((in  = fopen(file.c_str(), "rb")) == NULL) return ;
				if ((out = fopen(copiedfile.c_str(), "wb")) == NULL) { fclose(in); return; }

				if ((buf = (char *) malloc(16777216)) == NULL) { fclose(in); fclose(out); return; }

				lk.lock();
				progress_++;
				displayMsg_.sprintf("Compressing file: %s", 
					getFileNameWithoutDirectory(file).c_str());
				while ( (len = fread(buf, sizeof(char), sizeof(buf), in)) != NULL )
				{
					if (fwrite(buf, sizeof(char), len, out) == 0) {
						fclose(in); fclose(out);
						free(buf);
						_unlink(copiedfile.c_str());
						return;
					}
				}
				lk.unlock();
				fclose(in); fclose(out);
				free(buf);
			}
			else
			{
				predownloadZipFiles.push_back(file);
			}
		}
		if (!makeIntoZipFile(&mutex_, &progress_, &displayMsg_, zipFile, predownloadZipFiles, cancel_))
			return;
	}
	else
	{
		if (!makeIntoZipFile(&mutex_, &progress_, &displayMsg_, zipFile, copiedFiles, cancel_))
			return;
	}

	lk.lock();
	progress_ = (int)copiedFiles.size();
	lk.unlock();

	
	
	string docZipFileStr = qStringToStdString(publishFile_);
	remove(qStringToStdString(publishFile_).c_str());
	QFile::rename(stdStringToQString(zipFile), publishFile_);
	

	xmlfiles_.clear();
	origNames_.clear();
	destNames_.clear();

	lk.lock();
	displayMsg_ = tr("Publishing complete");
}


bool PublishingDlg::exchangeXmltext(std::vector<std::string> & fileNames, std::map<std::string, std::vector<std::string>> & orignames, std::map<std::string, std::vector<std::string>> & newtexts)
{	
	xmlDocPtr doc = NULL;	
	QDomDocument qdoc("xmldoc");


	for(int index=0; index< xmlcnt_; index++)
	{
		QFile* file = new QFile(stdStringToQString(fileNames[index]));
		if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
			return false;
		}  	
		if (!qdoc.setContent(file)) 
		{          
    		return false;      
		}      
		//Get the root element
		QDomElement docElem = qdoc.documentElement(); 
		QDomNodeList qnodelist = docElem.elementsByTagName("fileName");

		//exchange xml text data
		std::vector<std::string>::iterator origIter = orignames[fileNames[index]].begin();
		std::vector<std::string>::iterator netIter = newtexts[fileNames[index]].begin();
		while(origIter != orignames[fileNames[index]].end() && netIter != newtexts[fileNames[index]].end())
		{
			QString qOrigfile = stdStringToQString(*origIter);

			for (int i = 0; i < qnodelist.count(); i++)
			{
				QDomElement el = qnodelist.at(i).toElement();
				QString curFileName = el.text();
		
				if(curFileName == qOrigfile)
				{									
					el.childNodes().at(0).setNodeValue(stdStringToQString(*netIter));
					QString curFileNames = el.text();
					break;	
				}				
			}

			++origIter;
			++netIter;
		}		

		file->close();
		file->setFileName(stdStringToQString(fileNames[index]));

		if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
			return false;
		}

		QTextStream stream( file );
		qdoc.save(stream,  QDomNode::EncodingFromDocument);
		file->close();
		delete file;
	}

	return true;
}
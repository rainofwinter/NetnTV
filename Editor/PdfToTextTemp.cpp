#include "stdafx.h"
#include "PdfToTextTemp.h"
#include "Utils.h"
#include "FileUtils.h"
#include "Document.h"
#include "EditorDocument.h"
#include "Global.h"
#include "Scene.h"
#include "Text.h"
#include "Image.h"
#include "SceneObject.h"
#include "Root.h"
#include "EditorGlobal.h"

PdfToTextTemp::PdfToTextTemp(EditorDocument * document, const QString & pdfFilename, QWidget * parent)
{
	document_ = document;
	pdfFilename_ = pdfFilename;
	parent_ = parent;
}
void PdfToTextTemp::readXml(const QString & filename, QProgressDialog * progress1)
{
	std::string workdir = getDirectory(qStringToStdString(filename));
	progress1->setValue(0);
	int roop = 0;
	int sceneCnt = 1;
	QFile* file = new QFile(filename);
	file->open(QIODevice::ReadOnly | QIODevice::Text);
	Text * text = NULL;
	SceneSPtr s;
	QXmlStreamReader xml(file);	// file name
	bool firstText = true;
	bool firstScene = true;
	/* We'll parse the XML until we reach end of it.*/
	while(!xml.atEnd()){ //&&
		//!xml.hasError()) {
		progress1->setValue(roop++);
		if (roop > 99) roop = 0;
		qApp->processEvents();
		if (progress1->wasCanceled())
		{
			QMessageBox::information(
				0, 
				QObject::tr("Cancel"), 
				QObject::tr("Cancel"));
			return;
		}
		/* Read next element.*/
		QXmlStreamReader::TokenType token = xml.readNext();
		/* If token is just StartDocument, we'll go to next.*/
		if(token == QXmlStreamReader::StartDocument) {
			continue;
		}
		if(token == QXmlStreamReader::EndElement) {
			if(xml.name() == "PAGE") {
			}
		}
		/* If token is StartElement, we'll see if we can read it.*/
		if(token == QXmlStreamReader::StartElement) {
			// page : scene create
			if(xml.name() == "PAGE") {
				QXmlStreamAttributes attributes = xml.attributes();
				float scenewidth = attributes.value("width").toString().toFloat();
				float sceneheight = attributes.value("height").toString().toFloat();
				if (firstScene)
				{
					pdf2imgDir_ = importPdfWithoutText(pdfFilename_, scenewidth * 2, sceneheight * 2);
					firstScene = false;
				}
				s.reset(new Scene);
				s->setScreenWidth(scenewidth);
				s->setScreenHeight(sceneheight);
				s->setName(attributes.value("id").toString().toStdString());
				document_->document()->addScene(s);

				//QDir dir(stdStringToQString(getDirectory(qStringToStdString(filename))));
				//QString temp = dir.absolutePath();
				//QString title = stdStringToQString(getFileTitle(qStringToStdString(filename))) + ".xml_data";
				//QString folder = temp + "/" + title;
				//QString vecFileName = folder + "/" + QString("image-%1.vec").arg(sceneCnt);

				//FILE * f = fopen(qStringToStdString(vecFileName).c_str(), "rb");
				//bool exists = f != NULL;
				//if (f) fclose(f);

				//if (exists)
				//{
				//	QString pngfilename = vec2png(vecFileName, scenewidth, sceneheight);
				//	Image* img = (Image*)Global::instance().createSceneObject("Image");
				//	img->setFileName(qStringToStdString(pngfilename));
				//	img->setId("vec");
				//	s->root()->addChild(SceneObjectSPtr(img));
				//}

				firstText = true;
				sceneCnt++;

				Image* img = (Image*)Global::instance().createSceneObject("Image");
				img->setId("bg_img");

				QString imgfilename = pdf2imgDir_ + "/" + QString("f_%1.png").arg(9998+sceneCnt);
				img->setFileName(qStringToStdString(imgfilename));
				img->setWidth(scenewidth);
				img->setHeight(sceneheight);

				s->root()->addChild(SceneObjectSPtr(img));
				continue;
			}
			/*else if(xml.name() == "IMAGE")
			{
			QXmlStreamAttributes attributes = xml.attributes();
			Image* img = (Image*)Global::instance().createSceneObject("Image");
			float x = attributes.value("x").toString().toFloat();
			float y = attributes.value("y").toString().toFloat();
			float width = attributes.value("width").toString().toFloat();
			float height = attributes.value("height").toString().toFloat();
			img->setId(attributes.value("id").toString().toStdString());
			Transform t = img->transform();
			t.setTranslation(x, y, 0);
			img->setTransform(t);

			QString imgfilename = attributes.value("href").toString();
			std::string tempstr = qStringToStdString(imgfilename);
			if (getFileExtension(tempstr) != "jpg" ||
			getFileExtension(tempstr) != "png")
			{
			QImage qimage;
			qimage.load(imgfilename);
			std::string png = getDirectory(tempstr) + getFileTitle(tempstr) + ".png";
			imgfilename = stdStringToQString(png);
			qimage.save(imgfilename, "PNG");
			}
			img->setFileName(qStringToStdString(imgfilename));
			img->setWidth(width);
			img->setHeight(height);

			s->root()->addChild(SceneObjectSPtr(img));
			}*/
			// text : text create
			else if(xml.name() == "TEXT") {
				if (firstText)
				{
					SceneObjectSPtr newChildSPtr = s->findObject("vec");
					if (newChildSPtr)
						s->root()->addChild(newChildSPtr);
					firstText = false;
				}
				QXmlStreamAttributes attributes = xml.attributes();
				text = (Text*)Global::instance().createSceneObject("Text");
				text->setTextString(L"");
				text->setId(qStringToStdString(attributes.value("id").toString()));
				text->setCanSelectPart(true);
				float x = attributes.value("x").toString().toFloat();
				float y = attributes.value("y").toString().toFloat();
				/*Transform t = text->transform();
				t.setTranslation(x, y, 0);
				text->setTransform(t);*/

				xml.readNext();
				bool firstprop = true;

				while(xml.tokenType() != QXmlStreamReader::EndElement && xml.name() != "TEXT")
				{
					if(xml.name() == "TOKEN") {
						QXmlStreamAttributes tokenattributes = xml.attributes();
						QString fontDir = 
							QDesktopServices::storageLocation(QDesktopServices::FontsLocation);
						//QString fontFile = fontDir + "/malgun.ttf";
						QString localdir = stdStringToQString(getDirectory(qStringToStdString(filename)));
						QString fontname = tokenattributes.value("font-name").toString();

						std::wstring wstrfilename = fontname.toStdWString();
						std::string strfilename(wstrfilename.begin(), wstrfilename.end());
						unsigned pos = strfilename.find("@") + 1;
						strfilename = strfilename.substr(pos);

						QString localfontFile = localdir + stdStringToQString(strfilename) + ".ttf";
						QString fontFile = fontDir + "/" + stdStringToQString(strfilename) + ".ttf";
						if (QFile(localfontFile).exists())
							fontFile = localfontFile;
						else if (!QFile(fontFile).exists())
							fontFile = fontDir + "/malgun.ttf";
						TextProperties prop;
						std::vector<TextProperties> props = text->properties();
						TextProperties refprop = props.back();
						prop = props.back();
						//props.clear();
						TextFont font;
						font.fontFile = convertToRelativePath(qStringToStdString(fontFile));
						font.faceIndex = 0;
						font.bold = tokenattributes.value("bold").toString().toStdString() == "yes" ? true : false;
						font.italic = tokenattributes.value("italic").toString().toStdString() == "yes" ? true : false;
						font.pointSize = tokenattributes.value("font-size").toString().toFloat() * 72.27 / 96.0;

						prop.font = font;

						QString colorstring = tokenattributes.value("font-color").toString();
						float r = colorstring.mid(1,2).toLong(0, 16) / 255.0;
						float g = colorstring.mid(3,2).toLong(0, 16) / 255.0;
						float b = colorstring.mid(5,2).toLong(0, 16) / 255.0;
						prop.color = Color(r,g,b,1);

						if (refprop.color != prop.color ||
							refprop.font.bold != prop.font.bold ||
							refprop.font.italic != prop.font.italic ||
							refprop.font.pointSize != prop.font.pointSize )
						{
							if (firstprop)
							{
								props.clear();
								prop.index = 0;
							}
							else
								prop.index = text->textString().length() + 1;
							props.push_back(prop);
							text->setProperties(props);
						}

						QString textstring = xml.readElementText();
						if (firstprop)
						{
							firstprop = false;
							text->setTextString(textstring.toStdWString());
						}
						else
							text->setTextString(text->textString() + L" " + textstring.toStdWString());
					}
					xml.readNext();
				}
				Transform t = text->transform();
				int plusheight = 0;
				std::vector<TextProperties> props = text->properties();
				BOOST_FOREACH(const TextProperties & prop, props)
				{
					if (prop.font.pointSize > plusheight) plusheight = prop.font.pointSize;
				}
				plusheight = plusheight/3;
				t.setTranslation(x, y - plusheight, 0);
				text->setTransform(t);

				text->init(document_->document()->renderer());
				s->root()->addChild(SceneObjectSPtr(text));
				text->setLetterSpacing(-1.5);
			}
		}
	}
	/* Error handling. */
	if(xml.hasError()) {
		QMessageBox::critical(NULL,
			"QXSRExample::parseXML",
			xml.errorString(),
			QMessageBox::Ok);
	}
	progress1->setValue(100);
	/* Removes any device() or data from the reader
	* and resets its internal state to the initial state. */
	xml.clear();
	delete file;

	document_->callSceneListChanged();
}

QString PdfToTextTemp::importPdfWithoutText(const QString & fileName, const float & width, const float & height)
{
	int importedPDFCount = 0;
	QString outDir;// = QString("./Res/pdf_%1").arg(importedPDFCount);
	QDir dir;
	do
	{
		importedPDFCount++;
		outDir = QString("./Res/pdf_%1").arg(importedPDFCount);
		dir.setPath(outDir);
	}while (dir.exists());
	dir.mkpath(dir.absolutePath());

	// Create a progress dialog
	QProgressDialog progress1(QObject::tr("Converting file"),
		QObject::tr("Cancel"), 0, 100, parent_);
	progress1.setWindowTitle(QObject::tr("Progress"));
	progress1.setWindowModality(Qt::WindowModal);
	QProgressBar bar(&progress1);
	bar.setTextVisible(false);
	progress1.setBar(&bar);
	progress1.show();

	int roop = 0;

	progress1.setValue(0);
	qApp->processEvents();

	QVariant qwidth(width);
	QVariant qheight(height);

	QString program = EditorGlobal::instance().pdf2imgLocation();//"C:/git_working/pdf2img/Debug/pdf2img.exe";
	QDir tempdir("C:/temp");
	bool hasdir = tempdir.exists();
	if (!hasdir) _mkdir("C:/temp");
	QString newFileName = "C:/temp/380a558a-9f74-4f60-be48-0123181812ce.pdf";
	QFile::copy(fileName, newFileName);
	QStringList arguments;
	arguments << qStringToStdString(fileName).c_str() << qStringToStdString(dir.absolutePath()).c_str()
		<< qStringToStdString(qwidth.toString()).c_str() << qStringToStdString(qheight.toString()).c_str();

	QProcess *myProcess = new QProcess(parent_);
	myProcess->start(program, arguments);

	while(myProcess->state() != QProcess::NotRunning)
	{
		progress1.setValue(roop++);
		if (roop > 99) roop = 0;
		qApp->processEvents();
		if (progress1.wasCanceled())
		{
			myProcess->kill();
			myProcess->waitForFinished(-1);
			QMessageBox::information(
				0, 
				QObject::tr("Cancel"), 
				QObject::tr("Cancel"));
			return QString("");
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	}
	progress1.setValue(100);

	myProcess->kill();
	myProcess->waitForFinished(-1);

	QString pdf2imgDir = dir.path();

	return pdf2imgDir;
}
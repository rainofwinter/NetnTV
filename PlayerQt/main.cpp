#include "stdafx.h"
#include "Xml.h"

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "VideoMedia.h"

#include "Core.h"

#include "PcVideoPlayer.h"
#include "PcAudioPlayer.h"
#include "PcPlayerSupport.h"


#include "Root.h"
#include "Text.h"
#include "Text_2.h"
#include "TextFont.h"

#include "ScriptProcessor.h"

#include "MainWindow.h"

//TODO REMOVE
#include "AppImage.h"
#include "Camera.h"
#include "FileUtils.h"
#include "direct.h"

#include "LogDlg.h"
#include "Utils.h"

using namespace std;

FILE * gLogFile;
void Log(const std::string & logString)
{
	fprintf(gLogFile, "%s", logString.c_str());
}

void LogInit()
{
	gLogFile = fopen("log.txt", "wb");
}

void LogUninit()
{
	fclose(gLogFile);
}


//#define LOGGING

#ifdef LOGGING
#define LOG(str) Log(str);
#define LOG_INIT()	LogInit();
#define LOG_UNINIT() LogUninit();
#else
#define LOG(str)
#define LOG_INIT()
#define LOG_UNINIT()
#endif

LPDIRECTSOUND8 gDirectSound8 = 0;

void initMediaEnv(HWND hwnd)
{
	HRESULT hr;
	hr = DirectSoundCreate8(NULL, &gDirectSound8, NULL);
	LOG("DirectSound Create\r\n");
	if (hr == DS_OK)
	{
		hr = gDirectSound8->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);	
		LOG("DirectSound SetCooperativeLevel\r\n");
	}	
	
	av_register_all();
	LOG("av_register_all\r\n");
}

//#include "FittingRoom.h"

//#include "Model.h"
//#include "SpatialPartitioning.h"
//#include "DShow.h"
int main(int argc, char *argv[])
{
	/*
	HRESULT hr;

	IGraphBuilder *pGraph = NULL;
	IMediaControl *pControl = NULL;
	IMediaEvent   *pEvent = NULL;
	
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		int a = 4;
	}
	
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, 
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

	hr = pGraph->RenderFile(L"E:\\ST\\test.mp4", NULL);

	if (SUCCEEDED(hr))
	{
		// Run the graph.
		hr = pControl->Run();
		if (SUCCEEDED(hr))
		{
			// Wait for completion.
			long evCode;
			pEvent->WaitForCompletion(INFINITE, &evCode);

			// Note: Do not use INFINITE in a real application, because it
			// can block indefinitely.
		}
	}


	pControl->Release();
	pEvent->Release();
	pGraph->Release();
	CoUninitialize();
	*/
	/*
	Model * model;
	FILE * file = fopen("C:\\st\\model\\level\\level.stm", "rb");
	model = Model::read(file);
	KDTree kdTree;
	kdTree.genFromModel(model);

	bool intersect;
	Vector3 intPt;

	intersect = kdTree.intersect(&intPt, Vector3(97.551f, 7.687f, 93.203f), Vector3(80.975f, 7.687f, 128.964f));

	intersect = kdTree.intersect(&intPt, 
		Vector3(
			7.72f, 
			17.62f, 
			19.349f), 
		Vector3(
			22.649f, 
			-6.362f, 
			45.249f));

	fclose(file);
	delete model;*/
	/*
	Model * model;
	const char * fileName = "E:\\ST\\uangel3d\\scene\\scene.stm";
	//const char * fileName = "E:\\ST\\model\\level\\level.stm";
	FILE * file = fopen(fileName, "rb");
	model = Model::read(getDirectory(fileName).c_str(), file);
	fclose(file);
	KDTree kdTree(48);
	kdTree.genFromModel(NULL, model);

	delete model;
	*/


	Document * document = 0;
	QApplication a(argc, argv);
	
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);	
#endif
	
	std::string fileNameStr;

	if (argc == 2)
	{
		fileNameStr = argv[1];
		//remove starting and trailing quotes
		string::size_type qPos1 = fileNameStr.find_first_of("\"");
		string::size_type qPos2 = fileNameStr.find_last_of("\"");
		if (qPos1 == 0 && qPos2 == fileNameStr.size() - 1)
		{
			fileNameStr = fileNameStr.substr(1, fileNameStr.size() - 2);
		}			
	}
	/*
	try
	{	
		Reader(fileNameStr.c_str()).read(document);
		
		string directory = getDirectory(fileNameStr);
		chdir(directory.c_str());
		Global::instance().setDirectories(directory.c_str(), directory.c_str(), document->remoteReadServer());
		
		
		//try
		//{
		//	XmlWriter xmlWriter;
		//	xmlWriter.write(document, "c:/st/test.xml");

		//	XmlReader xmlReader;
		//	Document * newDocument = xmlReader.read("c:/st/test.xml");	
		//	delete document;
		//	document = newDocument;

		//} catch (const XmlException & e)
		//{
		//	cout << "Xml read error: " << e.line() << " : " << e.what() << "\n";

		//}
		

		document->readTextFeatures(directory + "__textFeatures.xml");		
	}
	catch(const Exception & e)
	{		
		QString msg;
		msg.sprintf("Could not open file: %s", e.what());
		QMessageBox::information(0, QObject::tr("Error"), msg);
		return 0;
	}
	catch(...)
	{
		QMessageBox::information(0, QObject::tr("Error"), QObject::tr("Could not open file"));
		return 0;
	}
	*/
	//setupFittingRoom(document, "modified.st");

	//document->setScriptFile("C:\\ST\\kbs bug\\script2.js");
	//document->setScriptFile("C:\\ST\\jsTest\\script.js");

	/*
	Camera windowCam;	
	PcVideoPlayer * videoPlayer = new PcVideoPlayer(document);
	PcAudioPlayer * audioPlayer = new PcAudioPlayer(document);
	PcPlayerSupport * playerSupport = new PcPlayerSupport;	
	Global::init(videoPlayer, audioPlayer, playerSupport);
	*/
	Q_INIT_RESOURCE(playerqt);

	bool isStartFileOpen = !(fileNameStr.empty());

	MainWindow w(isStartFileOpen);
	a.installEventFilter(&w);

//	initMediaEnv((HWND)w.winId());
	
	LogDlg dlg(&w);
	dlg.hide();
	w.setLog(&dlg);

	w.show();
	if (!fileNameStr.empty())
		w.initDocument(fileNameStr);
	bool ret = a.exec();

//	Global::uninit();

	return ret;
}

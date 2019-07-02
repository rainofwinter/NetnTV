#include "stdafx.h"
#include "Activation.h"
#include "EditorGlobal.h"
#include "SerialDlg.h"
//#include "SerialFromServerDlg.h"
#include <default.h>
#include <hex.h>
#include "Iphlpapi.h"

using namespace std;
using namespace CryptoPP;





///////////////////////////////////////////////////////////////////////////////
//hdd serial
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <boost/scope_exit.hpp>
# pragma comment(lib, "wbemuuid.lib")

/*
See http://msdn.microsoft.com/en-us/library/windows/desktop/aa390423(v=vs.85).aspx
*/

wstring getWmiProperty(IWbemClassObject *pclsObj, const wstring & propName)
{
	VARIANT vtProp;
	pclsObj->Get(propName.c_str(), 0, &vtProp, 0, 0);	
	wstring propStr = vtProp.bstrVal;
	VariantClear(&vtProp);
	return propStr;
}

template <typename T>
bool doWmiQuery(IWbemServices *pSvc, const wstring & queryStr, T & wmiQueryObj)
{
	IEnumWbemClassObject* pEnumerator = NULL;
	HRESULT hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t(queryStr.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL, &pEnumerator);

	if (FAILED(hres)) return false;

	IWbemClassObject *pclsObj;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		pclsObj = NULL;
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
		if(0 == uReturn) break;
		wmiQueryObj(pclsObj);
		pclsObj->Release();
	}
	pEnumerator->Release();
	return true;
}

struct WmiGetDriveLetter
{
	void operator () (IWbemClassObject *pclsObj)
	{
		wstring systemDeviceWStr = getWmiProperty(pclsObj, L"SystemDevice");
		wchar_t retBuffer[1024];
		for (wchar_t c = L'A'; c <= L'Z'; ++c)
		{
			wchar_t driveLetterS[3];
			wsprintf(driveLetterS, L"%c:", c);
			if (QueryDosDevice(driveLetterS, retBuffer, 1024))
			{				
				if (!wcscmp(retBuffer, systemDeviceWStr.c_str()))
				{
					driveLetter = driveLetterS;
					break;
				}
			}
		}
	}
	wstring driveLetter;
};

struct WmiGetAntecedent
{
	WmiGetAntecedent(const wstring & dependent)
	{
		dependentVal = dependent;
	}
	void operator () (IWbemClassObject *pclsObj)
	{
		wstring propWStr = getWmiProperty(pclsObj, L"Dependent");	
		wstring::size_type pos1 = propWStr.find_first_of(L'\"');
		wstring::size_type pos2 = propWStr.find_last_of(L'\"');
		wstring valWStr = propWStr.substr(pos1 + 1, pos2-pos1 - 1);

		if (valWStr == dependentVal)
		{
			propWStr = getWmiProperty(pclsObj, L"Antecedent");	
			pos1 = propWStr.find_first_of(L'\"');
			pos2 = propWStr.find_last_of(L'\"');
			val = propWStr.substr(pos1 + 1, pos2-pos1 - 1);
		}
	}
	wstring dependentVal;
	wstring val;
};

struct WmiGetVal
{
	WmiGetVal(const wstring & prop) {this->prop = prop;}
	void operator () (IWbemClassObject *pclsObj)
	{
		val = getWmiProperty(pclsObj, prop.c_str());
	}
	wstring prop;
	wstring val;
};

wstring getDiskSerialNumberWStr()
{
	HRESULT hres;

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
	bool coinit = true;
	if (FAILED(hres))
	{
		coinit = false;
		//may faile if it has already been called by Qt framework
	}

	IWbemServices *pSvc = NULL;
	IWbemLocator *pLoc = NULL;
	BOOST_SCOPE_EXIT( (&pSvc) (&pLoc) (&coinit)) {
		if (pSvc) pSvc->Release();
		if (pLoc) pLoc->Release();	
		if (coinit) CoUninitialize();
	} BOOST_SCOPE_EXIT_END

		hres =  CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if (FAILED(hres)) return L"";


	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &pLoc);
	if (FAILED(hres)) return L"";

	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);		
	if (FAILED(hres)) return L"";

	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres)) return L"";

	wstringstream querySs;		

	WmiGetDriveLetter wmiGetDriveLetter;
	doWmiQuery(pSvc, L"SELECT SystemDevice FROM Win32_OperatingSystem", wmiGetDriveLetter);
	if (wmiGetDriveLetter.driveLetter.empty()) return L"";

	querySs.str(L"");
	querySs << "SELECT * FROM Win32_LogicalDiskToPartition";
	WmiGetAntecedent wmiGetPartition(wmiGetDriveLetter.driveLetter);	
	doWmiQuery(pSvc, querySs.str(), wmiGetPartition);
	if (wmiGetPartition.val.empty()) return L"";

	querySs.str(L"");
	querySs << "SELECT * FROM Win32_DiskDriveToDiskPartition";
	WmiGetAntecedent wmiGetDeviceId(wmiGetPartition.val);
	doWmiQuery(pSvc, querySs.str(), wmiGetDeviceId);
	if (wmiGetDeviceId.val.empty()) return L"";

	querySs.str(L"");
	querySs << L"SELECT SerialNumber FROM WIN32_DiskDrive WHERE DeviceID = \"" << wmiGetDeviceId.val << "\"";
	WmiGetVal wmiGetSerial(L"SerialNumber");
	doWmiQuery(pSvc, querySs.str(), wmiGetSerial);
	if (wmiGetSerial.val.empty()) return L"";

	return wmiGetSerial.val;
}

string getDiskSerialNumber()
{
	wstring wstrSerial = getDiskSerialNumberWStr();
	return string(wstrSerial.begin(), wstrSerial.end());
}
///////////////////////////////////////////////////////////////////////////////

string getGuid()
{
	wchar_t buffer[512] = {0};
	DWORD size = sizeof(buffer);
	HKEY key = 0;
	LONG ret = 0;

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Cryptography", 0, 
		KEY_QUERY_VALUE | KEY_WOW64_64KEY, &key);

	if (ret != ERROR_SUCCESS) return "";	
	
	ret = RegQueryValueEx(key, 
		L"MachineGuid", 0, 0, (BYTE *)buffer, &size);

	if (ret != ERROR_SUCCESS) 
	{
		RegCloseKey(key);
		return "";
	}
	else
	{
		RegCloseKey(key);
	}

	return string(buffer, buffer + wcslen(buffer));
}

string getMAC()
{
	ULONG  bufferLength = 0;
	BYTE * buffer = 0;
	if (GetAdaptersInfo(0, &bufferLength) == ERROR_BUFFER_OVERFLOW)
	{
		buffer = new BYTE[bufferLength];
	}
	else
	{
		return "";
	}

	PIP_ADAPTER_INFO adapterInfo =
      reinterpret_cast<PIP_ADAPTER_INFO>(buffer);
	GetAdaptersInfo( adapterInfo, &bufferLength );
	
	unsigned int serialNumber;

	if (!adapterInfo)
	{
		delete [] buffer;
		return "";
	}
	
	QString macAddr;
	for (int i = 0; i < adapterInfo->AddressLength; ++i)
	{
		QString sub;
		sub.sprintf("%02x", adapterInfo->Address[i]);
		macAddr += sub;
	}

	delete [] buffer;
	return macAddr.toStdString();
}
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "md5.h"
#include <boost/crc.hpp>
string getActivationNumberFirstPart()
{
	std::string output;
	std::string input;
	std::string diskSerial = getDiskSerialNumber();

	if (!diskSerial.empty())
		input = diskSerial;
	else
		input += getGuid() + getMAC();

	Weak::MD5 hash;
	StringSource src(input, true,
		new HashFilter(hash, new HexEncoder(new StringSink(output))));
	
	return output.substr(0, 16);
}

string md5half(time_t val)
{
	std::string output;
	std::stringstream ss;
	ss << val;
	
	Weak::MD5 hash;
	StringSource src(ss.str(), true,
		new HashFilter(hash, new HexEncoder(new StringSink(output))));
	
	return output.substr(0, 16);
}

string getActivationNumberSecondPart()
{
	TrialState actTTrialState;	
	if (!actTTrialState.read()) 
	{
		actTTrialState.mostRecentTime = time(NULL);
		actTTrialState.trialStartedTime = 0;
		actTTrialState.write();	
	}	
	std::string correctActivationTime = md5half(actTTrialState.mostRecentTime);
	return correctActivationTime;
}
bool checkTrialState()
{
	string serial = getActivationNumberFirstPart();
	if (serial == "")
	{	
		QMessageBox::information(0, QObject::tr("Error"), 
			QObject::tr("Could not identify machine"));
		return false;
	}

	//string correctActivationTime = getActivationNumberSecondPart();

	TrialState actTTrialState;	
	if (!actTTrialState.read()) 
	{
		actTTrialState.mostRecentTime = time(NULL);
		actTTrialState.trialStartedTime = 0;
		actTTrialState.write();	
	}	
	std::string correctActivationTime = md5half(actTTrialState.mostRecentTime);

	bool failed = false;
	while (1)
	{
		TrialState trialState;

		if (failed)
		{
			//ask for activation key
			stringstream ss;
			//ss << correctSerial << "-" << correctActivationTime;
			ss << serial << correctActivationTime;

			
			SerialDlg dlg(QString::fromStdString(ss.str()), 0);
			if (!dlg.exec()) return false;

			trialState = dlg.trialState();
			
			
			//SerialFromServerDlg dlg(QString::fromStdString(ss.str()), 0);
			//if (!dlg.exec()) return false;
			//

			//trialState = dlg.trialState();
			
		}
		else
		{
			bool readRet = trialState.read();
			if (!readRet || trialState.isSerialNotSet())
			{
				failed = true;
				continue;
			}			
		}
		
		if (!trialState.isSerialEqual(serial) || 
			(!trialState.isPermanent() && failed && 			
			!trialState.isActivationTimeEqual(correctActivationTime)))
		{
			QMessageBox::information(0, "Trial", "Invalid key");
			failed = true;
			continue;
		}

		if (!trialState.update())
		{
			QMessageBox::information(0, "Trial", "System time has been set back");
			failed = true;
			continue;
		}

		if (trialState.isExpired())
		{
			QMessageBox::information(0, "Trial", "Trial expired");
			failed = true;
			continue;
		}

		trialState.write();

		//display remaining trial period if not permanent
		if (!trialState.isPermanent())
		{
			trialState.write();

			time_t remainingS = trialState.trialPeriodS() - trialState.elapsedS();
			
			time_t rD = (remainingS / 60 / 60 / 24);
			time_t rH = (remainingS / 60 / 60) % 24;
			time_t rM = (remainingS / 60) % 60;

			stringstream ss;
			ss << 
				rD << " days, " << rH << " hours, " << rM << 
				" minutes remaining in trial period";

			QMessageBox::information(0, "Trial", 
				QString::fromStdString(ss.str()));
		}

		break;
	}	
	return true;
}


///////////////////////////////////////////////////////////////////////////////
string TrialState::toString(const unsigned char data[TRUNCATEDSIZE]) const
{
	char buffer[TRUNCATEDSIZE];

	HexEncoder enc;
	enc.Put(data, TRUNCATEDSIZE);
	enc.MessageEnd();
	enc.Get((byte *)buffer, TRUNCATEDSIZE);
	
	return string(buffer, buffer + TRUNCATEDSIZE);
}

void TrialState::fromString(unsigned char data[TRUNCATEDSIZE], const std::string & hexStr)
{
	HexDecoder decoder;
	decoder.Put((unsigned char *)hexStr.c_str(), hexStr.size());
	decoder.MessageEnd();
	decoder.Get(data, TRUNCATEDSIZE*2);	
}

void TrialState::setSerial(const std::string & hexSerial)
{
	fromString(serial, hexSerial);
}

void TrialState::setActivationTime(const std::string & hexSerial)
{
	fromString(activationTime, hexSerial);
}

bool TrialState::isSerialNotSet() const
{
	bool equal = true;
	for (int i = 0; i < TRUNCATEDSIZE; ++i) equal &= (serial[i] == 0);
	return equal;
}

bool TrialState::isSerialEqual(const std::string & hexSerial) const
{
	return toString(serial) == hexSerial;	
}

bool TrialState::isActivationTimeEqual(const std::string & hexSerial) const
{
	return toString(activationTime) == hexSerial;
}

TrialState::TrialState()
{	
	memset(serial, 0, sizeof(serial));
	trialStartedTime = 0;
	mostRecentTime = 0;
	//90 days
	trialPeriod = 7776000;
	update();
}


std::string TrialState::writeStr()
{
	std::string outstr;

	
	char buffer[sizeof(TrialState)];
	memcpy(buffer, this, sizeof(TrialState));
	
	std::string pass = passPhrase();
	DefaultEncryptorWithMAC encryptor(pass.c_str(), new HexEncoder(new StringSink(outstr)));
	encryptor.Put((unsigned char *)buffer, sizeof(TrialState));
	encryptor.MessageEnd();
	return outstr;
}


bool TrialState::update()
{
	struct timeval mytime;    
	time_t curTime = time(NULL);

	//if clock has been set back
	if (curTime < mostRecentTime) return false;
	
	mostRecentTime = curTime;

	if (trialStartedTime == 0) trialStartedTime = mostRecentTime;
	return true;
}


time_t TrialState::elapsedS() const
{
	return mostRecentTime - trialStartedTime;
}

bool TrialState::read(const std::string & instr)
{
	try {
	string outstr;

	std::string pass = passPhrase();
	HexDecoder decryptor(new DefaultDecryptorWithMAC(pass.c_str(), new StringSink(outstr)));
	decryptor.Put((unsigned char *)instr.c_str(), instr.size());
	decryptor.MessageEnd();

	unsigned char * buffer = (unsigned char *)outstr.c_str();
	if (outstr.size() < sizeof(TrialState)) return false;
	memcpy(this, buffer, sizeof(TrialState));
	} catch (...)
	{
		return false;
	}

	return true;
}


#ifdef TRIALSTATE_REGISTRY

bool TrialState::read()
{
	std::string fromRegStr = 
		EditorGlobal::instance().settings()->value("settings").toString().toStdString();

	if (fromRegStr.empty()) return false;

	bool ret = read(fromRegStr);
	if (!ret) return false;

	return true;
}


bool TrialState::write()
{
	std::string dataStr = writeStr();
	EditorGlobal::instance().settings()->setValue(
		"settings", QString::fromStdString(dataStr));

	return true;
}

#endif

bool TrialState::readFile(const std::string & fileName)
{
	std::string fromFileStr;
	FILE * file = fopen(fileName.c_str(), "rb");
	if (file)
	{
		const size_t BUF_LEN = 4096;
		size_t read = 0;
		char buffer[BUF_LEN];
		while ((read = fread(buffer, 1, BUF_LEN, file)) > 0)	
			fromFileStr.insert(fromFileStr.end(), buffer, buffer + read);

		fclose(file);
	}

	if (!read(fromFileStr)) return false;
	return true;
}


char nextChar(unsigned long & holdRand)
{
	return (((holdRand = holdRand * 214013L + 2531011L) >> 16) & 0x7fff) % 255;
}

std::string TrialState::passPhrase() const
{
	unsigned long holdRand = 7483;

	std::string pass;
	for (int i = 0; i < 48; ++i)
	{
		pass.push_back(nextChar(holdRand));
	}

	return pass;	
}





///////////////////////////////////////////////////////////////////////////////
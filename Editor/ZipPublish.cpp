#include "stdafx.h"
#include "zip.h"
#include "FileUtils.h"
#include "ZipPublish.h"

#include <boost/scope_exit.hpp>

//TODO: Better error handling

using namespace std;
using namespace boost;

uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
{
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATAA ff32;

      hFind = FindFirstFileA(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}



bool makeIntoZipFile(
	boost::mutex * m, int * fileCounter, QString * displayMsg,
	const std::string & zipFileName, const std::vector<std::string> & files,
	const bool & cancel
	)
{
	unique_lock<mutex> lk(*m);	
	*fileCounter = 0;
	lk.unlock();
	zipFile zf;
	int err;
	zf = zipOpen64(zipFileName.c_str(), 0);

	int size_buf = 512 * 1024;
	unsigned char * buf = (unsigned char *)malloc(size_buf);

	BOOST_SCOPE_EXIT( (&buf) (&zf) )
	{
		int errClose = zipClose(zf,NULL);
		if (errClose != ZIP_OK)
		{
			//error
		}

		free(buf);		
	} BOOST_SCOPE_EXIT_END
	

	if (!zf)
	{
		//error
	}
	
	BOOST_FOREACH(std::string file, files)
	{
		if (file.empty()) continue;
		lk.lock();
		displayMsg->sprintf("Compressing file: %s", 
			getFileNameWithoutDirectory(file).c_str());
		lk.unlock();
		zip_fileinfo zi;
		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
		filetime(file.c_str(),&zi.tmz_date,&zi.dosDate);

		std::string savefilenameinzip = getFileNameWithoutDirectory(file);

		int zip64 = 0; //TODO set this if file is large

		FILE * fin = fopen64(file.c_str(),"rb");
		if (fin==NULL)
		{
			//error
			continue;
		}

		err = zipOpenNewFileInZip3_64(zf,savefilenameinzip.c_str(),&zi,
			 NULL,0,NULL,0,NULL, Z_DEFLATED,
			 Z_DEFAULT_COMPRESSION,0,
			 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			 NULL,0, zip64);

		if (err != ZIP_OK)
		{
			//error
		}

		

		int size_read = 0;
		do
		{
			err = ZIP_OK;
			size_read = (int)fread(buf,1,size_buf,fin);
			if (size_read < size_buf)
				if (feof(fin)==0)
			{
				//error
 				err = ZIP_ERRNO;
			}

			if (size_read>0)
			{
				err = zipWriteInFileInZip (zf,buf,size_read);
				if (err<0)
				{
					//error
				}

			}
		} while ((err == ZIP_OK) && (size_read>0));

		if (fin)
			fclose(fin);

		err = zipCloseFileInZip(zf);
		if (err!=ZIP_OK)
		{
			//error
		}
		lk.lock();
		++*fileCounter;
		if (cancel) return false;
		lk.unlock();
	}
	

	
	return true;
}

void spliceZipFile(const std::string & zipFile, const std::string & origZipFile)
{
	FILE * fout = fopen64(zipFile.c_str(), "wb");
	
	int size_buf = 512*1024;
	unsigned char * buf;
	buf = (unsigned char *)malloc(size_buf);


	int size_read = 0;
	int counter = 0;
	while(1)
	{
		std::string::size_type pos = origZipFile.find_last_of('.');
		std::string partFileName = origZipFile.substr(0, pos);
		char extension[10];
		sprintf(extension, ".%03d", counter + 1);
		partFileName += extension;

		FILE * fin = fopen64(partFileName.c_str(), "rb");
		if (!fin) break;
		do 
		{
			size_read = (int)fread(buf,1,size_buf,fin);
			if (size_read < size_buf)
				if (feof(fin)==0)
			{
				//error
			}

			if (size_read>0)
			{
				fwrite(buf, 1, size_read, fout);				
			}
		} while (size_read > 0);

		fclose(fin);		
		++counter;
	} while (size_read>0);

	fclose(fout);

	free(buf);
}

void splitZipFile(const std::string & zipFile, int partSize)
{
	FILE * fin = fopen64(zipFile.c_str(), "rb");
	
	int size_buf = partSize;
	unsigned char * buf;
	buf = (unsigned char *)malloc(size_buf);


	int size_read = 0;
	int counter = 0;
	do
	{
		size_read = (int)fread(buf,1,size_buf,fin);
		if (size_read < size_buf)
			if (feof(fin)==0)
		{
			//error
		}

		if (size_read>0)
		{		
			char extension[10];
			sprintf(extension, ".%03d", counter + 1);
			std::string partFileName = zipFile + extension;

			FILE * fout = fopen64(partFileName.c_str(), "wb");
			if (!fout)
			{
				//error
			}
			fwrite(buf, 1, size_read, fout);
			fclose(fout);
		}
		++counter;
	} while (size_read > 0);

	fclose(fin);

	free(buf);
}
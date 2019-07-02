bool makeIntoZipFile(boost::mutex * m, int * fileCounter, QString * displayMsg,
	const std::string & zipFileName, const std::vector<std::string> & files,
	const bool & cancel);

void splitZipFile(const std::string & zipFile, int partSize);

void spliceZipFile(const std::string & zipFile, const std::string & origZipFile);

/*
std::vector<std::string> files;
files.push_back("C:/koala.jpg");
files.push_back("c:/110112_Germany_star_opossum.jpg");

makeIntoZipFile("c:/test.zip", files);
splitZipFile("c:/test.zip", 128*1024);
spliceZipFile("c:/testSplice.zip", "c:/test.zip");
*/
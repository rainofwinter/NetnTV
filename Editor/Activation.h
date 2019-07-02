#pragma once

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "md5.h"

#define TRIALSTATE_REGISTRY

bool checkTrialState();

#define TRUNCATEDSIZE 16

class TrialState
{
public:
	//time_t values are all in s units
	
	//---------------------------------------------
	//unsigned __int64 serial;
	unsigned char serial[TRUNCATEDSIZE];
	unsigned char activationTime[TRUNCATEDSIZE];

	//---------------------------------------------
	time_t trialPeriod;
	time_t trialStartedTime;
	time_t mostRecentTime;

	TrialState();

public:
	std::string toString(const unsigned char data[TRUNCATEDSIZE]) const;
	
	void setSerial(const std::string & hexSerial);
	void setActivationTime(const std::string & hexSerial);
	
	bool isSerialNotSet() const;
	bool isSerialEqual(const std::string & hexSerial) const;
	bool isActivationTimeEqual(const std::string & hexSerial) const;
	std::string writeStr();
#ifdef TRIALSTATE_REGISTRY
	bool read();
	bool write();
#endif
	
	bool read(const std::string & instr);	
	bool readFile(const std::string & fileName);

	bool update();

	time_t elapsedS() const;
	time_t trialPeriodS() const {return trialPeriod;}

	boolean isPermanent() const {return trialPeriod == 0;}
	boolean isExpired() const {return trialPeriod != 0 && elapsedS() > trialPeriodS();}

private:
	void fromString(unsigned char data[TRUNCATEDSIZE], const std::string & hexStr);
	std::string passPhrase() const;
};


std::string getActivationNumberFirstPart();
std::string getActivationNumberSecondPart();
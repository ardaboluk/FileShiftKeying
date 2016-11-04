#ifndef WAVEHEADER_H
#define WAVEHEADER_H

#include <string>

class WaveHeader
{
private:
	std::string sGroupID;						// RIFF
	unsigned int dwFileLength;					//total file length minus 8, which is taken up by RIFF
	std::string sRiffType;						//always WAVE

public:
	WaveHeader();
	void setGroupID(std::string p_sGroupID);
	void setFileLength(unsigned int p_dwFileLength);
	void setRiffType(std::string p_sRiffType);
	
	std::string getGroupID();
	unsigned int getFileLength();
	std::string getRiffType();
};

#endif
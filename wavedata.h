
#ifndef WAVEDATA_H
#define WAVEDATA_H

#include <string>
#include <vector>

class WaveData
{
private:
	std::string sChunkID;								//"data"
	unsigned int dwChunkSize;							//length of header in bytes
	std::vector<short int>* shortArray;					//16-bit audio data

public:
	WaveData();

	void setChunkID(std::string p_sChunkID);
	void setChunkSize(unsigned int p_dwChunkSize);
	void setShortArray(std::vector<short int>* p_shortArray);

	std::string getChunkID();
	unsigned int getChunkSize();
	std::vector<short int>* getShortArray();
};

#endif
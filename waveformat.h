
#ifndef WAVEFORMAT_H
#define WAVEFORMAT_H

#include <string>

class WaveFormat
{
private:
	std::string sChunkID;									//Four bytes: "fmt "
	unsigned int dwChunkSize;								//Length of header in bytes
	unsigned short int wFormatTag;							//1 (MS PCM)
	unsigned short int wChannels;							//Number of channels
	unsigned int dwSamplesPerSec;							//Frequency of the audio in Hz... 44100
	unsigned int dwAvgBytesPerSec;							//for estimating RAM allocation
	unsigned short int wBlockAlign;							//sample frame size, in bytes
	unsigned short int wBitsPerSample;						//bits per sample

public:
	WaveFormat();

	void setChunkID(std::string p_sChunkID);
	void setChunkSize(unsigned int p_dwChunkSize);
	void setFormatTag(unsigned short int p_wFormatTag);
	void setChannels(unsigned short int p_wChannels);
	void setSamplesPerSec(unsigned int p_dwSamplesPerSec);
	void setAvgBytesPerSec(unsigned int p_dwAvgBytesPerSec);
	void setBlockAlign(unsigned short int p_wBlockAlign);
	void setBitsPerSample(unsigned short int p_wBitsPerSample);

	std::string getChunkID();
	unsigned int getChunkSize();
	unsigned short int getFormatTag();
	unsigned short int getChannels();
	unsigned int getSamplesPerSec();
	unsigned int getAvgBytesPerSec();
	unsigned short int getBlockAlign();
	unsigned short int getBitsPerSample();

};

#endif
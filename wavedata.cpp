
#include "wavedata.h"

WaveData::WaveData()
{

}

void WaveData::setChunkID(std::string p_sChunkID)
{
	sChunkID = p_sChunkID;
}

void WaveData::setChunkSize(unsigned int p_dwChunkSize)
{
	dwChunkSize = p_dwChunkSize;
}

void WaveData::setShortArray(std::vector<short int>* p_shortArray)
{
	shortArray = p_shortArray;
}

std::string WaveData::getChunkID()
{
	return sChunkID;
}

unsigned int WaveData::getChunkSize()
{
	return dwChunkSize;
}

std::vector<short int>* WaveData::getShortArray()
{
	return shortArray;
}
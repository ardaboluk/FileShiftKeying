
#include "waveformat.h"

WaveFormat::WaveFormat()
{

}

void WaveFormat::setChunkID(std::string p_sChunkID)
{
	sChunkID = p_sChunkID;
}

void WaveFormat::setChunkSize(unsigned int p_dwChunkSize)
{
	dwChunkSize = p_dwChunkSize;
}

void WaveFormat::setFormatTag(unsigned short int p_wFormatTag)
{
	wFormatTag = p_wFormatTag;
}

void WaveFormat::setChannels(unsigned short int p_wChannels)
{
	wChannels = p_wChannels;
}

void WaveFormat::setSamplesPerSec(unsigned int p_dwSamplesPerSec)
{
	dwSamplesPerSec = p_dwSamplesPerSec;
}

void WaveFormat::setAvgBytesPerSec(unsigned int p_dwAvgBytesPerSec)
{
	dwAvgBytesPerSec = p_dwAvgBytesPerSec;
}

void WaveFormat::setBlockAlign(unsigned short int p_wBlockAlign)
{
	wBlockAlign = p_wBlockAlign;
}

void WaveFormat::setBitsPerSample(unsigned short int p_wBitsPerSample)
{
	wBitsPerSample = p_wBitsPerSample;
}

std::string WaveFormat::getChunkID()
{
	return sChunkID;
}

unsigned int WaveFormat::getChunkSize()
{
	return dwChunkSize;
}

unsigned short int WaveFormat::getFormatTag()
{
	return wFormatTag;
}

unsigned short int WaveFormat::getChannels()
{
	return wChannels;
}

unsigned int WaveFormat::getSamplesPerSec()
{
	return dwSamplesPerSec;
}

unsigned int WaveFormat::getAvgBytesPerSec()
{
	return dwAvgBytesPerSec;
}

unsigned short int WaveFormat::getBlockAlign()
{
	return wBlockAlign;
}

unsigned short int WaveFormat::getBitsPerSample()
{
	return wBitsPerSample;
}

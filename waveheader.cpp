
#include "waveheader.h"

WaveHeader::WaveHeader()
{

}

void WaveHeader::setGroupID(std::string p_sGroupID)
{
	sGroupID = p_sGroupID;
}

void WaveHeader::setFileLength(unsigned int p_dwFileLength)
{
	dwFileLength = p_dwFileLength;
}

void WaveHeader::setRiffType(std::string p_sRiffType)
{
	sRiffType = p_sRiffType;
}

std::string WaveHeader::getGroupID()
{
	return sGroupID;
}

unsigned int WaveHeader::getFileLength()
{
	return dwFileLength;
}

std::string WaveHeader::getRiffType()
{
	return sRiffType;
}

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include "wavedata.h"
#include "waveformat.h"
#include "waveheader.h"

#define AMPLITUDE								10000							//amplitude of the bfsked sine wave that will be generated
#define SAMPLES_PER_SECOND						44100							//samples per second (44.1 kHZ is CD quality)

//!!!CAUTION!!! space and mark frequencies should be changed with caution if necessary. 1378 and 2756 are special values for Goertzel algortihm to 
//give correct result because 44100/2756=16 and 44100/5512=8. 16 and 8 are both power of two and goertzel algorithm requires the number of samples 
//it analyses to be a power of two
#define SPACE_FREQUENCY     2756
#define MARK_FREQUENCY      5512

//forward declarations
char convertBitsToByte(std::vector<short int>* p_bits_array);
std::vector<short int>* convertByteToBits(char p_byte);
void readWAVFile(std::string p_wav_filename, WaveHeader* p_waveHeader, WaveFormat* p_waveFormat, WaveData* p_waveData);
void extractDataFromBFSKedSineWave(std::vector<short int>* p_bfsked_sine_wave, std::vector<char>* p_demodulated_byte_array, int p_start_sample, int p_sample_count, int p_sample_offset);
float goertzel_magnitude(int num_samples, int TARGET_FREQUENCY, int SAMPLING_RATE, std::vector<short int>* p_samples_data);
void saveFile(std::string p_filename, std::vector<char>* p_demodulated_byte_array);
void readFile(std::string p_file_name, std::vector<char>* p_byte_array);
void generateBFSKedSineWave(std::vector<char>* p_bytes, std::vector<short int>* p_fsked_sine_wave, short int p_amplitude, int p_sample_rate, short int p_space_frequency, 
	short int p_mark_frequency);
void saveWAVFile(std::string p_wav_file_name, WaveHeader* p_wave_header, WaveFormat* p_wave_format, WaveData* p_wave_data);

int main(int argc, char** argv)
{

	int choice_modulate_demodulate = 0;
	std::cout<<"Please choose:\n";
	std::cout<<"1: Modulate file into WAV\n";
	std::cout<<"2: Demodulate WAV into file\n";

	std::cin>>choice_modulate_demodulate;

	if(choice_modulate_demodulate == 1)
	{
		std::string filename;
		std::string wav_filename;
		std::vector<char>* byte_array = new std::vector<char>();
		std::vector<short int>* bfsked_sine_wave = new std::vector<short int>();

		//get name of the file that will be converted to .wav and name of the wav file that will be created
		std::cout<< "Please enter name of the file you want to be encoded into WAV file\n";
		std::cin>>filename;
		std::cout<< "Please enter name of the WAV file you want to be created\n";
		std::cin>>wav_filename;

		readFile(filename, byte_array);
		generateBFSKedSineWave(byte_array, bfsked_sine_wave, AMPLITUDE, SAMPLES_PER_SECOND, SPACE_FREQUENCY, MARK_FREQUENCY);

		//set wrappers (set size of each chunk last)
		WaveData* waveData = new WaveData();
		waveData -> setChunkID("data");
		waveData -> setShortArray(bfsked_sine_wave);
		waveData -> setChunkSize((waveData -> getShortArray() -> size() * sizeof(waveData -> getShortArray() -> at(0))));

		WaveFormat* waveFormat = new WaveFormat();
		waveFormat -> setChunkID("fmt ");
		waveFormat -> setFormatTag(1);
		waveFormat -> setChannels(1);
		waveFormat -> setSamplesPerSec(SAMPLES_PER_SECOND);
		waveFormat -> setBitsPerSample(16);									//short int is 16-bit. So, 16-bit samples are used
		waveFormat -> setBlockAlign(waveFormat -> getChannels() * waveFormat -> getBitsPerSample() / 8);
		waveFormat -> setAvgBytesPerSec(waveFormat -> getSamplesPerSec() * waveFormat -> getBlockAlign());
		waveFormat -> setChunkSize(sizeof(waveFormat -> getFormatTag()) + sizeof(waveFormat -> getChannels()) + sizeof(waveFormat -> getSamplesPerSec()) + sizeof(waveFormat -> getAvgBytesPerSec())
			+ sizeof(waveFormat -> getBlockAlign()) + sizeof(waveFormat -> getBitsPerSample()));

		WaveHeader* waveHeader = new WaveHeader();
		waveHeader -> setGroupID("RIFF");
		//!!!CAUTION!!! while the length of the wave file is calculated, wave header groupid and wave header rifftype is not take into account
		//that is, 36 is the size of everything except "RIFF" and "WAVE"
		waveHeader -> setFileLength(36 + waveData -> getShortArray() -> size() * sizeof(waveData -> getShortArray() -> at(0)));
		waveHeader -> setRiffType("WAVE");

		//create the wav file and write the data into it
		saveWAVFile(wav_filename, waveHeader, waveFormat, waveData);

		//inform the user about generated file
		std::cout<<"\nSpecifications of the generated WAV file: \n";
		std::cout<<"Path of the WAV file generated: "<<wav_filename<<"\n";
		std::cout<<"Number of channels: "<<waveFormat -> getChannels()<<"\n";
		std::cout<<"Sample rate: "<<waveFormat -> getSamplesPerSec()<<"\n";
		std::cout<<"Average bytes per second: "<<waveFormat -> getAvgBytesPerSec()<<"\n";
		std::cout<<"Bits per sample: "<<waveFormat -> getBitsPerSample()<<"\n";
		std::cout<<"File size: "<<waveHeader -> getFileLength()<<"\n";

		//inform the user about sound wave
		std::cout<<"\nSpecifications of the sound wave: \n";
		std::cout<<"Amplitude: "<<AMPLITUDE<<"\n";
		std::cout<<"Space frequency: "<<SPACE_FREQUENCY<<"\n";
		std::cout<<"Mark frequency: "<<MARK_FREQUENCY<<"\n";

		//request the user to enter a character to terminate the program
		int termination_character = 0;
		std::cout<<"\nPlease enter a character to terminate the program: \n";
		std::cin>>termination_character;
	}
	else if(choice_modulate_demodulate == 2)
	{
		std::string wav_filename;
		std::string filename;

		//binary contents of the wav file
		std::vector<char>* wav_file_data = new std::vector<char>();

		std::vector<char>* demodulated_byte_array = new std::vector<char>();

		//wrappers for wav file
		WaveHeader* waveHeader = new WaveHeader();
		WaveFormat* waveFormat = new WaveFormat();
		WaveData*  waveData = new WaveData();

		//get the name of the wav file from user
		std::cout<<"Please enter the name of the WAV file you want to be modulated:\n";
		std::cin>>wav_filename;

		//get the name of the file that will be generated
		std::cout<<"Please enter the name of the file you want to be created:\n";
		std::cin>>filename;

		//read the WAV file
		readWAVFile(wav_filename, waveHeader, waveFormat, waveData);

		//extract data from samples and populate demodulated_byte_array
		extractDataFromBFSKedSineWave(waveData -> getShortArray(), demodulated_byte_array, 0, 32, 62);

		//create a file whose name user just gave and write demodulated_byte_array into the file
		saveFile(filename, demodulated_byte_array);
	}

	return 0;

}

//this method returns bits array for given byte
std::vector<short int>* convertByteToBits(char p_byte)
{
	//binary array of given byte
	std::vector<short int>* bits_of_given_byte = new std::vector<short int>();

	//loop for each bit, do a shift and do a logic AND
	for(int bit_counter= 0; bit_counter<8; bit_counter++)
		bits_of_given_byte -> insert(bits_of_given_byte -> begin(), (p_byte>>bit_counter)&1);

	return bits_of_given_byte;
}

//this method returns byte for given bits array
char convertBitsToByte(std::vector<short int>* p_bits_array)
{
	char byte_from_bits = 0;
	for(int bit_counter = 0; bit_counter<8; bit_counter++)
		byte_from_bits = byte_from_bits + p_bits_array -> at(bit_counter) * pow((float)2, 7 - bit_counter);
	return byte_from_bits;
}

//this method reads given file and populates p_byte_array with bytes in the file
void readFile(std::string p_file_name, std::vector<char>* p_byte_array)
{
	//get the file to read
	std::ifstream file_to_read(p_file_name, std::ios::binary);
	//copy all data into buffer
	p_byte_array -> assign(std::istreambuf_iterator<char>(file_to_read), std::istreambuf_iterator<char>());
}

//this method reads WAV file and populates given wrappers
void readWAVFile(std::string p_wav_filename, WaveHeader* p_waveHeader, WaveFormat* p_waveFormat, WaveData* p_waveData)
{
	//get the file to read
	std::ifstream wav_file_to_read(p_wav_filename, std::ios::binary);
	//sample data
	std::vector<short int>* sample_data = new std::vector<short int> ();

	//read the header
	//read group id
	char waveHeader_groupID[5];
	wav_file_to_read.get(waveHeader_groupID, sizeof(waveHeader_groupID), '\0');
	std::string s_waveHeaderGroupID(waveHeader_groupID);
	p_waveHeader -> setGroupID(s_waveHeaderGroupID);
	//read file length
	unsigned int waveHeader_fileLength;
	wav_file_to_read.read((char*) &waveHeader_fileLength, sizeof(waveHeader_fileLength));
	p_waveHeader  -> setFileLength(waveHeader_fileLength);
	//read riff type
	char waveHeader_riffType[5];
	wav_file_to_read.get(waveHeader_riffType, sizeof(waveHeader_riffType), '\0');
	std::string s_waveHeaderRiffType(waveHeader_riffType);
	p_waveHeader -> setRiffType(s_waveHeaderRiffType);
	//read the format
	//read chunk id
	char waveFormat_chunkID[5];
	wav_file_to_read.get(waveFormat_chunkID, sizeof(waveFormat_chunkID), '\0');
	std::string s_waveFormatChunkID(waveFormat_chunkID);
	p_waveFormat -> setChunkID(s_waveFormatChunkID);
	//read chunk size
	unsigned int waveFormat_chunkSize;
	wav_file_to_read.read((char*) &waveFormat_chunkSize, sizeof(waveFormat_chunkSize));
	p_waveFormat -> setChunkSize(waveFormat_chunkSize);
	//read format tag
	unsigned short int waveFormat_tag;
	wav_file_to_read.read((char*) &waveFormat_tag, sizeof(waveFormat_tag));
	p_waveFormat -> setFormatTag(waveFormat_tag);
	//read channels
	unsigned short int waveFormat_channels;
	wav_file_to_read.read((char*) &waveFormat_channels, sizeof(waveFormat_channels));
	p_waveFormat -> setChannels(waveFormat_channels);
	//read samples per second
	unsigned int waveFormat_samplesPerSec;
	wav_file_to_read.read((char*) &waveFormat_samplesPerSec, sizeof(waveFormat_samplesPerSec));
	p_waveFormat -> setSamplesPerSec(waveFormat_samplesPerSec);
	//read average bytes per second
	unsigned int waveFormat_avgBytesPerSec;
	wav_file_to_read.read((char*) &waveFormat_avgBytesPerSec, sizeof(waveFormat_avgBytesPerSec));
	p_waveFormat -> setAvgBytesPerSec(waveFormat_avgBytesPerSec);
	//read block align
	unsigned short int waveFormat_blockAlign;
	wav_file_to_read.read((char*) &waveFormat_blockAlign, sizeof(waveFormat_blockAlign));
	p_waveFormat -> setBlockAlign(waveFormat_blockAlign);
	//read bits per sample
	unsigned short int waveFormat_bitsPerSample;
	wav_file_to_read.read((char*) &waveFormat_bitsPerSample, sizeof(waveFormat_bitsPerSample));
	p_waveFormat -> setBitsPerSample(waveFormat_bitsPerSample);

	//read the data
	//read chunk id
	char waveData_chunkID[5];
	wav_file_to_read.get(waveData_chunkID, sizeof(waveData_chunkID), '\0');
	std::string s_waveDataChunkID(waveData_chunkID);
	p_waveData -> setChunkID(s_waveDataChunkID);
	//read chunk size
	unsigned int waveData_chunkSize;
	wav_file_to_read.read((char*) &waveData_chunkSize, sizeof(waveData_chunkSize));
	p_waveData -> setChunkSize(waveData_chunkSize);
	//read sample data
	int sample_count = p_waveData -> getChunkSize() / (p_waveFormat -> getBitsPerSample() / 8);
	for(int sample_counter = 0; sample_counter<sample_count; sample_counter++)
	{
		short int current_sample;
		wav_file_to_read.read((char*)&current_sample, sizeof(current_sample));
		sample_data -> push_back(current_sample);
	}
	p_waveData -> setShortArray(sample_data);
}

//this method extracts binary data from given wave samples
//analysing starts from p_start_sample. In each step samples up to p_sample_count is analysed. After an analysis complete, next analysis start from p_sample_offset after current start point
void extractDataFromBFSKedSineWave(std::vector<short int>* p_bfsked_sine_wave, std::vector<char>* p_demodulated_byte_array, int p_start_sample, int p_sample_count, int p_sample_offset)
{
	//start analysis from p_start_sample
	int current_analysis_start_index = p_start_sample;

	//this vector will hold samples up to p_sample_count and will be cleared for each analysis step
	std::vector<short int>* samples_to_be_analysed = new std::vector<short int>();

	//this vector will hold bits. When size reaches 8, bits will be converted to byte and the vector will be cleared. Byte will be pushed to p_demodulated_byte_array
	std::vector<short int>* bits_of_current_byte = new std::vector<short int>();

	while(current_analysis_start_index < p_bfsked_sine_wave->size())
	{
		//check if there are enough samples left for analyzing
		if(current_analysis_start_index + p_sample_count < p_bfsked_sine_wave->size()){

			//get samples to be analysed
			for(int sample_counter = 0; sample_counter < p_sample_count; sample_counter++)
			{
				samples_to_be_analysed->push_back(p_bfsked_sine_wave->at(current_analysis_start_index + sample_counter));
			}
		}else{
			break;
		}

		//analyse samples
		float space_frequency_magnitude = goertzel_magnitude(samples_to_be_analysed->size(), SPACE_FREQUENCY, SAMPLES_PER_SECOND, samples_to_be_analysed);
		float mark_frequency_magnitude = goertzel_magnitude(samples_to_be_analysed->size(), MARK_FREQUENCY, SAMPLES_PER_SECOND, samples_to_be_analysed);

		//if the wave which current samples represent is at space frequency, this means a 0 otherwise it's a 1
		if(space_frequency_magnitude > mark_frequency_magnitude)
		{
			bits_of_current_byte->push_back(0);
		}
		else
		{
			bits_of_current_byte->push_back(1);
		}

		//if the size of bits_of_current_byte reached 8, convert bits to byte, push the byte to p_demodulated_byte_array and clear bits_of_current_byte
		if(bits_of_current_byte->size() == 8)
		{
			char current_byte = convertBitsToByte(bits_of_current_byte);
			p_demodulated_byte_array->push_back(current_byte);
			bits_of_current_byte->clear();
		}

		//go to next analysis point
		current_analysis_start_index += p_sample_offset;

		//clear samples_to_be_analysed array
		samples_to_be_analysed->clear();
	}
}

//this method populates given p_fsked_sine_wave array with binary frequency shift keyed sine wave
void generateBFSKedSineWave(std::vector<char>* p_bytes, std::vector<short int>* p_fsked_sine_wave, short int p_amplitude, int p_sample_rate, short int p_space_frequency, 
	short int p_mark_frequency)
{
	short int amplitude = p_amplitude;								//amplitude of the wave
	short int space_frequency = p_space_frequency;					//space frequency for binary frequency shift keying 
	short int mark_frequency = p_mark_frequency;					//mark frequency for binary shift keying
	std::vector<short int>* bits_array_of_current_byte;				//array of bits of current byte
	int oscillation_count_for_space_frequency = 2;					//number of oscillations for each 0 bit

	float PI = 3.14159;												//PI number

	//loop through each byte
	for(int byte_counter = 0; byte_counter < p_bytes -> size(); byte_counter++)
	{
		//get bits of current byte
		bits_array_of_current_byte = convertByteToBits(p_bytes -> at(byte_counter));
		//generate a short wave for each bit of current byte
		for(int bit_counter = 0; bit_counter < bits_array_of_current_byte -> size(); bit_counter++)
		{
			//if the current byte is 0 generate oscillations at space frequency; else generate oscillations at mark frequency
			if(bits_array_of_current_byte -> at(bit_counter) == 0)
			{
				//generate oscillations at space frequency
				for(int oscillation_counter = 0; oscillation_counter < oscillation_count_for_space_frequency; oscillation_counter++)
				{					
					float radian_difference_between_two_consecutive_samples = (2*PI)/(p_sample_rate/space_frequency);
					for(float radian_for_current_sample = 0; radian_for_current_sample < (2*PI - 0.0001); radian_for_current_sample += radian_difference_between_two_consecutive_samples)
					{
						int sine_value_of_current_sample = amplitude * sin(radian_for_current_sample);
						p_fsked_sine_wave -> push_back(sine_value_of_current_sample);
					}
				}

				//generate a silence for some time
				for(int silence_counter = 0; silence_counter < 30; silence_counter++)
				{
					p_fsked_sine_wave->push_back(0);
				}
			}
			else
			{
				//generate oscillations at mark frequency
				for(int oscillation_counter = 0; oscillation_counter < 2 * oscillation_count_for_space_frequency; oscillation_counter++)
				{
					float radian_difference_between_two_consecutive_samples = (2*PI)/(p_sample_rate/mark_frequency);
					for(float radian_for_current_sample = 0; radian_for_current_sample < (2*PI); radian_for_current_sample += radian_difference_between_two_consecutive_samples)
					{
						int sine_value_of_current_sample = amplitude * sin(radian_for_current_sample);
						p_fsked_sine_wave -> push_back(sine_value_of_current_sample);
					}
				}

				//generate a silence for some time
				for(int silence_counter = 0; silence_counter < 30; silence_counter++)
				{
					p_fsked_sine_wave->push_back(0);
				}
			}
		}		
	}
}

//goertzel algorithm for analyzing frequency of given samples
float goertzel_magnitude(int numSamples,int TARGET_FREQUENCY,int SAMPLING_RATE, std::vector<short int>* data)
{
	int     k,i;
	float   floatnumSamples;
	float   omega,sine,cosine,coeff,q0,q1,q2,magnitude,real,imag;

	float   scalingFactor = numSamples / 2.0;

	float PI = 3.14159;

	floatnumSamples = (float) numSamples;
	k = (int) (0.5 + ((floatnumSamples * TARGET_FREQUENCY) / SAMPLING_RATE));
	omega = (2.0 * PI * k) / floatnumSamples;
	sine = sin(omega);
	cosine = cos(omega);
	coeff = 2.0 * cosine;
	q0=0;
	q1=0;
	q2=0;

	for(i=0; i<numSamples; i++)
	{
		q0 = coeff * q1 - q2 + data->at(i);
		q2 = q1;
		q1 = q0;
	}

	// calculate the real and imaginary results
	// scaling appropriately
	real = (q1 - q2 * cosine) / scalingFactor;
	imag = (q2 * sine) / scalingFactor;

	magnitude = sqrtf(real*real + imag*imag);
	return magnitude;
}

//this method saves the wav file with the waveHeader, waveFormat and waveData info
void saveWAVFile(std::string p_wav_file_name, WaveHeader* p_wave_header, WaveFormat* p_wave_format, WaveData* p_wave_data)
{
	//open or create the file
	std::ofstream wav_file(p_wav_file_name, std::ios::out|std::ios::binary);

	unsigned int uint_data_to_write = 0;
	unsigned short int ushortint_data_to_write = 0;

	//write wave header to the file
	wav_file.write(p_wave_header -> getGroupID().c_str(), sizeof(p_wave_header -> getGroupID().c_str()));

	uint_data_to_write = p_wave_header -> getFileLength();
	wav_file.write(reinterpret_cast<const char*>(&uint_data_to_write), sizeof(uint_data_to_write));

	wav_file.write(p_wave_header -> getRiffType().c_str(), sizeof(p_wave_header -> getRiffType().c_str()));

	//write wave format to the file
	wav_file.write(p_wave_format -> getChunkID().c_str(), sizeof(p_wave_format -> getChunkID().c_str()));

	uint_data_to_write = p_wave_format -> getChunkSize();
	wav_file.write(reinterpret_cast<const char*>(&uint_data_to_write), sizeof(uint_data_to_write));

	ushortint_data_to_write = p_wave_format -> getFormatTag();
	wav_file.write(reinterpret_cast<const char*>(&ushortint_data_to_write), sizeof(ushortint_data_to_write));

	ushortint_data_to_write = p_wave_format -> getChannels();
	wav_file.write(reinterpret_cast<const char*>(&ushortint_data_to_write), sizeof(ushortint_data_to_write));

	uint_data_to_write = p_wave_format -> getSamplesPerSec();
	wav_file.write(reinterpret_cast<const char*>(&uint_data_to_write), sizeof(uint_data_to_write));

	uint_data_to_write = p_wave_format -> getAvgBytesPerSec();
	wav_file.write(reinterpret_cast<const char*>(&uint_data_to_write), sizeof(uint_data_to_write));

	ushortint_data_to_write = p_wave_format -> getBlockAlign();
	wav_file.write(reinterpret_cast<const char*>(&ushortint_data_to_write), sizeof(ushortint_data_to_write));

	ushortint_data_to_write = p_wave_format -> getBitsPerSample();
	wav_file.write(reinterpret_cast<const char*>(&ushortint_data_to_write), sizeof(ushortint_data_to_write));

	//write wave data to the file
	wav_file.write(p_wave_data -> getChunkID().c_str(), sizeof(p_wave_data -> getChunkID().c_str()));

	uint_data_to_write = p_wave_data -> getChunkSize();
	wav_file.write(reinterpret_cast<const char*>(&uint_data_to_write), sizeof(uint_data_to_write));

	std::vector<short int>* wave_data = p_wave_data -> getShortArray();
	for(int wave_data_counter = 0; wave_data_counter < wave_data -> size(); wave_data_counter++)
	{
		short int wave_data_value = wave_data -> at(wave_data_counter);
		wav_file.write(reinterpret_cast<const char*>(&wave_data_value),sizeof(wave_data_value));
	}

	//close the file
	wav_file.close();
}

void saveFile(std::string p_filename, std::vector<char>* p_demodulated_byte_array)
{
	//open or create the file
	std::ofstream binary_file(p_filename, std::ios::out|std::ios::binary);
	//write bytes to the file
	binary_file.write(&p_demodulated_byte_array->at(0), p_demodulated_byte_array->size());
}
#pragma once
#include "AudioManager.h"

struct WAV_HEADER //44 bytes
{

	char RIFF[4];			//4 bytes RIFF 
	uint32_t fileSize;		//4 bytes taille du fichier
	char WAVE[4];			//4 bytes Header type du fichier
	
	char fmt[4];			//4 bytes marqueur du chunk de formattage (dernier byte reservé à NULL)
	uint32_t dataLength;	//4 bytes taille du chunk de formattage (RIFF + fileSize + WAVE + fmt)
 					
	uint32_t byteRate;		//4 bytes nmbre de byte à lire par secondes : sampleRate * bytePerBloc
	uint16_t bytePerBloc;	//2 bytes (nbrChannels * bitsPerSample / 8)
	
	char dataSubChunk[4];	//4 bytes
	WAV_INFO Info;
};



int getFileSize(FILE *file);



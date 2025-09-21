#pragma once
#include "stdafx.h"



	//séparation en 2 headers différent pour réutilisation d'infos
struct WAV_INFO
{
	uint16_t audioFormat = 0;
	uint16_t nbrChannels = 0;
	uint32_t sampleRate = 0;
	uint16_t bitsPerSample = 0;
	uint32_t dataSize = 0;
};

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


static uint16_t read_u16_le(const char* p) { uint16_t v; std::memcpy(&v, p, sizeof(v)); return v; }

static uint16_t read_u32_le(const char* p) { uint32_t v; std::memcpy(&v, p, sizeof(v)); return v; }
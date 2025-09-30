#pragma once
#include <cstdint>
#include <vector>


enum ID3_FLAGS
{
	Unsync,
	ExtHeader,
	Experiment,
	Footer
};



struct MP3_HEADER
{
	uint32_t header;
	uint16_t syncWord() const{ return (header >> 20) & 0b111111111111; }
	uint8_t ver() const {return (header >> 19) & 0b1; }
	uint8_t layer() const { return (header >> 17) & 0b11; }
	uint8_t errProt() const { return(header >> 16) & 0b1; }
	uint8_t	bitRate() const { return (header >> 12) & 0b1111; }
	uint8_t frequency() const {return (header >> 10) & 0b11; }
	uint8_t	padBit() const { return(header >> 9) & 0b1; }
	uint8_t privBit() const { return(header >> 8) & 0b1;}
	uint8_t mode() const { return(header >> 6) & 0b11; }
	uint8_t	modeExt() const { return(header >> 4) & 0b11;}
	uint8_t cpy() const { return(header >> 3) & 0b1; }
	uint8_t original() const { return (header >> 2) & 0b1; }
	uint8_t emphasis() const { return(header >> 0) & 0b11;}
};

struct ID3V2_HEADER//10 bytes
{
	char ID3Tag[3];
	char ID3Vers[2];
	uint8_t ID3Flags; // faudra verifié si le 4eme bit est set, si il l'est y'aura un footer de 10 bytes à la fin du fichier
	uint32_t ID3Size;	//faire gaffe, synchsafe integer donc le MSB est toujours 0 donc %0xxxxxxx

};

struct MP3File
{
	ID3V2_HEADER IDheader;
	MP3_HEADER fileheader;
	std::vector<uint8_t> data;
};


std::vector<bool> flagCheck(uint8_t flags);

int getFrameSize(uint32_t header);
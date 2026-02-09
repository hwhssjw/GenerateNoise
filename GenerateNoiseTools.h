#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

class GenerateNoiseTools
{
public:
	std::vector<unsigned char> GenerateNoise_BW_Return(uint8_t imgX, uint8_t imgY);

	void GenerateNoise_BW_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY);

	std::vector<unsigned char> GenerateNoise_RGB_Return(uint8_t imgX, uint8_t imgY);

	void GenerateNoise_RGB_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY);

	std::vector<unsigned char> GenerateNoise_ARGB_Return(uint8_t imgX, uint8_t imgY);

	void GenerateNoise_ARGB_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY);

	//Number of channels: 1¨C3,Number of size: 1¨C10,Number of uneven: 1¨C10
	std::vector<unsigned char> GenerateNoise_Island_Return(uint8_t imgX, uint8_t imgY, int8_t size, int8_t Channel,int8_t uneven, std::vector<unsigned char> IslandColor,std::vector<unsigned char> Seacolor);

	void GenerateNoise_Island_Save(std::string name, std::string Path,uint8_t imgX, uint8_t imgY, int8_t size, int8_t Channel, int8_t uneven, std::vector<unsigned char> IslandColor, std::vector<unsigned char> Seacolor);

	std::vector<unsigned char> GenerateNoise_Random_Cutom_Return(uint8_t imgX, uint8_t imgY,bool Fast, int channel, std::vector<std::vector<unsigned char>> color, std::vector<int> Probability);

	void GenerateNoise_Random_Cutom_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY, bool Fast, int channel, std::vector<std::vector<unsigned char>> color, std::vector<int> Probability);
protected:

	uint16_t RandA();

	uint16_t RandB();

	uint16_t RandC();

	uint16_t RandD();

	long long currentTimeMillis();

	void ResetRandSeedA();

	void ResetRandSeedB();

	uint32_t Synthesis();

	std::vector<unsigned char> GenerateNoiseRGB_ARGB_BWG(uint8_t imgX, uint8_t imgY,int8_t Channel);
};


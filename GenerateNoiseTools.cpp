#include "GenerateNoiseTools.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <array>
#include <filesystem>
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ctime>
#include <bitset> 
#include <functional>
#include <string>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(1, 65535);

static uint32_t RandSeedA = 1;
static uint32_t RandSeedB = 1;
//-----------------------------------------------------------------------------
long long GenerateNoiseTools::currentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
//-----------------------------------------------------------------------------
uint16_t GenerateNoiseTools::RandA()
{
	RandSeedA = (RandSeedA * 9301 + 49297) % 233280;
	return static_cast<uint16_t>(RandSeedA % 65535);
}
//-----------------------------------------------------------------------------
uint16_t GenerateNoiseTools::RandB()
{
    RandSeedB = (RandSeedB * 1664525 + 1013904223) % 4294967296;
    return static_cast<uint16_t>(RandSeedA % 65535);
}
//-----------------------------------------------------------------------------
uint16_t GenerateNoiseTools::RandC()
{
    return static_cast<uint16_t>(((RandSeedB * RandSeedA) ^ (RandSeedB + RandSeedA)) % 65535);
}
//------------------------------------------------------------------------------
uint16_t GenerateNoiseTools::RandD()
{
    return static_cast<uint16_t>((RandSeedB + RandSeedA + RandC()) ^ (RandSeedB + RandSeedA) + 1);
}

//----------------------------------------------------------------------------
void GenerateNoiseTools::ResetRandSeedA()
{
    RandSeedA = (std::rand() + static_cast<uint32_t>(currentTimeMillis() & 0xFFFFFFFF)) % 4294967296;
    RandSeedA ^= RandSeedA << 5;
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::ResetRandSeedB() {
    RandSeedB = (std::rand() + static_cast<uint32_t>(currentTimeMillis() & 0xFFFFFFFF)) % 4294967296;
    RandSeedB ^= RandSeedA << 5;
}
//-----------------------------------------------------------------------------
uint32_t GenerateNoiseTools::Synthesis()
{

    uint32_t a;
    a = ((RandA() + RandB()) ^ (RandC() + RandD()) + distrib(gen)) % 65535;
    a ^= a << 5;
    a ^= a << 7;
    a ^= a << 9;
    a ^= distrib(gen);
    return a;
}
//----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoiseRGB_ARGB_BWG(uint8_t imgX, uint8_t imgY, int8_t Channel)
{
    std::vector<unsigned char> img(imgX * imgY * Channel);


    for (int i = img.size() - 1; i > -1;) {

        ResetRandSeedA();

        ResetRandSeedB();

        for (int j = 100; i > 0 && j > 0; j--) {
            img[i] = Synthesis() % 255;
            i--;
        }
    }


    return img;
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoise_BW_Return(uint8_t imgX, uint8_t imgY)
{
    std::vector<unsigned char> img(imgX * imgY);
    std::vector<uint32_t> BW(((imgX * imgY) / 8) + 1);

    for (int i = BW.size() - 1; i >= 0; i--) BW[i] = Synthesis();

    int v = BW.size() - 1;
    int b = 0;

    for (int i = img.size() - 1; i >= 0; i--) {

        img[i] = ((BW[v] >> b) & 1) * 255;


        b++;


        if (b >= 32) b = 0; v--; if (v < 0)v = BW.size() - 1;

    }

    return img;
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::GenerateNoise_BW_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY)
{
    std::string __Name = name  + ".PNG";
    if (!stbi_write_png((Path + __Name).c_str(), imgX, imgY, 1, GenerateNoise_BW_Return(imgX,imgY).data(), imgX)) return;
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoise_RGB_Return(uint8_t imgX, uint8_t imgY)
{
    return GenerateNoiseRGB_ARGB_BWG(imgX,imgY,3);
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::GenerateNoise_RGB_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY)
{
    std::string __Name = name + ".PNG";
    if (!stbi_write_png((Path + __Name).c_str(), imgX, imgY, 3, GenerateNoise_RGB_Return(imgX, imgY).data(), imgX * 3)) return;
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoise_ARGB_Return(uint8_t imgX, uint8_t imgY)
{
    return GenerateNoiseRGB_ARGB_BWG(imgX, imgY, 4);
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::GenerateNoise_ARGB_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY)
{
    std::string __Name = name + ".PNG";
    if (!stbi_write_png((Path + __Name).c_str(), imgX, imgY, 4, GenerateNoise_ARGB_Return(imgX, imgY).data(), imgX * 4)) return;
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoise_Island_Return(uint8_t imgX, uint8_t imgY, int8_t size, int8_t Channel, int8_t uneven, std::vector<unsigned char> IslandColor, std::vector<unsigned char> Seacolor)
{
    if (!(Channel == IslandColor.size()))return;
    if (!(Channel == Seacolor.size()))return;
    
    if (0 < Channel && 4 > Channel)return;
    if (0 < size && 11 > size)return;
    if (0 < uneven && 11 > uneven)return;

    std::vector<unsigned char> img(imgX * imgY * Channel);
    
    if (IslandColor.size() < Channel || Seacolor.size() < Channel) {
        
        if (Channel >= 3) {
            IslandColor = { 0, 255, 0 };
            Seacolor = { 0, 0, 255 };
        }
        else {
            IslandColor = { 255 };
            Seacolor = { 0 };
        }
    }

    for (int y = 0; y < imgY; y++) {
        for (int x = 0; x < imgX; x++) {
            int index = (y * imgX + x) * Channel;
            for (int c = 0; c < Channel; c++) {
                img[index + c] = Seacolor[c];
            }
        }
    }

    int centerX = imgX / 2;
    int centerY = imgY / 2;

    float baseRadius = ((std::min)(imgX, imgY) / 3.0f) * (size / 10.0f);

    int numPoints = 6 + uneven;

    std::vector<float> angles(numPoints);
    std::vector<float> radii(numPoints);

    for (int i = 0; i < numPoints; i++) {
        
        angles[i] = (Synthesis() % 360) * (3.1415926535f / 180.0f);

        float randomFactor = 0.5f + (Synthesis() % 100) / 200.0f;
        radii[i] = baseRadius * randomFactor;
    }

    std::vector<std::pair<float, float>> sortedPoints(numPoints);
    for (int i = 0; i < numPoints; i++) {
        sortedPoints[i] = std::make_pair(angles[i], radii[i]);
    }
    std::sort(sortedPoints.begin(), sortedPoints.end());

    for (int i = 0; i < numPoints; i++) {
        angles[i] = sortedPoints[i].first;
        radii[i] = sortedPoints[i].second;
    }

    std::vector<std::pair<int, int>> edgePoints(numPoints);
    for (int i = 0; i < numPoints; i++) {
        int x = centerX + static_cast<int>(radii[i] * cos(angles[i]));
        int y = centerY + static_cast<int>(radii[i] * sin(angles[i]));

        x = (std::max)(0, (std::min)(x, imgX - 1));
        y = (std::max)(0, (std::min)(y, imgY - 1));

        edgePoints[i] = std::make_pair(x, y);
    }

    std::vector<std::pair<int, int>> smoothPoints;
    for (int i = 0; i < numPoints; i++) {
        int next_i = (i + 1) % numPoints;
        int prev_i = (i - 1 + numPoints) % numPoints;

        auto p0 = edgePoints[prev_i];
        auto p1 = edgePoints[i];
        auto p2 = edgePoints[next_i];

        smoothPoints.push_back(p1);

        for (int t = 1; t <= 3; t++) {
            float tNorm = t / 4.0f;

            float omt = 1.0f - tNorm;
            float x = omt * omt * p0.first + 2 * omt * tNorm * p1.first + tNorm * tNorm * p2.first;
            float y = omt * omt * p0.second + 2 * omt * tNorm * p1.second + tNorm * tNorm * p2.second;

            smoothPoints.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
        }
    }

    struct Edge {
        int y_max;
        float x_min;
        float slope_reciprocal;
    };

    int y_min = imgY, y_max = 0;
    for (const auto& point : smoothPoints) {
        y_min = (std::min)(y_min, point.second);
        y_max = (std::max)(y_max, point.second);
    }

    std::vector<std::vector<Edge>> edgeTable(y_max - y_min + 1);

    for (size_t i = 0; i < smoothPoints.size(); i++) {
        size_t next_i = (i + 1) % smoothPoints.size();
        auto p1 = smoothPoints[i];
        auto p2 = smoothPoints[next_i];

        if (p1.second == p2.second) continue;

        Edge edge;
        if (p1.second < p2.second) {
            edge.y_max = p2.second;
            edge.x_min = static_cast<float>(p1.first);
            edge.slope_reciprocal = static_cast<float>(p2.first - p1.first) / (p2.second - p1.second);
        }
        else {
            edge.y_max = p1.second;
            edge.x_min = static_cast<float>(p2.first);
            edge.slope_reciprocal = static_cast<float>(p1.first - p2.first) / (p1.second - p2.second);
        }

        int y_start = (std::min)(p1.second, p2.second);
        if (y_start >= y_min && y_start - y_min < edgeTable.size()) {
            edgeTable[y_start - y_min].push_back(edge);
        }
    }

    std::vector<Edge> activeEdgeTable;

    for (int y = y_min; y <= y_max; y++) {

        if (y - y_min < edgeTable.size()) {
            for (const auto& edge : edgeTable[y - y_min]) {
                activeEdgeTable.push_back(edge);
            }
        }

        activeEdgeTable.erase(
            std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(),
                [y](const Edge& e) { return e.y_max == y; }),
            activeEdgeTable.end()
        );

        std::sort(activeEdgeTable.begin(), activeEdgeTable.end(),
            [](const Edge& a, const Edge& b) { return a.x_min < b.x_min; });

        for (size_t i = 0; i + 1 < activeEdgeTable.size(); i += 2) {
            int x_start = static_cast<int>(activeEdgeTable[i].x_min);
            int x_end = static_cast<int>(activeEdgeTable[i + 1].x_min);

            if (x_start > x_end) std::swap(x_start, x_end);

            for (int x = x_start; x <= x_end && x < imgX; x++) {
                if (x >= 0) {
                    int index = (y * imgX + x) * Channel;
                    for (int c = 0; c < Channel; c++) {
                        img[index + c] = IslandColor[c];
                    }
                }
            }
        }

        for (auto& edge : activeEdgeTable) {
            edge.x_min += edge.slope_reciprocal;
        }
    }

    return img;
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::GenerateNoise_Island_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY, int8_t size, int8_t Channel, int8_t uneven, std::vector<unsigned char> IslandColor, std::vector<unsigned char> Seacolor)
{
    if (!(Channel == IslandColor.size()))return;
    if (!(Channel == Seacolor.size()))return;

    if (0 < Channel && 4 > Channel)return;
    if (0 < size && 11 > size)return;
    if (0 < uneven && 11 > uneven)return;

    std::string __Name = name + ".PNG";
    if (!stbi_write_png((Path + __Name).c_str(), imgX, imgY, 4, GenerateNoise_Island_Return(imgX, imgY, size, Channel ,uneven, IslandColor, Seacolor).data(), imgX * 4)) return;
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> GenerateNoiseTools::GenerateNoise_Random_Cutom_Return(uint8_t imgX, uint8_t imgY,bool Fast, int channel, std::vector<std::vector<unsigned char>> color, std::vector<int> Probability)
{
    if (0 < channel && 4 > channel)return;
    if (!(color.size() == Probability.size()))return;

    if (!(channel == color[0].size()))return;

    auto maxnumber = std::max_element(Probability.begin(), Probability.end());
    int max_index = std::distance(Probability.begin(), maxnumber);
    Probability.erase(maxnumber);

    std::vector<std::vector<uint32_t>> Location;

    if (Fast) {
        for (int i = 0; i < Probability.size(); i++) {
            std::vector<uint32_t> ca;

            for (int j = 0; j < Probability[i]; j++) {
                uint32_t a;
                do {
                    a = Synthesis() % (imgX * imgY);
                } while (
                    std::find(ca.begin(), ca.end(), a) != ca.end() ||
                    std::any_of(Location.begin(), Location.end(),
                        [a](const std::vector<uint32_t>& subVec) {
                            return std::find(subVec.begin(), subVec.end(), a) != subVec.end();
                        })
                    );
                ca.push_back(a);
            }
            Location.push_back(ca);
        }
    }
    else {
        for (int i = 0; i < Probability.size(); i++) {
            std::vector<uint32_t> ca;
            for (int j = 0; j < Probability[i]; j++) {
                ca.push_back(Synthesis() % (imgX * imgY));
            }
            Location.push_back(ca);
        }
    }

    std::vector<unsigned char> img(imgX * imgY * channel);
    std::vector<unsigned char>& max_color = color[max_index];

    for (int i = 0; i < imgX * imgY; i++) {
        for (int k = 0; k < channel; k++) {
            img[i * channel + k] = max_color[k];
        }
    }

    color.erase(color.begin() + max_index);

    for (int i = 0; i < Location.size(); i++) {
        for (int j = 0; j < Location[i].size(); j++) {
            uint32_t pos = Location[i][j];
            for (int k = 0; k < channel; k++) {
                img[pos * channel + k] = color[i][k];
            }
        }
    }
}
//-----------------------------------------------------------------------------
void GenerateNoiseTools::GenerateNoise_Random_Cutom_Save(std::string name, std::string Path, uint8_t imgX, uint8_t imgY, bool Fast, int channel, std::vector<std::vector<unsigned char>> color, std::vector<int> Probability)
{
    if (0 < channel && 4 > channel)return;
    if (!(color.size() == Probability.size()))return;

    if (!(channel == color[0].size()))return;

    std::string __Name = name + ".PNG";
    if (!stbi_write_png((Path + __Name).c_str(), imgX, imgY, 4, GenerateNoise_Random_Cutom_Return(imgX, imgY, Fast, channel, color, Probability).data(), imgX * 4)) return;
}

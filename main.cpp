// GenerateNoise.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
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
std::uniform_int_distribution<> distrib(1,4294967295);

static uint32_t RandSeedA = 1;
static uint32_t RandSeedB = 1;

int32_t X;
int32_t Y;
std::string Path;
std::string* Pathptr = &Path;

struct imgarr {
    std::vector <unsigned char> a;

};

struct BWarr {
    
    unsigned char Black;
};

struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

struct ARGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
};
//-----------------------------------------------------------------------------
// 扩展版：支持ARGB
void showColor(int r, int g, int b, int a = 255) {
    if (a == 255) {
        // 不透明：直接显示
        std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
    }
    else if (a == 0) {
        // 透明：显示棋盘格
        std::cout << "\033[48;2;180;180;180m"; // 浅灰
    }
    else {
        // 部分透明：混合到灰色背景
        float alpha = a / 255.0f;
        int bgR = 200, bgG = 200, bgB = 200;
        int blendedR = r * alpha + bgR * (1 - alpha);
        int blendedG = g * alpha + bgG * (1 - alpha);
        int blendedB = b * alpha + bgB * (1 - alpha);

        std::cout << "\033[48;2;" << blendedR << ";"
            << blendedG << ";" << blendedB << "m";
    }

    std::cout << "  ";  // 颜色块
    std::cout << "\033[0m";

    // 可选：显示Alpha值
    if (a != 255) {
        std::cout << " (Alpha=" << a << ")";
    }
    std::cout << std::endl;
}
//-----------------------------------------------------------------------------
void forPrint(std::vector<std::string>string,std::string Version) {
    
    std::cout << "\n" << Version << "\n";
    
    for (int i = 0; i < string.size();i++) {
        std::cout << std::to_string(i + 1)<< "." << string[i] << "\n";
    }
    std::cout  << "Enter number" << "\n";
}
//-----------------------------------------------------------------------------
// 设置控制台文本颜色
void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
enum ConsoleColors {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7,
    GRAY = 8,
    BRIGHT_BLUE = 9,
    BRIGHT_GREEN = 10,
    BRIGHT_CYAN = 11,
    BRIGHT_RED = 12,
    BRIGHT_MAGENTA = 13,
    BRIGHT_YELLOW = 14,
    BRIGHT_WHITE = 15
};
//-----------------------------------------------------------------------------

void DeletePreviousLine() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    // 获取当前控制台屏幕缓冲区信息
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return; // 获取失败，直接返回
    }

    // 如果光标不在第一行（行号从0开始）
    if (csbi.dwCursorPosition.Y > 0) {
        COORD targetPos = { 0, csbi.dwCursorPosition.Y - 1 }; // 上一行行首

        // 移动到上一行行首
        SetConsoleCursorPosition(hConsole, targetPos);

        // 获取控制台宽度
        int consoleWidth = csbi.dwSize.X;

        // 用空格覆盖整行
        DWORD charsWritten;
        FillConsoleOutputCharacter(
            hConsole,           // 控制台句柄
            ' ',                // 填充字符（空格）
            consoleWidth,       // 填充长度（整行宽度）
            targetPos,          // 起始位置
            &charsWritten       // 实际填充的字符数
        );

        // 恢复原来的属性（颜色等）
        FillConsoleOutputAttribute(
            hConsole,                  // 控制台句柄
            csbi.wAttributes,          // 原来的属性
            consoleWidth,              // 填充长度
            targetPos,                 // 起始位置
            &charsWritten              // 实际填充的字符数
        );

        // 光标留在上一行行首（如果需要回到原来位置，可以取消下面这行）
        // SetConsoleCursorPosition(hConsole, csbi.dwCursorPosition);
    }
}
//-----------------------------------------------------------------------------
//进度条0-100,time是多少秒刷新
void ProgressBar(int a) {
    if (a < 30)SetColor(BRIGHT_RED);
    else if (a < 60)SetColor(BRIGHT_YELLOW);
    else SetColor(BRIGHT_GREEN);
    a /= 5;
    std::string Bar;
    Bar.append("[");
    Bar.append(a, '#');
    Bar.append(20 - a, ' ');
    Bar.append("]");
    Bar.append(std::to_string(a));
    Bar.append("%");
    std::cout << Bar <<"\n";
    SetColor(WHITE);
}
//-----------------------------------------------------------------------------
long long currentTimeMillis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
//-----------------------------------------------------------------------------

uint16_t RandA() {
    RandSeedA = (RandSeedA * 9301 + 49297) % 233280;
    return static_cast<uint16_t>(RandSeedA % 65535);
}
//-----------------
void ResetRandSeed(){
    RandSeedA = (std::rand() + static_cast<uint32_t>(currentTimeMillis() & 0xFFFFFFFF)) % 4294967296;
    RandSeedA ^= RandSeedA << 5;
}
//-----------------
void ResetRandSeedB() {
    RandSeedB = (std::rand() + static_cast<uint32_t>(currentTimeMillis() & 0xFFFFFFFF)) % 4294967296;
    RandSeedB ^= RandSeedA << 5;
}
//-----------------
void ResetRandSeedTime() {
    for (;;) {
        Sleep(RandA() % 10000);
        ResetRandSeed();
        ResetRandSeedB();
    }
}
//-----------------
uint16_t RandB() {
    RandSeedB = (RandSeedB * 1664525 + 1013904223) % 4294967296;
    return static_cast<uint16_t>(RandSeedA % 65535);
}
//-----------------
uint16_t RandC() {
    return static_cast<uint16_t>(((RandSeedB * RandSeedA) ^ (RandSeedB + RandSeedA)) % 65535);
}
//-----------------
uint16_t RandD() {
    return static_cast<uint16_t>((RandSeedB + RandSeedA + RandC()) ^ (RandSeedB + RandSeedA) + 1);
}
//-----------------
uint32_t Synthesis()
{
    
    uint32_t a;
    a = ((RandA() + RandB()) ^ (RandC() + RandD()) + distrib(gen)) % 65535;
    a ^= a << 5;
    a ^= a << 7;
    a ^= a << 9;
    a ^= distrib(gen);
    return a;
}

//-----------------------------------------------------------------------------
bool canWriteToPath(const std::string& targetDir) {
    // 1. 构建用于测试的临时文件完整路径。
    // 使用双反斜杠`\\`是Windows路径中的转义，也可使用正斜杠`/`。
std::string testFilePath = targetDir + "\\__write_test.tmp";

// 2. 核心：尝试创建（并打开）测试文件。
HANDLE hFile = CreateFileA(
    testFilePath.c_str(),   // 要创建/打开的文件路径
    GENERIC_WRITE,          // 请求“写入”访问权限
    0,                      // 不共享（其他进程无法同时访问此文件）
    NULL,                   // 默认安全描述符
    CREATE_ALWAYS,          // 总是创建新文件；如果已存在则覆盖
    FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, // 文件属性：临时文件，关闭后自动删除
    NULL                    // 没有模板文件
);

// 3. 判断创建是否成功。
// INVALID_HANDLE_VALUE 是Windows API中表示无效句柄的常量，通常为-1。
// 如果 hFile 不等于这个值，说明文件创建/打开成功，即我们有写入权限。
bool hasPermission = (hFile != INVALID_HANDLE_VALUE);

// 4. 清理资源。
if (hasPermission) {
    // 如果文件创建成功，立即关闭句柄。
    // 由于指定了 FILE_FLAG_DELETE_ON_CLOSE，关闭句柄时系统会自动删除该临时文件。
    CloseHandle(hFile);
    // 理论上，上面关闭后文件已被系统删除。这里再调用一次DeleteFileA是双保险。
    DeleteFileA(testFilePath.c_str());
}
// 如果创建失败（hasPermission为false），则 hFile 是无效句柄，无需也无法关闭。

// 5. 返回检查结果。
return hasPermission;
}
//-----------------------------------------------------------------------------
bool isNumberWithFromChars(const std::string& str) {
    if (str.empty()) return false;

    double value = 0.0;
    // std::from_chars 转换
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

    // 判断：1. 没有错误发生 (ec == std::errc()) 
    //       2. 并且转换消耗了整个字符串 (ptr == str.data() + str.size())
    // 注意：from_chars 不会跳过前导空格
    return (ec == std::errc() && ptr == (str.data() + str.size()));
}
//-------------------------------------------------------------------------
bool FileExists(const char* path, const char* filename) {
    char fullPath[MAX_PATH];
    // 组合路径和文件名
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", path, filename);

    // 获取文件属性
    DWORD attrib = GetFileAttributesA(fullPath);

    // 文件存在且不是目录
    return (attrib != INVALID_FILE_ATTRIBUTES &&
        !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------

void SetXY (){
    std::string Input;
    for (;;) {
        std::cout << "Enter image Width\n";
        //输入X
        SetColor(BRIGHT_CYAN);
        std::cin >> Input;
        SetColor(WHITE);
        //判断是否按E
        if (Input == "E") std::exit(0);
        
        if (!isNumberWithFromChars(Input)) {
            SetColor(BRIGHT_RED);
            std::cout << "(X)Exceed !!!Please Enter a number\n";
            SetColor(WHITE);
        }
        else {
            X = std::stoi(Input)% 2147483647;
            //判断是否比2大
            if (X < 2) {
                SetColor(BRIGHT_RED);
                std::cout << "(X)Exceed !!!Enter a number greater than 2\n";
                SetColor(WHITE);
            }
            else {
                //判断是否比32768大
                if (X < 32769) {
                    break;
                }
                else {
                    SetColor(BRIGHT_RED);
                    std::cout << "(X)Exceed !!!Enter a number less than 32768\n";
                    SetColor(WHITE);
                }
            }
        }
    }

    //------------------
    for (;;) {
        std::cout << "Enter image length\n";
        //输入Y
        SetColor(BRIGHT_CYAN);
        std::cin >> Input;
        SetColor(WHITE);
        //判断是否按E
        if (Input == "E") std::exit(0);
        
        if (!isNumberWithFromChars(Input)) {
            SetColor(BRIGHT_RED);
            std::cout << "(X)Exceed !!!Please Enter a number\n";
            SetColor(WHITE);
        }
        else {
            Y = std::stoi(Input) % 2147483647;
            //判断是否比2大
            if (Y < 2) {
                SetColor(BRIGHT_RED);
                std::cout << "(X)Exceed !!!Enter a number greater than 2\n";
                SetColor(WHITE);
            }
            else {
                //判断是否比32768大
                if (Y < 32769) {
                    break;
                }
                else {
                    SetColor(BRIGHT_RED);
                    std::cout << "(X)Exceed !!!Enter a number less than 32768\n";
                    SetColor(WHITE);
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
void RGNTemplate (std::function<void(int,std::string)> RGNvoid,std::string name) {
    //输入次数
    std::cout << "Enter times(How many times)\n";
    int time;
    SetColor(BRIGHT_CYAN);
    std::cin >> time;
    SetColor(WHITE);
    
    

    std::vector<std::thread> threads;

    int nameint = 1;
    //创建名字
    std::string str = name + std::to_string(nameint);
    //改名字
    for (; FileExists((*Pathptr).c_str(), str.c_str()); nameint++) str = name + std::to_string(nameint);
    //创建线程
    {
        int timet = time + nameint;
        for (int i = nameint; timet > i; i++) {
            threads.push_back(std::thread(RGNvoid,i,name));
            Sleep(10);
        }
    }
    for (int i = 0; i < time; i++) {
        ProgressBar((i * 100) / time);
        threads[i].join();
        DeletePreviousLine();
    }

    std::cout << "OK\n";
}
//-----------------------------------------------------------------------------
void RGPNGTemplate(int a,int comp,int times,int Reset,std::string name){
    std::vector<unsigned char> img(X * Y * comp);
    //性能过慢
    /*int time = 0;
    time++;
    for (int i = X * Y * comp - 1; i > -1; i--) {
        img[i] = Synthesis() % 255;
        
        if (time == times) {
            time = 0;
            ResetRandSeed();
        }
    }
    */
    
    for (int i = X * Y * comp - 1;i > 0;){
        
        ResetRandSeed();
        for (int j = times;i > 0 && j > 0;j--) {
            i--;
            img[i] = Synthesis() % 255;
        }
    }

    {
        int xy = X * Y;
        xy--;
        for (int i = Reset; i > 0; i--) img[RandC() % xy] = Synthesis() % 255;
    }
    ResetRandSeed();
    //设置名字
    name += std::to_string(a) + ".PNG";
    //for (int i = 1; FileExists(Path.c_str(), Name.c_str() ); i++) Name = "Noise" + std::to_string(i) + ".PNG";  
    if (!stbi_write_png((*Pathptr + name).c_str(), X, Y, comp, img.data(),X * comp)) std::exit(1);

}
//-----------------------------------------------------------------------------
void RGPNG(int a,std::string name) {
    RGPNGTemplate(a, 1, 100,X, name);
}

//-----------------------------------------------------------------------------
void RGN() {
    RGNTemplate(RGPNG, "NoiseBWG");
}
//-----------------------------------------------------------------------------
std::string convertToWinPath(const std::string& path) {
    std::string result;
    for (char c : path) {
        if (c == '/' || c == '\\') {
            result += "\\\\"; // 转换为双反斜杠
        }
        else {
            result += c; // 其他字符原样保留
        }
    }
    return result;
}
//-----------------------------------------------------------------------------
void SetPath() {
    for (;;) {
        std::cout << "Enter Save Path.\n";

        //输入路径
        SetColor(BRIGHT_YELLOW);
        std::cin >> *Pathptr;
        SetColor(WHITE);
        std::string A = *Pathptr;
        A = A.substr(0, 3);
        //判断是否按E
        if (A == "E") {
            std::exit(0);
        }
        //判断是否有这个盘
        if (std::filesystem::exists(A)) {
            //改字符
            if (!(char((*Pathptr).back()) == char("/") ||char((*Pathptr).back()) == char("\\") || char((*Pathptr).back()) == char("\\\\")))*Pathptr +="/";
            *Pathptr = convertToWinPath(Path);
            std::cout << "\n" << *Pathptr << "\n";
            if (! std::filesystem::create_directories(*Pathptr)) std::cout << "OK\n"; 
            //if (!canWriteToPath(Path)) {

            //}
            break;
        }
        SetColor(BRIGHT_RED);
        std::cout << "(X)Exceed !!! No such Drive " << A.substr(0, 1) << "\n";
        SetColor(WHITE);
    }

}

//-----------------------------------------------------------------------------
void RGPNGBW(int a,std::string name) {
    std::vector<unsigned char> img(X * Y);
    std::vector<uint32_t> BW(((X * Y) / 8) + 1);

    // 生成随机位
    for (int i = BW.size() - 1; i >= 0; i--) BW[i] = Synthesis();
    

    // 初始化v和b（v指向最后一个BW元素，b从0开始）
    int v = BW.size() - 1;
    int b = 0;

    // 用随机位填充图像
    for (int i = img.size() - 1; i >= 0; i--) {
        // 获取当前位（0或1），乘以255得到0或255
        img[i] = ((BW[v] >> b) & 1) * 255;

        // 移动到下一个位
        b++;

        // 如果b达到32位，重置b并移动到前一个BW元素
        if (b >= 32) b = 0; v--; if (v < 0)v = BW.size() - 1;
           
    }
    std::string Name = name + std::to_string(a) + ".PNG";
    if (!stbi_write_png((Path + Name).c_str(), X, Y, 1, img.data(), X)) std::exit(1);
    ResetRandSeed();
}

//-----------------------------------------------------------------------------
void RGNBW() {
    RGNTemplate(RGPNGBW , "NoiseBW");
}
//-----------------------------------------------------------------------------
void RGPNGRGB (int a,std::string name){
    RGPNGTemplate(a, 3, 100, X + Y, name);
}
//-----------------------------------------------------------------------------
void RGNRGB(){
    RGNTemplate(RGPNGRGB, "NoiseRGB");
}
//-----------------------------------------------------------------------------
void RGPNGARGB(int a,std::string name) {
    RGPNGTemplate(a, 4, 100, X + Y, name);
}
//-----------------------------------------------------------------------------
void RGNARGB() {
    RGNTemplate(RGPNGARGB,  "NoiseARGB");
}
//-----------------------------------------------------------------------------
std::vector<unsigned char> Island(int X, int Y, int size, int Channel,
    int uneven, std::vector<unsigned char> Lcolor,
    std::vector<unsigned char> Scolor) {
    // 创建图像数据，初始化为海洋颜色
    std::vector<unsigned char> img(X * Y * Channel);

    // 确保颜色向量大小匹配通道数
    if (Lcolor.size() < Channel || Scolor.size() < Channel) {
        // 默认颜色：岛屿为绿色，海洋为蓝色
        if (Channel >= 3) {
            Lcolor = { 0, 255, 0 };  // 绿色
            Scolor = { 0, 0, 255 };  // 蓝色
        }
        else {
            Lcolor = { 255 };  // 灰度
            Scolor = { 0 };
        }
    }

    // 首先用海洋颜色填充整个图像
    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            int index = (y * X + x) * Channel;
            for (int c = 0; c < Channel; c++) {
                img[index + c] = Scolor[c];
            }
        }
    }

    // 中心点坐标
    int centerX = X / 2;
    int centerY = Y / 2;

    // 基础岛屿半径，受size参数影响 (1-10)
    float baseRadius = ((std::min)(X, Y) / 3.0f) * (size / 10.0f);

    // 生成uneven个角度点 (1-10)
    int numPoints = 6 + uneven;  // 至少6个点，加上uneven值

    // 生成随机角度和半径
    std::vector<float> angles(numPoints);
    std::vector<float> radii(numPoints);

    for (int i = 0; i < numPoints; i++) {
        // 使用Synthesis()生成随机角度 (0 到 2π)
        angles[i] = (Synthesis() % 360) * (3.1415926535f / 180.0f);

        // 随机半径，在基础半径附近波动
        float randomFactor = 0.5f + (Synthesis() % 100) / 200.0f;  // 0.5 到 1.0
        radii[i] = baseRadius * randomFactor;
    }

    // 按角度排序点，确保连接顺序正确
    std::vector<std::pair<float, float>> sortedPoints(numPoints);
    for (int i = 0; i < numPoints; i++) {
        sortedPoints[i] = std::make_pair(angles[i], radii[i]);
    }
    std::sort(sortedPoints.begin(), sortedPoints.end());

    // 重新提取排序后的角度和半径
    for (int i = 0; i < numPoints; i++) {
        angles[i] = sortedPoints[i].first;
        radii[i] = sortedPoints[i].second;
    }

    // 生成岛屿边缘点坐标
    std::vector<std::pair<int, int>> edgePoints(numPoints);
    for (int i = 0; i < numPoints; i++) {
        int x = centerX + static_cast<int>(radii[i] * cos(angles[i]));
        int y = centerY + static_cast<int>(radii[i] * sin(angles[i]));

        // 确保坐标在图像范围内
        x = (std::max)(0, (std::min)(x, X - 1));
        y = (std::max)(0, (std::min)(y, Y - 1));

        edgePoints[i] = std::make_pair(x, y);
    }

    // 添加平滑的中间点（使用贝塞尔曲线插值）
    std::vector<std::pair<int, int>> smoothPoints;
    for (int i = 0; i < numPoints; i++) {
        int next_i = (i + 1) % numPoints;
        int prev_i = (i - 1 + numPoints) % numPoints;

        // 当前点、前一个点和后一个点用于平滑
        auto p0 = edgePoints[prev_i];
        auto p1 = edgePoints[i];
        auto p2 = edgePoints[next_i];

        // 添加当前点
        smoothPoints.push_back(p1);

        // 生成两个中间点进行平滑（使用二次贝塞尔曲线）
        for (int t = 1; t <= 3; t++) {
            float tNorm = t / 4.0f;

            // 二次贝塞尔曲线公式：B(t) = (1-t)²P0 + 2(1-t)tP1 + t²P2
            float omt = 1.0f - tNorm;
            float x = omt * omt * p0.first + 2 * omt * tNorm * p1.first + tNorm * tNorm * p2.first;
            float y = omt * omt * p0.second + 2 * omt * tNorm * p1.second + tNorm * tNorm * p2.second;

            smoothPoints.push_back(std::make_pair(static_cast<int>(x), static_cast<int>(y)));
        }
    }

    // 使用扫描线算法填充岛屿
    // 首先构建边表(ET)和活动边表(AET)
    struct Edge {
        int y_max;   // 边的最大y值
        float x_min; // 当前x最小值（随着扫描线更新）
        float slope_reciprocal; // 1/斜率（dx/dy）
    };

    // 找到y的最小值和最大值
    int y_min = Y, y_max = 0;
    for (const auto& point : smoothPoints) {
        y_min = (std::min)(y_min, point.second);
        y_max = (std::max)(y_max, point.second);
    }

    // 创建边表
    std::vector<std::vector<Edge>> edgeTable(y_max - y_min + 1);

    // 填充边表
    for (size_t i = 0; i < smoothPoints.size(); i++) {
        size_t next_i = (i + 1) % smoothPoints.size();
        auto p1 = smoothPoints[i];
        auto p2 = smoothPoints[next_i];

        // 忽略水平线
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

        // 添加到边表中对应的y位置
        int y_start = (std::min)(p1.second, p2.second);
        if (y_start >= y_min && y_start - y_min < edgeTable.size()) {
            edgeTable[y_start - y_min].push_back(edge);
        }
    }

    // 活动边表
    std::vector<Edge> activeEdgeTable;

    // 扫描线填充
    for (int y = y_min; y <= y_max; y++) {
        // 将边表中y扫描线的边添加到活动边表
        if (y - y_min < edgeTable.size()) {
            for (const auto& edge : edgeTable[y - y_min]) {
                activeEdgeTable.push_back(edge);
            }
        }

        // 从活动边表中移除y_max == y的边
        activeEdgeTable.erase(
            std::remove_if(activeEdgeTable.begin(), activeEdgeTable.end(),
                [y](const Edge& e) { return e.y_max == y; }),
            activeEdgeTable.end()
        );

        // 对活动边表按x_min排序
        std::sort(activeEdgeTable.begin(), activeEdgeTable.end(),
            [](const Edge& a, const Edge& b) { return a.x_min < b.x_min; });

        // 填充扫描线
        for (size_t i = 0; i + 1 < activeEdgeTable.size(); i += 2) {
            int x_start = static_cast<int>(activeEdgeTable[i].x_min);
            int x_end = static_cast<int>(activeEdgeTable[i + 1].x_min);

            // 确保x_start <= x_end
            if (x_start > x_end) std::swap(x_start, x_end);

            // 填充从x_start到x_end的像素
            for (int x = x_start; x <= x_end && x < X; x++) {
                if (x >= 0) {
                    int index = (y * X + x) * Channel;
                    for (int c = 0; c < Channel; c++) {
                        img[index + c] = Lcolor[c];
                    }
                }
            }
        }

        // 更新活动边表中的x_min值（下一条扫描线）
        for (auto& edge : activeEdgeTable) {
            edge.x_min += edge.slope_reciprocal;
        }
    }

    return img;
}
//旧版BUG太多了
//-----------------------------------------------------------------------------
/*用改良版了
void GCloud(int Cloudtimes, unsigned char Cloudcolor[6], int CLoudSize, int nameNumber, std::string name) {
    std::vector<unsigned char> img(X * Y * 6);
    for (int i = img.size() - 1; i > -1;) {
        for (int j = 5; j > 2 && i > -1; j--) {
            img[i] = Cloudcolor[j];
            i--;
        }
    }
    std::vector <imgarr> imgsub;
    
   uint32_t cx = X / (Cloudtimes + 3);
   uint32_t cy = Y / (Cloudtimes + 3);


        //创建云朵

        
   for (int i = Cloudtimes - 1; i > -1; i--) {
        imgsub.push_back({ Island(cx, cy, CLoudSize, 6, 3, { Cloudcolor[0], Cloudcolor[1], Cloudcolor[2] }, { Cloudcolor[3], Cloudcolor[4], Cloudcolor[5] }) });
   }
    
    //坐标
    //添加小岛
    for (int i = Cloudtimes; i > 0; i--){
        uint32_t ccx = Synthesis() % (X - cx -1);
        uint32_t ccy = Synthesis() % (Y - cy -1);
               //cx是横 —
               //cy是竖 |
        for (int i = cy;i > 0;i--) {
            
            ccy++;
            
            for (int j = cx; j > 0; j--) {

                ccx++;
                //int k = ccx * 3 - 1; k > ccx * 3 -3 -1; k--
                for (int k = -1; k < 2;k++) {
                    img[(ccy * Y) + (ccx * 3) + k] = imgsub[i].a[(ccx * 3 )+ k];

                }
            }
        }
    }
    if (!stbi_write_png((Path + name).c_str(), X, Y, 3, img.data(), X * 3)) std::exit(1);
}
*/
void GCloud(int Cloudtimes, unsigned char Cloudcolor[6], int CLoudSize,
    int nameNumber, std::string name) {
    int channels = 3;  // 明确使用3通道（RGB）

    // 1. 创建背景图像（3通道）
    std::vector<unsigned char> img(X * Y * channels);

    // 用Cloudcolor[3-5]填充背景（假设这是背景色）
    for (int i = 0; i < X * Y * channels; i += channels) {
        img[i] = Cloudcolor[3];  // R
        img[i + 1] = Cloudcolor[4];  // G
        img[i + 2] = Cloudcolor[5];  // B
    }

    // 2. 创建云朵子图像集合
    std::vector<imgarr> imgsub;

    // 云朵大小
    uint32_t cloudWidth = X / (Cloudtimes + 1);
    uint32_t cloudHeight = Y / (Cloudtimes + 1);

    // 3. 生成Cloudtimes个云朵图像
    for (int i = 0; i < Cloudtimes; i++) {
        // 创建单个云朵图像（3通道）
        std::vector<unsigned char> cloud = Island(
            cloudWidth, cloudHeight, CLoudSize, channels, 3,
            { Cloudcolor[0], Cloudcolor[1], Cloudcolor[2] },  // 云朵颜色
            { Cloudcolor[3], Cloudcolor[4], Cloudcolor[5] }   // 透明/背景色
        );

        imgarr item;
        item.a = std::move(cloud);  // 转移所有权，避免拷贝
        imgsub.push_back(std::move(item));
    }

    // 4. 将云朵添加到主图像中
    for (int cloudIdx = 0; cloudIdx < Cloudtimes; cloudIdx++) {
        // 随机位置（确保在图像范围内）
        uint32_t posX = Synthesis() % (X - cloudWidth);
        uint32_t posY = Synthesis() % (Y - cloudHeight);

        // 复制云朵像素到主图像
        for (int y = 0; y < cloudHeight; y++) {
            for (int x = 0; x < cloudWidth; x++) {
                // 计算源像素索引（在云朵图像中）
                int srcIdx = (y * cloudWidth + x) * channels;

                // 计算目标像素索引（在主图像中）
                int dstY = posY + y;
                int dstX = posX + x;
                int dstIdx = (dstY * X + dstX) * channels;

                // 只复制非背景色的像素（实现透明效果）
                // 假设背景色是Cloudcolor[3-5]
                bool isBackground = true;
                for (int c = 0; c < channels; c++) {
                    if (imgsub[cloudIdx].a[srcIdx + c] != Cloudcolor[3 + c]) {
                        isBackground = false;
                        break;
                    }
                }

                // 如果不是背景色，复制到主图像
                if (!isBackground) {
                    for (int c = 0; c < channels; c++) {
                        img[dstIdx + c] = imgsub[cloudIdx].a[srcIdx + c];
                    }
                }
            }
        }
    }

    // 5. 保存图像
    std::string filename = *Pathptr + name + std::to_string(nameNumber) +".png";
    if (!stbi_write_png(filename.c_str(), X, Y, channels, img.data(), X * channels)) std::exit(1);
    
}
//-----------------------------------------------------------------------------
void Cloud(){
   
    
    int Cloudtimes;
    //012元素是云，345元素是背景,0=R,1=G,2=B,3=R,4=G,5=B
    unsigned char Cloudcolor[6] = {0,0,0,255,255,255};
    int CLoudSize = 5;
    

    //判断XY是否小于1024
    for (;;) {
        if (!(X < 1024 || Y < 1024))break;
        else if (X < 1024 || Y < 1024) {
            SetColor(BRIGHT_RED);
            std::cout << "(X)The values for length and height are too small(Minimum 1024)\n";
            SetColor(WHITE);
            std::cout << "1.Set size\n"
                << "2.Return\n"
                << "Enter number\n";
            SetColor(BRIGHT_CYAN);
            std::string Input;
            std::cin >> Input;
            SetColor(WHITE);
            if (Input == "1") {
                SetXY();
                Cloud();
            }
            else if (Input == "2") {
                return;
            }
            else SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
        }
    }
    for (;;) {

        forPrint({
            "Set Color",
            "Set Size",
            "Set Coluds times(How many times)",
            "Return",
            "Generate image"},
         "Set Properties(If not set, the default value will be used)");
        
        /*std::cout << "Set Properties(If not set, the default value will be used)\n"
        << "1.Set Color\n"
        << "2.Set Size \n"
        << "3.Set Coluds times(How many times)\n"
        << "4.Return\n"
        <<"5.Generate image\n"
        << "Enter number\n";*/
    SetColor(BRIGHT_CYAN);
    std::string InputB;
    std::cin >> InputB;
    SetColor(WHITE);
    if (InputB == "4")return;
   //按1逻辑 
    else if (InputB == "1") {
        for (;;) {
            forPrint({
                "Black and white(Coluds Black)",
                "Black and white(Colud White)",
                "Custom Colud color",
                "Return"}, " ");

            /*std::cout << "1.Black and white(Coluds Black)\n"
                << "2.Black and white(Colud White)\n"
                << "3.Custom Colud color\n"
                << "4.Return\n"
                "Enter number\n";
                */
            InputB = "0";
            SetColor(BRIGHT_CYAN);
            std::cin >> InputB;
            SetColor(WHITE);
            if (InputB == "1") {
                InputB = "0";
                Cloudcolor[0] = 0; Cloudcolor[1] = 0; Cloudcolor[2] = 0;
                Cloudcolor[3] = 255; Cloudcolor[4] = 255; Cloudcolor[5] = 255;
                break;
            }

            else if (InputB == "2") {
                InputB = "0";
                Cloudcolor[0] = 255; Cloudcolor[1] = 255; Cloudcolor[2] = 255;
                Cloudcolor[3] = 0; Cloudcolor[4] = 0; Cloudcolor[5] = 0;
                break;
            }

            else if (InputB == "3") {
                InputB = "0";
                int aa = 0;
                std::cout << "Colud color R(Red depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > -1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[0] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                
                std::cout << "Colud color G(Green depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > -1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[1] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                std::cout << "Colud color B(Blue depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > -1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[2] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                //____________________________________________________

                std::cout << "Background color R(Red depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > -1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[3] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                std::cout << "Background color G(Green depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > -1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[4] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                std::cout << "Background color B(Blue depth) (0 - 255)\n";
                SetColor(BRIGHT_CYAN);
                for (;;) {
                    std::cin >> aa;
                    if (!(aa > 1 && aa < 256)) {
                        SetColor(BRIGHT_RED);
                        std::cout << "Input value out of range\n";
                        SetColor(BRIGHT_CYAN);
                    }
                    else { break; }
                }
                Cloudcolor[5] = aa;
                SetColor(WHITE);
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                break;
            }
            else if (!(InputB == "4")) { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
            else { break; }
            }
    }
    else if (InputB =="2") {        //按2逻辑
        for (;;) {

            std::cout << "10 levels, the higher the level, the larger the size\n"
                << "Enter 0 to Return\n"
                << "Enter level\n";

            SetColor(BRIGHT_CYAN);
            std::cin >> CLoudSize;
            SetColor(WHITE);
            if (InputB == "0")return;
            else if (!(CLoudSize > 0 && CLoudSize > 10)) { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
            else break;
            }
        }
    else if (InputB == "3") {       //按3逻辑
        for (;;) {
            std::cout << "Enter number(1-10)\n";
            SetColor(BRIGHT_CYAN);
            std::cin >> Cloudtimes;
            SetColor(WHITE);
            if (Cloudtimes > 10 || Cloudtimes < 1) { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
            else break;
            }
        }
    else if (InputB == "5"){        //按5逻辑//生成逻辑
        std::cout << "Enter times(How many times)\n";
        int time;
        SetColor(BRIGHT_CYAN);
        std::cin >> time;
        SetColor(WHITE);
        
        std::string name;
        
        std::cout << "Img name\n";
        SetColor(BRIGHT_YELLOW);
        std::cin >> name;
        SetColor(WHITE);
        std::vector<std::thread> threads;

        int nameint = 1;
        //创建名字
        std::string str = name + std::to_string(nameint) +".png";
        //改名字
        for (; FileExists((*Pathptr).c_str(), str.c_str()); nameint++) str = name + std::to_string(nameint);
        //创建线程
        {
            int timet = time + nameint;
            for (int i = nameint; timet > i; i++) {
                threads.push_back(std::thread(GCloud,Cloudtimes,Cloudcolor,CLoudSize,i,name));
                Sleep(100);
            }
        }
        for (int i = 0; i < time; i++) {
            ProgressBar((i * 100) / time);
            threads[i].join();
            DeletePreviousLine();
        }

        std::cout << "OK\n";
    
        }
    }

}
//-----------------------------------------------------------------------------
void GCenter(std::string name ,int numberName,int channels,int size,int uneven,std::vector<unsigned char>LUcolor, std::vector<unsigned char>Scolor) {
    if (!stbi_write_png(((*Pathptr) + name + std::to_string(numberName)+ ".png").c_str(), X, Y, channels, Island(X, Y, size, channels, uneven, LUcolor, Scolor).data(), X * channels)) std::exit(1);
}
//-----------------------------------------------------------------------------
void Center() {
    
   
    int channels;
    channels = 1;
    int size = 5;
    int uneven = 5;
    std::vector<unsigned char>LUcolor(1);
    LUcolor[0] = 0;
    std::vector<unsigned char>Scolor(1);
    Scolor[0] = 255;

    for (;;) {
    
        if (!(X < 1024 || Y < 1024))break;
        else if (X < 1024 || Y < 1024) {
            SetColor(BRIGHT_RED);
            std::cout << "(X)The values for length and height are too small(Minimum 1024)\n";
            SetColor(WHITE);

            forPrint({"Set size",
                "Return"}, " ");

            /*std::cout << "1.Set size\n"
                << "2.Return\n"
                << "Enter number\n";
                */
            SetColor(BRIGHT_CYAN);
            std::string Input;
            std::cin >> Input;
            SetColor(WHITE);
            if (Input == "1") {
                SetXY();
                Cloud();
            }
            else if (Input == "2") {
                return;
            }
            else SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
        }
    }


    for (;;) {

        forPrint({"Set Color",
            "Set Size",
            "Set uneven",
            "Return",
            "Generate image"}, "Set Properties(If not set, the default value will be used)");
        
        /*std::cout << "Set Properties(If not set, the default value will be used)\n"
            << "1.Set Color\n"
            << "2.Set Size \n"
            << "3.Set uneven\n"
            << "4.Return\n"
            << "5.Generate image\n"
            << "Enter number\n";
            */
        SetColor(BRIGHT_CYAN);
        std::string InputB;
        std::cin >> InputB;
        SetColor(WHITE);
        if(InputB == "1") {
            int aa = 0;
            for (;;) {
                InputB = "0";
                
                forPrint({"Single channel(black and white)",
                    "Three-channel(RGB)",
                    "Four-channel(ARGB)"}, "Color Channel");
                
                /*std::cout << "Color Channel\n"
                    << "1.Single channel(black and white)\n"
                    << "2.Three-channel(RGB)\n"
                    << "3.Four-channel(ARGB)\n"
                    << "Enter number\n";
                    */
                std::cin >> InputB;

                if (InputB == "1") {
                    InputB = "0";
                    channels = 1;
                    std::cout << "Island color(0-255)(Black Depth)\n";
                    LUcolor.clear();
                    LUcolor.resize(1);
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Background color(0-255)(Black Depth)\n";
                    Scolor.clear();
                    Scolor.resize(1);
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                    break;
                }
                else if (InputB == "2") {
                    InputB = "0";
                    LUcolor.clear();
                    LUcolor.resize(3);

                    Scolor.clear();
                    Scolor.resize(3);

                    channels = 3;
                    
                    std::cout << "Island color R(Red depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Island color G(Green depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[1] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Island color B(BLUE depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[2] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    //_______________________________

                    std::cout << "Background color R(Red depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Background color G(Green depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[1] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Background color B(BLUE depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[2] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');


                    break;
                }
                else if (InputB == "3") {
                    InputB = "0";
                    LUcolor.clear();
                    LUcolor.resize(4);

                    Scolor.clear();
                    Scolor.resize(4);

                    channels = 4;

                    std::cout << "Island color R(Red depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Island color G(Green depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[1] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Island color B(BLUE depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[2] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Island color A(Transparency) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[3] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    //_______________________________

                    std::cout << "Background color R(Red depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[0] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Background color G(Green depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[1] = aa;
                    SetColor(WHITE);

                    std::cout << "Background color B(BLUE depth) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    Scolor[2] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                    std::cout << "Background color A(Transparency) (0 - 255)\n";
                    SetColor(BRIGHT_CYAN);
                    for (;;) {
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                    }
                    LUcolor[3] = aa;
                    SetColor(WHITE);
                    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');




                    break;
                }
                else { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
            }
        }
else if (InputB == "2") {
    for (;;) {

        std::cout << "10 levels, the higher the level, the larger the size\n"
            << "Enter 0 to Return\n"
            << "Enter level\n";

        SetColor(BRIGHT_CYAN);
        std::cin >> size;
        SetColor(WHITE);
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        if (InputB == "0")return;
        else if (size >= 0 && size <= 10) { break; }
        else { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
    }
        }

else if (InputB == "3") {
            for (;;) {

                std::cout << "10 levels, the higher the level, the larger the size\n"
                    << "Enter 0 to Return\n"
                    << "Enter level\n";

                SetColor(BRIGHT_CYAN);
                std::cin >> uneven;
                SetColor(WHITE);
                if (InputB == "0")return;
                else if (!(uneven >= 0 && uneven <= 10)) { SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
                else break;
            }
        }
else if (InputB == "4") { break; }else if(InputB == "5")
{
    std::string name;
    std::cout << "Enter img name\n";
    SetColor(BRIGHT_YELLOW);
    std::cin >> name;
    SetColor(WHITE);

    int times;
    std::cout << "How many times\n";
    SetColor(BRIGHT_CYAN);
    std::cin >> times;
    SetColor(WHITE);

    int nameint = 1;
    //创建名字
    std::string str = name + std::to_string(nameint) + ".png";
    //改名字
    for (; FileExists((*Pathptr).c_str(), str.c_str()); nameint++);
    //生成
    for (; times > 0; times--) { 
        nameint++; 
        GCenter(name, nameint, channels, size, uneven, LUcolor, Scolor); 

    }
    break;
}else{ SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
    }
    //Island
}
//----------------------------------------------------------------------------- //color是第XX个颜色  //acc是否精确    //per是第XX个颜色的像素数       //namberName是加在名字后面的数字

void GRandomcutom (bool acc, int channel, std::vector<std::vector<unsigned char>> color, std::vector<int> Per, std::string name, int numberName) {
    if (!(channel == color[0].size())) {
        std::exit(1);
    }

    // 1. 找到并移除最大颜色
    auto maxnumber = std::max_element(Per.begin(), Per.end());
    int max_index = std::distance(Per.begin(), maxnumber);
    Per.erase(maxnumber);

    std::vector<std::vector<uint32_t>> Location;

    // 2. 生成位置
    if (acc) {
        for (int i = 0; i < Per.size(); i++) {
            std::vector<uint32_t> ca;

            for (int j = 0; j < Per[i]; j++) {  // 注意：这里应该是Per[i]，不是Per[i]-1
                uint32_t a;
                do {
                    a = Synthesis() % (X * Y);
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
        for (int i = 0; i < Per.size(); i++) {
            std::vector<uint32_t> ca;
            for (int j = 0; j < Per[i]; j++) {  // 注意：这里应该是Per[i]
                ca.push_back(Synthesis() % (X * Y));
            }
            Location.push_back(ca);
        }
    }

    // 3. 创建图像并用最大颜色填充背景
    std::vector<unsigned char> img(X * Y * channel);
    std::vector<unsigned char>& max_color = color[max_index];

    // 正确填充背景：每个像素的所有通道
    for (int i = 0; i < X * Y; i++) {
        for (int k = 0; k < channel; k++) {
            img[i * channel + k] = max_color[k];
        }
    }

    // 4. 移除最大颜色
    color.erase(color.begin() + max_index);

    // 5. 绘制其他颜色到指定位置
    for (int i = 0; i < Location.size(); i++) {
        for (int j = 0; j < Location[i].size(); j++) {  // 正确：使用size()
            uint32_t pos = Location[i][j];
            for (int k = 0; k < channel; k++) {
                img[pos * channel + k] = color[i][k];  // 正确：pos * channel
            }
        }
    }

    // 6. 保存图像
    if (!stbi_write_png(((*Pathptr) + name + std::to_string(numberName) + ".png").c_str(),
        X, Y, channel, img.data(), X * channel)) {
        std::exit(1);
    }
}


/*
void GRandomcutom(bool acc, int channel, std::vector<std::vector<unsigned char>>color, std::vector<int>Per, std::string name,int numberName) {
    if (!(channel == color[0].size())) {
        std::exit(1);
    }
    
    auto maxnumber = std::max_element(Per.begin(), Per.end());

    int max_index = std::distance(Per.begin(), maxnumber);

    Per.erase(maxnumber);


    std::vector<std::vector<uint32_t>> Location;
    if (acc) {
        
   
        for (int i = 0;i < Per.size();i++) {
            std::vector<uint32_t> ca;

            for (int j = 0; j < Per[i] - 1;j++) {
                uint32_t a = Synthesis();
                
                do {
                    a = Synthesis() % (X * Y);
                    
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
        for (int i = 0; i < Per.size(); i++) {
            std::vector<uint32_t> ca;

            for (int j = 0; j < Per[i] - 1; j++) {
                
                ca.push_back(Synthesis() % (X * Y));

            }
            Location.push_back(ca);
        }
    }

    std::vector<unsigned char> img(X * Y * channel,color[0][0]);
    
    if (channel == 3) {
        //我敢在for里写除说明我觉得编译器会优化
        for (int i = 0; i < (img.size() / 3); i++) {

            for (int j = 0; j < img.size(); j++) {
                img[i * j] = color[max_index][i];
            }
        }
    }
    else if (channel == 4) {

        for (int i = 0; i < (img.size() / 4); i++) {

            for (int j = 0; j < img.size(); j++) {
                img[i * j] = color[max_index][i];
            }
        }

    }

    color.erase(color.begin() + max_index);


    for (int i = 0;i < Location.size();i++) {

        for (int j = 0;j < Location[i][j];j++) {
            
            for(int k = 0;k < channel; k++) img[((Location[i][j] * channel) - 1) + k] = color[i][k];


        }
    }
    

    if (!stbi_write_png(((*Pathptr) + name + std::to_string(numberName) + ".png").c_str(), X, Y, channel, img.data(), X * channel)) std::exit(1);
    
}

*/


//-----------------------------------------------------------------------------
void Randomcutom(){
    
    int channel = 1;

    std::vector<BWarr>BWA;
    std::vector<RGB>RGBZ;
    std::vector<ARGB>ARGBZ;
    std::vector<int> Percentage;

    for (;;) {
        

        forPrint({"Set Color channel",
            "Set color",
            "Return",
            "Generate img"}, "Set Properties(If not set, the default value will be used)");
        
        /*std::cout << "Set Properties(If not set, the default value will be used)\n"
            << "1.Set Color channel \n"
            
            << "2.Set color\n"
            << "3.Return\n"
            << "4.Generate img\n"
            << "Enter number\n";
            */
        std::string InputB;
        SetColor(BRIGHT_CYAN);
        std::cin >> InputB;
        SetColor(WHITE);

        if (InputB == "3") { break; }
        else if (InputB == "1") {
            for (;;) {
                BWA.clear();
                RGBZ.clear();
                ARGBZ.clear();
                Percentage.clear();
                
                

                /*std::cout << "Color Channel\n"
                    << "1.Single channel(black and white)\n"
                    << "2.Three-channel(RGB)\n"
                    << "3.Four-channel(ARGB)\n"
                    << "Enter number\n";*/
                for (;;) {

                    forPrint({ "Single channel(black and white)",
                    "Three-channel(RGB)",
                    "Four-channel(ARGB)" }, "Color Channel");

                    SetColor(BRIGHT_CYAN);
                    std::cin >> channel;
                    SetColor(WHITE);
                    if (!(channel > 0 && 4 > channel)) {
                        SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); 
                    }else if(!(channel == 1)){
                        channel++;
                        
                        break;
                    }
                    else { break; }

                }

                break;

            }
        }
        else if(InputB == "2"){
            for (;;) {
                InputB = "0";
                std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                
                forPrint({"add color",
                    "Delete color",
                    "View color",
                    "Clear color",
                    "Return"}, "");

                /*std::cout << "1.add color\n"
                    << "2.Delete color\n"
                    << "3.View color\n"
                    << "4.Clear color\n"
                    "Enter number\n";
                    */
                SetColor(BRIGHT_CYAN);
                std::cin >> InputB;
                SetColor(WHITE);
                //开始设置
                
                
                if(InputB == "1") {
                    //判断通道
                    
                    int aa = 0;
                    
                    if (channel == 1) {
                        
                            std::cout << "Enter color(0-255)(Black Depth)\n";
                            BWarr cachea ;
                            cachea.Black = 0;
                            SetColor(BRIGHT_CYAN);

                            for (;;) {
                                std::cin >> aa;
                                if (!(aa > -1 && aa < 256)) {
                                    SetColor(BRIGHT_RED);
                                    std::cout << "Input value out of range\n";
                                    SetColor(BRIGHT_CYAN);
                                }
                                else { break; }
                            }
                            cachea.Black = aa;
                            SetColor(WHITE);
                            BWA.push_back(cachea);
                            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                        for (;;) {
                            std::cout << "Generation probability(1 -100)\n";
                            int cacheb;
                            SetColor(BRIGHT_CYAN);
                            std::cin >> cacheb;
                            SetColor(WHITE);
                            if (cacheb > 100) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                Percentage.push_back(cacheb);
                                
                                break;
                            }
                        }
                    }

                    else if (channel == 3){
                        unsigned char R = 0;
                        unsigned char G = 0;
                        unsigned char B = 0;

                        
                        std::cout << "Enter color R(0-255)(Red Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        for (;;) {
                            std::cin >> aa;
                            if (!(aa > -1 && aa < 256)) {
                                SetColor(BRIGHT_RED);
                                std::cout << "Input value out of range\n";
                                SetColor(BRIGHT_CYAN);
                            }
                            else { break; }

                        }
                        R = aa;
                        SetColor(WHITE);
                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        std::cout << "Enter color G(0-255)(Green Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        
                        for (;;) {
                            std::cin >> aa;
                            if (!(aa > -1 && aa < 256)) {
                                SetColor(BRIGHT_RED);
                                std::cout << "Input value out of range\n";
                                SetColor(BRIGHT_CYAN);
                            }
                            else { break; }

                        }
                        G = aa;
                        SetColor(WHITE);
                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        std::cout << "Enter color B(0-255)(B Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        for (;;) {
                            std::cin >> aa;
                            if (!(aa > -1 && aa < 256)) {
                                SetColor(BRIGHT_RED);
                                std::cout << "Input value out of range\n";
                                SetColor(BRIGHT_CYAN);
                            }
                            else { break; }
                        }
                        B = aa;
                        SetColor(WHITE);
                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        RGBZ.push_back({ R,G,B });
                        //判断
                        for (;;) {
                            std::cout << "Generation probability(1 -100)\n";
                            int cacheb;
                            SetColor(BRIGHT_CYAN);
                            std::cin >> cacheb;
                            SetColor(WHITE);
                            if (cacheb > 100) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                Percentage.push_back(cacheb);
                                break;
                            }
                        }
                    }
                    else if (channel == 4){
                        unsigned char R = 0;
                        unsigned char G = 0;
                        unsigned char B = 0;
                        unsigned char A = 0;


                        std::cout << "Enter color R(0-255)(Red Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        for (;;) {
                            std::cin >> aa;
                            if (!(aa > -1 && aa < 256)) {
                                SetColor(BRIGHT_RED);
                                std::cout << "Input value out of range\n";
                                SetColor(BRIGHT_CYAN);
                            }
                            else { break; }
                        }
                        R = aa;
                        SetColor(WHITE);
                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        std::cout << "Enter color G(0-255)(Green Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        
                        for (;;) {
                                std::cin >> aa;
                                if (!(aa > -1 && aa < 256)) {
                                    SetColor(BRIGHT_RED);
                                    std::cout << "Input value out of range\n";
                                    SetColor(BRIGHT_CYAN);
                                }
                                else { break; }
                        }
                        G = aa;
                        SetColor(WHITE);
                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        std::cout << "Enter color B(0-255)(B Depth)\n";
                        SetColor(BRIGHT_CYAN);
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                        SetColor(WHITE);
                        B = aa;
                        std::cout << "Enter Transparency(0-255)\n";
                        SetColor(BRIGHT_CYAN);
                        std::cin >> aa;
                        if (!(aa > -1 && aa < 256)) {
                            SetColor(BRIGHT_RED);
                            std::cout << "Input value out of range\n";
                            SetColor(BRIGHT_CYAN);
                        }
                        else { break; }
                        A = aa;
                        SetColor(WHITE);

                        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

                        ARGBZ.push_back({ R,G,B,A });

                        for (;;) {
                            std::cout << "Generation probability(1 -100)\n";
                            int cacheb;
                            SetColor(BRIGHT_CYAN);
                            std::cin >> cacheb;
                            SetColor(WHITE);
                            if (cacheb > 100) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                Percentage.push_back(cacheb);
                                break;
                            }
                        }
                    

                    }
                    else std::exit(1);

                }
                else if (InputB == "2") {
                    for (;;) {

                        int cacheb;
                        std::cout << "Enter TokenNumber(You don't know the token goes to (View color) to see)\n";
                        SetColor(BRIGHT_CYAN);
                        std::cin >> cacheb;
                        SetColor(WHITE);

                        if (channel == 1) {
                            if (cacheb > BWA.size()) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                std::cout << "OK\n";
                                BWA.erase(BWA.begin() + cacheb);
                                Percentage.erase(Percentage.begin() + cacheb);
                                break;
                            }
                        }
                        else if (channel == 3) {
                            if (cacheb > RGBZ.size()) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                std::cout << "OK\n";
                                RGBZ.erase(RGBZ.begin() + cacheb);
                                Percentage.erase(Percentage.begin() + cacheb);
                                break;
                            }
                        }
                        else if (channel == 4) {
                            if (cacheb > ARGBZ.size()) {
                                SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
                            }
                            else {
                                std::cout << "OK\n";
                                ARGBZ.erase(ARGBZ.begin() + cacheb);
                                Percentage.erase(Percentage.begin() + cacheb);
                                break;
                            }
                        }
                        else std::exit(1);

                    }
                }
                else if (InputB == "3") {
                    if(channel == 1){
                        std::cout << "TokenNumber" << std::setw(16) << "black(Depth)" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "\n";
                        for (int i = 0; i < BWA.size();i++) {
                            
                            std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(BWA[i].Black) << std::setw(32) << std::to_string(Percentage[i]) << std::setw(48);
                            showColor(BWA[i].Black, BWA[i].Black, BWA[i].Black);
                            std::cout << std::setw(64) << "\n";
                        }
                    }
                    else if (channel == 3) {
                        
                        std::cout << "TokenNumber" << std::setw(16) << "R  G  B" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "\n";
                        for (int i = 0; i < RGBZ.size(); i++) {

                            std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(RGBZ[i].R)<< "  "<< static_cast<int>(RGBZ[i].G) << "  " << static_cast<int>(RGBZ[i].B) << std::setw(32) << std::to_string(Percentage[i]) << std::setw(48);
                            showColor(RGBZ[i].R, RGBZ[i].G, RGBZ[i].B);
                            std::cout << std::setw(64) << "\n";
                        }
                    }
                    else if (channel == 4) {

                        std::cout << "TokenNumber" << std::setw(16) << "R  G  B  A" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "\n";
                        for (int i = 0; i < ARGBZ.size(); i++) {

                            std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(ARGBZ[i].R) << "  " << static_cast<int>(ARGBZ[i].G) << "  " << static_cast<int>(ARGBZ[i].B) << "  " << static_cast<int>(ARGBZ[i].A) << std::setw(32) << std::to_string(Percentage[i]) << std::setw(48);
                            showColor(ARGBZ[i].R, ARGBZ[i].G, ARGBZ[i].B , ARGBZ[i].A);
                            std::cout << std::setw(64) << "\n";
                        }
                    }

                }

                else if (InputB == "4") {
                    std::cout << "Clear all?\n"
                        <<"Enter(y/n)";
                    std::string yn;
                    std::cin >>yn;
                    if (yn == "y") {
                        BWA.clear();
                        RGBZ.clear();
                        ARGBZ.clear();
                        Percentage.clear();
                    }
                }
                else if(InputB == "5"){
                    break;
                
                }

                else{ SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE); }
            }
        }

        else if (InputB == "4") {      //生成
            


            bool acc = true;
            while (true) {
                std::cout << "Is it accurate?(Choosing (y) will take more time, but it will generate strictly according to the percentage.)\n"
                    << "(y/n)\n";
                std::string ca;
                std::cin >> ca;
                if (ca == "y" || ca == "n") {
                    if (ca == "y") {
                        acc = true;
                    }
                    else
                    {
                        acc = false;
                    }
                    break;
                }
                else {
                    SetColor(BRIGHT_RED); std::cout << "(X)Please enter (y/n)\n"; SetColor(WHITE);
                }
            }
            uint64_t Total = 0;
            
            //这样写我自信的觉得应该不会溢出，除非有萨比试，（就算你全输100，也要输？？？次）
            for (int i = 0 ;i < Percentage.size();i++) {
                Total += Percentage[i];
            }
            
            std:: cout<< Total;


            std::vector<int>NOFP;

            for (int i = 0; i < Percentage.size(); i++) {
                NOFP.push_back((Total * 100) / Percentage[i]);
                NOFP[i] = (X * Y * 100) / (NOFP[i] + (NOFP[i] /10));
                
            }

            {

                if (channel == 1) {
                    std::cout << "TokenNumber" << std::setw(16) << "black(Depth)" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "Pixel count" << std::setw(80) << "\n";
                    for (int i = 0; i < BWA.size(); i++) {

                        std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(BWA[i].Black) << std::setw(32) << std::to_string((Total * 10)/ Percentage[i]) << "%" << std::setw(48);
                        showColor(BWA[i].Black, BWA[i].Black, BWA[i].Black);
                        std::cout << std::setw(64) << NOFP[i] << std::setw(80) << "\n";
                    }
                }
                else if (channel == 3) {

                    std::cout << "TokenNumber" << std::setw(16) << "R  G  B" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "Pixel count" << std::setw(80) << "\n";
                    for (int i = 0; i < RGBZ.size(); i++) {

                        std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(RGBZ[i].R) << "  " << static_cast<int>(RGBZ[i].G) << "  " << static_cast<int>(RGBZ[i].B) << std::setw(32) << std::to_string((Total * 10)/ Percentage[i]) << "%" << std::setw(48);
                        showColor(RGBZ[i].R, RGBZ[i].G, RGBZ[i].B);
                        std::cout << std::setw(64) << NOFP[i] << std::setw(80) << "\n";
                    }
                }
                else if (channel == 3) {

                    std::cout << "TokenNumber" << std::setw(16) << "R  G  B  A" << std::setw(32) << "Probability" << std::setw(48) << "Demo Color" << std::setw(64) << "Pixel count" << std::setw(80) << "\n";
                    for (int i = 0; i < ARGBZ.size(); i++) {

                        std::cout << std::to_string(i) << std::setw(16) << static_cast<int>(ARGBZ[i].R) << "  " << static_cast<int>(ARGBZ[i].G) << "  " << static_cast<int>(ARGBZ[i].B) << "  " << static_cast<int>(ARGBZ[i].A) << std::setw(32) << std::to_string((Total * 10) / Percentage[i]) << "%" << std::setw(48);
                        showColor(ARGBZ[i].R, ARGBZ[i].G, ARGBZ[i].B, ARGBZ[i].A);
                        std::cout << std::setw(64) << NOFP[i] << std::setw(80) << "\n";
                    }
                }



            }


         
            
            std::string name;
            std::cout << "Enter img name\n";
            SetColor(BRIGHT_YELLOW);
            std::cin >> name;
            SetColor(WHITE);

            int times;
            std::cout << "How many times\n";
            SetColor(BRIGHT_CYAN);
            std::cin >> times;
            SetColor(WHITE);

            std::vector<std::thread> threads;

            int nameint = 1;
            //创建名字
            std::string str = name + std::to_string(nameint);
            //改名字
            for (; FileExists((*Pathptr).c_str(), str.c_str()); nameint++) str = name + std::to_string(nameint);
            
            std::vector<std::vector<unsigned char>>zhq;
            //转换
            if (channel == 1) {
                for (int i = 0; i < BWA.size();i++) {
                    std::vector<unsigned char>a;
                    a.push_back( BWA[i].Black);
                    zhq.push_back(a);
                }

            }
            else if (channel == 3) {
                for (int i = 0; i < RGBZ.size(); i++) {
                    std::vector<unsigned char>a;
                    a.push_back(RGBZ[i].R);
                    a.push_back(RGBZ[i].G);
                    a.push_back(RGBZ[i].B);
                    zhq.push_back(a);
                }
            }
            else if (channel == 4) {
                for (int i = 0; i < ARGBZ.size(); i++) {
                    std::vector<unsigned char>a;
                    a.push_back(ARGBZ[i].R);
                    a.push_back(ARGBZ[i].G);
                    a.push_back(ARGBZ[i].B);
                    a.push_back(ARGBZ[i].A);
                    zhq.push_back(a);
                }
            }

            //创建线程
            {
                int timet = times + nameint;
                
                if (channel == 1) {
                    for (int i = nameint; timet > i; i++) {
                        
                        threads.push_back(std::thread(GRandomcutom, acc, channel, zhq,NOFP,name,i ));
                        Sleep(100);
                    }
                }
                else if (channel == 3) {
                    for (int i = nameint; timet > i; i++) {
                        threads.push_back(std::thread(GRandomcutom, acc, channel, zhq, NOFP, name, i));
                        Sleep(100);
                    }
                }
                else if (channel == 4) {
                    for (int i = nameint; timet > i; i++) {
                        threads.push_back(std::thread(GRandomcutom, acc, channel, zhq, NOFP, name, i));
                        Sleep(100);
                    }
                }
                
            }
            
            for (int i = 0; i < times; i++) {
                ProgressBar((i * 100) / times);
                threads[i].join();
                DeletePreviousLine();
            }

            std::cout << "OK\n";
            
            
        }
    }
}
//-----------------------------------------------------------------------------
void Custom(){
    for (;;) {
        std::cout << "\n"
            <<"Noise Mode\n"
            << "1.Cloud\n"
            << "2.Random(Custom probability)\n"
            << "3.Center\n"
            << "4.Return\n"
            //<< "4.Surroundings\n"
            << "Enter number\n";
        SetColor(BRIGHT_CYAN);
        std::string InputB;
        std::cin >> InputB;
        SetColor(WHITE);
        if (InputB == "1")Cloud();
        if (InputB == "3")Center();
        if (InputB == "2")Randomcutom();
        if (InputB == "4")break;
    }
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int main()
{
    
    Path = "D:\\Noise";
    //退出输入E
    //std::cout << "Exit Input :“E”\n";
    
    SetPath();
    
    //设置XY
    SetXY();

    //真正的运行生成噪点
    for (;;) {
        forPrint({
            "Set size",
            "Set Path",
            "Random Generate noise(Black, white, and gray)",
            "Random Generate noise(Black and white)",
            "Random Generate noise(Colorful,Opaque)",
            "Random Generate noise(Colorful,Transparent)",
            "Custom"}, " ");
        /*std::cout << "1.Set size\n"
            << "2.Set Path\n" 
            << "3.Random Generate noise(Black, white, and gray)\n" 
            << "4.Random Generate noise(Black and white)\n"
            << "5.Random Generate noise(Colorful,Opaque)\n"
            << "6.Random Generate noise(Colorful,Transparent)\n"
            << "7.Custom\n" 
            << "8.Exit(Input :“E”or 8)\n" 
            << "Enter number\n";*/
        SetColor(BRIGHT_CYAN);
        std::string InputA;
        std::cin >> InputA;
        SetColor(WHITE);
        //按E或8退出
        if (InputA == "E" || InputA == "8") std::exit(0);
        else if (InputA == "1") SetXY();
        else if (InputA == "2") SetPath();
        else if (InputA == "3") RGN();
        else if (InputA == "4") RGNBW();
        else if (InputA == "5") RGNRGB();
        else if (InputA == "6") RGNARGB();
        else if (InputA == "7")Custom();
        else SetColor(BRIGHT_RED); std::cout << "(X)Please enter a valid number\n"; SetColor(WHITE);
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件


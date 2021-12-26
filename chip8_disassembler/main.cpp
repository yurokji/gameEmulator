#define SDL_MAIN_HANDLED


#include <iostream>
#include <fstream>

#include "chip8_decode.h"
#include <windows.h>
#include <string>
#include <format>


using namespace std;

const int windowHeight = CHIP8_HEIGHT * CHIP8_DISPLAY_ZOOM;
const int windowWidth = CHIP8_WIDTH * CHIP8_DISPLAY_ZOOM;



int main()
{
    unsigned code = 0xE0;
    ifstream in("./roms/pong.ch8", ios::in | ios::binary | ios::ate);
    ifstream::pos_type size;
    char* romData;
    if (!in.is_open()) {
        cerr << "'. /roms/pong.ch8' 파일 열기를 실패했습니다." << endl;
    }

    size = in.tellg();
    romData = new char[size];
    in.seekg(0, ios::beg);
    in.read(romData, size);
    in.close();

    // 16진수 형태로 롬 내용 보기
    cout << "rom content" << endl;
    unsigned short temp = 0;
    for (int i = 0; i < size; i += 2)
    {
        if (i % 16 == 0)
        {
            cout << endl;
            cout << setfill('0') << setw(4) << hex << i << "h:    ";
        }
        temp = ((unsigned char)romData[i] << 8) | (unsigned char)romData[i + 1];
        cout << setfill('0') << setw(4) << hex << temp << " ";
    }
    cout << endl;
 
    // 파일 쓰기 준비
    ofstream ofile("./roms/pong.asm");
    string str = "p test";
    std::stringstream ss; 
    std::string s, s2;
    std::string res;
    if (ofile.is_open()) {
        for (int i = 0; i < size; i += 2)
        {
            unsigned short temp = ((unsigned char)romData[i] << 8) | (unsigned char)romData[i + 1];
            res = decode(temp);
            ss << setfill('0') << setw(4) << hex << 0x200+i;
            s = ss.str();
            ss.str("");
            ss << "\t"<< setfill('0') << setw(4) << hex << temp;
            s2 = ss.str();
            ss.str("");
            ofile <<s << s2 << "\t" << res << endl;
             
        }
    }

    ofile.close();
    return 0;
}


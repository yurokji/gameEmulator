#define SDL_MAIN_HANDLED


#include <iostream>
#include <fstream>
#include <iomanip>
#include "chip8.h"
#include <windows.h>

using namespace std;

const int windowHeight = CHIP8_HEIGHT * CHIP8_DISPLAY_ZOOM;
const int windowWidth = CHIP8_WIDTH * CHIP8_DISPLAY_ZOOM;



int main()
{
    unsigned code = 0xE0;
    ifstream in("./roms/tetris.ch8", ios::in | ios::binary | ios::ate);
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
        temp =  ((unsigned char) romData[i] << 8)  | (unsigned char) romData[i + 1];
        cout <<setfill('0') << setw(4) << hex << temp << " ";
    }
    cout << endl;
    


    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Chip-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth,
        windowHeight, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        SDL_Quit();
        return 2;
    }


    //SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    bool quit = false;
    SDL_Event event;


    Chip8 myChip8;
    myChip8.loadProgram(romData, size);

    while (!quit) {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                char keyIn = event.key.keysym.sym;
                int keyOut = myChip8.mapKey(keyIn);
                if (keyOut != -1)
                {
                    myChip8.keyboard[keyOut] = true;
                }

            }
            else if (event.type == SDL_KEYUP)
            {
                char keyIn = event.key.keysym.sym;
                int keyOut = myChip8.mapKey(keyIn);
                if (keyOut != -1)
                {
                    myChip8.keyboard[keyOut] = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_Rect rect;
        for (int py = 0; py < CHIP8_HEIGHT; py++)
        {
            for (int px = 0; px < CHIP8_WIDTH; px++)
            {
                if (myChip8.display_pixels[py][px] == true)
                {
                    rect.x = px * CHIP8_DISPLAY_ZOOM;
                    rect.y = py * CHIP8_DISPLAY_ZOOM;
                    rect.w = CHIP8_DISPLAY_ZOOM;
                    rect.h = CHIP8_DISPLAY_ZOOM;
                    SDL_RenderFillRect(renderer, &rect);
                }
                
            }
        }

        SDL_RenderPresent(renderer);
        
        if (myChip8.DT > 0)
        {
            Sleep(100);
            myChip8.DT-=1;
        }
        if (myChip8.ST > 0)
        {
            Beep(1500, 100 * myChip8.ST);
            myChip8.ST = 0;
        }
        // 다음 명령어 가져오기
        unsigned short code = myChip8.fetch();

        // 다음 명령어를 순차대로 가져온다
        myChip8.PC += 2;

        // 가져온 명령어 실행하기
        myChip8.execute(code);
        

    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}


//밑바닥부터 만드는 C++ 게임 에뮬레이터
//작성자: 로우레벨 코더
//08 어셈블러 작성 및 시연

#define SDL_MAIN_HANDLED



#include "chip8.h"

const int sceenHeight = CHIP8_HEIGHT * CHIP8_DISPLAY_ZOOM;
const int screenWidth = CHIP8_WIDTH * CHIP8_DISPLAY_ZOOM;
const int infoHeight = 250;
const int infoWidth = CHIP8_WIDTH * CHIP8_DISPLAY_ZOOM;
const int windowHeight = sceenHeight + infoHeight;
const int windowWidth = screenWidth;
const int playButtonX = infoWidth - 300;
const int playButtonY = windowHeight - 200;
const int pauseButtonX = infoWidth - 300;
const int pauseButtonY = windowHeight - 200;
const int stepButtonX = infoWidth - 150;
const int stepButtonY = windowHeight - 200;

int mode = CHIP8_ASSEMBLY_MODE;
//int step_mode = CHIP8_STEP_MODE;
int step_mode = CHIP8_PLAY_MODE;
//int mode = CHIP8_DISASSEMBLY_MODE;
//int mode = CHIP8_BINARY_MODE;

const SDL_Color WHITE = { 255,255,255 };
const SDL_Color BLACK = { 255,255,255 };
const SDL_Color GRAY = { 127,127,127 };
const SDL_Color RED = BLACK;
const SDL_Color GREEN = { 0,255,0 };
const SDL_Color BLUE = { 0,0,255 };
const SDL_Color YELLOW = { 255,255,0 };



//클릭된 점 (mx, my)이  중심(cx, cy), 반지름 r로 지정된 원안에 있는지 검사
bool isInCircle(int mx, int my, int cx, int cy, int r)
{
    int distSquared = (mx - cx) * (mx - cx) + (my - cy) * (my - cy);
    if (distSquared < r * r) return true;
    else return false;
}


//SDL 폰트로 텍스트 렌더링 텍스쳐, 텍스쳐 rect 생성
void getText(SDL_Renderer* renderer, int x, int y, SDL_Color textColor, std::string text,
    TTF_Font* font, SDL_Texture** texture, SDL_Rect* rect) {
    int text_width;
    int text_height;
    SDL_Surface* surface;
    surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}


int main()
{
    Chip8 myChip8;
    unsigned code = 0xE0;
    std::string str;
    std::vector <std::string> vecLines;
    std::ifstream::pos_type size;
    std::ifstream in;
    std::string filename = "./roms/pong2.asm";
    
    if (mode == CHIP8_ASSEMBLY_MODE)
    {
        if (filename.substr(filename.find_last_of(".") + 1) == "asm")
        {
            std::cout << "어셈블리 파일을 기계어로 변환합니다." << std::endl;
            in.open(filename);
            if (!in.is_open())
            {
                std::cerr << filename + "파일 열기를 실패했습니다." << std::endl;
            }
            while (std::getline(in, str))
            {
                vecLines.push_back(str);
                //std::cout << str << std::endl;

            }
            in.close();


            myChip8.assemble(vecLines);
            std::cout << "어셈블리 완료" << std::endl;
        }
    }

    else {

        std::cout << "기계어 프로그램 입력되었습니다." << std::endl;
        in.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
        if (!in.is_open()) {
            std::cerr << filename + "파일 열기를 실패했습니다." << std::endl;
        }

        myChip8.romSize = in.tellg();
        myChip8.romData = new char [myChip8.romSize];
        in.seekg(0, std::ios::beg);
        in.read(myChip8.romData, myChip8.romSize);
        in.close();
    }



    // 16진수 형태로 롬 내용 보기
    std::cout << "rom content" << std::endl;
    unsigned short temp = 0;
    for (int i = 0; i < myChip8.romSize; i += 2)
    {
        if (i % 16 == 0)
        {
            std::cout << std::endl;
            std::cout << std::setfill('0') << std::setw(4) << std::hex << i << "h:    ";
        }
        temp =  ((unsigned char)myChip8.romData[i] << 8)  | (unsigned char)myChip8.romData[i + 1];
        std::cout << std::setfill('0') << std::setw(4) << std::hex << temp << " ";
    }
    std::cout << std::endl;



    //프로그램을 ROM에서 RAM으로 복사
    myChip8.loadProgram();



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



    

    std::stringstream ss;
    std::string s;
    std::string font_path = "C:\\Windows\\Fonts\\NanumGothicBold.ttf";
    SDL_Rect text_step_rect, text_line_rect, text_inst_rect, text_PC_rect, text_I_rect, text_DT_rect, text_ST_rect, text_SP_rect;
    SDL_Texture* text_step_texture, *text_line_texture, *text_inst_texture, *text_PC_texture, *text_I_texture, *text_DT_texture, *text_ST_texture, *text_SP_texture;
    SDL_Rect text_general_reg_rect[CHIP8_NUM_GENERAL_REGS];
    SDL_Rect text_keyboard_rect[CHIP8_NUM_KEYS];
    SDL_Texture *text_general_reg_texture[CHIP8_NUM_GENERAL_REGS];
    SDL_Texture *text_keyboard_texture[CHIP8_NUM_KEYS];
    TTF_Init();
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), 20);

    SDL_Texture *imgPlayButton = IMG_LoadTexture(renderer, CHIP8_IMG_PLAY_BUTTON_PATH);
    SDL_Texture* imgPauseButton = IMG_LoadTexture(renderer, CHIP8_IMG_PAUSE_BUTTON_PATH);
    SDL_Texture* imgStepButton = IMG_LoadTexture(renderer, CHIP8_IMG_STEP_BUTTON_PATH);
    int buttonWidth, buttonHeight;
    SDL_QueryTexture(imgPlayButton, NULL, NULL, &buttonWidth, &buttonHeight);
    SDL_Rect playButtonRect; 
    playButtonRect.x = playButtonX;
    playButtonRect.y = playButtonY;
    playButtonRect.w = buttonWidth;
    playButtonRect.h = buttonHeight;
    int playButtonCenterX = playButtonRect.x + playButtonRect.w / 2;
    int playButtonCenterY = playButtonRect.y + playButtonRect.h / 2;
    int buttonRadius = (int)(playButtonRect.w / 2.0);

    SDL_QueryTexture(imgPauseButton, NULL, NULL, &buttonWidth, &buttonHeight);
    SDL_Rect pauseButtonRect;
    pauseButtonRect.x = pauseButtonX;
    pauseButtonRect.y = pauseButtonY;
    pauseButtonRect.w = buttonWidth;
    pauseButtonRect.h = buttonHeight;
    int pauseButtonCenterX = pauseButtonRect.x + pauseButtonRect.w / 2;
    int pauseButtonCenterY = pauseButtonRect.y + pauseButtonRect.h / 2 ;

    SDL_QueryTexture(imgStepButton, NULL, NULL, &buttonWidth, &buttonHeight);
    SDL_Rect stepButtonRect;
    stepButtonRect.x = stepButtonX;
    stepButtonRect.y = stepButtonY;
    stepButtonRect.w = buttonWidth;
    stepButtonRect.h = buttonHeight;
    int stepButtonCenterX = stepButtonRect.x + stepButtonRect.w / 2;
    int stepButtonCenterY = stepButtonRect.y + stepButtonRect.h / 2 ;



    int mouseX, mouseY;

    bool stepModeSelected = false;
    bool executeStep = false;
    bool stepOne = false;

    if (mode == CHIP8_DISASSEMBLY_MODE)
    {
        // 파일 쓰기 준비
        std::ofstream ofile("./roms/pong.asm");
        std::string str = "p test";
        std::stringstream ss;
        std::string s, s2;
        std::string res;
        if (ofile.is_open()) {
            for (int i = 0; i < myChip8.romSize; i += 2)
            {
                unsigned short temp = ((unsigned char)myChip8.romData[i] << 8) | (unsigned char)myChip8.romData[i + 1];
                res = myChip8.decode(temp);
                ss << std::setfill('0') << std::setw(4) << std::hex << 0x200 + i;
                s = ss.str();
                ss.str("");
                ss << "\t" << std::setfill('0') << std::setw(4) << std::hex << temp;
                s2 = ss.str();
                ss.str("");
                ofile << s << s2 << "\t" << res << std::endl;

            }
        }

        ofile.close();
    }
    else
    {
        while (!quit)
        {
            while (SDL_PollEvent(&event))
            {
                SDL_GetMouseState(&mouseX, &mouseY);
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
                else if (event.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        mouseX = event.button.x;
                        mouseY = event.button.y;

                        //std::cout << "마우스 왼쪽 버튼 클릭: "<<std::dec << mouseX <<", " << mouseY <<" )" << std::endl;
                        //std::cout << "버튼 중앙: " << std::dec << playButtonCenterX << ", " << playButtonCenterY << " )" << std::endl;
                        //std::cout << "버튼 위치: " << std::dec << playButtonRect.x << ", " << playButtonRect.y << " )" << std::endl;
                        if (isInCircle(mouseX, mouseY, pauseButtonCenterX, pauseButtonCenterY, buttonRadius))
                        {
                            stepModeSelected = !mode;
                            stepOne = true;
                        }
                        if (step_mode == CHIP8_STEP_MODE)
                        {
                            if (isInCircle(mouseX, mouseY, stepButtonCenterX, stepButtonCenterY, buttonRadius))
                            {
                                executeStep = true;
                                stepOne = !stepOne;
                                
                            }

                        }
                        mode = stepModeSelected;
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



            if (myChip8.DT > 0)
            {
                Sleep(10);
                myChip8.DT -= 1;
            }
            if (myChip8.ST > 0)
            {
                Beep(1500, 10 * myChip8.ST);
                myChip8.ST = 0;
            }

            SDL_Rect debugRect;
            debugRect.x = 0;
            debugRect.y = sceenHeight;
            debugRect.w = screenWidth;
            debugRect.h = infoHeight;
            SDL_SetRenderDrawColor(renderer, 70, 70, 70, 0);
            SDL_RenderFillRect(renderer, &debugRect);

            

            if (step_mode == CHIP8_PLAY_MODE)
            {
                //std::cout << "CHIP8_PLAY_MODE" << std::endl;
                // 다음 명령어 가져오기
                code = myChip8.fetch();
                // 다음 명령어를 순차대로 가져온다
                myChip8.PC += 2;
                // 가져온 명령어 실행하기
                myChip8.execute(code);
                SDL_RenderCopy(renderer, imgPauseButton, NULL, &pauseButtonRect);
            }

            else
            {

                if (executeStep)
                {
                    if(stepOne)
                    { // 다음 명령어 가져오기
                        code = myChip8.fetch();
                        // 다음 명령어를 순차대로 가져온다
                        myChip8.PC += 2;
                    }
                        
                    else
                    {
                        // 가져온 명령어 실행하기
                        myChip8.execute(code);
                        
                    }
                    executeStep = false;
                }
            }
            if (step_mode == CHIP8_STEP_MODE && stepOne)
                getText(renderer, 10, windowHeight - 200 - 25, BLACK, "Instruction Fetched...", font, &text_step_texture, &text_step_rect);
            else if(step_mode == CHIP8_STEP_MODE && !stepOne)
                getText(renderer, 10, windowHeight - 200  - 25, BLACK, "Instruction Executed...", font, &text_step_texture, &text_step_rect);
            else 
                getText(renderer, 10, windowHeight - 200 - 25, BLACK, "Play Mode", font, &text_step_texture, &text_step_rect);
            SDL_RenderCopy(renderer, text_step_texture, NULL, &text_step_rect);
            SDL_DestroyTexture(text_step_texture);
            getText(renderer, 10, windowHeight - 200, BLACK, "=======================================================", font, &text_line_texture, &text_line_rect);
            SDL_RenderCopy(renderer, text_line_texture, NULL, &text_line_rect);
            SDL_DestroyTexture(text_line_texture);
            ss << std::setfill('0') << std::setw(4) << std::hex << code;
            s = ss.str();
            ss.str("");
            getText(renderer, 10, windowHeight - 200 + 25 , BLACK, "Machine Code: " + s, font, &text_inst_texture, &text_inst_rect);
            SDL_RenderCopy(renderer, text_inst_texture, NULL, &text_inst_rect);
            SDL_DestroyTexture(text_inst_texture);
            getText(renderer, 10, windowHeight - 200 + 25 * 2, BLACK, "Assembly Code: " + std::string(myChip8.decode(code)), font, &text_inst_texture, &text_inst_rect);
            SDL_RenderCopy(renderer, text_inst_texture, NULL, &text_inst_rect);
            SDL_DestroyTexture(text_inst_texture);

            for (int i = 0; i < 8; i++)
            {
                ss << std::setw(1) << std::hex << i;
                s = "V" + ss.str() + ":";
                ss.str("");
                ss << std::setfill('0') << std::setw(2) << std::hex << (int)myChip8.V[i];
                s += ss.str();
                ss.str("");
                getText(renderer, 10 + 100 * i, windowHeight - 200 + (25 * 3), BLACK, std::string(s), font, &text_general_reg_texture[i], &text_general_reg_rect[i]);
                SDL_RenderCopy(renderer, text_general_reg_texture[i], NULL, &text_general_reg_rect[i]);
                SDL_DestroyTexture(text_general_reg_texture[i]);

            }

            for (int i = 8; i < CHIP8_NUM_GENERAL_REGS; i++)
            {
                ss << std::setw(1) << std::hex << i;
                s = "V" + ss.str() + ":";
                ss.str("");
                ss << std::setfill('0') << std::setw(2) << std::hex << (int)myChip8.V[i];
                s += ss.str();
                ss.str("");
                getText(renderer, 10 + 100 * (i - 8), windowHeight - 200 + (25 * 4), BLACK, std::string(s), font, &text_general_reg_texture[i], &text_general_reg_rect[i]);
                SDL_RenderCopy(renderer, text_general_reg_texture[i], NULL, &text_general_reg_rect[i]);
                SDL_DestroyTexture(text_general_reg_texture[i]);
            }

            ss << std::setfill('0') << std::setw(4) << std::hex << (int)myChip8.I;
            s = ss.str();
            ss.str("");
            getText(renderer, 10, windowHeight - 200 + (25 * 5), BLACK, "I: " + s, font, &text_I_texture, &text_I_rect);
            SDL_RenderCopy(renderer, text_I_texture, NULL, &text_I_rect);
            SDL_DestroyTexture(text_I_texture);

            ss << std::setfill('0') << std::setw(2) << std::hex << (int)myChip8.DT;
            s = ss.str();
            ss.str("");
            getText(renderer, 10 + (50 * 2), windowHeight - 200 + (25 * 5), BLACK, "DT: " + s, font, &text_DT_texture, &text_DT_rect);
            SDL_RenderCopy(renderer, text_DT_texture, NULL, &text_DT_rect);
            SDL_DestroyTexture(text_DT_texture);

            ss << std::setfill('0') << std::setw(2) << std::hex << (int)myChip8.ST;
            s = ss.str();
            ss.str("");
            getText(renderer, 10 + (50 * 4), windowHeight - 200 + (25 * 5), BLACK, "ST: " + s, font, &text_ST_texture, &text_ST_rect);
            SDL_RenderCopy(renderer, text_ST_texture, NULL, &text_ST_rect);
            SDL_DestroyTexture(text_ST_texture);


            ss << std::setfill('0') << std::setw(4) << std::hex << (int)myChip8.PC;
            s = ss.str();
            ss.str("");
            getText(renderer, 10 + (50 * 6), windowHeight - 200 + (25 * 5), BLACK, "PC: " + s, font, &text_PC_texture, &text_PC_rect);
            SDL_RenderCopy(renderer, text_PC_texture, NULL, &text_PC_rect);
            SDL_DestroyTexture(text_PC_texture);


            ss << std::setfill('0') << std::setw(4) << std::hex << (int)myChip8.SP;
            s = ss.str();
            ss.str("");
            getText(renderer, 10 + (50 * 8), windowHeight - 200 + (25 * 5), BLACK, "SP: " + s, font, &text_SP_texture, &text_SP_rect);
            SDL_RenderCopy(renderer, text_SP_texture, NULL, &text_SP_rect);
            SDL_DestroyTexture(text_SP_texture);
            if (step_mode == CHIP8_STEP_MODE)
            {
                SDL_RenderCopy(renderer, imgPlayButton, NULL, &playButtonRect);
            }
            else
            {
                SDL_RenderCopy(renderer, imgPauseButton, NULL, &pauseButtonRect);
            }
            
            SDL_RenderCopy(renderer, imgStepButton, NULL, &stepButtonRect);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}


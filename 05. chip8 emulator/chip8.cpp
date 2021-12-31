#include "chip8.h"
#include <cstring>
#include <iostream>
using namespace std;

void Chip8::loadProgram(const char* data, int len)
{
    memcpy(&this->memory[CHIP8_PROGRAMDATA_ADDR], data, len);
    this->PC = CHIP8_PROGRAMDATA_ADDR;

}
void Chip8::clearDisplay()
{
    //display_pixels[CHIP8_HEIGHT][CHIP8_WIDTH]
    memset(this->display_pixels, 0, sizeof(this->display_pixels));
}

unsigned short Chip8::fetch()
{
    unsigned short temp = 0;

    temp = (unsigned char)this->memory[this->PC] << 8;
    temp = temp | (unsigned char)this->memory[this->PC + 1];
    return temp;
}

void Chip8::stackPush(unsigned short value)
{
    this->SP++;
    this->stack[this->SP] = value;
}
unsigned short Chip8::stackPop()
{
    unsigned short value = this->stack[this->SP];
    this->SP--;
    return value;
}

bool Chip8::drawSprite(const char* ptrSprite, int px, int py, int n)
{   
    bool pixelFlag = false;
    int newX, newY;
    for (int yy = 0; yy < n; yy++)
    {
        //스프라이트 이미지 한 줄을 가져온다
        char row = ptrSprite[yy];
        
        for (int xx = 0; xx < 8; xx++)
        {
            unsigned char checkbit = 0b10000000 >> xx;
            bool pixelOn = row & checkbit;
            //현재 x위치에서 픽셀이 칠해질지 검사
            if (pixelOn)
            {
                //화면 x축 y축 wrapping
                newX = (px + xx) % CHIP8_WIDTH;
                newY = (py + yy) % CHIP8_HEIGHT;
                //화면이 지워졌는지 검사
                if (this->display_pixels[newY][newX])
                {
                    pixelFlag = true;
                }
                this->display_pixels[newY][newX] ^= true;
            }   
        }
    }
    return pixelFlag;
}



bool Chip8::isKeyPressed(int key)
{
    bool result = this->keyboard[key];
    return result;

}
//  SDL에서 입력받은 PC키보드의 키를 Chip8키에 대응시킴 
int Chip8::mapKey(char inputKey)
{
    //눌린키가 chip8키보드일경우만 키 값을 리턴
    //0~9, A-F가 눌리지 않았으면, 예를 들어 t키가 눌렸다면
    // -1을 리턴
    int result = CHIP8_WRONG;
    for (int x = 0; x < CHIP8_NUM_KEYS; x++)
    {
        if (this->keymap[x] == inputKey)
        {
            result = x;
            return x;
        }
    }
    return result;
}

char Chip8::SDLKeyPressed()
{
    SDL_Event event;
    char inKey;
    char outKey;
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_KEYDOWN) {
            inKey = event.key.keysym.sym;
            outKey = mapKey(inKey);
            //눌린 키가 0~9, A~F 키일때 정상 작동
            if (outKey != CHIP8_WRONG) return outKey;
        }
    }
    //눌린 키가 0~9, A~F가 아닌 키일때 포함
    return CHIP8_WRONG;


}

void Chip8::execute8000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
        switch (n)
    {
        //LD Vx, Vy: Vx에 Vy값을 입력한다
        case 0x0:
            this->V[x] = this->V[y];
            break;
        //OR Vx, Vy: Vx와 Vy를 OR연산하여 Vx에 넣는다
        case 0x1:
            this->V[x] = this->V[x] | this->V[y];
            break;
        //AND Vx, Vy: Vx와 Vy를 AND연산하여 Vx에 넣는다
        case 0x2:
            this->V[x] = this->V[x] & this->V[y];
            break;
         //XOR Vx, Vy: Vx와 Vy를 XOR연산하여 Vx에 넣는다
        case 0x3:
            this->V[x] = this->V[x] ^ this->V[y];
            break;
        //ADD Vx, Vy: Vx와 Vy를 ADD(덧셈)연산하여 Vx에 넣는다
        // 덧셈의 결과가 255보다 크면 
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
        case 0x4:
            this->TMP = this->V[x] + this->V[y];
            if (this->TMP > 255)
            {
                this->V[0xf] = true;
            }
            else
            {
                this->V[0xf] = false;
            }
            this->V[x] = this->TMP;
            break;
            //SUB Vx, Vy: Vx = Vx - Vy
            // 뺄셈의 결과가 0보다 크면, 즉 Vx > Vy라면   
            //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
        case 0x5:
            this->TMP = this->V[x] - this->V[y];
            if (this->V[x] > this->V[y])
            {
                this->V[0xf] = true;
            }
            else
            {
                this->V[0xf] = false;
            }
            this->V[x] = this->TMP;
            break;
            //SHR Vx, Vy: Vx의 마지막 이진수 비트가 1이라면 (자리 내림이 발생했다면)
            // Vf를 1로 만들고, 아니라면 0으로 만든다.
            //그리고 Vx를 2로 나눠서 Vx에 저장한다
        case 0x6:
            if (this->V[x] & 0b00000001)
            {
                this->V[0xf] = true;
            }
            else
            {
                this->V[0xf] = false;
            }
            this->V[x] /= 2;
            break;
            //SUBN Vx, Vy: Vx = Vy - Vx
            // 뺄셈의 결과가 0보다 크면, 즉 Vy > Vx라면   
            //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
        case 0x7:
            this->TMP = this->V[y] - this->V[x];
            if (this->V[y] > this->V[x])
            {
                this->V[0xf] = true;
            }
            else
            {
                this->V[0xf] =false;
            }
            this->V[x] = this->TMP;
            break;
            //SHL Vx, Vy: Vx의 가장 상위 비트가 1이라면 (자리 올림이 발생했다면)
            // Vf를 1로 만들고, 아니라면 0으로 만든다.
            //그리고 Vx를 2로 곱해서 Vx에 저장한다
        case 0xE:
            if (this->V[x] & 0b10000000)
            {
                this->V[0xf] = true;
            }
            else
            {
                this->V[0xf] = false;
            }
            this->V[x] *= 2;
            break;
    }
}
void Chip8::executeE000(unsigned short _code)
{

         
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8)  & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = (_code) & 0x00ff;

    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //SKP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 눌렸다면 다음 명령어를 건너뛴다
        case 0x9E:
            if (isKeyPressed(this->V[x]))
                this->PC += 2;
            break;
        //SKNP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 안눌렸다면(키보드를 뗐다면)  다음 명령어를 건너뛴다
        case 0xA1:
            if (!isKeyPressed(this->V[x]))
                this->PC += 2;
            break;
    }
}
void Chip8::executeF000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = _code & 0x00ff;
    char K;
    unsigned char hundreds_digit, tens_digit, ones_digit;
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //LD Vx, DT: 딜레이 타이머 값을 넣는다
        case 0x07:
            this->V[x] = this->DT;
            break;
        //LD Vx, K: 키 입력을 기다렸다가, 키가 입력되면
        //입력된 키값(K)를 Vx에 넣는다
        case 0x0A:
            K  = SDLKeyPressed();
            this->V[x] = K;
            break;
        //LD DT, Vx: Vx의 값을 딜레이 타이머로 설정한다
        case 0x15:
            this->DT = this->V[x];
            break;
         //LD ST, Vx: Vx의 값을 사운드 타이머로 설정한다
        case 0x18:
            this->ST = this->V[x];
            break;
        // ADD I, Vx, I와 Vx값을 더해 I에 입력한다
        case 0x1E:
            this->I += this->V[x];
            break;
        // LD F, Vx: Vx에 있는 스프라이트 번호 x 스프라이트크기 -> I에 주소로서 넣어준다
        case 0x29:
            this->I = this->V[x] * CHIP8_SPRITE_SIZE;
            break;
            // Vx에 들어있는 주소를 BCD코드형태로 4자리 십진수로변환해
            // I의 각 니블을 memory주소에 각각 넣어준다.
            // I에는 백의자리 니블값을
            // I+1에는 십의자리 니블값을
            // I+2에는 일의자리 니블값을
            //ex) Vx = 0x7F -> 7x16 + 15 =127(십진수)-->
            // I_0 = 7, I_1 = 2, I_2 = 1, I_3 = 0; BCD코드 (십진수 자리수마다 별도로 넣어줌)
        case 0x33:
            hundreds_digit = this->V[x] / 100; // 1
            tens_digit = this->V[x] / 100 % 10;
            ones_digit = this->V[x] % 10;
            //cout << hundreds_digit << " " << tens_digit << " " << ones_digit << endl;
            this->memory[this->I] = hundreds_digit;
            this->memory[this->I + 1] = tens_digit;
            this->memory[this->I + 2] = ones_digit;
            break;
        //LD [I], Vx: 메모리 시작위치(I에 저장된)로부터 V0부터 Vx까지 레지스터값을
        //차례로 메모리에 순차적으로 저장한다
        case 0x55:
            for (int i = 0; i <= x; i++)
            {
                this->memory[this->I + i] = this->V[i];
            }
            break;
        //LD Vx, [I], Vx: I레지스터에 있는 메모리 주소를 참조하여, 
        // 그 주소에 저장된 값을 읽어 Vx에 입력한다
        case 0x65:
            for (int i = 0; i <= x; i++)
            {
                this->V[i] = this->memory[this->I + i];
            }
            break;


    }
}

void Chip8::executeGeneral(unsigned short _code)
{
    unsigned char nibble3 = _code >> 12;
    unsigned short nnn = _code & 0x0fff;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char kk = _code  & 0x00ff;
    unsigned char nibble0 = _code & 0x000f;
    
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nibble3)
    {
        //JP: PC값을 바꿔줌
        case 0x1:
            this->PC = nnn;
            break;
        //CALL: nnn에 있는 서브루틴으로 분기
        case 0x2:
            stackPush(this->PC);
            this->PC = nnn;
            break;
        //SE: Vx == kk라면 다음 명령어를 건너뛴다(조건분기)
        case 0x3:
            if (this->V[x] == kk)
            {
                this->PC += 2;
            }
            break;
        //SNE: Vx != kk라면 다음 명령어를 건너뛴다(조건분기)
        case 0x4:
            if (this->V[x] != kk)
            {
                this->PC += 2;
            }
            break;
        //SEV: Vx == Vy라면 다음 명령어를 건너뛴다(조건분기)
        case 0x5:
            if (this->V[x] == this->V[y])
            {
                this->PC += 2;
            }
            break;
        //LD: Vx에 kk값을 입력한다
        case 0x6:
            this->V[x] = kk;
            break;
        //ADD: Vx에 들어있는 값에 kk를 더한다.
        case 0x7:
            this->V[x] += kk;
            break;
        case 0x8:
            execute8000(_code);
            break;
        //SNEV: Vx != Vy라면 다음 명령어를 건너뛴다(조건분기)
        case 0x9:
            if (this->V[x] != this->V[y])
            {
                this->PC += 2;
            }
            break;
        //LDI: I레지스터에 nnn값을 입력한다
        case 0xA:
            this->I = nnn;
            break;
        //JPn: V0레지스터의 값과 nnn을 더한 주소로 분기한다
        case 0xB:
            this->PC = this->V[0] + nnn;
            break;
        //RND: 무작위로 바이트값을 생성한 후 kk와 AND연산한 값을
        // Vx 레지스터에 넣는다
        case 0xC:
            srand(time(NULL));
            //this->V[x] = (rand() % 256) & kk;
            this->V[x] =  111 & kk;
            break;
        //DRW: I레지스터에 있는 메모리주소에서 n바이트만큼
            // 스프라이트를 읽어서 (Vx, Vy) 픽셀 위치에서 그린다
        case 0xD:
            //지워진 픽셀이 하나라도 있다면
            //Vf레지스터에 플래그값 1을 저장하고,
            //아니라면 플래그값 0을 저장한다.
            this->V[0xf] = drawSprite((const char*)&this->memory[this->I], this->V[x], this->V[y], nibble0);
            break;
        case 0xE:
            executeE000(_code);
            break;
        case 0xF:
            executeF000(_code);
            break;

    }
}
void Chip8::execute(unsigned short _code)
{
    switch (_code)
    {
        //CLS: 디스플레이를 지운다
    case 0x00E0:
        clearDisplay();
        break;
        //RET: 스택을 참조하여 다음 명령어가 실행될 주소로 건너뛴다
    case 0x00EE:
        this->PC = stackPop();
        break;
    default:
        executeGeneral(_code);

    }

}

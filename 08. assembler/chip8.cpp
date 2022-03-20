#include "chip8.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <map>

void Chip8::loadProgram()
{
    memcpy(&this->memory[CHIP8_PROGRAMDATA_ADDR], this->romData, this->romSize);
    this->PC = CHIP8_PROGRAMDATA_ADDR;

}
void Chip8::clearDisplay()
{
    //display_pixels[CHIP8_HEIGHT][CHIP8_WIDTH]
    memset(this->display_pixels, 0, sizeof(this->display_pixels));
}


int hexSingleChar2Num(std::string hexStr)
{
    int ascii_val = int(*hexStr.c_str());
    if (ascii_val >= int(*"0") && ascii_val <= int(*"9"))
    {
        return ascii_val - int(*"0");
    }
    else if (ascii_val >= int(*"A") && ascii_val <= int(*"F"))
    {
        return 10 + ascii_val - int(*"A");
    }
    else
    {
        return -1;
    }
}
int hexStr2Num(std::string hexStr)
{
    int len = hexStr.size();
    int res = 0;
    for (int i = 0; i < len; i++)
    {
        int num = hexSingleChar2Num(hexStr.substr(i, 1));
        if (num == -1)
        {
            return -1;
        }
        res = (res << 4) + num;
    }
    return res;

}






short encode(std::vector<std::string>& token)
{

    int size = token.size();
    std::string v[4];
    int op, x, y, n, kk, nnn, nnnn;
    bool isvalid;
    for (int i = 0; i < size; i++)
    {
        // 대문자로
        std::transform(token.at(i).begin(), token.at(i).end(), token.at(i).begin(), ::toupper);
        v[i] = token.at(i);
        
        
    }
    if (size == 1)
    {
        if (v[0].compare("CLS") == 0)
            return 0x00E0;
        else if (v[0].compare("RET") == 0)
            return 0x00EE;
        else
            return -1;
    }
    else if (size == 2)
    {
        // SYS ignored


        if (v[0].compare(".DATA") == 0 && hexStr2Num(v[1]) != -1)
        {
            nnnn = hexStr2Num(v[1]);
            return nnnn;
        }
        else if (v[0].compare("JP") == 0 && hexStr2Num(v[1]) != -1)
        {
            nnn = hexStr2Num(v[1]);
            return 0x1000 | nnn;
        }
        else if (v[0].compare("CALL") == 0 && hexStr2Num(v[1]) != -1)
        {
            nnn = hexStr2Num(v[1]);
            return 0x2000 | nnn;
        }
        else if (v[0].compare("SKP") == 0 && v[1].substr(0, 1).compare("V") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            return 0xE09E | x;
        }
        else if (v[0].compare("SKNP") == 0 && v[1].substr(0, 1).compare("V") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            return 0xE0A1 | x;
        }
    }
    else if (size == 3)
    {
        // 명령어 Vx Vy
        if (v[1].substr(0, 1).compare("V") == 0 && v[2].substr(0, 1).compare("V") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            y = hexStr2Num(v[2].substr(1, 1));
            if (v[0].compare("SE") == 0)
                op = 0x5000;
            else if (v[0].compare("SNE") == 0)
                op = 0x9000;
            else if (v[0].compare("LD") == 0)
                op = 0x8000;
            else if (v[0].compare("OR") == 0)
                op = 0x8001;
            else if (v[0].compare("AND") == 0)
                op = 0x8002;
            else if (v[0].compare("XOR") == 0)
                op = 0x8003;
            else if (v[0].compare("ADD") == 0)
                op = 0x8004;
            else if (v[0].compare("SUB") == 0)
                op = 0x8005;
            else if (v[0].compare("SHR") == 0)
                op = 0x8006;
            else if (v[0].compare("SUBN") == 0)
                op = 0x8007;
            else if (v[0].compare("SHL") == 0)
                op = 0x800E;

            return op | (x << 8) | (y << 4);
        }
        // SE Vx Byte
        else if (v[0].compare("SE") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && hexStr2Num(v[2]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            kk = hexStr2Num(v[2].c_str());
            return 0x3000 | x | kk;
        }
        // SNE Vx Byte
        else if (v[0].compare("SNE") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && hexStr2Num(v[2]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            kk = hexStr2Num(v[2]);
            return 0x4000 | x | kk;
        }
        // LD Vx Byte
        else if (v[0].compare("LD") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && hexStr2Num(v[2]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            kk = hexStr2Num(v[2]);
            return 0x6000 | x | kk;
        }
        // ADD Vx Byte
        else if (v[0].compare("ADD") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && hexStr2Num(v[2]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            kk = hexStr2Num(v[2]);
            return 0x7000 | x | kk;
        }
        // RND Vx Byte
        else if (v[0].compare("RND") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && hexStr2Num(v[2]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            kk = hexStr2Num(v[2]);
            return 0xC000 | x | kk;
        }
        // LD I addr
        else if (v[0].compare("LD") == 0 && v[1].compare("I") == 0 && hexStr2Num(v[2]) != -1)
        {
            nnn = hexStr2Num(v[2]);
            return 0xA000 | nnn;
        }
        // LD Vx DT
        else if (v[0].compare("LD") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && v[2].compare("DT") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            return 0xF007 | x;
        }
        // LD Vx K
        else if (v[0].compare("LD") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2]) != -1 && v[2].compare("K") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            return 0xF00A | x;
        }
        // LD DT Vx
        else if (v[0].compare("LD") == 0 && v[1].compare("DT") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[2].substr(1, 1));
            x = x << 8;
            return 0xF015 | x;
        }
        // LD ST Vx
        else if (v[0].compare("LD") == 0 && v[1].compare("ST") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[2].substr(1, 1));
            x = x << 8;
            return 0xF018 | x;
        }
        // ADD I Vx
        else if (v[0].compare("ADD") == 0 && v[1].compare("I") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[2].substr(1, 1));
            x = x << 8;
            return 0xF01E | x;
        }
        // LD F Vx
        else if (v[0].compare("LD") == 0 && v[1].compare("F") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[2].substr(1, 1));
            x = x << 8;
            return 0xF029 | x;
        }
        // LD B Vx
        else if (v[0].compare("LD") == 0 && v[1].compare("B") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[2].substr(1, 1));
            x = x << 8;
            return 0xF033 | x;
        }
        // LD [I] Vx
        else if (v[0].compare("LD") == 0 && v[1].compare("[I]") == 0 && v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1).c_str());
            x = x << 8;
            return 0xF055 | x;
        }
        // LD Vx [I]
        else if (v[0].compare("LD") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 && v[2].compare("[I]") == 0)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            return 0xF065 | x;
        }
    }
    else if (size == 4)
    {
        if (v[0].compare("DRW") == 0 && v[1].substr(0, 1).compare("V") == 0 && hexStr2Num(v[1].substr(1, 1)) != -1 &&
            v[2].substr(0, 1).compare("V") == 0 && hexStr2Num(v[2].substr(1, 1)) != -1 && hexStr2Num(v[3]) != -1)
        {
            x = hexStr2Num(v[1].substr(1, 1));
            x = x << 8;
            y = hexStr2Num(v[2].substr(1, 1));
            y = y << 4;
            n = hexStr2Num(v[3]);
            return 0xD000 | x | y | n;
        }
    }
    return -1;
}

bool Chip8::assemble(std::vector<std::string>& vec)
{


    //for(int i=0; i<vec.size(); i++)
    //    std::cout << vec.at(i) << std::endl;
    std::string temp;
    std::vector <std::vector<std::string>> tokens;
    std::vector <int> assembly_lines;
    // 프리 프로세싱 단계
    int count = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        temp = vec.at(i);
        std::transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
        while (temp.find("\t") != -1)
        {
            temp.replace(temp.find("\t"), 1, " ");
        }
        if (temp.find(";") != -1)
        {
            temp.replace(temp.find(";"), temp.length(), "");
        }

        std::stringstream ssline(temp);
        std::string str;
        std::vector<std::string> token;

        std::string temp_no_blanks = temp;
        temp_no_blanks.erase(std::remove_if(temp_no_blanks.begin(), temp_no_blanks.end(), ::isspace), temp_no_blanks.end());

        if (temp_no_blanks.size() != 0)
        {
            while (std::getline(ssline, str, ' '))
            {
                if (str.size() != 0)
                {
                    //std::cout << str << std::endl;
                    token.push_back(str);
                }

            }
            tokens.push_back(token);
        }
    }


    //메모리 시작위치를 찾는다
    int begin_addr = 0;
    int tksize = 0;
    int lnum = -1;
    for (int i = 0; i < tokens.size(); i++)
    {
        tksize = tokens.at(i).size();
        if (tksize == 2)
        {
            std::string tok1 = tokens.at(i).at(0);
            std::string tok2 = tokens.at(i).at(1);
            if (tok1.compare(".ORG") == 0 && hexStr2Num(tok2))
            {
                bool valid;
                lnum = i;
                begin_addr = hexStr2Num(tok2);
                break;

            }
        }
    }
    if (lnum >=0) tokens.erase(tokens.begin() + lnum);

    //주소 라벨을 찾는다
    short code = 0;
    this->romData = new char[1024];
    this->romSize = tokens.size() * 2;

    std::string single_token = "";
    std::size_t found;
    std::map<std::string, int> label_map;
    int addr = begin_addr;
    std::vector<int> line_to_delete;
    for (int i = 0; i < tokens.size(); i++)
    {
        tksize = tokens.at(i).size();
        if (tksize == 1)
        {
            //한개짜리 토큰의 행에서 라벨을 찾는다
            single_token = tokens.at(i).at(0);
            std::size_t pos = single_token.find_last_of(":");
            if (pos == single_token.size() - 1)
            {
                single_token.replace(single_token.find(":"), 1, "");
                std::cout << single_token << std::endl;
                line_to_delete.push_back(i);
                label_map.insert({ "@" + single_token, addr });
                addr -= 2;
            }
        }
        addr += 2;
    }

    std::vector<int>::iterator iter;
    int prevLine = tokens.size() + 1;
    for (int k = 0; k < line_to_delete.size(); k++)
    {
        int num_line = line_to_delete.at(k);
        if (prevLine < num_line)
            num_line -= k;
        tokens.erase(tokens.begin() + num_line);
        prevLine = num_line;
    }

    // 주소 라벨을 모두 16진수로 치환한다
    for (int i = 0; i < tokens.size(); i++)
    {
        tksize = tokens.at(i).size();
        for (int k = 0; k < tksize; k++)
        {
            if (label_map.find(tokens.at(i).at(k)) != label_map.end())
            {
                std::stringstream stream;
                stream << std::hex << label_map.find(tokens.at(i).at(k))->second;
                std::string result(stream.str());
                tokens.at(i).at(k) = result;
            }
        }
    }


    for (int i = 0; i < tokens.size(); i++)
    {
        tksize = tokens.at(i).size();
        std::cout << std::hex << begin_addr + i * 2;
        for (int k = 0; k < tksize; k++)
        {
            std::cout << " " << tokens.at(i).at(k);
        }
        std::cout << std::endl;
    }

    this->romSize = tokens.size() * 2;
    for (int i = 0; i < tokens.size(); i++)
    {
        code = encode(tokens.at(i));
        if (code == -1)
        {
            return false;
        }
        this->romData[i * 2] = code >> 8;
        this->romData[i * 2 + 1] = code & 0x00ff;
    }
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
        if (this->V[x] + this->V[y] > 255)
        {
            this->V[0xf] = 1;
        }
        else
        {
            this->V[0xf] = 0;
        }
        this->V[x] = this->V[x] + this->V[y];
        break;
        //SUB Vx, Vy: Vx = Vx - Vy
        // 뺄셈의 결과가 0보다 크면, 즉 Vx > Vy라면   
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
    case 0x5:

        if (this->V[x] > this->V[y])
        {
            this->V[0xf] = 1;
        }
        else
        {
            this->V[0xf] = 0;
        }
        this->V[x] = this->V[x] - this->V[y];
        break;
        //SHR Vx, Vy: Vx의 마지막 이진수 비트가 1이라면 (자리 내림이 발생했다면)
        // Vf를 1로 만들고, 아니라면 0으로 만든다.
        //그리고 Vx를 2로 나눠서 Vx에 저장한다
    case 0x6:
        if (this->V[x] & 0b00000001)
        {
            this->V[0xf] = 1;
        }
        else
        {
            this->V[0xf] = 0;
        }
        this->V[x] /= 2;
        break;
        //SUBN Vx, Vy: Vx = Vy - Vx
        // 뺄셈의 결과가 0보다 크면, 즉 Vy > Vx라면   
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
    case 0x7:
        if (this->V[y] > this->V[x])
        {
            this->V[0xf] = 1;
        }
        else
        {
            this->V[0xf] = 0;
        }
        this->V[x] = this->V[y] - this->V[x];
        break;
        //SHL Vx, Vy: Vx의 가장 상위 비트가 1이라면 (자리 올림이 발생했다면)
        // Vf를 1로 만들고, 아니라면 0으로 만든다.
        //그리고 Vx를 2로 곱해서 Vx에 저장한다
    case 0xE:
        if (this->V[x] & 0b10000000)
        {
            this->V[0xf] = 1;
        }
        else
        {
            this->V[0xf] = 0;
        }
        this->V[x] *= 2;
        break;
    }
}
void Chip8::executeE000(unsigned short _code)
{


    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
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

//0000 0010 //+2
//1111 1101 //1의 보수
//1111 1110 //2의 보수 FE = -2

void Chip8::executeF000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = _code & 0x00ff;
    char K;
    int temp;
    int  hundreds_digit, tens_digit, ones_digit;
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
        K = SDLKeyPressed();
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
        temp = (int)this->V[x];
        hundreds_digit = temp / 100; // 1
        tens_digit = (temp - hundreds_digit * 100) / 10;
        ones_digit = temp - hundreds_digit * 100 - tens_digit * 10;
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
    unsigned char kk = _code & 0x00ff;
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
        this->V[x] = (rand() % 256) & kk;
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

std::string Chip8::decode8000(unsigned short _code)
{

    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    std::stringstream ss;
    std::string s;
    std::string s2;
    switch (n)
    {
        //LD Vx, Vy: Vx에 Vy값을 입력한다
    case 0x0:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tLD\tV" + s + " V" + s2;
        //OR Vx, Vy: Vx와 Vy를 OR연산하여 Vx에 넣는다
    case 0x1:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tOR\tV" + s + " V" + s2;
        //AND Vx, Vy: Vx와 Vy를 AND연산하여 Vx에 넣는다
    case 0x2:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tAND\tV" + s + " V" + s2;
        //XOR Vx, Vy: Vx와 Vy를 XOR연산하여 Vx에 넣는다
    case 0x3:
        //예를 들어: 기계어가 0x8a43
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tXOR\tV" + s + " V" + s2;
        //ADD Vx, Vy: Vx와 Vy를 ADD(덧셈)연산하여 Vx에 넣는다
        // 덧셈의 결과가 255보다 크면 
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
    case 0x4:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tADD\tV" + s + " V" + s2;
    case 0x5:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSUB\tV" + s + " V" + s2;
        //SHR Vx, Vy: Vx의 마지막 이진수 비트가 1이라면 (자리 내림이 발생했다면)
        // Vf를 1로 만들고, 아니라면 0으로 만든다.
        //그리고 Vx를 2로 나눠서 Vx에 저장한다
    case 0x6:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\SHR\tV" + s + " V" + s2;
        break;
        //SUBN Vx, Vy: Vx = Vy - Vx
        // 뺄셈의 결과가 0보다 크면, 즉 Vy > Vx라면   
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
    case 0x7:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSUBN\tV" + s + " V" + s2;
        //SHL Vx, Vy: Vx의 가장 상위 비트가 1이라면 (자리 올림이 발생했다면)
        // Vf를 1로 만들고, 아니라면 0으로 만든다.
        //그리고 Vx를 2로 곱해서 Vx에 저장한다
    case 0xE:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSHL\tV" + s + " V" + s2;
    }
}

std::string Chip8::decodeE000(unsigned short _code)
{


    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = (_code) & 0x00ff;
    std::stringstream ss;
    std::string s;
    std::string s2;
    std::string res;
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //SKP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 눌렸다면 다음 명령어를 건너뛴다
    case 0x9E:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        return "\tSKP\tV" + s;
        //SKNP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 안눌렸다면(키보드를 뗐다면)  다음 명령어를 건너뛴다
    case 0xA1:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        return "\tSKNP\tV" + s;
    }
}

std::string Chip8::decodeF000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = _code & 0x00ff;
    char K;
    unsigned char hundreds_digit, tens_digit, ones_digit;
    std::stringstream ss;
    std::string s;
    std::string s2;
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //LD Vx, DT: 딜레이 타이머 값을 넣는다
    case 0x07:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tV" + s + " DT";
        //LD Vx, K: 키 입력을 기다렸다가, 키가 입력되면
        //입력된 키값(K)를 Vx에 넣는다
    case 0x0A:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tV K";
        //LD DT, Vx: Vx의 값을 딜레이 타이머로 설정한다
    case 0x15:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tDT V" + s;
        //LD ST, Vx: Vx의 값을 사운드 타이머로 설정한다
    case 0x18:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tST V" + s;
        // ADD I, Vx, I와 Vx값을 더해 I에 입력한다
    case 0x1E:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tADD\tI V" + s;
        // LD F, Vx: Vx에 있는 스프라이트 번호 x 스프라이트크기 -> I에 주소로서 넣어준다
    case 0x29:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tF V" + s;
        // Vx에 들어있는 주소를 BCD코드형태로 4자리 십진수로변환해
        // I의 각 니블을 memory주소에 각각 넣어준다.
        // I에는 백의자리 니블값을
        // I+1에는 십의자리 니블값을
        // I+2에는 일의자리 니블값을
        //ex) Vx = 0x7F -> 7x16 + 15 =127(십진수)-->
        // I_0 = 7, I_1 = 2, I_2 = 1, I_3 = 0; BCD코드 (십진수 자리수마다 별도로 넣어줌)
    case 0x33:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tB V" + s;
        //LD [I], Vx: 메모리 시작위치(I에 저장된)로부터 V0부터 Vx까지 레지스터값을
        //차례로 메모리에 순차적으로 저장한다
    case 0x55:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\LD\t[I] V" + s;
        //LD Vx, [I]: I레지스터에 있는 메모리 주소를 참조하여, 
        // 그 주소에 저장된 값을 읽어 Vx에 입력한다
    case 0x65:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tV" + s + " [I]";

    }
}


std::string Chip8::decodeGeneral(unsigned short _code)
{
    unsigned char nibble3 = _code >> 12;
    unsigned short nnn = _code & 0x0fff;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char kk = _code & 0x00ff;
    unsigned char n = _code & 0x000f;
    std::stringstream ss;
    std::string s;
    std::string s2;
    std::string s3;
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nibble3)
    {
        //JP: PC값을 바꿔줌
    case 0x1:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tJP\t" + s;
        //CALL: nnn에 있는 서브루틴으로 분기
    case 0x2:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tCALL\t" + s;
        //SE: Vx == kk라면 다음 명령어를 건너뛴다(조건분기)
    case 0x3:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSE\tV" + s + " " + s2;
        //SNE: Vx != kk라면 다음 명령어를 건너뛴다(조건분기)
    case 0x4:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSNE\tV" + s + " " + s2;
        //SEV: Vx == Vy라면 다음 명령어를 건너뛴다(조건분기)
    case 0x5:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSE\tV" + s + " V" + s2;
        //LD: Vx에 kk값을 입력한다
    case 0x6:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str(""); 
        return "\tLD\tV" + s + " " + s2;
    case 0x7:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tADD\tV" + s + " " + s2;
    case 0x8:
        return decode8000(_code);
        //SNEV: Vx != Vy라면 다음 명령어를 건너뛴다(조건분기)
    case 0x9:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSNE\tV" + s + " V" + s2;
        //LDI: I레지스터에 nnn값을 입력한다
    case 0xA:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tLD\tI " + s;
        //JPn: V0레지스터의 값과 nnn을 더한 주소로 분기한다
    case 0xB:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tJP\tV0 " + s;
    case 0xC:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tRND\tV" + s + " " + s2;
        //DRW: I레지스터에 있는 메모리주소에서 n바이트만큼
            // 스프라이트를 읽어서 (Vx, Vy) 픽셀 위치에서 그린다
    case 0xD:
        //지워진 픽셀이 하나라도 있다면
        //Vf레지스터에 플래그값 1을 저장하고,
        //아니라면 플래그값 0을 저장한다.
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(n);
        s3 = ss.str();
        ss.str("");
        return"\tDRW\tV" + s + " V" + s2 + " " + s3;
    case 0xE:
        return decodeE000(_code);
    case 0xF:
        return decodeF000(_code);
    }
}
std::string Chip8::decode(unsigned short _code)
{

    switch (_code)
    {
        //CLS: 디스플레이를 지운다
    case 0x00E0:
        return "\tCLS";
        //RET: 스택을 참조하여 다음 명령어가 실행될 주소로 건너뛴다
    case 0x00EE:
        return "\tRET";
    default:
        return decodeGeneral(_code);
    }
}






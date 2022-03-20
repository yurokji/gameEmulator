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
        // �빮�ڷ�
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
        // ��ɾ� Vx Vy
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
    // ���� ���μ��� �ܰ�
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


    //�޸� ������ġ�� ã�´�
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

    //�ּ� ���� ã�´�
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
            //�Ѱ�¥�� ��ū�� �࿡�� ���� ã�´�
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

    // �ּ� ���� ��� 16������ ġȯ�Ѵ�
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
        //��������Ʈ �̹��� �� ���� �����´�
        char row = ptrSprite[yy];

        for (int xx = 0; xx < 8; xx++)
        {
            unsigned char checkbit = 0b10000000 >> xx;
            bool pixelOn = row & checkbit;
            //���� x��ġ���� �ȼ��� ĥ������ �˻�
            if (pixelOn)
            {
                //ȭ�� x�� y�� wrapping
                newX = (px + xx) % CHIP8_WIDTH;
                newY = (py + yy) % CHIP8_HEIGHT;
                //ȭ���� ���������� �˻�
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
//  SDL���� �Է¹��� PCŰ������ Ű�� Chip8Ű�� ������Ŵ 
int Chip8::mapKey(char inputKey)
{
    //����Ű�� chip8Ű�����ϰ�츸 Ű ���� ����
    //0~9, A-F�� ������ �ʾ�����, ���� ��� tŰ�� ���ȴٸ�
    // -1�� ����
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
            //���� Ű�� 0~9, A~F Ű�϶� ���� �۵�
            if (outKey != CHIP8_WRONG) return outKey;
        }
    }
    //���� Ű�� 0~9, A~F�� �ƴ� Ű�϶� ����
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
        //LD Vx, Vy: Vx�� Vy���� �Է��Ѵ�
    case 0x0:
        this->V[x] = this->V[y];
        break;
        //OR Vx, Vy: Vx�� Vy�� OR�����Ͽ� Vx�� �ִ´�
    case 0x1:
        this->V[x] = this->V[x] | this->V[y];
        break;
        //AND Vx, Vy: Vx�� Vy�� AND�����Ͽ� Vx�� �ִ´�
    case 0x2:
        this->V[x] = this->V[x] & this->V[y];
        break;
        //XOR Vx, Vy: Vx�� Vy�� XOR�����Ͽ� Vx�� �ִ´�
    case 0x3:
        this->V[x] = this->V[x] ^ this->V[y];
        break;
        //ADD Vx, Vy: Vx�� Vy�� ADD(����)�����Ͽ� Vx�� �ִ´�
        // ������ ����� 255���� ũ�� 
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
        // ������ ����� 0���� ũ��, �� Vx > Vy���   
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
        //SHR Vx, Vy: Vx�� ������ ������ ��Ʈ�� 1�̶�� (�ڸ� ������ �߻��ߴٸ�)
        // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
        //�׸��� Vx�� 2�� ������ Vx�� �����Ѵ�
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
        // ������ ����� 0���� ũ��, �� Vy > Vx���   
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
        //SHL Vx, Vy: Vx�� ���� ���� ��Ʈ�� 1�̶�� (�ڸ� �ø��� �߻��ߴٸ�)
        // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
        //�׸��� Vx�� 2�� ���ؼ� Vx�� �����Ѵ�
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

    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //SKP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� ���ȴٸ� ���� ��ɾ �ǳʶڴ�
    case 0x9E:
        if (isKeyPressed(this->V[x]))
            this->PC += 2;
        break;
        //SKNP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� �ȴ��ȴٸ�(Ű���带 �ôٸ�)  ���� ��ɾ �ǳʶڴ�
    case 0xA1:
        if (!isKeyPressed(this->V[x]))
            this->PC += 2;
        break;
    }
}

//0000 0010 //+2
//1111 1101 //1�� ����
//1111 1110 //2�� ���� FE = -2

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
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //LD Vx, DT: ������ Ÿ�̸� ���� �ִ´�
    case 0x07:
        this->V[x] = this->DT;
        break;
        //LD Vx, K: Ű �Է��� ��ٷȴٰ�, Ű�� �ԷµǸ�
        //�Էµ� Ű��(K)�� Vx�� �ִ´�
    case 0x0A:
        K = SDLKeyPressed();
        this->V[x] = K;
        break;
        //LD DT, Vx: Vx�� ���� ������ Ÿ�̸ӷ� �����Ѵ�
    case 0x15:
        this->DT = this->V[x];
        break;
        //LD ST, Vx: Vx�� ���� ���� Ÿ�̸ӷ� �����Ѵ�
    case 0x18:
        this->ST = this->V[x];
        break;
        // ADD I, Vx, I�� Vx���� ���� I�� �Է��Ѵ�
    case 0x1E:
        this->I += this->V[x];
        break;
        // LD F, Vx: Vx�� �ִ� ��������Ʈ ��ȣ x ��������Ʈũ�� -> I�� �ּҷμ� �־��ش�
    case 0x29:
        this->I = this->V[x] * CHIP8_SPRITE_SIZE;
        break;
        // Vx�� ����ִ� �ּҸ� BCD�ڵ����·� 4�ڸ� �������κ�ȯ��
        // I�� �� �Ϻ��� memory�ּҿ� ���� �־��ش�.
        // I���� �����ڸ� �Ϻ���
        // I+1���� �����ڸ� �Ϻ���
        // I+2���� �����ڸ� �Ϻ���
        //ex) Vx = 0x7F -> 7x16 + 15 =127(������)-->
        // I_0 = 7, I_1 = 2, I_2 = 1, I_3 = 0; BCD�ڵ� (������ �ڸ������� ������ �־���)
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
        //LD [I], Vx: �޸� ������ġ(I�� �����)�κ��� V0���� Vx���� �������Ͱ���
        //���ʷ� �޸𸮿� ���������� �����Ѵ�
    case 0x55:
        for (int i = 0; i <= x; i++)
        {
            this->memory[this->I + i] = this->V[i];
        }
        break;
        //LD Vx, [I], Vx: I�������Ϳ� �ִ� �޸� �ּҸ� �����Ͽ�, 
        // �� �ּҿ� ����� ���� �о� Vx�� �Է��Ѵ�
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

    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nibble3)
    {
        //JP: PC���� �ٲ���
    case 0x1:
        this->PC = nnn;
        break;
        //CALL: nnn�� �ִ� �����ƾ���� �б�
    case 0x2:
        stackPush(this->PC);
        this->PC = nnn;
        break;
        //SE: Vx == kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x3:
        if (this->V[x] == kk)
        {
            this->PC += 2;
        }
        break;
        //SNE: Vx != kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x4:
        if (this->V[x] != kk)
        {
            this->PC += 2;
        }
        break;
        //SEV: Vx == Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x5:
        if (this->V[x] == this->V[y])
        {
            this->PC += 2;
        }
        break;
        //LD: Vx�� kk���� �Է��Ѵ�
    case 0x6:
        this->V[x] = kk;
        break;
        //ADD: Vx�� ����ִ� ���� kk�� ���Ѵ�.
    case 0x7:
        this->V[x] += kk;
        break;
    case 0x8:
        execute8000(_code);
        break;
        //SNEV: Vx != Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x9:
        if (this->V[x] != this->V[y])
        {
            this->PC += 2;
        }
        break;
        //LDI: I�������Ϳ� nnn���� �Է��Ѵ�
    case 0xA:
        this->I = nnn;
        break;
        //JPn: V0���������� ���� nnn�� ���� �ּҷ� �б��Ѵ�
    case 0xB:
        this->PC = this->V[0] + nnn;
        break;
        //RND: �������� ����Ʈ���� ������ �� kk�� AND������ ����
        // Vx �������Ϳ� �ִ´�
    case 0xC:
        srand(time(NULL));
        this->V[x] = (rand() % 256) & kk;
        break;
        //DRW: I�������Ϳ� �ִ� �޸��ּҿ��� n����Ʈ��ŭ
            // ��������Ʈ�� �о (Vx, Vy) �ȼ� ��ġ���� �׸���
    case 0xD:
        //������ �ȼ��� �ϳ��� �ִٸ�
        //Vf�������Ϳ� �÷��װ� 1�� �����ϰ�,
        //�ƴ϶�� �÷��װ� 0�� �����Ѵ�.
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
        //CLS: ���÷��̸� �����
    case 0x00E0:
        clearDisplay();
        break;
        //RET: ������ �����Ͽ� ���� ��ɾ ����� �ּҷ� �ǳʶڴ�
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
        //LD Vx, Vy: Vx�� Vy���� �Է��Ѵ�
    case 0x0:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tLD\tV" + s + " V" + s2;
        //OR Vx, Vy: Vx�� Vy�� OR�����Ͽ� Vx�� �ִ´�
    case 0x1:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tOR\tV" + s + " V" + s2;
        //AND Vx, Vy: Vx�� Vy�� AND�����Ͽ� Vx�� �ִ´�
    case 0x2:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tAND\tV" + s + " V" + s2;
        //XOR Vx, Vy: Vx�� Vy�� XOR�����Ͽ� Vx�� �ִ´�
    case 0x3:
        //���� ���: ��� 0x8a43
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tXOR\tV" + s + " V" + s2;
        //ADD Vx, Vy: Vx�� Vy�� ADD(����)�����Ͽ� Vx�� �ִ´�
        // ������ ����� 255���� ũ�� 
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
        //SHR Vx, Vy: Vx�� ������ ������ ��Ʈ�� 1�̶�� (�ڸ� ������ �߻��ߴٸ�)
        // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
        //�׸��� Vx�� 2�� ������ Vx�� �����Ѵ�
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
        // ������ ����� 0���� ũ��, �� Vy > Vx���   
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
    case 0x7:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSUBN\tV" + s + " V" + s2;
        //SHL Vx, Vy: Vx�� ���� ���� ��Ʈ�� 1�̶�� (�ڸ� �ø��� �߻��ߴٸ�)
        // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
        //�׸��� Vx�� 2�� ���ؼ� Vx�� �����Ѵ�
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
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //SKP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� ���ȴٸ� ���� ��ɾ �ǳʶڴ�
    case 0x9E:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        return "\tSKP\tV" + s;
        //SKNP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� �ȴ��ȴٸ�(Ű���带 �ôٸ�)  ���� ��ɾ �ǳʶڴ�
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
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //LD Vx, DT: ������ Ÿ�̸� ���� �ִ´�
    case 0x07:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tV" + s + " DT";
        //LD Vx, K: Ű �Է��� ��ٷȴٰ�, Ű�� �ԷµǸ�
        //�Էµ� Ű��(K)�� Vx�� �ִ´�
    case 0x0A:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tV K";
        //LD DT, Vx: Vx�� ���� ������ Ÿ�̸ӷ� �����Ѵ�
    case 0x15:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tDT V" + s;
        //LD ST, Vx: Vx�� ���� ���� Ÿ�̸ӷ� �����Ѵ�
    case 0x18:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tST V" + s;
        // ADD I, Vx, I�� Vx���� ���� I�� �Է��Ѵ�
    case 0x1E:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tADD\tI V" + s;
        // LD F, Vx: Vx�� �ִ� ��������Ʈ ��ȣ x ��������Ʈũ�� -> I�� �ּҷμ� �־��ش�
    case 0x29:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tF V" + s;
        // Vx�� ����ִ� �ּҸ� BCD�ڵ����·� 4�ڸ� �������κ�ȯ��
        // I�� �� �Ϻ��� memory�ּҿ� ���� �־��ش�.
        // I���� �����ڸ� �Ϻ���
        // I+1���� �����ڸ� �Ϻ���
        // I+2���� �����ڸ� �Ϻ���
        //ex) Vx = 0x7F -> 7x16 + 15 =127(������)-->
        // I_0 = 7, I_1 = 2, I_2 = 1, I_3 = 0; BCD�ڵ� (������ �ڸ������� ������ �־���)
    case 0x33:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\tLD\tB V" + s;
        //LD [I], Vx: �޸� ������ġ(I�� �����)�κ��� V0���� Vx���� �������Ͱ���
        //���ʷ� �޸𸮿� ���������� �����Ѵ�
    case 0x55:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        return "\LD\t[I] V" + s;
        //LD Vx, [I]: I�������Ϳ� �ִ� �޸� �ּҸ� �����Ͽ�, 
        // �� �ּҿ� ����� ���� �о� Vx�� �Է��Ѵ�
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
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nibble3)
    {
        //JP: PC���� �ٲ���
    case 0x1:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tJP\t" + s;
        //CALL: nnn�� �ִ� �����ƾ���� �б�
    case 0x2:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tCALL\t" + s;
        //SE: Vx == kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x3:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSE\tV" + s + " " + s2;
        //SNE: Vx != kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x4:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSNE\tV" + s + " " + s2;
        //SEV: Vx == Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x5:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setfill('0') << std::setw(2) << std::hex << int(kk);
        s2 = ss.str();
        ss.str("");
        return "\tSE\tV" + s + " V" + s2;
        //LD: Vx�� kk���� �Է��Ѵ�
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
        //SNEV: Vx != Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
    case 0x9:
        ss << std::setw(1) << std::hex << int(x);
        s = ss.str();
        ss.str("");
        ss << std::setw(1) << std::hex << int(y);
        s2 = ss.str();
        ss.str("");
        return "\tSNE\tV" + s + " V" + s2;
        //LDI: I�������Ϳ� nnn���� �Է��Ѵ�
    case 0xA:
        ss << std::setfill('0') << std::setw(3) << std::hex << int(nnn);
        s = ss.str();
        ss.str("");
        return "\tLD\tI " + s;
        //JPn: V0���������� ���� nnn�� ���� �ּҷ� �б��Ѵ�
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
        //DRW: I�������Ϳ� �ִ� �޸��ּҿ��� n����Ʈ��ŭ
            // ��������Ʈ�� �о (Vx, Vy) �ȼ� ��ġ���� �׸���
    case 0xD:
        //������ �ȼ��� �ϳ��� �ִٸ�
        //Vf�������Ϳ� �÷��װ� 1�� �����ϰ�,
        //�ƴ϶�� �÷��װ� 0�� �����Ѵ�.
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
        //CLS: ���÷��̸� �����
    case 0x00E0:
        return "\tCLS";
        //RET: ������ �����Ͽ� ���� ��ɾ ����� �ּҷ� �ǳʶڴ�
    case 0x00EE:
        return "\tRET";
    default:
        return decodeGeneral(_code);
    }
}






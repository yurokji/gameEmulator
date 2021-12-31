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
            // ������ ����� 0���� ũ��, �� Vx > Vy���   
            //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
            //SHR Vx, Vy: Vx�� ������ ������ ��Ʈ�� 1�̶�� (�ڸ� ������ �߻��ߴٸ�)
            // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
            //�׸��� Vx�� 2�� ������ Vx�� �����Ѵ�
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
            // ������ ����� 0���� ũ��, �� Vy > Vx���   
            //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
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
            //SHL Vx, Vy: Vx�� ���� ���� ��Ʈ�� 1�̶�� (�ڸ� �ø��� �߻��ߴٸ�)
            // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
            //�׸��� Vx�� 2�� ���ؼ� Vx�� �����Ѵ�
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
void Chip8::executeF000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = _code & 0x00ff;
    char K;
    unsigned char hundreds_digit, tens_digit, ones_digit;
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
            K  = SDLKeyPressed();
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
            hundreds_digit = this->V[x] / 100; // 1
            tens_digit = this->V[x] / 100 % 10;
            ones_digit = this->V[x] % 10;
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
    unsigned char kk = _code  & 0x00ff;
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
            //this->V[x] = (rand() % 256) & kk;
            this->V[x] =  111 & kk;
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

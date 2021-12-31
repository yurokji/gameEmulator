#include "chip8_decode.h"
#include <cstring>
#include <iostream>
using namespace std;



string decode8000(unsigned short _code)
{
    
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    stringstream ss;
    string s;
    string s2;
    switch (n)
    {
        //LD Vx, Vy: Vx�� Vy���� �Է��Ѵ�
        case 0x0:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tLD\tV" + s + " V" + s2;
        //OR Vx, Vy: Vx�� Vy�� OR�����Ͽ� Vx�� �ִ´�
        case 0x1:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tOR\tV" + s + " V" + s2;
        //AND Vx, Vy: Vx�� Vy�� AND�����Ͽ� Vx�� �ִ´�
        case 0x2:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tAND\tV" + s + " V" + s2;
            //XOR Vx, Vy: Vx�� Vy�� XOR�����Ͽ� Vx�� �ִ´�
        case 0x3:
            //���� ���: ��� 0x8a43
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tXOR\tV" + s + " V" + s2;
        //ADD Vx, Vy: Vx�� Vy�� ADD(����)�����Ͽ� Vx�� �ִ´�
        // ������ ����� 255���� ũ�� 
        //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
        case 0x4:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tADD\tV" + s + " V" + s2;
        case 0x5:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSUB\tV" + s + " V" + s2;
            //SHR Vx, Vy: Vx�� ������ ������ ��Ʈ�� 1�̶�� (�ڸ� ������ �߻��ߴٸ�)
            // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
            //�׸��� Vx�� 2�� ������ Vx�� �����Ѵ�
        case 0x6:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\SHR\tV" + s + " V" + s2;
            break;
            //SUBN Vx, Vy: Vx = Vy - Vx
            // ������ ����� 0���� ũ��, �� Vy > Vx���   
            //�÷��� ����� ������ Vf�� 1�� �����, �ƴҰ�� Vf�� 0���� �����
        case 0x7:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSUBN\tV" + s + " V" + s2;
            //SHL Vx, Vy: Vx�� ���� ���� ��Ʈ�� 1�̶�� (�ڸ� �ø��� �߻��ߴٸ�)
            // Vf�� 1�� �����, �ƴ϶�� 0���� �����.
            //�׸��� Vx�� 2�� ���ؼ� Vx�� �����Ѵ�
        case 0xE:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSHL\tV" + s + " V" + s2;
    }
}

string decodeE000(unsigned short _code)
{

         
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8)  & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = (_code) & 0x00ff;
    stringstream ss;
    string s;
    string s2;
    string res;
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //SKP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� ���ȴٸ� ���� ��ɾ �ǳʶڴ�
        case 0x9E:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            return "\tSKP\tV" + s;
        //SKNP Vx: Vx�������Ϳ� ������ Ű���� Ű�� ���ȴ��� Ȯ���ؼ�
        //���� �ȴ��ȴٸ�(Ű���带 �ôٸ�)  ���� ��ɾ �ǳʶڴ�
        case 0xA1:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            return "\tSKNP\tV" + s;
    }
}

string decodeF000(unsigned short _code)
{
    unsigned char nibble3 = (_code >> 12) & 0x000f;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char n = (_code) & 0x000f;
    unsigned char nn = _code & 0x00ff;
    char K;
    unsigned char hundreds_digit, tens_digit, ones_digit;
    stringstream ss;
    string s;
    string s2;
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nn)
    {
        //LD Vx, DT: ������ Ÿ�̸� ���� �ִ´�
        case 0x07:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tV" + s + " DT";
        //LD Vx, K: Ű �Է��� ��ٷȴٰ�, Ű�� �ԷµǸ�
        //�Էµ� Ű��(K)�� Vx�� �ִ´�
        case 0x0A:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tV K";
        //LD DT, Vx: Vx�� ���� ������ Ÿ�̸ӷ� �����Ѵ�
        case 0x15:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tDT V" + s;
         //LD ST, Vx: Vx�� ���� ���� Ÿ�̸ӷ� �����Ѵ�
        case 0x18:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tST V" + s;
        // ADD I, Vx, I�� Vx���� ���� I�� �Է��Ѵ�
        case 0x1E:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tADD\tI V" + s;
        // LD F, Vx: Vx�� �ִ� ��������Ʈ ��ȣ x ��������Ʈũ�� -> I�� �ּҷμ� �־��ش�
        case 0x29:
            ss << setw(1) << hex << int(x);
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
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tB V" + s;
        //LD [I], Vx: �޸� ������ġ(I�� �����)�κ��� V0���� Vx���� �������Ͱ���
        //���ʷ� �޸𸮿� ���������� �����Ѵ�
        case 0x55:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\LD\t[I] V" + s;
        //LD Vx, [I], Vx: I�������Ϳ� �ִ� �޸� �ּҸ� �����Ͽ�, 
        // �� �ּҿ� ����� ���� �о� Vx�� �Է��Ѵ�
        case 0x65:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tADD\tV" + s + " [I]";
            
    }
}

string decodeGeneral(unsigned short _code)
{
    unsigned char nibble3 = _code >> 12;
    unsigned short nnn = _code & 0x0fff;
    unsigned char x = (_code >> 8) & 0x000f;
    unsigned char y = (_code >> 4) & 0x000f;
    unsigned char kk = _code & 0x00ff;
    unsigned char n = _code & 0x000f;
    stringstream ss;
    string s;
    string s2;
    string s3;
    // ��ɾ ���� ���� �Ϻ��� ���� �з�
    switch (nibble3)
    {
        //JP: PC���� �ٲ���
        case 0x1:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tJP\t" + s;
        //CALL: nnn�� �ִ� �����ƾ���� �б�
        case 0x2:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tCALL\t" + s;
        //SE: Vx == kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
        case 0x3:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSE\tV" + s + " " + s2;
         //SNE: Vx != kk��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
        case 0x4:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSNE\tV" + s + " " + s2;
        //SEV: Vx == Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
        case 0x5:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSE\tV" + s + " V" + s2;
        //LD: Vx�� kk���� �Է��Ѵ�
        case 0x6:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tLD\tV" + s + " " + s2;
        case 0x7:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tADD\tV" + s + " " + s2;
        case 0x8:
            return decode8000(_code);
        //SNEV: Vx != Vy��� ���� ��ɾ �ǳʶڴ�(���Ǻб�)
        case 0x9:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSNE\tV" + s + " V" + s2;
        //LDI: I�������Ϳ� nnn���� �Է��Ѵ�
        case 0xA:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tLD\tI " + s;
        //JPn: V0���������� ���� nnn�� ���� �ּҷ� �б��Ѵ�
        case 0xB:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tJP\tV0 " + s;
        case 0xC:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tRND\tV" + s + " " + s2;
        //DRW: I�������Ϳ� �ִ� �޸��ּҿ��� n����Ʈ��ŭ
            // ��������Ʈ�� �о (Vx, Vy) �ȼ� ��ġ���� �׸���
        case 0xD:
            //������ �ȼ��� �ϳ��� �ִٸ�
            //Vf�������Ϳ� �÷��װ� 1�� �����ϰ�,
            //�ƴ϶�� �÷��װ� 0�� �����Ѵ�.
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(n);
            s3 = ss.str();
            ss.str("");
            return"\tDRW\tV" + s + " V" + s2 + " " + s3;
        case 0xE:
            return decodeE000(_code);
        case 0xF:
            return decodeF000(_code);
    }
}
string decode(unsigned short _code)
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


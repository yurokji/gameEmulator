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
        //LD Vx, Vy: Vx에 Vy값을 입력한다
        case 0x0:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tLD\tV" + s + " V" + s2;
        //OR Vx, Vy: Vx와 Vy를 OR연산하여 Vx에 넣는다
        case 0x1:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tOR\tV" + s + " V" + s2;
        //AND Vx, Vy: Vx와 Vy를 AND연산하여 Vx에 넣는다
        case 0x2:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tAND\tV" + s + " V" + s2;
            //XOR Vx, Vy: Vx와 Vy를 XOR연산하여 Vx에 넣는다
        case 0x3:
            //예를 들어: 기계어가 0x8a43
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tXOR\tV" + s + " V" + s2;
        //ADD Vx, Vy: Vx와 Vy를 ADD(덧셈)연산하여 Vx에 넣는다
        // 덧셈의 결과가 255보다 크면 
        //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
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
            //SHR Vx, Vy: Vx의 마지막 이진수 비트가 1이라면 (자리 내림이 발생했다면)
            // Vf를 1로 만들고, 아니라면 0으로 만든다.
            //그리고 Vx를 2로 나눠서 Vx에 저장한다
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
            // 뺄셈의 결과가 0보다 크면, 즉 Vy > Vx라면   
            //플래그 결과를 저장할 Vf를 1로 만들고, 아닐경우 Vf를 0으로 만든다
        case 0x7:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSUBN\tV" + s + " V" + s2;
            //SHL Vx, Vy: Vx의 가장 상위 비트가 1이라면 (자리 올림이 발생했다면)
            // Vf를 1로 만들고, 아니라면 0으로 만든다.
            //그리고 Vx를 2로 곱해서 Vx에 저장한다
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
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //SKP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 눌렸다면 다음 명령어를 건너뛴다
        case 0x9E:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            return "\tSKP\tV" + s;
        //SKNP Vx: Vx레지스터에 지정된 키보드 키가 눌렸는지 확인해서
        //만약 안눌렸다면(키보드를 뗐다면)  다음 명령어를 건너뛴다
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
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nn)
    {
        //LD Vx, DT: 딜레이 타이머 값을 넣는다
        case 0x07:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tV" + s + " DT";
        //LD Vx, K: 키 입력을 기다렸다가, 키가 입력되면
        //입력된 키값(K)를 Vx에 넣는다
        case 0x0A:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tV K";
        //LD DT, Vx: Vx의 값을 딜레이 타이머로 설정한다
        case 0x15:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tDT V" + s;
         //LD ST, Vx: Vx의 값을 사운드 타이머로 설정한다
        case 0x18:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tST V" + s;
        // ADD I, Vx, I와 Vx값을 더해 I에 입력한다
        case 0x1E:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tADD\tI V" + s;
        // LD F, Vx: Vx에 있는 스프라이트 번호 x 스프라이트크기 -> I에 주소로서 넣어준다
        case 0x29:
            ss << setw(1) << hex << int(x);
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
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\tLD\tB V" + s;
        //LD [I], Vx: 메모리 시작위치(I에 저장된)로부터 V0부터 Vx까지 레지스터값을
        //차례로 메모리에 순차적으로 저장한다
        case 0x55:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            return "\LD\t[I] V" + s;
        //LD Vx, [I], Vx: I레지스터에 있는 메모리 주소를 참조하여, 
        // 그 주소에 저장된 값을 읽어 Vx에 입력한다
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
    // 명령어를 가장 상위 니블값에 의해 분류
    switch (nibble3)
    {
        //JP: PC값을 바꿔줌
        case 0x1:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tJP\t" + s;
        //CALL: nnn에 있는 서브루틴으로 분기
        case 0x2:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tCALL\t" + s;
        //SE: Vx == kk라면 다음 명령어를 건너뛴다(조건분기)
        case 0x3:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSE\tV" + s + " " + s2;
         //SNE: Vx != kk라면 다음 명령어를 건너뛴다(조건분기)
        case 0x4:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSNE\tV" + s + " " + s2;
        //SEV: Vx == Vy라면 다음 명령어를 건너뛴다(조건분기)
        case 0x5:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setfill('0') << setw(2) << hex << int(kk);
            s2 = ss.str();
            ss.str("");
            return "\tSE\tV" + s + " V" + s2;
        //LD: Vx에 kk값을 입력한다
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
        //SNEV: Vx != Vy라면 다음 명령어를 건너뛴다(조건분기)
        case 0x9:
            ss << setw(1) << hex << int(x);
            s = ss.str();
            ss.str("");
            ss << setw(1) << hex << int(y);
            s2 = ss.str();
            ss.str("");
            return "\tSNE\tV" + s + " V" + s2;
        //LDI: I레지스터에 nnn값을 입력한다
        case 0xA:
            ss << setfill('0') << setw(3) << hex << int(nnn);
            s = ss.str();
            ss.str("");
            return "\tLD\tI " + s;
        //JPn: V0레지스터의 값과 nnn을 더한 주소로 분기한다
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
        //DRW: I레지스터에 있는 메모리주소에서 n바이트만큼
            // 스프라이트를 읽어서 (Vx, Vy) 픽셀 위치에서 그린다
        case 0xD:
            //지워진 픽셀이 하나라도 있다면
            //Vf레지스터에 플래그값 1을 저장하고,
            //아니라면 플래그값 0을 저장한다.
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


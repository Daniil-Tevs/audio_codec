#include <iostream>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
#include <vector>
#include <string>
#include <fstream>
std::vector<char> make_hex(int a)
{
    std::vector<char> tmp;
    while(a>0)
    {
        tmp.push_back(a%256);
        a/=256;
    }
    char b;
    for(int i=0;i<tmp.size()/2;i++) { b=tmp[i];tmp[i]=tmp[tmp.size()-i-1];tmp[tmp.size()-i-1]=b;}
    return tmp;
}
void make_waw(int channels,int rate,int size, spx_int16_t * data)
{

    std::ofstream out("../output.waw",std::ifstream::binary);
    char* m_out = new char;
    std::string tmp;
    int s=0;
    tmp="RIFF";
    for(int i=0;i<tmp.size();i++) { m_out[i] = tmp[i];s++; }
    for(int i=0;i<4;i++) { m_out[s] = 0; s++;}
    tmp="WAVEfmt ";
    for(char &i : tmp) { m_out[s] = i; s++;}
    m_out[s]=16;s++;
    for(int i=0;i<3;i++) { m_out[s] = 0; s++;}
    m_out[s]=1;s++;m_out[s]=0;s++;
    m_out[s]=channels;s++;m_out[s]=0;s++;
    std::vector<char> tmp_vec = make_hex(rate);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out[s] = tmp_vec[i]; }
    else{m_out[s]=0;}s++;}
    tmp_vec = make_hex(rate*channels*2);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out[s] = tmp_vec[i]; }
        else{m_out[s]=0;}s++;}
    m_out[s]=2*channels;s++;m_out[s]=0;s++;
    m_out[s]=16;s++;m_out[s]=0;s++;
    for(int i=0;i<s;i++)
        out<<m_out[i];
//    itmp = le_int(rate);
//    fwrite(&itmp, 4, 1, file);
//
//    itmp = le_int(rate*channels*2);
//    fwrite(&itmp, 4, 1, file);
//
//    stmp = le_short(2*channels);
//    fwrite(&stmp, 2, 1, file);
//
//    stmp = le_short(16);
//    fwrite(&stmp, 2, 1, file);
//
//    fprintf (file, "data");
//
//    itmp = le_int(0x7fffffff);
//    fwrite(&itmp, 4, 1, file);

}
int main() {
    setlocale(LC_ALL,"Rus");
    //std::ifstream ind("../Hello.dat");
    std::ifstream inb("../Hello.enc");

    void *dec_state;

    SpeexBits dec_bits;
    speex_bits_init(&dec_bits);
    dec_state = speex_decoder_init(&speex_nb_mode);

    spx_int16_t out[10000];
    int frame_size =20;
    speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);
//    speex_decoder_ctl(dec_state,3,&out);

    inb.unsetf(std::ios::dec);
    inb.setf(std::ios::hex);
    char in_b[100], in_f[10],in_s[10];
    int s=0;
    bool fl = true;
    std::vector<char> t;
    char byte;
    while(!inb.eof())
    {
        int b,c;
        if(fl){inb>>byte;}
        else {fl= true;}
        b = (byte >> 4) & 0xF;
        c = byte & 0xF;
        s+=b;
        s+=c;
//        for(int i=0;i<b+c;i++) {
//            inb>>in_b[i];
//        }
        for(int i=0;i<b+c;i++) {
            char a;
            inb>>a;
            int k =int(a);
            if(int(a)==17 || int(a) == 102 || int(a) == 22 || int(a) == 97) {byte=a;fl = false; break; }
//            if(i<c)
//                inb>>in_b[b+i];
//            else
//                inb>>in_b[i-c];
            if(i<c)
                in_b[b+i]=a;
            else
                in_b[i-c]=a;
        }
//          for(int i=0;i<c;i++)
//              inb>>in_s[i];
//          for(int i=0;i<b;i++)
//              inb>>in_f[i];
//          for(int i=0;i<b+c;i++)
//              if(i<b){in_b[i]=in_f[i];}
//              else{in_b[i]=in_s[i-b];}

        std::cout<<std::hex<<int(b) <<" "<<int(c)<<" ";
        for(int i=0;i<b+c;i++)
            std::cout<<std::hex<<int(in_b[i])<<" ";
        std::cout<<std::endl;
        speex_bits_read_from(&dec_bits,in_b,b+c);
        speex_decode_int(dec_state, &dec_bits,out);
        speex_bits_reset(&dec_bits);
    }
    std::cout<<std::dec<<s<<std::endl;
    std::ofstream out_f("../output.txt");
    for(int i=0;i<s+100;i++)
        out_f<<out[i]<<" ";
    std::cout<<out[0];
    make_waw(1,8000,123,out);
    return 0;
}

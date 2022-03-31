#include <iostream>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
#include <vector>
#include <string>
#include <fstream>

std::vector<char> make_hex(int a,int fl)
{
    std::vector<char> tmp;
    while(a>0)
    {
        tmp.push_back(a%256);
        a/=256;
    }
    char b;
    if(fl==1)
    {
        for (int i = 0; i < tmp.size() / 2; i++) {
            b = tmp[i];
            tmp[i] = tmp[tmp.size() - i - 1];
            tmp[tmp.size() - i - 1] = b;
        }
    }
    return tmp;
}

void make_waw(int channels,int rate,int size_data, const std::string& file_data)
{

    std::ofstream out("../output.wav",std::ifstream::binary);
    std::vector<char> m_out;
    std::string tmp;
    std::vector<char> tmp_vec;
    tmp="RIFF";
    for(int i=0;i<tmp.size();i++) { m_out.push_back(tmp[i]);}
    tmp_vec = make_hex(120000+44,0);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out.push_back(tmp_vec[i]); }
        else{m_out.push_back(0);}}
    tmp="WAVEfmt ";
    for(char &i : tmp) { m_out.push_back(i);}
    m_out.push_back(16);
    for(int i=0;i<3;i++) { m_out.push_back(0);}
    m_out.push_back(1);m_out.push_back(0);;
    m_out.push_back(channels);m_out.push_back(0);
    tmp_vec = make_hex(rate,1);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out.push_back(tmp_vec[i]);}
    else{m_out.push_back(0);}}
    tmp_vec = make_hex(rate*channels*2,1);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out.push_back(tmp_vec[i]); }
        else{m_out.push_back(0);}}
    m_out.push_back(2*channels);m_out.push_back(0);
    m_out.push_back(16);m_out.push_back(0);
    tmp="data";
    for(char &i : tmp) { m_out.push_back(i);}
    tmp_vec = make_hex(120000,1);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out.push_back(tmp_vec[i]); }
        else{m_out.push_back(0);}}
    for(int i=0;i<m_out.size();i++)
        out<<m_out[i];
    std::ifstream in(file_data);
    while(!in.eof())
    {
        char a;
        in>>std::hex>>a;

        out<<std::hex<<a;
    }
}

int main() {
    std::string name ="../Hello.dat", tmp_file = "../output.txt";
    setlocale(LC_ALL,"Rus");
    std::ifstream inb(name);

    void *dec_state;

    SpeexBits dec_bits;
    speex_bits_init(&dec_bits);
    dec_state = speex_decoder_init(&speex_nb_mode);
    std::ofstream out_f(tmp_file);

    int frame_size =20;
    speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);

    if(name[name.size()-1]=='c') {
        inb.unsetf(std::ios::dec);
        inb.setf(std::ios::hex);
    }
    char in_b[100];
    int s=0,byte_t;
    bool fl = true;
    std::vector<char> t;
    std::vector<spx_int16_t > out_i;
    spx_int16_t out[10000];
    char byte;
    while(!inb.eof())
    {
        if(name[name.size()-1]=='c') {
            int b,c;
            if(fl){inb>>byte;}
            else {fl= true;}
            b = (byte >> 4) & 0xF;
            c = byte & 0xF;
            s+=b;
            s+=c;
            for (int i = 0; i < b + c; i++) {
                char a;
                inb >> a;
                if (int(a) == 17 || int(a) == 102 || int(a) == 22 || int(a) == 97) {
                    byte = a;
                    fl = false;
                    break;
                }
                if (i < c)
                    in_b[b + i] = a;
                else
                    in_b[i - c] = a;
            }
            std::cout<<std::hex<<int(b) <<" "<<int(c)<<" ";
            for(int i=0;i<b+c;i++)
                std::cout<<std::hex<<int(in_b[i])<<" ";
            std::cout<<std::endl;
            speex_bits_read_from(&dec_bits,in_b,b+c);
            speex_decode_int(dec_state, &dec_bits,out);
            speex_bits_reset(&dec_bits);
            for(int i=0;i<(b+c)*8;i++)
            {
                out_f <<  out[i] << " ";
                s++;}
            t.clear();
        }
      if(name[name.size()-1]=='t'){
          int b,c;
          inb>>std::dec>>byte_t;
          b = byte_t/10;
          c = byte_t%10;
          for (int i = 0; i < b + c; i++) {
              int a;
              inb>>std::hex>>a;
              t.push_back(a);
          }
          for (int i = 0; i < b + c; i++) {
              if (i < c)
                  in_b[b + i] = t[i];
              else
                  in_b[i - c] = t[i];
          }
          std::cout<<std::hex<<int(b) <<" "<<int(c)<<" ";
          for(int i=0;i<b+c;i++)
              std::cout<<std::hex<<int(in_b[i])<<" ";
        std::cout<<std::endl;
        speex_bits_read_from(&dec_bits,in_b,b+c);
        speex_decode_int(dec_state, &dec_bits,out);
        speex_bits_reset(&dec_bits);
        for(int i=0;i<t.size()*8;i++) { out_f << std::hex << (int) out[i] << " "; s++;}
          t.clear();
      }
    }
    std::cout<<std::dec<<s<<std::endl;
    make_waw(1,8000,s,tmp_file);
    return 0;
}

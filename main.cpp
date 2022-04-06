#include <iostream>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
#include <string>
#include <fstream>

void make_header_wav(const std::string& output_wav,int channels,int rate)//без размера данных и файла
{
    std::ofstream out(output_wav,std::ios::binary);
    int tmp_i;
    short tmp_s;
    out.write("RIFF",4);
    out.write("0",4);
    out.write("WAVEfmt ",8);
    tmp_i=16;
    out.write(reinterpret_cast<const char *>(&tmp_i), sizeof(tmp_i) );
    tmp_s=1;
    out.write(reinterpret_cast<const char *>(&tmp_s), sizeof(tmp_s) );
    tmp_s=channels;
    out.write(reinterpret_cast<const char *>(&tmp_s), sizeof(tmp_s) );
    tmp_i=rate;
    out.write(reinterpret_cast<const char *>(&tmp_i), sizeof(tmp_i) );
    tmp_i=rate*channels*2;
    out.write(reinterpret_cast<const char *>(&tmp_i), sizeof(tmp_i) );
    tmp_s=2*channels;
    out.write(reinterpret_cast<const char *>(&tmp_s), sizeof(tmp_s) );
    tmp_s=16;
    out.write(reinterpret_cast<const char *>(&tmp_s), sizeof(tmp_s) );
    out.write("data",4);
    out.write("0",4);
}
//std::string extension(const std::string& name)
//{
//    std::string tmp;
//    int fl=0;
//    for(int i=0;i<name.size()-1;i++)
//        if((name[i]=='.' && name[i+1]!='.' && name[i+1]!='/') || (fl==1)) { tmp += name[i + 1]; fl=1;}
//    return tmp;
//}

int main() {
    std::string name_input ="../Lem2400.enc", name_out = "../output.wav";
    setlocale(LC_ALL,"Rus");
    std::ifstream inb(name_input,std::ios::binary);
    make_header_wav(name_out,1,8000);
    std::ofstream out_f(name_out,std::ios::app | std::ios::binary);

    void *dec_state;

    SpeexBits dec_bits;
    speex_bits_init(&dec_bits);
    dec_state = speex_decoder_init(&speex_nb_mode);

    char in_b[100];
    int s=0;
    spx_int16_t out[10000];

    if(name_input.find(".enc")!=-1) {
        char byte;
        while(!inb.eof()) {
            int second, first;
            inb.read(&byte, sizeof(byte));
            first = (byte >> 4) & 0xF;
            second = byte & 0xF;
            inb.read(in_b, first);
            inb.read(in_b+first, second);
            std::cout << std::hex << int(first) << " " << int(second) << " ";
            for (int i = 0; i < second + first; i++)
                std::cout << std::hex << int(in_b[i]) << " ";
            std::cout << std::endl;
            speex_bits_read_from(&dec_bits, in_b,first);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            speex_bits_read_from(&dec_bits, in_b+first,second);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            s++;
        }
    }
    else if(name_input.find(".dat")!=-1) {
        int byte;
        int c;
        while(!inb.eof()) {
            int second, first;
            inb>>std::dec>>byte;
            first = byte%10;
            second = byte / 10;
            for(int i=0;i<first+second;i++)
            {
                inb>>std::hex>>c;
                in_b[i] = c;
            }
            std::cout << std::hex << int(first) << " " << int(second) << " ";
            for (int i = 0; i < second + first; i++)
                std::cout << std::hex << int(in_b[i]) << " ";
            std::cout << std::endl;
            speex_bits_read_from(&dec_bits, in_b,first);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            speex_bits_read_from(&dec_bits, in_b+first,second);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            s++;
        }
    }
    else
        std::cout<<"Error, program don't work with this extension file";

    out_f.clear();
    int size_data = s*320,size_file = size_data+44;
    out_f.close();
    out_f.flush();
    out_f.open(name_out,std::ios::binary | std::ios::in);
    out_f.seekp(4);
    out_f.write(reinterpret_cast<const char *>(&size_file), 4);
    out_f.seekp(40);
    out_f.write(reinterpret_cast<const char *>(&size_data), 4);
    return 0;
}

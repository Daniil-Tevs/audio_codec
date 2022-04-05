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

void make_header_wav(const std::string& output_wav,int channels,int rate)//без размера данных и файла
{
    std::ofstream out(output_wav,std::ios::binary);
    std::vector<char> m_out;
    std::string tmp;
    std::vector<char> tmp_vec;
    tmp="RIFF";
    for(int i=0;i<tmp.size();i++) { m_out.push_back(tmp[i]);}
    tmp_vec = make_hex(33696+44,0);
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
    tmp_vec = make_hex(12000,1);
    for(int i=0;i<4;i++) {if(i<tmp_vec.size()) { m_out.push_back(tmp_vec[i]); }
        else{m_out.push_back(0);}}
    for(int i=0;i<m_out.size();i++)
        out<<m_out[i];
}
std::string extension(const std::string& name)
{
    std::string tmp;
    int fl=0;
    for(int i=0;i<name.size()-1;i++)
        if((name[i]=='.' && name[i+1]!='.' && name[i+1]!='/') || (fl==1)) { tmp += name[i + 1]; fl=1;}
    return tmp;
}

int main() {
    std::string name_input ="../Hello.enc", name_out = "../output.wav",tmp_file = "../output.txt";
    setlocale(LC_ALL,"Rus");
    std::ifstream inb(name_input,std::ios::binary);
    std::string extension_file = extension(name_input);
    make_header_wav(name_out,1,8000);
    std::ofstream out_f(name_out,std::ios::app | std::ios::binary);

    void *dec_state;

    SpeexBits dec_bits;
    speex_bits_init(&dec_bits);
    dec_state = speex_decoder_init(&speex_nb_mode);

    int frame_size =20;
    speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);

    char in_b[100];
    int s=0,byte_t;
    bool fl = true;
    std::vector<char> t;
    std::vector<spx_int16_t > out_i;
    spx_int16_t out[10000];
    char byte;

    if(name_input.find(".enc")) {
        while(!inb.eof()) {
            int second, first;
            /*if (fl) { inb >> byte; }
            else { fl = true; }*/
            inb.read(&byte, sizeof(byte));
            second = (byte >> 4) & 0xF;
            first = byte & 0xF;
            inb.read(in_b, second);
            inb.read(in_b + second, first);
            /*for (int i = 0; i < second; i++) {
                inb >> in_b[first + i];
                if (int(in_b[first + i]) == 17 || int(in_b[first + i]) == 102 || int(in_b[first + i]) == 22 || int(in_b[first + i]) == 97) {
                    byte = in_b[first + i];
                    fl = false;
                    break;
                }
            }*/
            /*if(fl) {
                for (int i = 0; i < first; i++) {
                    inb >> in_b[i];
                    if (int(in_b[i]) == 17 || int(in_b[i]) == 102 || int(in_b[i]) == 22 || int(in_b[i]) == 97) {
                        byte = in_b[i];
                        fl = false;
                        break;
                    }
                }
            }*/
            std::cout << std::hex << int(second) << " " << int(first) << " ";
            for (int i = 0; i < second + first; i++)
                std::cout << std::hex << int(in_b[i]) << " ";
            std::cout << std::endl;
            speex_bits_read_from(&dec_bits, in_b+second,first);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            speex_bits_read_from(&dec_bits, in_b,second);
            speex_decode_int(dec_state, &dec_bits, out);
            out_f.write(reinterpret_cast<const char *>(&out), 160);
            speex_bits_reset(&dec_bits);
            t.clear();
            s+=320;
        }
    }
    else if(extension_file == "dat") {
        while(!inb.eof()) {
            int b, c;
            inb >> std::dec >> byte_t;
            b = byte_t / 10;
            c = byte_t % 10;
            for (int i = 0; i < b + c; i++) {
                int a;
                inb >> std::hex >> a;
                t.push_back(a);
            }
            for (int i = 0; i < b + c; i++) {
                if (i < c)
                    in_b[b + i] = t[i];
                else
                    in_b[i - c] = t[i];
            }
            std::cout << std::hex << int(b) << " " << int(c) << " ";
            for (int i = 0; i < b + c; i++)
                std::cout << std::hex << int(in_b[i]) << " ";
            std::cout << std::endl;
            speex_bits_read_from(&dec_bits, in_b, b + c);
            speex_decode_int(dec_state, &dec_bits, out);
            speex_bits_reset(&dec_bits);
            for (int i = 0; i < t.size() * 8; i++) {
                out_f << std::hex << (int) out[i] << " ";
                s++;
            }
            t.clear();
        }
    }
    else
        std::cout<<"Error, program don't work with this extension file";
    out_f.clear();

//    int size_f=44+s;
//    out_f.open(name_out,std::ios::ate);
//    out_f.seekp(5);
//
////    out_f.write(reinterpret_cast<const char *>(size_f), 1);
//    std::cout<<std::dec<<s<<std::endl;
//    out_f.close();
//    inb.close();
//    make_header_wav(name_out,1,8000,s);
//    std::ifstream in_e(tmp_file,std::ios::binary);
//    std::ofstream out_e(name_out,std::ios::app);
//    while(!in_e.eof())
//    {
//        char a;
//        in_e>>std::hex>>a;
//        out_e<<std::hex<<a;
//    }
//    std::cout<<std::dec<<s<<std::endl;
    int size_data = s,size_file = size_data+44;
    out_f.close();
    out_f.flush();
    out_f.open(name_out,std::ios::binary | std::ios::in);
    out_f.seekp(4);
    out_f.write(reinterpret_cast<const char *>(&size_file), 4);
    out_f.seekp(40);
    out_f.write(reinterpret_cast<const char *>(&size_data), 4);
    return 0;
}

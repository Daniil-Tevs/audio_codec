#include <fstream>
#include <string>
#include <vector>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
class wav{
public:
    wav(int channels,int rate)
    {
        m_header.SizeFile = 0;
        m_header.Length_of_format_data = 16;
        m_header.Type_of_format = 1;
        m_header.Channels = channels;
        m_header.SampleRate = rate;
        m_header.SampleRate_Channels_2 = rate*channels*2;   /// SampleRate*Channels*2
        m_header.Channels_2 = channels*2;              /// Channels*2
        m_header.Bits_per_sample = 16;         /// 16
        m_header.SizeData=0;
    }
    wav(std::string from) {
        std::ifstream in(from, std::ios::binary);
        in.read(reinterpret_cast<char *>(&m_header), sizeof(wav_header));
        int tmp;
        for(int i=0;i<m_header.SizeData;i++)
        {
            in>>tmp;
            m_data.push_back(tmp);
        }
    }

    wav(const wav& sound)
    {
        m_header = sound.m_header;
        for (int i = 0; i < m_header.SizeData; i++)
            m_data.push_back(sound.m_data[i]);
    }
    wav& operator=(const wav& sound)
    {
        m_data.clear();
        m_header = sound.m_header;
        for (int i = 0; i < m_header.SizeData; i++)
            m_data.push_back(sound.m_data[i]);
        return *this;
    }

    ~wav(){    };

    void upload(std::string from) {
//        if(from.find(".wav")!=-1)
        std::ifstream in(from, std::ios::binary);
        in.read(reinterpret_cast<char *>(&m_header), sizeof(wav_header));
        char tmp;
        for(int i=0;i<m_header.SizeData;i++)
        {
            in>>std::hex>>tmp;
            m_data.push_back(tmp);
        }
    };
    void save(std::string where)
    {
        std::ofstream out(where,std::ios::binary);
        out.write(reinterpret_cast<const char *>(&m_header), sizeof (m_header));
        for(int i=0;i<m_data.size();i++)
            out.write(reinterpret_cast<const char *>(&m_data[i]), 1);
    }
    void decode(std::string from)
    {
        std::ifstream inb(from,std::ios::binary);
        void *dec_state;
        SpeexBits dec_bits;
        speex_bits_init(&dec_bits);
        dec_state = speex_decoder_init(&speex_nb_mode);

        char in_b[100];
        int s=0;
        spx_int16_t out[10000];

        if(from.find(".enc")!=-1) {
            char byte;
            while(!inb.eof()) {
                int second, first;
                inb.read(&byte, sizeof(byte));
                first = (byte >> 4) & 0xF;
                second = byte & 0xF;
                inb.read(in_b, first);
                inb.read(in_b+first, second);
//                std::cout << std::hex << int(first) << " " << int(second) << " ";
//                for (int i = 0; i < second + first; i++)
//                    std::cout << std::hex << int(in_b[i]) << " ";
//                std::cout << std::endl;
                speex_bits_read_from(&dec_bits, in_b,first);
                speex_decode_int(dec_state, &dec_bits, out);
                for(int i=0;i<160;i++)
                    m_data.push_back(out[i]);
                speex_bits_reset(&dec_bits);
                speex_bits_read_from(&dec_bits, in_b+first,second);
                speex_decode_int(dec_state, &dec_bits, out);
                for(int i=0;i<160;i++)
                    m_data.push_back(out[i]);
                speex_bits_reset(&dec_bits);
                s++;
            }
        }
        else if(from.find(".dat")!=-1) {
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
//                std::cout << std::hex << int(first) << " " << int(second) << " ";
//                for (int i = 0; i < second + first; i++)
//                    std::cout << std::hex << int(in_b[i]) << " ";
//                std::cout << std::endl;
                speex_bits_read_from(&dec_bits, in_b,first);
                speex_decode_int(dec_state, &dec_bits, out);
                for(int i=0;i<160;i++)
                    m_data.push_back(out[i]);
                speex_bits_reset(&dec_bits);
                speex_bits_read_from(&dec_bits, in_b+first,second);
                speex_decode_int(dec_state, &dec_bits, out);
                for(int i=0;i<160;i++)
                    m_data.push_back(out[i]);
                speex_bits_reset(&dec_bits);
                s++;
            }
        }
        else
            std::cout<<"Error, program don't work with this extension file";
        speex_bits_destroy(&dec_bits);
        speex_decoder_destroy(dec_state);
        m_header.SizeFile = m_data.size()+44;
        m_header.SizeData = m_data.size();
    }
    std::vector<char> encode()
    {
        SpeexBits enc_bits;
        void *enc_state;
        speex_bits_init(&enc_bits);
        enc_state = speex_encoder_init (&speex_nb_mode);
        int frame_size, nbBytes,MAX_NB_BYTES;
        int quality = 4;
        std::vector<char> out_bytes;
        speex_encoder_ctl(enc_state,SPEEX_GET_FRAME_SIZE,&frame_size);
        speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
        spx_int16_t * input_frame = new spx_int16_t[frame_size];
        char* byte_ptr;
        for(int i=0;i<m_data.size();i+=frame_size)
        {
            for(int j=0;j<frame_size;j++)
                input_frame[j] = m_data[i+j];
            speex_bits_reset(&enc_bits);
            speex_encode_int(enc_state, input_frame,&enc_bits);
            MAX_NB_BYTES = speex_bits_nbytes(&enc_bits);
            if(i==0)
                byte_ptr = new char[MAX_NB_BYTES];
            nbBytes = speex_bits_write(&enc_bits, byte_ptr, MAX_NB_BYTES);
            for(int j=0;j<MAX_NB_BYTES;j++)
                out_bytes.push_back(byte_ptr[j]);
        }
        delete[] byte_ptr;
        delete[] input_frame;
        speex_bits_destroy(&enc_bits);
        speex_encoder_destroy(enc_state);
        return out_bytes;
    }

private:
#pragma pack(1)
    struct wav_header
    {
        char              Begin[4]={'R','I','F','F'};
        unsigned int      SizeFile{};
        char              Type[4]={'W','A','V','E'};
        char              chunk_marker[4]={'f','m','t',' '};
        unsigned int      Length_of_format_data{};
        unsigned short    Type_of_format{};
        unsigned short    Channels{};
        unsigned int      SampleRate{};
        unsigned int      SampleRate_Channels_2{};   /// SampleRate*Channels*2
        unsigned short    Channels_2{};              /// Channels*2
        unsigned short    Bits_per_sample{};         /// 16
        char              DataHeader[4]={'d','a','t','a'};
        unsigned int      SizeData{};
    };
#pragma pack()
    wav_header m_header;
    std::vector<spx_int16_t> m_data;
};
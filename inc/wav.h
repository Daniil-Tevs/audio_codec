#include <fstream>
#include <string>
#include <vector>
#include "speex.h"
#include "speex_bits.h"
#include "speex_types.h"
class wav{
public:

    wav(int channels,int rate);
    wav(std::string from);
    wav(const wav& sound);
    wav& operator=(const wav& sound);
    ~wav()=default;
    void upload(std::string from);
    void save(std::string where);
    void decodeTxtFiles(std::string from);
    void decodeBinaryFiles(std::string from);
    std::vector<char> encode();
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
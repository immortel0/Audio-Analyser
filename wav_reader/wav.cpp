#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<cstdint>

const std::string FILE_PATH = "data/musicfile1.wav";


const char hex_arr[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

struct WavHeader{
    char chunk_ID[4];              //  4  riff_mark[4];
    uint32_t chunk_size;           //  4  file_size;
    char format[4];                //  4  wave_str[4];
 
    char sub_chunk1_ID[4];         //  4  fmt_str[4];
    uint32_t sub_chunk1_size;      //  4  pcm_bit_num;
    uint16_t audio_format;         //  2  pcm_encode;
    uint16_t num_channels;         //  2  sound_channel;
    uint32_t sample_rate;          //  4  pcm_sample_freq;
    uint32_t byte_rate;            //  4  byte_freq;
    uint16_t block_align;          //  2  block_align;
    uint16_t bits_per_sample;      //  2  sample_bits;
 
    char sub_chunk2_ID[4];         //  4  data_str[4];
    uint32_t sub_chunk2_size;      //  4  sound_size;
}; 


// const WAV_FILE read_wav(std::string file_name){
//     std::istream FILE("file_name", std::ios::binary);

//     if(!FILE.is_open())

// }




std::string intToBin(uint8_t num){
    std::string out = "";
    
    for(int i = 0; i < 8; i++){
        std::string add = (num & 1 << i) ? "1" : "0";
        out = add + out;
    }
    
    return out;
}



std::string intToHex(uint8_t num){
    std::string out = "";

    out = hex_arr[(num & 0b00001111)];
    out = hex_arr[(num & 0b11110000) >> 4] + out;

    return out;

}



int main(){
    std::ifstream WAVE_FILE(FILE_PATH);

    std::string raw_data;

    getline(WAVE_FILE, raw_data);

    for(int i = 0; i < raw_data.size(); i = i + 4){
        std::cout << intToHex(i) << ": ";

        for(int j = 0; j < 4; j++){
            std::cout << intToHex(raw_data[i + j]);
        }

        std::cout << " : ";

        for(int j = 0; j < 4; j++){
            std::cout << raw_data[i + j];
        }
        
        std::cout << " : ";

        for(int j = 0; j < 4; j++){
            std::cout << (unsigned int)(uint8_t)raw_data[i + j] << " ";
        }

        

        std::cout << std::endl;
    }

    WAVE_FILE.close();

    return 0;
    
}
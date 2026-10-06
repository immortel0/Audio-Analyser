#include<fstream>
#include<string>
#include<cstdint>
#include<iostream>


class WAV{
private:
    struct HEADER{
        char FILE_ID[4];            //RIFF
        uint8_t FILE_SIZE[4];       //Size
        char FILE_FORMAT[4];        //WAVE
        
        char SUBCHUNK1_ID[4];       //fmt 
        uint8_t SUBCHUNK1_SIZE[4] ; //16
        uint8_t AUDIO_FORMAT[2];
        uint8_t NUM_CHANNELS[2];    //1, 2, etc
        uint8_t SAMPLE_RATE[4];     //44100Hz etc
        uint8_t BYTE_RATE[4];       
        uint8_t BLOCK_ALIGN[2];
        uint8_t BITS_PER_SAMPLE[2];
    };

    std::ifstream FILE;

    void getHeader(){
        
    }


public:
    WAV(std::string file_path){
        FILE.open(file_path, std::ios::binary);

        if(!FILE.is_open()){
            std::cerr << "\033[31m" << "error : couldn't open \"" << file_path << "\"" << "\033[0m" << std::endl;
            return;
        }


    }

    
    ~WAV(){
        FILE.close();
    }
};


int main(){
    WAV newSound("/musicfile1.wav");
    return 0;
}
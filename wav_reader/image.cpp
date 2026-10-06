#ifndef IMAGE_H
#define IMAGE_H

#include<fstream>
#include<vector>
#include<cstdint>
#include<cmath>
#include<string>
#include<iostream>

typedef uint8_t u8;


struct Pixel{
    u8 red;
    u8 green;
    u8 blue;

    Pixel(u8 R, u8 G, u8 B) : blue(B), green(G), red(R) {}
    bool operator==(const Pixel& other);
};

struct Position{
    int x;
    int y;

    Position(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}
    Position() {}
    Position operator+(const Position &other);
    Position operator*(const int scale);
    bool operator==(const Position &other);
};

class Image{
    const double PI = 3.141592658979323;

    int HEIGHT, WIDTH;

    std::vector<Pixel> buffer;

    int getPadding(int width_in_px, int bits_per_px);
    void convertToLittleEndian(int num, std::vector<u8>& arr);
    void append(std::ofstream &file, const std::vector<u8> &data);
    void appendHeader(std::ofstream &file);
    void appendPixel(std::ofstream &file, const Pixel& pixel);



public:
    Image(int h, int w) : HEIGHT(h), WIDTH(w) {
        buffer = std::vector<Pixel>(h * w, Pixel(255, 255, 255));
    }

    void saveImage(std::string filename, bool notify);
    void drawLine(const Position& A, const Position& B, const Pixel& colour);
    void drawTriangle(const Position& A, const Position& B, const Position& C, const Pixel& colour);
    void drawHexagon(const Position& Center, const int radius, const Pixel& colour);
    void floodFill(const Position& A, const Pixel& colour);
};

#endif


// BMP METADATA CONSTANTS

namespace {
    std::vector<u8> bmp_header_file_size = {0x46, 0, 0, 0};
    std::vector<u8> bmp_header_offset = {0x36, 0, 0, 0};
    std::vector<u8> dib_header_size = {0x28, 0, 0, 0};
    std::vector<u8> dib_header_image_width = {0x2, 0, 0, 0};
    std::vector<u8> dib_header_image_height = {0x2, 0, 0, 0};
    std::vector<u8> dib_header_plane_used = {0x1, 0};
    std::vector<u8> dib_header_bits_per_pix = {0x18, 0};
    std::vector<u8> dib_header_compression = {0, 0, 0, 0};
    std::vector<u8> dib_header_image_data_size = {0x10, 0, 0, 0};
    std::vector<u8> dib_header_print_res = {0x13, 0x0B, 0, 0};
    std::vector<u8> header_null = {0, 0, 0, 0};
    std::vector<u8> two_null = {0, 0};
    std::vector<u8> one_null = {0};
}

//  FUNCTION DEFINATION STARTS

bool Pixel::operator==(const Pixel &other){
    return (
        red == other.red &&
        green == other.green &&
        blue == other.blue
    );  
}

Position Position::operator+(const Position &other){
    return Position(x + other.x, y + other.y);
}

Position Position::operator*(const int scale){
    return Position(x * scale, y * scale);
}

bool Position::operator==(const Position &other){
    return (
        x == other.x &&
        y == other.y
    );
}

int Image::getPadding(int width_in_px, int bits_per_px){
    int bytes_per_line = (bits_per_px * width_in_px) / 8;
    return (4 - (bytes_per_line % 4)) % 4;
}

void Image::convertToLittleEndian(int num, std::vector<u8> &arr){
    arr[0] = (u8)(num & (255));
    arr[1] = (u8)((num & (255 << 8)) >> 8);
    arr[2] = (u8)((num & (255 << 16)) >> 16);
    arr[3] = (u8)((num & (255 << 24)) >> 24);
}

void Image::append(std::ofstream &file, const std::vector<u8> &data){
    for(auto& x : data)
        file << x;
}

void Image::appendHeader(std::ofstream &file){
    file << "BM";
    append(file, bmp_header_file_size);
    append(file, header_null);
    append(file, bmp_header_offset);

    append(file, dib_header_size);
    append(file, dib_header_image_width);
    append(file, dib_header_image_height);
    append(file, dib_header_plane_used);
    append(file, dib_header_bits_per_pix);
    append(file, dib_header_compression);
    append(file, dib_header_image_data_size);
    append(file, dib_header_print_res);
    append(file, dib_header_print_res);
    append(file, header_null);
    append(file, header_null);

}

void Image::appendPixel(std::ofstream &file, const Pixel &pixel){
    file << pixel.blue;
    file << pixel.green;
    file << pixel.red;
}

void Image::saveImage(std::string filename, bool notify = true){
    std::ofstream File(filename, std::ios::binary);   

    int padding = getPadding(WIDTH, 24);
    int image_data_size = HEIGHT * (3 * WIDTH + padding);

    // bmp_header_file_size = image_data_size + 54;
    // dib_header_image_width = WIDTH;
    // dib_header_image_height = HEIGHT;
    // dib_header_image_data_size = image_data_size

    convertToLittleEndian(image_data_size + 54, bmp_header_file_size);
    convertToLittleEndian(WIDTH, dib_header_image_width);
    convertToLittleEndian(HEIGHT, dib_header_image_height);
    convertToLittleEndian(image_data_size, dib_header_image_data_size);

    appendHeader(File);

    for (int y = 0; y < HEIGHT; y++){
        for (int x = 0; x < WIDTH; x++){
            appendPixel(File, buffer[y * WIDTH + x]);
        }

        for (int i = 0; i < padding; ++i)
            append(File, one_null);
    }

    File.close();
    
    if(notify)
        std::cout << "Saved image as: " << filename << std::endl;
}

void Image::drawLine(const Position& A, const Position& B, const Pixel& colour){
    Position Start;
    Position End;

    if((A.x == B.x) && (A.y == B.y)){
        buffer[A.y * WIDTH + A.x] = colour;
        return;
    }

    float deltaX = (float)(A.x - B.x);
    float deltaY = (float)(A.y - B.y);

    if(abs(deltaY) < abs(deltaX)){
        float slope = deltaY/deltaX;

        if(A.x > B.x){
            Start = B;
            End = A;
        }
    
        else{
            Start = A;
            End = B;
        }
        // cout << "X first\n";
    
        for(int x = Start.x; x <= End.x; x++){
            buffer[round(Start.y + ((x - Start.x) * slope)) * WIDTH + x] = colour;
        }   
    }

    else{
        float slope = deltaX/deltaY;

        if(A.y > B.y){
            Start = B;
            End = A;
        }
    
        else{
            Start = A;
            End = B;
        }
        
        // cout << "Y first \n";
    
        for(int y = Start.y; y <= End.y; y++){
            buffer[y * WIDTH + round(Start.x + (slope * (y - Start.y)))] = colour;
        }   
    }   
}

void Image::drawTriangle(const Position& A, const Position& B, const Position& C, const Pixel& colour){
    drawLine(A, B, colour);
    drawLine(B, C, colour);
    drawLine(C, A, colour); 
}

void Image::drawHexagon(const Position& center, const int radius, const Pixel &colour){
    Position Center(center);
    int R = radius;

    Position A(Center + Position(round(R * sin(1 * PI/3)), round(R * cos(1 * PI/3))  ));
    Position B(Center + Position(round(R * sin(2 * PI/3)), round(R * cos(2 * PI/3))  ));
    Position C(Center + Position(round(R * sin(3 * PI/3)), round(R * cos(3 * PI/3))  ));
    Position D(Center + Position(round(R * sin(4 * PI/3)), round(R * cos(4 * PI/3))  ));
    Position E(Center + Position(round(R * sin(5 * PI/3)), round(R * cos(5 * PI/3))  ));
    Position F(Center + Position(round(R * sin(6 * PI/3)), round(R * cos(6 * PI/3))  ));

    drawLine(A, B, colour);
    drawLine(B, C, colour);
    drawLine(C, D, colour);
    drawLine(D, E, colour);
    drawLine(E, F, colour);
    drawLine(F, A, colour);
}

void Image::floodFill(const Position& P, const Pixel& colour){
    
    if(P.x < 0 || P.x > WIDTH)
        return;   
    if(P.y < 0 || P.y > HEIGHT)
        return;
    
    if(buffer[P.y * WIDTH + P.x] == Pixel(colour)){
        return;
    }
    buffer[P.y * WIDTH + P.x] = colour;

    floodFill(Position(P.x + 1, P.y), colour);
    floodFill(Position(P.x - 1, P.y), colour);
    floodFill(Position(P.x, P.y + 1), colour);
    floodFill(Position(P.x, P.y - 1), colour);  

}   
#include "mycodec.h"

using namespace std;

#define BITS 5

uint8_t num_buckets = 32, bucket_size = 256 / num_buckets;

void write_encoded_image(vector<bitset<BITS>>& encoded_image, int width, int height, string filename) {
    ofstream outfile(filename, ios::out | ios::binary);
    outfile.write(((char*)&width), sizeof(int));
    outfile.write(((char*)&height), sizeof(int));

    cout << "Writing encoded image to file: with width: " << width << " and height: " << height << endl;
    
    for(int i = 0; i < encoded_image.size(); i++) {
        outfile.write((char*)&encoded_image[i], sizeof(bitset<BITS>));
    }
    outfile.close();
}

void bucket_encode(std::vector<uint8_t> image, int width, int height, std::string filename){
    vector<bitset<5>> encoded_image;

    for(auto& pixel : image){
        uint8_t bucket_idx = pixel / bucket_size;
        encoded_image.push_back(bitset<5>(bucket_idx));
    }

    write_encoded_image(encoded_image, width, height, filename);
}

vector<uint8_t> bucket_decode(std::string filename){
    ifstream infile(filename, ios::in | ios::binary);
    int width, height;

    infile.read((char*)&width, sizeof(int));
    infile.read((char*)&height, sizeof(int));

    cout << "Decoding file...: Width: " << width << ", Height: " << height << endl;

    vector<uint8_t> image;
    image.clear();
    
    while(!infile.eof()) {
        bitset<BITS> bucket_idx;
        infile.read((char*)&bucket_idx, sizeof(bitset<BITS>));
        uint8_t intensity = bucket_idx.to_ulong() * bucket_size;
        image.push_back(intensity);
    }
    infile.close();
    return image;
}
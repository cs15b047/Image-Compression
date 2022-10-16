#include "rle.h"
#include <unordered_map>

using namespace std;

void write_encoded_image(vector<pair<uint8_t, short int>>& encoded_image, int width, int height, string filename) {
    ofstream outfile(filename, ios::out | ios::binary);
    outfile.write(((char*)&width), sizeof(int));
    outfile.write(((char*)&height), sizeof(int));

    cout << "Writing encoded image to file: with width: " << width << " and height: " << height << endl;
    
    for(int i = 0; i < encoded_image.size(); i++) {
        outfile.write((char*)&encoded_image[i].first, sizeof(uint8_t));
        outfile.write((char*)&encoded_image[i].second, sizeof(short int));
    }
    outfile.close();
}

void rle_encode(vector<uint8_t> image, int width, int height, string filename) {
    vector<pair<uint8_t, short int>> encoded_image;
    encoded_image.clear();

    cout << "Encoding image with " << image.size() << " pixels" << endl;

    int len = image.size();
    int ptr = 0;
    while(ptr < len) {
        int start = ptr;
        while(ptr < len && image[ptr] == image[start]){
            ptr++;
        }
        pair<uint8_t, short int> p = make_pair(image[start], ptr - start);
        encoded_image.push_back(p);
    }

    cout << "Encoded image elements: " << encoded_image.size() << ", Estimated size: " << ( (sizeof(short int) + sizeof(uint8_t)) * encoded_image.size() ) << " bytes" << endl;

    write_encoded_image(encoded_image, width, height, filename);   
}


vector<uint8_t> rle_decode(string filename) {
    ifstream infile(filename, ios::in | ios::binary);
    int width, height;

    infile.read((char*)&width, sizeof(int));
    infile.read((char*)&height, sizeof(int));

    cout << "Decoding file...: Width: " << width << ", Height: " << height << endl;

    vector<uint8_t> image;
    image.clear();
    
    while(!infile.eof()) {
        uint8_t intensity;
        short int count;
        infile.read((char*)&intensity, sizeof(uint8_t));
        infile.read((char*)&count, sizeof(short int));
        for(short int i = 0; i < count; i++) {
            image.push_back(intensity);
        }
    }
    cout << "Decoded " << image.size() << " elements" << endl;
    infile.close();

    return image;
}
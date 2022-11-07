#include "mycodec.h"

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

template <typename T>
vector<pair<T, short int>> rle_encode(vector<T>& image) {
    vector<pair<T, short int>> encoded_image;
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

    return encoded_image;  
}

template <typename T>
vector<T> rle_decode(vector<pair<T, short int>>& encoded_image) {
    vector<T> image;
    image.clear();
    int num_elements  = 0;
    
    for(auto element : encoded_image) {
        uint8_t intensity = element.first;
        short int count = element.second;
        for(short int i = 0; i < count; i++) {
            image.push_back(intensity);
        }
        num_elements++;
    }
    cout << "Decoded " << num_elements << " elements" << endl;

    return image;
}

template vector<pair<uint8_t, short int>> rle_encode(vector<uint8_t>& image);
template vector<pair<short int, short int>> rle_encode(vector<short int>& image);

template vector<uint8_t> rle_decode(vector<pair<uint8_t, short int>>& encoded_image);
template vector<short int> rle_decode(vector<pair<short int, short int>>& encoded_image);
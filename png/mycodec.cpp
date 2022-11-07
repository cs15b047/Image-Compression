#include "mycodec.h"
#include "compress.h"

using namespace std;

/*
Ideas:
1. RLE encoding: <intensity[0,255], count>: a) directly including all channels --> image blows up to 4MB, b) for each channel separately
2. RGB to YCbCr conversion
3. Bucketing CbCr values into 128 buckets and representing each intensity in 7 bits using bitsets --> lossy
4. ZLib compression
5. WIP: DCT
*/ 

string compress_image(vector<uint8_t>& image, int width, int height, string filename) {
    string filepath = "compressed/" + filename + ".bin";
    // Combination 1: Bucketing + Delta + ZLib
    image = bucket_rgb(image, width, height, filename);
    image = rgb2ycbcr(image);
    image = delta_encode<uint8_t>(image);
    
    vector<char> buffer = write_vec_to_buffer<uint8_t>(image, width, height, filepath);
    compress(buffer, filepath);

    return filepath;
}

vector<uint8_t> decompress_image(string filename) {
    vector<char> buffer = decompress1(filename);
    vector<uint8_t> image = read_vec_from_buffer<uint8_t>(buffer);
    
    image = delta_decode<uint8_t>(image);
    image = ycbcr2rgb(image);
    image = restore_bucket(image);
    
    return image;
}

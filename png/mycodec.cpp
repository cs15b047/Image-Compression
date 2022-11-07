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
    image = rgb2ycbcr(image);
    image = bucket_ycbcr(image, width, height);
    
    vector<double> dct = apply_dct(image, width, height);
    dct = delta_encode(dct);
    
    // auto encoded_image = rle_encode(image);
    vector<char> buffer = write_vec_to_buffer(dct, width, height, filename);
    compress(buffer, filepath);
    return filepath;
}

vector<uint8_t> decompress_image(string filename) {
    vector<char> img_buffer = decompress1(filename);
    pair<int, int> img_size = read_img_size(img_buffer);
    int width = img_size.first, height = img_size.second;
    
    // for decoding RLE:
    // vector<std::pair<uint8_t, short int>> encoded_image = read_vec_from_buffer<std::pair<uint8_t, short int>>(img_buffer);
    // vector<uint8_t> decompressed_image = rle_decode(encoded_image);

    // for decoding DCT:
    vector<double> dct = read_vec_from_buffer<double>(img_buffer);
    dct = delta_decode(dct);

    vector<uint8_t> decoded_image = apply_idct(dct, width, height);
    
    decoded_image = restore_bucket_ycbcr(decoded_image);
    decoded_image = ycbcr2rgb(decoded_image);
    return decoded_image;
}

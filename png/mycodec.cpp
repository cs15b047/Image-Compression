#include "mycodec.h"
#include "compress.h"

using namespace std;

/*
Ideas:
1. RLE encoding: <intensity[0,255], count>: a) directly including all channels --> image blows up to 4MB, b) for each channel separately
2. RGB to YCbCr conversion
3. Bucketing CbCr values into 128 buckets and representing each intensity in 7 bits using bitsets --> lossy
4. ZLib compression
5. DCT + Quantization
*/ 

// Combination 1: Bucketing + Delta + ZLib (RLE worsens compression)
// Combination 2: YCbCr + DCT + Quantization + ZLib (Delta worsens compression)

string compress_image(vector<uint8_t>& image, int width, int height, string filename) {
    string filepath = "compressed/" + filename + ".bin";
    image = rgb2ycbcr(image);
    image = bucket_ycbcr(image, width, height);
    vector<float> dct = apply_dct(image, width, height);
    vector<short> dct_coeffs = vector<short>(dct.begin(), dct.end());
    
    vector<char> buffer = write_vec_to_buffer<short>(dct_coeffs, width, height, filepath);
    compress(buffer, filepath);

    return filepath;
}

vector<uint8_t> decompress_image(string filename) {
    vector<char> buffer = decompress1(filename);
    pair<int, int> dimensions = read_img_size(buffer);
    vector<short> dct = read_vec_from_buffer<short>(buffer);
    
    int width = dimensions.first, height = dimensions.second;
    
    vector<float> dct_float = vector<float>(dct.begin(), dct.end());
    vector<uint8_t> image = apply_idct(dct_float, width, height);
    image = restore_bucket_ycbcr(image);
    image = ycbcr2rgb(image);
    
    return image;
}

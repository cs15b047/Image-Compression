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

string compress_image(vector<uint8_t>& image, int width, int height) {
    string filename = "compressed_image.bin";
    image = rgb2ycbcr(image);
    image = bucket_ycbcr(image, width, height);
    
    vector<double> dct = apply_dct(image, width, height);
    vector<uint8_t> reconstructed_image = apply_idct(dct, width, height);
    calculate_roundtrip_error(image, reconstructed_image, 3 * width * height);
    image = reconstructed_image;
    
    rle_encode(image, width, height, filename);
    compress(filename);
    return filename;
}

vector<uint8_t> decompress_image(string filename) {
    decompress(filename);
    vector<uint8_t> decompressed_image;
    decompressed_image = rle_decode(filename);
    decompressed_image = restore_bucket_ycbcr(decompressed_image);
    decompressed_image = ycbcr2rgb(decompressed_image);
    return decompressed_image;
}

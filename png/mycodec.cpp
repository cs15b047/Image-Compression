#include "mycodec.h"
#include "compress.h"

using namespace std;

/*
Simplistic Ideas:
1. RLE encoding: <intensity[0,255], count>: a) directly including all channels --> image blows up to 4MB, b) for each channel separately
2. Bucketing intensity values into 26 buckets(0-25) and representing each intensity in 5 bits using bitsets --> lossy
*/ 

string compress_image(vector<uint8_t>& image, int width, int height) {
    string filename = "compressed_image.bin";
    // bucket_encode(image, width, height, filename);
    image = bucket_rgb(image, width, height, filename);
    rle_encode(image, width, height, filename);
    compress(filename);
    return filename;
}

vector<uint8_t> decompress_image(string filename) {
    decompress(filename);
    vector<uint8_t> decompressed_image;
    decompressed_image = rle_decode(filename);
    decompressed_image = restore_bucket(decompressed_image);
    return decompressed_image;
}

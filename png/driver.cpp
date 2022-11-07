#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "refpng.h"
#include "mycodec.h"
#include "compress.h"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc != 3) {
        cout << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    read_png_file(argv[1]);

    cout << "Original file size: " << (file_size(argv[1]) / 1024) << " KB" << endl;

    stringstream ss(argv[1]);
    string folder, filename;
    getline(ss, folder, '/');
    getline(ss, filename, '.');

    vector<uint8_t> image;
    image.clear();
    copy_row_pointers_to_vector(image, row_pointers, width, height);
    string compressed_filepath = compress_image(image, width, height, filename);
    image = decompress_image(compressed_filepath);
    row_pointers = copy_vector_to_row_pointers(image, width, height);
    cout << "Image decompressed through custom codec. Writing to PNG file: " << argv[2] << endl;
    write_png_file(argv[2]);

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "refpng.h"
#include "mycodec.h"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc != 3) {
        cout << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }

    read_png_file(argv[1]);

    vector<uint8_t> image;
    image.clear();
    copy_row_pointers_to_vector(image, row_pointers, width, height);
    string filename = compress_image(image, width, height);
    
    image = decompress_image(filename);
    row_pointers = copy_vector_to_row_pointers(image, width, height);

    cout << "Image decompressed through custom codec. Writing to PNG file: " << argv[2] << endl;

    write_png_file(argv[2]);

    return 0;
}
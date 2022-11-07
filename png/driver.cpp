#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "refpng.h"
#include "mycodec.h"
#include "compress.h"

using namespace std;

string file_type(string filename) {
    int loc = filename.find_last_of(".");
    string file_type = filename.substr(loc + 1);
    return file_type;
}

string get_filename(string filepath){
    stringstream ss(filepath);
    string folder, filename;
    getline(ss, folder, '/');
    getline(ss, filename, '.');
    return filename;
}

vector<uint8_t> read_png_image(string input_filepath) {
    read_png_file(input_filepath.c_str());
    string filename = get_filename(input_filepath);

    vector<uint8_t> image;
    image.clear();
    copy_row_pointers_to_vector(image, row_pointers, width, height);
    return image;
}

void write_png_image(vector<uint8_t>& image, string compressed_filepath, string output_filepath) {
    row_pointers = copy_vector_to_row_pointers(image, width, height);
    write_png_file(output_filepath.c_str());
}

vector<uint8_t> read_ppm_image(string input_filepath) {
    ifstream infile(input_filepath, ios::in | ios::binary);
    string magic_number;
    int max_color_value;
    infile >> magic_number;
    infile >> width >> height; // setting global values
    infile >> max_color_value;
    
    #ifdef debug
    cout << "Reading PPM image from file: with width: " << width << " and height: " << height << endl;
    cout << "Max color value: " << max_color_value << endl;
    #endif

    int num_pixels = width * height * 3;
    vector<uint8_t> image(num_pixels);
    infile.read((char*)&image[0], num_pixels * sizeof(uint8_t));
    infile.close();

    vector<vector<uint8_t>> channels = separate_channels(image, width * height);

    // channels --> 1 --> actual red, 2 --> actual green, 0 --> actual blue

    channels = {channels[1], channels[2], channels[0]};
    image = merge_channels(channels, width * height);

    cout << "Number of image pixels: " << image.size() << endl;

    return image;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        cout << "Usage: " << argv[0] << " <input_file> <output_file>" << endl;
        return 1;
    }
    string input_filepath = argv[1], output_filepath = argv[2];
    cout << "Original file size: " << (file_size(argv[1]) / 1024) << " KB" << endl;

    if(file_type(input_filepath) == "png"){
        // Read and compress
        vector<uint8_t> image = read_png_image(input_filepath);
        string compressed_filepath = compress_image(image, width, height, get_filename(input_filepath));

        // Decompress and write
        vector<uint8_t> restored_image = decompress_image(compressed_filepath);
        write_png_image(restored_image, compressed_filepath, output_filepath);
    } else if(file_type(input_filepath) == "ppm") {
        vector<uint8_t> image = read_ppm_image(input_filepath);
        string compressed_filepath = compress_image(image, width, height, get_filename(input_filepath));

        // Decompress and write
        vector<uint8_t> restored_image = decompress_image(compressed_filepath);
        write_png_image(restored_image, compressed_filepath, output_filepath);
    } else {
        cout << "Input file is not a PNG or PPM image." << endl;
        return 1;
    }

    

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include "refpng.h"
#include <jpeglib.h>
#include "mycodec.h"
#include "compress.h"

using namespace std;

string file_type(string filename) {
    int loc = filename.find_last_of(".");
    string file_type = filename.substr(loc + 1);
    transform(file_type.begin(), file_type.end(), file_type.begin(), ::tolower);
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

vector<uint8_t> read_jpeg_image(string input_filepath) {
    vector<uint8_t> image;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * infile;
    JSAMPARRAY buffer;
    int row_stride;

    if ((infile = fopen(input_filepath.c_str(), "rb")) == NULL) {
        fprintf(stderr, "can't open %s for reading  ", input_filepath.c_str());
        exit(1);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    int num_pixels = width * height * 3;

    row_stride = width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        for(int i = 0; i < row_stride; i++) {
            image.push_back(buffer[0][i]);
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return image;


}


int main(int argc, char* argv[]) {
    if(argc != 4) {
        cout << "Usage: " << argv[0] << " <input_file> <compressed_file> <output_file>" << endl;
        return 1;
    }
    string input_filepath = argv[1], compressed_filepath = argv[2], output_filepath = argv[3];
    cout << "Original file size: " << (file_size(argv[1]) / 1024) << " KB" << endl;

    omp_set_num_threads(40);
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads());

    if(file_type(input_filepath) == "png"){
        // Read and compress
        vector<uint8_t> image = read_png_image(input_filepath);
        compress_image(image, width, height, compressed_filepath);

        // Decompress and write
        vector<uint8_t> restored_image = decompress_image(compressed_filepath);
        write_png_image(restored_image, compressed_filepath, output_filepath);
    } else if(file_type(input_filepath) == "ppm") {
        vector<uint8_t> image = read_ppm_image(input_filepath);
        compress_image(image, width, height, compressed_filepath);

        // Decompress and write
        vector<uint8_t> restored_image = decompress_image(compressed_filepath);
        write_png_image(restored_image, compressed_filepath, output_filepath);
    } else if(file_type(input_filepath) == "jpg" || file_type(input_filepath) == "jpeg") {
        vector<uint8_t> image = read_jpeg_image(input_filepath);
        compress_image(image, width, height, compressed_filepath);

        // Decompress and write
        vector<uint8_t> restored_image = decompress_image(compressed_filepath);
        write_png_image(restored_image, compressed_filepath, output_filepath);
    } else {
        cout << "Input file is not a PNG or PPM image." << endl;
        return 1;
    }

    fftw_cleanup_threads();
    

    return 0;
}
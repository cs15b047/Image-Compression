#include "utils.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

template <typename T>
vector<vector<T>> separate_channels(vector<T>& image_, int image_size) {
    vector<vector<T>> image(3, vector<T>(image_size));

    #ifdef debug
    cout << "image size: " << image_size << endl;
    #endif
    
    for (int i = 0; i < image_size; i++) {
        image[0][i] = image_[3 * i]; // isolate the 1st channel (Y in YCbCr)
        image[1][i] = image_[3 * i + 1]; // isolate the 2nd channel (Cb in YCbCr)
        image[2][i] = image_[3 * i + 2]; // isolate the 3rd channel (Cr in YCbCr)
    }
    return image;
}

template <typename T>
vector<T> merge_channels(vector<vector<T>>& image, int image_size) {
    vector<T> combined_image(3 * image_size);
    for (int i = 0; i < image_size; i++) {
        combined_image[3 * i] = image[0][i];
        combined_image[3 * i + 1] = image[1][i];
        combined_image[3 * i + 2] = image[2][i];
    }
    return combined_image;
}

void calc_histogram(vector<uint8_t>& image) {
    vector<int> histogram(256, 0);
    for(auto& pix: image) {
        histogram[pix]++;
    }
    cout << "Histogram: " << endl;
    for(int i = 0 ; i < 256; i++){
        cout << histogram[i] << " ";
    }
    cout << endl;
}

void calculate_roundtrip_error(vector<uint8_t>& image, vector<uint8_t>& image_restored, int image_size) {
    // Calc error
    double total_error = 0.0, max_error = 0.0;
    for(int i = 0; i < image_size; i++) {
        double curr_error = fabs((int)image[i] - (int)image_restored[i]);
        max_error = max(max_error, curr_error);
        total_error += curr_error;
    }

    cout << "Error: " << total_error << endl;
    cout << "Max error: " << max_error << endl;
}

double clip(double pix) {
    return min(max(pix, 0.0), 255.0);
}

template <typename T>
vector<char> write_vec_to_buffer(vector<T>& encoded_image, int width, int height, string filename) {
    int image_size = encoded_image.size();
    vector<char> buffer(3 * sizeof(int) + image_size * sizeof(T));

    #ifdef debug
    cout << "Writing " << image_size << " elements of size " << sizeof(T) << " to buffer" << endl;
    cout << "Writing encoded image to buffer: width: " << width << " and height: " << height << endl;
    #endif
    memcpy(buffer.data(), &image_size, sizeof(int));
    memcpy(buffer.data() + sizeof(int), &width, sizeof(int));
    memcpy(buffer.data() + 2 * sizeof(int), &height, sizeof(int));
    memcpy(buffer.data() + 3 * sizeof(int), encoded_image.data(), image_size * sizeof(T));

    #ifdef debug
    cout << "Buffer size: " << buffer.size() << endl;
    #endif
    assert(buffer.size() == image_size * sizeof(T) + 3 * sizeof(int));
    
    return buffer;
}

template <typename T>
vector<T> read_vec_from_buffer(vector<char> buffer) {
    int width, height, num_elements;
    memcpy(&num_elements, buffer.data(), sizeof(int));
    memcpy(&width, buffer.data() + sizeof(int), sizeof(int));
    memcpy(&height, buffer.data() + 2 * sizeof(int), sizeof(int));

    #ifdef debug
    cout << "Reading encoded image from buffer: width: " << width << " and height: " << height << ", number of elements: " << num_elements << endl;
    #endif

    assert(buffer.size() == 3 * sizeof(int) + num_elements * sizeof(T));
    vector<T> encoded_image(num_elements);
    
    memcpy(encoded_image.data(), buffer.data() + 3 * sizeof(int), num_elements * sizeof(T));

    #ifdef debug
    cout << "Encoded image size: " << encoded_image.size() << endl;
    #endif
    
    return encoded_image;
}

pair<int, int> read_img_size(vector<char> buffer) {
    int width, height, num_elements;
    memcpy(&num_elements, buffer.data(), sizeof(int));
    memcpy(&width, buffer.data() + sizeof(int), sizeof(int));
    memcpy(&height, buffer.data() + 2 * sizeof(int), sizeof(int));

    return make_pair(width, height);
}


template vector<int> merge_channels(vector<vector<int>>& image, int image_size);
template vector<short> merge_channels(vector<vector<short>>& image, int image_size);
template vector<uint8_t> merge_channels<uint8_t>(vector<vector<uint8_t>>& image, int image_size); 
template vector<double> merge_channels<double>(vector<vector<double>>& image, int image_size);
template vector<float> merge_channels<float>(vector<vector<float>>& image, int image_size);

template vector<vector<int>> separate_channels(vector<int>& image_, int image_size);
template vector<vector<short>> separate_channels<short>(vector<short>& image_, int image_size);
template vector<vector<uint8_t>> separate_channels<uint8_t>(vector<uint8_t>& image_, int image_size);
template vector<vector<double>> separate_channels<double>(vector<double>& image_, int image_size);
template vector<vector<float>> separate_channels<float>(vector<float>& image_, int image_size);

template vector<char> write_vec_to_buffer<uint8_t>(vector<uint8_t>& encoded_image, int width, int height, string filename);
template vector<char> write_vec_to_buffer<double>(vector<double>& encoded_image, int width, int height, string filename);
template vector<char> write_vec_to_buffer<float>(vector<float>& encoded_image, int width, int height, string filename);
template vector<char> write_vec_to_buffer<short>(vector<short>& encoded_image, int width, int height, string filename);
template vector<char> write_vec_to_buffer<pair<uint8_t, short int>>(vector<pair<uint8_t, short int>>& encoded_image, int width, int height, string filename);
template vector<char> write_vec_to_buffer<pair<short int, short int>>(vector<pair<short int, short int>>& encoded_image, int width, int height, string filename);

template vector<uint8_t> read_vec_from_buffer(vector<char> buffer);
template vector<short> read_vec_from_buffer(vector<char> buffer);
template vector<double> read_vec_from_buffer(vector<char> buffer);
template vector<float> read_vec_from_buffer(vector<char> buffer);
template vector<pair<uint8_t, short int>> read_vec_from_buffer(vector<char> buffer);
template vector<pair<short int, short int>> read_vec_from_buffer(vector<char> buffer);

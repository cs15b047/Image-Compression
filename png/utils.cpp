#include "util.h"

#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

template <typename T>
vector<vector<T>> separate_channels(vector<T>& image_, int image_size) {
    vector<vector<T>> image(3, vector<T>(image_size));

    cout << "image size: " << image_size << endl;
    
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

template vector<uint8_t> merge_channels<uint8_t>(vector<vector<uint8_t>>& image, int image_size); 
template vector<vector<uint8_t>> separate_channels<uint8_t>(vector<uint8_t>& image_, int image_size);
template vector<double> merge_channels<double>(vector<vector<double>>& image, int image_size);
template vector<vector<double>> separate_channels<double>(vector<double>& image_, int image_size);

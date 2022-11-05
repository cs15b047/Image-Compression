#include "mycodec.h"
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

vector<double> dct2D(vector<uint8_t>& image, int width, int height) {
    int image_size = width * height;
    vector<double> image_double(image_size, 0.0), image_dct(image_size, 0.0);
    // 1. Normalize image to [0, 1]
    for(int i = 0; i < image_size; i++) {
        image_double[i] = (double)image[i];
    }

    // 2. Compute DCT
    fftw_plan dct2d = fftw_plan_r2r_1d(image_size, image_double.data(), image_dct.data(), FFTW_REDFT10, FFTW_ESTIMATE);
    fftw_execute(dct2d);
    // rescale DCT by 2 * image_size
    for(int i = 0; i < image_size; i++) {
        image_dct[i] /= (2 * image_size);
    }

    auto min_it = min_element(image_dct.begin(), image_dct.end());
    cout << "min DCT: " << *min_it << endl;
    auto max_it = max_element(image_dct.begin(), image_dct.end());
    cout << "max DCT: " << *max_it << endl;

    return image_dct;
}


vector<uint8_t> idct2D(vector<double>& image_dct, int width, int height) {
    int image_size = width * height;
    vector<double> image_restored(image_size, 0.0);
    vector<uint8_t> image_restored_uint8(image_size, 0);

    // IDCT
    fftw_plan idct2d = fftw_plan_r2r_1d(width * height, image_dct.data(), image_restored.data(), FFTW_REDFT01, FFTW_ESTIMATE);
    fftw_execute(idct2d);

    auto min_it = min_element(image_restored.begin(), image_restored.end());
    cout << "min IDCT: " << *min_it << endl;
    auto max_it = max_element(image_restored.begin(), image_restored.end());
    cout << "max IDCT: " << *max_it << endl;

    // 4. Convert back to uint8_t
    for(int i = 0; i < image_size; i++) {
        double pix = clip(image_restored[i]);
        image_restored_uint8[i] = ((uint8_t)pix);
    }

    return image_restored_uint8;
}

vector<double> apply_dct(vector<uint8_t>& image_, int width, int height) {
    // take 1st 8x8 block of image
    int image_size = width * height;
    vector<vector<uint8_t>> image = separate_channels<uint8_t>(image_, image_size);
    
    // Compute DCT for each channel
    vector<vector<double>> image_dct(3); 
    for(int i = 0; i < 3; i++) {
        image_dct[i] = dct2D(image[i], width, height);
    }
    vector<double> image_dct_merged = merge_channels<double>(image_dct, image_size);

    return image_dct_merged;
}

vector<uint8_t> apply_idct(vector<double>& image_, int width, int height) {
    int image_size = width * height;
    vector<vector<double>> image = separate_channels<double>(image_, image_size);
    vector<uint8_t> image_restored(3 * image_size, 0);

    // Compute IDCT for each channel
    vector<vector<uint8_t>> image_idct(3);
    for(int i = 0; i < 3; i++) {
        image_idct[i] = idct2D(image[i], width, height);
    }
    image_restored = merge_channels<uint8_t>(image_idct, image_size);

    return image_restored;
}
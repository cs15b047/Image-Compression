#include "mycodec.h"
#include "util.h"

#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

vector<double> dct2D(vector<uint8_t>& image, int width, int height) {
    int image_size = width * height;
    vector<double> image_double(image_size, 0.0), image_dct(image_size, 0.0);
    // Convert to double
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
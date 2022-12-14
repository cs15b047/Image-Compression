#include "mycodec.h"

#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

int block_size;
vector<double> quantization_table;
float quality;

void setup_dct_params() {
    block_size = 8;
    quality = 100;
    quantization_table.resize(block_size * block_size);
    vector<double> Q = {
        16,11,10,16,24,40,51,61,
        12,12,14,19,26,28,60,55,
        14,13,16,24,40,57,69,56,
        14,17,22,29,51,87,80,62,
        18,22,37,56,68,109,103,77,
        24,35,55,64,81,104,113,92,
        49,64,78,87,103,121,120,101,
        72,92,95,98,112,100,103,99
    };

    quantization_table = Q;
    float S = (quality < 50) ? (5000 / quality) : (200 - 2 * quality);
    for(int i = 0; i < block_size * block_size; i++) {
        double entry = floor(round((S * Q[i] + 50) / 100));
        quantization_table[i] = max(1, (int)entry);
    }
}

vector<float> quantize(vector<float>& block) {
    vector<float> quantized_block = block;
    for(int i = 0; i < block_size * block_size; i++) {
        quantized_block[i] = quantization_table[i] * round(quantized_block[i] / quantization_table[i]);
    }
    return quantized_block;
}

vector<float> reverse_quantize(vector<float>& block) {
    vector<float> quantized_block;
    quantized_block.resize(block_size * block_size);
    for(int i = 0; i < block_size * block_size; i++) {
        quantized_block[i] = block[i] * quantization_table[i];
    }
    return quantized_block;
}

vector<float> dct2D(fftw_plan& dct2d, vector<uint8_t>& image, int width, int height) {
    int image_size = width * height;
    vector<double> image_double(image_size, 0.0), image_dct(image_size, 0.0);
    image_double = vector<double>(image.begin(), image.end());
    // 2. Compute
    fftw_execute_r2r(dct2d, image_double.data(), image_dct.data());
    // rescale DCT by 255
    for(int i = 0; i < image_size; i++) {
        image_dct[i] = image_dct[i] / 255.0;
    }
    vector<float> image_dct_float = vector<float>(image_dct.begin(), image_dct.end());

    return image_dct_float;
}

vector<uint8_t> idct2D(fftw_plan& idct2d, vector<float>& image_dct_float, int width, int height) {
    int image_size = width * height;
    vector<double> image_restored(image_size, 0.0);
    vector<uint8_t> image_restored_uint8(image_size, 0);
    vector<double> image_dct = vector<double>(image_dct_float.begin(), image_dct_float.end());

    // IDCT
    fftw_execute_r2r(idct2d, image_dct.data(), image_restored.data());

    // 4. Convert back to uint8_t
    for(int i = 0; i < image_size; i++) {
        double pix = clip(image_restored[i]);
        image_restored_uint8[i] = ((uint8_t)pix);
    }

    return image_restored_uint8;
}

vector<float> apply_dct(vector<uint8_t>& image_, int width, int height) {
    setup_dct_params();
    int image_size = width * height;
    vector<vector<uint8_t>> image = separate_channels<uint8_t>(image_, image_size);
    
    vector<vector<float>> image_dct(3, vector<float>(image_size, 0.0)); 
    int non_zero = 0, total = width * height;
    fftw_plan dct_plan = fftw_plan_r2r_2d(block_size, block_size, NULL, NULL, FFTW_REDFT10, FFTW_REDFT10, FFTW_ESTIMATE);
    #pragma omp parallel for collapse(3)
    // Compute DCT for each channel
    for(int ch = 0; ch < 3; ch++) {
        // Compute DCT for each block of configured size
        for(int i = 0; i < height; i += block_size) {
            for(int j = 0; j < width; j += block_size) {
                // Copy data from block
                vector<uint8_t> block(block_size * block_size, 0);
                for(int k = 0; k < block_size; k++) {
                    for(int l = 0; l < block_size; l++) {
                        if(i + k < height && j + l < width) {
                            block[k * block_size + l] = image[ch][(i + k) * width + (j + l)];
                        }
                    }
                }

                // Compute DCT of block
                vector<float> block_dct = dct2D(dct_plan, block, block_size, block_size);

                // Quantize DCT
                if(ch == 0) {
                    block_dct = quantize(block_dct);
                    #ifdef debug
                    for(auto& x: block_dct) {
                        if(x != 0) non_zero++;
                    }
                    #endif
                }
                
                // Copy DCT back to appropriate block
                for(int k = 0; k < block_size; k++) {
                    for(int l = 0; l < block_size; l++) {
                        if(i + k < height && j + l < width) {
                            image_dct[ch][(i+k) * width + (j + l)] = block_dct[k * block_size + l];
                        }
                    }
                }
            }
        }
    }
    #ifdef debug
    cout << "Fraction of non-zero after quantization of DCT: " << (float) non_zero / total << endl;
    #endif
    vector<float> image_dct_merged = merge_channels<float>(image_dct, image_size);

    return image_dct_merged;
}

vector<uint8_t> apply_idct(vector<float>& image_, int width, int height) {
    setup_dct_params();
    int image_size = width * height;
    vector<vector<float>> image = separate_channels<float>(image_, image_size);
    vector<uint8_t> image_restored(3 * image_size, 0);

    // Compute IDCT for each channel
    fftw_plan idct_plan = fftw_plan_r2r_2d(block_size, block_size, NULL, NULL, FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    vector<vector<uint8_t>> image_idct(3, vector<uint8_t>(image_size, 0));
    #pragma omp parallel for collapse(3)
    for(int ch = 0; ch < 3; ch++) {
        for(int i = 0; i < height; i += block_size) {
            for(int j = 0; j < width; j += block_size) {
                // Copy data from DCT to block
                vector<float> block(block_size * block_size, 0);
                for(int k = 0; k < block_size; k++) {
                    for(int l = 0; l < block_size; l++) {
                        if(i + k < height && j + l < width) {
                            block[k * block_size + l] = image[ch][(i + k) * width + (j + l)];
                        }
                    }
                }

                // Compute IDCT of block
                vector<uint8_t> block_idct = idct2D(idct_plan, block, block_size, block_size);
                
                // Copy IDCT back to appropriate block
                for(int k = 0; k < block_size; k++) {
                    for(int l = 0; l < block_size; l++) {
                        if(i + k < height && j + l < width) {
                            image_idct[ch][(i+k) * width + (j + l)] = block_idct[k * block_size + l];
                        }
                    }
                }
            }
        }
    }
    image_restored = merge_channels<uint8_t>(image_idct, image_size);

    return image_restored;
}
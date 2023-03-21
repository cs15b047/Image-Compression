#include "quantization.h"

using namespace std;

int block_size;
vector<double> quantization_table;
float quality;
vector<double> Q;

void transform_quality(float quality, vector<double>& quantzn_table) {
    quantzn_table = Q;
    float S = (quality < 50) ? (5000 / quality) : (200 - 2 * quality);
    for(int i = 0; i < block_size * block_size; i++) {
        double entry = floor(round((S * Q[i] + 50) / 100));
        quantzn_table[i] = max(1, (int)entry);
    }
}

void setup_dct_params() {
    block_size = 8;
    quality = 100;
    quantization_table.resize(block_size * block_size);
    Q = {
        16,11,10,16,24,40,51,61,
        12,12,14,19,26,28,60,55,
        14,13,16,24,40,57,69,56,
        14,17,22,29,51,87,80,62,
        18,22,37,56,68,109,103,77,
        24,35,55,64,81,104,113,92,
        49,64,78,87,103,121,120,101,
        72,92,95,98,112,100,103,99
    };
    transform_quality(quality, quantization_table);
}

vector<float> quantize(vector<float>& block, float loc_x, float loc_y) {
    loc_x = min(loc_x, 1.0f - loc_x);
    loc_y = min(loc_y, 1.0f - loc_y);
    float quality_factor = loc_x + loc_y; // [0, 1]
    
    // actual quality factor = quality * quality_factor
    vector<double> local_quantization_table;
    float local_quality = quality * quality_factor + (1 - quality_factor) * (quality/2);
    transform_quality(local_quality, local_quantization_table);

    vector<float> quantized_block = block;
    for(int i = 0; i < block_size * block_size; i++) {
        quantized_block[i] = local_quantization_table[i] * round(quantized_block[i] / local_quantization_table[i]);
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
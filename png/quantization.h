#ifndef QUANTZN_H
#define QUANTZN_H

#include <vector>
#include <math.h>
#include <iostream>

extern int block_size;
extern std::vector<double> quantization_table;
extern float quality;
extern std::vector<double> Q;

void transform_quality(float quality, std::vector<double>& quantzn_table);
void setup_dct_params();
std::vector<float> quantize(std::vector<float>& block, float loc_x, float loc_y);
std::vector<float> reverse_quantize(std::vector<float>& block);


#endif // QUANT_H
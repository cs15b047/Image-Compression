#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <stdint.h>
#include <string>
#include <cstring>
#include <cassert>
#include <omp.h>

void calculate_roundtrip_error(std::vector<uint8_t>& image, std::vector<uint8_t>& image_restored, int image_size);
void calc_histogram(std::vector<uint8_t>& image);

template <typename T>
std::vector<T> merge_channels(std::vector<std::vector<T> >& image, int image_size);
template <typename T>
std::vector<std::vector<T> > separate_channels(std::vector<T>& image_, int image_size);

double clip(double pix);

std::pair<int, int> read_img_size(std::vector<char> buffer);
template <typename T>
std::vector<T> read_vec_from_buffer(std::vector<char> buffer);
template <typename T>
std::vector<char> write_vec_to_buffer(std::vector<T>& encoded_image, int width, int height, std::string filename);

#endif // UTILS_H